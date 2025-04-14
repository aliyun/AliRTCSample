import configJson from '~/config.json'

const formmatUrl = (url: string, params: any) => {
  if (!params) return url;
  const args = Object.keys(params).map(key => `${key}=${params[key]}`).join('&');
  return `${url.includes('?') ? url : `${url}?`}${args}`;
};


export const request: any = (
  method: 'GET' | 'POST',
  url: string,
  params: object,
  headers: any = {},
  removeAppServer = false,
) => {
  let res: Response;
  url = `${removeAppServer ? '' : APP_SERVER_DOMAIN}${url}`;
  if (method === 'GET') {
    url = formmatUrl(url, params);
  }
  return new Promise((resolve, reject) => {
    fetch(url, {
      method,
      body: method === 'POST' ? JSON.stringify(params) : undefined,
      headers: {
        'Content-Type': 'application/json',
        ...headers,
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
          if (data.code && data.code !== 200) {
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


const APP_SERVER_DOMAIN = 'http://localhost:3001';

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
    channelId,
    appId,
    userId,
    appKey: configJson.appKey,
  };
  const result = (await request('GET', `${APP_SERVER_DOMAIN}/login`, loginParam)) as {
    token: string;
    gslb: string[];
  };
  return result;
};
