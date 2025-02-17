import router from '@ohos.router';
import common from '@ohos.app.ability.common';
import window from '@ohos.window';
import Prompt from '@system.prompt';
import {CommonTitleBar} from '../view/CommonTitleBar'
import * as model from './chat/Model';
import { ChatPresenter } from './chat/Presenter';
import Delegate from './chat/Delegate';
import { DingRtcConstants,DingRtcVideoView } from '@dingrtc/dingrtc';
import { backgroundTaskManager } from '@kit.BackgroundTasksKit';
import { wantAgent, WantAgent } from '@kit.AbilityKit';
import { BusinessError } from '@kit.BasicServicesKit';

@Extend(Image)
function imageStyle() {
  .width('20%')
  .objectFit(ImageFit.ScaleDown)
  .sourceSize({width:30,height:30})
}

function timeConversion(time: number) : string {
  let hour = 0;
  let minutes = 0;
  let second = 0;
  const temp = time % 3600;

  if (time > 3600) {
    hour = Math.floor(time / 3600);
    if (temp !== 0) {
      if (temp > 60) {
        minutes = Math.floor(temp / 60);
        if (temp % 60 !== 0) {
          second = temp % 60;
        }
      } else {
        second = temp;
      }
    }
  } else {
    minutes = Math.floor(time / 60);
    if (time % 60 !== 0) {
      second = time % 60;
    }
  }

  return `${hour < 10 ? '0' + hour : hour}:${minutes < 10 ? '0' + minutes : minutes}:${second < 10 ? '0' + second : second}`;
}

@Entry
@Component
struct Chat {

  TAG: string = "MultiChat"
  private context = getContext(this) as common.UIAbilityContext;
  myUid: string = ''
  chat: ChatPresenter = ChatPresenter.getInstance()
  intervalID: number = -1;
  @State loginInfo? : model.LoginInfo = {
    appId: "",
    cname: "",
    uid: "",
    token: "",
  }
  @State local: model.User = new model.User("")
  @State users: Array<model.User> = []
  @State roomInfo: string = "房间名: "
  @State title: string = "钉钉会议"
  @State isShowMore: boolean = false;
  @State isShowStats: boolean = true;


  startContinuousTask() {
    let wantAgentInfo: wantAgent.WantAgentInfo = {
      // 点击通知后，将要执行的动作列表
      // 添加需要被拉起应用的bundleName和abilityName
      wants: [
        {
          bundleName: "com.ding.rtc.demo",
          abilityName: "DemoAbility"
        }
      ],
      // 指定点击通知栏消息后的动作是拉起ability
      actionType: wantAgent.OperationType.START_ABILITY,
      // 使用者自定义的一个私有值
      requestCode: 0,
      // 点击通知后，动作执行属性
      actionFlags: [wantAgent.WantAgentFlags.UPDATE_PRESENT_FLAG]
    };

    // 通过wantAgent模块下getWantAgent方法获取WantAgent对象
    wantAgent.getWantAgent(wantAgentInfo).then((wantAgentObj: WantAgent) => {
      backgroundTaskManager.startBackgroundRunning(this.context,
        backgroundTaskManager.BackgroundMode.AUDIO_RECORDING, wantAgentObj).then(() => {
        // 此处执行具体的长时任务逻辑，如放音等。
        console.info(`Succeeded in operationing startBackgroundRunning.`);
      }).catch((err: BusinessError) => {
        console.error(`Failed to operation startBackgroundRunning. Code is ${err.code}, message is ${err.message}`);
      });
    });
  }

  stopContinuousTask() {
    backgroundTaskManager.stopBackgroundRunning(this.context).then(() => {
      console.info(`Succeeded in operationing stopBackgroundRunning.`);
    }).catch((err: BusinessError) => {
      console.error(`Failed to operation stopBackgroundRunning. Code is ${err.code}, message is ${err.message}`);
    });
  }


