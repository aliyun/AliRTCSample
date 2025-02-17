import prompt from '@ohos.promptAction';
import router from '@ohos.router';
import CommonConstants from '../viewmodel/CommonConstants';
import {CommonTitleBar} from '../view/CommonTitleBar'
import { LoginInfo } from './chat/Model';
import AbilityAccessCtrl from '@ohos.abilityAccessCtrl';
import {DingRtcSDK} from '@dingrtc/dingrtc'


@Extend(Text)
function TextStyle() {
  .fontSize(15)
  .width('30%')
  .height(50)
  .textAlign(TextAlign.Start | TextAlign.Center)
  .backgroundColor(Color.White)
}

@Extend(TextInput)
function TextInputStyle() {
  .width('70%')
  .height(50)
  .copyOption(CopyOptions.LocalDevice)
}

@Extend(Divider)
function DividerStyle() {
  .strokeWidth(1)
  .color('#F1F3F5')
}

@Entry
@Component
struct Home {
  TAG: string = "Home"
  @State cname: string = CommonConstants.CHANNEL_ID
  @State uid: string = CommonConstants.UID_ID
  @State userName: string = ""
  @State isShowProgress: boolean = false
  random_uid: number = Math.floor(Math.random() * 1000000)

  onPageShow() {
    this.requestPermission()
  }


  build() {
    Column() {
      CommonTitleBar({showLeft:true,
        onLeftClick() {
          router.pushUrl({ url: 'pages/Setting'}, (err)=> {
            if(err) {
              console.error(`Invoke pushUrl failed, code is ${err.code}, message is ${err.message}`)
              return
            }
            console.info("Invoke pushUrl success.")
          })
        },
        title:'钉钉会议'})

      Flex({direction: FlexDirection.Column}) {

        Flex({ direction: FlexDirection.Row , justifyContent: FlexAlign.Center }) {
          Text('房间名').TextStyle()
          TextInput({placeholder: "Please input channel name."})
            .type(InputType.Normal)
            .onChange((value: string) => {
              this.cname = value;
            })
            .TextInputStyle()
        }
        .padding({left:0,right:10,top:10,bottom:10})
        Divider().DividerStyle();

        Flex({ direction: FlexDirection.Row , justifyContent: FlexAlign.Center }) {
          Text('用户ID').TextStyle()
          TextInput({placeholder: "Please input user ID." , text: `${this.random_uid}`})
            .type(InputType.Number)
            .onChange((value: string) => {
              this.uid = value;
            })
            .TextInputStyle()

        }
        .padding({left:0,right:10,top:10,bottom:10})
        Divider().DividerStyle();

        Flex({ direction: FlexDirection.Row , justifyContent: FlexAlign.Center }) {
          Text('用户名').TextStyle()
          TextInput({placeholder: "Please input user name." , text: ('ohos_' + this.random_uid) })
            .type(InputType.Normal)
            .onChange((value: string) => {
              this.userName = value;
            })
            .TextInputStyle()
        }
        .padding({left:0,right:10,top:10,bottom:0})

      }
      .height('38%')
      .width('80%')
      .margin(80)
      .padding({left:5,right:10,top:1,bottom:0})
      .backgroundColor(Color.White)

      Button('加入房间', { type: ButtonType.Capsule, stateEffect: true })
        .width('80%')
        .height(50)
        .fontSize(16)
        .fontWeight(FontWeight.Medium)
        .backgroundColor('#007DFF')
        .onClick(()=> {
          this.transToChat();
        })

      LoadingProgress()
        .color(Color.Blue)
        .width(80)
        .height(80)
        .margin({ top: 30 })
        .visibility(this.isShowProgress ? Visibility.Visible : Visibility.Hidden)

      Blank()

      Text('Build version: ' + DingRtcSDK.getSDKBuild())
        .margin({bottom: 10})

    }
    .height('100%')
    .width('100%')
    .backgroundColor($r('app.color.background'))

  }

  async getAppToken() {
    console.info(this.TAG,"start getAppToken")
    this.isShowProgress = false;
    let param: LoginInfo = {
      appId: CommonConstants.APP_ID, //TODO: 使用用户自己的APPID 以及 Token
      token: CommonConstants.TOKEN,
      cname: this.cname,
      uid: this.uid,
      uname: this.userName,
    }

    router.pushUrl({ url: 'pages/Chat',  params: param }, (err)=> {
      if(err) {
        console.error(`Invoke pushUrl failed, code is ${err.code}, message is ${err.message}`)
        return
      }
      console.info("Invoke pushUrl success.")
    })
  }

  transToChat(): void {


    if(this.cname === "") {
      prompt.showToast({ duration: 2000, message: "Please input channel name."})
      return
    }
    if(this.uid === "") {
      prompt.showToast({ duration: 2000, message: "Please input user id."})
      return
    }

    this.isShowProgress = true;
    this.getAppToken();
  }

  async requestPermission() {
    let atManager = AbilityAccessCtrl.createAtManager();
    let context = getContext(this);
    try {
      //Mic.
      atManager.requestPermissionsFromUser(context, ["ohos.permission.MICROPHONE"], (err, data) => {
        if(err) {
          prompt.showToast({message: `request mic error: ${err}`})
          return
        }
        let grant: boolean = data.authResults[0] === AbilityAccessCtrl.GrantStatus.PERMISSION_GRANTED;
        if(!grant) {
          prompt.showToast({message: "麦克风未权限", duration: 2000})
        }

      })

      //Camera.
      atManager.requestPermissionsFromUser(context, ["ohos.permission.CAMERA"], (err, data)=>{
        if(err) {
          prompt.showToast({message: `request camera error: ${err}`})
          return
        }
        if(data.authResults[0] !== AbilityAccessCtrl.GrantStatus.PERMISSION_GRANTED) {
          prompt.showToast({
            message:'摄像头未授权',
            duration:2000
          })
        }

      });
    } catch(err) {
      console.info(this.TAG, `Request permission error: ${err}`)
    }
  }
}