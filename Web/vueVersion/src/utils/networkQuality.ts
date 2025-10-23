import DingRTC, { CameraVideoTrack, DingRTCClient, NetworkQuality, RemoteUser, TrackMediaType } from 'dingrtc';
import { getAppToken } from './request';
import configJson from '~/config.json';
import { message } from 'ant-design-vue';
import { parseSearch } from './tools';

const networkQualityMap: Record<NetworkQuality, string> = {
  0: '网络状态未知',
  1: '网络极佳',
  2: '网络较好',
  3: '网络一般',
  4: '网络较差',
  5: '网络极差',
  6: '网络已断开',
};
const appId = parseSearch('appId') || configJson.appId || '';
const channel = `${Math.ceil(Math.random() * 10000)}`;
const userIdA = `${Math.ceil(Math.random() * 10000)}`;
const userNameA = `Web-${Math.ceil(Math.random() * 100)}`;
const userIdB = `${Math.ceil(Math.random() * 10000)}`;
const userNameB = `Web-${Math.ceil(Math.random() * 100)}`;
let uplinkClient: DingRTCClient;
let downlinkClient: DingRTCClient;
let cameraTrack: CameraVideoTrack = null;
const testResult = {
  // 记录上行网络质量数据
  uplinkNetworkQualities: [] as number[],
  // 记录下行网络质量数据
  downlinkNetworkQualities: [] as number[],
  average: {
    uplinkNetworkQuality: 0 as NetworkQuality,
    downlinkNetworkQuality: 0 as NetworkQuality,
  },
};
// 1. 检测上行网络质量
async function testUplinkNetworkQuality() {
  cameraTrack = await DingRTC.createCameraVideoTrack({ dimension: 'VD_640x480' });
  uplinkClient.on('network-quality', (uplinkNetworkQuality: NetworkQuality) => {
    testResult.uplinkNetworkQualities.push(uplinkNetworkQuality);
  });
  // 加入用于测试的房间，房间号需要随机，避免冲突
  const { gslb, token } = await getAppToken(userIdA, appId, channel);
  DingRTC.setClientConfig({ gslb });
  await uplinkClient.join({
    appId,
    channel,
    uid: userIdA,
    userName: userNameA,
    token,
  });
  await uplinkClient.publish([cameraTrack]);
}
// 2. 检测下行网络质量
async function testDownlinkNetworkQuality() {
  // 加入用于测试的房间，房间号需要随机，避免冲突
  const { gslb, token } = await getAppToken(userIdB, appId, channel);
  DingRTC.setClientConfig({ gslb });
  await downlinkClient.join({
    appId,
    channel,
    uid: userIdB,
    userName: userNameB,
    token,
  });
  downlinkClient.on('user-published', async (user: RemoteUser, media: TrackMediaType) => {
    if (media === 'video') {
      await downlinkClient.subscribe(user.userId, media);
      downlinkClient.on('network-quality', (_, downlinkNetworkQuality: NetworkQuality) => {
        testResult.downlinkNetworkQualities.push(downlinkNetworkQuality);
      });
    }
  });
}
export const startTestNetworkQuality = () => {
  uplinkClient = DingRTC.createClient();
  downlinkClient = DingRTC.createClient();
  return new Promise((resolve) => {
    // 3. 开始检测
    testUplinkNetworkQuality();
    testDownlinkNetworkQuality();
    // 4. 15s 后停止检测，计算平均网络质量
    setTimeout(() => {
      // 计算上行平均网络质量
      if (testResult.uplinkNetworkQualities.length > 0) {
        testResult.average.uplinkNetworkQuality = Math.ceil(
          testResult.uplinkNetworkQualities.reduce((value, current) => value + current, 0) /
            testResult.uplinkNetworkQualities.length,
        ) as NetworkQuality;
      }
      if (testResult.downlinkNetworkQualities.length > 0) {
        // 计算下行平均网络质量
        testResult.average.downlinkNetworkQuality = Math.ceil(
          testResult.downlinkNetworkQualities.reduce((value, current) => value + current, 0) /
            testResult.downlinkNetworkQualities.length,
        ) as NetworkQuality;
      }
      const { uplinkNetworkQuality, downlinkNetworkQuality } = testResult.average;
      message.info(
        `上行：${networkQualityMap[uplinkNetworkQuality]}，下行：${networkQualityMap[downlinkNetworkQuality]}`,
      );
      cameraTrack.close();
      // 检测结束，清理相关状态。
      uplinkClient.leave();
      downlinkClient.leave();
      resolve({});
    }, 15 * 1000);
  });
};
