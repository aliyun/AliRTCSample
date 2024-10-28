import DingRTC from 'dingrtc';
import { defineStore } from 'pinia';
import { reactive } from 'vue';

export const useClient = () => {
  const store = useInnerClientStore();
  return store.client;
};

const client = DingRTC.createClient();
export const useInnerClientStore = defineStore('ClientStore', {
  state: () => ({
    client: reactive(client),
  }),
});
