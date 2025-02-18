<template>
  <a-popover placement="right" trigger="click">
    <template #content>
      <div class="toolbox-panel">
        <div class="setting-item">
          <div class="setting-item-title">同步鼠标位置</div>
          <div class="setting-item-value">
            <a-switch @change="onSwitch" :checked="courseSyncEnabled" />
          </div>
        </div>
        <div class="setting-item">
          <div class="setting-item-title">鼠标同步类型</div>
          <div class="setting-item-value">
            <a-radio-group :value="courseSyncType" @change="onSyncTypeChange">
              <a-radio value="write">仅书写时</a-radio>
              <a-radio value="all">全部</a-radio>
            </a-radio-group>
          </div>
        </div>
      </div>
    </template>
    <slot></slot>
  </a-popover>
</template>

<script setup lang="ts">
import { message } from 'ant-design-vue';
import { ref, onMounted } from 'vue';
import { RtcWhiteboard } from '@dingrtc/whiteboard';

interface IProps {
  whiteboard: RtcWhiteboard
}

const props = defineProps<IProps>();
const courseSyncEnabled = ref(true);
const courseSyncType = ref<'write' | 'all'>('write');

const onSwitch = () => {
  if (courseSyncEnabled.value) {
    props.whiteboard.disableCursorSync();
  } else {
    props.whiteboard.enableCursorSync();
  }
  courseSyncEnabled.value = !courseSyncEnabled.value;
  message.success(`已${courseSyncEnabled.value ? '开启' : '关闭'}同步鼠标位置`);
};

const onSyncTypeChange = (value) => {
  courseSyncType.value = value.target.value;
  props.whiteboard.setCursorSyncType(courseSyncType.value);
};

onMounted(() => {
  props.whiteboard.enableCursorSync();
});
</script>

<style lang="less" scoped>
.toolbox-panel {
  width: 300px;
  background: #ffffff;
  border-radius: 2px;
  display: flex;
  flex-direction: column;
  gap: 10px;

  .setting-item {
    display: flex;
    justify-content: space-between;
  }
  .setting-item ～ .setting-item {
    margin-top: 10px;
    position: relative;
    &::before {
      content: '';
      position: absolute;
      top: -5px;
      left: 20px;
      right: 20px;
      height: 1px;
      background: #e8e8e8;
    }
  }
  .setting-item-value {
  }
}
</style>
