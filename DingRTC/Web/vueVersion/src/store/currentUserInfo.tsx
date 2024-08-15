import { defineStore } from 'pinia';

import { dingAppConf } from '@src/utils/tools';

const urlObj = new URL(window.location.href);
const userId = urlObj.searchParams.get('userId');
const channel = urlObj.searchParams.get('channel');

export const useCurrentUserInfo = defineStore('CurrentUserInfo', {
  state: () => ({
    appId: dingAppConf.appId,
    userId: userId || `${Math.ceil(Math.random() * 10000)}`,
    userName: `Web-${Math.ceil(Math.random() * 100)}`,
    channel: channel || `${Math.ceil(Math.random() * 10000)}`,
    channelDuration: 86400,
    channelDelayClose: 60,
  }),
});
