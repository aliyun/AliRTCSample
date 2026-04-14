const { sdkJoin } = require('../../services/usesdk.js');
const { userConfig } = require('../../services/service/user_config.js');
const DingRTC = require('../../jssdk/dingrtc_miniapp_client.js').default;

const randomUid = `mini_${Math.floor(Math.random() * 1000)}`;

Page({
  data: {
    sdkVer: DingRTC.VERSION,
    appId: userConfig.appId,
    userId: randomUid, // userConfig.userId,
    channelId: 'fff', //userConfig.channelId,
    userName: randomUid,
    env: userConfig.env, //'onertcPre'; // or 'onertcOnline'
    joining: false,
    selectedPreEnvAppId: 0,
    preEnvAppIdArr: ['qy4dz5ec', 'qy4dz5ec'], //lq5iskw5
    selectedProdEnvAppId: 0,
    prodEnvAppIdArr: ['a4zfr1hn', 'qf4swipp'],
  },

  onPreAppIdPickerChange(e) {
    this.setData({
      selectedPreEnvAppId: e.detail.value,
    });
  },
  onProdAppIdPickerChange(e) {
    this.setData({
      selectedProdEnvAppId: e.detail.value,
    });
  },
  async onJoinClick() {
    let isMissingJoinParam = false;
    let missingJoinParamName;
    const appId =
      this.data.appId ||
      (env === 'onertcPre'
        ? this.data.preEnvAppIdArr[this.data.selectedPreEnvAppId]
        : this.data.prodEnvAppIdArr[this.data.selectedProdEnvAppId]);
    if (!appId) {
      isMissingJoinParam = true;
      missingJoinParamName = 'appId';
    }
    if (!this.data.userId) {
      isMissingJoinParam = true;
      missingJoinParamName = 'userId';
    }
    if (!this.data.channelId) {
      isMissingJoinParam = true;
      missingJoinParamName = 'channelId';
    }

    if (isMissingJoinParam) {
      wx.showToast({
        title: 'no ' + missingJoinParamName,
        icon: 'none',
        duration: 2000,
      });
      return;
    }

    this.setData({ joining: true });
    getApp().globalData.joinInfo = {
      ...this.data, appId,
      maxCache: this.data.maxCache,
      minCache: this.data.minCache,
    };
    try {
      await sdkJoin(this.data);
    } catch (error) {
      console.log('join error', error);
      this.setData({ joining: false });
      return;
    }
    wx.reLaunch({
      url: '/packageA/pages/meeting/index',
      // url: '/pages/meeting/index',
    });
  },

  onMaxCacheChange(event) {
    const value = event.detail.value;
    this.setData({
      maxCache: value,
    });
  },
  onMinCacheChange(event) {
    const value = event.detail.value;
    this.setData({
      minCache: value,
    });
  },
  onAppIdChange(event) {
    const value = event.detail.value;
    this.setData({
      appId: value,
    });
  },
  onUserIdChange(event) {
    const value = event.detail.value;
    this.setData({
      userId: value,
    });
  },
  onChannelIdChange(event) {
    const value = event.detail.value;
    this.setData({
      channelId: value,
    });
  },
  onUserNameChange(event) {
    const value = event.detail.value;
    this.setData({
      userName: value,
    });
  },
  onEnvRadioChange(event) {
    console.log('e.detail.value===', event.detail.value);
    const newEnv = event.detail.value;
    let appIdNeed = this.data.appId;
    if (newEnv === 'onertcOnline') {
      appIdNeed = 'qf4swipp';
    } else if (newEnv === 'onertcPre') {
      appIdNeed = 'qy4dz5ec';
    }
    this.setData({
      env: newEnv,
      appId: appIdNeed,
    });
  },
});
