import configJson from '~/config.json';
import { parseSearch } from './tools';
import MD5 from 'crypto-js/md5';

const formmatUrl = (url: string, params: any) => {
  if (!params) return url;
  const args = Object.keys(params)
    .map((key) => `${key}=${params[key]}`)
    .join('&');
  return `${url.includes('?') ? url : `${url}?`}${args}`;
};

const env = parseSearch('env') || configJson.env;

export const request: any = (
  method: 'GET' | 'POST',
  url: string,
  params: object,
  headers: any = {},
) => {
  let res: Response;
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
): Promise<{ token: string; gslb?: string[] }> => {
  if (token) {
    return {
      token,
    };
  }
  const loginParam = {
    channelId,
    appId,
    userId,
    appKey: configJson.appKey,
  };
  const result = (await request('GET', `${APP_SERVER_DOMAIN}/login`, loginParam)) as {
    token: string;
    gslb?: string[] 
  };
  return result;
};

export const startASR = async (appId: string, channel: string, functions: number) => {
  const headers = {
    'DingRTC-Signature': MD5(appId).toString(),
  };

  const result = (await request(
    'POST',
    `${APP_SERVER_DOMAIN}/api/asr/start`,
    {
      appId,
      channelId: channel,
      env,
      function: functions,
    },
    headers,
  )) as { taskId: string };
  return result;
};

export const stopASR = async (appId: string, channel: string, taskId: string) => {
  const headers = {
    'DingRTC-Signature': MD5(appId).toString(),
  };
  return await request(
    'POST',
    `${APP_SERVER_DOMAIN}/api/asr/stop`,
    {
      appId,
      channelId: channel,
      env,
      function: 1,
      taskId,
    },
    headers,
  );
};
