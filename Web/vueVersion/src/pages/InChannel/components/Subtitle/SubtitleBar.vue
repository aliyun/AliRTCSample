
<template>
  <Row class="subTitle">
    <Space class="subTitleHeader">
      <Popover trigger="click" :arrow="false">
        <template #content>
          <ChangeTranslatePanel />
        </template>
        <Tooltip :arrow="false" title="翻译">
            <Icon type="icon-XDS_translate" />
          </Tooltip>
      </Popover>
      <Tooltip :arrow="false" title="关闭字幕">
        <Icon type="icon-XDS_Close" @click="closeSubtitle" />
      </Tooltip>
    </Space>
    <Col class="subTitleContent">
      <template v-if="sortedSpeakers.length">
        <Row class="subTitleList">
          <Row v-for="uid in sortedSpeakers" class="subTitleItem">
            <Col class="subTitleName">{{ channelInfo.allUsers.find(user => user.userId === uid)?.userName }}</Col>
            <Col v-if="messageMap[uid]?.origin && (asrInfo.transLang === 'source' || asrInfo.dualLang)" class="subTitleSource">{{ messageMap[uid]?.origin }}</Col>
            <Col v-if="messageMap[uid]?.trans && asrInfo.transLang !== 'source'" :class="[asrInfo.dualLang ? 'subTitleTranslated' : 'subTitleSource']">{{ messageMap[uid]?.trans }}</Col>
          </Row>
        </Row>
      </template>
      <template v-else>
        暂无人说话..
      </template>
    </Col>
  </Row>
</template>
<script lang="ts" setup>
import { Row, Col, Tooltip, Space, Popover } from 'ant-design-vue';
import { onMounted, reactive, ref, computed, onBeforeUnmount } from 'vue';
import Icon from '~/components/Icon';
import { ASRMessage } from 'dingrtc-asr';
import { useAsrInfo, useChannelInfo, useClient } from '~/store';
import { ChangeTranslatePanel } from './TranslatePanel';
import { RemoteUser } from 'dingrtc';

const asrInfo = useAsrInfo();
const channelInfo = useChannelInfo();

interface MessageInfo {
  timerId?: number;
  origin?: string;
  trans?: string;
  modifyTime?: number;
}

const MAXSPAKERS = 2;
const messageMap = reactive<Record<string, MessageInfo>>({});
const speakers = ref<string[]>([]);
const client = useClient();

const handleUserLeft = (user: RemoteUser) => {
  removeSpeaker(user.userId);
}

onMounted(() => {
  asrInfo.asr.on('message', (message) => {
    addMessage(message);
  })
  client.on('user-left', handleUserLeft)
})

onBeforeUnmount(() => {
  client.off('user-left', handleUserLeft)
})

const sortedSpeakers = computed(() => speakers.value.sort((a, b) => messageMap[a].modifyTime - messageMap[b].modifyTime));

const removeSpeaker = (toRemoved: string) => {
  const index = speakers.value.findIndex(it => it === toRemoved);
  if (index === -1) return
  speakers.value.splice(index, 1);
  delete messageMap[toRemoved];
}

const addSpeaker = (uid: string) => {
  const index = speakers.value.findIndex(item => item === uid);
    if (index > -1) speakers.value.splice(index, 1);
    else if (speakers.value.length === MAXSPAKERS) {
      const toRemoved = speakers.value[0];
      const timerId = window.setTimeout(() => {
        removeSpeaker(toRemoved)
      }, 2.5 * 1000)
      messageMap[toRemoved].timerId = timerId;
    }
    speakers.value.push(uid);
}

const addMessage = (message: ASRMessage) => {
  const { uid, translated, sentence, timestamp } = message;
  if (messageMap[uid]?.timerId != null) {
    clearTimeout(messageMap[uid]?.timerId)
  }
  addSpeaker(uid);
  const timerId = window.setTimeout(() => {
    removeSpeaker(uid);
  }, 5 * 1000)
  const temp = Object.assign({}, messageMap[uid], { timerId });
  if (translated) temp.trans = sentence;
  else temp.origin = sentence;
  if (!temp.modifyTime) temp.modifyTime = timestamp;
  messageMap[uid] = temp;
}


const closeSubtitle = () => {
  asrInfo.asr.setEnabled(false);
  asrInfo.enabled = false;
}

</script>
<style lang="less" scoped>
@import url('./index.module.less');
</style>
