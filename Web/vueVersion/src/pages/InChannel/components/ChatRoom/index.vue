<template>
  <Drawer
    class="chatContainer"
    :open="open"
    :width="500"
    title="聊天"
    @close="onClose"
    :mask-closable="true"
    placement="right"
  >
    <Row class="chatWrapper">
      <Col :span="8">
        <Sessions />
      </Col>
      <Col :span="16">
        <template v-if="showMessages">
          <Message />
          <Divider class="chatDivider" />
          <Editor />
        </template>
        <Empty v-else description="请先选择会话"	 />
      </Col>
    </Row>
  </Drawer>
</template>
<script lang="ts" setup>
import { Divider, Drawer, Row, Col, Empty } from 'ant-design-vue';
import { computed } from 'vue';
import { useCurrentUserInfo, useRTMInfo } from '~/store';
import Message from './Message.vue';
import Editor from './Editor.vue';
import Sessions from './Sessions.vue';

const rtmInfo = useRTMInfo();

const currentUserInfo = useCurrentUserInfo();

const showMessages = computed(() => {
  const activeSession = rtmInfo.sessions.find(item => item.sessionId === rtmInfo.activeSessionId);
  return activeSession?.members?.some(item => item.userId === currentUserInfo.userId)
})

const onClose = () => {
  rtmInfo.enabled = false;
};

const open = computed(() => rtmInfo.enabled);
</script>
<style lang="less" scoped>
@import url('./index.module.less');
</style>
