/// <reference types="electron-vite/node" />

declare module '*.module.less' {
  const classes: {
    readonly [key: string]: string;
  };
  export default classes;
  declare module '*.less';
}

declare module '*.mp3' {
  export default string;
}

interface HTMLMediaElement {
  captureStream: () => MediaStream;
}

declare module '*.vue' {
  import type {DefineComponent} from 'vue'
  const component: DefineComponent<object, object, any>
  export default component
}