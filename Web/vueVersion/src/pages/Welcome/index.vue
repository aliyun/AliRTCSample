<template>
  <div class="wrap">
    <Title :level="5" class="demoTitle">DingRTC Demo</Title>
    <Button :disabled="testing" class="testNetwork" @click="onTestNetwork">
      {{ testing ? `请等待${ticktok}s` : '测试网络' }}
    </Button>
    <!-- 添加设备检测按钮 -->
    <Button class="testDevBtn" @click="startDeviceTestHandler">设备检测</Button>
    <!-- 设备检测容器 -->
    <Modal v-model:visible="showDeviceTest" title="设备检测" footer="">
      <div id="dev-check-root" class="dev-check-root"></div>
    </Modal>
    <div class="main">
      <Preview />
      <Join />
    </div>
  </div>
</template>

<script setup lang="ts">
import { nextTick, ref } from "vue";
import { Typography, Button, Modal } from 'ant-design-vue';
import Preview from "./components/Preview.vue";
import Join from "./components/Join.vue";
import { startTestNetworkQuality } from "~/utils/networkQuality";

// 添加设备检测相关的导入
import { DeviceChecker } from 'dingrtc-device-checker';
import { logger } from "~/utils/tools";
import { useClient } from "~/store";
const { Title } = Typography;

const client = useClient()
logger.info('DeviceChecker.name', DeviceChecker.name, DeviceChecker.pluginName)

// 添加设备检测相关的响应式变量
const showDeviceTest = ref(false);

// 添加设备检测函数
const startDeviceTestHandler = () => {
  showDeviceTest.value = true;
  // 等待DOM更新后执行设备检测
  nextTick(async () => {
    const container = document.getElementById('dev-check-root');
    if (container) {
      // @ts-ignore
      const result = await client.startPlugin(DeviceChecker.pluginName, { rootElement: container });
      logger.info('设备检测结果', result);
      showDeviceTest.value = false
    }
  });
};
// 状态管理
const testing = ref(false);
const ticktok = ref(15);

// 方法
const onTestNetwork = async () => {
  if (!testing.value) {
    testing.value = true;
    ticktok.value = 15;

    const intervalId = setInterval(() => {
      ticktok.value = Math.max(0, ticktok.value - 1);
    }, 1000);

    await startTestNetworkQuality();

    testing.value = false;
    clearInterval(intervalId);
  }
};
</script>

<style lang="less" scoped>
@import url('./index.module.less');

.testNetwork {
  position: absolute;
  top: 18px;
  left: 170px;
}

.testDevBtn {
  position: absolute;
  top: 18px;
  left: 270px;
}

.dev-check-root {
  width: 100%;
  height: 100%;
  border: 1px solid gray;
}
</style>
