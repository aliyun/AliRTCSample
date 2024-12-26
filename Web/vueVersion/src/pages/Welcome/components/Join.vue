<template>
  <Card class="joinPanel" title="加入频道">
    <Form layout="vertical" :model="formData">
      <FormItem label="应用Id" name="appId" required>
        <Input v-model:value="formData.appId" placeholder="请输入appId" />
      </FormItem>
      <FormItem label="用户Id" name="userId" required>
        <Input v-model:value="formData.userId" placeholder="请输入uid" />
      </FormItem>
      <FormItem label="频道Id" name="channelName" required>
        <Input v-model:value="formData.channelName" placeholder="请输入channelId" />
      </FormItem>
      <FormItem label="用户名" name="userName" required>
        <Input v-model:value="formData.userName" placeholder="请输入userName" />
      </FormItem>
      <FormItem class="joinButton">
        <Button type="primary" size="large" :loading="joining" @click="onJoin"> 加入 </Button>
      </FormItem>
    </Form>
  </Card>
</template>

<script setup lang="ts">
import { ref, reactive } from 'vue';
import { message, Form, Input, Button, Card, FormItem } from 'ant-design-vue';
import { getAppToken } from '~/utils/request';
import { print } from '~/utils/tools';
import {
  CameraVideoTrack,
  LocalTrack,
  MicrophoneAudioTrack,
  RemoteAudioTrack,
  SubscribeParam,
} from 'dingrtc';
import { useClient, useGlobalFlag, useChannelInfo, useCurrentUserInfo } from '~/store';

const joining = ref(false);


const channelInfo = useChannelInfo();
const globalFlag = useGlobalFlag();

const currentUserInfo = useCurrentUserInfo();

const formData = reactive({
  appId: currentUserInfo.appId,
  userId: currentUserInfo.userId,
  channelName: currentUserInfo.channel,
  userName: currentUserInfo.userName,
});

const client = useClient();

const onJoin = async () => {
  joining.value = true;

  const {
    userId: uid,
    appId: app,
    channelName,
    userName: name,
    // duration: newDuration,
    // delay: newDelay,
    // gslb,
    // token,
  } = formData;

  if (!uid || !app || !channelName || !name) {
    message.error('请检查参数填写');
    return;
  }
  print('localJoinChannel');
  joining.value = true;
  try {
    const appTokenResult = await getAppToken(uid, app, channelName);
    const loginParam = {
      appId: app,
      userId: uid,
      userName: name,
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
      joining.value = false;
      currentUserInfo.$patch({
        userId: uid,
        appId: app,
        userName: name,
        channel: channelName,
      });
      channelInfo.$patch({ timeLeft: result.timeLeft });
      message.success('加入房间成功');
      channelInfo.$patch({ remoteUsers: result.remoteUsers });
      const subParams: SubscribeParam[] = [{ uid: 'mcu', mediaType: 'audio', auxiliary: false }];
      for (const user of result.remoteUsers) {
        if (user.hasAuxiliary) {
          subParams.push({ uid: user.userId, mediaType: 'video', auxiliary: true });
        }
        if (user.hasVideo) {
          subParams.push({ uid: user.userId, mediaType: 'video', auxiliary: false });
        }
      }
      const tasks = [];
      const subTask = client.batchSubscribe(subParams).then((batchSubscribeResult) => {
        for (const { error, track, uid: usrId, auxiliary } of batchSubscribeResult) {
          if (error) {
            message.info(
              `subscribe user ${usrId} ${auxiliary ? 'screenShare' : 'camera'} failed: ${JSON.stringify(error)}`,
            );
            print(
              `subscribe user ${usrId} ${auxiliary ? 'screenShare' : 'camera'} failed: ${JSON.stringify(error)}`,
            );
            continue;
          }
          if (track.trackMediaType === 'audio') {
            print(`subscribe ${usrId} audio`);
            const audioTrack = track as RemoteAudioTrack;
            channelInfo.$patch({ mcuAudioTrack: audioTrack, subscribeAudio: 'mcu' });
            audioTrack.play();
          } else {
            if (!channelInfo.mainViewUserId) {
              channelInfo.$patch({
                mainViewUserId: usrId,
                mainViewPreferType: auxiliary ? 'auxiliary' : 'camera',
              });
            }
            print(`subscribe user ${usrId} ${auxiliary ? 'screenShare' : 'camera'}`);
            channelInfo.$patch({ remoteUsers: [...client.remoteUsers] });
          }
        }
        if (channelInfo.cameraTrack && !channelInfo.mainViewUserId) {
          channelInfo.$patch({
            mainViewUserId: uid,
            mainViewPreferType: 'camera',
          });
        }
      });
      tasks.push(subTask);
      const localTracks: LocalTrack[] = [
        channelInfo.cameraTrack as CameraVideoTrack,
        channelInfo.micTrack as MicrophoneAudioTrack,
      ].filter((item) => !!item);
      if (localTracks.length) {
        channelInfo.updateTrackStats(uid);
        const pubTask = client
          .publish(localTracks)
          .then(() => {
            channelInfo.updatePublishedTracks(localTracks.map(item => item.getTrackId()), 'add')
            print(`publish ${localTracks.map((item) => item.trackMediaType)} tracks`);
          })
          .catch((e) => {
            message.info(
              `publish ${localTracks.map((item) => item.trackMediaType)} tracks failed: ${JSON.stringify(e)}`,
            );
            print(
              `publish ${localTracks.map((item) => item.trackMediaType)} tracks failed: ${JSON.stringify(e)}`,
            );
            throw e;
          });
        tasks.push(pubTask);
      }
      Promise.all(tasks).finally(() => {
        subParams.forEach(({ mediaType, uid }) => {
          if (mediaType === 'video') {
            channelInfo.updateTrackStats(uid)
          }
        })
        globalFlag.$patch({ joined: true });
      });
    } catch (e: any) {
      joining.value = false;
      globalFlag.$patch({ joined: false });
      message.error(`加入房间失败${e?.reason || e?.message || JSON.stringify(e)}`);
    }
  } catch (error) {
    globalFlag.$patch({ joined: false });
    joining.value = false;
    message.error(`访问appServer失败${JSON.stringify(error)}`);
    print('error to join', error);
  }
};
</script>

<style lang="less" scoped>
@import url('../index.module.less');
</style>
