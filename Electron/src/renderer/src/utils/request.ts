import configJson from '~/config.json'

export const request = (url: string, params: any) => {
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

const formmatUrl = (url: string, params: any) => {
  if (!params) return url;
  const args = Object.keys(params).map(key => `${key}=${params[key]}`).join('&');
  return `${url.includes('?') ? url : `${url}?`}${args}`;
};

const env = configJson.env;


const APP_SERVER_DOMAIN = `https://${env === 'onertcPre' ? 'pre-' : ''}onertc-demo-app-server.dingtalk.com/`;

// const APP_SERVER_DOMAIN = 'http://localhost:3001';

const token = configJson.token || '';

export const getAppToken = async (
  userId: string,
  appId: string,
  channelId: string,
): Promise<{ token: string, gslb?: string[] }> => {
  if (token) {
    return {
      token,
    }
  }
  const loginParam = {
    passwd: 'hello1234',
    room: channelId,
    tokensid: false,
    env,
    tokenRole: '',
    appid: appId,
    userid: userId,
  };
  const result = (await request(`${APP_SERVER_DOMAIN}/login`, loginParam)) as {
    token: string;
    gslb: string[];
  };
  return result;
};
