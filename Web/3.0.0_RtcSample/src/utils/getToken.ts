import { request } from './request';

export const AppConf = {
  appId: '******',
  serverDomain: 'https://***.com/',
};

export const getAppId = () => {
  return AppConf.appId;
};

/**
 *
 * @param userId
 * @param channelName
 * @param appId
 * @returns Promise<{token}>
 */
export const getAppToken = async (
  userId: string,
  appId: string,
  channelId: string,
): Promise<{ token: string }> => {
  const loginParam = {
    passwd: '******',
    room: channelId,
    tokensid: false,
    appid: appId,
    userid: userId,
  };
  const result = (await request(`${AppConf.serverDomain}/login`, loginParam)) as {
    token: string;
  };
  return result;
};
