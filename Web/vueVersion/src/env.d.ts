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