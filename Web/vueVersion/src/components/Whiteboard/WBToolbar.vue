<template>
  <div
    :class="{
      'dingrtc-wb-tb': true,
      'dingrtc-wb-tb--hidden': toolbarHidden,
    }"
  >
    <div :class="{ 'hide-hander': true, 'hide-hander--sticky': toolbarHidden }" @click="toggleHide">
      <Icon :type="toolbarHidden ? 'icon-rightArrow' : 'icon-leftArrow'" />
    </div>

    <!-- 选择 -->
    <div
      @click="setToolType(ToolType.Select)"
      class="dingrtc-wb-tb__item dingrtc-withtip"
      :class="{
        'dingrtc-wb-tb__item--selected': toolType === ToolType.Select,
      }"
      data-tip="图形选择"
    >
      <Icon type="icon-click" />
    </div>

    <!-- 激光笔 -->
    <div
      class="dingrtc-wb-tb__item dingrtc-withtip"
      :class="{
        'dingrtc-wb-tb__item--selected': toolType === ToolType.LaserPointer,
      }"
      @click="setToolType(ToolType.LaserPointer)"
      data-tip="激光笔"
    >
      <Icon type="icon-laser-pointer" :style="{ fontSize: '20px' }" />
    </div>

    <!-- 画笔 -->
    <a-popover placement="right" trigger="click">
      <template #content>
        <div class="dingrtc-wb-popup">
          <div class="dingrtc-wb-popup__item">
            <span class="dingrtc-wb-popup__item__label">线宽：</span>
            <a-slider
              :style="{ flex: '1', width: '140px' }"
              v-model="penWidth"
              :step="1"
              :min="1"
              :max="20"
              @change="handlerLineWidthChange"
            />
          </div>
          <div class="dingrtc-wb-popup__item">
            <ToolColors :selectedColor="strokeStyle" @selectColor="didSelectedColor" />
          </div>
        </div>
      </template>
      <div
        class="dingrtc-wb-tb__item dingrtc-withtip"
        :class="{
          'dingrtc-wb-tb__item--selected': toolType === ToolType.Pen,
        }"
        data-tip="画笔"
        @click="setAsPen"
      >
        <Icon type="icon-pen" />
        <div
          :class="{
            'dingrtc-wb-tb__item__triangle--selected': toolType === ToolType.Pen,
            'dingrtc-wb-tb__item__triangle': toolType !== ToolType.Pen,
          }"
        />
      </div>
    </a-popover>

    <!-- 图形 -->
    <a-popover placement="right" trigger="click">
      <template #content>
        <div class="dingrtc-wb-popup">
          <div class="dingrtc-wb-popup__item dingrtc-wb-popup__item--no-flex">
            <div class="dingrtc-wb-popup__item2">
              <a-tooltip title="直线">
                <div
                  :class="{
                    'dingrtc-wb-popup__item__select': true,
                    'dingrtc-wb-popup__item__select--selected': toolType === ToolType.Line,
                  }"
                  @click="setAsPolygon(ToolType.Line, ShapeFillType.none)"
                >
                  <Icon type="icon-minus" />
                </div>
              </a-tooltip>

              <a-tooltip title="箭头">
                <div
                  :class="{
                    'dingrtc-wb-popup__item__select': true,
                    'dingrtc-wb-popup__item__select--selected': toolType === ToolType.Arrow,
                  }"
                  @click="setAsPolygon(ToolType.Arrow, ShapeFillType.none)"
                >
                  <Icon type="icon-arrow-left2" />
                </div>
              </a-tooltip>

              <a-tooltip title="椭圆, 按住shift可绘制正圆">
                <div
                  :class="{
                    'dingrtc-wb-popup__item__select': true,
                    'dingrtc-wb-popup__item__select--selected':
                      fillType === 'none' && toolType === ToolType.Ellipse,
                  }"
                  @click="setAsPolygon(ToolType.Ellipse, ShapeFillType.none)"
                >
                  <Icon type="icon-radio-unchecked" />
                </div>
              </a-tooltip>

              <a-tooltip title="矩形, 按住shift可绘制正方形">
                <div
                  :class="{
                    'dingrtc-wb-popup__item__select': true,
                    'dingrtc-wb-popup__item__select--selected':
                      fillType === 'none' && toolType === ToolType.Rect,
                  }"
                  @click="setAsPolygon(ToolType.Rect, ShapeFillType.none)"
                >
                  <Icon type="icon-checkbox-unchecked" />
                </div>
              </a-tooltip>
            </div>
            <div class="dingrtc-wb-popup__item2">
              <a-tooltip title="椭圆, 按住shift可绘制正圆">
                <div
                  :class="{
                    'dingrtc-wb-popup__item__select': true,
                    'dingrtc-wb-popup__item__select--selected':
                      fillType === 'color' && toolType === ToolType.Ellipse,
                  }"
                  @click="setAsPolygon(ToolType.Ellipse, ShapeFillType.color)"
                >
                  <Icon type="icon-circle-h" />
                </div>
              </a-tooltip>
              <a-tooltip title="矩形, 按住shift可绘制正方形">
                <div
                  :class="{
                    'dingrtc-wb-popup__item__select': true,
                    'dingrtc-wb-popup__item__select--selected':
                      fillType === 'color' && toolType === ToolType.Rect,
                  }"
                  @click="setAsPolygon(ToolType.Rect, ShapeFillType.color)"
                >
                  <Icon type="icon-rect-h" />
                </div>
              </a-tooltip>
            </div>
          </div>
          <div class="dingrtc-wb-popup__item">
            <span class="dingrtc-wb-popup__item__label">线宽: </span>
            <a-slider
              :style="{ flex: '1', maxWidth: '140px' }"
              v-model="shapeWidth"
              :step="1"
              :min="1"
              :max="20"
              @change="handlerLineWidthChange"
            />
          </div>
          <ToolColors :selectedColor="strokeStyle" @selectColor="didSelectedColor" />
        </div>
      </template>
      <div
        :class="{
          'dingrtc-withtip': true,
          'dingrtc-wb-tb__item': true,
          'dingrtc-wb-tb__item--selected': isShapeSelected,
        }"
        data-tip="图形"
        @click="setAsPolygon(ToolType.Line, ShapeFillType.none)"
      >
        <Icon type="icon-solid" />
        <div
          :class="
            isShapeSelected ? 'dingrtc-wb-tb__item__triangle--selected' : 'dingrtc-wb-tb__item__triangle'
          "
        />
      </div>
    </a-popover>

    <!-- 图章 -->
    <Stamps :selected="toolType === ToolType.Stamp" @select="setToolType(ToolType.Stamp)" :whiteboard="whiteboard">
      <div
        :class="{
          'dingrtc-withtip': true,
          'dingrtc-wb-tb__item': true,
          'dingrtc-wb-tb__item--selected': isStampSelected,
        }"
        data-tip="图章"
      >
        <Icon type="icon-stamp" />
        <div
          :class="
            isStampSelected ? 'dingrtc-wb-tb__item__triangle--selected' : 'dingrtc-wb-tb__item__triangle'
          "
        />
      </div>
    </Stamps>

    <!-- 文本 -->
    <a-popover placement="right" trigger="click">
      <template #content>
        <div class="dingrtc-wb-popup">
          <div class="dingrtc-wb-popup__item">
            <a-tooltip title="加粗">
              <div
                :class="{
                  'dingrtc-wb-popup__item__select': true,
                  'dingrtc-wb-popup__item__select--selected': bold,
                }"
                @click="toggleFontBold"
              >
                <Icon type="icon-bold" />
              </div>
            </a-tooltip>
            <a-tooltip title="斜体">
              <div
                :class="{
                  'dingrtc-wb-popup__item__select': true,
                  'dingrtc-wb-popup__item__select--selected': italic,
                }"
                @click="toggleItalics"
              >
                <Icon type="icon-italic" />
              </div>
            </a-tooltip>
          </div>
          <div class="dingrtc-wb-popup__item">
            <span class="dingrtc-wb-popup__item__label">字号：</span>
            <a-slider
              :style="{ flex: '1', width: '140px' }"
              v-model:value="fontSize"
              :step="2"
              :min="12"
              :max="60"
              @change="handlerFontSizeChange"
            />
          </div>
          <ToolColors :selectedColor="strokeStyle" @selectColor="didSelectedColor" />
        </div>
      </template>
      <div
        :class="{
          'dingrtc-withtip': true,
          'dingrtc-wb-tb__item': true,
          'dingrtc-wb-tb__item--selected': toolType === ToolType.Text,
        }"
        data-tip="文本"
        @click="setToolType(ToolType.Text)"
      >
        <Icon type="icon-text" />
        <div
          :class="
            toolType === ToolType.Text
              ? 'dingrtc-wb-tb__item__triangle--selected'
              : 'dingrtc-wb-tb__item__triangle'
          "
        />
      </div>
    </a-popover>

    <!-- 橡皮擦 -->
    <a-popover placement="right" trigger="click">
      <template #content>
        <div class="dingrtc-wb-popup" :style="{ width: 'auto' }">
          <div class="dingrtc-wb-popup__item">
            <a-tooltip title="擦除涂抹到的轨迹（类似橡皮擦）">
              <div
                :class="{
                  'dingrtc-wb-popup__item__select': true,
                  'dingrtc-wb-popup__item__select--selected': toolType === ToolType.Eraser,
                }"
                @click="setAsEraser()"
              >
                <Icon type="icon-brush" />
              </div>
            </a-tooltip>
            <a-tooltip title="删除与擦除轨迹有交汇的所有内容">
              <div
                :class="{
                  'dingrtc-wb-popup__item__select': true,
                  'dingrtc-wb-popup__item__select--selected': toolType === ToolType.Delete,
                }"
                @click="setToolType(ToolType.Delete)"
              >
                <Icon type="icon-delete" />
              </div>
            </a-tooltip>
          </div>
          <div class="dingrtc-wb-popup__item">
            <span>大小：</span>
            <a-slider
              :style="{ flex: '1', maxWidth: '140px', minWidth: '100px' }"
              v-model:value="earserWidth"
              :step="1"
              :min="10"
              :max="100"
              @change="handlerLineWidthChange"
            />
          </div>
          <div class="dingrtc-wb-popup__item" :style="{ display: 'block' }">
            <a-button
              size="small"
              type="text"
              :style="{ display: 'block', marginBottom: '5px' }"
              @click="clearContents(false, WBClearType.All)"
            >
              清除所有内容
            </a-button>
            <a-button
              type="text"
              size="small"
              :style="{ display: 'block', marginBottom: '5px' }"
              @click="clearContents(true, WBClearType.All)"
            >
              清除当前页
            </a-button>
            <a-button
              type="text"
              size="small"
              :style="{ display: 'block', marginBottom: '5px' }"
              @click="clearMyContents(WBClearType.DRAWS, true)"
            >
              清除我的笔迹（当前页）
            </a-button>
            <a-button
              type="text"
              size="small"
              :style="{ display: 'block', marginBottom: '5px' }"
              @click="clearMyContents(WBClearType.DRAWS, false)"
            >
              清除我的笔迹（所有）
            </a-button>
            <a-button
              type="text"
              size="small"
              :style="{ display: 'block', marginBottom: '5px' }"
              @click="clearMyContents(WBClearType.BACKGROUND_IMAGE, true)"
            >
              清除我的背景图（当前页）
            </a-button>
            <a-button
              type="text"
              size="small"
              :style="{ display: 'block', marginBottom: '5px' }"
              @click="clearMyContents(WBClearType.BACKGROUND_IMAGE, false)"
            >
              清除我的背景图（所有）
            </a-button>
          </div>
        </div>
      </template>
      <div
        :class="{
          'dingrtc-withtip': true,
          'dingrtc-wb-tb__item': true,
          'dingrtc-wb-tb__item--selected': isEraseSelected,
        }"
        data-tip="删除"
        @click="setToolType(ToolType.Delete)"
      >
        <Icon type="icon-eraser" />
        <div
          :class="
            isEraseSelected ? 'dingrtc-wb-tb__item__triangle--selected' : 'dingrtc-wb-tb__item__triangle'
          "
        />
      </div>
    </a-popover>

    <!-- 图片 -->
    <ImageTool :whiteboard="whiteboard" >
      <div class="dingrtc-wb-tb__item dingrtc-withtip" data-tip="图片">
        <Icon type="icon-images" />
        <div class="dingrtc-wb-tb__item__triangle" />
      </div>
    </ImageTool>

    <!-- 文档 -->
    <DocTool :whiteboard="whiteboard" >
      <div class="dingrtc-wb-tb__item dingrtc-withtip" data-tip="文档">
        <Icon type="icon-docs" />
        <div class="dingrtc-wb-tb__item__triangle" />
      </div>
    </DocTool>

    <!-- 撤销 -->
    <div @click="undo" class="dingrtc-wb-tb__item dingrtc-withtip" data-tip="撤销">
      <Icon type="icon-undo" />
    </div>

    <!-- 重做 -->
    <div @click="redo" class="dingrtc-wb-tb__item dingrtc-withtip" data-tip="重做">
      <Icon type="icon-redo" />
    </div>

    <!-- 截图 -->
    <a-popover placement="right" trigger="click">
      <template #content>
        <div class="dingrtc-wb-popup">
          <a-button size="small" :loading="snapshotingAll" @click="snapshot('all')">
            全部内容
          </a-button>
          <a-button
            size="small"
            style="margin-left: 10px"
            :loading="snapshotingView"
            @click="snapshot('view')"
          >
            可见区域
          </a-button>
        </div>
      </template>
      <div class="dingrtc-wb-tb__item dingrtc-withtip" data-tip="截图">
        <Icon type="icon-snapshot" />
      </div>
    </a-popover>

    <!-- 设置 -->
    <Settings :whiteboard="whiteboard" >
      <div class="dingrtc-wb-tb__item dingrtc-withtip" data-tip=" 设置">
        <Icon type="icon-setting-fill" />
        <div class="dingrtc-wb-tb__item__triangle" />
      </div>
    </Settings>
  </div>
