<template>
  <div class="dingrtc-wb-pages">
    <div class="dingrtc-wb-pages__wrapper">
      <div class="dingrtc-wb-pages__wrapper__docs">
        <a-select
          ref="docsSelectRef"
          :value="activeDocId"
          style="width: 200px"
          @change="handleSelectChange"
        >
          <a-select-option v-for="(doc, index) in docs" :key="doc.docId" :value="doc.docId">
            <div class="wb-doc">
              <span class="wb-doc-name">{{ index === 0 ? '默认白板' : doc.name }}</span>
              <Icon v-if="index > 0" type="doc-delete_doc" @click="deleteDoc(doc.docId)"> </Icon>
            </div>
          </a-select-option>
        </a-select>
      </div>

      <div class="dingrtc-wb-pages__item">
        <button
          v-if="isDefaultWhiteboard"
          class="dingrtc-withtip dingrtc-withtip-top dingrtc-wb-pages__btn"
          data-tip="添加一页"
          @click="addPage"
        >
          <Icon type="icon-plus" :style="{ fontSize: '10px' }" />
        </button>

        <button
          class="dingrtc-withtip dingrtc-withtip-top dingrtc-wb-pages__btn"
          data-tip="前一页"
          :style="{ paddingLeft: '6px' }"
          @click="updatePageIndex(pageIndex - 1)"
        >
          <Icon type="icon-angle-double-left" :style="{ fontSize: '16px' }" />
        </button>
        <div class="dingrtc-wb-pages__page-count">
          {{ `${pageIndex + 1} / ${pageCount}` }}
        </div>

        <button
          class="dingrtc-withtip dingrtc-withtip-top dingrtc-wb-pages__btn"
          data-tip="后一页"
          @click="updatePageIndex(pageIndex + 1)"
        >
          <Icon type="icon-angle-double-right" :style="{ fontSize: '16px' }" />
        </button>
        <button
          v-if="isDefaultWhiteboard"
          class="dingrtc-withtip dingrtc-withtip-top dingrtc-wb-pages__btn"
          data-tip="删除当前页"
          @click="removePage"
        >
          <Icon type="icon-minus" :style="{ fontSize: '10px' }" />
        </button>
      </div>
      <div class="dingrtc-wb-pages__item">
        <a-popover destroyTooltipOnHide placement="top" trigger="click">
          <template #content>
            <div class="dingrtc-wb-pages__slider">
              <a-slider
                :min="10"
                :max="500"
                vertical
                tooltipOpen
                :value="scale * 100"
                @change="updateScale"
              />
            </div>
          </template>
          <div class="dingrtc-wb-pages__zoom-rate">
            {{ `${Math.round(scale * 100)}%` }}
          </div>
        </a-popover>
      </div>
    </div>
  </div>
</template>

<script lang="ts" setup>
import { ref, computed, onMounted, onBeforeUnmount } from 'vue';
import { message } from 'ant-design-vue';
import Icon from '~/components/Icon';
import { RtcWhiteboard } from '@dingrtc/whiteboard';

interface IProps {
  activeDocId: string;
  whiteboard: RtcWhiteboard
}

const props = defineProps<IProps>();


const pageCount = ref(props.whiteboard.getTotalNumberOfPages());
const pageIndex = ref(props.whiteboard.getCurrentPageNumber());
const scale = ref(props.whiteboard.scale);
const docs = ref(props.whiteboard.enumerateDocs());

const isDefaultWhiteboard = computed(() => props.activeDocId === 'default');


function addPage() {
  props.whiteboard.addPage();
}

function removePage() {
  props.whiteboard.removePage();
}

function updatePageIndex(pageIndex: number) {
  if (pageIndex < 0 || pageIndex >= props.whiteboard.getTotalNumberOfPages()) return;
  props.whiteboard.gotoPage(pageIndex);
}

function updateScale(nextScale: number) {
  nextScale /= 100;
  if (nextScale > 0.1 && nextScale < 5) {
    props.whiteboard.setScale(nextScale);
    scale.value = nextScale;
  }
}

function onPageChange() {
  pageCount.value = props.whiteboard.getTotalNumberOfPages();
  pageIndex.value = props.whiteboard.getCurrentPageNumber();
}

function onDocSwitched() {
  message.info(`已切换到${props.whiteboard.activeDoc.name || props.whiteboard.activeDoc.docId}`);
}

function onViewportUpdate() {
  scale.value = props.whiteboard.scale;
}

const onDocChanged = () => {
  docs.value = props.whiteboard.enumerateDocs();
};

