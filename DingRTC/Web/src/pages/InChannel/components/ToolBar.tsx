import { Button, Row, Space, Col } from 'dingtalk-design-desktop';
import { memo, useCallback } from 'react';
import { useRecoilValue, useSetRecoilState } from 'recoil';
import { client, constantConfig, currentUserInfo, globalFlag, localChannelInfo } from '~/store';
import styles from '../index.module.less';
import { Camera, Mic, Screen } from '~/components/Device';
import { useDevice } from '~/hooks/device';
import Icon from '~/components/Icon';

interface IProps {
  onLeave: () => void;
}

const Index = (props: IProps) => {
  const { onLeave: _onLeave } = props;
  const { hideLog } = useRecoilValue(constantConfig);
  const { screenTrack } = useRecoilValue(localChannelInfo);
  const IClient = useRecoilValue(client);
  const setShowGlobalFlag = useSetRecoilState(globalFlag);
  const { isMobile, env } = useRecoilValue(constantConfig);
  const { channel, appId } = useRecoilValue(currentUserInfo);
  const { operateCamera, operateMic, operateScreen, cameraEnabled, micEnabled } = useDevice('in');
  const onLeave = useCallback(() => {
    _onLeave();
    IClient.leave();
  }, [IClient, _onLeave]);

  const onShowLog = useCallback(() => {
    setShowGlobalFlag((prev) => ({ ...prev, showLog: true }));
  }, []);
  return (
    <Row className={styles.toolBarWrap}>
      <Row className={styles.confInfo} gutter={16}>
        <Col>
          <span>应用: </span>
          <span>{appId}</span>
        </Col>
        <Col>
          <span>环境: </span>
          <span>{env}</span>
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
        {!hideLog && !isMobile ? (
          <Row className={styles.toolBtn} onClick={onShowLog}>
            <Icon type="iconicon_XDS_Calendar" />
            <span style={{ marginTop: 4 }}>操作日志</span>
          </Row>
        ) : null}
      </Space>
      <Button type="danger" onClick={onLeave}>
        离开
      </Button>
    </Row>
  );
};

export default memo(Index);
