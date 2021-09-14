import {
  AliEngineScreenShareType,
  AliEngineSubscribeState,
} from '../../data/main'
import VideoType from "../../data/videotype"
import Package from '../../../../package.json'
import AliRtcEngine from 'aliyun-webrtc-electron-sdk'
export default {
  state: {
    isInit: false,
    version: Package.version,
    channel: "",
    displayName: "",

    viewList: [],
    userList: [],

    isJoinChannel: false,
    isSetLocalVideo: false,
    isPreview: false,

    isPublishAudio: false,
    isPublishCamera: false,
    isPublishScreen: false,
    isPublishSimulcast: false,
    isMuteLocalMic: false,
    isMuteLocalCamera: false,
  },
  mutations: {
    SET_RTC_DATA: (state, data) => {
      Object.keys(data).forEach(function (key) {
        state[key] = data[key]
      })
    },
    INIT: (state) => {
      if (!state.isInit) {
        window.aliElectronRtc = new AliRtcEngine({
          extension: {},
          h5CompatibleMode: true
        })
        state.isInit = true
      }
    },
    /**
     * 增加用户
     */
    ADD_USER: (state, userid) => {
      console.log('增加用户')
      let userList = JSON.parse(JSON.stringify(state.userList));
      if (userList.getIndexByProprety(userid, "userid") == -1) {
        let userinfo = aliElectronRtc.getUserInfo(userid);
        userList.push(userinfo)
        state.userList = userList
      }
    },
    /**
     * 更新用户
     */
    UPDATE_USER: (state, userid) => {
      console.log('更新用户')
      let userList = JSON.parse(JSON.stringify(state.userList));
      let userinfo = aliElectronRtc.getUserInfo(userid);
      if (userinfo) {
        console.log(userinfo)
        userList.distinctPush(userinfo, "userid")
        state.userList = userList
      }
    },
    /**
     * 移除用户
     */
    REMOVE_USER: (state, userid) => {
      console.log('移除用户')
      let userList = JSON.parse(JSON.stringify(state.userList));
      let index = userList.getIndexByProprety(userid, "userid")
      if (index !== -1) {
        userList.splice(index, 1)
        state.userList = userList
      }
    },
    /**
     * 更新视图
     */
    UPDATE_VIEW: (state, info) => {
      let viewList = JSON.parse(JSON.stringify(state.viewList));
      let index = viewList.getIndexByProprety(info.userid, "userid")
      let subscribe
      if (index === -1) {
        subscribe = {
          userid: info.userid,
          hasCamera: false,
          hasScreen: false
        }
        if (info.state === AliEngineSubscribeState.AliEngineStatsSubscribed) {
          if (info.type === VideoType.camera) {
            subscribe.hasCamera = true
          } else if (info.type === VideoType.screen) {
            subscribe.hasScreen = true
          }
        }
      } else {
        subscribe = viewList[index]
        if (info.state === AliEngineSubscribeState.AliEngineStatsSubscribed) {
          if (info.type === VideoType.camera) {
            subscribe.hasCamera = true
          } else if (info.type === VideoType.screen) {
            subscribe.hasScreen = true
          }
        } else if (info.state === AliEngineSubscribeState.AliEngineStatsNoSubscribe) {
          if (info.type === VideoType.camera) {
            subscribe.hasCamera = false
          } else if (info.type === VideoType.screen) {
            subscribe.hasScreen = false
          }
        }
      }
      viewList.distinctPush(subscribe, "userid")
      state.viewList = viewList
    },
    /**
     * 移除视图
     */
    REMOVE_VIEW: (state, userid) => {
      let viewList = JSON.parse(JSON.stringify(state.viewList));
      if (viewList.getIndexByProprety(userid, "userid") !== -1) {
        viewList.splice(viewList.getIndexByProprety(userid, "userid"), 1)
        state.viewList = viewList
      }
    },
    Leave_Channel_Init: (state) => {
      state.isJoinChannel = false;
      state.isPreview = false;
      state.isPublishAudio = false;
      state.isPublishCamera = false;
      state.isPublishScreen = false;
      state.isPublishSimulcast = false;
      state.isMuteLocalMic = false;
      state.isMuteLocalCamera = false;
      state.viewList = [];
      state.userList = [];
    }
  },
}