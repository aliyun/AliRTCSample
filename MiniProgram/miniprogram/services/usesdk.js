const DingRTC = require('../jssdk/dingrtc_miniapp_client.js').default;
const { getAppToken, logger } = require('../services/service/tools.js');
const { store } = require('../services/service/store.js');

DingRTC.setClientConfig({ _consoleLogLevel: 'debug' });
DingRTC.setLogLevel('debug');
store.client = DingRTC.createClient();

export const sdkJoin = async (joinParam) => {
  const uid = joinParam.userId;
  const appId = joinParam.appId;
  const channelName = joinParam.channelId;
  const env = joinParam.env;
  const newDuration = '';
  const newDelay = '';

  store.joinParam = { uid, appId, channelName, env };

  const appTokenResult = await getAppToken(uid, appId, channelName, newDuration, newDelay, env);
  logger.info('appTokenResult', appTokenResult);

  if (appTokenResult.gslb) {
    DingRTC.setClientConfig({
      gslb: appTokenResult.gslb
    });
  } else {
    DingRTC.setClientConfig({
      gslb:
        env === 'onertcOnline'
          ? 'https://gslb.dingrtc.com'
          : 'https://pre-gslb.dingrtc.com',
    });
  }


  const joinResult = await store.client.join({
    appId,
    token: appTokenResult.token,
    uid,
    channel: channelName,
    userName: joinParam.userName,
    initMicMuted: true,
  });

  logger.info('joinResult', joinResult);
};

export const sdkLeave = async () => {
  const leaveResult = await store.client.leave();
  logger.info('leaveResult', leaveResult);
};

export const sdkPub = async () => {
  const pubResult = await store.client.publish();
  logger.info('pub result', pubResult);
  return pubResult;
};

export const sdkUnpub = async () => {
  const unpubResult = await store.client.unpublish();
  logger.info('unpub result', unpubResult);
};

export const sdkSubAudio = async () => {
  const subAudioResult = await store.client.subscribe('mcu', 'audio');
  logger.info('subAudioResult remote info', subAudioResult);
  return subAudioResult;
};

export const sdkUnsubAudio = async () => {
  const unsubAudioResult = await store.client.unsubscribe('mcu', 'audio');
  logger.info('unsubAudioResult', unsubAudioResult);
  return unsubAudioResult;
};

export const sdkSubVideo = async (userItem) => {
  const videoUser = userItem;
  if (!videoUser) {
    logger.info('no user to sub');
    return;
  }
  store.client.setRemoteDefaultVideoStreamType('FHD');
  const subVideoResult = await store.client.subscribe((videoUser.userId), 'video');
  logger.info('subVideoResult remote info', subVideoResult);
  return subVideoResult;
};

export const sdkUnsubVideo = async (userItem) => {
  if (!userItem || !userItem?.userId) {
    logger.info('no user to unsub');
    return;
  }
  const subVideoResult = await store.client.unsubscribe(userItem.userId, 'video');
  logger.info('sdkUnsubVideo result', subVideoResult);
  return subVideoResult;
};
