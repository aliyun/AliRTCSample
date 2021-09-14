<template>
  <div id="login">
    <div class="main">
      <div class="title">ELECTRON Sample <span> v {{version}}</span></div>
      <el-input v-model="channel" placeholder="请输入频道号" @keyup.native="keyboardJoin"></el-input>
      <el-input v-model="displayName" placeholder="请输入displayName" @keyup.native="keyboardJoin"></el-input>
      <el-button type="primary" :disabled="canNotJoin" @click="login">进入频道</el-button>
    </div>
  </div>
</template>

<script>
  export default {
    name: 'login',
    data() {
        return {
            version: this.$store.state.rtc.version,
            channel: this.$store.state.rtc.channel,
            displayName: this.$store.state.rtc.displayName,
        }
    },
    mounted() {
      document.title = "electron-vue " + this.version
    },
    methods: {
        cancel(){
          this.$emit("loginEvent");
        },
        keyboardJoin(event){
          if(this.canNotJoin) return;
          if (event.keyCode == 13) {
            this.login();
          }
        },
        login(){
          // 应用设置
          this.$store.commit('INIT')
          this.initSetting()
          this.$emit("loginEvent")
        },
        initSetting(){
          // 保存AppServer channel displayName
          this.$store.commit('SET_RTC_DATA', {
            channel: this.channel,
            displayName: this.displayName,
          });
        }
    },
    computed: {
      // 是否填写了频道、displayName
      canNotJoin() {
        return !( this.channel && this.displayName)
      }
    },
  }
</script>

<style lang="scss" scoped>
  /* CSS */
  #login{
      min-height: 100vh;
      position: relative;
      background-color: #006eff;
      display: flex;
      justify-content: center;
      align-items: center;
      .main{
        width: 300px;
        border-radius: 5px;
        padding: 20px;
        background: #fff;
        .title{
          text-shadow: 1px -1px #ccc;
          text-align: center;
          font-size: 20px;
          line-height: 40px;
          span{
            font-size: 16px;
          }
        }
        /deep/.el-select{
          // width: 100%;
          margin-bottom: 10px;
        }
        .el-input{
          margin-bottom: 10px;
        }
        .el-button{
          width: 100%;
        }
      }
      .demonstration{
        font-size: 14px;
        color: #8492a6;
        line-height: 44px;
      }
  }
</style>
