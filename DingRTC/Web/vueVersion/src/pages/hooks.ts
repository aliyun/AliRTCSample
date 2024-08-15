import { Modal } from 'ant-design-vue';
import DingRTC from 'dingrtc';
import VConsole from 'vconsole';
import { onMounted, onUnmounted } from 'vue';

import { useDevice } from '@src/hooks/useDevices';
import { useConstanceConfig } from '@src/store/constantConfig';
import { print } from '@src/utils/tools';

export const useInitOneRTC = () => {
  const { hideLog, isMobile, isIOS, isWeixin } = useConstanceConfig();
  const { updateDeviceList } = useDevice();
  onMounted(() => {
    if (!hideLog && isMobile) {
      new VConsole({});
    }
    print('start demo', navigator.userAgent);
    const supported = DingRTC.checkSystemRequirements();
    if (!supported) {
      alert('当前浏览器不支持通话');
    }
    DingRTC.on('camera-changed', (info) => {
      updateDeviceList('camera', info);
    });
    DingRTC.on('microphone-changed', (info) => {
      updateDeviceList('microphone', info);
    });
    DingRTC.on('playback-device-changed', (info) => {
      updateDeviceList('playback-device', info);
    });
    DingRTC.on('autoplay-failed', (track) => {
      if (isIOS && isWeixin) {
        const wxBridge = window.WeixinJSBridge;
        print(wxBridge);
        if (wxBridge) {
          wxBridge.invoke('getNetworkType', {}, () => {
            print('autoTriggerPlay');
            track.play();
          });
        } else {
          document.addEventListener(
            'WeixinJSBridgeReady',
            () => {
              wxBridge.invoke('getNetworkType', {}, () => {
                print('autoTriggerPlay');
                track.play();
              });
            },
            false,
          );
        }
        return;
      }
      Modal.info({
        content: '由于浏览器自动播放限制，点击确定后才会开始播放音频',
        onOk: () => {
          track.play();
        },
      });
    });
  });
  onUnmounted(() => {
    DingRTC.removeAllListeners();
  });
};
