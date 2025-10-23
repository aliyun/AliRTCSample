<template>
  <Row class="chatEditor">
    <Col :span="24" class="chatEditorInput">
      <Textarea  ref="textareaRef" placeholder="请输入消息" focused  @press-enter="sendMessage" v-model:value="inputText" :max-length="100" />
    </Col>
    <Col :span="24" class="chatEditorButton">
      <span v-if="globalFlag.isMac">↵ 发送 / ⌘↵ 换行</span>
      <span v-else>↵ 发送 / CTRL+↵ 换行</span>
      <Button :disabled="!inputText" type="primary"  @click="sendMessage">发送</Button>
    </Col>
  </Row>
</template>
<script lang="ts" setup>
import { Row, Button, Textarea, Col } from 'ant-design-vue';
import { ref } from 'vue';
import { useRTMInfo, useCurrentUserInfo, useGlobalFlag } from '~/store';

const rtmInfo = useRTMInfo();
// @ts-ignore
window.rtmInfo = rtmInfo;
const currentUserInfo = useCurrentUserInfo();
const globalFlag = useGlobalFlag();
const textareaRef = ref<HTMLTextAreaElement>(null);
const inputText = ref('');

const encoder = new TextEncoder();
const sendMessage = (e?: KeyboardEvent | MouseEvent) => {
  if ((e as KeyboardEvent).key === 'Enter') {
    if (e.shiftKey || e.ctrlKey || e.metaKey) return;
    else e.preventDefault();
  }
  inputText.value = inputText.value.trim();
  if (!inputText.value) return
  const data = encoder.encode(inputText.value)
  rtmInfo.rtm.publish(rtmInfo.activeSessionId, data);
  const activeSession = rtmInfo.sessions.find(item => item.sessionId === rtmInfo.activeSessionId);
  console.log('activeSession===', activeSession);
  activeSession.messages.push({
    message: data,
    broadcast: true,
    uid: currentUserInfo.userId,
    sessionId: rtmInfo.activeSessionId,
    timestamp: Date.now(),
  })
  inputText.value = '';
};


</script>
<style lang="less" scoped>
@import url('./index.module.less');
</style>
