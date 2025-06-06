import { Button, Row, Space, Col } from 'dingtalk-design-desktop';
import { memo, useCallback, useEffect, useState } from 'react';
import { useRecoilState, useRecoilValue } from 'recoil';
import { client, constantConfig, currentUserInfo, globalFlag, localChannelInfo } from '~/store';
import styles from '../index.module.less';
import { Camera, Mic, Screen } from '~/components/Device';
import { useDevice } from '~/hooks/device';
import Icon from '~/components/Icon';
import Settings from './Settings';
import classNames from 'classnames';

interface IProps {
  onLeave: () => void;
}

const Index = (props: IProps) => {
  const { onLeave: _onLeave } = props;
  const { screenTrack } = useRecoilValue(localChannelInfo);
  const [{ hideToolBar }, setGlobalFlag] = useRecoilState(globalFlag);
  const rtcClient = useRecoilValue(client);
  const [showSetting, setShowSetting] = useState(false);
  const { isMobile } = useRecoilValue(constantConfig);
  const { channel, appId } = useRecoilValue(currentUserInfo);
  // const { rtcStats } = useNetworkStats()
  const { operateCamera, operateMic, operateScreen, cameraEnabled, micEnabled } = useDevice('in');
  const onLeave = useCallback(async () => {
    rtcClient.leave();
    _onLeave();
  }, [rtcClient, _onLeave]);

  useEffect(() => {
    let timer: number = null;
    const hideAfterStill = () => {
      if (timer) clearTimeout(timer);
      setGlobalFlag((prev) => ({ ...prev, hideToolBar: false }));
      timer = window.setTimeout(() => {
        setGlobalFlag((prev) => ({ ...prev, hideToolBar: true }));
      }, 10000);
    }
    document.addEventListener('mousemove', hideAfterStill)
    return () => {
      document.removeEventListener('mousemove', hideAfterStill)
    }
  }, []);

  const toggleShowSettings = useCallback(() => {
    setShowSetting((prev) => !prev)
  }, [])

  return (
    <Row className={classNames(styles.toolBarWrap, hideToolBar ? styles.hideToolBar : '')}>
      <Row className={styles.confInfo} gutter={16}>
        <Col>
          <span>应用: </span>
          <span>{appId}</span>
        </Col>
        <Col>
          <span>频道: </span>
          <span>{channel}</span>
        </Col>
      </Row>
      <Space size={[0, 8]}>
        <Row className={styles.toolBtn} onClick={operateCamera}>
          <Camera />
          <span>{cameraEnabled ? '关摄像头' : '开摄像头'}</span>
        </Row>
        <Row className={styles.toolBtn} onClick={operateMic}>
          <Mic />
          <span>{micEnabled ? '静音' : '解除静音'}</span>
        </Row>
        {isMobile ? null : (
          <Row
            onClick={operateScreen}
            className={`${styles.toolBtn} ${screenTrack ? styles.stopShare : ''}`}
          >
            <Screen />
            <span>{screenTrack ? '结束共享' : '共享'}</span>
          </Row>
        )}
        <Row className={styles.toolBtn} onClick={toggleShowSettings}>
          <Icon type='iconicon_x_Settings' />
          <span style={{ marginTop: 4 }}>设置</span>
        </Row>
      </Space>
      <Button type="danger" onClick={onLeave}>
        离开
      </Button>
      {showSetting ? <Settings onClose={toggleShowSettings} /> : null}
    </Row>
  );
};

export default memo(Index);
