import { defineStore } from 'pinia';

export const useGlobalFlagStore = defineStore('globalFlag', {
  state: () => ({
    showLog: false,
    mcuAudioSubscribed: false,
  }),
  actions: {
    toggleShowLog() {
      this.showLog = !this.showLog;
    },
  },
});
