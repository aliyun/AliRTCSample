<template>
  <div class="wrap" :class="view.userid">
    <view-item 
      v-for="item in viewDomList" 
      :key="`${item.userid}_${item.type}`" 
      :userid="item.userid" 
      :type="item.type"
    ></view-item>
  </div>
</template>

<script>
import VideoType from "../../../data/videotype"
import ViewItem from "./ViewItem/ViewItem"
export default {
  components: {
    ViewItem
  },
  props: {
    view: {
      type: Object,
      default: {}
    }
  },
  data() {
    return {

    }
  },
  computed:{
    viewDomList(){
      if(!this.view.hasCamera && !this.view.hasScreen ){
          return []
      }
      if(this.view.hasCamera && this.view.hasScreen) {
        return [
            {
              userid: this.view.userid,
              type: VideoType.camera
            },
            {
              userid: this.view.userid,
              type: VideoType.screen
            }
          ]
      }
      if(this.view.hasCamera) {
        return [{
            userid: this.view.userid,
            type: VideoType.camera
          }] 
      }
      if(this.view.hasScreen) {
        return [{
          userid: this.view.userid,
          type: VideoType.screen
        }] 
      }
      // switch(this.view.video_track){
      //   case AliRtcVideoTrack.AliRtcVideoTrackNo:{
      //     return []
      //   }
      //   break;
      //   case AliRtcVideoTrack.AliRtcVideoTrackCamera:{
      //     return [{
      //       user_id: this.view.user_id,
      //       type: VideoType.camera
      //     }]
      //   }
      //   break;
      //   case AliRtcVideoTrack.AliRtcVideoTrackScreen:{
      //     return [{
      //       user_id: this.view.user_id,
      //       type: VideoType.screen
      //     }]
      //   }
      //   break;
      //   case AliRtcVideoTrack.AliRtcVideoTrackBoth:{
      //     return [
      //       {
      //         user_id: this.view.user_id,
      //         type: VideoType.camera
      //       },
      //       {
      //         user_id: this.view.user_id,
      //         type: VideoType.screen
      //       }
      //     ]
      //   }
      //   break;
      //   default:{
      //     return []
      //   }
      // }
    }
  }
};
</script>

<style lang="scss" scoped>
  /* .video-wrap{
    background: #333;
    margin-bottom: 10px;
    position: relative;
    .video{
      width: 182px;
      height: 103px;
      background: transparent;
      
    }
  } */
</style>
