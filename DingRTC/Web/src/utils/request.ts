import { parseSearch } from './tools';

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

const APP_SERVER_DOMAIN = 'https://******';

const token = parseSearch('token') || '';

export const getAppToken = async (
  userId?: string,
  appId?: string,
  channelId?: string,
): Promise<{ token: string, gslb?: string[] }> => {
  console.log(appId, userId, channelId);
  if (token) {
    return {
      token,
      gslb: [],
    }
  }
  const loginParam = {};
  const result = (await request(`${APP_SERVER_DOMAIN}`, loginParam)) as {
    token: string;
    gslb: string[];
  };
  return result;
};
