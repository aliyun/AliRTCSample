<template>
  <a-popover placement="right" trigger="click">
    <template #content>
      <div class="dingrtc-wb-popup">
        <div class="dingrtc-wb-popup__item dingrtc-wb-popup__item--fixedWith">
          <div
            class="dingrtc-wb-popup__item__list"
            v-for="stamp in stamps"
            :key="stamp.id"
          >
            <div
              :class="{
                'dingrtc-wb-popup__item__list__item': true,
                'dingrtc-wb-popup__item__list__item--resizable': stamp.resizable,
                'dingrtc-wb-popup__item__list__item--selected':
                  selectedStamp === stamp.id
              }"
              @click="selectStamp(stamp.id)"
            >
              <img :src="stamp.url" :alt="stamp.id" />
            </div>
          </div>
        </div>
      </div>
    </template>
    <slot></slot>
  </a-popover>
</template>

<script lang="ts" setup>
import { ref, onMounted } from 'vue';
import { RtcWhiteboard } from '@dingrtc/whiteboard';

interface IProps {
  whiteboard: RtcWhiteboard;
}

const props = defineProps<IProps>();
defineEmits<{ (e: 'select'): void }>();

const stamps = ref([
  {
    id: 'star',
    url: 'https://img.alicdn.com/imgextra/i3/O1CN01Yxeqol2002jcyPMVv_!!6000000006786-2-tps-32-29.png',
    resizable: false
  },
  {
    id: 'cross',
    url: 'https://img.alicdn.com/imgextra/i2/O1CN01WBoIQY28QAFSRvjXG_!!6000000007926-2-tps-32-32.png',
    resizable: false
  },
  {
    id: 'heart',
    url: 'https://img.alicdn.com/imgextra/i4/O1CN01EM5zzt1DOr1DbI7xh_!!6000000000207-2-tps-32-32.png',
    resizable: false
  },
  {
    id: 'zan',
    url: 'https://img.alicdn.com/imgextra/i2/O1CN012fbuSE1xgglc5aekA_!!6000000006473-2-tps-32-32.png',
    resizable: false
  },
  {
    id: 'tick',
    url: 'https://img.alicdn.com/imgextra/i1/O1CN01pzfExM1FxK3bfznLG_!!6000000000553-2-tps-32-32.png',
    resizable: false
  },
  {
    id: 'rocket',
    url: 'https://img.alicdn.com/imgextra/i4/O1CN01rhHylr1Dlkp20GgKY_!!6000000000257-2-tps-128-128.png',
    resizable: true
  },
  {
    id: 'house',
    url: 'https://img.alicdn.com/imgextra/i2/O1CN012eL3Vy1ZCPoBnZouA_!!6000000003158-2-tps-128-128.png',
    resizable: true
  }
]);

const selectedStamp = ref('');

const selectStamp = (id: string) => {
  selectedStamp.value = id;
  props.whiteboard.setStamp(id);
};

onMounted(() => {
  stamps.value.forEach(stamp => {
    props.whiteboard.addStamp(stamp.id, stamp.url, stamp.resizable);
  });
});
</script>

<style lang="less" scoped>
.dingrtc-wb-tb {
  &__item {
    font-size: 18px;
    display: inline-flex;
    width: 30px;
    height: 30px;
    padding: 2px;
    justify-content: center;
    align-items: center;
    border-radius: 5px;
    cursor: pointer;
    margin: 1px 0;
    color: #333;
    position: relative;
    &:hover {
      background-color: lightgray;
    }
    &--selected {
      // background-color: @deep-grey;
      color: #0899f9;
      &:hover {
        background-color: lightgray;
      }
    }
    &__triangle {
      position: absolute;
      right: 2px;
      bottom: 3px;
      border: 3px solid transparent;
      border-right-color: #333;
      border-bottom-color: #333;
      &--selected {
        position: absolute;
        right: 2px;
        bottom: 3px;
        border: 3px solid transparent;
        border-right-color: #0899f9;
        border-bottom-color: #0899f9;
      }
    }
  }
}

.dingrtc-wb-popup {
  font-size: 12px;
  pointer-events: all;
  button:hover {
    background-color: #eee;
  }
  &__item {
    display: flex;
    align-items: center;
    flex-wrap: wrap;
    button {
      font-size: 12px !important;
    }
    &__label {
      font-size: 12px;
      padding-right: 5px;
    }
    &__list {
      width: 25%;
      display: flex;
      justify-content: center;
      padding: 8px 0;
      &__item {
        cursor: pointer;
        width: 30px;
        height: 30px;
        position: relative;
        display: flex;
        justify-content: center;
        align-items: center;
        border-radius: 3px;
        &--resizable {
          &::before,
          &::after {
            position: absolute;
            content: '';
            width: 0;
            height: 0;
            border: 3px solid rgba(0, 0, 0, 0.3);
          }
          &::before {
            left: 3px;
            top: 3px;
            border-bottom-color: transparent;
            border-right-color: transparent;
          }
          &::after {
            right: 3px;
            bottom: 3px;
            border-top-color: transparent;
            border-left-color: transparent;
          }
        }
        &--selected {
          box-shadow: 0 0 1px 2px rgba(0, 0, 0, 0.1);
        }
        img {
          width: 16px;
          height: 16px;
        }
      }
    }
    &--no-flex {
      display: block;
    }
    &--fixedWith {
      width: 160px;
    }
  }

  &__item ~ &__item {
    margin-top: 5px;
    position: relative;
    line-height: 32px;
    padding-top: 5px;
    &::before {
      position: absolute;
      content: '';
      width: 100%;
      height: 1px;
      background-color: #eee;
      top: 0;
    }
  }

  &__item2 {
    display: flex;
    align-items: center;
    flex-wrap: wrap;
    width: 160px;
  }
}
</style>
