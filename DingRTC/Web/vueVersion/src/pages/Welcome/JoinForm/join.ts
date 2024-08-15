import { message as Toast } from 'ant-design-vue';
import {
  CameraVideoTrack,
  MicrophoneAudioTrack,
  RemoteAudioTrack,
  RemoteUser,
  RemoteVideoTrack,
} from 'dingrtc';

import { useGlobalFlagStore } from '@src/store';
import { useClient } from '@src/store/client';
import { useLocalChannelInfoStore } from '@src/store/localChannelInfo';
import { useRemoteChannelInfoStore } from '@src/store/remoteChannelnfo';
import { print } from '@src/utils/tools';

export const useJoinMethod = () => {
  const remoteChannelStore = useRemoteChannelInfoStore();
  const localChannelStore = useLocalChannelInfoStore();
  const globalStore = useGlobalFlagStore();
  const client = useClient();
  const subscribeUser = async (user: RemoteUser) => {
    const fn = (auxiliary: boolean) =>
      client
        .subscribe(user.userId, 'video', auxiliary)
        .then((track: RemoteAudioTrack | RemoteVideoTrack) => {
          print(`subscribe user ${user.userId} ${auxiliary ? 'screenShare' : 'camera'}}`);
          remoteChannelStore.$patch({
            remoteUsers: [...client.remoteUsers],
          });
          return track;
        })
        .catch((e: Error) => {
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
    if (user.hasVideo) {
      await fn(false);
    }
    if (user.hasAuxiliary) {
      await fn(true);
    }
  };

  const subscribeRemoteUsers = (remoteUsers: RemoteUser[]) => {
    for (const user of remoteUsers) {
      subscribeUser(user).then(() => {
        console.log('sub success : ', user);
      });
    }
    remoteChannelStore.$patch({
      remoteUsers: remoteUsers,
    });
  };

  const publishLocalTrack = () => {
    const localTracks = [localChannelStore.cameraTrack, localChannelStore.micTrack].filter(
      (item) => !!item,
    ) as (CameraVideoTrack | MicrophoneAudioTrack)[];
    if (localChannelStore.cameraTrack) {
      localChannelStore.cameraTrack.stop();
    }
    if (localTracks.length) {
      return client
        ?.publish(localTracks)
        .then(() => {
          localChannelStore.$patch({
            publishedTracks: client.localTracks,
          });
          print(`publish ${localTracks.map((item) => item.trackMediaType)} tracks`);
        })
        .catch((e) => {
          Toast.info(
            `publish ${localTracks.map(
              (item) => item.trackMediaType,
            )} tracks failed: ${JSON.stringify(e)}`,
          );
          print(
            `publish ${localTracks.map(
              (item) => item.trackMediaType,
            )} tracks failed: ${JSON.stringify(e)}`,
          );
          throw e;
        });
    }
  };

  const subscribeAudio = () => {
    return client.subscribe('mcu', 'audio').then((track) => {
      print(`subscribe mcu audio`);
      const audioTrack = track as RemoteAudioTrack;
      remoteChannelStore.$patch({
        mcuAudioTrack: audioTrack,
      });
      globalStore.$patch({
        mcuAudioSubscribed: true,
      });
      audioTrack.play();
    });
  };

  const subscribeInOnePc = (remoteUsers: RemoteUser[]) => {
    const subParams = [{ uid: 'mcu', mediaType: 'audio', auxiliary: false }];
    console.log('type : ', Array.isArray(remoteUsers));
    remoteUsers.forEach((user) => {
      console.log('user : ', user);
      if (user.hasVideo) {
        subParams.push({ uid: user.userId, mediaType: 'video', auxiliary: false });
      }
      if (user.hasAuxiliary) {
        subParams.push({ uid: user.userId, mediaType: 'video', auxiliary: true });
      }
    });
    remoteChannelStore.$patch({
      remoteUsers: remoteUsers,
    });
    // eslint-disable-next-line @typescript-eslint/ban-ts-comment
    // @ts-ignore
    client.batchSubscribe(subParams).then((batchSubscribeResult) => {
      for (const { error, track, uid: usrId, auxiliary } of batchSubscribeResult) {
        if (error) {
          Toast.info(
            `subscribe user ${usrId} ${auxiliary ? 'screenShare' : 'camera'} failed: ${JSON.stringify(error)}`,
          );
          print(
            `subscribe user ${usrId} ${auxiliary ? 'screenShare' : 'camera'} failed: ${JSON.stringify(error)}`,
          );
          continue;
        }
        if (track?.trackMediaType === 'audio') {
          print(`subscribe ${usrId} audio`);
          const audioTrack = track as RemoteAudioTrack;
          remoteChannelStore.$patch({ mcuAudioTrack: audioTrack });
          globalStore.$patch({ mcuAudioSubscribed: true });
          audioTrack.play();
        } else {
          print(`subscribe user ${usrId} ${auxiliary ? 'screenShare' : 'camera'}`);
          remoteChannelStore.$patch({ remoteUsers: client.remoteUsers });
        }
      }
    });
  };
  return { publishLocalTrack, subscribeAudio, subscribeInOnePc, subscribeRemoteUsers };
};
