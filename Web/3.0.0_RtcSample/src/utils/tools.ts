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

const mobileReg = /(phone|pad|pod|iPhone|iPod|ios|iPad|Android|Mobile|BlackBerry|IEMobile|MQQBrowser|JUC|Fennec|wOSBrowser|BrowserNG|WebOS|Symbian|Windows Phone)/i;
const iOSReg = /(iPhone|iPad|iPod|iOS)/i;
const weixinReg = /micromessenger.([\d.]+)/i

export const isMobile = () => navigator.userAgent.match(mobileReg);

export const isIOS = () => navigator.userAgent.match(iOSReg);

export const isWeixin = () => navigator.userAgent.match(weixinReg);