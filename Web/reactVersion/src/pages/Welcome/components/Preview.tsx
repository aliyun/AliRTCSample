import { Row, Col, Divider, Avatar } from 'dingtalk-design-desktop';
import { memo, useEffect, useRef, useState } from 'react';
import styles from '../index.module.less';
import { Camera, Mic } from '~/components/Device';
import { useDevice } from '~/hooks/device';
import { useRecoilValue } from 'recoil';
import { currentUserInfo } from '~/store';
import { isIOS, isWeixin } from '~/utils/tools';
import PlayResumer from '~/components/PlayResumer';

const Preview = () => {
  const { cameraEnabled, openMicAndCameraSameTime, operateCamera, operateMic } = useDevice('pre');
  const { userName } = useRecoilValue(currentUserInfo);
  const ref = useRef(null);
  const [cameraTrack, setCameraTrack] = useState(null)

  useEffect(() => {
    openMicAndCameraSameTime().then(([track]) => {
      setCameraTrack(track);
    })
  }, []);

  useEffect(() => {
    if (cameraTrack && !(isIOS() && isWeixin())) {
      cameraTrack.play(ref.current, { fit: 'cover' });
    }
  }, [cameraTrack])

  return (
    <Row className={styles.previewWrap}>
      <PlayResumer track={cameraTrack} container={ref.current} />
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
