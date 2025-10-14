#ifndef __APP_CONFIG_H__
#define __APP_CONFIG_H__

#include <stdint.h>
#include <string>
#include <vector>
#include "nlohmann/json.hpp"

// Configuration used for basic test
struct AppConfig {
    // pre: https://pre-onertc-demo-app-server.dingtalk.com
    // online: https://onertc-demo-app-server.dingtalk.com
    std::string appServer;
    std::string appId;
    std::string userId;
    std::string userName;
    std::string channelId;
    std::string env;
    std::string passwd;
    
    bool publishAudio;
    bool publishVideo;
    bool subscribeAllAudio;
    // bool subscribeAllVideo;
    bool subscribeAllCameraTracks;
    bool subscribeAllScreenTracks;
    std::string subscribeCameraTrackResolution;
    bool dumpPlaybackAudio;
    bool dumpRemoteVideo;

    bool pushExternalAudio;
    std::string pcmFilePath;
    int pcmSampleRate;
    int pcmChannels;
    int pcmReadFreq;
    
    bool pushExternalVideo;
    std::string videoFilePath;
    std::string videoPixelFormat;
    int videoWidth;
    int videoHeight;
    int videoFps;
    int videoRotation;
    int videoBitrate;

    int64_t loopPlayCount; // -1 indefinitely
    int64_t joinLeaveCount; // -1 indefinitely
    // int64_t sleepMsBeforeLeave;
};

void to_json(nlohmann::json& j, const AppConfig& appConfig);
void from_json(const nlohmann::json& j, AppConfig& appConfig);

AppConfig DefaultAppConfigForTest();
bool ParseAppConfig(const std::string& appConfigFilepath, AppConfig& appConfig);
bool ParseMultiAppConfig(const std::string& appConfigFilepath, std::vector<AppConfig>& appConfig);


#endif // __APP_CONFIG_H__
