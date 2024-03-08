import { memo, useCallback, useEffect, useMemo, useRef, useState } from 'react';
import RtcEngine, {
  CameraVideoTrack,
  DingRTCClient,
  LocalTrack,
  LocalVideoTrack,
  MicrophoneAudioTrack,
  RemoteAudioTrack,
  RemoteUser,
  SimpleLogger,
  loggerService,
  RemoteStreamType,
  TrackMediaType,
} from 'dingrtc';
import { getAppId, getAppToken } from '../utils/getToken';
import VConsole from 'vconsole';
import {
  Button,
  Input,
  Form,
  Space,
  Row,
  message,
  Col,
  Select,
  Typography,
  Modal,
} from 'dingtalk-design-desktop';
import UserItem from './UserItem';
import { isIOS, isMobile, isWeixin } from '../utils/tools';

const { Title, Text } = Typography;

const { Item } = Form;

interface DeviceInfo {
  label: string;
  value: string;
}

type DeviceType = 'camera' | 'playback-device' | 'microphone';

loggerService.injectLogSender((logs: string[] = []) => {
  const outputArea = document.getElementById('logOutput');
  if (outputArea && logs.length) {
    for (const log of logs) {
      if (!log.includes('DEMOAPP')) {
        continue;
      }
      outputArea.innerText += `${JSON.parse(log)}\r\n`;
    }
  }
  return Promise.resolve({ success: true });
});

setInterval(() => {
  loggerService.flushLog();
}, 500);

const logger = SimpleLogger.getLogger('DEMOAPP');
logger.info('ua', navigator.userAgent);
const clearVideo = (id: string) => {
  const otherVideoDom = document.getElementById(id)?.querySelector('video');
  if (otherVideoDom) document.getElementById(id)?.removeChild(otherVideoDom);
};

interface UserStatus {
  subscribed?: boolean;
  cameraEnabled?: boolean;
  micEnabled?: boolean;
}

interface UserStatusMap {
  [userId: string]: UserStatus;
}

logger.info('start demo app');

