import { logger } from './tools';

export const tryJsonParse = (str: string) => {
  try {
    const obj = JSON.parse(str);
    return obj;
  } catch (error) {
    logger.error('tryJsonParse error', error, str);
    return {};
  }
};

interface ILocalStoredData {
  previewCamOnOff?: 'on' | 'off';
  previewMicOnOff?: 'on' | 'off';
}

export enum ELocalStorageKeys {
  LocalStoredData = 'LocalStoredData',
}

const setItemToLocalStorage = (itemKey, itemValue): void => {
  try {
    window.localStorage?.setItem(itemKey, itemValue);
  } catch (err) {
    logger.error('FAI setItemToLocalStorage detail', err);
  }
};

const getItemFromLocalStorage = (itemKey): string => {
  try {
    const getItem = window.localStorage?.getItem(itemKey) || '';
    return getItem;
  } catch (err) {
    logger.error('FAI getItemFromLocalStorage detail', err);
    return '';
  }
};

/**
 * 本类管理所有的localstorage 设计的key和读写入口，不要单独直接使用localstorage对象和自定义key
 * 如果需要新增key，可以扩展 localStoredData
 */
class LocalStorageMgr {
  private localStoredData: ILocalStoredData = {};
  constructor() {
    this.localStoredData =
      tryJsonParse(getItemFromLocalStorage(ELocalStorageKeys.LocalStoredData)) || {};
  }

  // 保存到变量，且更新 localstorage
  updateLocalStoredData = (partialLocalData: ILocalStoredData) => {
    this.localStoredData = { ...this.localStoredData, ...partialLocalData };
    setItemToLocalStorage(ELocalStorageKeys.LocalStoredData, JSON.stringify(this.localStoredData));
  };

  getLocalStoredData = () => {
    return this.localStoredData;
  };
}

let preConfSettingsMgr: LocalStorageMgr;
export const getLocalStorageMgr = () => {
  if (!preConfSettingsMgr) {
    preConfSettingsMgr = new LocalStorageMgr();
  }

  return preConfSettingsMgr;
};
