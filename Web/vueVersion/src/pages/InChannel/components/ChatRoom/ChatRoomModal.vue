<template>
  <Modal
    :title="scene === 'joinSession' ? '加入会话' : '私聊消息'"
    :width="400"
    :open="open"
    cancel-text="取消"
    ok-text="确认"
    @ok="onOk"
    @cancel="onCancel"
  >
    <Textarea v-model:value="inputText" :placeholder="scene === 'joinSession' ? '请输入会话Id' : '请输入消息'" />
  </Modal>
</template>
<script setup lang="ts">
import { Modal, Textarea } from 'ant-design-vue';
import { ref } from 'vue';
const inputText = ref('');

const props = defineProps<{
  scene: 'privateChat' | 'joinSession',
  cancel: () => void;
  open: boolean;
  ok: (inputText: string) => void;
}>();

const onOk = () => {
  props.ok(inputText.value);
  onCancel();
};

const onCancel = () => {
  inputText.value = '';
  props.cancel();
}

</script>
<!-- <style lang="less" scoped>
@import url('../index.module.less');
</style> -->