  delegate: Delegate = {
    onJoin: (result: number, channel: string, userId: string, elapsed: number): void => {
      if(result != 0) {
        Prompt.showToast({ duration: 2000, message: 'joinChannel failed result: ' + result + ' ,elapsed: ' + elapsed });
        return;
      }
      this.chat.pubAudio(this.local.isAudioOn);
      this.chat.pubVideo(this.local.isVideoOn);
      // this.chat.enableSpeakerphone(this.local.isSpeakerphone);

      this.startContinuousTask();
    },

    onLeave: (result: number): void => {
      Prompt.showToast({ duration: 2000, message: 'leaveChannel result: ' + result });
      this.stopContinuousTask();
    },

    onUserJoin: (uid: string): void => {
      let user = this.users.find(user => user.uid === uid);
      if (!user) {
        user = new model.User(uid);
        user.nickName = this.chat.getUserInfo(uid)?.displayName;
        user.local = (uid == this.myUid);
        if (user.local) {
          this.users[0] = user;
        }
        if (uid != this.myUid) {
          this.users.push(user);
        }
        this.updateRoomInfo();
      } else {
        console.warn(`onUserJoin: ${uid} is exist, ignore.`);
      }
    },

    onUserLeave: (uid: string, reason: number): void => {
      let user = this.users.find(user => user.uid === uid);
      if (user) {
        this.users = this.users.filter(user => user.uid !== uid);
        this.updateRoomInfo();
      } else {
        console.warn(`onUserLeave: ${uid} is not exist, ignore.`);
      }
    },

    onChannelRemainingTimeNotify: (remainingTimeInSec: number): void => {
      Prompt.showToast({ duration: 2000, message: '频道将在: ' + remainingTimeInSec + ' 秒后结束' });
      let count = remainingTimeInSec;
      this.intervalID = setInterval(() => {
        if (count <= 0) {
          if(this.intervalID != -1) {
            clearInterval(this.intervalID)
          }
          this.title = "会议已结束";
          return;
        }
        count--
        this.title = timeConversion(count);
      }, 1000)
    },

    onBye: (code: DingRtcConstants.RtcEngineOnByeType): void => {
      Prompt.showToast({ duration: 2000, message: 'onBye code: ' + code });
    },

    onConnectionStatusChanged: (status: DingRtcConstants.RtcEngineConnectionStatus,
      reason: DingRtcConstants.RtcEngineConnectionStatusChangeReason): void => {
      console.warn(`onConnectionStatusChanged status: ${status} ,reason : ${reason}.`);
    },

    onNetworkQualityChanged: (uid: string, upQuality: DingRtcConstants.RtcEngineNetworkQuality,
      downQuality: DingRtcConstants.RtcEngineNetworkQuality): void => {
      //TODO:
    }
  }

  async keepScreenOn() {
    let windowClass = await window.getLastWindow(this.context) //获取窗口实例
    await windowClass.setWindowKeepScreenOn(true) //设置窗口常亮或取消
  }

  updateRoomInfo() {
    this.roomInfo ="房间名: " + this.loginInfo?.cname + " ,  当前房间人数: " + this.users.length;
  }

  initPrefer() : void {
    this.local.isAudioOn = true;
    this.local.isVideoOn = true;
    this.local.isPreviewOn = this.local.isVideoOn;
    this.local.isCameraOn = this.local.isVideoOn;
    this.local.isSpeakerphone = true;
  }

  aboutToAppear(): void {
    console.info(this.TAG, '=== aboutToAppear ===');
    this.keepScreenOn();
    this.chat = ChatPresenter.getInstance()
    this.chat.addDelegate(this.delegate)
    this.loginInfo = router.getParams() as model.LoginInfo;
    this.myUid = this.loginInfo.uid;
    this.local = new model.User(this.loginInfo?.uid ?? '');
    this.local.nickName = this.loginInfo.uname;
    this.initPrefer();
    this.users.push(this.local)
    this.chat.createRtc(this.context,DingRtcConstants.LogLevel.Info);
    this.chat.setDefaultAudioRouteToSpeakerphone(this.local.isSpeakerphone);
    this.chat.setVideoEncodeConfig(this.local.width,this.local.height);
    this.chat.changePreview(this.local.isPreviewOn);
    this.chat.join(this.loginInfo);
    this.updateRoomInfo();
  }

