import DingRTC from 'dingrtc';
import { onMounted } from 'vue';

import { parseSearch } from '@src/utils/tools';

export const disableSinglePC = parseSearch('singlePC') === 'false';
export const useInitPcMode = () => {
  onMounted(() => {
    if (disableSinglePC) {
      DingRTC.setClientConfig({ singlePC: false });
    }
  });
};