function gotoPage(index: number) {
  props.whiteboard.gotoPage(index);
}

function addWbListener() {
  props.whiteboard.on('page-index-changed', onPageChange);
  props.whiteboard.on('viewport-updated', onViewportUpdate);
  props.whiteboard.on('doc-created', onDocChanged);
  props.whiteboard.on('doc-switched', onDocSwitched);
  props.whiteboard.on('doc-deleted', onDocChanged);
  props.whiteboard.on('doc-updated', onDocChanged);
}

function removeWbListener() {
  props.whiteboard.off('page-index-changed', onPageChange);
  props.whiteboard.off('viewport-updated', onViewportUpdate);
  props.whiteboard.off('doc-created', onDocChanged);
  props.whiteboard.off('doc-switched', onDocSwitched);
  props.whiteboard.off('doc-deleted', onDocChanged);
  props.whiteboard.off('doc-updated', onDocChanged);
}

const handleSelectChange = (docId: string) => {
  props.whiteboard.switchToDoc(docId);
};

const deleteDoc = (docId: string) => {
  props.whiteboard.deleteDoc(docId);
};

onMounted(() => {
  addWbListener();
  onDocChanged();
});

onBeforeUnmount(() => {
  removeWbListener();
});
</script>

<style lang="less" scoped>
.dingrtc-wb-pages {
  pointer-events: all;
  position: absolute;
  z-index: 12;
  padding: 5px;
  right: 20px;
  bottom: 20px;
  font-size: 14px;
  color: #2b3139;
  &__zoom-rate {
    font-size: 12px;
    user-select: none;
    width: 40px;
    text-align: center;
  }
  &__wrapper {
    width: 100%;
    display: flex;
    justify-content: center;
    align-items: center;
    &__docs {
      margin-right: 8px;
    }
  }
  &__wrapper ~ &__wrapper {
    margin-top: 4px;
  }
  &__page-count {
    text-align: center;
    user-select: none;
  }
  &__btn,
  &__btn-add {
    border: none;
    background: transparent;
    outline: none;
    padding: 5px 8px;
    cursor: pointer;
    display: flex;
    align-items: center;
    &--disabled {
      cursor: not-allowed;
      svg {
        fill: #c0c4cc;
      }
    }
  }
  &__item {
    display: flex;
    justify-content: center;
    align-items: center;
    height: 32px;
    padding-left: 5px;
    background-color: #fff;
    border-radius: 5px;
    box-shadow: 0 0 8px rgba(0, 0, 0, 0.1);
    padding: 0 12px;
  }
  &__item ~ &__item {
    margin-left: 8px;
  }
  &__slider {
    height: 120px;
    margin: 0 -5px;
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

.wb-courseware-alide-mask {
  position: fixed;
  top: 0;
  right: 0;
  left: 0;
  opacity: 0;
  bottom: 0;
  transition: all 0.3s ease-in-out;
  z-index: 99;
}

.wb-courseware-alide {
  position: fixed;
  top: 0;
  right: 0;
  width: 260px;
  background-color: #f9f9f9;
  bottom: 0;
  box-shadow: rgba(0, 0, 0, 0.15) 0px 8px 24px 0px;
  transition: all 0.3s ease-in-out;
  z-index: 100;
  user-select: none;
  &__hiden {
    right: -280px;
  }
  header {
    height: 60px;
    display: flex;
    justify-content: space-between;
    font-size: 15px;
    padding: 20px 10px 0;
    span {
      font-weight: 500;
    }
    i {
      cursor: pointer;
    }
  }
  .wb-courseware-list {
    overflow-y: auto;
    height: calc(100% - 60px);
    .courseware-preview {
      margin: 20px 0;
      img {
        width: 230px;
        height: auto;
        border: 1px solid #fff;
        border-radius: 5px;
        margin: 0 auto;
        display: block;
        cursor: pointer;
      }
      img.courseware-preview__active {
        border-color: #4091f7;
      }
      .courseware-index {
        display: flex;
        justify-content: space-between;
        padding: 0 15px;
        margin-top: 5px;
        color: #777;
      }

      i {
        cursor: pointer;
      }
    }
  }
}

:global(.wb-doc) {
  display: flex;
  align-items: center;
  justify-content: space-between;
}
:global(.wb-doc-name) {
  width: calc(100% - 20px);
  overflow: hidden;
  text-overflow: ellipsis;
  white-space: nowrap;
}

:global(.doc-delete) {
  font-size: 14px;
  color: #afafaf;
}
</style>
