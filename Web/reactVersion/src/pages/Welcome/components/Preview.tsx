import { Row, Col, Divider, Avatar, Modal } from 'dingtalk-design-desktop';
import { memo, useEffect, useRef } from 'react';
import styles from '../index.module.less';
import { Camera, Mic } from '~/components/Device';
import { useDevice } from '~/hooks/device';
import { useRecoilValue } from 'recoil';
import { currentUserInfo } from '~/store';
import { isIOS, isWeixin } from '~/utils/tools';

const Preview = () => {
  const { cameraEnabled, openMicAndCameraSameTime, operateCamera, operateMic } = useDevice('pre');
  const { userName } = useRecoilValue(currentUserInfo);
  const ref = useRef(null);

  useEffect(() => {
    openMicAndCameraSameTime().then(([track]) => {
      if (isIOS() && isWeixin()) {
        Modal.centerConfirm({
          title: '提示',
          content: 'iOS微信浏览器需要在回调内播放',
          okText: '确认',
          cancelText: '取消',
          onOk() {
            track.play(ref.current, { fit: 'cover' });
          },
          onCancel() {
            track.play(ref.current, { fit: 'cover' });
          }
        })
      } else {
        track.play(ref.current, { fit: 'cover' });
      }
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
