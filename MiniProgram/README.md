## DingRTC 微信小程序 Demo

基于 **DingRTC（oneRTC）** 音视频通信 SDK 的微信小程序示例项目，演示了如何在微信小程序中实现实时音视频通话功能。

## 功能特性

- **加入/离开频道**：支持通过 AppId、频道名、用户名加入音视频房间
- **音视频通话**：支持麦克风和摄像头的开启/关闭
- **远端用户管理**：自动处理远端用户的加入、离开及音视频流订阅
- **视频旋转**：支持远端视频画面旋转适配
- **网络状态监控**：实时监听连接状态变化

## 项目结构

```
miniprogram/
├── pages/
│   └── join/                # 入会页面（首页）
├── packageA/
│   └── pages/
│       └── meeting/         # 会议页面（音视频通话）
├── jssdk/
│   └── dingrtc_miniapp_client.js  # DingRTC 小程序 SDK
├── services/
│   └── usesdk.js            # SDK 调用封装
├── app.js
├── app.json
└── app.wxss
```

## 快速开始

### 前置条件

- [微信开发者工具](https://developers.weixin.qq.com/miniprogram/dev/devtools/download.html)
- 有效的 DingRTC AppId

### 安装与运行

1. 克隆项目到本地

2. 安装 npm 依赖

```shell
cd miniprogram
npm install
```

3. 在微信开发者工具中导入本项目

4. 点击菜单栏「工具」→「构建 npm」

5. 编译运行小程序

### 使用方式

1. 在入会页面输入 **AppId**、**频道名** 和 **用户名**
2. 点击加入频道，进入会议页面
3. 在会议页面中可进行音视频通话操作
