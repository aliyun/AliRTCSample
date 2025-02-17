@Component
export struct ItemImage {
  @State text: string = "";
  @State img: Resource = $r('sys.media.ohos_ic_public_close');

  ItemImage(text: string, img: Resource) {
    this.text = text;
    this.img = img;
  }

  build() {
    Column() {
      Image(this.img)
        .width('24vp')
        .height('24vp')
        .objectFit(ImageFit.Contain)
      Text(this.text)
        .fontSize("12fp")
        .margin({ top: "4vp" })
    }

  }
}
