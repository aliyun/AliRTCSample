// pages/meeting/meeting.js
const app = getApp()
const Utils = require('../../utils/util.js')
const AliRtcMiniappSDK = require("../../sdk/aliyun-webrtc-miniapp-sdk");
const max_user = 10;
const Layouter = require("../../utils/layout.js");

const AUTO_LEAVE_STATE = true
const LEAVE_TIMEOUT = 10 * 60 * 1000;

Page({
  data: {
    media: [],
    mic: true,
    camera: true,
    beauty: 5,
    isPublish: false,
  },

  /**
   * 生命周期函数--监听页面加载
   */
  onLoad(options) {
    this.uid = app.globalData.authInfo.userid;
    this.channel = options.channel;
    this.displayName = options.displayName;
    // alirtc client
    this.client = null;
    // 布局控制
    this.layouter = null;
    // 避免用户离会频繁
    this.leaving = false;
    // 自动离会timer
    this.onLeaveTimeoutId = null

    wx.setNavigationBarTitle({
      title: `房间号：${this.channel}`
    });

    // 保存屏幕常亮
    wx.setKeepScreenOn({
      keepScreenOn: true
    });
  },

  /**
   * 生命周期函数--监听页面初次渲染完成
   */
  onReady() {
    // 初始化页面控制器
    this.initLayouter();
    // 初始化SDK并入会推流
    this.initAliRtcChannel().then( url => {
      Utils.log(`pushing ${url}`);
      let ts = new Date().getTime()
      this.addMedia(0, this.uid, url, {
        key: ts,
        name: this.displayName
      });
      wx.nextTick(() => {
        this.onPublish()
        // 限时体验10分钟，超时离会。
        if(AUTO_LEAVE_STATE) {
          this.onLeaveTimeout()
        }
      })
    }).catch(e => {
      Utils.log(`init alirtc client failed: ${e}`);
      wx.showToast({
        title: `客户端初始化失败`,
        icon: 'none',
        duration: 5000
      });
      // 初始化客户端失败 返回
      setTimeout(()=> {
        this.navigateBack()
      },1000)
    })
  },

  /**
   * 初始化SDK 并入会推流
   * 
   */
  initAliRtcChannel() {
    return new Promise((resolve, reject) => {
      let client = {}
      client = new AliRtcMiniappSDK()
      this.client = client;
      // 订阅事件
      this.subscribeEvents(client);
      let authInfo = app.globalData.authInfo
      client.join(authInfo, () => {
        Utils.log(`client join channel success`);
        // 推流获取推流地址
        client.getPublishUrl(url => {
          resolve(url);
        }, e => {
          Utils.log(`client getPublishUrl failed: ${e.code} ${e.reason}`);
          wx.showToast({
            title: 'getPublishUrl failed',
            icon: 'none',
            duration: 2000
          })
          reject(e)
        });
      }, e => {
        Utils.log(`client join channel failed`);
        wx.showToast({
          title: `client join channel failed（${e.reason}）`,
          icon: 'none',
          duration: 2000
        })
        reject(e)
      })

    });
  },

  /**
   * 自动离会定时器
   */
  onLeaveTimeout() {
    if(!this.onLeaveTimeoutId) {
      this.onLeaveTimeoutId = setTimeout(()=> {
        let showToastDelay = 2000
        wx.showToast({
          title: `10分钟体验时间已到，如需继续体验请重新入会`,
          icon: 'none',
          duration: showToastDelay
        })
        setTimeout(()=>{
          this.onLeave()
        }, showToastDelay)
      }, LEAVE_TIMEOUT)
    }
  },

  /**
   * check if current media list has specified uid & mediaType component
   */
  hasMedia(mediaType, uid) {
    let media = this.data.media || [];
    return media.filter(item => {
      return item.type === mediaType && `${item.uid}` === `${uid}`
    }).length > 0
  },

  /**
   * add media to view
   * type: 0 - pusher, 1 - player
   * *important* here we use ts as key, when the key changes
   * the media component will be COMPLETELY refreshed
   * this is useful when your live-player or live-pusher
   * are in a bad status - say -1307. In this case, update the key
   * property of media object to fully refresh it. The old media
   * component life cycle event detached will be called, and
   * new media component life cycle event ready will then be called
   */
  addMedia(mediaType, uid, url, options) {
    Utils.log(`add media ${mediaType} ${uid} ${url}`);
    let media = this.data.media || [];

    if (mediaType === 0) {
      //pusher
      let tag = 0;
      if(media.length >= 1 && media[0].type === 0) {
        tag = 1
      }
      media.splice(0, tag, {
        key: options.key,
        type: mediaType,
        uid: `${uid}`,
        name: options.name,
        holding: false,
        url: url,
        left: 0,
        top: 0,
        width: 0,
        height: 0
      });
    } else {
      //player
      media.push({
        key: options.key,
        type: mediaType,
        uid: `${uid}`,
        holding: false,
        name: options.name,
        audio: options.audio,
        video: options.video,
        url: url,
        left: 0,
        top: 0,
        width: 0,
        height: 0
      });
    }

    media = this.syncLayout(media);
    return this.refreshMedia(media);
  },

  /**
   * remove media from view
   */
  removeMedia(uid) {
    Utils.log(`remove media ${uid}`);
    let media = this.data.media || [];
    media = media.filter(item => {
      return `${item.uid}` !== `${uid}`
    });

    if (media.length !== this.data.media.length) {
      media = this.syncLayout(media);
      this.refreshMedia(media);
    } else {
      Utils.log(`media not changed: ${JSON.stringify(media)}`)
      return Promise.resolve();
    }
  },

  /**
   * update media object
   * the media component will be fully refreshed if you try to update key
   * property.
   */
  updateMedia(uid, options) {
    Utils.log(`update media ${uid} ${JSON.stringify(options)}`);
    let media = this.data.media || [];
    let changed = false;
    for (let i = 0; i < media.length; i++) {
      let item = media[i];
      if (`${item.uid}` === `${uid}`) {
        media[i] = Object.assign(item, options);
        changed = true;
        Utils.log(`after update media ${uid} ${JSON.stringify(item)}`)
        break;
      }
    }

    if (changed) {
      return this.refreshMedia(media);
    } else {
      Utils.log(`media not changed: ${JSON.stringify(media)}`)
      return Promise.resolve();
    }
  },

  /**
   * call setData to update a list of media to this.data.media
   * this will trigger UI re-rendering
   */
  refreshMedia(media) {
    return new Promise((resolve) => {
      for (let i = 0; i < media.length; i++) {
        if (i < max_user) {
          //show
          media[i].holding = false;
        } else {
          //hide 超过十人隐藏
          media[i].holding = true;
        }
      }

      if (media.length > max_user) {
        wx.showToast({
          title: '由于房内人数超过10人，部分视频未被加载显示',
          icon: 'none'
        });
      }

      Utils.log(`updating media: ${JSON.stringify(media)}`);
      this.setData({
        media: media
      }, () => {
        resolve();
      });
    });
  },

  /**
   * 生命周期函数--监听页面显示
   */
  onShow() {
    let media = this.data.media || [];
    media.forEach(item => {
      if (item.type === 0) {
        //return for pusher
        return;
      }
      let player = this.getPlayerComponent(item.uid);
      if (!player) {
        Utils.log(`player ${item.uid} component no longer exists`, "error");
      } else {
        // while in background, the player maybe added but not starting
        // in this case we need to start it once come back
        player.start();
      }
    });
  },

  /**
   * 生命周期函数--监听页面隐藏
   */
  onHide() {},

  onError(e) {
    Utils.log(`error: ${JSON.stringify(e)}`);
  },

  /**
   * 生命周期函数--监听页面卸载
   */
  onUnload() {
    // unlock index page join button
    let pages = getCurrentPages();
    if (pages.length > 1) {
      //unlock join
      let indexPage = pages[1];
      indexPage.unlockJoin();
    }

    // 取消离会计时器
    if(this.onLeaveTimeoutId) {
      clearTimeout(this.onLeaveTimeoutId)
      this.onLeaveTimeoutId = null;
    }

    // unpublish sdk and leave channel
    try {
      if(this.data.isPublish) {
        this.client && this.client.unpublish(()=>{
          this.setData({
            isPublish: false
          })
        });
      }
    } catch (e) {
      Utils.log(`unpublish failed ${e}`);
    }
    this.client && this.client.leave(()=>{
      Utils.log('leave success')
    },()=>{
      Utils.log('leave failed')
    });
  },

  /**
   *  停止推流离会操作在onUnload钩子中实现
   */
  onLeave() {
    if (!this.leaving) {
      this.leaving = true;
      this.navigateBack();
    }
  },

  /**
   * navigate to previous page
    */
  navigateBack() {
    Utils.log(`attemps to navigate back`);
    if (getCurrentPages().length > 1) {
      //have pages on stack
      wx.navigateBack({});
    } else {
      wx.redirectTo({
        url: '../login/login',
      });
    }
  },

  /**
   * 静音
   */
  onMuteMic() {
    if (this.data.mic) {
      this.client.muteLocal('audio', () => {
        Utils.log('muteLocal audio success')
        this.setData({
          mic: !this.data.mic
        })
      }, () => {
        Utils.log('muteLocal audio failed')
      });
    } else {
      this.client.unmuteLocal('audio', () => {
        Utils.log('unmuteLocal success')
        this.setData({
          mic: !this.data.mic
        })
      }, () => {
        Utils.log('unmuteLocal audio failed')
      });
    }
  },

  /**
   * 推流设置
   */
  onRepublish() {
    let ts = new Date().getTime();
    if (!this.data.isPublish) {
      // 未推流状态进行推流

      this.client.getPublishUrl(url => {
        this.addMedia(0, this.uid, url, {
          key: ts,
          name: this.displayName
        });
        wx.nextTick(()=>{
          this.onPublish()
        })
      }, e => {
        Utils.log(`client getPublishUrl failed: ${e.code} ${e.reason}`);
        wx.showToast({
          title: 'getPublishUrl failed',
          icon: 'none',
          duration: 2000
        })
      });
    } else {
      // 已推流状态停止推流后重新推流
      this.client.unpublish(() => {
        this.setData({
          isPublish: false
        })
        Utils.log('client unpublish success')
        this.removeMedia(this.uid);
        this.client.getPublishUrl(url => {
          this.addMedia(0, this.uid, url, {
            key: ts,
            name: this.displayName
          });
          wx.nextTick(()=>{
            this.onPublish()
          })
        }, e => {
          Utils.log(`client getPublishUrl failed: ${e.code} ${e.reason}`);
          wx.showToast({
            title: `client getPublishUrl failed`,
            icon: 'none',
            duration: 2000
          })
        });
      }, err => {
        Utils.log('client unpublish failed')
        wx.showToast({
          title: 'unpublish failed',
          icon: 'none',
          duration: 2000
        })
      });
    }
  },

  onResubscribe(e) {
    let {uid} = e.detail
    let media = this.data.media || [];
    media = media.filter(item => {
      return item.type === mediaType && `${item.uid}` === `${uid}`
    });
    if(media && media.length) {
      let {audio, video} = media
      // todo:resub
      this.client && this.client.unsubscribe(uid, () => {
        this.removeMedia(uid);
        this.client.subscribe(uid, (url) => {
          Utils.log(`stream ${uid} subscribed successful`);
          let media = this.data.media || [];
          let matchItem = null;
          for (let i = 0; i < media.length; i++) {
            let item = this.data.media[i];
            if (`${item.uid}` === `${uid}`) {
              //if existing, record this as matchItem and break
              matchItem = item;
              break;
            }
          }
    
          if (!matchItem) {
            //if not existing, add new media
            this.addMedia(1, uid, url, {
              key: ts,
              name: name,
              audio,
              video
            })
          } else {
            // if existing, update property
            // change key property to refresh live-player
            this.updateMedia(matchItem.uid, {
              url: url,
              key: ts,
              audio,
              video
            });
          }
        }, e => {
          Utils.log(`stream subscribed failed ${e} ${e.code} ${e.reason}`);
        });
      },() => {

      })
    }
  },

  /**
   * 摄像头方向切换回调
   */
  onSwitchCamera() {
    Utils.log(`switching camera`);
    const alirtcPusher = this.getPusherComponent();
    alirtcPusher && alirtcPusher.switchCamera();
  },

  /**
   * 美颜回调
   */
  onMakeup() {
    let beauty = this.data.beauty == 5 ? 0 : 5;
    this.setData({
      beauty: beauty
    })
  },

  /**
   * 获取屏幕尺寸以用于之后的视窗计算
   */
  initLayouter() {
    // get window size info from systemInfo
    const systemInfo = app.globalData.systemInfo;
    // 64 is the height of bottom toolbar
    this.layouter = new Layouter(
      systemInfo.windowWidth, 
      systemInfo.windowHeight - 64
    );
  },

  /**
   * calculate size based on current media length
   * sync the layout info into each media object
   */
  syncLayout(media) {
    let sizes = this.layouter.getSize(media.length);
    for (let i = 0; i < sizes.length; i++) {
      let size = sizes[i];
      let item = media[i];

      if (item.holding) {
        //skip holding item
        continue;
      }

      item.left = parseFloat(size.x).toFixed(2);
      item.top = parseFloat(size.y).toFixed(2);
      item.width = parseFloat(size.width).toFixed(2);
      item.height = parseFloat(size.height).toFixed(2);
    }
    return media;
  },

  /**
   * return player component via uid
   */
  getPlayerComponent(uid) {
    const alirtcPlayer = this.selectComponent(`#rtc-player-${uid}`);
    return alirtcPlayer;
  },

  /**
   * return pusher component
   */
  getPusherComponent() {
    const alirtcpusher = this.selectComponent(`#rtc-pusher`);
    return alirtcpusher;
  },

  /**
   * 注册stream事件
   */
  subscribeEvents(client) {
    /**
     * fired when new stream join the channel
     */
    client.on("stream-added", e => {
      let {uid, displayName} = e;
      const ts = new Date().getTime();
      Utils.log(`stream ${uid} added`);
      /**
       * subscribe to get corresponding url
       */
      client.subscribe(uid, (url) => {
        Utils.log(`stream ${uid} subscribed successful`);
        let media = this.data.media || [];
        let matchItem = null;
        for (let i = 0; i < media.length; i++) {
          let item = this.data.media[i];
          if (`${item.uid}` === `${uid}`) {
            //if existing, record this as matchItem and break
            matchItem = item;
            break;
          }
        }

        if (!matchItem) {
          //if not existing, add new media
          this.addMedia(1, uid, url, {
            key: ts,
            name: displayName,
            audio: true,
            video: true
          })
        } else {
          // if existing, update property
          // change key property to refresh live-player
          this.updateMedia(matchItem.uid, {
            url: url,
            key: ts,
          });
        }
      }, e => {
        Utils.log(`stream subscribed failed ${e} ${e.code} ${e.reason}`);
      });
    });

    /**
     * remove stream when it leaves the channel
     */
    client.on("stream-removed", e => {
      let {uid} = e;
      Utils.log(`stream ${uid} removed`);
      this.removeMedia(uid);
    });

    client.on("mute-audio", (e) => {
      let {uid} = e;
      this.updateMedia(uid, {
        audio: false
      })
    })

    client.on("mute-video", (e) => {
      let {uid} = e;
      this.updateMedia(uid, {
        video: false
      })
    })

    client.on("unmute-audio", (e) => {
      let {uid} = e;
      this.updateMedia(uid, {
        audio: true
      })
    })

    client.on("unmute-video", (e) => {
      let {uid} = e;
      this.updateMedia(uid, {
        video: true
      })
    })

    client.on("leave", (e) => {
      let {uid} = e;
      Utils.log(`${uid} leave`)
      this.removeMedia(uid);
    })

    /**
     * when bad thing happens - we recommend you to do a 
     * full reconnect when meeting such error
     * it's also recommended to wait for few seconds before
     * reconnect attempt
     */
    client.on("error", err => {
      let errObj = err || {};
      let code = errObj.code || 0;
      let reason = errObj.reason || "";
      Utils.log(`[error] code: ${code} reason ${reason}`)
      if(code !== 100002){
        wx.showToast({
          title: `[error] code: ${code} reason ${reason}`,
          icon: 'none',
          duration: 2000
        })
      }else {
        wx.showModal({
          title: '发生错误',
          content: '[100002]websocket断开,是否重连？',
          success: (res) => {
            if (res.confirm) {
              const pages = getCurrentPages();
              const currentPage = pages[pages.length - 1];
              const url = `/${currentPage.route}`;
              let optionsStr = "?"
              for(let i in this.options) {
                optionsStr += i + '=' + this.options[i] + '&'
              }
              optionsStr = optionsStr.substring(0, optionsStr.length-1)
              wx.redirectTo({
                url: `${url}${optionsStr}`
              })
            } else if (res.cancel) {
              this.navigateBack();
            }
          }
        })
      }
    });
  }
})