</template>

<script lang="ts" setup>
import { ref, computed, onMounted } from 'vue';
import DocTool from './DocTool.vue';
import Icon from '~/components/Icon';
import ImageTool from './ImageTool.vue';
import Settings from './Settings.vue';
import Stamps from './Stamps.vue';
import { ToolType, WBClearType, ShapeFillType } from '@dingrtc/whiteboard';
import { randomWbColor } from '../../constants/index';
import ToolColors from './ColorPicker.vue';
import { RtcWhiteboard } from '@dingrtc/whiteboard';

interface IProps {
  whiteboard: RtcWhiteboard;
}
const props = defineProps<IProps>();
const fontSize = ref(24);
const bold = ref(props.whiteboard.bold);
const italic = ref(props.whiteboard.italic);
const penWidth = ref(props.whiteboard.lineWidth);
const shapeWidth = ref(3);
const earserWidth = ref(50);
const strokeStyle = ref(randomWbColor());
const toolType = ref(props.whiteboard.getToolType());
const fillType = ref(props.whiteboard.fillType);
const snapshotingAll = ref(false);
const snapshotingView = ref(false);
const toolbarHidden = ref(false);

const isShapeSelected = computed(() => {
  return (
    (fillType.value === 'none' &&
      (toolType.value === ToolType.Line ||
        toolType.value === ToolType.Arrow ||
        toolType.value === ToolType.Rect ||
        toolType.value === ToolType.Ellipse)) ||
    (fillType.value === 'color' &&
      (toolType.value === ToolType.Rect || toolType.value === ToolType.Ellipse))
  );
});