  aboutToDisappear(): void {
    console.info(this.TAG, '=== aboutToDisappear ===');
    if(this.intervalID != -1) {
      clearInterval(this.intervalID)
    }
    this.chat.leave()
    this.chat.removeDelegate(this.delegate)
    this.chat.release()
  }

  onPageShow() {
    console.info(this.TAG, '=== onPageShow ===');

  }


  onPageHide(): void {
    console.info(this.TAG, '=== onPageHide ===');

  }

  @Builder popupBuilder() {
    Flex({direction: FlexDirection.Row}) {

      Image(this.local.isCameraOn? $r('app.media.camera_on') : $r('app.media.camera_off'))
        .imageStyle()
        .overlay(this.local.isCameraOn? "停止采集" : "开启采集",{align:Alignment.Bottom,offset:{x:0,y:5}})
        .onClick(()=>{
          if(this.chat.enableVideo(!this.local.isCameraOn)) {
            this.local.isCameraOn = !this.local.isCameraOn
          }
        })

      Image(this.local.isFrontCamera? $r('app.media.switch_camera_1') : $r('app.media.switch_camera_2'))
        .imageStyle()
        .overlay("切换相机",{align:Alignment.Bottom,offset:{x:0,y:5}})
        .onClick(()=>{
          this.local.isFrontCamera = !this.local.isFrontCamera;
          this.chat.switchCamera()
        })

      Image(this.local.isAudioMuted? $r('app.media.mute_audio_1') : $r('app.media.mute_audio_2'))
        .imageStyle()
        .overlay(this.local.isAudioMuted? "恢复音频" : "Mute音频",{align:Alignment.Bottom,offset:{x:0,y:5}})
        .onClick(()=>{
          if(this.chat.muteAudio(!this.local.isAudioMuted)) {
            this.local.isAudioMuted = !this.local.isAudioMuted
          }
        })

      Image(this.local.isVideoMuted? $r('app.media.mute_video_1') : $r('app.media.mute_video_2'))
        .imageStyle()
        .overlay(this.local.isVideoMuted? "恢复视频" : "Mute视频",{align:Alignment.Bottom,offset:{x:0,y:5}})
        .onClick(()=>{
          if(this.chat.muteVideo(!this.local.isVideoMuted)) {
            this.local.isVideoMuted = !this.local.isVideoMuted
          }
        })

      Image(this.isShowStats? $r('app.media.stats') : $r('app.media.stats'))
        .imageStyle()
        .overlay(this.isShowStats? "关闭统计" : "恢复统计",{align:Alignment.Bottom,offset:{x:0,y:5}})
        .onClick(()=>{
        })

    }
    .padding(10)
    .width("100%")
    .height('200vp')
    .backgroundColor(Color.White)
  }

