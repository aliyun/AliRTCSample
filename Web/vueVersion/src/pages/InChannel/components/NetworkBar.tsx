import { Popover } from 'ant-design-vue';
import styles from '../index.module.less';
import Icon from '~/components/Icon';
import { useChannelInfo } from '~/store';

export const NetworkScore = ({
  className,
  signalNumber,
}) => {
  return (
    <svg
      viewBox="0 0 1024 1024"
      version="1.1"
      class={className}
      xmlns="http://www.w3.org/2000/svg"
      id="mx_n_1710492163232"
    >
      <path
        d="M176 512m64 0l32 0q64 0 64 64l0 128q0 64-64 64l-32 0q-64 0-64-64l0-128q0-64 64-64Z"
        fill={signalNumber > 0 ? 'rgba(28,163,61,0.9)' : ''}
      />
      <path
        d="M432 384m64 0l32 0q64 0 64 64l0 256q0 64-64 64l-32 0q-64 0-64-64l0-256q0-64 64-64Z"
        fill={signalNumber > 1 ? 'rgba(28,163,61,0.9)' : ''}
      />
      <path
        d="M688 256m64 0l32 0q64 0 64 64l0 384q0 64-64 64l-32 0q-64 0-64-64l0-384q0-64 64-64Z"
        fill={signalNumber > 2 ? 'rgba(28,163,61,0.9)' : ''}
      />
    </svg>
  );
};



const NetworkCardContent = () => {
  const channelInfo = useChannelInfo()
  return (
    <div class={styles.cardWrapper}>
      <div class={styles.labelWrap}>
        <span class={styles.label}>{'码率'}</span>
        <span class={styles.label}>{'延迟'}</span>
        {/* <span class={styles.label}>{'丢包率'}</span> */}
      </div>
      <div class={styles.valueWrapper}>
        <div class={styles.value}>
          <span style={{ marginRight: '0.1rem' }}>
            &uarr; {channelInfo.rtcStats.localBitrate ? `${Math.round(channelInfo.rtcStats.localBitrate / 1000)} kbps` : '--'}
          </span>{' '}
          &darr; {channelInfo.rtcStats.remoteBitrate ? `${Math.round(channelInfo.rtcStats.remoteBitrate / 1000)} kbps` : '--'}
        </div>
        <div class={styles.value}>
          {channelInfo.rtcStats.rtt ? `${Math.round(channelInfo.rtcStats.rtt)} ms` : '--'}
        </div>
        {/* <div className={styles.value}>
          {`${Math.round(rtcStats.loss)} %`}
        </div> */}
      </div>
    </div>
  );
};

export const ScoreMap = (score: number): { text: string; icon: any } => {
  const scoreMap: { [key: number]: any } = {
    0: {
      text: '网络状态未知',
      icon: <Icon type="icon-DisconnectNetwork" class={styles.disconnected} />,
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
      icon: <Icon type="icon-DisconnectNetwork" class={styles.disconnected} />,
    },
  };
  return scoreMap[score];
};

export const NetworkDetector = () => {
  const channelInfo = useChannelInfo()
  return (
    <Popover
      placement="bottomLeft"
      title={`${ScoreMap(channelInfo.networkQuality).text}`}
      arrow={false}
      content={<NetworkCardContent />}
      trigger="hover"
    >
      <div class={styles.networkTrigger}>{ScoreMap(channelInfo.networkQuality).icon}</div>
    </Popover>
  );
};
