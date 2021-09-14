<template>
  <div class="user-info">
    <div class="info-middle">
        <div class="info-middle-left">
          <div>
            <el-avatar size="large" :src="circleUrl"></el-avatar>
          </div>
          <div class="displayName">{{user.displayName}}</div>
        </div>
        <div class="info-middle-right">
          <div class="wrap" >
            <el-checkbox v-model="user.subScribedAudio" :disabled="!user.hasAudio">音频</el-checkbox>
            <div class="top" :class="user.hasAudio ? 'pointer':'not-allowed'" @click.stop="audio"></div>
          </div>
          <div class="wrap">
            <el-checkbox v-model="user.subScribedCameraMaster" :disabled="!user.hasCameraMaster">大流</el-checkbox>
            <div class="top" :class="user.hasCameraMaster ? 'pointer':'not-allowed'" @click.stop="cameraMaster"></div>
          </div>
          <div class="wrap">
            <el-checkbox v-model="user.subScribedCameraSlave" :disabled="!user.hasCameraSlave">小流</el-checkbox>
            <div class="top" :class="user.hasCameraSlave ? 'pointer':'not-allowed'" @click.stop="cameraSlave"></div>
          </div>
          <div class="wrap">
            <el-checkbox v-model="user.subScribedScreenSharing" :disabled="!user.hasScreenSharing">屏幕</el-checkbox>
            <div class="top" :class="user.hasScreenSharing ? 'pointer':'not-allowed'" @click.stop="screenSharing"></div>
          </div>
      </div>            
    </div>
  </div>
</template>

<script>
import {AliEngineVideoTrack, AliEngineVideoStreamType} from "../../../data/main"
export default {
  props: {
    user: {
      type: Object,
      default: {}
    }
  },
  data(){
    return {
      remoteDialogVisible: false,
      circleUrl: "https://cube.elemecdn.com/3/7c/3ea6beec64369c2642b92c6726f1epng.png",
      muteState: this.user.muteAudioPlaying
    }
  },
  
  methods: {
    audio(){
      if(!this.user.hasAudio) return;
      aliElectronRtc.subscribeRemoteAudioStream(this.user.userid, !this.user.subScribedAudio);
    },
    /**
     * 视频大流
     * 订阅了小流的时候，切换为大流
     * 没有订阅小流的时候，切换是否订阅
     */
    cameraMaster(){
      if(!this.user.hasCameraMaster) return;
      if(this.user.subScribedCameraSlave){
        aliElectronRtc.setRemoteVideoStreamType(this.user.userid, AliEngineVideoStreamType.AliEngineVideoStreamTypeHigh);
      }else {
        aliElectronRtc.subscribeRemoteVideoStream(this.user.userid, AliEngineVideoTrack.AliEngineVideoTrackCamera, !this.user.subScribedCameraMaster);
      }
    },
    /**
     * 视频小流
     * 订阅大流的时候，切换为小流
     * 没有订阅大流的时候，切换为小流并设置订阅
     */
    cameraSlave(){
      if(!this.user.hasCameraSlave) return;
      if(this.user.subScribedCameraMaster) {
        aliElectronRtc.setRemoteVideoStreamType(this.user.userid, AliEngineVideoStreamType.AliEngineVideoStreamTypeLow);
      }else {
        if(this.user.subScribedCameraSlave){
          aliElectronRtc.subscribeRemoteVideoStream(this.user.userid, AliEngineVideoTrack.AliEngineVideoTrackCamera, false);
        }else{
          aliElectronRtc.setRemoteVideoStreamType(this.user.userid, AliEngineVideoStreamType.AliEngineVideoStreamTypeLow);
          aliElectronRtc.subscribeRemoteVideoStream(this.user.userid, AliEngineVideoTrack.AliEngineVideoTrackCamera, true);
        }
      }
    },
    screenSharing(){
      if(!this.user.hasScreenSharing) return;
      aliElectronRtc.subscribeRemoteVideoStream(this.user.userid, AliEngineVideoTrack.AliEngineVideoTrackScreen, !this.user.subScribedScreenSharing);
    },
  }
};
</script>

<style lang="scss">
  .user-info{
    border-radius: 4px;
    padding: 10px;
    background: #fff;
    box-shadow: 0 2px 4px rgba(0, 0, 0, .12), 0 0 6px rgba(0, 0, 0, .04);
    margin: 5px 6px;
    .info-middle{
      display: flex;
      >div{
        flex: 1;
        .mute{
          cursor: pointer;
          font-size: 24px;
        }
        .displayName{
          font-size: 14px;
          text-align: center;
        }
        .wrap{
          position: relative;
          .top{
            z-index: 100;
            position: absolute;
            left:0;
            top: 0;
            width: 100%;
            height: 100%;
          }
          .pointer{
            cursor: pointer;
          }
          .not-allowed{
            cursor: not-allowed;
          }
        }
      }
    }
  }
</style>
