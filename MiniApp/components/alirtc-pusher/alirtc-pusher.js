// components/alirtc-pusher/alirtc-pusher.js
const Utils = require("../../utils/util.js")
const app = getApp()

Component({
  /**
   * 组件的属性列表
   */
  properties: {
    minBitrate: {
      type: Number,
      value: 200
    },
    maxBitrate: {
      type: Number,
      value: 500
    },
    width: {
      type: Number,
      value: 0
    },
    height: {
      type: Number,
      value: 0
    },
    x: {
      type: Number,
      value: 0
    },
    y: {
      type: Number,
      value: 0
    },
    name: {
      type: String,
      value: ""
    },
    mic: {
      type: Boolean,
      value: true
    },
    camera: {
      type: Boolean,
      value: true
    },
    beauty: {
      type: String,
      value: 0
    },
    aspect: {
      type: String,
      value: "3:4"
    },
    /**
     * 0 - loading, 1 - ok, 2 - error
     */
    status: {
      type: String,
      value: "loading",
      observer: function (newVal, oldVal, changedPath) {
        // Utils.log(`player status changed from ${oldVal} to ${newVal}`);
      }
    },
    url: {
      type: String,
      value: "",
      observer: function (newVal, oldVal, changedPath) {
        // 属性被改变时执行的函数（可选），也可以写成在methods段中定义的方法名字符串, 如：'_propertyChange'
        // 通常 newVal 就是新设置的数据， oldVal 是旧数据
        // Utils.log(`pusher url changed from ${oldVal} to ${newVal}, path: ${changedPath}`);
      }
    }
  },

  /**
   * 组件的初始数据
   */
  data: {
    pusherContext: null,
    detached: false
  },

  /**
   * 组件的方法列表
   */
  methods: {
    start() {
      return new Promise((resolve, reject)=>{
        Utils.log(`starting pusher`);
        if(!this.data.pusherContext) {
          this.data.pusherContext = wx.createLivePusherContext(this)
        }
        this.data.pusherContext.stop();
        if (this.data.detached) {
          Utils.log(`try to start pusher while component already detached`);
          reject()
          return;
        }
        this.data.pusherContext.start().then(()=>{
          resolve()
        }).catch(()=>{
          reject()
        });
      })
    },

    /**
     * 停止推送
     */
    stop() {
      Utils.log(`stopping pusher`);
      this.data.pusherContext.stop();
    },

    /**
     * 切换摄像头
     */
    switchCamera() {
      this.data.pusherContext.switchCamera();
    },

    /**
     * 推流状态更新回调
     */
    recorderStateChange: function (e) {
      Utils.log(`live-pusher code: ${e.detail.code} - ${e.detail.message}`)

      if (e.detail.code === 1008) {
        // Utils.log(`live-pusher started`);
        if(this.data.status === "loading") {
          this.setData({
            status: "ok"
          })
        }
      }

      if(e.detail.code === 5001){
        if(app.globalData.systemInfo.platform === "android") {
          this.triggerEvent('republish')
          wx.showToast({
            title: '推流异常，正在重新推流',
            icon: 'none',
            duration: 2000
          })
        }
      }

      if (e.detail.code === -1307) {
        Utils.log('live-pusher stopped', "error");
        this.triggerEvent('republish');
        this.setData({
          status: "error"
        })
        wx.showToast({
          title: '网络异常，正在重新推流',
          icon: 'none',
          duration: 2000
        })
      }
    },

    /**
     * 网络变化通知
     * @param {*} e 
     */
    recorderNetChange: function(e) {
      // Utils.log(`network: ${JSON.stringify(e.detail)}`);
    }
  },

  /**
   * 组件生命周期ready 在组件在视图层布局完成后执行
   * 创建pusherContext
   */
  ready: function () {
    Utils.log("pusher ready");
    this.data.pusherContext || (this.data.pusherContext = wx.createLivePusherContext(this));
  },

  /**
   * 组件生命周期moved 在组件实例被移动到节点树另一个位置时执行		
   */
  moved: function () {
    Utils.log("pusher moved");
   },

  /**
   * 组件生命周期detached 在组件实例被从页面节点树移除时执行
   * 自动停止推流		
   */
  detached: function () {
    Utils.log("pusher detached");
    this.data.pusherContext && this.data.pusherContext.stop();
    this.data.detached = true;
  }
})
