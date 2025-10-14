#include "app_config.h"

#include <fstream>

#include "nlohmann/json.hpp"
#include "tlog/tlog.h"

AppConfig DefaultAppConfigForTest() {
    AppConfig appConfig;
    appConfig.appServer = "https://pre-onertc-demo-app-server.dingtalk.com";
    appConfig.env = "onertcPre";
    appConfig.appId = "64o1r7v2";
    appConfig.channelId = "r1818";
    appConfig.userId = "linux_user_001";
    appConfig.userName = "linux_user_001";
    appConfig.passwd = some-password;
    
    appConfig.publishAudio = true;
    appConfig.publishVideo = true;
    appConfig.subscribeAllAudio = true;
    //appConfig.subscribeAllVideo = true;
    appConfig.subscribeAllCameraTracks = true;
    appConfig.subscribeAllScreenTracks = true;
    appConfig.subscribeCameraTrackResolution = "SD";
    appConfig.dumpPlaybackAudio = false;
    appConfig.dumpRemoteVideo = false;

    appConfig.pushExternalAudio = appConfig.publishAudio;
    appConfig.pcmFilePath = "alan_famous.pcm";
    appConfig.pcmSampleRate = 48000;
    appConfig.pcmChannels = 2;
    appConfig.pcmReadFreq = 40;
    
    appConfig.pushExternalVideo = appConfig.publishVideo;
    appConfig.videoFilePath = "alan.yuv";
    appConfig.videoPixelFormat = "I420";
    appConfig.videoWidth = 720;
    appConfig.videoHeight = 1280;
    appConfig.videoFps = 25;
    appConfig.videoRotation = 0;
    appConfig.videoBitrate = 0;

    appConfig.loopPlayCount = -1;
    appConfig.joinLeaveCount = 1;
    return appConfig;
}

void to_json(nlohmann::json& j, const AppConfig& appConfig) {
    j["appServer"] = appConfig.appServer;
    j["appId"] = appConfig.appId;
    j["userId"] = appConfig.userId;
    j["userName"] = appConfig.userName;
    j["channelId"] = appConfig.channelId;
    j["env"] = appConfig.env;
    j["passwd"] = appConfig.passwd;

    j["publishAudio"] = appConfig.publishAudio;
    j["publishVideo"] = appConfig.publishVideo;
    j["subscribeAllAudio"] = appConfig.subscribeAllAudio;
    //j["subscribeAllVideo"] = appConfig.subscribeAllVideo;
    j["subscribeAllCameraTracks"] = appConfig.subscribeAllCameraTracks;
    j["subscribeAllScreenTracks"] = appConfig.subscribeAllScreenTracks;
    j["subscribeCameraTrackResolution"] = appConfig.subscribeCameraTrackResolution;
    j["dumpPlaybackAudio"] = appConfig.dumpPlaybackAudio;
    j["dumpRemoteVideo"] = appConfig.dumpRemoteVideo;

    j["pushExternalAudio"] = appConfig.pushExternalAudio;
    j["pcmSampleRate"] = appConfig.pcmSampleRate;
    j["pcmChannels"] = appConfig.pcmChannels;
    j["pcmReadFreq"] = appConfig.pcmReadFreq;

    j["pushExternalVideo"] = appConfig.pushExternalVideo;
    j["videoFilePath"] = appConfig.videoFilePath;
    j["videoPixelFormat"] = appConfig.videoPixelFormat;
    j["videoWidth"] = appConfig.videoWidth;
    j["videoHeight"] = appConfig.videoHeight;
    j["videoFps"] = appConfig.videoFps;
    j["videoRotation"] = appConfig.videoRotation;
    j["videoBitrate"] = appConfig.videoBitrate;

    j["loopPlayCount"] = appConfig.loopPlayCount;
    j["joinLeaveCount"] = appConfig.joinLeaveCount;
}

