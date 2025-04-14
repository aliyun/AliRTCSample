<template>
  <Row class="chatMessages">
    <Row ref="listRef">
      <Comment
        v-for="item in messages"
        :author="getUserNick(item.uid)"
        :datetime="item.broadcast ? '' : '私聊'"
        :content="decoder.decode(item.message)"
        :class="[item.uid === currentUserInfo.userId ? 'chatRight' : '']"
      />
    </Row>
  </Row>
</template>
<script lang="ts" setup>
import { Row, Comment } from 'ant-design-vue';
import { computed, ref, watch } from 'vue';
import { useChannelInfo, useCurrentUserInfo, useRTMInfo } from '~/store';

const decoder = new TextDecoder();
const channelInfo = useChannelInfo();
const currentUserInfo = useCurrentUserInfo();
const listRef = ref<HTMLDivElement>(null);

const getUserNick = (uid: string) => 
  channelInfo.allUsers.find((user) => user.userId === uid)?.userName;

const rtmInfo = useRTMInfo();
const scrollTimer = ref<number>(0);

const messages = computed(() => {
  const activeSession = rtmInfo.sessions.find(item => item.sessionId === rtmInfo.activeSessionId);
  return activeSession?.messages;
});

watch(
  messages,
  () => {
    if (scrollTimer.value) window.clearTimeout(scrollTimer.value);
    scrollTimer.value = window.setTimeout(() => {
      // @ts-ignore
      listRef.value?.$el?.scrollTo({ top: listRef.value?.$el?.scrollHeight, behavior: 'smooth' });
    }, 200);
  },
  { deep: true },
);
</script>
<style lang="less" scoped>
@import url('./index.module.less');
</style>
