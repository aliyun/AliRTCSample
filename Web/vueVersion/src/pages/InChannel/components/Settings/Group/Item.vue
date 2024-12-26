<template>
  <Row class="group">
    <Row>
      <Col class="groupBrief">
        <Text>
          <Tooltip :title="`id:${ id }`">
            id:{{ id }}
          </Tooltip>
          <Text type="secondary">
          ({{ data.users.length }}人)
          </Text>
        </Text>
        <Tooltip :title="`分组名:${ data.name }`">
          <Text>
            分组名:{{ data.name }}
          </Text>
        </Tooltip>
      </Col>
      <Divider type="vertical" />
      <Tooltip :title="data.users.map((item) => `uid:${item.userId}${ item.userData ? `,用户数据:${item.userData}` : ''};`)">
        <Text class="uidList">
          成员列表:
          <Text v-for="item in data.users">{{ `uid:${item.userId}${ item.userData ? `,用户数据:${item.userData}` : ''};` }}</Text>
        </Text>
      </Tooltip>
    </Row>
    <Row class="actions">
      <Tag v-for="item in tags" :key="item.text" :color="item.color" size="small">
        {{ item.text }}
      </Tag>
      <Tooltip :arrow="false" :overlay-inner-style="{ backgroundColor: 'rgba(245, 247, 250, 0.9)'}">
        <template #title>
          <List>
            <List.Item
              v-for="item in actions"
              :key="item.text"
              class="actionsItem"
              style="cursor: pointer"
              @click="item.onClick"
            >
              {{ item.text }}
            </List.Item>
          </List>
        </template>
        <div class="items">
          <Icon type="icon-XDS_List" />
        </div>
      </Tooltip>
    </Row>
  </Row>
  <GroupSettingModal v-if="showModal" :name="props.data.name" :ok="onNameOk" type="modifyName" :cancel="closeModal" />
</template>
<script setup lang="ts">
import { Row, Tag, Tooltip, List, Typography, Col, Divider, message } from 'ant-design-vue';
import Icon from '~/components/Icon';
import { useClient, useChannelInfo } from '~/store';
import { computed, ref } from 'vue';
import { MicrophoneAudioTrack } from 'dingrtc';
import GroupSettingModal from './SettingModal.vue';

const { Text } = Typography;

const channelInfo = useChannelInfo();
const client = useClient();
const props = defineProps(['data']);
const showModal = ref(false);

const { id } = props.data;

const unsubGroup = async () => {
  if (channelInfo.subscribeAudio === id) {
    await client.unsubscribeGroup(id);
    channelInfo.$patch({ subscribeAudio: '', mcuAudioTrack: null });
  }
};

const onNameOk = (name: string) => {
  try {
    client.setGroupName(id, name)
  } catch (e: any) {
    message.error(e.message)
  };
}

const closeModal = () => {
  showModal.value = false;
};

const dismiss = async () => {
  await unsubGroup();
  await client.dismissGroup(id);
};

const leave = async () => {
  await unsubGroup();
  await client.leaveGroup(id);
};
const subGroup = async () => {
  if (channelInfo.subscribeAudio && channelInfo.subscribeAudio !== id) {
    await client.unsubscribeGroup(channelInfo.subscribeAudio);
  }
  const audioTrack = await client.subscribeGroup(id);
  audioTrack.play();
  channelInfo.$patch({ subscribeAudio: id, mcuAudioTrack: audioTrack });
};

const actions = computed(() => {
  const buttons = [
    {
      text: '设置名称',
      show: props.data.joined,
      onClick: () => {
        showModal.value = true;
      },
    },
    {
      text: '混音',
      show: props.data.joined && !props.data.audioPublished,
      onClick: () => {
        client.mixAudioToGroup(channelInfo.micTrack as MicrophoneAudioTrack, true, id).then(() => {
          channelInfo.$patch({ groups: client.groups });
        });
      },
    },
    {
      text: '取消混音',
      show: props.data.joined && props.data.audioPublished,
      onClick: () => {
        client.mixAudioToGroup(channelInfo.micTrack as MicrophoneAudioTrack, false, id).then(() => {
          channelInfo.$patch({ groups: client.groups });
        });
      },
    },
    {
      text: '订阅',
      show: props.data.joined && channelInfo.subscribeAudio !== id,
      onClick: () => {
        subGroup();
      },
    },
    {
      text: '取消订阅',
      show: props.data.joined && channelInfo.subscribeAudio === id,
      onClick: () => {
        unsubGroup();
      },
    },
    {
      text: '离开',
      show: props.data.joined,
      onClick: () => {
        leave();
      },
    },
    {
      text: '解散',
      show: true,
      onClick: () => {
        dismiss();
      },
    },
  ].filter((item) => item.show);
  return buttons;
});
const tags = computed(() => {
  const blueTexts: string[] = [];
  if (channelInfo.subscribeAudio === id) {
    blueTexts.push('订阅');
  }
  if (props.data.audioPublished) {
    blueTexts.push('混音');
  }
  const list = [
    {
      color: 'green',
      show: props.data.joined,
      text: '已加入',
    },
    {
      color: 'blue',
      show: props.data.joined && (channelInfo.subscribeAudio === id || props.data.audioPublished),
      text: `已${blueTexts.join('、')}`,
    },
  ];
  return list.filter((item) => item.show);
});
</script>
<style lang="less" scoped>
@import url('../index.module.less');
</style>
