<template>
    <el-scrollbar class="user-list" style="height:100%">
      <user-info v-for="user in userList" :key="user.displayName" :user="user"></user-info>
    </el-scrollbar>
</template>

<script>
import UserInfo from './UserInfo/UserInfo'
export default {
  components:{
    UserInfo
  },
  data() {
    return {}
  },
  computed:{
    userList() {
      return this.$store.state.rtc.userList
    }
  },
  watch: {
    // userList(n) {
    //   this.userList = this.$store.state.rtc.userList
    // }
  }
};
</script>

<style lang="scss">
  .el-scrollbar__wrap{
    overflow-x: hidden;
  }
</style>