const isEraseSelected = computed(() => {
  return toolType.value === ToolType.Delete || toolType.value === ToolType.Eraser;
});

const isStampSelected = computed(() => {
  return toolType.value === ToolType.Stamp;
});

const toggleHide = () => {
  toolbarHidden.value = !toolbarHidden.value;
};

const setToolType = (type: ToolType) => {
  props.whiteboard.setToolType(type);
  toolType.value = type;
};

const setAsEraser = () => {
  props.whiteboard.lineWidth = earserWidth.value;
  setToolType(ToolType.Eraser);
};

const setAsPen = () => {
  props.whiteboard.lineWidth = penWidth.value;
  setToolType(ToolType.Pen);
};

const setAsPolygon = (type: ToolType, nFillType: ShapeFillType) => {
  props.whiteboard.lineWidth = shapeWidth.value;
  setToolType(type);
  fillType.value = nFillType;
  props.whiteboard.fillType = nFillType;
  if (props.whiteboard.selectedShape) {
    props.whiteboard.setSelectedShapeStyle({
      strokeStyle: strokeStyle.value,
      fillStyle: strokeStyle.value,
      fillType: nFillType,
    });
  }
};

const handlerLineWidthChange = (nlineWidth: number) => {
  if (nlineWidth > 0) {
    if (props.whiteboard.selectedShape) {
      props.whiteboard.setSelectedShapeStyle({
        lineWidth: nlineWidth,
      });
    } else {
      props.whiteboard.lineWidth = nlineWidth;
    }
    if (toolType.value === ToolType.Pen) {
      penWidth.value = nlineWidth;
    } else if (toolType.value === ToolType.Eraser) {
      earserWidth.value = nlineWidth;
    } else {
      shapeWidth.value = nlineWidth;
    }
  }
};