export const UserBlock = memo((props: { label: string; channelName: string }) => {
  const [rtcClient, setRtcClient] = useState<DingRTCClient>();
  const [joined, setJoined] = useState(false);
  const [joining, setJoining] = useState(false);
  const [openingCamera, setOpeningCamera] = useState(false);
  const [userStatus, setUserStatus] = useState<UserStatusMap>({});
  const [appId, setAppId] = useState(getAppId());
  const [userName, setUserName] = useState(`${Math.ceil(Math.random() * 10000)}`);
  const [mcuAudioSubscribe, setMcuAudioSubscribed] = useState(false);
  const [loading, setLoading] = useState(false);
  const [mainVideoUserId, setMainVideoUserId] = useState('');
  const [otherUsers, setOtherUsers] = useState<RemoteUser[]>([]);
  const [cameraTrack, setCameraTrack] = useState<CameraVideoTrack>();
  const [screenCastVideoTrack, setScreenCastVideoTrack] = useState<LocalVideoTrack>();
  const [micTrack, setMicTrack] = useState<MicrophoneAudioTrack>();
  const [remoteAudioTrack, setRemoteAudioTrack] = useState<RemoteAudioTrack>(null);
  const [channelName, setChannelName] = useState(props.channelName);
  const [userId, setUserId] = useState(`${Math.ceil(Math.random() * 10000)}`);
  const otherVideosWrapperRef = useRef<HTMLDivElement>(null);
  const [facingMode] = useState<'user' | 'environment'>('user');
  const [micList, setMicList] = useState<DeviceInfo[]>([]);
  const [speakerList, setSpeakerList] = useState<DeviceInfo[]>([]);
  const [cameraList, setCamearList] = useState<DeviceInfo[]>([]);
  const [currentDevice, setCurrentDevice] = useState({
    mic: '',
    speaker: '',
    camera: '',
  });

  const changeDevice = useCallback((device: string, deviceId: string) => {
    setCurrentDevice((data) => ({
      ...data,
      [device]: deviceId,
    }));
  }, []);

  const localTracks = useMemo(() => {
    const result: LocalTrack[] = [];
    cameraTrack && result.push(cameraTrack);
    micTrack && result.push(micTrack);
    screenCastVideoTrack && result.push(screenCastVideoTrack);
    return result;
  }, [cameraTrack, screenCastVideoTrack, micTrack]);

  const viewBigger = (user: RemoteUser) => {
    if (user.userId === userId && screenCastVideoTrack) {
      if (screenCastVideoTrack.isPlaying) {
        screenCastVideoTrack.stop();
      } else {
        screenCastVideoTrack.play('#mainVideo');
      }
      return;
    }
    if (user.hasAuxiliary) {
      if (user.auxiliaryTrack) {
        user.auxiliaryTrack.stop();
        rtcClient.unsubscribe(user.userId, 'video', true).then(() => {
          logger.info(`unsubscribe user ${user.userId} screenShare`);
          setOtherUsers([...rtcClient.remoteUsers]);
        });
      } else {
        rtcClient.subscribe(user.userId, 'video', true).then((track) => {
          logger.info(`subscribe user ${user.userId} screenShare`);
          setMainVideoUserId(`${user.userId}_screenCast`);
          track.play('#mainVideo');
        });
      }
    }
  };

  const updateStatus = useCallback((uid: string, newData: UserStatus) => {
    setUserStatus((data: UserStatusMap) => ({
      ...data,
      [uid]: {
        ...data[uid],
        ...newData,
      },
    }));
  }, []);

  const updateDeviceList = (deviceType: DeviceType, info: any) => {
    const fnMap: Record<DeviceType, any> = {
      camera: setCamearList,
      'playback-device': setSpeakerList,
      microphone: setMicList,
    };
    const fn = fnMap[deviceType];
    logger.info(`${deviceType} ${info.state}`, info.device.label);
    fn((prev: DeviceInfo[]) => {
      const index = prev.findIndex((item) => item.value === info.device.deviceId);
      if (info.state === 'active' && index === -1) {
        return [...prev, { value: info.device.deviceId, label: info.device.label }];
      } else if (info.state === 'inactive') {
        prev.splice(index, 1);
        return [...prev];
      }
    });
  };

  useEffect(() => {
    RtcEngine.getSupportedCodec().then((result) => {
      console.log(result);
    })
    // eslint-disable-next-line no-new
    new VConsole({});
    const supported = RtcEngine.checkSystemRequirements();
    if (!supported) {
      message.error('当前浏览器不支持通话');
    }

    RtcEngine.getCameras().then((list) => {
      setCamearList(list.map((item) => ({ value: item.deviceId, label: item.label })));
      changeDevice('camera', list?.[0]?.deviceId);
    });
    RtcEngine.on('camera-changed', (info) => {
      updateDeviceList('camera', info);
    });
    RtcEngine.on('microphone-changed', (info) => {
      updateDeviceList('microphone', info);
    });
    RtcEngine.getMicrophones().then((list) => {
      setMicList(list.map((item) => ({ value: item.deviceId, label: item.label })));
      changeDevice('mic', list?.[0]?.deviceId);
    });
    RtcEngine.on('playback-device-changed', (info) => {
      updateDeviceList('playback-device', info);
    });
    RtcEngine.getPlaybackDevices().then((list) => {
      setSpeakerList(list.map((item) => ({ value: item.deviceId, label: item.label })));
      changeDevice('speaker', list?.[0]?.deviceId);
    });
    RtcEngine.on('autoplay-failed', (track) => {
      if (isIOS() && isWeixin()) {
        // @ts-ignore
        const wxBridge: any = window.WeixinJSBridge;
        if (wxBridge) {
          wxBridge.invoke('getNetworkType', {}, () => {
            logger.info('autoTriggerPlay');
            track.play();
          });
        } else {
          document.addEventListener(
            'WeixinJSBridgeReady',
            () => {
              wxBridge.invoke('getNetworkType', {}, () => {
                logger.info('autoTriggerPlay');
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
      RtcEngine.removeAllListeners();
    };
  }, []);

  const createCamTrack = () => {
    if (cameraTrack) return;
    setOpeningCamera(true);
    return RtcEngine.createCameraVideoTrack({
      frameRate: 15,
      facingMode: isIOS() ? facingMode : undefined,
      deviceId: currentDevice.camera || undefined,
    })
      .then((track) => {
        logger.info('got cameraTrack');
        setCameraTrack(track);
        setOpeningCamera(false);
        if (joined) {
          rtcClient.publish(track);
          logger.info(`publish camera`);
        }
        updateStatus(userId, { cameraEnabled: true });
        track.play(`#uid${userId}`, {
          mirror: true,
          fit: 'cover',
        });
      })
      .catch(() => {
        setOpeningCamera(false);
      });
  };
  const createMicTrack = () => {
    if (micTrack) return;
    RtcEngine.createMicrophoneAudioTrack({
      deviceId: currentDevice.mic || undefined,
    }).then((track) => {
      setMicTrack(track);
      logger.info('got MicTrack');
      updateStatus(userId, { micEnabled: true });
      if (joined) {
        rtcClient.publish(track);
      }
    });
  };

  useEffect(() => {
    if (!remoteAudioTrack || isMobile()) return;
    remoteAudioTrack.setSpeaker(currentDevice.speaker || undefined);
  }, [remoteAudioTrack, currentDevice.speaker]);

  const localJoinChannel = async () => {
    logger.info('localJoinChannel');
    const client = RtcEngine.createClient();
    setJoining(true);
    setRtcClient(client);
    try {
      const appTokenResult = await getAppToken(userId, appId, channelName);
      const loginParam = {
        appId,
        userId,
        userName,
        channelName,
        appToken: appTokenResult.token,
      };
      try {
        const result = await client.join({
          appId: loginParam.appId,
          token: loginParam.appToken,
          uid: loginParam.userId,
          channel: loginParam.channelName,
          userName: loginParam.userName,
        });
        setJoining(false);
        message.success('加入房间成功');
        setJoined(true);
        setOtherUsers([...result.remoteUsers]);
        for (const user of result.remoteUsers) {
          updateStatus(user.userId, { micEnabled: !user.audioMuted });
          if (user.hasVideo) {
            client.subscribe(user.userId, 'video').then((track) => {
              logger.info(`subscribe user ${user.userId} video`);
              updateStatus(user.userId, { cameraEnabled: true, subscribed: true });
              track.play(`#uid${user.userId}`);
            });
          }
        }
        setLoading(true);
        client.subscribe('mcu', 'audio').then((track) => {
          logger.info(`subscribe mcu audio`);
          const audioTrack = track as RemoteAudioTrack;
          audioTrack.play();
          setMcuAudioSubscribed(true);
          setLoading(false);
          setRemoteAudioTrack(audioTrack);
        });
      } catch (e: any) {
        setJoining(false);
        setJoined(false);
        message.error(`加入房间失败${e?.reason || e}`);
      }
    } catch (error) {
      setJoined(false);
      setJoining(false);
      message.error(`访问appServer失败${JSON.stringify(error)}`);
      logger.error('error to join', error);
    }
  };

  const clearRoom = () => {
    otherUsers.forEach((user) => {
      if (user.auxiliaryTrack) {
        user.auxiliaryTrack.stop();
      }
    });
    setOtherUsers([]);
    setJoined(false);
    setLoading(false);
    setMcuAudioSubscribed(false);
    if (cameraTrack) {
      cameraTrack.stop();
      cameraTrack.close();
      setCameraTrack(null);
    }
    if (micTrack) {
      micTrack.close();
      setMicTrack(null);
    }
    if (screenCastVideoTrack) {
      screenCastVideoTrack.close();
      setScreenCastVideoTrack(null);
    }
    if (remoteAudioTrack) {
      remoteAudioTrack.stop();
      setRemoteAudioTrack(null);
    }
    setUserStatus({});
    setRtcClient(null);
  };

  useEffect(() => {
    if (!rtcClient) return;
    rtcClient.on('user-joined', (user: RemoteUser) => {
      logger.info(`user ${user.userId} joined`);
      setOtherUsers([...rtcClient.remoteUsers]);
    });
    rtcClient.on('network-quality', (uplink, downlink) => {
      console.log(`uplink: ${uplink}, downlink: ${downlink}`);
    });
  
    rtcClient.on('stream-type-changed', (uid: string, streamType: RemoteStreamType) => {
      logger.info(`user ${uid} streamType changeTo ${streamType}`);
    });

    rtcClient.on('connection-state-change', (current, _, reason) => {
      logger.info(`connection-state-change ${current} ${reason || ''}`);
      if (current === 'disconnected') {
        message.info(reason);
        clearRoom();
      }
    });
    rtcClient.on('user-published', (user, mediaType, auxiliary) => {
      logger.info(`user ${user.userId} published ${auxiliary ? 'screenShare' : mediaType}}`);
      setOtherUsers([...rtcClient.remoteUsers]);
      if (mediaType !== 'video') {
        updateStatus(user.userId, { micEnabled: true });
        return;
      }
      if (!auxiliary) {
        rtcClient.subscribe(user.userId, mediaType, auxiliary).then((track) => {
          logger.info(`subscribe user ${user.userId} ${auxiliary ? 'screenShare' : mediaType}}`);
          updateStatus(user.userId, { subscribed: true, cameraEnabled: true });
          track.play(`#uid${user.userId}`);
        });
      }
    });
    rtcClient.on(
      'user-unpublished',
      (user: RemoteUser, mediaType: TrackMediaType, auxiliary: boolean) => {
        logger.info(`user ${user.userId} unpublished ${auxiliary ? 'screenShare' : mediaType}}`);
        const track = auxiliary ? user.auxiliaryTrack : user.videoTrack;
        setOtherUsers([...rtcClient.remoteUsers]);
        if (mediaType !== 'video') {
          updateStatus(user.userId, { micEnabled: false });
          return;
        }
        if (track) {
          track.stop();
          rtcClient.unsubscribe(user.userId, mediaType, auxiliary).then(() => {
            logger.info(
              `unsubscribe user ${user.userId} ${auxiliary ? 'screenShare' : mediaType}}`,
            );
            updateStatus(user.userId, { subscribed: false, cameraEnabled: false });
          });
        }
      },
    );
    rtcClient.on('volume-indicator', (uids: string[]) => {
      if (!uids.length) return;
      logger.info(`${uids.join()} is speaking`);
      const divs = [...otherVideosWrapperRef.current.querySelectorAll('div')];
      for (const dom of divs) {
        if (uids.includes(dom.id)) {
          dom.style.borderColor = 'green';
          dom.style.borderWidth = '3px';
        } else {
          dom.style.borderColor = 'gray';
          dom.style.borderWidth = '1px';
        }
      }
    });
    rtcClient.on('user-info-updated', (uid, msg) => {
      logger.info(`user ${uid}: ${msg}`);
      const [action, mediaType] = msg.split('-');
      const type = mediaType === 'audio' ? 'micEnabled' : 'cameraEnabled';
      updateStatus(uid, { [type]: action === 'unmute' });
    });
    rtcClient.on('user-left', (user: RemoteUser) => {
      logger.info(`user ${user.userId} left`);
      if (userStatus?.[user.userId]?.subscribed) {
        logger.info(`unsubscribe user ${user.userId} camera`);
        updateStatus(user.userId, { subscribed: false });
      }
      if (mainVideoUserId === `${user.userId}_screenCast`) {
        clearVideo('mainVideo');
      }
      setOtherUsers([...rtcClient.remoteUsers]);
    });
    return () => {
      rtcClient.removeAllListeners();
    };
  }, [rtcClient]);

  useEffect(() => {
    if (!screenCastVideoTrack) return;
    const clearTrack = () => {
      screenCastVideoTrack.stop();
      setScreenCastVideoTrack(null);
    };
    screenCastVideoTrack.on('track-ended', clearTrack);
    return () => {
      screenCastVideoTrack.off('track-ended', clearTrack);
    };
  }, [screenCastVideoTrack, mainVideoUserId]);

  const localShareScreen = async () => {
    const [videoTrack] = await RtcEngine.createScreenVideoTrack({});
    if (joined) {
      rtcClient.publish(videoTrack);
    }
    setScreenCastVideoTrack(videoTrack as LocalVideoTrack);
  };

  const changeMicDevice = (deviceId: string) => {
    changeDevice('mic', deviceId);
    logger.info(`mic changeTo ${deviceId}`);
    if (micTrack) {
      micTrack.setDevice(deviceId);
    }
  };

  const changeCameraDevice = (deviceId: string) => {
    changeDevice('camera', deviceId);
    logger.info(`camera changeTo ${deviceId}`);
    if (cameraTrack) {
      cameraTrack.setDevice(deviceId);
    }
  };

  const changeSpeakerDevice = (deviceId: string) => {
    changeDevice('speaker', deviceId);
    logger.info(`speaker changeTo ${deviceId}`);
    if (remoteAudioTrack) {
      remoteAudioTrack.setSpeaker(deviceId);
    }
  };

  return (
    <Space direction="vertical" className="block-wrapper">
      <div style={{ fontSize: 16, height: 20 }}>
        <Title level={5}>web 端 {props.label}</Title>
        {joined ? <Text type="secondary" className="shareTip">有共享Icon时可双击用户小窗口，在主窗口展示订阅的该用户的共享流，再次双击取消订阅</Text> : null}
      </div>
      <Form layout="inline">
        <Title level={6}>频道相关: &nbsp;&nbsp;&nbsp;&nbsp;</Title>
        <Item label="appId">
          <Input
            style={{ width: 90 }}
            value={appId}
            disabled={joined}
            onChange={(e) => setAppId(e.target.value)}
          />
        </Item>
        <Item label="用户ID">
          <Input
            disabled={joined}
            style={{ width: 80 }}
            value={userId}
            onChange={(e) => setUserId(e.target.value)}
          />
        </Item>
        <Item label="用户名称">
          <Input
            disabled={joined}
            style={{ width: 80 }}
            value={userName}
            onChange={(e) => setUserName(e.target.value)}
          />
        </Item>
        <Item label="房间号">
          <Input
            disabled={joined}
            style={{ width: 80 }}
            value={channelName}
            onChange={(e) => setChannelName(e.target.value)}
          />
        </Item>
        <Item>
          <Button disabled={joined || joining} type="primary" onClick={localJoinChannel}>
            加入房间
          </Button>
        </Item>
        <Item>
          <Button
            disabled={!joined}
            onClick={() => {
              rtcClient?.leave();
              clearRoom();
            }}
          >
            离开房间
          </Button>
        </Item>
      </Form>
      <Space>
        <Form layout="inline">
          <Title level={6}>{joined ? '基础操作' : '本地预览'}: &nbsp;&nbsp;&nbsp;&nbsp;</Title>
          <Item>
            <Button
              onClick={() => {
                if (!micTrack) {
                  createMicTrack();
                } else {
                  micTrack?.setMuted(userStatus?.[userId]?.micEnabled).then(() => {
                    updateStatus(userId, {
                      micEnabled: !userStatus?.[userId]?.micEnabled,
                    });
                  });
                }
              }}
            >
              {userStatus?.[userId]?.micEnabled && micTrack ? '关闭麦克风' : '开启麦克风'}
            </Button>
          </Item>
          <Item>
            <Button
              disabled={openingCamera}
              onClick={() => {
                if (!cameraTrack) {
                  createCamTrack();
                } else {
                  cameraTrack?.setMuted(userStatus?.[userId]?.cameraEnabled).then(() => {
                    updateStatus(userId, {
                      cameraEnabled: !userStatus?.[userId]?.cameraEnabled,
                    });
                  });
                }
              }}
            >
              {userStatus?.[userId]?.cameraEnabled && cameraTrack ? '关闭摄像头' : '开启摄像头'}
            </Button>
          </Item>

          {joined ? (
            <>
              {isMobile() ? null : (
                <Item>
                  <Button
                    onClick={() => {
                      if (screenCastVideoTrack) {
                        screenCastVideoTrack.close();
                        setScreenCastVideoTrack(null);
                      } else {
                        localShareScreen();
                      }
                    }}
                  >
                    {screenCastVideoTrack ? '结束共享' : '开启共享'}
                  </Button>
                </Item>
              )}
              <Item>
                <Button
                  disabled={!joined || !localTracks.length}
                  onClick={() => {
                    rtcClient?.publish(localTracks);
                  }}
                >
                  发布
                </Button>
              </Item>
              <Item>
                <Button
                  disabled={!joined || !localTracks.length}
                  onClick={() => {
                    rtcClient?.unpublish();
                  }}
                >
                  取消发布
                </Button>
              </Item>
              {!mcuAudioSubscribe ? (
                <Item>
                  <Button
                    disabled={loading}
                    onClick={() => {
                      setLoading(true);
                      rtcClient.subscribe('mcu', 'audio').then((audioTrack) => {
                        audioTrack.play();
                        setRemoteAudioTrack(audioTrack as RemoteAudioTrack);
                        setLoading(false);
                        setMcuAudioSubscribed(true);
                        logger.info(`subscribe mcu audio`);
                      });
                    }}
                  >
                    订阅全局音频
                  </Button>
                </Item>
              ) : (
                <Item>
                  <Button
                    disabled={loading}
                    onClick={() => {
                      setLoading(true);
                      rtcClient.unsubscribe('mcu', 'audio').then(() => {
                        remoteAudioTrack.stop();
                        setRemoteAudioTrack(null);
                        setMcuAudioSubscribed(false);
                        logger.info(`unsubscribe mcu audio`);
                        setLoading(false);
                      });
                    }}
                  >
                    取消订阅全局音频
                  </Button>
                </Item>
              )}
            </>
          ) : null}
        </Form>
      </Space>
      <Row ref={otherVideosWrapperRef} className="video-wrapper">
        <Row className="small-video-items">
          {[
            {
              userId,
              videoTrack: cameraTrack,
              userName,
              hasAuxiliary: !!screenCastVideoTrack,
              auxiliaryTrack: screenCastVideoTrack,
            },
            ...otherUsers,
          ].map((user) => (
            <UserItem
              isLocal={user.userId === userId}
              user={user as RemoteUser}
              key={user.userId}
              updateStatus={updateStatus}
              status={userStatus[user.userId]}
              client={rtcClient}
              onDoubleClick={() => viewBigger(user as RemoteUser)}
            />
          ))}
        </Row>
        <Col id="mainVideo" />
      </Row>
      <Row>
        <Form layout="inline">
          <Title level={6}>设备列表: &nbsp;&nbsp;&nbsp;&nbsp;</Title>
          {isMobile() ? null : (
            <Item label="扬声器设备">
              <Select
                style={{ width: 150 }}
                value={currentDevice.speaker}
                options={speakerList}
                onChange={(value) => changeSpeakerDevice(value)}
              />
            </Item>
          )}
          <Item label="麦克风设备">
            <Select
              style={{ width: 150 }}
              value={currentDevice.mic}
              onChange={(value) => changeMicDevice(value)}
              options={micList}
            />
          </Item>
          <Item label="摄像头设备">
            <Select
              style={{ width: 150 }}
              value={currentDevice.camera}
              options={cameraList}
              onChange={(value) => changeCameraDevice(value)}
            />
          </Item>
        </Form>
      </Row>
      <Row>
        <Title level={6}>操作日志: &nbsp;&nbsp;&nbsp;&nbsp;</Title>
        <div id="logOutput" />
      </Row>
    </Space>
  );
});
