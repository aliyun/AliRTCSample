<template>
  <a-popover placement="right" trigger="click">
    <template #content>
      <div class="toolbox-panel">
        <a-input-group compact>
          <a-input v-model:="docUrl" placeholder="pdf文档链接地址" />
          <a-button type="primary" @click="loadPdf">加载 pdf</a-button>
        </a-input-group>

        <a-divider />

        <a-button
          type="primary"
          class="action-btn"
          @click="transcodePdf"
          :loading="!!transcodePdfProgress"
        >
          {{ transcodePdfProgress || '上传文件转码为pdf' }}</a-button
        >
        <a-button
          type="primary"
          class="action-btn"
          @click="transcodeDoc"
          :loading="!!transcodeDocProgress"
        >
          {{ transcodeDocProgress || '上传文件转码为图片文档' }}
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
const docUrl = ref('');
const transcodePdfProgress = ref('');
const transcodeDocProgress = ref('');

const loadPdf = () => {
  if (docUrl.value) {
    props.whiteboard.addPdfDoc(docUrl.value, '示例pdf 文档');
  }
};

const transcodePdf = () => {
  props.whiteboard
    .transcodeFile({
      targetType: 'pdf',
      onUploadProgress: state => {
        transcodePdfProgress.value = '上传中 0%';
        if (state!.loaded !== state!.total) {
          transcodePdfProgress.value = `上传中 ${(
            (state!.loaded / state!.total) *
            100
          ).toFixed(2)}%`;
          console.log(`已上传${(state!.loaded / state!.total) * 100}%`);
        } else {
          transcodePdfProgress.value = '上传成功';
          message.success('上传成功');
        }
      },
      onTranscodeProgress: state => {
        if (state === 'init') {
          transcodePdfProgress.value = '开始转码';
        } else if (state === 'success') {
          transcodePdfProgress.value = '';
          message.success('转码完成');
        } else if (state === 'failed') {
          transcodePdfProgress.value = '转码失败';
        } else if (state === 'processing') {
          transcodePdfProgress.value = '转码中';
        }
      }
    })
    .finally(() => {
      transcodePdfProgress.value = '';
    });
};

const transcodeDoc = () => {
  props.whiteboard
    .transcodeFile({
      targetType: 'jpg',
      onUploadProgress: state => {
        transcodeDocProgress.value = '上传中 0%';
        if (state!.loaded !== state!.total) {
          transcodeDocProgress.value = `上传中 ${(
            (state!.loaded / state!.total) *
            100
          ).toFixed(2)}%`;
          console.log(`已上传${(state!.loaded / state!.total) * 100}%`);
        } else {
          transcodeDocProgress.value = '上传成功';
          message.success('上传成功');
        }
      },
      onTranscodeProgress: state => {
        if (state === 'init') {
          transcodeDocProgress.value = '开始转码';
        } else if (state === 'success') {
          transcodeDocProgress.value = '';
          message.success('转码完成');
        } else if (state === 'failed') {
          transcodeDocProgress.value = '转码失败';
        } else if (state === 'processing') {
          transcodeDocProgress.value = '转码中';
        }
      }
    })
    .finally(() => {
      transcodeDocProgress.value = '';
    });
};
</script>

<style lang="less" scoped>
.toolbox-panel {
  width: 400px;
  background: #ffffff;
  border-radius: 2px;

  input {
    width: 300px;
  }

  .action-btn ~ .action-btn {
    margin-left: 10px;
  }
}
</style>
