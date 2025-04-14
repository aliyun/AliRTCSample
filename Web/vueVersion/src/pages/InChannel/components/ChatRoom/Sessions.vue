<template>
  <Row class="chatSessions">
    <Row class="chatSessionsHeader">
      <TypographyText strong>会话列表</TypographyText>
      <Button type="link" @click="openJoinSessionModal">加入会话</Button>
    </Row>
    <Row ref="listRef">
      <Tree
        :show-icon="true"
        :show-line="{ showLeafIcon: false }"
        :expanded-keys="expandedKeys"
        :tree-data="treeData"
        :root-style="{ width: '100%' }"
        @expand="onExpand"
        @select="onSelect"
        :block-node="true"
      >
        <template #leafIcon>
          <Icon type="icon-icon_XDS_Multiuser" />
        </template>
        <template #switcherIcon>
          <Icon type="icon-icon_XDS_Multiuser" />
        </template>
        <template #title="{ title, key }">
          <Row v-if="key.includes('#session')" class="chatSessionsItem">
            <span style="color: #1890ff">{{ title }}</span>
            <Tooltip
              :arrow="false"
              :overlay-inner-style="{ backgroundColor: 'rgba(245, 247, 250, 0.9)' }"
            >
              <template #title>
                <List>
                  <List.Item
                    v-for="item in getSessionActions(title)"
                    :key="item.title"
                    class="actionsItem"
                    @click="item.onClick"
                  >
                    {{ item.title }}
                  </List.Item>
                </List>
              </template>
              <div class="smallViewActions">
                <Icon type="icon-XDS_List" />
              </div>
            </Tooltip>
          </Row>
          <Row v-else class="chatSessionsItem">
            <span style="color: #1890ff">{{ getUserNick(key) }}</span>
            <Tooltip title="私聊">
              <Icon v-if="key.split('#')[1] !== currentUserInfo.userId" type="icon-icon_dialogshape" @click="(e) => onClickPrivateChat(e, key)" />
            </Tooltip>
          </Row>
        </template>
      </Tree>
    </Row>
  </Row>
  <ChatRoomModal
    :open="!!privateChatInfo.uid"
    :ok="onPrivateChat"
    :cancel="closeModal"
    scene="privateChat"
  />
  <ChatRoomModal
    :open="showJoinSessionModal"
    :ok="joinSession"
    :cancel="closeModal"
    scene="joinSession"
  />
</template>
<script lang="ts" setup>
import {
  Row,
  Tree,
  TreeProps,
  Button,
  Tooltip,
  TypographyText,
  List,
  message,
} from 'ant-design-vue';
import { computed, ref } from 'vue';
import { useChannelInfo, useCurrentUserInfo, useRTMInfo } from '~/store';
import difference from 'lodash-es/difference';
import Icon from '~/components/Icon';
import ChatRoomModal from './ChatRoomModal.vue';
import { useRTMInfoHooks } from '~/hooks/channel';

const encoder = new TextEncoder();
const channelInfo = useChannelInfo();
const currentUserInfo = useCurrentUserInfo();
const listRef = ref<HTMLDivElement>(null);
const expandedKeys = ref<string[]>([]);
const showJoinSessionModal = ref(false);
const privateChatInfo = ref({ sessionId: '', uid: '' });

const { joinSession, leaveSession, closeSession } = useRTMInfoHooks()

const getUserNick = (uid: string) => {
  const [_, userId] = uid.split('#');
  return channelInfo.allUsers.find((user) => user.userId === userId)?.userName;
};

const rtmInfo = useRTMInfo();

const closeModal = () => {
  privateChatInfo.value = { sessionId: '', uid: '' };
  showJoinSessionModal.value = false;
};

const openJoinSessionModal = () => {
  showJoinSessionModal.value = true;
};

const onExpand = (keys: (number | string)[], { expanded, node }) => {
  const tempKeys = ((node.parent ? node.parent.children : treeData.value) || []).map(
    ({ key }) => key,
  );
  if (expanded) {
    expandedKeys.value = difference(keys, tempKeys).concat(node.key);
  } else {
    expandedKeys.value = keys as string[];
  }
};

const getSessionActions = (sessionId: string) => {
  const session = rtmInfo.sessions.find((item) => item.sessionId === sessionId);
  const list = [
    {
      title: '加入',
      show: !session.members.some((item) => item.userId === currentUserInfo.userId),
      onClick: () => joinSession(sessionId),
    },
    {
      title: '离开',
      show: session.members.some((item) => item.userId === currentUserInfo.userId),
      onClick: () => leaveSession(sessionId)
    },
    {
      title: '解散',
      show: true,
      onClick: () => closeSession(sessionId),
    }
  ];
  return list.filter((item) => item.show);
};

const onClickPrivateChat = (e: MouseEvent, uid: string) => {
  e.stopPropagation();
  const [sessionId, userId] = uid.split('#');
  privateChatInfo.value = { sessionId, uid: userId };
};

const onSelect = (_, { selected, node }) => {
  if (selected && node.key.includes('#session')) {
    rtmInfo.activeSessionId = node.title;
  }
};

const onPrivateChat = (inputText: string) => {
  const data = encoder.encode(inputText);
  rtmInfo.rtm.publish(privateChatInfo.value.sessionId, data, privateChatInfo.value.uid);
  // const activeSession = rtmInfo.sessions.find((item) => item.sessionId === privateChatInfo.value.sessionId);
  // activeSession.messages.push({
  //   message: data,
  //   broadcast: true,
  //   uid: currentUserInfo.userId,
  //   sessionId: privateChatInfo.value.sessionId,
  //   timestamp: Date.now(),
  // });
};

const treeData = computed(() => {
  const data: TreeProps['treeData'] = rtmInfo.sessions.map((session) => ({
    title: session.sessionId,
    key: session.sessionId + '#session',
    children: session.members.map((item) => ({
      title: `${session.sessionId}#${item.userId}`,
      key: `${session.sessionId}#${item.userId}`,
    })),
  }));
  return data;
});
</script>
<style lang="less" scoped>
@import url('./index.module.less');
</style>
