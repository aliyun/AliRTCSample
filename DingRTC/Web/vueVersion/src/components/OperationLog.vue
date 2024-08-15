<script lang="ts" setup>
import { Drawer as ADrawer } from 'ant-design-vue';
</script>

<template>
  <ADrawer
    :visible="true"
    :mask="false"
    class="custom-class"
    root-class-name="root-class-name"
    :root-style="{ color: 'blue' }"
    style="color: red"
    title="Basic Drawer"
    placement="bottom"
  >
    <div id="logOutput"></div>
  </ADrawer>
</template>

<style scoped>
#logOutput {
  width: 100%;
  background-color: #fff;
  overflow-y: auto;
  overflow-x: hidden;
  color: #000;
  white-space: pre-wrap;
  word-break: break-all;
  min-height: 100px;
  max-height: 500px;
}
</style>