const handlerFontSizeChange = (nfontSize: number) => {
  if (props.whiteboard.selectedShape) {
    props.whiteboard.setSelectedShapeFontStyle({
      fontSize: nfontSize,
    });
  } else {
    props.whiteboard.fontSize = nfontSize;
  }
  fontSize.value = nfontSize;
};

const toggleItalics = () => {
  if (props.whiteboard.selectedShape) {
    props.whiteboard.setSelectedShapeFontStyle({
      italic: !italic.value,
    });
  } else {
    props.whiteboard.italic = !italic.value;
  }
  italic.value = !italic.value;
};

const toggleFontBold = () => {
  if (props.whiteboard.selectedShape) {
    props.whiteboard.setSelectedShapeFontStyle({
      bold: !bold.value,
    });
  } else {
    props.whiteboard.bold = !bold.value;
  }
  bold.value = !bold.value;
};

const didSelectedColor = (color: string) => {
  if (props.whiteboard.selectedShape) {
    props.whiteboard.setSelectedShapeStyle({
      strokeStyle: color,
      fillStyle: color,
    });
  }
  props.whiteboard.fillStyle = color;
  props.whiteboard.strokeStyle = color;
  strokeStyle.value = color;
};

const snapshot = async (mode: 'all' | 'view') => {
  mode === 'all' ? (snapshotingAll.value = true) : (snapshotingView.value = true);
  await props.whiteboard.snapshot(true, mode);
  snapshotingAll.value = false;
  snapshotingView.value = false;
};

