<template>
  <div class="screen-share">
    <el-row>
      <el-col :span="8" class="demonstration">共享Type: </el-col>
      <el-col :span="16">
        <el-select v-model="sourceType">
          <el-option
            v-for="(value, key) in screenShareTypeList"
            :key="value"
            :label="key | labelFilter('AliEngineScreenShare')"
            :value="value"
          ></el-option>
        </el-select>
      </el-col>
    </el-row>
    <el-row>
      <el-col :span="8" class="demonstration">共享Source: </el-col>
      <el-col :span="16">
        <el-select v-model="sourceId">
          <el-option
            v-for="(value, key) in sourceList"
            :key="key"
            :label="value.sourceTitle"
            :value="value.sourceId"
          ></el-option>
        </el-select>
      </el-col>
    </el-row>
    <div class="btn-list">
      <el-button @click="cancel">取消</el-button>
      <el-button type="primary" @click="startScreenShare">确定</el-button>
    </div>
  </div>
</template>

<script>
import {AliEngineScreenShareType} from '../../data/main'
export default {
  data() {
    return {
      sourceType: AliEngineScreenShareType.AliEngineScreenShareDesktop,
      screenShareTypeList: AliEngineScreenShareType,
      sourceId: "",
      sourceList: [],
    };
  },
  mounted() {
    this.$nextTick(() => {
      this.sourceType = aliElectronRtc.getCurrentScreenShareSourceType()
      aliElectronRtc.getScreenShareSourceInfo(this.sourceType).then((sourceInfo)=>{
        this.sourceList = sourceInfo
        this.sourceId = aliElectronRtc.getCurrentScreenShareSourceId()
      })
    });
  },
  methods: {
    startScreenShare() {
      let config = {
        isShareByRegion: false,
        shareRegion: {
          originX: 0,
          originY: 0,
          width: 0,
          height: 0
        }
      }
      if(this.sourceType === AliEngineScreenShareType.AliEngineScreenShareDesktop) {
        aliElectronRtc.startScreenShareByDesktopId(this.sourceId, config)
      }else if(this.sourceType === AliEngineScreenShareType.AliEngineScreenShareWindow) {
        aliElectronRtc.startScreenShareByWindowId(this.sourceId, config)
      }
    },
    cancel() {
      this.$emit("close")
    }
  },
  watch: {
    sourceType(){
      aliElectronRtc.getScreenShareSourceInfo(this.sourceType).then((sourceInfo)=>{
        this.sourceList = sourceInfo
        this.sourceId = this.sourceList[0].sourceId
      })
    },
  }
};
</script>

<style lang="scss" scoped>
  .screen-share{
    .el-row{
      margin-bottom: 10px;
    }
    .btn-list{
      display: flex;
      justify-content: center;
      .el-button{
        width: 30%;
      }
    }
    .origin{
      line-height: 28px;
      text-align: right;
      padding-right: 3px;
    }
  }
</style>