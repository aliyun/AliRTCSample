<template>
  <Modal :title="type === 'join' ? '加入分组' : '设置名称'" open :width="400" cancel-text="取消" ok-text="确认" @ok="joinOk"
    @cancel="cancel">
    <Form :label-col="{ span: 6 }" labelAlign="left" :model="formData">
      <Form.Item v-if="type === 'join'" label="分组Id" name="groupId" required>
        <Input v-model:value="formData.groupId" placeholder="请填写分组Id" />
      </Form.Item>
      <Form.Item label="分组名称" name="groupName" required>
        <Input v-model:value="formData.groupName" placeholder="相同Id分组名称以后设置的为准" />
      </Form.Item>
      <Form.Item v-if="type === 'join'" label="自定义数据" name="userData">
        <Input v-model:value="formData.userData" placeholder="加入分组时可携带自定义数据" />
      </Form.Item>
      <Form.Item v-if="type === 'join'" name="mixToGroup" required>
        <Checkbox v-model:checked="formData.mixToGroup"> 混音到分组 </Checkbox>
      </Form.Item>
    </Form>
  </Modal>
</template>
<script setup lang="ts">
import { Modal, Input, Checkbox, Form } from 'ant-design-vue';
import { reactive } from 'vue';
import { logger } from '~/utils/tools';

const props = defineProps<{
  ok: (...args: any) => void;
  cancel: () => void;
  type: 'join' | 'modifyName';
  name?: string;
}>();

const { cancel, ok, type } = props;

const formData = reactive({
  groupId: '',
  groupName: '',
  mixToGroup: true,
  userData: '',
})

const joinOk = () => {
  if (type === 'join') {
    logger.info('formData', formData);
    ok(formData);
  } else if (type === 'modifyName') {
    ok(formData.groupName);
  }
  cancel();
};

</script>
<style lang="less" scoped>
@import url('../index.module.less');
</style>
