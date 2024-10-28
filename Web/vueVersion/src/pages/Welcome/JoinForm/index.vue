<script setup lang="ts">
import { Button, Form, Input } from 'ant-design-vue';
import { message as Toast } from 'ant-design-vue';
import DingRTC from 'dingrtc';
import { ref } from 'vue';

import { isInRoom } from '@src/Router';
import { useClient } from '@src/store/client';
import { useCurrentUserInfo } from '@src/store/currentUserInfo';
import { useLocalChannelInfoStore } from '@src/store/localChannelInfo';
import { useMainViewStore } from '@src/store/mainView';
import { getAppToken, print } from '@src/utils/tools';

import { rules } from './constant';
import { disableSinglePC, useInitPcMode } from './initPcMode';
import { useJoinMethod } from './join';

const joining = ref(false);
const formState = useCurrentUserInfo();
const localChannelInfoStore = useLocalChannelInfoStore();
const client = useClient();
useInitPcMode();
const mainViewStore = useMainViewStore();
const { publishLocalTrack, subscribeAudio, subscribeRemoteUsers, subscribeInOnePc } =
  useJoinMethod();

const onFinish = async () => {
  joining.value = true;
  try {
    const appTokenResult = await getAppToken(formState.userId, formState.appId, formState.channel);
    if (appTokenResult.gslb?.length) {
      DingRTC.setClientConfig({ gslb: appTokenResult.gslb });
    }

    try {
      const result = await client.join({
        appId: formState.appId,
        token: appTokenResult.token,
        uid: formState.userId,
        channel: formState.channel,
        userName: formState.userName,
      });
      // eslint-disable-next-line @typescript-eslint/ban-ts-comment
      // @ts-ignore
      localChannelInfoStore.$patch({ timeLeft: result.timeLeft });

      if (disableSinglePC) {
        subscribeRemoteUsers(result.remoteUsers);
        subscribeAudio();
      } else {
        subscribeInOnePc(result.remoteUsers);
      }

      publishLocalTrack();
      joining.value = false;
      Toast.success('加入房间成功');
      mainViewStore.$patch({
        userId: formState.userId,
      });
      isInRoom.value = true;
    } catch (e: unknown) {
      await client.leave();
      joining.value = false;
      if (e instanceof Error) {
        isInRoom.value = false;
        Toast.error(`加入房间失败${e?.message || JSON.stringify(e)}`);
      }
    }
  } catch (error) {
    joining.value = false;
    isInRoom.value = false;
    Toast.error(`访问appServer失败${JSON.stringify(error)}`);
    print('error to join', error);
  }
};
</script>

<template>
  <div class="Join-Form-Wrapper">
    <Form
      layout="vertical"
      :model="formState"
      name="basic"
      autocomplete="off"
      @finish="onFinish"
      :rules="rules"
    >
      <Form.Item label="应用Id" name="appId">
        <Input v-model:value="formState.appId" />
      </Form.Item>
      <Form.Item label="用户Id" name="userId">
        <Input v-model:value="formState.userId" />
      </Form.Item>
      <Form.Item label="channel" name="channel">
        <Input v-model:value="formState.channel" />
      </Form.Item>
      <Form.Item label="用户名" name="userName">
        <Input v-model:value="formState.userName" />
      </Form.Item>
      <Form.Item>
        <Button type="primary" html-type="submit" :loading="joining" :style="{ width: '100%' }"
          >join</Button
        >
      </Form.Item>
    </Form>
  </div>
</template>

<style lang="less">
.Join-Form-Wrapper {
  background-color: #fff;
  width: 400px;
  box-shadow: 0 0 80px 0 rgba(126, 153, 178, 0.3);
  height: 500px;
  overflow: hidden;
  border-radius: 12px;
  display: flex;
  justify-content: center;
  align-items: center;
  .ant-form-item {
    margin-bottom: 8px;
  }
}
@media (max-width: 768px) {
  .Join-Form-Wrapper {
    flex: 1;
    height: auto;
  }
}
</style>
