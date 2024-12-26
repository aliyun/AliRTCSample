import { Popover } from 'dingtalk-design-desktop';
import styles from '../../index.module.less';
import { DisconnectNetwork } from './DisconnectNetwork';
import { NetworkScore } from './NormalNetwork';
import { useLocalChannel, useNetworkStats } from '~/hooks/channel';

const NetworkCardContent = () => {
  const { rtcStats } = useNetworkStats();
  return (
    <div className={styles.cardWrapper}>
      <div className={styles.labelWrap}>
        <span className={styles.label}>{'码率'}</span>
        <span className={styles.label}>{'延迟'}</span>
        {/* <span className={styles.label}>{'丢包率'}</span> */}
      </div>
      <div className={styles.valueWrapper}>
        <div className={styles.value}>
          <span style={{ marginRight: '0.1rem' }}>
            &uarr; {rtcStats.localBitrate ? `${Math.round(rtcStats.localBitrate / 1000)} kbps` : '--'}
          </span>{' '}
          &darr; {rtcStats.remoteBitrate ? `${Math.round(rtcStats.remoteBitrate / 1000)} kbps` : '--'}
        </div>
        <div className={styles.value}>
          {rtcStats.rtt ? `${Math.round(rtcStats.rtt)} ms` : '--'}
        </div>
        {/* <div className={styles.value}>
          {`${Math.round(rtcStats.loss)} %`}
        </div> */}
      </div>
    </div>
  );
};

NetworkCardContent.displayName = 'NetworkCardContent';

export const ScoreMap = (score: number): { text: string; icon: any } => {
  const scoreMap: { [key: number]: any } = {
    0: {
      text: '网络状态未知',
      icon: <DisconnectNetwork className={styles.networkIcon} />,
    },
    1: {
      text: '网络极佳',
      icon: <NetworkScore signalNumber={3} className={styles.networkIcon} />,
    },
    2: {
      text: '网络较好',
      icon: <NetworkScore signalNumber={3} className={styles.networkIcon} />,
    },
    3: {
      text: '网络一般',
      icon: <NetworkScore signalNumber={2} className={styles.networkIcon} />,
    },
    4: {
      text: '网络较差',
      icon: <NetworkScore signalNumber={2} className={styles.networkIcon} />,
    },
    5: {
      text: '网络极差',
      icon: <NetworkScore signalNumber={1} className={styles.networkIcon} />,
    },
    6: {
      text: '网络已断开',
      icon: <DisconnectNetwork className={styles.networkIcon} />,
    },
  };
  return scoreMap[score];
};

export const NetworkDetector = () => {
  const { networkQuality } = useLocalChannel();
  return (
    <Popover
      placement="bottomLeft"
      title={`${ScoreMap(networkQuality).text}`}
      arrow={false}
      content={<NetworkCardContent />}
      trigger="hover"
    >
      <div className={styles.networkTrigger}>{ScoreMap(networkQuality).icon}</div>
    </Popover>
  );
};
