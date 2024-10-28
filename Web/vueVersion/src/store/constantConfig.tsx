import { defineStore } from 'pinia';

import { dingAppConf, isIOS, isMobile, isWeixin, logLevel } from '@src/utils/tools';

export const useConstanceConfig = defineStore('constantConfig', {
  state: () => ({
    isMobile: !!isMobile(),
    hideLog: logLevel === 'none',
    env: dingAppConf.env,
    isIOS: !!isIOS(),
    isWeixin: !!isWeixin(),
  }),
});
