import { onMounted, onUnmounted } from 'vue';
import { useChannelInfo, useClient } from '~/store';
import { isIOS, logger } from '~/utils/tools';
import { useDevice } from './device';

// 过滤瞬时切换，只在真正切到后台一段时间后才重建
const HIDDEN_THRESHOLD = 1000;

interface MicIntentSnapshot {
  hadMic: boolean;
  wasEnabled: boolean;
  wasPublished: boolean;
}

/**
 * iOS 切后台会中断 AudioSession，回前台后原麦克风轨可能只产出静音包，
 * 而 SDK 的 setEnabled/setDevice 在采集失败时会静默降级成无声合成轨，无法自愈。
 * 这里在回到前台时无条件重走一遍「打开麦克风 → 获取音频流 → 推流」。
 */
export const useMicBackgroundRecovery = () => {
  if (!isIOS()) return;

  const channelInfo = useChannelInfo();
  const client = useClient();
  const { rebuildMicTrack } = useDevice('in');

  let hiddenAt = 0;
  let snapshot: MicIntentSnapshot | null = null;

  const onVisibilityChange = () => {
    if (document.visibilityState === 'hidden') {
      hiddenAt = performance.now();
      const { micTrack } = channelInfo;
      // iOS 可能在后台期间就通过 track-ended 把 micTrack 置空，
      // 回前台时已无从推断用户原本是否开着麦，因此切走前先快照
      snapshot = {
        hadMic: !!micTrack,
        wasEnabled: !!micTrack?.enabled,
        wasPublished: !!micTrack && channelInfo.publishedTracks.has(micTrack.getTrackId()),
      };
      return;
    }

    const intent = snapshot;
    snapshot = null;
    if (!intent) return;

    const hiddenDuration = performance.now() - hiddenAt;
    if (hiddenDuration <= HIDDEN_THRESHOLD) return;
    if (!intent.hadMic) return;
    // 用户主动关麦时设备已释放，静音是预期行为，重建反而会重新抢占麦克风
    if (!intent.wasEnabled) return;
    // 断连/离会过程中重建后 publish 必然失败
    if (client.connectionState !== 'connected') return;

    logger.info(`page visible after ${Math.round(hiddenDuration)}ms hidden, recovering micTrack`);
    rebuildMicTrack(intent.wasPublished).catch((e) => {
      logger.error('micTrack recovery failed', e);
    });
  };

  onMounted(() => {
    document.addEventListener('visibilitychange', onVisibilityChange);
  });

  onUnmounted(() => {
    document.removeEventListener('visibilitychange', onVisibilityChange);
  });
};
