<template>
  <header>
    <div class="header-room">
      <span>房间号：{{channel}}</span>
    </div>
  </header>
</template>

<script>
export default {
  name: "main-header",
  computed: {
    channel() {
      return this.$store.state.rtc.channel
    },
  }
};
</script>

<style lang="scss">
  header{
    z-index: 1;
    position: absolute;
    top:0;
    left:0;
    height: 40px;
    font-size: 16px;
    text-shadow: 1px 1px 3px #fff;
    // color: #fff;
    // background: #016EFF;
    width: 100%;
    text-align: center;
    line-height: 40px;
    .header-room{
      display: flex;
      span{
        display: inline-block;
        flex: 1;
      }
    }
  }
</style>
