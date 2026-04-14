const {
  sdkPub,
  sdkSubVideo,
  sdkUnsubVideo,
  sdkSubAudio,
  sdkUnsubAudio,
  sdkLeave,
  sdkUnpub,
} = require('../../../services/usesdk.js');
const { store } = require('../../../services/service/store.js');
const DingRTC = require('../../../jssdk/dingrtc_miniapp_client.js').default;

const getLogger = () => store.client.logger;
const userLivePlayerCtxMap = new Map();
const MAX_COUNT_OF_USER_HAS_VIDEO_IN_VIEW = 50;

Page({
  data: {
    max_count_of_user_has_video_in_view: MAX_COUNT_OF_USER_HAS_VIDEO_IN_VIEW,
    miniSdkVer: DingRTC.VERSION,
    pusherInputValue: '',
    pusherUrl: '',
    audioRtmpPlayUrl: '',
    playerInputValue: '',
    userList: [],
    partialUserListToView: [],
    joinInfo: {},
    pubState: 'notPublished', // notPublished publishing published

    videoHeight: 1280,
    videoWidth: 720,
    enableMic: false,
    enableCamera: false,
  },

  onLoad() {
    const userList = store.client.remoteUsers.map((u) => ({
      userId: u.userId,
      hasVideo: u.hasVideo,
      videoMuted: u.videoMuted,
      rtmpPullUrl: '',
    }));
    this.setData({
      userList,
      partialUserListToView: this.getPartialUserHasVideoArr(userList),
      joinInfo: {
        ...getApp().globalData.joinInfo,
        minCache: parseFloat(getApp().globalData.joinInfo.minCache) || 0.2,
        maxCache: parseFloat(getApp().globalData.joinInfo.maxCache) || 0.8,
      },
    });
    console.log('getApp().globalData.joinInfo', getApp().globalData.joinInfo)
    this.listenSdkUserChange();
  },
  onUnload() {
    // 页面销毁时
    this.getPusherContext().stop(); // 停止推流，释放摄像头和麦克风
    this.livePusherContext = null;
  },
  getPartialUserHasVideoArr(userList) {
    return userList
      .sort((user1, user2) => {
        const p1 = user1.hasVideo ? 1 : 0;
        const p2 = user2.hasVideo ? 1 : 0;
        return p2 - p1;
      })
      .slice(0, MAX_COUNT_OF_USER_HAS_VIDEO_IN_VIEW);
  },

  listenSdkUserChange() {
    store.client.on('user-joined', (newUser) => {
      const userList = [
        ...this.data.userList,
        {
          userId: newUser.userId,
          hasVideo: newUser.hasVideo,
          videoMuted: newUser.videoMuted,
          rtmpPullUrl: '',
        },
      ];
      this.setData({
        userList,
        partialUserListToView: this.getPartialUserHasVideoArr(userList),
      });
    });
    store.client.on('user-left', (leftUser) => {
      const userList = this.data.userList.filter((user) => user.userId !== leftUser.userId);
      this.setData({
        userList,
        partialUserListToView: this.getPartialUserHasVideoArr(userList),
      });
    });
    store.client.on('user-published', (updatedUser, mediaType, auxiliary) => {
      const userList = this.data.userList.map((user) => {
        if (user.userId === updatedUser.userId) {
          const newUserObj = { ...user };
          if (mediaType === 'audio') {
            newUserObj.hasAudio = true;
            newUserObj.audioMuted = false;
          } else if (mediaType === 'video' && !auxiliary) {
            newUserObj.hasVideo = true;
            newUserObj.videoMuted = false;
          } else {
            // TODO: 屏幕共享流的订阅需要实现
            // newUserObj.hasAuxiliary = true
            // newUserObj.auxiliaryMuted = false;
          }
          return newUserObj;
        }
        return user;
      });

      this.setData({
        userList,
        partialUserListToView: this.getPartialUserHasVideoArr(userList),
      });
    });

    store.client.on('user-unpublished', (updatedUser, mediaType, auxiliary) => {
      const userList = this.data.userList.map((user) => {
        if (user.userId === updatedUser.userId) {
          const newUserObj = { ...user };
          if (mediaType === 'audio') {
            newUserObj.hasAudio = false;
          } else if (mediaType === 'video' && !auxiliary) {
            newUserObj.hasVideo = false;
            sdkUnsubVideo({ userId: user.userId });
            newUserObj.rtmpPullUrl = '';
          } else {
            // TODO: 屏幕共享流的订阅需要实现
            // newUserObj.hasAuxiliary = false
          }
          return newUserObj;
        }
        return user;
      });
      this.setData({
        userList,
        partialUserListToView: this.getPartialUserHasVideoArr(userList),
      });
    });

    store.client.on('user-info-updated', (uid, mediaChange) => {
      const userList = this.data.userList.map((user) => {
        if (user.userId === uid) {
          const newUserObj = { ...user };
          if (mediaChange === 'mute-audio') {
            newUserObj.audioMuted = true;
          } else if (mediaChange === 'unmute-audio') {
            newUserObj.audioMuted = false;
          } else if (mediaChange === 'mute-video') {
            newUserObj.videoMuted = true;
          } else {
            newUserObj.videoMuted = false;
          }
          return newUserObj;
        }
        return user;
      });
      this.setData({
        userList,
        partialUserListToView: this.getPartialUserHasVideoArr(userList),
      });
    });

    store.client.on('connection-state-change', (currentState, preState) => {
      getLogger().info('demo: got connection-state-change', currentState, preState);
      if (currentState === 'reconnecting') {
        wx.showToast({
          title: 'reconnecting',
          icon: 'none',
          duration: 8000,
        });
      } else if (currentState === 'connected' && preState === 'reconnecting') {
        wx.showToast({
          title: 'reconnect succeeded',
          icon: 'none',
          duration: 8000,
        });
      }
      if (currentState === 'disconnected') {
        wx.showToast({
          title: 'reconnect failed',
          icon: 'none',
          duration: 8000,
        });
        sdkLeave();
        wx.reLaunch({
          url: '/pages/join/index',
        });
      }
    });

    store.client.on('pub-media-reconnect-started', () => {
      wx.showToast({
        title: 'pub media reconnecting',
        icon: 'none',
        duration: 8000,
      });
    });
    store.client.on('pub-media-reconnect-failed', () => {
      wx.showToast({
        title: 'pub reconnect failed',
        content: 'pub reconnect failed',
        showCancel: false,
        success() {
          getLogger().info('ok clicked on pub-media-fail modal');
        },
      });
    });

    store.client.on('pub-media-reconnect-succeeded', (newRtmpUrl) => {
      wx.showToast({
        title: 'pub media reconnect succeded',
        icon: 'none',
        duration: 8000,
      });
      this.setData({
        pusherUrl: newRtmpUrl,
      });
    });

    store.client.on('sub-media-reconnect-started', (subParam) => {
      wx.showToast({
        title: `sub reconnecting ${JSON.stringify(subParam)}`,
        icon: 'none',
        duration: 8000,
      });
    });
    store.client.on('sub-media-reconnect-failed', (subParam) => {
      wx.showToast({
        title: 'sub reconnect failed',
        content: `sub reconnecting failed: ${JSON.stringify(subParam)}`,
        showCancel: false,
        success() {
          getLogger().info('ok clicked on sub-media-fail modal');
        },
      });
    });

    store.client.on('sub-media-reconnect-succeeded', (subResult) => {
      wx.showToast({
        title: `sub media reconnect succeded, ${JSON.stringify(subResult)}`,
        icon: 'none',
        duration: 8000,
      });
      if (subResult.uid === 'mcu') {
        this.setData({ audioRtmpPlayUrl: subResult.rtmpUrl });
        return;
      }
      const newUserList = this.data.userList.map((user) => {
        if (user.userId === subResult.uid) {
          if (subResult.auxiliary) {
            getLogger().error('cannot show sub screen video for now');
          } else {
            return { ...user, rtmpPullUrl: subResult.rtmpUrl };
          }
        }
        return user;
      });
      this.setData({
        userList: newUserList,
        partialUserListToView: this.getPartialUserHasVideoArr(newUserList),
      });
    });
    store.client.on('cvo-update', (subResult) => {
      const newUserList = this.data.userList.map((user) => {
        if (user.userId === subResult.uid) {
          if (subResult.auxiliary) {
            getLogger().error('cannot show sub screen video for now');
          } else {
            return { ...user, rotation: subResult.rotation };
          }
        }
        return user;
      });
      this.setData({
        userList: newUserList,
        partialUserListToView: this.getPartialUserHasVideoArr(newUserList),
      });
    });

    store.client.on('publish-ready', () => {
      getLogger().info('publish-ready')
      wx.showToast({
        title: 'publish-ready',
        icon: 'none',
        duration: 8000,
      })
    })
    store.client.on('subscribe-ready', (subParam) => {
      getLogger().info('subscribe-ready', subParam);
      store.client.setRemoteVideoStreamType((subParam.uid), 'LD', false);
      wx.showToast({
        title: `subscribe-ready ${JSON.stringify(subParam)}`,
        icon: 'none',
        duration: 2000,
      })
    })
  },

  onLeaveClick() {
    sdkLeave();
    wx.reLaunch({
      url: '/pages/join/index',
    });
  },

  onTestRepushBtnClick() {
    getLogger().info('onTestRepushBtnClick');
    this.restartPusher();
  },

  // onTestResoLevelBtnClick() {
  //   getLogger().info('onTestResoLevelBtnClick');
  //   // 以180为基准，成倍放大level，默认为4倍，720p  这里是对应的倍数以2为底的指数
  //   let curResolutionLevelExp = this.curResolutionLevelExp || 2;
  //   const newResolutionLevelExp = (curResolutionLevelExp + 1) % 3
  //   this.curResolutionLevelExp = newResolutionLevelExp
  //   this.setData({
  //     videoHeight: 320 * Math.pow(2, newResolutionLevelExp),
  //     videoWidth: 180 * Math.pow(2, newResolutionLevelExp),
  //   })
  //   getLogger().info('new resolution', {
  //     videoHeight: 320 * Math.pow(2, newResolutionLevelExp),
  //     videoWidth: 180 * Math.pow(2, newResolutionLevelExp),
  //   })
  //   this.getPusherContext().stop({
  //     complete: () => {
  //       getLogger().info('pusher context stop complete');
  //       this.getPusherContext().start()
  //     }
  //   })
  // },

  async onPubClick() {
    this.setData({ pubState: 'publishing' });
    const url = await sdkPub();
    this.setData({
      pusherUrl: url,
    });
    // setTimeout(() => {
    //   getLogger().info('test new url')
    //   this.setData({
    //     pusherUrl: `${url}&retry=true`,
    //   });
    // }, 10000)
    this.setData({ pubState: 'published' });

    // 动态推到钉钉
    // setTimeout(() => {
    //   this.setData({pusherUrl: 'rtmp://dtlive-vip-push.dingtalk.com/live/8da77e36-7055-4fec-8521-f0a58c86fa24?auth_key=1761049194-0-0-e0035a2744e9cce16bb0c7f0f4be769a'})
    //   this.setData({ pubState: 'published' })
    // }, 1000)
  },

  onStateChange(event) {
    getLogger().info('statechange event:', event.detail, event);

    // 当出现网络切换提示时，可判断是否需要重启推流
    if (/network type has changed/i.test(event.detail?.message || '')) {
      getLogger().info('网络切换，重启推流');
      this.restartPusher();
    }
  },

  getPusherContext() {
    if (!this.livePusherContext) {
      const context = wx.createLivePusherContext('pusher', this);
      this.livePusherContext = context;
    }
    return this.livePusherContext;
  },

  restartPusher() {
    // 先停止
    this.getPusherContext().stop({
      success: () => {
        console.log('原推流已停止');
        // 延迟一点再启动（避免冲突）
        setTimeout(() => {
          this.getPusherContext().start({
            success: () => {
              console.log('新推流已启动');
            },
            fail: (err) => console.error('重启失败', err),
          });
        }, 2000);
      },
      fail: (err) => console.error('stop 失败', err),
    });
  },

  onUnpubClick() {
    sdkUnpub();
    this.setData({ pubState: 'notPublished', pusherUrl: '' });
  },
  onSwitchCam() {
    this.getPusherContext().switchCamera();
  },
  onPausePush() {
    this.getPusherContext().pause();
  },
  onResumePush() {
    this.getPusherContext().resume();
  },
  onChangeMicEnable() {
    getLogger().info('change mic enable to', !this.data.enableMic);
    if (this.data.enableMic) {
      store.client.muteMic();
    } else {
      store.client.unmuteMic();
    }
    this.setData({
      enableMic: !this.data.enableMic,
    });
  },
  onChangeCamEnable() {
    getLogger().info('change cam enable to', !this.data.enableCamera);
    if (this.data.enableCamera) {
      store.client.muteCam();
    } else {
      store.client.unmuteCam();
    }
    this.setData({
      enableCamera: !this.data.enableCamera,
    });
  },

  async onSubVideoClick(event) {
    const userItem = this.data.userList[event.currentTarget.dataset.index];
    let rtmpPullUrl = await sdkSubVideo(userItem);
    const newUserList = [];
    this.data.userList.forEach((user) => {
      if (userItem.userId === user.userId) {
        user.rtmpPullUrl = rtmpPullUrl;
        user.cvoRotation = 0;
      }
      newUserList.push(user);
    });

    this.setData({
      userList: newUserList,
      partialUserListToView: this.getPartialUserHasVideoArr(newUserList),
    });


  },

  onUnsubVideoClick(event) {
    const userItem = this.data.userList[event.currentTarget.dataset.index];

    sdkUnsubVideo(userItem).then(() => {
      const newUserList = [];
      this.data.userList.forEach((user) => {
        if (userItem.userId === user.userId) {
          user.rtmpPullUrl = '';
        }
        newUserList.push(user);
      });
      this.setData({
        userList: newUserList,
        partialUserListToView: this.getPartialUserHasVideoArr(newUserList),
      });
    });
  },

  onSetSubFhd(event) {
    const userItem = this.data.userList[event.currentTarget.dataset.index];

    store.client.setRemoteVideoStreamType((userItem.userId), 'FHD').then(() => {
      getLogger().info('set sub fhd', userItem.userId)
    });
  },
  onChangePlayerUrl(event) {
    const userItem = this.data.userList[event.currentTarget.dataset.index];
    const debugurl = "rtmp://pull.rms-test.aliyunrtc.com/TCcsz/DQrSJ?auth_key=1767523561-0-0-6058a953e74a060a84778ef2c3bd9321";
    const newUserList = this.data.userList.map((user) => {
      if (user.userId === userItem.userId) {
        if (user.rtmpPullUrlBackup) {
          const result = { ...user, rtmpPullUrl: user.rtmpPullUrlBackup, rtmpPullUrlBackup: '' };
          getLogger().info('update user url', result)
          return result

        } else {
          // let livePlayerCtx = this._getPlayerContext(userItem.userId)
          // livePlayerCtx.stop({
          //   complete: () => {
          //     getLogger().info('player context pause complete');
          //     // livePlayerCtx.play();
          //   },
          // });
          const result = { ...user, rtmpPullUrlBackup: user.rtmpPullUrl, rtmpPullUrl: debugurl };
          getLogger().info('update user url', result)
          return result
        }
      }
      return user;
    });
    this.setData({
      userList: newUserList,
      partialUserListToView: this.getPartialUserHasVideoArr(newUserList),
    });
  },

  onSetSubLd(event) {
    const userItem = this.data.userList[event.currentTarget.dataset.index];

    store.client.setRemoteVideoStreamType((userItem.userId), 'LD').then(() => {
      getLogger().info('set sub ld', userItem.userId)
    });
  },
  _getPlayerContext(userId) {
    let livePlayerCtx = userLivePlayerCtxMap.get(userId);
    if (!livePlayerCtx) {
      livePlayerCtx = wx.createLivePlayerContext(userId, this);
      userLivePlayerCtxMap.set(userId, livePlayerCtx);
    }
    return livePlayerCtx;
  },

  onReconnectSub(event) {
    const userId = event.currentTarget.dataset.userId;
    let livePlayerCtx = this._getPlayerContext(userId)
    livePlayerCtx.stop({
      complete: () => {
        getLogger().info('player context stop complete');
        livePlayerCtx.play();
      },
    });
  },
  onSubAudioClick() {
    sdkSubAudio().then((audioPlayUrl) => {
      this.setData({ audioRtmpPlayUrl: audioPlayUrl });
    });
  },
  onUnsubAudioClick() {
    sdkUnsubAudio().then(() => {
      this.setData({ audioRtmpPlayUrl: '' });
    });
  },

  onPushStateChange(event) {
    store.client.logger.info('onbind event', event.type, event.detail, event);
  },

  // 推流input实时输入
  onPusherInput(e) {
    this.setData({
      pusherInputValue: e.detail.value,
    });
  },

  // 点击按钮设置live-pusher的URL
  setPusherUrl() {
    this.setData({
      pusherUrl: this.data.pusherInputValue,
    });
  },

  // 拉流input实时输入
  onPlayerInput(e) {
    this.setData({
      playerInputValue: e.detail.value,
    });
  },

  onPusherNetStatusEvent(event) {
    store.client.reportPusherNetStatus(event.detail);
  },
  onPusherStateChangeEvent(event) {
    store.client.reportPusherStateChange(event.detail);
  },

  onMcuAudioPlayerNetStatus(event) {
    const subParam = {
      uid: 'mcu',
      mediaType: 'audio',
      auxiliary: false,
    };
    store.client.reportPlayerNetStatus(subParam, event.detail);
  },
  onMcuAudioPlayerStateChange(event) {
    const subParam = {
      uid: 'mcu',
      mediaType: 'audio',
      auxiliary: false,
    };
    store.client.reportPlayerStateChange(subParam, event.detail);
  },

  onVideoPlayerStateChange(event) {
    const subParam = {
      uid: event.currentTarget.dataset.userId,
      mediaType: 'video',
      auxiliary: false,
    };
    store.client.reportPlayerStateChange(subParam, event.detail);
  },
  onVideoPlayerNetStatus(event) {
    const subParam = {
      uid: event.currentTarget.dataset.userId,
      mediaType: 'video',
      auxiliary: false,
    };
    store.client.reportPlayerNetStatus(subParam, event.detail);
  },

  onPlayerError(event) {
    getLogger().info(
      'onPlayerError event:',
      event.currentTarget.dataset.userId,
      event.detail,
      event,
    );
  },
});
