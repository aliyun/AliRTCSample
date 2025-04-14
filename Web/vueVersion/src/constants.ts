import { isIOS } from '~/utils/tools';

export const frameRates = [5, 10, 15, 20, 30];

export const optionzationModeList = [
  {
    label: '均衡',
    value: 'balanced',
  },
  {
    label: '清晰优先',
    value: 'detail',
  },
  {
    label: '流畅优先',
    value: 'motion',
  },
];

export const videoDimensions = [
  'VD_120x120',
  'VD_160x120',
  'VD_180x180',
  'VD_240x180',
  'VD_320x180',
  'VD_240x240',
  'VD_320x240',
  'VD_424x240',
  'VD_360x360',
  'VD_480x360',
  'VD_640x360',
  'VD_480x480',
  'VD_640x480',
  'VD_840x480',
  'VD_960x540',
  'VD_960x720',
  'VD_1280x720',
  'VD_1920x1080',
  'VD_2560x1440'
];

if (isIOS()) {
  videoDimensions.pop();
}

export const DEFAULT_WHITEBOARD_ID = 'default';