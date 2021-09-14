<template>
  <div class="view-wrap" :class="userid">
    <div class="video" :id="`${userid}_${type}`" ref="video"></div>
    <div class="display-name">{{displayName}}_{{label}}</div>
  </div>
</template>

<script>
import VideoType from "../../../../data/videotype"
export default {
  props: {
    userid: {
			type: String,
			default: ""
		},
		type: {
			type: Number,
			default: VideoType.camera
		}
  },
  data() {
    return {
      displayName: ""
    }
	},
	mounted() {
		this.$nextTick(() => {
      aliElectronRtc.setRemoteViewConfig(this.userid, this.$refs.video , this.type);
      if(aliElectronRtc.getUserInfo(this.userid)){
        this.displayName = aliElectronRtc.getUserInfo(this.userid).displayName
      }
		});
  },
  computed:{
    label() {
      return this.type === 1 ? "camera" : "screen"
    },
  },
};
</script>

<style lang="scss" scoped>
  .view-wrap{
    background: #000;
    margin-bottom: 10px;
    position: relative;
    cursor: pointer;
    .video{
      width: 182px;
      height: 103px;
      background: transparent;
    }
    .display-name{
      user-select: none;
      position: absolute;
      left: 5px;
      bottom: 5px;
      color: #fff;
      text-shadow: 3px 2px 8px rgba(0,0,0,.8);
    }
  }
</style>
