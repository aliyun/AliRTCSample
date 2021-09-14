<template>
  <div id="app">
    <login v-if="login" @loginEvent="loginEvent"></login>
    <main-page v-else></main-page>
  </div>
</template>

<script>
  import {AliEnginePublishState,AliEngineOnByeType, AliEngineErrorCode, AliEngineSubscribeState} from './data/main'
  import Login from "./views/Login/login"
  import MainPage from "./views/MainPage/MainPage"
  import Error from "./utils/error"
  import VideoType from './data/videotype'
  import { AppId, AppKey } from "./data/config"
  export default {
    name: 'electron-vue',
    components:{
      Login,
      MainPage
    },
    data(){
      return {
        login: true,
      }
    },
    created() {
      /**
       * 处理刷新或关闭窗口情况
       */
      window.addEventListener('beforeunload',() => {
        if(this.$store.state.rtc.isInit){
          if(this.$store.state.rtc.isJoinChannel) {
            aliElectronRtc.leaveChannel()
            this.$store.commit("Leave_Channel_Init")
          }
          aliElectronRtc.destroy()
          aliElectronRtc = null
          this.$store.commit("SET_RTC_DATA", { isInit: false })
        }
      })
    },
    methods: {
      loginEvent() {
        if(!AppId || !AppKey) {
          alert('请先在src/renderer/data/config.js下配置AppId和AppKey')
          return
        }
        this.login = false
      },
       /**
       * 检测视频
       * @returns 
       */
      initEventListener() {
        /**
         * 加入频道结果的回调
         */
        aliElectronRtc.on("onJoinChannelResult",(result, channel, elapsed)=>{
          console.log("onJoinChannelResult", result, channel, elapsed);
          if(result === 0) {
            this.$store.commit("SET_RTC_DATA",{ isJoinChannel: true });
          }else {
            console.log(result)
            this.$message({
              message: `joinError[${result}]${Error.parseErrorCode(result)}`,
              type: 'error'
            });
          }
        })
        /**
         * 离开频道结果的回调
         */
        aliElectronRtc.on("onLeaveChannelResult", (result)=> {
          console.log("onLeaveChannelResult", result);
          this.$store.commit("Leave_Channel_Init")
          this.login = true
        })
        /**
         * 远端用户上线回调
         */
        aliElectronRtc.on("onRemoteUserOnLineNotify",(userid, elapsed)=>{
          console.log("onRemoteUserOnLineNotify",userid, elapsed);
          this.$store.commit("ADD_USER",userid)
        })

        /**
         * 远端用户下线回调
         */
        aliElectronRtc.on("onRemoteUserOffLineNotify",(userid, reason)=>{
          // 用户列表
          console.log('onRemoteUserOffLineNotify',userid, reason)
          this.$store.commit("REMOVE_USER",userid)
          this.$store.commit("REMOVE_VIEW",userid)
        })
        /**
         * 音频推流变化回调
         */
        aliElectronRtc.on("onAudioPublishStateChanged",(oldState, newState, elapseSinceLastState, channel)=> {
          console.log("onAudioPublishStateChanged",oldState, newState, elapseSinceLastState, channel)
          if(newState === AliEnginePublishState.AliEngineStatsPublished){
            this.$store.commit("SET_RTC_DATA",{
              isPublishAudio: true,
            });
          }else if(newState === AliEnginePublishState.AliEngineStatsNoPublish) {
            this.$store.commit("SET_RTC_DATA",{
              isPublishAudio: false,
            });
          }
        })
        /**
         * 视频推流变化回调
         */
        aliElectronRtc.on("onVideoPublishStateChanged",(oldState, newState, elapseSinceLastState, channel)=> {
          console.log("onVideoPublishStateChanged",oldState, newState, elapseSinceLastState, channel)
          if(newState === AliEnginePublishState.AliEngineStatsPublished){
            this.$store.commit("SET_RTC_DATA",{
              isPublishCamera: true,
            });
          }else if(newState === AliEnginePublishState.AliEngineStatsNoPublish) {
            this.$store.commit("SET_RTC_DATA",{
              isPublishCamera: false,
            });
          }
        })
        /**
         * 大小流变化回调
         */
        aliElectronRtc.on("onDualStreamPublishStateChanged",(oldState, newState, elapseSinceLastState)=> {
          console.log("onDualStreamPublishStateChanged",oldState, newState, elapseSinceLastState)
          if(newState === AliEnginePublishState.AliEngineStatsPublished){
            this.$store.commit("SET_RTC_DATA",{
              isPublishSimulcast: true,
            });
          }else if(newState === AliEnginePublishState.AliEngineStatsNoPublish) {
            this.$store.commit("SET_RTC_DATA",{
              isPublishSimulcast: false,
            });
          }
        })
        /**
         * 屏幕共享推流信息变化回调
         */
        aliElectronRtc.on("onScreenSharePublishStateChangedWithInfo",( oldState, newState, elapseSinceLastState, channel, screenShareInfo)=> {
          console.log("onScreenSharePublishStateChangedWithInfo",oldState, newState,elapseSinceLastState, channel, screenShareInfo)
          if(newState === AliEnginePublishState.AliEngineStatsPublished){
            this.$store.commit("SET_RTC_DATA",{
              isPublishScreen: true,
            });
          }else if(newState === AliEnginePublishState.AliEngineStatsNoPublish) {
            this.$store.commit("SET_RTC_DATA",{
              isPublishScreen: false,
            });
          }
        })

        /**
         * 远端用户音视频流发生变化时回调	
         */
        aliElectronRtc.on("onRemoteTrackAvailableNotify",(userid, audioTrack, videoTrack)=>{
          console.log("onRemoteTrackAvailableNotify",userid, audioTrack, videoTrack);
          // 用户列表添加
          this.$store.commit("UPDATE_USER", userid)
        })
        /**
         * 订阅音频变化回调
         */
        aliElectronRtc.on("onAudioSubscribeStateChanged", (userid, oldState, newState, elapseSinceLastState, channel)=> {
          console.log('onAudioSubscribeStateChanged',userid, oldState, newState, elapseSinceLastState, channel)
          this.$store.commit("UPDATE_USER", userid)
        })
        /**
         * 订阅视频变化回调
         */
        aliElectronRtc.on("onVideoSubscribeStateChanged", (userid, oldState, newState, elapseSinceLastState, channel)=> {
          console.log('onVideoSubscribeStateChanged',userid, oldState, newState, elapseSinceLastState, channel)
          if(newState === AliEngineSubscribeState.AliEngineStatsSubscribed || newState === AliEngineSubscribeState.AliEngineStatsNoSubscribe){
            this.$store.commit("UPDATE_USER", userid)
            this.$store.commit("UPDATE_VIEW", {
              userid,
              type: VideoType.camera,
              state: newState
            })
          }
        })
        /**
         * 订阅屏幕变化回调
         */
        aliElectronRtc.on("onScreenShareSubscribeStateChanged", (userid, oldState, newState, elapseSinceLastState, channel)=> {
          console.log('onScreenShareSubscribeStateChanged',userid, oldState, newState, elapseSinceLastState, channel)
          if(newState === AliEngineSubscribeState.AliEngineStatsSubscribed || newState === AliEngineSubscribeState.AliEngineStatsNoSubscribe){
            this.$store.commit("UPDATE_USER", userid)
            this.$store.commit("UPDATE_VIEW", {
              userid,
              type: VideoType.screen,
              state: newState
            })
          }
        })
        /**
         * 订阅大小流变化回调
         */
        aliElectronRtc.on("onSubscribeStreamTypeChanged", (userid, oldState, newState, elapseSinceLastState, channel)=> {
          console.log('onSubscribeStreamTypeChanged',userid, oldState, newState, elapseSinceLastState, channel)
          this.$store.commit("UPDATE_USER", userid)
        })

        /**
         * 警告回调	
         */
        aliElectronRtc.on('onOccurWarning', (warnCode, msg) => {
          console.log('onOccurWarning', warnCode, msg)
        });
        /**
         * 错误回调
         */
        aliElectronRtc.on('onOccurError', (errorCode, msg) => {
          switch (errorCode) {
            case AliEngineErrorCode.AliEngineErrorInvaildState:
            case AliEngineErrorCode.AliEngineErrorSessionRemoved:
            case AliEngineErrorCode.AliEngineErrorIceConnectionHeartbeatTimeout:
              this.$message({
                type: "error",
                message: `房间状态错误: ${code}`,
                offset: 150
              })
              setTimeout(()=> {
                aliElectronRtc.leaveChannel();
                aliElectronRtc.destroy()
                aliElectronRtc = null
              }, 3000)
            break;
          }
        });

        /**
         * 被服务器踢出或者频道关闭时回调	
         */
        aliElectronRtc.on('onBye', (code) => {
          let onbyeMessage = ""
          switch (code){
            case AliEngineOnByeType.AliEngineOnByeBeKickOut:
              onbyeMessage = "被服务器踢出";
              break;
            case AliEngineOnByeType.AliEngineOnByeChannelTerminated:
              onbyeMessage = "频道关闭";
              break;
            case AliEngineOnByeType.AliEngineOnByeUserReplaced:
              onbyeMessage = "同一个ID在其他端登录,被服务器踢出";
              break;
            default:
              onbyeMessage = "未知错误onbye";
          }
          this.$message({
            message: onbyeMessage,
            type: 'error'
          });
          setTimeout(()=> {
            window.location.reload();
          }, 1000)
        });
      }
    },
    watch: {
      "$store.state.rtc.isInit"(isInit,o){
        // 实例化后立刻开始监听事件
        if(isInit){
          this.initEventListener();
        }
      }
    }
  }
</script>

<style lang="scss">
  .el-select{
    width: 100%;
  }
</style>
