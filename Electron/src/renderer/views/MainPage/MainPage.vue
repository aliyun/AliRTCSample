<template>
  <div class="main-page">
    <main-header />
    <div class="container">
      <div class="container-box">
        <div id="local-view"></div>
      </div>
      <div class="remote-info" v-if="hasUser">
        <user-list></user-list>
      </div>
      <div class="remote-view" v-if="hasUser">
        <view-list></view-list>
      </div>
    </div>
    <main-footer />
  </div>
</template>

<script>
import Utils from "../../utils/utils";
import MainHeader from "./MainHeader/MainHeader";
import MainFooter from "./MainFooter/MainFooter";
import UserList from "../../components/UserList/UserList";
import ViewList from "../../components/ViewList/ViewList";

export default {
  name: "main-page",
  data() {
    return {
      localVideo: null,
    };
  },
  components: {
    MainHeader,
    MainFooter,
    UserList,
    ViewList,
  },
  computed: {
    channel() {
      return this.$store.state.rtc.channel;
    },
    displayName() {
      return this.$store.state.rtc.displayName;
    },
    hasUser() {
      return this.$store.state.rtc.userList.length > 0;
    },
  },
  mounted() {
    this.$nextTick(() => {
      this.localVideo = document.getElementById("local-view");
      this.init();
    });
  },
  methods: {
    init() {
      if (this.channel && this.displayName) {
        aliElectronRtc.setLocalViewConfig(this.localVideo);
        aliElectronRtc.startPreview();
        this.$store.commit("SET_RTC_DATA", {
          isSetLocalVideo: true,
          isPreview: true,
        });
        let authInfo = Utils.getRTCAuthInfo(this.channel)
        authInfo.displayName = this.displayName;
        aliElectronRtc.joinChannel(authInfo, {
          autoSubscribeAudio: true,
          autoSubscribeVideo: true,
        });
      } else {
        this.$message({
          message: "empty channel or displayName",
          type: "error",
        });
      }
    },
  },
};
</script>

<style lang="scss">
/* CSS */
.main-page {
  .container {
    position: absolute;
    display: flex;
    width: 100%;
    // bottom:113px;
    // top:40px;
    top: 0;
    bottom: 0;
    background: #f8f8f8;
    .container-box {
      // width: calc(100% - 436px);
      width: 100%;
      height: 100%;
      position: relative;
      background: #2f2f2f;
      #local-view {
        width: 100%;
        height: 100%;
        background: transparent;
        position: relative;
      }
    }
    .remote-info {
      position: absolute;
      right: 218px;
      margin-left: 9px;
      width: 180px;
      height: calc(100% - 113px);
      padding: 40px 7px 10px 7px;
    }
    .remote-view {
      position: absolute;
      right: 0;
      margin-left: 9px;
      width: 200px;
      height: calc(100% - 113px);
      padding: 40px 9px 10px 9px;
      // background: #fff;
    }
  }
  .slider {
    position: fixed;
    left: 0;
    top: 60px;
    width: 200px;
    height: calc(100% - 200px);
  }
}
</style>