void from_json(const nlohmann::json& j, AppConfig& appConfig) {
    appConfig.appServer = j.value("appServer", appConfig.appServer);
    appConfig.env = j.value("env", appConfig.env);
    appConfig.appId = j.value("appId", appConfig.appId);
    appConfig.channelId = j.value("channelId", appConfig.channelId);
    appConfig.userId = j.value("userId", appConfig.userId);
    appConfig.userName = j.value("userName", appConfig.userName);
    appConfig.passwd = j.value("passwd", appConfig.passwd);
    
    appConfig.publishAudio = j.value("publishAudio", appConfig.publishAudio);
    appConfig.publishVideo = j.value("publishVideo", appConfig.publishVideo);
    appConfig.subscribeAllAudio = j.value("subscribeAllAudio", appConfig.subscribeAllAudio);
    //appConfig.subscribeAllVideo = j.value("subscribeAllVideo", appConfig.subscribeAllVideo);
    appConfig.subscribeAllCameraTracks = j.value("subscribeAllCameraTracks", appConfig.subscribeAllCameraTracks);
    appConfig.subscribeAllScreenTracks = j.value("subscribeAllScreenTracks", appConfig.subscribeAllScreenTracks);
    appConfig.subscribeCameraTrackResolution = j.value("subscribeCameraTrackResolution", appConfig.subscribeCameraTrackResolution);
    appConfig.dumpPlaybackAudio = j.value("dumpPlaybackAudio", appConfig.dumpPlaybackAudio);
    appConfig.dumpRemoteVideo = j.value("dumpRemoteVideo", appConfig.dumpRemoteVideo);

    appConfig.pushExternalAudio = j.value("pushExternalAudio", appConfig.pushExternalAudio);
    appConfig.pcmFilePath = j.value("pcmFilePath", appConfig.pcmFilePath);
    appConfig.pcmSampleRate = j.value("pcmSampleRate", appConfig.pcmSampleRate);
    appConfig.pcmChannels = j.value("pcmChannels", appConfig.pcmChannels);
    appConfig.pcmReadFreq = j.value("pcmReadFreq", appConfig.pcmReadFreq);
    
    appConfig.pushExternalVideo = j.value("pushExternalAudio", appConfig.pushExternalVideo);
    appConfig.videoFilePath = j.value("videoFilePath", appConfig.videoFilePath);
    appConfig.videoPixelFormat = j.value("videoPixelFormat", appConfig.videoPixelFormat);
    appConfig.videoWidth = j.value("videoWidth", appConfig.videoWidth);
    appConfig.videoHeight = j.value("videoHeight", appConfig.videoHeight);
    appConfig.videoFps = j.value("videoFps", appConfig.videoFps);
    appConfig.videoRotation = j.value("videoRotation", appConfig.videoRotation);
    appConfig.videoBitrate = j.value("videoBitrate", appConfig.videoBitrate);

    appConfig.loopPlayCount = j.value("loopPlayCount", appConfig.loopPlayCount);
    appConfig.joinLeaveCount = j.value("joinLeaveCount", appConfig.joinLeaveCount);
}

bool ParseAppConfig(const std::string& appConfigFilepath, AppConfig& appConfig) {
    if (appConfigFilepath.empty()) {
        TLOGE("App config file path is empty");
        return false;
    }

    std::ifstream file_stream(appConfigFilepath);
    file_stream.open(appConfigFilepath.c_str());
    if (!file_stream.is_open()) {
        TLOGE("App config file open failed");
        return false;
    }

    nlohmann::json j_app_config = nlohmann::json::parse(file_stream, nullptr, false);
    if (j_app_config.is_object()) {
        from_json(j_app_config, appConfig);
        
        nlohmann::json j_dump;
        to_json(j_dump, appConfig);
        TLOGI("App read app config from json file: %s", j_dump.dump(4).c_str());
    } else {
        TLOGE("App config file pass failed");
    }
    file_stream.close();
    return true;
}

bool ParseMultiAppConfig(const std::string& appConfigFilepath, std::vector<AppConfig>& appConfigs) {
    if (appConfigFilepath.empty()) {
        TLOGE("App config file path is empty");
        return false;
    }
    std::ifstream file_stream(appConfigFilepath);
    file_stream.open(appConfigFilepath.c_str());
    if (!file_stream.is_open()) {
        TLOGE("App config file open failed");
        return false;
    }
    nlohmann::json j_app_configs = nlohmann::json::parse(file_stream, nullptr, false);
    if (j_app_configs.is_array()) {
        for (int i = 0; i < j_app_configs.size(); i++) {
            AppConfig appConfig;
            from_json(j_app_configs[i], appConfig);
            appConfigs.push_back(appConfig);
        }
    } else {
        TLOGE("App config file pass failed");
    }
    file_stream.close();
    return true;
}