  build() {
    RelativeContainer() {

      CommonTitleBar({showLeft: true, leftImg:this.local.isSpeakerphone ? $r('app.media.speaker') : $r('app.media.ear_speaker'),
        onLeftClick:() => {
          this.local.isSpeakerphone = !this.local.isSpeakerphone;
          this.chat.enableSpeakerphone(this.local.isSpeakerphone);
        },title:this.title, showRight:true, onRightClick:()=> {
          router.back()
        }
        })
        .id('title')
        .alignRules({top: { anchor: '__container__', align: VerticalAlign.Top },})

      Flex({direction: FlexDirection.Row}) {
        Image(this.local.isAudioOn? $r('app.media.pub_audio') : $r('app.media.unpub_audio'))
          .imageStyle()
          .overlay(this.local.isAudioOn? "关闭音频" : "打开音频",{align:Alignment.Bottom,offset:{x:0,y:20}})
          .onClick(()=>{
            if(this.chat.pubAudio(!this.local.isAudioOn)) {
              this.local.isAudioOn = !this.local.isAudioOn
            }
          })

        Image(this.local.isVideoOn? $r('app.media.pub_video') : $r('app.media.unpub_video'))
          .imageStyle()
          .overlay(this.local.isVideoOn? "关闭视频" : "打开视频",{align:Alignment.Bottom,offset:{x:0,y:20}})
          .onClick(()=>{
            if(this.chat.pubVideo(!this.local.isVideoOn)) {
              this.local.isVideoOn = !this.local.isVideoOn
            }
          })

        Image(this.local.isPreviewOn? $r('app.media.preview_on') : $r('app.media.preview_off'))
          .imageStyle()
          .overlay(this.local.isPreviewOn? "关闭预览" : "打开预览",{align:Alignment.Bottom,offset:{x:0,y:20}})
          .onClick(()=>{
            if(this.chat.changePreview(!this.local.isPreviewOn)) {
              this.local.isPreviewOn = !this.local.isPreviewOn
            }
          })

        Image(this.local.isScreenOn? $r('app.media.pub_screen') : $r('app.media.unpub_screen'))
          .imageStyle()
          .overlay(this.local.isScreenOn? "关闭共享" : "打开共享",{align:Alignment.Bottom,offset:{x:0,y:20}})
          .onClick(()=>{
          })

        Image($r('app.media.more'))
          .imageStyle()
          .overlay("更多",{align:Alignment.Bottom,offset:{x:0,y:20}})
          .onClick(()=>{
            this.isShowMore = true;
          })
          .bindPopup(this.isShowMore, {
            builder: this.popupBuilder,
            placement: Placement.Top,
            showInSubWindow: false,
            onStateChange: (event) => {
              if (!event.isVisible) {
                this.isShowMore = false
              }
            },
            // 设置弹窗显示动效与退出动效为缩放动效
            transition:TransitionEffect.scale({ x: 1, y: 0 }).animation({ duration: 500, curve: Curve.Ease })
          })
      }
      .id('bottom_bar')
      .alignRules({
        bottom: { anchor: '__container__', align: VerticalAlign.Bottom },
        left: { anchor: '__container__', align: HorizontalAlign.Start }})
      .padding(10)
      .width("100%")
      .height('47vp')
      .backgroundColor(Color.White)

      Text(this.roomInfo)
        .id('room_info')
        .fontColor(Color.Red)
        .fontSize(15)
        .align(Alignment.TopStart)
        .fontWeight(FontWeight.Medium)
        .alignRules({
          bottom: { anchor: 'bottom_bar', align: VerticalAlign.Top },
          left: { anchor: '__container__', align: HorizontalAlign.Start }})
        .margin({left: 10, bottom: 10})


      Flex({ direction: FlexDirection.Column }) {
        Grid(){
          ForEach(this.users, (item: model.User) => {
            GridItem() {
              Stack({ alignContent: Alignment.TopStart }) {
                DingRtcVideoView({
                  canvasId: item.uid,
                  onLoad: (() => {
                    console.info(this.TAG, `User:${item.uid} surface create.`)
                    let user = this.users.find(user => user.uid == item.uid)
                    if(user) this.chat?.attach(user)
                  }),
                  onDestroy: (() => {
                    console.info(this.TAG, `User:${item.uid} surface release.`)
                  })
                }).width('100%').height('100%')
                Text(`${item.uid} ${item.nickName}`)
                  .width('100%')
                  .fontColor(Color.White)
                  .textAlign(TextAlign.Start)
                  .margin(2)
              }
            }.width('50%').height(250)
          }, (item: model.User) => `${item.uid}`)
        }
        .cachedCount(4)
        .columnsGap(1)
        .rowsGap(1)
        .columnsTemplate('1fr 1fr')
      }
      .alignRules({
        top: { anchor: 'title', align: VerticalAlign.Bottom },
        bottom: { anchor: 'room_info', align: VerticalAlign.Top },
        left: { anchor: '__container__', align: HorizontalAlign.Start }
      })
      .padding(2)
    }
    .width('100%')
    .height('100%')
    .padding({left:1,right:1})
    .backgroundColor($r('app.color.background'))
  }
}