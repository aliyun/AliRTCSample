import configJson from '@src/config.json';

export const parseSearch = (key: string) => {
  const url = new URL(location.href);
  const params = new URLSearchParams(url.search);
  return params.get(key);
};

export const logLevel = parseSearch('logLevel') === 'debug' ? 'debug' : 'none';

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

export const dingAppConf = {
  appId: parseSearch('appId') || configJson.appId,
  env: parseSearch('env') || configJson.env,
};

export const parseTime = (millseconds: number) => {
  const baseTime = Math.floor(millseconds / 1000);
  const seconds = Math.floor(baseTime % 60);
  const minutes = Math.floor((baseTime % 3600) / 60);
  const hours = Math.floor((baseTime % (24 * 3600)) / 3600);
  return `${hours}:${minutes >= 10 ? minutes : `0${minutes}`}:${
    seconds >= 10 ? seconds : `0${seconds}`
  }`;
};

const mobileReg =
  /(phone|pad|pod|iPhone|iPod|ios|iPad|Android|Mobile|BlackBerry|IEMobile|MQQBrowser|JUC|Fennec|wOSBrowser|BrowserNG|WebOS|Symbian|Windows Phone)/i;
const iOSReg = /(iPhone|iPad|iPod|iOS)/i;
const weixinReg = /micromessenger.([\d.]+)/i;

export const isMobile = () => navigator.userAgent.match(mobileReg);

export const isIOS = () => navigator.userAgent.match(iOSReg);

export const isWeixin = () => navigator.userAgent.match(weixinReg);

export const request = (url: string, params: object) => {
  let res: Response;
  return new Promise((resolve, reject) => {
    fetch(formmatUrl(url, params), {
      headers: {
        'Content-Type': 'application/json',
      },
    })
      .then((response) => {
        res = response;
        if (response.ok) {
          return response.json();
        } else {
          return response.text();
        }
      })
      .then((data) => {
        if (res?.ok) {
          if (data.code) {
            reject(data);
          } else {
            resolve(data.data);
          }
        } else {
          reject(data);
        }
      })
      .catch((error) => {
        reject(error);
      });
  });
};

const formmatUrl = (url: string, params: object) => {
  if (!params) return url;
  const args = Object.keys(params)
    // eslint-disable-next-line @typescript-eslint/ban-ts-comment
    // @ts-ignore
    .map((key) => `${key}=${params[key]}`)
    .join('&');
  return `${url.includes('?') ? url : `${url}?`}${args}`;
};

const token = configJson.token || '';

const APP_SERVER_DOMAIN = 'http://localhost:3001';
export const getAppToken = async (
  userId: string,
  appId: string,
  channelId: string,
): Promise<{ token: string; gslb?: string[] }> => {
  if (appId === '******' || !configJson.appKey) {
    throw new TypeError('请先参考readme 调整参数配置');
  }

  if (token) {
    return {
      token,
    };
  }

  const loginParam = {
    passwd: 'hello1234',
    channelId,
    tokensid: false,
    env: parseSearch('env') || configJson.env,
    tokenRole: '',
    appId,
    userId,
    appKey: configJson.appKey,
  };

  const result = (await request(`${APP_SERVER_DOMAIN}/login`, loginParam)) as {
    token: string;
    gslb: string[];
  };
  return result;
};
export const determineBaseURL = (basePath?: string) => {
  if (basePath) {
    return window.location.origin + basePath;
  }
  const { protocol, host, pathname } = window.location;
  // 检查最后一段是否包含文件名，如 index.html
  const pathSegments = pathname.split('/').filter(Boolean);
  const lastSegment = pathSegments[pathSegments.length - 1];
  const isFile = lastSegment.includes('.');

  if (isFile) {
    // 如果 URL 中包含文件名，则从路径中移除文件名以获取基地址
    pathSegments.pop();
  }

  return `${protocol}//${host}/${pathSegments.join('/')}/`;
};
