#include "rtcengine_helper.h"

#include <condition_variable>
#include <mutex>
#include <vector>

#include "nlohmann/json.hpp"
#include "HttpRequest.h"

#include "tlog/utils.h"
#include "tlog/tlog.h"


static std::string URLEncode(const std::string& url) {
    const char kSafeCharacters[] = "-_.~";
    std::string encoded;
    encoded.reserve(url.length());

    for (unsigned char character : url) {
        if (((character >= 'A') && (character <= 'Z')) || ((character >= 'a') && (character <= 'z')) ||
            ((character >= '0') && (character <= '9')) || (strchr(kSafeCharacters, character) != nullptr)) {
            // Copy unreserved character.
            encoded += character;
        } else {
            // Escape character.
            encoded += tlog::StringPrintf("%%%02X", character);
        }
    }

    return encoded;
}

bool RequestAuthInfo(const std::string &appServer,
                     const std::string &appId,
                     const std::string &userId,
                     const std::string &userName,
                     const std::string &channelId,
                     const std::string &env,
                     const std::string &passwd,
                     ding::rtc::RtcEngineAuthInfo &authInfo) {
    std::string url = appServer + "/login?" +
                      "passwd=" + URLEncode(passwd) +
                      "&appid=" + URLEncode(appId) +
                      "&userid=" + URLEncode(userId) +
                      "&user=" + URLEncode(userName) +
                      "&room=" + URLEncode(channelId) +
                      "&env=" + URLEncode(env) +
                      "&tokensid=false";
    TLOGI("RequestAuthInfo, url: %s", url.c_str());

    std::condition_variable cv;
    std::mutex cv_m;
    bool request_completed = false;
    int response_code = -1;

    tinyhttp::HttpRequest req;
    int timeout_sec = 15;
    req.setOption(tinyhttp::Option::REQUEST_TIMEOUT, &timeout_sec, sizeof(timeout_sec));

    req.onResponse([channelId, &cv, &cv_m, &response_code, &request_completed, &authInfo](int status,
                                      const tinyhttp::HeadersVector& headers,
                                      const std::string& body) {
        TLOGI("onResponse, status=%d, headers=%zu, body=%zu %s", status, headers.size(), body.size(), body.c_str());
        for (const auto& kv : headers) {
            TLOGD("headers %s: %s", kv.first.c_str(), kv.second.c_str());
        }

        int code = -1;
        if (status == 200) {
            std::string appId;
            std::string userId;
            std::string token;
            std::vector<std::string> gslbs;

            auto j_resp = nlohmann::json::parse(body, nullptr, false);
            if (j_resp.is_object()) {
                auto j_code = j_resp["code"];
                if (j_code.is_number_integer()) {
                    code = j_code.get<int>();

                    auto j_data = j_resp["data"];
                    if (j_data.is_object()) {
                        appId = j_data["appid"].get<std::string>();
                        userId = j_data["userid"].get<std::string>();
                        token = j_data["token"].get<std::string>();
                        gslbs = j_data["gslb"].get<std::vector<std::string>>();
                    }
                }
            }

            if (code == 0 || code == 200) {
                authInfo.appId = appId.c_str();
                authInfo.channelId = channelId.c_str();
                authInfo.userId = userId.c_str();
                authInfo.token = token.c_str();
                std::string gslbServer;
                if (gslbs.size() > 0) {
                    gslbServer = gslbs[0];
                }
                authInfo.gslbServer = gslbServer.c_str();
            }
        }

        {
            std::lock_guard<std::mutex> lk(cv_m);
            request_completed = true;
            response_code = code;
        }
        cv.notify_all();
    });
    req.onError([&cv, &cv_m, &response_code, &request_completed](int errCode, const std::string& errMsg) {
        TLOGI("appserver failed: errCode=%d, errMsg=%s", errCode, errMsg.c_str());
        {
            std::lock_guard<std::mutex> lk(cv_m);
            request_completed = true;
            response_code = errCode;
        }
        cv.notify_all();
    });
    req.open("GET", url, "");

    {
        std::unique_lock<std::mutex> lk(cv_m);
        TLOGI("wait request request_completed ...");
        cv.wait(lk, [&request_completed]{ return request_completed; });
    }

    return (response_code == 0 || response_code == 200);
}

std::string RtcEngineStatsToString(const RtcEngineStats& stats) {
    std::stringstream ss;
    ss << "{"
        << "duration:" << stats.duration << ","
        << "txBytes:" << stats.txBytes << ","
        << "txAudioBytes:" << stats.txAudioBytes << ","
        << "txVideoBytes:" << stats.txVideoBytes << ","
        << "txKBitrate:" << stats.txKBitrate << ","
        << "txAudioKBitrate:" << stats.txAudioKBitrate << ","
        << "txVideoKBitrate:" << stats.txVideoKBitrate << ","
        << "txPacketLossRate:" << stats.txPacketLossRate << ","
        << "rxBytes:" << stats.rxBytes << ","
        << "rxPackets:" << stats.rxPackets << ","
        << "rxAudioBytes:" << stats.rxAudioBytes << ","
        << "rxVideoBytes:" << stats.rxVideoBytes << ","
        << "rxKBitrate:" << stats.rxKBitrate << ","
        << "rxAudioKBitrate:" << stats.rxAudioKBitrate << ","
        << "rxVideoKBitrate:" << stats.rxVideoKBitrate << ","
        << "rxPacketLossRate:" << stats.rxPacketLossRate << ","
        << "rxLostPackets:" << stats.rxLostPackets << ","
        << "lastmileDelay:" << stats.lastmileDelay << ","
        << "connectTimeMs:" << stats.connectTimeMs << ","
        << "systemCpuRate:" << stats.systemCpuRate << ","
        << "appCpuRate:" << stats.appCpuRate << ","
        << "}";
    return ss.str();
}

