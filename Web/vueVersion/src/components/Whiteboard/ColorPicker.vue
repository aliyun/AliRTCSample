<template>
  <div class="dingrtc-wb-popup__item dingrtc-wb-popup__item--fixedWith">
    <div
      class="dingrtc-wb-popup__item__colors"
      v-for="(color, index) in allWbColors"
      :key="color"
    >
      <div
        :class="{
          'dingrtc-wb-popup__item__colors__color-dot': true,
          'dingrtc-wb-popup__item__colors__color-dot--selected':
            selectedColor === color
        }"
        :style="{
          backgroundColor: color,
          boxShadow: index === allWbColors.length - 1 ? '0 0 2px #ddd' : 'none'
        }"
        @click="didSelectedColor(color)"
      />
    </div>
  </div>
</template>

<script lang="ts" setup>
import { reactive } from 'vue';
import { colors } from '../../constants/index';

const allWbColors = reactive([...colors]);

defineProps({
  selectedColor: String
});

const emit = defineEmits<{
  (e: 'selectColor', color: string): void;
}>();

const didSelectedColor = (color: string) => {
  emit('selectColor', color);
};
</script>

<style lang="less" scoped>
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
    &__colors {
      width: 25%;
      display: flex;
      justify-content: center;
      padding: 8px 0;
      &__color-dot {
        cursor: pointer;
        width: 20px;
        height: 20px;
        position: relative;
        &--selected::before {
          position: absolute;
          left: -5px;
          top: -5px;
          width: 30px;
          height: 30px;
          border: 1px solid #0899f9;
          content: '';
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
