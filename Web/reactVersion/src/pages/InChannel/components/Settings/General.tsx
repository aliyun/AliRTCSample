import {
  Row,
  Form,
  Select,
  Checkbox,
} from 'dingtalk-design-desktop';
import { memo, useCallback, useMemo, useState } from 'react';
import { useLocalChannel, useRemoteChannel } from '~/hooks/channel';
import { isIOS } from '~/utils/tools';
  
  
const videoDimensions = [
  'VD_120x120',
  'VD_160x120',
  'VD_180x180',
  'VD_240x180',
  'VD_320x180',
  'VD_240x240',
  'VD_320x240',
  'VD_424x240',
  'VD_360x360',
  'VD_480x360',
  'VD_640x360',
  'VD_480x480',
  'VD_640x480',
  'VD_840x480',
  'VD_960x540',
  'VD_960x720',
  'VD_1280x720',
  'VD_1920x1080',
];
  
if (isIOS()) {
  videoDimensions.pop();
}
  
const RemoteStreamTypeOptions = [
  {
    label: '超清',
    value: 'FHD',
  },
  {
    label: '高清',
    value: 'HD',
  },
  {
    label: '标清',
    value: 'SD',
  },
  {
    label: '流畅',
    value: 'LD',
  },
]
  
  
const General = () => {
  const { micTrack, publishedTracks, cameraTrack, publish, unpublish, defaultRemoteStreamType, client } = useLocalChannel();
  const { mcuAudioTrack, subscribeMCUAudio, unsubscribeMCUAudio, subscribeAllVideo, subscribeAllRemoteVideo, unsubscribeAllRemoteVideo, subscribeAudio } = useRemoteChannel();
  const [loading, setLoading] = useState({
    camera: false,
    mic: false,
    remoteAudio: false,
    remoteVideo: false,
  })
  const isMicPublish = useMemo(() => publishedTracks.includes(micTrack), [micTrack, publishedTracks]);
  const isCameraPublish = useMemo(() => publishedTracks.includes(cameraTrack), [cameraTrack, publishedTracks]);
  const onFieldsChange = useCallback(
    (changedFields: any[]) => {
      const [{ name, value }] = changedFields;
      if (name[0] === 'videoPublish') {
        if (value && !isCameraPublish) {
          setLoading((prev) => ({ ...prev, camera: true }))
          publish([cameraTrack]).finally(() => {
            setLoading((prev) => ({ ...prev, camera: false }))
          });
        }
        if (!value && isCameraPublish) {
          setLoading((prev) => ({ ...prev, camera: true }))
          unpublish([cameraTrack]).finally(() => {
            setLoading((prev) => ({ ...prev, camera: false }))
          });
        }
      } else if (name[0] === 'audioPublish') {
        if (value && !isMicPublish) {
          setLoading((prev) => ({ ...prev, mic: true }))
          publish([micTrack]).finally(() => {
            setLoading((prev) => ({ ...prev, mic: false }))
          });;
        }
        if (!value && isMicPublish) {
          setLoading((prev) => ({ ...prev, mic: true }))
          unpublish([micTrack]).finally(() => {
            setLoading((prev) => ({ ...prev, mic: false }))
          });
        }
      } else if (name[0] === 'videoSubscribe') {
        if (value && !subscribeAllVideo) {
          setLoading((prev) => ({ ...prev, remoteVideo: true }))
          subscribeAllRemoteVideo().finally(() => {
            setLoading((prev) => ({ ...prev, remoteVideo: false }))
          });
        }
        if (!value && subscribeAllVideo) {
          setLoading((prev) => ({ ...prev, remoteVideo: true }))
          unsubscribeAllRemoteVideo().finally(() => {
            setLoading((prev) => ({ ...prev, remoteVideo: false }))
          });
        }
      } else if (name[0] === 'audioSubscribe') {
        if (value && !mcuAudioTrack) {
          setLoading((prev) => ({ ...prev, remoteAudio: true }))
          subscribeMCUAudio().finally(() => {
            setLoading((prev) => ({ ...prev, remoteAudio: false }))
          });
        }
        if (!value && mcuAudioTrack) {
          setLoading((prev) => ({ ...prev, remoteAudio: true }))
          unsubscribeMCUAudio().finally(() => {
            setLoading((prev) => ({ ...prev, remoteAudio: false }))
          });
        }
      } else if (name[0] === 'defaultRemoteStreamType') {
        client.setRemoteDefaultVideoStreamType(value)
      }
    },
    [mcuAudioTrack, isMicPublish, isCameraPublish, mcuAudioTrack, subscribeAllVideo, subscribeAllRemoteVideo, unsubscribeAllRemoteVideo, subscribeAudio],
  );
  return (
    <Row>
      <Form
        style={{ width: '100%' }}
        labelCol={{ span: 6 }}
        labelAlign="left"
        onFieldsChange={onFieldsChange}
        initialValues={{
          videoPublish: isCameraPublish,
          audioPublish: isMicPublish,
          videoSubscribe: subscribeAllVideo,
          defaultRemoteStreamType,
          audioSubscribe: !!mcuAudioTrack,
        }}
      >
        <Form.Item label="发布摄像头流" name="videoPublish" valuePropName='checked' help="开启则发布摄像头流，关闭则取消发布">
          <Checkbox disabled={loading.camera} />
        </Form.Item>
        <Form.Item label="发布麦克风流" name="audioPublish" valuePropName='checked' help="开启则发布麦克风流，关闭则取消发布">
          <Checkbox disabled={loading.mic} />
        </Form.Item>
        <Form.Item label="订阅远端视频" name="videoSubscribe" valuePropName='checked' help="开启则订阅全部视频，关闭则取消订阅">
          <Checkbox disabled={loading.remoteVideo} />
        </Form.Item>
        <Form.Item label="默认订阅档位" name="defaultRemoteStreamType" help="设置后对下次订阅生效">
          <Select options={RemoteStreamTypeOptions} />
        </Form.Item>
        <Form.Item label="订阅远端音频" name="audioSubscribe" valuePropName='checked' help="开启则订阅全部音频，关闭则取消订阅">
          <Checkbox disabled={loading.remoteAudio} />
        </Form.Item>
      </Form>
    </Row>
  )
}
  
export default memo(General);
  