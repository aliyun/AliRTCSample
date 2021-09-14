<template>
  <div class="main-footer">
    <div>
      <i
        @click="publishDialogVisible = true"
        :style="'background-image:url(' + publishUrl + ')'"
      ></i>
      <span>推流设置</span>
    </div>
    <div>
      <i
        @click="publishScreen"
        :style="'background-image:url(' + screenShareUrl + ')'"
      ></i>
      <span>屏幕共享</span>
    </div>
    <div>
      <i
        @click="preview"
        :style="'background-image:url(' + previewUrl + ')'"
      ></i>
      <span>预览</span>
    </div>
    <div>
      <i
        @click="joinChannel"
        :class="channelClass"
        :style="'background-image:url(' + channelUrl + ')'"
      ></i>
      <span>频道</span>
    </div>
    <div>
      <i
        @click="muteLocalMic"
        :style="'background-image:url(' + micUrl + ')'"
      ></i>
      <span>静音</span>
    </div>
    <div>
      <i
        @click="muteLocalCamera"
        :style="'background-image:url(' + cameraUrl + ')'"
      ></i>
      <span>摄像头</span>
    </div>
    <el-dialog
      append-to-body
      title="推流"
      :visible.sync="publishDialogVisible"
      width="420px"
      center
    >
      <publish-set @close="publishDialogVisible = false" />
    </el-dialog>
    <el-dialog
      append-to-body
      title="屏幕共享"
      :visible.sync="screenShareDialogVisible"
      destroy-on-close
      width="400px"
      center
    >
      <screen-share @close="screenShareDialogVisible = false" />
    </el-dialog>
  </div>
</template>

<script>
import PublishSet from "../../../components/PublishSet/PublishSet";
import ScreenShare from "../../../components/ScreenShare/ScreenShare";
import { AliEngineVideoTrack } from "../../../data/main";
export default {
  components: {
    PublishSet,
    ScreenShare,
  },
  data() {
    return {
      publishDialogVisible: false,
      screenShareDialogVisible: false,
      publishUrl: require("../../../assets/img/publish.png"),
    };
  },
  methods: {
    beautyFun(enable) {
      this.beauty = enable;
    },
    publishScreen() {
      if (this.funCanUse) {
        if (this.$store.state.rtc.isPublishScreen) {
          aliElectronRtc.stopScreenShare();
        } else {
          this.screenShareDialogVisible = true;
        }
      } else {
        this.$message({
          message: "入会后才能推屏幕流",
          type: "error",
        });
      }
    },
    preview() {
      if (this.$store.state.rtc.isPreview) {
        aliElectronRtc.stopPreview();
        this.$store.commit("SET_RTC_DATA", { isPreview: false });
        this.$message({
          message: "已关闭预览",
          type: "success",
        });
      } else {
        if (!this.$store.state.rtc.isSetLocalVideo) {
          aliElectronRtc.setLocalViewConfig(
            document.getElementById("local-view")
          );
        }
        aliElectronRtc.startPreview();
        this.$store.commit("SET_RTC_DATA", { isPreview: true });
        this.$message({
          message: "已开启预览",
          type: "success",
        });
      }
    },
    joinChannel() {
      if (!this.$store.state.rtc.isJoinChannel) {
        this.$emit("init");
      } else {
        aliElectronRtc.stopPreview();
        aliElectronRtc.leaveChannel();
      }
    },
    muteLocalMic() {
      aliElectronRtc.muteLocalMic(!this.$store.state.rtc.isMuteLocalMic);
      this.$store.commit("SET_RTC_DATA", {
        isMuteLocalMic: !this.$store.state.rtc.isMuteLocalMic,
      });
    },
    muteLocalCamera() {
      aliElectronRtc.muteLocalCamera(
        !this.$store.state.rtc.isMuteLocalCamera,
        AliEngineVideoTrack.AliEngineVideoTrackCamera
      );
      this.$store.commit("SET_RTC_DATA", {
        isMuteLocalCamera: !this.$store.state.rtc.isMuteLocalCamera,
      });
    },
  },
  computed: {
    channelClass() {
      return this.$store.state.rtc.isJoinChannel ? "on" : "off";
    },
    screenShareUrl() {
      return this.$store.state.rtc.isPublishScreen
        ? require("../../../assets/img/screen-on.png")
        : require("../../../assets/img/screen.png");
    },
    // 预览图标url
    previewUrl() {
      return this.$store.state.rtc.isPreview
        ? require("../../../assets/img/preview-on.png")
        : require("../../../assets/img/preview.png");
    },
    // 频道图标url
    channelUrl() {
      return this.$store.state.rtc.isJoinChannel
        ? require("../../../assets/img/off.png")
        : require("../../../assets/img/on.png");
    },
    // 麦克风图标url
    micUrl() {
      return this.$store.state.rtc.isMuteLocalMic
        ? require("../../../assets/img/mic-on.png")
        : require("../../../assets/img/mic.png");
    },
    // 相机图标url
    cameraUrl() {
      return this.$store.state.rtc.isMuteLocalCamera
        ? require("../../../assets/img/camera-on.png")
        : require("../../../assets/img/camera.png");
    },
    // 实例化后且入会才能进行操作
    funCanUse() {
      return this.$store.state.rtc.isJoinChannel;
    },
  },
};
</script>

<style lang="scss">
.main-footer {
  position: absolute;
  bottom: 0;
  left: 0;
  height: 113px;
  width: 100%;
  // background: #f8f8f8;
  display: flex;
  justify-content: center;
  div {
    margin: 0 13px;
    display: flex;
    flex-direction: column;
    justify-content: center;
    align-items: center;
    height: 100%;
    i {
      width: 60px;
      height: 60px;
      border-radius: 30px;
      background-color: #e5e5e5;
      margin-bottom: 12px;
      cursor: pointer;
      background-repeat: no-repeat;
      background-position: center center;
    }
    .water-mark {
      width: 60px;
      height: 60px;
      border-radius: 0px;
      background-color: transparent;
    }
    .on {
      background-color: #f5222d;
    }
    .off {
      background-color: #67c23a;
    }
    span {
      font-size: 14px;
      font-weight: 800;
      text-shadow: 1px 1px 3px #fff;
    }
  }
}
</style>
