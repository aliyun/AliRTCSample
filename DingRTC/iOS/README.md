# DingRTC Demo 使用指导

## 1. 注册阿里云账号
访问阿里云 [注册账号](https://help.aliyun.com/document_detail/324609.html) 页面，根据提示创建阿里云账号。

## 2. 开通服务
参考 [开通服务](https://help.aliyun.com/document_detail/111590.html) 页面，根据提示快速开通RTC服务并购买时长包。

## 3. 创建应用
参考 [创建应用](https://help.aliyun.com/document_detail/124297.html) 页面，通过在控制台创建应用，可以获取您的应用ID。

## 3. 下载 Demo 源码
此 Demo 源代码托管在 [GitHub](https://github.com/aliyun/AliRTCSample) 上，开发者可以自行克隆或下载。

## 4. 下载 SDK
音视频通信 SDK 可从 [下载页面](https://help.aliyun.com/document_detail/71770.html) 下载，并放置到 Demo 源码的 SDK 目录中。

## 5. 生成临时 Token
用户在创建应用并且获取到应用ID后，还需要 Token 才可以使用音视频通信RTC服务。对于 Demo 用户可以参考 [接入工具](https://help.aliyun.com/document_detail/146833.html) 生成临时Token。

## 6. 编译 Demo 源码
使用 Xcode（14.0 或以上版本）打开源码工程 BasicVideoCall，然后将 AppID 和 Token 拷贝到 Demo 中相应位置，即可开始编译运行。

## 7. 开发环境要求
- 兼容 iOS 12.0 及以上版本
- Xcode 14.0 或以上版本
