import { RemoteUser } from 'dingrtc';
import { ComputedRef, Ref } from 'vue';

import { DisplayType, useMainViewStore } from '@src/store/mainView';
import { useSmallViewStreamStore } from '@src/store/smallViewStream';

interface IDbClickProps {
  cameraEnableRef: ComputedRef<boolean>;
  auxiliaryEnableRef: ComputedRef<boolean>;
  userId: string;
  user: Ref<Partial<RemoteUser>>;
}

export const useDbClickSmallView = ({
  cameraEnableRef,
  auxiliaryEnableRef,
  userId,
}: IDbClickProps) => {
  return () => {
    const mainViewStore = useMainViewStore();
    const smallViewMapStore = useSmallViewStreamStore();
    const cameraEnable = cameraEnableRef.value;
    const auxiliaryEnable = auxiliaryEnableRef.value;
    mainViewStore.stream?.stop();
    const oldMainUserSmallStream = smallViewMapStore[mainViewStore.userId];
    oldMainUserSmallStream?.stop();
    const smallViewStream = smallViewMapStore[userId];
    smallViewStream?.stop();
    if (mainViewStore.userId === userId) {
      if (mainViewStore.displayType === DisplayType.Camera) {
        smallViewMapStore[mainViewStore.userId]?.stop();
        const newStatus = auxiliaryEnable ? DisplayType.Screen : DisplayType.Avatar;
        mainViewStore.$patch({
          displayType: newStatus,
        });
      } else if (mainViewStore.displayType === DisplayType.Screen) {
        smallViewMapStore[mainViewStore.userId]?.stop();
        mainViewStore.$patch({
          displayType: cameraEnable ? DisplayType.Camera : DisplayType.Avatar,
        });
      } else {
        if (auxiliaryEnable && cameraEnable) {
          mainViewStore.$patch({
            displayType: DisplayType.Camera,
          });
        } else if (auxiliaryEnable) {
          mainViewStore.$patch({
            displayType: DisplayType.Screen,
          });
        } else if (cameraEnable) {
          mainViewStore.$patch({
            displayType: DisplayType.Camera,
          });
        }
      }
    } else {
      if (!cameraEnable && !auxiliaryEnable) {
        mainViewStore.$patch({
          userId,
          displayType: DisplayType.Avatar,
        });
      } else if (cameraEnable && !auxiliaryEnable) {
        mainViewStore.$patch({
          userId,
          displayType: DisplayType.Camera,
        });
      } else if (!cameraEnable && auxiliaryEnable) {
        mainViewStore.$patch({
          userId,
          displayType: DisplayType.Screen,
        });
      } else if (cameraEnable && auxiliaryEnable) {
        mainViewStore.$patch({
          userId,
          displayType:
            mainViewStore.displayType === DisplayType.Camera
              ? DisplayType.Screen
              : DisplayType.Camera,
        });
      }
    }
  };
};
