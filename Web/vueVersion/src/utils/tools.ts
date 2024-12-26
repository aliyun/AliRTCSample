let logTraceIdAcc = 0;
export const genTraceId = () => {
  return `trace-${logTraceIdAcc++}`;
};


export const dataURLtoBlob = (dataurl: string) => {
  const arr = dataurl.split(',');
  const mime = arr[0].match(/:(.*?);/)[1];
  const bstr = atob(arr[1]);
  let n = bstr.length;
  const u8arr = new Uint8Array(n);
  while (n--) {
    u8arr[n] = bstr.charCodeAt(n);
  }
  return new Blob([u8arr], { type: mime });
};

export const downloadFile = (url: string, name = "What's the fuvk") => {
  const a = document.createElement('a');
  a.setAttribute('href', url);
  a.setAttribute('download', name);
  a.setAttribute('target', '_blank');
  const clickEvent = document.createEvent('MouseEvents');
  clickEvent.initEvent('click', true, true);
  a.dispatchEvent(clickEvent);
};

export const downloadFileByBase64 = (base64: string, name: string) => {
  const myBlob = dataURLtoBlob(base64);
  const myUrl = URL.createObjectURL(myBlob);
  downloadFile(myUrl, name);
};

export const parseSearch = (key: string) => {
  const url = new URL(location.href);
  const params = new URLSearchParams(url.search);
  return params.get(key);
}

const mobileReg = /(phone|pad|pod|iPhone|iPod|ios|iPad|Android|Mobile|BlackBerry|IEMobile|MQQBrowser|JUC|Fennec|wOSBrowser|BrowserNG|WebOS|Symbian|Windows Phone)/i;
const iOSReg = /(iPhone|iPad|iPod|iOS)/i;
const weixinReg = /micromessenger.([\d.]+)/i

export const isMobile = () => navigator.userAgent.match(mobileReg);

export const isIOS = () => navigator.userAgent.match(iOSReg);

export const isWeixin = () => navigator.userAgent.match(weixinReg);

export const logLevel = parseSearch('logLevel') === 'none' ? 'none' : 'debug';

export const print = (...args: any) => {
  if (logLevel === 'none') return;
  const outputArea = document.getElementById('logOutput');
  if (outputArea) {
    const now = new Date();
    const timeStr = `${now.toLocaleString()}:${now.getMilliseconds()}`;
    outputArea.innerText += `[${timeStr} ${args.join(':')}]\r\n`;
  }
  console.log(...args);
};

export const parseTime = (baseTime: number) => {
  const seconds = Math.floor(baseTime % 60);
  const minutes = Math.floor(baseTime % 3600 / 60);
  const hours =  Math.floor(baseTime / 3600);
  return `${hours}:${minutes >= 10 ? minutes : `0${minutes}`}:${seconds >= 10 ? seconds : `0${seconds}`}`;
};
