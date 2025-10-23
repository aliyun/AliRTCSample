<template>
  <Row>
    <Form :model="formData" style="width: 100%" :label-col="{ span: 6 }" label-align="left">
      <Form.Item label="功能选项" name="functionNumber">
        <RadioGroup v-model:value="formData.functionNumber" :options="ASROptions" :disabled="!!asrInfo.asrTaskId" @change="onAsrFunctionChange" />
      </Form.Item>
      <Form.Item label="启用字幕" name="enable" help="字幕仅自己可见">
        <Checkbox v-model:checked="formData.enable" @change="onAsrEnableChange" />
      </Form.Item>
      <Divider orientation="left" plain>
        <Text>翻译设置</Text>
      </Divider>
      <Form.Item label="你说的语言" name="originLang">
        <Select v-model:value="formData.originLang" :options="speakLanguages" @change="onSpeakLangChange"/>
      </Form.Item>
      <Form.Item label="你看的语言" name="transLang">
        <Select v-model:value="formData.transLang" :options="translateLanguages" @change="onTransLangChange"/>
      </Form.Item>
      <Form.Item label="显示双语" name="dualLang">
        <Select v-model:value="formData.dualLang" :options="dualLangConfigs" @change="onDualLangChange"/>
      </Form.Item>
    </Form>
  </Row>
</template>
<script lang="ts" setup>
import { Row, Form, Checkbox, Divider, Typography, Select, RadioGroup } from 'ant-design-vue';
import { reactive, computed } from 'vue';
import { useAsrHooks } from '~/hooks/channel';
import { useAsrInfo, useCurrentUserInfo } from '~/store';
import { startASR, stopASR } from '~/utils/request';

const ASROptions = [
  {
    label: '字幕',
    value: 1,
  },
  {
    label: '会后纪要',
    value: 2,
  },
  {
    label: '字幕+会后纪要',
    value: 3,
  },
]

const translateLanguages = computed(() =>
  asrInfo.asr.supportedTranslateLanguages.map((item) => ({
    label: item.displayName, value: item.language,
  })),
);

const speakLanguages = computed(() =>
  asrInfo.asr.supportedSpeakLanguages.map((item) => ({
    label: item.displayName, value: item.language,
  })),
);

const { changeTranslateLanguages, changeDualLang } = useAsrHooks();

const dualLangConfigs = [
  {
    label: '显示',
    value: 'true',
  },
  {
    label: '不显示',
    value: 'false',
  },
];

const { Text } = Typography;

const asrInfo = useAsrInfo();
const currentUserInfo = useCurrentUserInfo();

const formData = reactive({
  originLang: asrInfo.originLang,
  transLang: asrInfo.transLang,
  dualLang: String(asrInfo.dualLang),
  enable: asrInfo.enabled,
  functionNumber: asrInfo.functionNumber,
});

const onAsrEnableChange = (e) => {
  const value = e.target.checked;
  asrInfo.asr.setEnabled(value);
  asrInfo.enabled = value;
  if (!asrInfo.asrTaskId && value) {
    startASR(currentUserInfo.appId, currentUserInfo.channel, asrInfo.functionNumber).then(({ taskId }) => {
    asrInfo.asrTaskId = taskId;
  }).catch(console.log);
  }
  if (!value && asrInfo.asrTaskId) {
    stopASR(currentUserInfo.appId, currentUserInfo.channel, asrInfo.asrTaskId).then(() => {
      asrInfo.asrTaskId = '';
    }).catch(console.log);
  }
};

const onAsrFunctionChange = (e) => {
  const value = e.target.value;
  asrInfo.functionNumber = value;
}

const onSpeakLangChange = (value) => {
  asrInfo.asr.setCurrentSpeakLanguage(value);
  asrInfo.originLang = value;
}

const onTransLangChange = (value) => {
  changeTranslateLanguages(value);
}

const onDualLangChange = (value) => {
  changeDualLang(value === 'true');
}

</script>
