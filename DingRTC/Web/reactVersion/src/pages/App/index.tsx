import InChannel from '../../pages/InChannel';
import { memo, useEffect } from 'react';
import { useRecoilValue } from 'recoil';
import { constantConfig, globalFlag } from '../../store';
import Welcome from '../../pages/Welcome';
import { Row, Toast, Modal } from 'dingtalk-design-desktop';
import styles from './index.module.less';
import DingRTC from 'dingrtc';
import { print } from '~/utils/tools';
import VConsole from 'vconsole';
import { useDevice } from '~/hooks/device';


const App = () => {
  const { joined } = useRecoilValue(globalFlag);
  const { updateDeviceList } = useDevice();
  const { hideLog, isMobile, isIOS, isWeixin } = useRecoilValue(constantConfig);
  useEffect(() => {
    if (!hideLog && isMobile) {
      // eslint-disable-next-line no-new
      new VConsole({});
    }
    print('start demo', navigator.userAgent);
    const supported = DingRTC.checkSystemRequirements();
    if (!supported) {
      Toast.error('当前浏览器不支持通话');
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
        // @ts-ignore
        const wxBridge: any = window.WeixinJSBridge;
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
      Modal.centerConfirm({
        content: '由于浏览器自动播放限制，点击确定后才会开始播放音频',
        onOk: () => {
          track.play();
        },
      });
    });
    return () => {
      DingRTC.removeAllListeners();
    };
  }, []);

  return (
    <Row className={styles.wrap}>
      {joined ? <InChannel /> : <Welcome />}
    </Row>
  );
};

export default memo(App);
