import { computed } from 'vue';

import { useMainViewStore } from '@src/store/mainView';

import { useAllUsers } from './useAllUsers';

export const useMainUser = () => {
  const allUsers = useAllUsers();
  const mainViewStore = useMainViewStore();
  const mainViewUser = computed(() => {
    return allUsers.value.find((item) => item.userId === mainViewStore.userId) || allUsers.value[0];
  });
  return mainViewUser;
};
