import { Row, Col, Divider, Avatar } from 'dingtalk-design-desktop';
import { memo, useEffect, useRef } from 'react';
import styles from '../index.module.less';
import { Camera, Mic } from '~/components/Device';
import { useDevice } from '~/hooks/device';
import { useRecoilValue } from 'recoil';
import { currentUserInfo } from '~/store';

const Preview = () => {
  const { cameraEnabled, openMicAndCameraSameTime, operateCamera, operateMic } = useDevice('pre');
  const { userName } = useRecoilValue(currentUserInfo);
  const ref = useRef(null);

  useEffect(() => {
    openMicAndCameraSameTime().then(([track]) => {
      track.play(ref.current, { fit: 'cover' });
    })
  }, []);

  return (
    <Row className={styles.previewWrap}>
      <Col ref={ref} className={!cameraEnabled ? styles.avatar : styles.camera}>
        <Avatar size="large">{userName}</Avatar>
      </Col>
      <Row className={styles.devices}>
        <Col span={11}>
          <Camera onClick={operateCamera} />
        </Col>
        <Col span={1}>
          <Divider type="vertical" />
        </Col>
        <Col span={11}>
          <Mic onClick={operateMic} />
        </Col>
      </Row>
    </Row>
  );
};

export default memo(Preview);
