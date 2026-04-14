# DingRTC Android Full Feature Demo

这是一个基于DngRTC SDK 的 Android 全功能示例工程，展示了如何实现音视频通话、会议、白板等核心功能。

## 🚀 快速开始

### 1. 环境要求
- Android Studio Arctic Fox 或更高版本
- JDK 11 或更高版本
- Android SDK API Level 19+ (minSdkVersion)

### 2. 配置 AppID 和 AppKey
在运行项目之前，你需要在代码中填入有效的 AppID 和 AppKey。

打开 `src/main/java/com/ding/rtc/demo/token/TokenGenerator.java` 文件，找到以下位置并填入你的凭证：

```java
// TokenGenerator.java
private static final String APP_ID = "your_app_id_here";
private static final String APP_KEY = "your_app_key_here";
```

### 3. 编译与运行
1. 使用 Android Studio 打开项目根目录。
2. 点击 **Sync Project with Gradle Files** 同步依赖。
3. 连接 Android 设备或启动模拟器。
4. 点击 **Run** 按钮安装并启动应用。

## ⚠️ 注意事项

- **权限申请**：首次启动时请允许应用获取摄像头、麦克风等权限。
- **高级功能依赖**：**白板、虚拟背景、美颜、智能降噪**等功能需要额外引入对应的 `.so` 库才能正常运行。
- **功能配置**：**AI Agent、实时字幕**功能需要正确配置 `assets/config.json` 文件（包含服务器地址等信息）方可使用。
