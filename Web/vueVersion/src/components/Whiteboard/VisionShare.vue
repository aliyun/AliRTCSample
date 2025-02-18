<template>
  <div class="wb-admin">
    <a-tooltip placement="bottom">
      <template #title>
        <span style="font-size: 12px">
          所有用户定位到我当前的视角，仅同步一次，且仅对当前页面生效
        </span>
      </template>
      <a-button
        size="small"
        class="top-btn"
        type="primary"
        ghost
        @click="alignVision"
      >
        <Icon type="icon-locate" />
      </a-button>
    </a-tooltip>

    <a-tooltip placement="bottom">
      <template #title>
        <span style="font-size: 12px">调整当前页面到初始视角</span>
      </template>
      <a-button
        size="small"
        class="top-btn"
        type="primary"
        ghost
        @click="center"
      >
        一键居中
      </a-button>
    </a-tooltip>

    <a-tooltip color="#fff" placement="bottom" v-if="isOthersSharing">
      <template #title>
        <div style="color: #333">
          <template v-if="isFollowing">
            正在跟随
            {{ sharingUser.name }}
            的视角 <br />
            点击关闭跟随
          </template>
          <template v-else>
            {{ sharingUser.name }}
            正在分享视角 <br />
            点击开始跟随
          </template>
        </div>
      </template>
      <a-button
        size="small"
        type="primary"
        class="top-btn"
        :style="{
          ...(isFollowing
            ? {
                backgroundColor: '#73d13d',
                borderColor: '#73d13d',
                color: '#fff'
              }
            : {})
        }"
        @click="toggleFollow"
      >
        {{ sharingUser.name }}
        正在分享视角
      </a-button>
    </a-tooltip>

    <a-button
      size="small"
      class="top-btn"
      type="primary"
      ghost
      :danger="isMeSharing"
      @click="toggleShare"
    >
      {{ isMeSharing ? '正在共享视角' : '视角共享' }}
    </a-button>
    <a-tooltip placement="bottomLeft">
      <template #title>
        <span>
          {{ role === 'Admin' ? '当前角色为 Admin，可以删除任何内容' : '' }}
          {{
            role === 'Attendee' ? '当前角色为 Attendee，可以操作自己的内容' : ''
          }}
          {{ role === 'Viewer' ? '当前角色为 viewer，仅观看权限' : '' }}
        </span>
      </template>
      <a-button size="small" type="primary" @click="toggleAdmin">
        {{ role }}
      </a-button>
    </a-tooltip>
  </div>
</template>

<script setup lang="ts">
import { message } from 'ant-design-vue';
import { onMounted, reactive, ref } from 'vue';
import Icon from '~/components/Icon';
import { RtcWhiteboard } from '@dingrtc/whiteboard';

interface IProps {
  whiteboard: RtcWhiteboard
}

const props = defineProps<IProps>();
const isFollowing = ref(false); // 是否正在看别人共享视角
const isOthersSharing = ref(false); // 是否有人正在共享视角
const isMeSharing = ref(false); // 我是否正在共享视角
const sharingUser = reactive({ name: '', userId: '' });
const role = ref<'Admin' | 'Attendee' | 'Viewer'>('Admin');

const center = () => props.whiteboard.center();

const alignVision = () => {
  props.whiteboard.alignVision();
};

const toggleAdmin = () => {
  if (role.value === 'Admin') {
    role.value = 'Attendee';
  } else if (role.value === 'Attendee') {
    role.value = 'Viewer';
  } else if (role.value === 'Viewer') {
    role.value = 'Admin';
  }
  props.whiteboard.setRoleType(role.value);
};

const toggleShare = () => {
  if (isMeSharing.value) {
    props.whiteboard.stopVisionShare();
    message.success('您已停止视角分享');
  } else {
    message.success('已开始共享视角，其他用户将自动跟随您的视角');
    isFollowing.value = false;
    isOthersSharing.value = false;
    sharingUser.name = '';
    sharingUser.userId = '';
    props.whiteboard.startVisionShare();
  }
  isMeSharing.value = !isMeSharing.value;
};

const toggleFollow = () => {
  if (isFollowing.value) {
    props.whiteboard.stopFollowVision();
  } else {
    props.whiteboard.startFollowVision();
  }
  isFollowing.value = !isFollowing.value;
};

onMounted(() => {
  props.whiteboard.setRoleType('Admin');
  props.whiteboard.on('user-vision-share-start', (userId, userName) => {
    isOthersSharing.value = true;
    message.info(
      `${userName} 正在共享视角，已自动跟随，点击右上角按钮取消跟随`
    );
    sharingUser.name = userName;
    sharingUser.userId = userId;
    props.whiteboard.startFollowVision();
    isFollowing.value = true;
  });
  props.whiteboard.on('user-vision-share-stop', (userId, userName) => {
    isOthersSharing.value = false;
    isFollowing.value = false;
    if (userId !== props.whiteboard.userId) {
      message.info(`${userName} 已停止共享视角`);
    }
    sharingUser.userId = '';
    sharingUser.name = '';
  });
});
</script>

<style lang="less" scoped>
.wb-admin {
  position: absolute;
  right: 0;
  top: 16px;
  z-index: 10;
  font-size: 12px;
  padding: 5px 16px;
  min-height: 30px;
  display: flex;
  align-items: center;
  color: #333;
  user-select: none;
  height: 24px;
  &__admin-user-name {
    font-weight: bold;
  }
  &__admin-user-label {
    margin-left: 3px;
    color: #1890ff;
  }
  .top-btn {
    display: flex;
    align-items: center;
    margin-right: 10px;
    margin-left: 5px;
    font-size: 12px !important;
  }
}
</style>
