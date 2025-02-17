@Component
export struct CommonTitleBar {
  public showLeft?:boolean = false;
  @Prop leftImg?:Resource = $r('app.media.settings');
  public onLeftClick?: () => void = () =>{ };
  @Prop title?: string = "Title"; // 标题文字
  public showRight?:boolean = false;
  @Prop rightImg?:Resource = $r('app.media.close_icon');
  public onRightClick?: () => void  = () =>{};


  build() {
    Column() {
      Flex({ alignItems: ItemAlign.Center }) {
        Stack({ alignContent: Alignment.Start }) {
          Image(this.leftImg)
            .width(40)
            .height(40)
            .objectFit(ImageFit.ScaleDown)
            .align(Alignment.Center)
            .onClick(() => {
              this.onLeftClick?.()
            })
        }
        .padding({ left: 15 })
        .layoutWeight(1)
        .visibility(this.showLeft ? Visibility.Visible : Visibility.Hidden)

        Stack({ alignContent: Alignment.Center }) {
          Text(this.title)
            .fontSize(20)
              // .fontColor('#0000ff')
            .width('70%')
            .textAlign(TextAlign.Center)
            .height('100%')
        }
        .layoutWeight(2)


        Stack({ alignContent: Alignment.End }) {
          Image(this.rightImg)
            .width(40)
            .height(40)
            .objectFit(ImageFit.ScaleDown)
            .onClick(() => {
              this.onRightClick?.()
            })
        }
        .padding({ right: 15 })
        .height('100%')
        .layoutWeight(1)
        .visibility(this.showRight ? Visibility.Visible : Visibility.Hidden)

      }
      .width('100%')
      .height(50)
      .backgroundColor(Color.White)


      Divider().strokeWidth(1).color('#F1F3F5')
    }

  }
}
