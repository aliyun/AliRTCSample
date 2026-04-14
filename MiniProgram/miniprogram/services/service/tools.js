export const logger = {
  info: (...args) => console.info('%c[demo app]:', 'background: rgba(255,255,0,0.3);', ...args),
  error: (...args) => console.error('%c[demo app]:', 'background: rgba(255,255,0,0.3);', ...args),
};

export const getAppToken = async (
  userId,
  appId,
  channelId,
  duration = '',
  delay = '',
  env = 'onertcOnline',
) => {
  // TODO: 让业务服务端用 appid + appsecret 生成 token 返回，前端为了方便调试也可以前端直接生成，但生产环境禁止在前端生成
  const msg = '请实现 getAppToken 函数，返回 token';
  console.error(msg);
  throw new Error(msg);
};

const myFetch = (url, params) => {
  return new Promise((resolve, reject) => {
    wx.request({
      url,
      method: params.method,
      data: params.body,
      header: params.header,
      success: (res) => resolve(res),
      fail: (err) => reject(err),
    });
  });
};

const request = (method, url, params, env, headers = {}, removeAppServer = false) => {
  // @ts-ignore
  const APP_SERVER_DOMAIN = `https://${env === 'onertcPre' ? 'pre-' : ''}onertc-demo-app-server.dingtalk.com/`;
  let res;

  url = `${removeAppServer ? '' : APP_SERVER_DOMAIN}${url}`;
  if (method === 'GET') {
    url = formatUrl(url, params);
  }
  return new Promise((resolve, reject) => {
    myFetch(url, {
      method,
      body: method === 'POST' ? JSON.stringify(params) : undefined,
      headers: {
        'Content-Type': 'application/json',
        ...headers,
      },
    })
      .then((response) => {
        console.log('response====', response);
        if (response.statusCode === 200) {
          res = response;
          resolve(response.data.data);
        } else {
          reject(response.errMsg);
        }
      })
      .catch((error) => {
        reject(error);
      });
  });
};

const formatUrl = (url, params) => {
  if (!params) return url;
  const args = Object.keys(params)
    .map((key) => `${key}=${params[key]}`)
    .join('&');
  return `${url.includes('?') ? url : `${url}?`}${args}`;
};
