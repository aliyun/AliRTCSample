import { message as Toast } from 'ant-design-vue';
import { RemoteAudioTrack, RemoteUser, TrackMediaType } from 'dingrtc';
import { storeToRefs } from 'pinia';

import { useClient } from '@src/store/client';
import { useRemoteChannelInfoStore } from '@src/store/remoteChannelnfo';
import { print } from '@src/utils/tools';

export const useRemoteChannel = () => {
  const remoteChannelStore = useRemoteChannelInfoStore();
  const { mcuAudioTrack, speakers } = storeToRefs(remoteChannelStore);
  const IClient = useClient();

  const subscribe = (user: RemoteUser, mediaType: TrackMediaType, auxiliary?: boolean) => {
    return IClient.subscribe(user.userId, mediaType, auxiliary)
      .then((track) => {
        print(`subscribe user ${user.userId} ${auxiliary ? 'screenShare' : 'camera'}}`);
        remoteChannelStore.$patch({ remoteUsers: IClient.remoteUsers });
        console.log('IClient.remoteUsers : ', IClient.remoteUsers);
        return track;
      })
      .catch((e) => {
        Toast.info(
          `subscribe user ${user.userId} ${
            auxiliary ? 'screenShare' : 'camera'
          } failed: ${JSON.stringify(e)}`,
        );
        print(
          `subscribe user ${user.userId} ${
            auxiliary ? 'screenShare' : 'camera'
          } failed: ${JSON.stringify(e)}`,
        );
        throw e;
      });
  };

  const unSubscribe = (user: RemoteUser, mediaType: TrackMediaType, auxiliary?: boolean) => {
    if (!auxiliary && user.videoTrack) user.videoTrack.stop();
    if (auxiliary && user.auxiliaryTrack) user.auxiliaryTrack.stop();

    return IClient.unsubscribe(user.userId, mediaType, auxiliary)
      .then(() => {
        remoteChannelStore.$patch({
          remoteUsers: [...IClient.remoteUsers],
        });
        print(`unsubscribe user ${user.userId} ${auxiliary ? 'screenShare' : 'camera'}`);
      })
      .catch((e) => {
        Toast.info(
          `unsubscribe user ${user.userId} ${
            auxiliary ? 'screenShare' : 'camera'
          } failed: ${JSON.stringify(e)}`,
        );
        print(
          `unsubscribe user ${user.userId} ${
            auxiliary ? 'screenShare' : 'camera'
          } failed: ${JSON.stringify(e)}`,
        );
        throw e;
      });
  };

  const subscribeMCUAudio = () => {
    return IClient.subscribe('mcu', 'audio')
      .then((track) => {
        print(`subscribe mcu audio`);
        remoteChannelStore.$patch({
          mcuAudioTrack: track as RemoteAudioTrack,
        });
        track.play();
        return track;
      })
      .catch((e) => {
        Toast.info(`subscribe mcu audio failed: ${JSON.stringify(e)}`);
        print(`subscribe mcu audio failed: ${JSON.stringify(e)}`);
        throw e;
      });
  };

  const unsubscribeMCUAudio = () => {
    mcuAudioTrack.value?.stop();
    return IClient.unsubscribe('mcu', 'audio')
      .then(() => {
        print(`unsubscribe mcu audio`);
        remoteChannelStore.$patch({ mcuAudioTrack: null });
      })
      .catch((e) => {
        Toast.info(`subscribe mcu audio failed: ${JSON.stringify(e)}`);
        print(`subscribe mcu audio failed: ${JSON.stringify(e)}`);
        throw e;
      });
  };

  return {
    speakers,
    subscribe,
    unSubscribe,
    mcuAudioTrack,
    subscribeMCUAudio,
    unsubscribeMCUAudio,
  };
};
