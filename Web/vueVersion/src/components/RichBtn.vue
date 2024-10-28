<script setup lang="ts">
import { Popover } from 'ant-design-vue';
import { ref } from 'vue';

import { CaretUpOutlined, CaretDownOutlined } from '@ant-design/icons-vue';

const props = defineProps<{ hasSelector: boolean; text: string }>();
const showPanel = ref(false);
const updateVisible = () => {
  showPanel.value = !showPanel.value;
};
</script>

<template>
  <div class="richBtnWrapper">
    <div class="topPart">
      <slot name="BtnIcon"></slot>
      <Popover :open="showPanel" trigger="click" :onOpenChange="(open) => (showPanel = open)">
        <template #content>
          <slot name="panelName" :updateVisible="updateVisible"></slot>
        </template>
        <div
          v-if="props.hasSelector"
          class="selectorIcon"
          @click="
            (e) => {
              e.stopPropagation();
            }
          "
        >
          <CaretUpOutlined v-if="showPanel" /><CaretDownOutlined v-else />
        </div>
      </Popover>
    </div>
    <div class="textPart">{{ props.text }}</div>
  </div>
</template>

<style lang="less" scoped>
.richBtnWrapper {
  width: 70px;
  height: 55px;
  border-radius: 4px;
  overflow: hidden;
  display: flex;
  flex-direction: column;
  justify-content: center;
  align-items: center;
  &:hover {
    background-color: #dcdcdc;
    cursor: pointer;
  }
  .topPart {
    flex: 1;
    width: 100%;
    display: flex;
    justify-content: center;
    align-items: center;
    position: relative;
    .btnIcon {
      height: 24px;
      width: 100%;
    }
    .selectorIcon {
      position: absolute;
      height: 20px;
      width: 20px;
      right: 0;
      margin-right: 2px;
      display: flex;
      justify-content: center;
      align-items: center;
      border-radius: 2px;
      overflow: hidden;
      &:hover {
        background-color: #b3b3b4;
      }
    }
  }
  .textPart {
    width: 100%;
    text-align: center;
    font-size: 12px;
    display: block;
    opacity: 0.56;
    height: 17px;
    white-space: nowrap;
    color: rgba(23, 26, 29, 0.85);
  }
}
</style>
