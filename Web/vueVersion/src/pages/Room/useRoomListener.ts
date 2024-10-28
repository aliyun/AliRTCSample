import { message as Toast } from 'ant-design-vue';
import { RemoteStreamType, RemoteUser, TrackMediaType } from 'dingrtc';
import { onMounted, onBeforeUnmount, reactive } from 'vue';

import { isInRoom } from '@src/Router';
import { useRemoteChannel } from '@src/hooks/useRemoteChannel';
import { useClient } from '@src/store/client';
import { useRemoteChannelInfoStore } from '@src/store/remoteChannelnfo';
import { print } from '@src/utils/tools';

import { clearRoom } from './clearRoom';

export const useRoomListener = () => {
  const client = useClient();
  const remoteChannelStore = useRemoteChannelInfoStore();
  const { subscribe, unSubscribe } = useRemoteChannel();
  // 初始化监听
  onMounted(() => {
    if (!client) return;
    if (client.connectionState !== 'connected') {
      isInRoom.value = false;
      return;
    }
    client.on('user-joined', (user: RemoteUser) => {
      console.log(`user ${user.userId} joined`);
      remoteChannelStore.$patch({ remoteUsers: client.remoteUsers });
    });

    client.on('stream-type-changed', (uid: string, streamType: RemoteStreamType) => {
      print(`user ${uid} streamType changeTo ${streamType}`);
    });

    client.on('connection-state-change', (current, _, reason) => {
      print(`connection-state-change ${current} ${reason || ''}`);
      if (current === 'disconnected') {
        Toast.info(reason || '');
        clearRoom();
      }
    });
    client.on('user-published', (user, mediaType, auxiliary) => {
      print(`user ${user.userId} published ${auxiliary ? 'screenShare' : mediaType}}`);
      remoteChannelStore.$patch({
        remoteUsers: client.remoteUsers,
      });
      if (mediaType !== 'video') {
        return;
      }
      subscribe(user, 'video', auxiliary);
    });
    client.on(
      'user-unpublished',
      (user: RemoteUser, mediaType: TrackMediaType, auxiliary?: boolean) => {
        remoteChannelStore.$patch({
          remoteUsers: [...client.remoteUsers],
        });
        if (mediaType !== 'video') {
          return;
        }
        unSubscribe(user, 'video', auxiliary);
      },
    );
    client.on('volume-indicator', (uids: string[]) => {
      if (uids.length) {
        print(`${uids.join(',')} is speaking`);
      }
      remoteChannelStore.$patch({ speakers: uids });
    });
    client.on('user-info-updated', (uid, msg) => {
      print(`user ${uid}: ${msg}`);
      remoteChannelStore.$patch({
        remoteUsers: reactive(client.remoteUsers),
      });
    });
    client.on('user-left', (user: RemoteUser) => {
      print(`user ${user.userId} left`);
      remoteChannelStore.$patch({
        remoteUsers: client.remoteUsers,
      });
    });
  });
  onBeforeUnmount(() => {
    client.removeAllListeners();
  });
};
