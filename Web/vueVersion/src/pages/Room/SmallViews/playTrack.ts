import { Modal } from 'ant-design-vue';
import { LocalVideoTrack, RemoteVideoTrack } from 'dingrtc';
import { Ref } from 'vue';

import { isIOS, isWeixin } from '@src/utils/tools';

export const playTrack = (props: {
  track?: RemoteVideoTrack | LocalVideoTrack;
  wrapper: Ref<HTMLDivElement>;
  objectFit?: 'contain' | 'fill';
}) => {
  if (props.track && props.wrapper.value) {
    if (isIOS() || isWeixin()) {
      Modal.info({
        title: '提示',
        content: 'iOS微信浏览器需要在回调内播放',
        okText: '确认',
        onOk() {
          props.track?.play(props.wrapper.value, {
            fit: props.objectFit || 'cover',
          });
        },
        onCancel() {
          props.track?.play(props.wrapper.value, {
            fit: props.objectFit || 'cover',
          });
        },
      });
    } else {
      setTimeout(() => {
        props.track?.play(props.wrapper.value, {
          fit: props.objectFit || 'cover',
        });
      });
    }
  }
};
