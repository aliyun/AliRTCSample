// pages/index/index.js.js

const app = getApp()
const Utils = require('../../utils/util.js')
const appServer = require('../../utils/config')

Page({
  /**
   * 页面的初始数据
   */
  data: {
    // whether to disable join btn or not
    disableJoin: true,
    channel: "",
    displayName: Math.random().toString(36).replace(/[^a-z]+/g, '').substr(0, 5),
  },

  onGotUserInfo: function(e){
    if(!e.detail.userInfo) {
      wx.showToast({
        title: '必须授权登录才能进行通话',
        icon: 'none',
        duration: 1500
      })
      return
    }

    let userInfo = e.detail.userInfo;
    // store data for next launch use
    wx.setStorage({
      key: 'userInfo',
      data: userInfo,
    })
    this.onJoin();
  },

  checkJoinLock: function() {
    return !(this.lock || false);
  },
  
  lockJoin: function() {
    this.lock = true;
  },

  unlockJoin: function() {
    this.lock = false;
  },

  /**
   * 验证输入频道是否有效
   * @param {String} channel 频道
   */
  regChannel: function(channel) {
    let reg = /^[a-zA-Z0-9_\-]{1,20}$/
    if(!reg.test(channel)) {
      return false
    }
    return true
  },

  onJoin: function () {
    let channel = this.data.channel || "";
    let displayName = this.data.displayName || Math.random().toString(36).replace(/[^a-z]+/g, '').substr(0, 5);
    if (!this.regChannel(channel)) {
      wx.showToast({
        title: '请提供一个有效的房间名(20位内字母数字中下划线)',
        icon: 'none',
        duration: 4000
      })
    } else {
      if(this.checkJoinLock()) {
        this.lockJoin();
        // 请求appServer返回入会鉴权信息，请求接口名及参数请根据业务实际情况进行修改。
        wx.request({
          url: appServer + "/getRtcAuth?channelId=" + channel,
          data: '',
          header: {
            "content-type": "application/json"
          },
          method: 'GET',
          dataType: 'json',
          responseType: 'text',
          success: (res) => {
            let authInfo = res.data.data;
            authInfo.channel = channel;
            authInfo.displayName = displayName;
            app.globalData.authInfo = authInfo;
            wx.navigateTo({
              url: `../meeting/meeting?channel=${channel}&displayName=${displayName}`
            });
          },
          fail: function (res) {
            Utils.log('request error:', res);
          },
          complete: function (res) { },
        })
        
      }
    }
  },
  onInputChannel: function (e) {
    let channel = e.detail.value;
    this.setData({
      channel
    },()=>{
    this.checkCanJoin()
    })
  },

  onInputDisplayName: function (e) {
    let displayName = e.detail.value;
    this.setData({
      displayName
    }, ()=> {
      this.checkCanJoin()
    })
  },

  checkCanJoin() {
    if(this.data.channel && this.data.displayName) {
      this.setData({
        disableJoin: false
      })
    }else {
      this.setData({
        disableJoin: true
      })
    }
  },

  urlChange: function(e) {
    this.setData({
      urlIndex: e.detail.value
    })
  },
})