const clearContents = (curPage: boolean, type: WBClearType) => {
  props.whiteboard.clearContents(curPage ? curPage : false, type);
};

const clearMyContents = (type: WBClearType, curPage: boolean) => {
  props.whiteboard.clearUserContents(props.whiteboard.userId, !!curPage, type);
};

const undo = () => {
  props.whiteboard.undo();
};

const redo = () => {
  props.whiteboard.redo();
};

onMounted(() => {
  props.whiteboard.enableCursorSync();
  didSelectedColor(strokeStyle.value);
  setToolType(ToolType.Pen);
});
</script>

<style lang="less" scoped>
.dingrtc-wb-tb {
  border-radius: 5px;
  font-size: 12px;
  padding: 8px;
  position: absolute;
  z-index: 10;
  left: 10px;
  top: 50%;
  transform: translateY(-50%);
  display: flex;
  flex-direction: column;
  justify-content: center;
  align-items: center;
  user-select: none;
  font-size: 12px;
  box-shadow: 2px 2px 8px rgba(20, 14, 14, 0.2);
  background-color: #fff;
  pointer-events: all;
  transition: all 0.3s ease-in-out;
  &--hidden {
    box-shadow: none;
    left: -45px;
  }
  .hide-hander {
    position: absolute;
    right: -21px;
    top: calc(50% - 22px);
    width: 20px;
    height: 25px;
    border-top-right-radius: 50%;
    border-bottom-right-radius: 50%;
    display: flex;
    align-items: center;
    justify-content: center;
    box-shadow: 2px 2px 8px rgba(20, 14, 14, 0.2);
    z-index: 11;
    background-color: #fff;
    cursor: pointer;
    transition: all 0.3s ease-in-out;
  }
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
    > .iconfont {
      font-size: 18px;
    }
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
    &__color-dot {
      position: absolute;
      right: 0;
      bottom: 3px;
      width: 6px;
      height: 6px;
      border-radius: 6px;
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

.dingrtc-withtip {
  position: relative;
  &::after {
    content: attr(data-tip);
    position: absolute;
    // top:-30px;
    // right: -40px;
    // width: 100px;
    min-width: 80px;
    background-color: black;
    color: #fff;
    text-align: center;
    border-radius: 6px;
    padding: 5px;
    z-index: 100;
    transform: translateX(calc(90% - 10px));
    font-size: 12px;
    opacity: 0;
    transition: all 0.3s ease-in-out;
    display: none;
  }
  &:hover::after {
    display: block;
    opacity: 1;
    transform: translateX(90%);
  }
}
.dingrtc-withtip-top {
  &::after {
    transform: translate(-50%, -30px);
    left: 50%;
  }
  &:hover::after {
    transform: translate(-50%, -40px);
  }
}

.dingrtc-wb-popup {
  font-size: 12px;
  button:hover {
    background-color: #eee;
  }
  &__item {
    display: flex;
    align-items: center;
    flex-wrap: wrap;
    button {
      font-size: 12px !important;
      border: none;
    }
    &__label {
      font-size: 12px;
      padding-right: 5px;
    }
    &__select {
      font-size: 18px;
      display: inline-flex;
      width: 40px;
      height: 36px;
      padding: 8px 0;
      justify-content: center;
      align-items: center;
      border-radius: 3px;
      cursor: pointer;
      color: #333;
      position: relative;
      &:hover {
        background-color: lightgray;
      }
      &--selected {
        color: #0899f9;
        &:hover {
          background-color: lightgray;
        }
      }
    }
    // &__select ~ &__select {
    //   margin-left: 5px;
    // }
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

.toolbar-instruction {
  list-style: none;
  max-width: 350px;
  padding: 10px 10px;
  color: #555;
  font-size: 12px;
  &__item {
    display: flex;
    &-title {
      min-width: 60px;
    }
  }
  &__item ~ &__item {
    margin-top: 10px;
  }
}
</style>
