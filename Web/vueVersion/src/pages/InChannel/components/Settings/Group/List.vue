<template>
  <Row class="groupContainer">
    <Row class="groupContainerHeader">
      <Button type="primary" @click="openModal">
        加入分组
      </Button>
      <Button :disabled="channelInfo.groups.some((item) => item.joined)" @click="mixToChannel">
        混音到大厅
      </Button>
      <Button :disabled="channelInfo.groups.some((item) => item.joined)" @click="subChannel">
        订阅大厅
      </Button>
      <Button type="link" @click="onRefresh">
        刷新
      </Button>
    </Row>
    <Space direction="vertical">
      <GroupItem v-for="item in channelInfo.groups" :key="item.id" :data="item" />
    </Space>
    <GroupSettingModal v-if="showModal" :ok="onJoin" :cancel="closeModal" type="join" />
  </Row>
</template>
<script setup lang="ts">
import { Button, message, Row, Space } from 'ant-design-vue';
import { MicrophoneAudioTrack, RemoteAudioTrack } from 'dingrtc';
import { useClient, useChannelInfo } from '~/store';
import { onMounted, onUnmounted, ref } from 'vue';
import GroupSettingModal from './SettingModal.vue';
import GroupItem from './Item.vue';

const client = useClient();
const channelInfo = useChannelInfo();
const showModal = ref(false);

const openModal = () => {
  showModal.value = true;
}

const onRefresh = () => {
  channelInfo.$patch({ groups: [...client.groups] });
};
const onJoin = async (joinFromData) => {
  const { groupId, groupName, mixToGroup: publishAudioToGroup, userData = '' } = joinFromData;
  if (channelInfo.subscribeAudio === 'mcu' && channelInfo.mcuAudioTrack) {
    await client.unsubscribe('mcu', 'audio');
    channelInfo.$patch({
      groups: [...client.groups],
      subscribeAudio: '',
      mcuAudioTrack: null,
    });
  }
  await client.joinGroup({
    groupId,
    publishAudioToGroup,
    userData,
  });
  try {
    client.setGroupName(groupId, groupName)
  } catch (e: any) {
    message.error(e.message)
  };
};

const closeModal = () => {
  showModal.value = false;
};

const mixToChannel = () => {
  client.mixAudioToGroup(channelInfo.micTrack as MicrophoneAudioTrack, true).then(() => {
    message.info('混音到大厅成功')
  })
}

const subChannel = async () => {
  if (channelInfo.subscribeAudio && channelInfo.subscribeAudio === 'mcu') {
    return;
  } else if (channelInfo.subscribeAudio) {
    await client.unsubscribeGroup(channelInfo.subscribeAudio);
  }
  const audioTrack = (await client.subscribe('mcu', 'audio')) as RemoteAudioTrack;
  message.info('订阅成功');
  audioTrack.play();
  channelInfo.$patch({ subscribeAudio: 'mcu', mcuAudioTrack: audioTrack });

};
onMounted(() => {
  channelInfo.$patch({
    groups: [...client.groups],
  });
});
onUnmounted(() => {
  closeModal();
});
</script>
<style lang="less" scoped>
@import url('../index.module.less');
</style>
