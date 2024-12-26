import { Col, Row } from 'dingtalk-design-desktop';
import { ReactNode, memo } from 'react';
import styles from './index.module.less';
import { useNetworkStats } from '~/hooks/channel';

const Statistic = (props: { title: string, items: Array<{ label: string, value: ReactNode }> }) => {
  const { title, items } = props;
  return (
    <Row className={styles.qualityItem}>
      <Row>{title}</Row>
      <Row>
        {items.map((item, idx) => (
          <Col span={8} key={`${item.label}${idx}`}>
            <Row className={styles.qualityLabel}>
              <Col span={8} >{item.label}</Col>
              <Col span={14}>{item.value}</Col>
            </Row>
          </Col>
        ))}
      </Row>
    </Row>
  )
}

const Metrics = () => {
  const { rtcStats } = useNetworkStats();

  const networkItems = [
    {
      label: '带宽',
      value: <span>&uarr; {rtcStats.localBitrate ? `${Math.round(rtcStats.localBitrate / 1000)} kbps` : '--'}</span>,
    },
    // {
    //   label: '丢包率',
    //   value: `${rtcStats.loss} %`,
    // },

    {
      label: '',
      value: <span>&darr; {rtcStats.remoteBitrate ? `${Math.round(rtcStats.remoteBitrate / 1000)} kbps` : '--'}</span>,
    },
    {
      label: '延迟',
      value: rtcStats.rtt ? `${rtcStats.rtt} ms` : '--',
    },

  ]
  const audioItems = [
    {
      label: '码率',
      value: <span>&uarr; {rtcStats.localAudioBitrate ? `${Math.round(rtcStats.localAudioBitrate / 1000)} kbps` : '--'}</span>,
    },
    {
      label: '',
      value: <span>&darr; {rtcStats.remoteAudioBitrate ? `${Math.round(rtcStats.remoteAudioBitrate / 1000)} kbps` : '--'}</span>,
    },
  ]
  const videoItems = [
    {
      label: '分辨率',
      value: <span>&uarr; {rtcStats.localCameraResolution ? `${rtcStats.localCameraResolution.width}*${rtcStats.localCameraResolution.height}` : '--'}</span>,
    },
    {
      label: '码率',
      value: <span>&uarr; {rtcStats.localCameraBitrate ? `${Math.round(rtcStats.localCameraBitrate / 1000)} kbps` : '--'}</span>,
    },
    {
      label: '帧率',
      value: <span>&uarr; {rtcStats.localCameraFPS ? `${rtcStats.localCameraFPS} fps` : '--'}</span>,
    },
    {
      label: '',
      value: <span>&darr; {rtcStats.remoteCameraResolution ? `${rtcStats.remoteCameraResolution.width}*${rtcStats.remoteCameraResolution.height}` : '--'}</span>,
    },
    {
      label: '',
      value: <span>&darr; {rtcStats.remoteCameraBitrate ? `${Math.round(rtcStats.remoteCameraBitrate / 1000)} kbps` : '--'}</span>,
    },
    {
      label: '',
      value: <span>&darr; {rtcStats.remoteCameraFPS ? `${rtcStats.remoteCameraFPS} fps` : '--'}</span>,
    },
  ]
  const screenItems = [
    {
      label: '分辨率',
      value: <span>&uarr; {rtcStats.localScreenResolution ? `${rtcStats.localScreenResolution.width}*${rtcStats.localScreenResolution.height}` : '--'}</span>,
    },
    {
      label: '码率',
      value: <span>&uarr; {rtcStats.localScreenBitrate ? `${Math.round(rtcStats.localScreenBitrate / 1000)} kbps` : '--'}</span>,
    },
    {
      label: '帧率',
      value: <span>&uarr; {rtcStats.localScreenFPS ? `${rtcStats.localScreenFPS} fps` : '--'}</span>,
    },
    {
      label: '',
      value: <span>&darr; {rtcStats.remoteScreenResolution ? `${rtcStats.remoteScreenResolution.width}*${rtcStats.remoteScreenResolution.height}` : '--'}</span>,
    },
    {
      label: '',
      value: <span>&darr; {rtcStats.remoteScreenBitrate ? `${Math.round(rtcStats.remoteScreenBitrate / 1000)} kbps` : '--'}</span>,
    },
    {
      label: '',
      value: <span>&darr; {rtcStats.remoteScreenFPS ? `${rtcStats.remoteScreenFPS} fps` : '--'}</span>,
    },
  ]

  return (
    <Row>
      <Statistic title='网络' items={networkItems} />
      <Statistic title='音频' items={audioItems} />
      <Statistic title='视频' items={videoItems} />
      <Statistic title='屏幕分享' items={screenItems} />
    </Row>
  )
};


export default memo(Metrics);
