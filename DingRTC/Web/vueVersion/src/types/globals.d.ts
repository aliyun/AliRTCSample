interface HTMLAudioElement {
  captureStream(): MediaStream;
}

interface HTMLVideoElement {
  captureStream(): MediaStream;
}

interface Window {
  // eslint-disable-next-line @typescript-eslint/no-explicit-any
  WeixinJSBridge: any;
}

declare module '*.vue' {
  import type { DefineComponent } from 'vue';
  const component: DefineComponent;
  export default component;
}
