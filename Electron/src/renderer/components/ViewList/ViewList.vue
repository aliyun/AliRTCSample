<template>
    <el-scrollbar class="view-list" style="height: 100%">
      <view-info v-for="view in viewList" :key="view.userid" :view="view"></view-info>
    </el-scrollbar>
</template>

<script>
import ViewInfo from './ViewInfo/ViewInfo'
export default {
  components:{
    ViewInfo
  },
  computed:{
    viewList() {
      return this.$store.state.rtc.viewList
    }
  },
};
</script>

<style lang="scss">
  .el-scrollbar__wrap{
    overflow-x: hidden;
  }
</style>
