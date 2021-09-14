<template>
  <div class="publish-set">
    <el-button type="primary" @click="audioPubHandler">{{publishAudio ? '停audio流' : '推audio流'}}</el-button>
    <el-button type="primary" @click="cameraPubHandler">{{publishCamera ? '停camera流' : '推camera流'}}</el-button>
    <el-button type="primary" @click="simulcastHandler">{{publishSimulcast ? '停camera多流' : '推camera多流'}}</el-button>
  </div>
</template>

<script>
  export default {
    methods: {
      audioPubHandler() {
        aliElectronRtc.publishLocalAudioStream(!this.publishAudio)
      },
      cameraPubHandler() {
        aliElectronRtc.publishLocalVideoStream(!this.publishCamera)
      },
      simulcastHandler() {
        aliElectronRtc.publishLocalDualStream(!this.publishSimulcast)
      },
    },
    computed:{
      publishAudio() {
        return this.$store.state.rtc.isPublishAudio
      },
      publishCamera() {
        return this.$store.state.rtc.isPublishCamera
      },
      publishSimulcast() {
        return this.$store.state.rtc.isPublishSimulcast
      }
    },
  }
</script>

<style lang="scss" scoped>
  .publish-set{
    /deep/.demonstration{
      font-size: 14px;
      color: #8492a6;
      line-height: 28px;
    }
    .el-checkbox{
      display: block;
      margin-bottom: 10px;
    }
    /deep/.el-row{
      margin-bottom: 10px;
    }
    .el-button{
      width: 30%;
    }
    /deep/.el-button.setEncoder{
      width: 100%;
    }
  }
</style>
