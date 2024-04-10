import { getAppToken } from '~/utils/request';
import { client, currentUserInfo, globalFlag, localChannelInfo, remoteChannelInfo, smallViewTrackMap } from '~/store';
import { print } from '~/utils/tools';
import DingRTC, { RemoteAudioTrack, RemoteUser } from 'dingrtc';
import { Button, Card, Form, Input, Toast } from 'dingtalk-design-desktop';
import { useCallback, memo, useState } from 'react';
import { useRecoilState, useRecoilValue, useSetRecoilState } from 'recoil';
import styles from '../index.module.less';
import { useForm } from 'dingtalk-design-desktop/lib/form/Form';

const Join = () => {
  const setGlobalData = useSetRecoilState(globalFlag);
  const [{ userName, userId, channel, appId }, setCurrentUserInfo] = useRecoilState(currentUserInfo);
  const clientInfo = useRecoilValue(localChannelInfo);
  const setRemoteChannelInfo = useSetRecoilState(remoteChannelInfo);
  const setViewMap = useSetRecoilState(smallViewTrackMap);
  const setClient = useSetRecoilState(client);
  const setLocalChannelInfo = useSetRecoilState(localChannelInfo);
  const initialValues = {
    appId,
    userId,
    userName,
    channelName: channel,
  };

  const [joining, setJoining] = useState(false);

  const [form] = useForm<typeof initialValues>();
  const onJoin = useCallback(async () => {
    const { userId: uid, appId: app, channelName, userName: name } = await form.getFieldsValue();
    if (!uid || !app || !channelName || !name) {
      Toast.error('请检查参数填写');
      return;
    }
    print('localJoinChannel');
    setJoining(true);
    const newClient = DingRTC.createClient();
    setClient(newClient);
    try {
      const appTokenResult = await getAppToken(uid, app, channelName);
      const loginParam = {
        appId: app,
        userId: uid,
        userName,
        channelName,
        appToken: appTokenResult.token,
      };
      if (appTokenResult.gslb.length) {
        DingRTC.setClientConfig({ gslb: appTokenResult.gslb });
      }
      try {
        const result = await newClient.join({
          appId: loginParam.appId,
          token: loginParam.appToken,
          uid: loginParam.userId,
          channel: loginParam.channelName,
          userName: loginParam.userName,
        });
        setJoining(false);
        setCurrentUserInfo({
          userId: uid,
          appId: app,
          userName,
          channel: channelName,
        });
        Toast.success('加入房间成功');
        setGlobalData((pre) => ({ ...pre, joined: true }));
        setRemoteChannelInfo((prev) => ({ ...prev, remoteUsers: result.remoteUsers }));
        const subscribeUser = async (user: RemoteUser) => {
          const fn = (auxiliary: boolean) => newClient.subscribe(user.userId, 'video', auxiliary).then((track: any) => {
            print(`subscribe user ${user.userId} ${auxiliary ? 'screenShare' : 'camera'}}`);
            setViewMap((prev) => ({ ...prev, [user.userId]: prev[user.userId] || track }));
            setRemoteChannelInfo((prev) => ({ ...prev, remoteUsers: [...newClient.remoteUsers] }));
            return track;
          }).catch(e => {
            Toast.info(`subscribe user ${user.userId} ${auxiliary ? 'screenShare' : 'camera'} failed: ${JSON.stringify(e)}`);
            print(`subscribe user ${user.userId} ${auxiliary ? 'screenShare' : 'camera'} failed: ${JSON.stringify(e)}`);
            throw e;
          });
          if (user.hasVideo) {
            await fn(false);
          }
          if (user.hasAuxiliary) {
            await fn(true);
          }
        }        
        for (const user of result.remoteUsers) {
          subscribeUser(user)
        }
        const localTracks = [clientInfo.cameraTrack, clientInfo.micTrack].filter(item => !!item);
        if (clientInfo.cameraTrack) {
          clientInfo.cameraTrack.stop();
          setViewMap((prev) => ({ ...prev, [uid]: clientInfo.cameraTrack }));
        }
        if (localTracks.length) {
          newClient?.publish(localTracks).then(() => {
            setLocalChannelInfo((prev) => ({ ...prev, publishedTracks: [...newClient.localTracks] }));
            print(`publish ${localTracks.map((item) => item.trackMediaType)} tracks`);
          }).catch(e => {
            Toast.info(`publish ${localTracks.map((item) => item.trackMediaType)} tracks failed: ${JSON.stringify(e)}`);
            print(`publish ${localTracks.map((item) => item.trackMediaType)} tracks failed: ${JSON.stringify(e)}`);
            throw e;
          });
        }
        newClient.subscribe('mcu', 'audio').then((track) => {
          print(`subscribe mcu audio`);
          const audioTrack = track as RemoteAudioTrack;
          setRemoteChannelInfo((prev) => ({ ...prev, mcuAudioTrack: audioTrack }));
          setGlobalData((pre) => ({ ...pre, mcuAudioSubscribed: true }));
          audioTrack.play();
        });
      } catch (e: any) {
        setJoining(false);
        setGlobalData((pre) => ({ ...pre, joined: false }));
        Toast.error(`加入房间失败${e?.reason || e?.message || JSON.stringify(e)}`);
      }
    } catch (error) {
      setGlobalData((pre) => ({ ...pre, joined: false }));
      setJoining(false);
      Toast.error(`访问appServer失败${JSON.stringify(error)}`);
      print('error to join', error);
    }
  }, [form, clientInfo]);

  const onFormChange = useCallback((field: any) => {
    const [item] = field;
    if (item.name[0] !== 'userName') return
    setCurrentUserInfo((prev) => ({ ...prev, userName: item.value }));
  }, [])

  return (
    <Card className={styles.joinPanel} title="加入频道">
      <Form layout="vertical" form={form} initialValues={initialValues} onFieldsChange={onFormChange}>
        <Form.Item name="appId" required label="应用Id">
          <Input placeholder="请输入appId" />
        </Form.Item>
        <Form.Item name="userId" required label="用户Id">
          <Input placeholder="请输入userId" />
        </Form.Item>
        <Form.Item name="channelName" required label="频道名">
          <Input placeholder="请输入channelName" />
        </Form.Item>
        <Form.Item name="userName" required label="用户名">
          <Input placeholder="请输入userName" />
        </Form.Item>
        <Form.Item>
          <Button htmlType="submit" size="large" type="primary" disabled={joining} onClick={onJoin}>
            加入
          </Button>
        </Form.Item>
      </Form>
    </Card>
  );
};

export default memo(Join);
