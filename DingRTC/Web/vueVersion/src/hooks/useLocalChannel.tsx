import { message as Toast } from 'ant-design-vue';
import { LocalTrack, LocalVideoTrack, VideoDimension } from 'dingrtc';

import { useClient } from '@src/store/client';
import { useLocalChannelInfoStore } from '@src/store/localChannelInfo';
import { print } from '@src/utils/tools';

interface EncoderConfig {
  type: 'screenTrack' | 'cameraTrack';
  track: LocalVideoTrack;
  dimension?: VideoDimension;
  frameRate?: number;
}

export const useLocalChannel = () => {
  const localInfoStore = useLocalChannelInfoStore();
  const IClient = useClient();
  const publish = (tracks: LocalTrack[]) => {
    return IClient?.publish(tracks)
      .then(() => {
        localInfoStore.$patch({
          publishedTracks: IClient.localTracks,
        });
        print(`publish ${tracks.map((item) => item.trackMediaType)} tracks`);
      })
      .catch((e) => {
        Toast.info(
          `publish ${tracks.map(
            (item) => item.trackMediaType,
          )} tracks failed: ${JSON.stringify(e)}`,
        );
        print(
          `publish ${tracks.map(
            (item) => item.trackMediaType,
          )} tracks failed: ${JSON.stringify(e)}`,
        );
        throw e;
      });
  };

  const updateEncoder = (config: EncoderConfig) => {
    const { track, dimension, frameRate, type } = config;
    track
      .setEncoderConfiguration({
        dimension,
        frameRate,
      })
      .then(() => {
        localInfoStore.updateStore({ [type]: track });
      });
  };

  const unPublish = (tracks: LocalTrack[]) => {
    return IClient.unpublish(tracks)
      .then(() => {
        localInfoStore.$patch({
          publishedTracks: IClient.localTracks,
        });
        print(`unpublish ${tracks.map((item) => item.trackMediaType)} tracks`);
        return;
      })
      .catch((e) => {
        Toast.info(
          `unpublish ${tracks.map(
            (item) => item.trackMediaType,
          )} tracks failed: ${JSON.stringify(e)}`,
        );
        print(
          `unpublish ${tracks.map(
            (item) => item.trackMediaType,
          )} tracks failed: ${JSON.stringify(e)}`,
        );
        throw e;
      });
  };

  return {
    client: IClient,
    publish,
    screenTrack: localInfoStore.screenTrack,
    unPublish,
    micTrack: localInfoStore.micTrack,
    cameraTrack: localInfoStore.cameraTrack,
    updateEncoder,
    publishedTracks: localInfoStore.publishedTracks,
    customVideoTrack: localInfoStore.customVideoTrack,
    customAudioTrack: localInfoStore.customAudioTrack,
  };
};
