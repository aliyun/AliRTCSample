<template>
  <a-popover placement="right" trigger="click">
    <template #content>
      <div class="toolbox-panel">
        <div>
          <a-input v-model:value="imgUrl" placeholder="请输入图片 url" />
        </div>
        <a-button class="action-btn" @click="addImage">添加图片图形</a-button>
        <a-button class="action-btn" @click="addBgImage">设置为背景图</a-button>

        <a-divider />

        <a-button
          type="primary"
          class="action-btn"
          @click="uploadImage"
          :loading="!!uploadImgProgress"
        >
          {{ uploadImgProgress || '上传图片图形' }}
        </a-button>

        <a-button
          type="primary"
          class="action-btn"
          @click="uploadBgImage"
          :loading="!!uploadBgProgress"
        >
          {{ uploadBgProgress || '上传为背景图' }}
        </a-button>
      </div>
    </template>
    <slot></slot>
  </a-popover>
</template>

<script setup lang="ts">
import { message } from 'ant-design-vue';
import { ref } from 'vue';
import { RtcWhiteboard } from '@dingrtc/whiteboard';

interface IProps {
  whiteboard: RtcWhiteboard
}

const props = defineProps<IProps>();
const imgUrl = ref('');
const uploadImgProgress = ref('');
const uploadBgProgress = ref('');

const addImage = () => {
  if (!imgUrl.value) {
    message.error('请输入图片链接');
    return;
  }
  props.whiteboard.addImageShape(imgUrl.value);
};

const addBgImage = () => {
  if (!imgUrl.value) {
    message.error('请输入图片链接');
    return;
  }
  props.whiteboard.setBackgroundImage(imgUrl.value);
};

const uploadImage = () => {
  props.whiteboard.uploadImage(false, state => {
    uploadImgProgress.value = '0%';
    if (state!.loaded !== state!.total) {
      uploadImgProgress.value = `${(
        (state!.loaded / state!.total) *
        100
      ).toFixed(2)}%`;
      console.log(`已上传${(state!.loaded / state!.total) * 100}%`);
    } else {
      uploadImgProgress.value = '';
      message.success('上传成功');
    }
  });
};

const uploadBgImage = () => {
  props.whiteboard.uploadImage(true, state => {
    uploadBgProgress.value = '0%';
    if (state!.loaded !== state!.total) {
      uploadBgProgress.value = `${(
        (state!.loaded / state!.total) *
        100
      ).toFixed(2)}%`;
      console.log(`已上传${(state!.loaded / state!.total) * 100}%`);
    } else {
      uploadBgProgress.value = '';
      message.success('上传成功');
    }
  });
};
</script>

<style lang="less" scoped>
.toolbox-panel {
  width: 300px;
  background: #ffffff;
  border-radius: 2px;

  input {
    margin-bottom: 10px;
  }
  .action-btn ~ .action-btn {
    margin-left: 10px;
  }
}
</style>