std::string LocalAudioStatsToString(const LocalAudioStats& localAudioStats) {
    std::stringstream ss;
        ss << "{"
           << "track:" << localAudioStats.track << ","
           << "sentBitrate:" << localAudioStats.sentBitrate << ","
           << "sentSamplerate:" << localAudioStats.sentSamplerate << ","
           << "numChannel:" << localAudioStats.numChannel << ","
           << "inputLevel:" << localAudioStats.inputLevel << ","
           << "}";
           return ss.str();
}
std::string LocalVideoStatsToString(const LocalVideoStats& localVideoStats) {
     std::stringstream ss;
        ss << "{"
           << "track:" << localVideoStats.track << ","
           << "targetEncodeBitrate:" << localVideoStats.targetEncodeBitrate << ","
           << "actualEncodeBitrate:" << localVideoStats.actualEncodeBitrate << ","
           << "sentBitrate:" << localVideoStats.sentBitrate << ","
           << "sentFps:" << localVideoStats.sentFps << ","
           << "encodeFps:" << localVideoStats.encodeFps << ","
           << "captureFps:" << localVideoStats.captureFps << ","
           << "avgQpPerSec:" << localVideoStats.avgQpPerSec << ","
           << "encoderFrameWidth:" << localVideoStats.encoderFrameWidth << ","
           << "encoderFrameHeight:" << localVideoStats.encoderFrameHeight << ","
           << "captureFrameWidth:" << localVideoStats.captureFrameWidth << ","
           << "captureFrameHeight:" << localVideoStats.captureFrameHeight << ","
           << "}";
           return ss.str();
}
std::string RemoteAudioStatsToString(const RemoteAudioStats& remoteAudioStats){
    std::stringstream ss;
        ss << "{"
           << "userId:" << remoteAudioStats.userId.c_str() << ","
           << "packetLossRate:" << remoteAudioStats.packetLossRate << ","
           << "recvBitrate:" << remoteAudioStats.recvBitrate << ","
           << "totalFrozenTimes:" << remoteAudioStats.totalFrozenTime << ","
           << "}";
           return ss.str();
}
std::string RemoteVideoStatsToString(const RemoteVideoStats& remoteVideoStats) {
    std::stringstream ss;
        ss << "{"
           << "userId:" << remoteVideoStats.userId.c_str() << ","
           << "track:" << remoteVideoStats.track << ","
           << "width:" << remoteVideoStats.width << ","
           << "height:" << remoteVideoStats.height << ","
           << "recvBitrate:" << remoteVideoStats.recvBitrate << ","
           << "decoderOutputFrameRate:" << remoteVideoStats.decoderOutputFrameRate << ","
           << "rendererOutputFrameRate:" << remoteVideoStats.rendererOutputFrameRate << ","
           << "packetLossRate:" << remoteVideoStats.packetLossRate << ","
           << "}";
           return ss.str();
}

const char* to_string(RtcEngineUserOfflineReason reason) {
    switch (reason) {
        case RtcEngineUserOfflineQuit:
            return "UserOfflineQuit";
        case RtcEngineUserOfflineDropped:
            return "UserOfflineDropped";
    }
    return "";
}

const char* to_string(RtcEngineVideoTrack videoTrack) {
    switch (videoTrack) {
        case RtcEngineVideoTrackNo:
            return "VideoTrackNo";
        case RtcEngineVideoTrackCamera:
            return "VideoTrackCamera";
        case RtcEngineVideoTrackScreen:
            return "VideoTrackScreen";
        case RtcEngineVideoTrackBoth:
            return "VideoTrackBoth";
    }
    return "";
}

const char* to_string(RtcEnginePublishState state) {
    switch (state) {
        case RtcEngineStatsPublishIdle:
            return "Idle";
        case RtcEngineStatsNoPublish:
            return "NoPublish";
        case RtcEngineStatsPublishing:
            return "Publishing";
        case RtcEngineStatsPublished:
            return "Published";
    }
    return "";
}

const char* to_string(RtcEngineSubscribeState state) {
    switch (state) {
        case RtcEngineStatsSubscribeIdle:
            return "Idle";
        case RtcEngineStatsNoSubscribe:
            return "NoSubscribe";
        case RtcEngineStatsSubscribing:
            return "Subscribing";
        case RtcEngineStatsSubscribed:
            return "Subscribed";
    }
    return "";
}
