#include <errno.h>
#include <stdarg.h>  // For va_list and related operations
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <memory>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <condition_variable>
#include <mutex>

#include <linux/limits.h>

#include "HttpRequest.h"
#include "engine_interface.h"
#include "nlohmann/json.hpp"
#include "tlog_print.h"

using namespace ding::rtc;
using json = nlohmann::json;

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

static int g_signum = 0;
static void SignalHandler(int signum) {
    switch (signum) {
        case SIGINT:
            TLOG_PRINT("Signal SIGINT (%d) caught", signum);
            break;
        case SIGTERM:
            TLOG_PRINT("Signal SIGTERM (%d) caught", signum);
            break;
        default:
            TLOG_PRINT("Signal (%d) caught", signum);
    }
    g_signum = signum;
}

void SleepForMilliseconds(int64_t milliseconds) {
    struct timespec sleep_time;
    sleep_time.tv_sec = milliseconds / 1000;
    sleep_time.tv_nsec = (milliseconds % 1000) * 1000000;
    while (nanosleep(&sleep_time, &sleep_time) != 0 && errno == EINTR)
        ;  // Ignore signals and wait for the full interval to elapse.
}

class RtcEventListener;
class RtcAudioFrameObserver;

struct RtcContext {
    RtcEngine* engine = nullptr;
    RtcEventListener* listener = nullptr;
    RtcAudioFrameObserver* audioFrameObserver = nullptr;
    // RtcVideoFrameObserver* videoFrameObserver = nullptr;
    
    // pre: https://pre-onertc-demo-app-server.dingtalk.com
    // online: https://onertc-demo-app-server.dingtalk.com
    std::string appServer;
    std::string appId;
    std::string userId;
    std::string userName;
    std::string channelName;
    std::string env;
    std::string passwd;
    
    int joinLeaveCount = 1;
    int64_t sleepMsBeforeLeave = 3000;

    bool publishAudio = true;
    bool publishVideo = true;
    bool subscribeAllAudio = true;
    bool subscribeAllVideo = true;

    bool pushExternalAudio = true;
    std::string pcmFilePath;
    int pcmSampleRate = 16000;
    int pcmChannels = 1;
    int pcmReadFreq = 40;
    
    bool pushExternalVideo = true;
    std::string videoFilePath;
    std::string videoPixelFormat = "I420";
    int videoWidth = 1920;
    int videoHeight = 1080;
    int videoFps = 30;
    int videoRotation = 0;

    int64_t loopPlayCount = 0;

    bool joinResponsed = false;
    int joinResult = -1;

    volatile bool quit = false;

    bool externalAudioFinished = false;
    std::thread externalAudioThread;

    bool externalVideoFinished = false;
    std::thread externalVideoThread;
};

RtcEngineVideoPixelFormat ConvertPixelFormat(const std::string& pixelFormat) {
    if (pixelFormat == "I420") return RtcEngineVideoI420;
    else if (pixelFormat == "NV12") return RtcEngineVideoNV12;
    else if (pixelFormat == "BGRA") return RtcEngineVideoBGRA;
    else if (pixelFormat == "ARGB") return RtcEngineVideoARGB;
    else if (pixelFormat == "RGBA") return RtcEngineVideoRGBA;
    else if (pixelFormat == "ABGR") return RtcEngineVideoABGR;
    else return RtcEngineVideoI420;
}

size_t CalcBufferSize(RtcEngineVideoPixelFormat pixel_format, int width, int height) {
  size_t buffer_size = 0;
  switch (pixel_format) {
    case RtcEngineVideoI420:
    case RtcEngineVideoNV12:
    case RtcEngineVideoNV21:
    // case VideoType::kIYUV:
    // case VideoType::kYV12:
    {
      int half_width = (width + 1) >> 1;
      int half_height = (height + 1) >> 1;
      buffer_size = width * height + half_width * half_height * 2;
      break;
    }
    // case VideoType::kARGB4444:
    // case VideoType::kRGB565:
    // case VideoType::kARGB1555:
    // case VideoType::kYUY2:
    // case VideoType::kUYVY:
    //   buffer_size = width * height * 2;
    //   break;
    // case VideoType::kRGB24:
    //   buffer_size = width * height * 3;
    //   break;
    case RtcEngineVideoBGRA:
    case RtcEngineVideoARGB:
    case RtcEngineVideoRGBA:
    case RtcEngineVideoABGR:
      buffer_size = width * height * 4;
      break;
    default:
      break;
  }
  return buffer_size;
}

int PushExternalVideoSource(RtcContext *ctx) {
    TLOG_PRINT("PushExternalVideoSource, videoFilePath:%s, width:%d, height:%d, fps:%d", ctx->videoFilePath.c_str(), ctx->videoWidth, ctx->videoHeight, ctx->videoFps);

    ctx->externalVideoThread = std::thread([=] {
        FILE *fp = fopen(ctx->videoFilePath.c_str(), "rb");
        if (fp) {
            int64_t frameCount = 0;
            RtcEngineVideoPixelFormat pixelFormat = ConvertPixelFormat(ctx->videoPixelFormat);
            int bufferSize = CalcBufferSize(pixelFormat, ctx->videoWidth, ctx->videoHeight);

            int64_t delay = 0;
            auto startClock = std::chrono::high_resolution_clock::now();
            auto lastStatsClock = std::chrono::steady_clock::now();
            uint8_t *buffer = new uint8_t[bufferSize];
            int64_t loopPlayCount = ctx->loopPlayCount;
            while (!ctx->quit) {
                auto readStartClock = std::chrono::high_resolution_clock::now();
                size_t readBytes = fread(buffer, 1, bufferSize, fp);
                int64_t readCostTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - readStartClock).count();
                // TLOG_PRINT("videoReadThread Reading yuv readBytes:%zu, bufferSize:%d, readCostTime:%ld", readBytes, bufferSize, readCostTime);
                if (readBytes == 0) {
                    TLOG_PRINT("Reading yuv EOF, loopPlayCount: %" PRId64, loopPlayCount);
                    if (--loopPlayCount > 0) {
                        fseek(fp, 0, SEEK_SET);
                        continue;
                    } else {
                        break;
                    }
                }

                // 配置外部视频流参数
                RtcEngineVideoFrame frame;
                memset(&frame, 0, sizeof(frame));
                frame.frameType = RtcEngineVideoFrameRaw;
                frame.pixelFormat = pixelFormat;
                frame.width = ctx->videoWidth;
                frame.height = ctx->videoHeight;
                frame.stride[0] = ctx->videoWidth;
                frame.stride[1] = frame.stride[2] = ctx->videoWidth >> 1;
                frame.rotation = ctx->videoRotation;
                frame.data = buffer;
                frame.timestamp = frameCount * 1000 / ctx->videoFps;

                delay = frame.timestamp;
                int64_t elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startClock).count();
                if (delay - elapsed > 5) {
                    SleepForMilliseconds(delay - elapsed);
                }
                
                auto pushStartClock = std::chrono::high_resolution_clock::now();
                ctx->engine->PushExternalVideoFrame(&frame, RtcEngineVideoTrackCamera);
                int64_t pushCostTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - pushStartClock).count();

                frameCount++;

                // TLOG_PRINT("videoPushThread PushExternalVideoFrame frameCount:%ld, delay:%ld, elapsed:%ld, delta:%ld, pushCostTime:%ld",
                //     frameCount, delay, elapsed, (delay - elapsed), pushCostTime);
                int64_t elpasedStats = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastStatsClock).count();
                if (elpasedStats >= 5000) {
                    TLOG_PRINT("videoPushThread PushExternalVideoFrame frameCount: %" PRId64, frameCount);
                    lastStatsClock = std::chrono::steady_clock::now();
                }
            }
            if (buffer) {
                delete[] buffer;
                buffer = nullptr;
            }
        }
        ctx->externalVideoFinished = true;
        if (fp)
            fclose(fp);
    });
    
    return 0;
}

int PushExternalAudioSource(RtcContext *ctx) {
    TLOG_PRINT("PushExternalAudioSource, pcmFilePath:%s, sampleRate:%d, pcmChannels:%d", ctx->pcmFilePath.c_str(), ctx->pcmSampleRate, ctx->pcmChannels);

    // 开启外部音频推流
    ctx->engine->SetExternalAudioSource(true, ctx->pcmSampleRate, ctx->pcmChannels);

    ctx->externalAudioThread = std::thread([=] {
        FILE *fp = fopen(ctx->pcmFilePath.c_str(), "rb");
        if (fp) {
            int64_t sampleCount = 0;
            int bytesPerSample = 2;
            int samplesToRead = ctx->pcmSampleRate / (1000 / ctx->pcmReadFreq);
            int bufferSize = samplesToRead * ctx->pcmChannels * bytesPerSample;
            int64_t delay = 0;
            auto startClock = std::chrono::high_resolution_clock::now();
            auto lastStatsClock = std::chrono::steady_clock::now();
            uint8_t *buffer = new uint8_t[bufferSize];
            int64_t loopPlayCount = ctx->loopPlayCount;
            while (!ctx->quit) {
                size_t readBytes = fread(buffer, 1, bufferSize, fp);
                // TLOG_PRINT("Reading pcm readBytes: %zu, bufferSize: %d", readBytes, bufferSize);
                if (readBytes == 0) {
                    TLOG_PRINT("Reading pcm EOF, loopPlayCount: %" PRId64, loopPlayCount);
                    if (--loopPlayCount > 0) {
                        fseek(fp, 0, SEEK_SET);
                        continue;
                    } else {
                        break;
                    }
                }

                RtcEngineAudioFrame frame;
                frame.type = RtcEngineAudioFramePcm16;
                frame.bytesPerSample = bytesPerSample;
                frame.samplesPerSec = ctx->pcmSampleRate;
                frame.channels = ctx->pcmChannels;
                frame.buffer = buffer;
                frame.samples = readBytes / frame.bytesPerSample / frame.channels;
                frame.timestamp = sampleCount * 1000 / ctx->pcmSampleRate;
                // sampleCount += frame.samples;

                delay = frame.timestamp;
                int64_t elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startClock).count();
                if (delay - elapsed > 5) {
                    SleepForMilliseconds(delay - elapsed);
                }
                    
                if (frame.samples > 0) {
                    ctx->engine->PushExternalAudioFrame(&frame);
                    sampleCount += frame.samples;
                    // TLOG_PRINT("audioPushThread PushExternalAudioFrame sampleCount:%ld, delay:%ld, elapsed: %ld, delta:%ld,",
                    //     sampleCount, delay, elapsed, (delay - elapsed));
                }

                int64_t elpasedStats = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastStatsClock).count();
                if (elpasedStats >= 5000) {
                    TLOG_PRINT("audioPushThread PushExternalAudioFrame sampleCount: %" PRId64, sampleCount);
                    lastStatsClock = std::chrono::steady_clock::now();
                }
            }
            if (buffer) {
                delete[] buffer;
                buffer = nullptr;
            }
        }
        ctx->externalAudioFinished = true;
        if (fp)
            fclose(fp);
    });

    return 0;
}

static std::string SystemTimestamp(bool withMs = false) {
    auto now_time = std::chrono::system_clock::now();
    auto tm_time = localtime(std::chrono::system_clock::to_time_t(now_time));
    std::ostringstream oss;
    oss << std::put_time(&tm_time, "%Y%m%d%H%M%S");
    if (withMs) {
        auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(now_time.time_since_epoch()).count() % 1000;
        oss << std::setfill('0') << std::setw(3) << millis;
    }
    return oss.str();
}

class RtcAudioFrameObserver : public RtcEngineAudioFrameObserver {
    RtcContext* ctx_;
    FILE* pcm_file_;
public:
    RtcAudioFrameObserver(RtcContext *ctx) : ctx_(ctx), pcm_file_(nullptr) {

    }
    ~RtcAudioFrameObserver() {
        if (pcm_file_) {
            fclose(pcm_file_);
            pcm_file_ = nullptr;
        }
    }

    void OnPlaybackAudioFrame(ding::rtc::RtcEngineAudioFrame &frame) override {
        if (!pcm_file_) {
            char filename[PATH_MAX] = {0};
            snprintf(filename, PATH_MAX, "audio_playback_%d_%d_%s.pcm",
                frame.samplesPerSec, frame.channels, SystemTimestamp().c_str());
            pcm_file_ = fopen(filename, "wb+");
        }

        if (pcm_file_) {
            int bytesToWrite = frame.samples * frame.channels * frame.bytesPerSample;
            if (bytesToWrite > 0) {
                fwrite(frame.buffer, 1, bytesToWrite, pcm_file_);
            }
        }
    }

    void OnCapturedAudioFrame(ding::rtc::RtcEngineAudioFrame &frame) override {}
    
    void OnProcessCapturedAudioFrame(ding::rtc::RtcEngineAudioFrame &frame) override {}

    void OnPublishAudioFrame(ding::rtc::RtcEngineAudioFrame &frame) override {}
};

//*/

class RtcEventListener : public RtcEngineEventListener {
    RtcContext* ctx_;
public:
    RtcEventListener(RtcContext *ctx) : ctx_(ctx) {}
    virtual ~RtcEventListener() {}

    /**
     * @brief 加入频道结果
     * @details 当应用调用 {@link RtcEngine::JoinChannel}
     * 方法时，该回调表示成功/失败加入频道，并且返回频道加入的相关信息以及加入频道耗时
     * @param result 加入频道结果，成功返回0，失败返回错误码
     * @param channel 频道id.
     * @param userId  用户ID
     * @param elapsed 加入频道耗时
     */
    virtual void OnJoinChannelResult(int result, const char* channel, const char* userId, int elapsed) {
        TLOG_PRINT("App OnJoinChannelResult, result:%d, channel:%s, userId:%s, elapsed:%d",
               result,
               channel ? channel : "",
               userId ? userId : "",
               elapsed);
        ctx_->joinResponsed = true;
        ctx_->joinResult = result;
    }

    /**
     * @brief 离开频道结果
     * @details 应用调用 {@link RtcEngine::LeaveChannel}
     * 方法时，该回调表示成功/失败离开频道，回调将会返回离会的result和该频道的基本信息,如果 {@link
     * RtcEngine::LeaveChannel} 后直接 {@link RtcEngine::Destory} SDK，将不会收到此回调
     * @param result 离开频道结果，成功返回0，失败返回错误码
     * @param stats 本次频道内会话的数据统计汇总。
     */
    virtual void OnLeaveChannelResult(int result, RtcEngineStats stats) {
        TLOG_PRINT("App OnLeaveChannelResult, result:%d", result);
    }

    /**
     * @brief 远端用户加入频道回调
     * @param uid 用户ID 从App server分配的唯一标示符
     * @param elapsed 用户加入频道时的耗时
     */
    virtual void OnRemoteUserOnLineNotify(const char* uid, int elapsed) {
        TLOG_PRINT("App OnRemoteUserOnLineNotify, uid:%s, elapsed:%d", uid ? uid : "", elapsed);
    }

    /**
     * @brief 远端用户离开频道回调
     * @param uid 用户ID 从App server分配的唯一标示符
     * @param reason 用户离线的原因，详见 {@link RtcEngineUserOfflineReason}
     */
    virtual void OnRemoteUserOffLineNotify(const char* uid, RtcEngineUserOfflineReason reason) {
        TLOG_PRINT("App OnRemoteUserOffLineNotify, uid:%s, reason:%d", uid ? uid : "", reason);
    }

    /**
     * @brief 被服务器踢出频道的消息
     * @param code onBye类型，详见 {@link RtcEngineOnByeType}
     */
    virtual void OnBye(RtcEngineOnByeType code) {
        TLOG_PRINT("App OnBye, code:%d", code);
    }

    /**
     * @brief 网络连接状态改变的回调
     * @param status 当前网络链接状态，详见 {@link RtcEngineConnectionStatus}
     * @param reason 网络链接状态改变原因，详见 {@link RtcEngineConnectionStatusChangeReason}
     */
    virtual void OnConnectionStatusChanged(RtcEngineConnectionStatus status,
                                          RtcEngineConnectionStatusChangeReason reason) {
        TLOG_PRINT("App OnConnectionStatusChanged, status:%d, reason:%d", status, reason);
    }

    /**
     * @brief 网络质量变化时发出的消息
     * @param uid  网络质量发生变化的用户uid
     * @param upQuality 上行网络质量，详见 {@link RtcEngineNetworkQuality}
     * @param downQuality 下行网络质量，详见 {@link RtcEngineNetworkQuality}
     * @note 当网络质量发生变化时触发，uid为空时代表用户自己的网络质量发生变化
     */
    virtual void OnNetworkQualityChanged(const char* uid,
                                         RtcEngineNetworkQuality upQuality,
                                         RtcEngineNetworkQuality downQuality) {
        TLOG_PRINT("App OnNetworkQualityChanged, uid:%s, upQuality:%d, downQuality:%d", (uid?uid:""), upQuality, downQuality);
    }

    /**
     * @brief 埋点log
     * @param log 埋点统计信息
     * @note 用处不大，后面移除
     */
    virtual void OnStatisticsLog(String const& log) { 
        TLOG_PRINT("App OnStatisticsLog, log:%s", log.c_str());
    }

    /**
     * @brief 如果engine出现warning，通过这个消息通知app
     * @param warn 警告类型
     * @param msg 警告信息
     */
    virtual void OnOccurWarning(int warn, const char* msg) {
        TLOG_PRINT("App OnOccurWarning, warn:%d, msg:%s", warn, msg ? msg : "");
    }

    /**
     * @brief 如果engine出现error，通过这个消息通知app
     * @param error  错误类型，参考 {@link RtcEngineErrorCode}
     * @param msg 错误描述
     */
    virtual void OnOccurError(int error, const char* msg) {
        TLOG_PRINT("App OnOccurError, error:%d, msg:%s", error, msg ? msg : "");
    }

    /**
     * @brief 埋点log
     * @param log 埋点统计信息
     * @note 用处不大，后面移除
     */
    virtual void OnStatisticsLevelLog(int level, const String& log) {
        TLOG_PRINT("App OnStatisticsLevelLog, level:%d, log:%s", level, log.c_str());
    }

    /**
     * @brief dingtalk质量报告，相对OnLocalAudioStats, OnRemoteVideoStats, etc.
     *        更加简练，适合在UI上展示
     * @param quality 采集到的各项指标数据
     */
    virtual void OnQualityReport(const QualityReport& quality) {
        TLOG_PRINT("App OnQualityReport, quality:%d", quality);
    }

    /**
     * @brief 本地音频统计信息
     * @param localAudioStats 本地视频统计信息
     * @note SDK每两秒触发一次此统计信息回调
     */
    virtual void OnLocalAudioStats(const LocalAudioStats& localAudioStats) {
        std::stringstream ss;
        ss << "{"
           << "track:" << localAudioStats.track << ","
           << "sentBitrate:" << localAudioStats.sentBitrate << ","
           << "sentSamplerate:" << localAudioStats.sentSamplerate << ","
           << "numChannel:" << localAudioStats.numChannel << ","
           << "inputLevel:" << localAudioStats.inputLevel << ","
           << "}";
        TLOG_PRINT("App OnLocalAudioStats, localAudioStats:%s", ss.str().c_str());
    }

    /**
     * @brief 本地视频统计信息
     * @param localVideoStats 本地视频统计信息
     * @note SDK每两秒触发一次此统计信息回调
     */
    virtual void OnLocalVideoStats(const LocalVideoStats& localVideoStats) {
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
        TLOG_PRINT("App OnLocalVideoStats, localVideoStats:%s", ss.str().c_str());
    }

    /**
     * @brief 远端音频统计信息
     * @param remoteAudioStats 远端视频统计信息
     * @note SDK每两秒触发一次此统计信息回调
     */
    virtual void OnRemoteAudioStats(const RemoteAudioStats& remoteAudioStats) {
        std::stringstream ss;
        ss << "{"
           << "userId:" << remoteAudioStats.userId.c_str() << ","
           << "packetLossRate:" << remoteAudioStats.packetLossRate << ","
           << "recvBitrate:" << remoteAudioStats.recvBitrate << ","
           << "totalFrozenTime:" << remoteAudioStats.totalFrozenTime << ","
           << "speechExpandRate:" << remoteAudioStats.speechExpandRate << ","
           << "}";
        TLOG_PRINT("App OnRemoteAudioStats, remoteAudioStats:%s", ss.str().c_str());
    }

    /**
     * @brief 远端视频统计信息
     * @param remoteVideoStats 远端视频统计信息
     * @note SDK每两秒触发一次此统计信息回调
     */
    virtual void OnRemoteVideoStats(const RemoteVideoStats& remoteVideoStats) {
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
           << "stuckTime:" << remoteVideoStats.stuckTime
           << "}";
        TLOG_PRINT("App OnRemoteVideoStats, remoteVideoStats:%s", ss.str().c_str());
    }

    /**
     * @brief 当前会话统计信息回调
     * @param stats 会话统计信息
     * @note SDK每两秒触发一次此统计信息回调
     */
    virtual void OnStats(const RtcEngineStats& stats) {
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
        TLOG_PRINT("App OnStats, stats:%s", ss.str().c_str());
    }

    /**
     * @brief 音频推流变更回调
     * @param oldState 之前的推流状态
     * @param newState 当前的推流状态
     * @param elapseSinceLastState 状态变更时间间隔
     * @param channel 当前频道id
     */
    virtual void OnAudioPublishStateChanged(RtcEnginePublishState oldState,
                                            RtcEnginePublishState newState,
                                            int elapseSinceLastState,
                                            const char* channel) {
        TLOG_PRINT("App OnAudioPublishStateChanged, oldState:%d, newState:%d, elapseSinceLastState:%d, channel:%s",
               oldState,
               newState,
               elapseSinceLastState,
               channel ? channel : "");
    }

    /**
     * @brief 视频推流变更回调
     * @param oldState 之前的推流状态
     * @param newState 当前的推流状态
     * @param elapseSinceLastState 状态变更时间间隔
     * @param channel 当前频道id
     */
    virtual void OnVideoPublishStateChanged(RtcEnginePublishState oldState,
                                            RtcEnginePublishState newState,
                                            int elapseSinceLastState,
                                            const char* channel) {
        TLOG_PRINT("App OnVideoPublishStateChanged, oldState:%d, newState:%d, elapseSinceLastState:%d, channel:%s",
               oldState,
               newState,
               elapseSinceLastState,
               channel ? channel : "");
    }

    /**
     * @brief 视频推流变更回调
     * @param oldState 之前的推流状态
     * @param newState 当前的推流状态
     * @param elapseSinceLastState 状态变更时间间隔
     * @param channel 当前频道id
     */
    virtual void OnScreenSharePublishStateChanged(RtcEnginePublishState oldState,
                                                  RtcEnginePublishState newState,
                                                  int elapseSinceLastState,
                                                  const char* channel) {
        TLOG_PRINT("App OnScreenSharePublishStateChanged, oldState:%d, newState:%d, elapseSinceLastState:%d, channel:%s",
               oldState,
               newState,
               elapseSinceLastState,
               channel ? channel : "");
    }

    /**
     * @brief 远端用户的音视频流发生变化回调
     * @details 该回调在以下场景会被触发
     * - 当远端用户从未推流变更为推流（包括音频和视频）
     * - 当远端用户从已推流变更为未推流（包括音频和视频）
     * @param uid userId，从App server分配的唯一标示符
     * @param audioTrack 音频流类型，详见 {@link RtcEngineAudioTrack}
     * @param videoTrack 视频流类型，详见 {@link RtcEngineVideoTrack}
     */
    virtual void OnRemoteTrackAvailableNotify(const char* uid,
                                              RtcEngineAudioTrack audioTrack,
                                              RtcEngineVideoTrack videoTrack) {
        TLOG_PRINT("App OnRemoteTrackAvailableNotify, uid:%s, audioTrack:%d, videoTrack:%d",
               uid ? uid : "",
               audioTrack,
               videoTrack);
    }

    /**
     * @brief 远端用户静音/取消静音回调
     * @param uid 远端用户ID
     * @param isMute 该用户是否静音
     * - true: 静音
     * - false: 取消静音
     */
    virtual void OnUserAudioMuted(const char* uid, bool isMute) {
        TLOG_PRINT("App OnUserAudioMuted, uid:%s, isMute:%d", uid ? uid : "", isMute);
    }

    /**
     * @brief 对端用户发送视频黑帧数据发送通知
     * @param uid 执行 muteVideo 的用户
     * @param isMute
     * - true: 推流黑帧
     * - false: 正常推流
     */
    virtual void OnUserVideoMuted(const char* uid, bool isMute, RtcEngineVideoTrack videoTrack) {
        TLOG_PRINT("App OnUserVideoMuted, uid:%s, isMute:%d, videoTrack:%d", uid ? uid : "", isMute, videoTrack);
    };

    /**
     * @brief 远端用户的第一帧视频帧显示时触发这个消息
     * @param uid user id
     * @param videoTrack 屏幕流或者相机流，参考 {@link RtcEngineVideoTrack}
     * @param width 视频宽度
     * @param height 视频高度
     * @param elapsed 本地用户加入频道直至该回调触发的延迟总耗时（毫秒）
     * @note 该接口用于远端用户的第一帧视频帧显示时的回调
     */
    virtual void OnFirstRemoteVideoFrameDrawn(
        const char* uid, RtcEngineVideoTrack videoTrack, int width, int height, int elapsed) {
        TLOG_PRINT("App OnFirstRemoteVideoFrameDrawn, uid:%s, videoTrack:%d, width:%d, height:%d, elapsed:%d",
               uid ? uid : "",
               videoTrack,
               width,
               height,
               elapsed);
    }

    /**
     * @brief 预览开始显示第一帧视频帧时触发这个消息
     * @param width 本地预览视频宽度
     * @param height 本地预览视频高度
     * @param elapsed 从本地用户加入频道直至该回调触发的延迟总耗时（毫秒）
     * @note 该接口用于预览开始显示第一帧视频帧时的回调
     */
    virtual void OnFirstLocalVideoFrameDrawn(int width, int height, int elapsed) {
        TLOG_PRINT("App OnFirstLocalVideoFrameDrawn, width:%d, height:%d, elapsed:%d", width, height, elapsed);
    }

    /**
     * @brief 音频订阅情况变更回调
     * @param uid userId，从App server分配的唯一标示符
     * @param oldState 之前的订阅状态，详见 {@link AliRTCSdk::RtcEngineSubscribeState}
     * @param newState 当前的订阅状态，详见 {@link AliRTCSdk::RtcEngineSubscribeState}
     * @param elapseSinceLastState 两次状态变更时间间隔(毫秒)
     * @param channel 当前频道id
     */
    virtual void OnAudioSubscribeStateChanged(const char* uid,
                                              RtcEngineSubscribeState oldState,
                                              RtcEngineSubscribeState newState,
                                              int elapseSinceLastState,
                                              const char* channel) {
        TLOG_PRINT(
            "App OnAudioSubscribeStateChanged, uid:%s, oldState:%d, newState:%d, elapseSinceLastState:%d, channel:%s",
            uid ? uid : "",
            oldState,
            newState,
            elapseSinceLastState,
            channel ? channel : "");
    }

    /**
     * @brief 相机流订阅情况变更回调
     * @param uid userId，从App server分配的唯一标示符
     * @param oldState 之前的订阅状态，详见 {@link AliRTCSdk::RtcEngineSubscribeState}
     * @param newState 当前的订阅状态，详见 {@link AliRTCSdk::RtcEngineSubscribeState}
     * @param elapseSinceLastState 两次状态变更时间间隔(毫秒)
     * @param channel 当前频道id
     */
    virtual void OnVideoSubscribeStateChanged(const char* uid,
                                              RtcEngineSubscribeState oldState,
                                              RtcEngineSubscribeState newState,
                                              int elapseSinceLastState,
                                              const char* channel) {
        TLOG_PRINT(
            "App OnVideoSubscribeStateChanged, uid:%s, oldState:%d, newState:%d, elapseSinceLastState:%d, channel:%s",
            uid ? uid : "",
            oldState,
            newState,
            elapseSinceLastState,
            channel ? channel : "");
    }

    /**
     * @brief 屏幕分享流订阅情况变更回调
     * @param uid userId，从App server分配的唯一标示符
     * @param oldState 之前的订阅状态，详见 {@link AliRTCSdk::RtcEngineSubscribeState}
     * @param newState 当前的订阅状态，详见 {@link AliRTCSdk::RtcEngineSubscribeState}
     * @param elapseSinceLastState 两次状态变更时间间隔(毫秒)
     * @param channel 当前频道id
     */
    virtual void OnScreenShareSubscribeStateChanged(const char* uid,
                                                    RtcEngineSubscribeState oldState,
                                                    RtcEngineSubscribeState newState,
                                                    int elapseSinceLastState,
                                                    const char* channel) {
        TLOG_PRINT(
            "App OnScreenShareSubscribeStateChanged, uid:%s, oldState:%d, newState:%d, elapseSinceLastState:%d, "
            "channel:%s",
            uid ? uid : "",
            oldState,
            newState,
            elapseSinceLastState,
            channel ? channel : "");
    }

    /**
     * @brief 大小订阅情况变更回调
     * @param uid userId，从App server分配的唯一标示符
     * @param oldStreamType 之前的订阅的大小流类型，详见 {@link AliRTCSdk::RtcEngineVideoStreamType}
     * @param newStreamType 当前的订阅的大小流类型，详见 {@link AliRTCSdk::RtcEngineVideoStreamType}
     * @param elapseSinceLastState 大小流类型变更时间间隔(毫秒)
     * @param channel 当前频道id
     */
    virtual void OnSubscribeStreamTypeChanged(const char* uid,
                                              RtcEngineVideoStreamType oldStreamType,
                                              RtcEngineVideoStreamType newStreamType,
                                              int elapseSinceLastState,
                                              const char* channel) {
        TLOG_PRINT(
            "App OnSubscribeStreamTypeChanged, uid:%s, oldStreamType:%d, newStreamType:%d, elapseSinceLastState:%d, "
            "channel:%s",
            uid ? uid : "",
            oldStreamType,
            newStreamType,
            elapseSinceLastState,
            channel ? channel : "");
    }

    /**
     * @brief 音频采集设备音量回调
     * @param level 音频采集设备音量值，取值[0, 100]
     */
    virtual void OnRecordingDeviceAudioLevel(int level) {
        TLOG_PRINT("App OnRecordingDeviceAudioLevel, level:%d", level);
    }

    /**
     * @brief 音频播放设备音量回调
     * @param level 音频播放设备音量值，取值[0, 100]
     */
    virtual void OnPlayoutDeviceAudioLevel(int level) {
        TLOG_PRINT("App OnPlayoutDeviceAudioLevel, level:%d", level);
    }

    /**
     * @brief 音频设备状态变更
     * @param deviceId 设备id
     * @param deviceType  设备类型
     * @param deviceState 设备状态
     */
    virtual void OnAudioDeviceStateChanged(const char* deviceId,
                                           RtcEngineDeviceType deviceType,
                                           RtcEngineDeviceState deviceState) {
        TLOG_PRINT("App OnAudioDeviceStateChanged, deviceId:%s, deviceType:%d, deviceState:%d", (deviceId?deviceId:""), deviceType, deviceState);
    }

#if defined(__ANDROID__) || (defined(__APPLE__) && TARGET_OS_IOS)
    /**
     * @brief 音频路由发生变化回调
     * @details 所谓“音频路由”，是指声音是从手机的扬声器还是从听筒中播放出来，音频路由变化也就是声音的播放位置发生了变化
     * - 当音频路由为听筒时，声音比较小，只有将耳朵凑近才能听清楚，隐私性较好，适合用于接听电话。
     * - 当音频路由为扬声器时，声音比较大，不用将手机贴脸也能听清，因此可以实现“免提”的功能。
     * @param audioRouteType 当前使用的音频路由，详情参考{@link RtcEngineAudioRouteType}
     */
    virtual void OnAudioRouteChanged(RtcEngineAudioRouteType audioRouteType) {}
#endif
};

std::string URLEncode(const std::string& url) {
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
            encoded += StringPrintf("%%%02X", character);
        }
    }

    return encoded;
}

int RequestAuthInfo(const std::string& appServer,
                    const std::string& appId,
                    const std::string& userId,
                    const std::string& userName,
                    const std::string& channelName,
                    const std::string& env,
                    const std::string& passwd,
                    RtcEngineAuthInfo& authInfo) {
    std::string url = appServer + "/login?" +
                      "passwd=" + URLEncode(passwd) +
                      "&appid=" + URLEncode(appId) +
                      "&userid=" + URLEncode(userId) +
                      "&user=" + URLEncode(userName) +
                      "&room=" + URLEncode(channelName) +
                      "&env=" + URLEncode(env) +
                      "&tokensid=false";

    std::condition_variable cv;
    std::mutex cv_m;
    bool request_completed = false;
    int status_code = -1;

    tinyhttp::HttpRequest req;
    int timeout_sec = 15;
    req.setOption(tinyhttp::Option::REQUEST_TIMEOUT, &timeout_sec, sizeof(timeout_sec));

    req.onResponse([channelName, &cv, &cv_m, &status_code, &request_completed, &authInfo](int status,
                                      const tinyhttp::HeadersVector& headers,
                                      const std::string& body) {
        TLOG_PRINT("onResponse, status=%d, headers=%zu, body=%zu %s", status, headers.size(), body.size(), body.c_str());
        for (const auto& kv : headers) {
            TLOG_PRINT("headers %s: %s", kv.first.c_str(), kv.second.c_str());
        }

        int code = -1;
        if (status == 200) {
            std::string appId;
            std::string userId;
            std::string token;
            std::vector<std::string> gslbs;

            auto j_resp = json::parse(body, nullptr, false);
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
                // authInfo.appId = appId.c_str();
                // authInfo.channelId = channelName.c_str();
                // authInfo.userId = userId.c_str();
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
            status_code = code;
        }
        cv.notify_all();
    });
    req.onError([&cv, &cv_m, &status_code, &request_completed](int errCode, const std::string& errMsg) {
        TLOG_PRINT("login appserver failed: errCode=%d, errMsg=%s", errCode, errMsg.c_str());
        {
            std::lock_guard<std::mutex> lk(cv_m);
            request_completed = true;
            status_code = errCode;
        }
        cv.notify_all();
    });
    req.open("GET", url, "");

    {
        std::unique_lock<std::mutex> lk(cv_m);
        TLOG_PRINT("xxx wait request request_completed ...");
        cv.wait(lk, [&request_completed]{ return request_completed; });
    }

    return status_code;
}

#define GET_STRING_FROM_JSON(jobj, key) \
    if (jobj[#key].is_string()) { \
        auto j_##key = jobj[#key].get<std::string>(); \
        if (!j_##key.empty()) key = j_##key; \
    }

#define GET_INT_FROM_JSON(jobj, key) \
    if (jobj[#key].is_number()) { \
        auto j_##key = jobj[#key].get<int>(); \
        key = j_##key; \
    }

#define GET_INT64_FROM_JSON(jobj, key) \
    if (jobj[#key].is_number()) { \
        auto j_##key = jobj[#key].get<int64_t>(); \
        key = j_##key; \
    }

#define GET_BOOLEAN_FROM_JSON(jobj, key) \
    if (jobj[#key].is_boolean()) { \
        auto j_##key = jobj[#key].get<bool>(); \
        key = j_##key; \
    }

std::string gen_random(const int len) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::string tmp_s;
    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i) {
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    
    return tmp_s;
}

void ParseAppConfig(RtcContext *ctx, const std::string& appConfigFilePath) {
    std::string appServer;
    std::string appId;
    std::string userId;
    std::string userName;
    std::string channelName;
    std::string env;
    std::string passwd;

    int joinLeaveCount = 1;
    int64_t sleepMsBeforeLeave = 3000;

    bool publishAudio = true;
    bool publishVideo = true;
    bool subscribeAllAudio = true;
    bool subscribeAllVideo = true;

    // pcm文件及音频参数
    bool pushExternalAudio = true;
    std::string pcmFilePath;
    int pcmSampleRate = 16000;
    int pcmChannels = 1;
    int pcmReadFreq = 40; // audio duration (ms) each time push
    
    // yuv文件及视频参数
    bool pushExternalVideo = true;
    std::string videoFilePath;
    std::string videoPixelFormat;
    int videoWidth = 1920;
    int videoHeight = 1080;
    int videoFps = 30;
    int videoRotation = 0;

    int64_t loopPlayCount = 0;

    if (!appConfigFilePath.empty()) {
        // read app config from json file
        std::ifstream fileStream(appConfigFilePath);
        fileStream.open(appConfigFilePath.c_str());
        if (fileStream.is_open()) {
            auto j_appConfig = json::parse(fileStream, nullptr, false);
            if (j_appConfig.is_object()) {
                TLOG_PRINT("App read app config from json file...");
                GET_STRING_FROM_JSON(j_appConfig, appServer);
                GET_STRING_FROM_JSON(j_appConfig, appId);
                GET_STRING_FROM_JSON(j_appConfig, userId);
                GET_STRING_FROM_JSON(j_appConfig, userName);
                GET_STRING_FROM_JSON(j_appConfig, channelName);
                GET_STRING_FROM_JSON(j_appConfig, env);
                GET_STRING_FROM_JSON(j_appConfig, passwd);

                GET_INT_FROM_JSON(j_appConfig, joinLeaveCount);
                GET_INT64_FROM_JSON(j_appConfig, sleepMsBeforeLeave);

                GET_BOOLEAN_FROM_JSON(j_appConfig, publishAudio);
                GET_BOOLEAN_FROM_JSON(j_appConfig, publishVideo);
                GET_BOOLEAN_FROM_JSON(j_appConfig, subscribeAllAudio);
                GET_BOOLEAN_FROM_JSON(j_appConfig, subscribeAllVideo);
                
                GET_BOOLEAN_FROM_JSON(j_appConfig, pushExternalAudio);
                GET_STRING_FROM_JSON(j_appConfig, pcmFilePath);
                GET_INT_FROM_JSON(j_appConfig, pcmSampleRate);
                GET_INT_FROM_JSON(j_appConfig, pcmChannels);
                GET_INT_FROM_JSON(j_appConfig, pcmReadFreq);
                
                GET_BOOLEAN_FROM_JSON(j_appConfig, pushExternalVideo);
                GET_STRING_FROM_JSON(j_appConfig, videoFilePath);
                GET_STRING_FROM_JSON(j_appConfig, videoPixelFormat);
                GET_INT_FROM_JSON(j_appConfig, videoWidth);
                GET_INT_FROM_JSON(j_appConfig, videoHeight);
                GET_INT_FROM_JSON(j_appConfig, videoFps);
                GET_INT_FROM_JSON(j_appConfig, videoRotation);
                GET_INT64_FROM_JSON(j_appConfig, loopPlayCount);

                TLOG_PRINT("parse json result: ");
                TLOG_PRINT("    appServer: %s", appServer.c_str());
                TLOG_PRINT("    appId: %s", appId.c_str());
                TLOG_PRINT("    userId: %s", userId.c_str());
                TLOG_PRINT("    userName: %s", userName.c_str());
                TLOG_PRINT("    channelName: %s", channelName.c_str());
                TLOG_PRINT("    env: %s", env.c_str());
                TLOG_PRINT("    passwd: %s", passwd.c_str());
                TLOG_PRINT("    joinLeaveCount: %d", joinLeaveCount);
                TLOG_PRINT("    sleepMsBeforeLeave: %" PRId64, sleepMsBeforeLeave);
                TLOG_PRINT("    publishAudio: %d", publishAudio);
                TLOG_PRINT("    publishVideo: %d", publishVideo);
                TLOG_PRINT("    subscribeAllAudio: %d", subscribeAllAudio);
                TLOG_PRINT("    subscribeAllVideo: %d", subscribeAllVideo);
                TLOG_PRINT("    pcmFilePath: %s", pcmFilePath.c_str());
                TLOG_PRINT("    pcmSampleRate: %d", pcmSampleRate);
                TLOG_PRINT("    pcmChannels: %d", pcmChannels);
                TLOG_PRINT("    pcmReadFreq: %d", pcmReadFreq);
                TLOG_PRINT("    videoFilePath: %s", videoFilePath.c_str());
                TLOG_PRINT("    videoPixelFormat: %s", videoPixelFormat.c_str());
                TLOG_PRINT("    videoWidth: %d", videoWidth);
                TLOG_PRINT("    videoHeight: %d", videoHeight);
                TLOG_PRINT("    videoFps: %d", videoFps);
                TLOG_PRINT("    videoRotation: %d", videoRotation);
                TLOG_PRINT("    loopPlayCount: %" PRId64, loopPlayCount);
            } else {
                TLOG_PRINT("App parse user info failed...");
            }
            fileStream.close();
        } else {
            TLOG_PRINT("App no config file opened");
            return;
        }
    }

#define SET_RTCCONTEXT_VAR(ctx, key) ctx->key = key
    SET_RTCCONTEXT_VAR(ctx, appServer);
    SET_RTCCONTEXT_VAR(ctx, appId);
    SET_RTCCONTEXT_VAR(ctx, userId);
    SET_RTCCONTEXT_VAR(ctx, userName);
    SET_RTCCONTEXT_VAR(ctx, channelName);
    SET_RTCCONTEXT_VAR(ctx, env);
    SET_RTCCONTEXT_VAR(ctx, passwd);

    SET_RTCCONTEXT_VAR(ctx, joinLeaveCount);
    SET_RTCCONTEXT_VAR(ctx, sleepMsBeforeLeave);
    
    SET_RTCCONTEXT_VAR(ctx, publishAudio);
    SET_RTCCONTEXT_VAR(ctx, publishVideo);
    SET_RTCCONTEXT_VAR(ctx, subscribeAllAudio);
    SET_RTCCONTEXT_VAR(ctx, subscribeAllVideo);

    SET_RTCCONTEXT_VAR(ctx, pushExternalAudio);
    SET_RTCCONTEXT_VAR(ctx, pcmFilePath);
    SET_RTCCONTEXT_VAR(ctx, pcmSampleRate);
    SET_RTCCONTEXT_VAR(ctx, pcmChannels);
    SET_RTCCONTEXT_VAR(ctx, pcmReadFreq);

    SET_RTCCONTEXT_VAR(ctx, pushExternalVideo);
    SET_RTCCONTEXT_VAR(ctx, videoFilePath);
    SET_RTCCONTEXT_VAR(ctx, videoPixelFormat);
    SET_RTCCONTEXT_VAR(ctx, videoWidth);
    SET_RTCCONTEXT_VAR(ctx, videoHeight);
    SET_RTCCONTEXT_VAR(ctx, videoFps);
    SET_RTCCONTEXT_VAR(ctx, videoRotation);

    SET_RTCCONTEXT_VAR(ctx, loopPlayCount);
}

RtcEngine* InitRtcEngine(RtcContext *ctx) {
    // 设置SDK日志路径
    char cwd[PATH_MAX] = {0};
    getcwd(cwd, sizeof(cwd));
    char logPath[PATH_MAX];
    snprintf(logPath, PATH_MAX, "%s/rtc_log", cwd);
    TLOG_PRINT("logPath: %s", logPath);
    RtcEngine::SetLogDirPath(logPath);

    nlohmann::json j_extras;
    j_extras["disableLogToConsole"] = true;
    if (ctx->subscribeAllVideo) {
        j_extras["videoDumpPath"] = std::string(cwd);
    }

    std::string extras = j_extras.dump();
    RtcEngine* engine = RtcEngine::Create(extras.c_str());

    ctx->engine = engine;
    ctx->listener = new RtcEventListener(ctx);

    // 设置SDK事件回调
    engine->SetEngineEventListener(ctx->listener);

    return engine;
}

int JoinAndLeaveChannel(RtcContext *ctx, RtcEngineAuthInfo& authInfo) {
    // 入会前配置
    RtcEngine* engine = ctx->engine;

    engine->PublishLocalAudioStream(ctx->publishAudio);
    engine->PublishLocalVideoStream(ctx->publishVideo);

    engine->SubscribeAllRemoteAudioStreams(ctx->subscribeAllAudio);
    //engine->SubscribeAllRemoteVideoStreams(ctx->subscribeAllVideo);
    engine->SubscribeAllRemoteVideoStreams(ctx->subscribeAllCameraTracks, ctx->subscribeAllScreenTracks);
    ding::rtc::RtcEngineVideoStreamType restype;
    if (app_config_.subscribeCameraTrackResolution == "LD") {
        restype = ding::rtc::RtcEngineVideoStreamType::RtcEngineVideoStreamTypeLD;
    }
    else if (app_config_.subscribeCameraTrackResolution == "SD") {
        restype = ding::rtc::RtcEngineVideoStreamType::RtcEngineVideoStreamTypeSD;
    }
    else if (app_config_.subscribeCameraTrackResolution == "HD") {
        restype = ding::rtc::RtcEngineVideoStreamType::RtcEngineVideoStreamTypeHD;
    }
    else if (app_config_.subscribeCameraTrackResolution == "FHD") {
        restype = ding::rtc::RtcEngineVideoStreamType::RtcEngineVideoStreamTypeFHD;
    }
    engine->SetRemoteDefaultVideoStreamType(restype);

    if (ctx->subscribeAllAudio) {
        ctx->audioFrameObserver = new RtcAudioFrameObserver(ctx);
        engine->RegisterAudioFrameObserver(ctx->audioFrameObserver);
        engine->EnableAudioFrameObserver(true, RtcEngineAudioPositionPlayback);
    }

    // if (ctx->subscribeAllVideo) {
    //     ctx->videoFrameObserver = new RtcVideoFrameObserver(ctx);
    //     engine->RegisterVideoFrameObserver(ctx->videoFrameObserver);
    // }

    // 订阅视频大小流（所有远端），默认订阅大流
    // RtcEngineVideoStreamType subAllVideoType = RtcEngineVideoStreamTypeHigh; // 视频大小流:
    // RtcEngineVideoStreamTypeHigh, RtcEngineVideoStreamTypeLow
    // engine->SetRemoteDefaultVideoStreamType(subAllVideoType);

    // 测试订阅指定人音视频流
    // OnRemoteUserOnLineNotify 通知远端入会uid
    // OnRemoteTrackAvailableNotify 通知开始接收远端音视频流
    // std::string remote_uid = "gsvegpnuly";

    // 订阅指定人视频（目前支持Camera流）
    // bool subVideo = true;
    // RtcEngineVideoTrack videoTrack = RtcEngineVideoTrackCamera; // 视频track类型: RtcEngineVideoTrackCamera,
    // RtcEngineVideoTrackScreen, RtcEngineVideoTrackBoth
    // engine->SubscribeRemoteVideoStream(remote_uid.c_str(), videoTrack, subVideo);

    // RtcEngineVideoStreamType videoStreamType = RtcEngineVideoStreamTypeHigh; // 视频大小流:
    // RtcEngineVideoStreamTypeHigh, RtcEngineVideoStreamTypeLow
    // engine->SetRemoteVideoStreamType(remote_uid.c_str(), videoStreamType);

    RtcEngineVideoEncoderConfiguration videoEncoderConfig;
    videoEncoderConfig.dimensions = RtcEngineVideoDimensions(ctx->videoWidth, ctx->videoHeight);
    videoEncoderConfig.frameRate = ctx->videoFps;
    engine->SetVideoEncoderConfiguration(videoEncoderConfig);

    // 外部视频输入，需在入会前启用
    // 这里使用外部YUV数据取代摄像头
    if (ctx->pushExternalVideo) {
        engine->SetExternalVideoSource(true, RtcEngineVideoTrackCamera);
    }

    // join channel
    engine->JoinChannel(authInfo, ctx->userName.c_str());

    while (!ctx->joinResponsed) {
        // 等待join成功
        SleepForMilliseconds(100);
        if (g_signum) {
            break;
        }
    }

    if (ctx->joinResult == 0) {
        ctx->externalAudioFinished = !ctx->pushExternalAudio;
        ctx->externalVideoFinished = !ctx->pushExternalVideo;

        // 入会成功后，开启外部音视频推流
        if (ctx->pushExternalAudio) {
            PushExternalAudioSource(ctx);
        }
        if (ctx->pushExternalVideo) {
            PushExternalVideoSource(ctx);
        }

        while (!ctx->externalAudioFinished || !ctx->externalVideoFinished) {
            if (g_signum) {
                ctx->quit = true;
                break;
            }
            SleepForMilliseconds(1000);
        }

        if (ctx->pushExternalAudio) {
            engine->SetExternalAudioSource(false, ctx->pcmSampleRate, ctx->pcmChannels);
        }
        if (ctx->pushExternalVideo) {
            engine->SetExternalVideoSource(false, RtcEngineVideoTrackCamera);
        }

        int64_t msToSleep = ctx->sleepMsBeforeLeave;
        while (!g_signum && msToSleep > 0) {
            int onceToSleep = MIN(100, msToSleep);
            SleepForMilliseconds(onceToSleep);
            msToSleep -= onceToSleep;
        }

        TLOG_PRINT("LeaveChannel...");

        engine->LeaveChannel();
    }

    return 0;
}

int main(int argc, char** argv) {
    char cwd[PATH_MAX] = {0};
    getcwd(cwd, sizeof(cwd));

    TLOG_PRINT("App start running from workdir:%s ... argc:%d", cwd, argc);

    signal(SIGINT, SignalHandler);
    signal(SIGTERM, SignalHandler);

    RtcContext *ctx = new RtcContext();

    std::string appConfigFilePath("app_config.json");
    // pass config file by argv
    if (argc > 1) {
        appConfigFilePath = argv[1];
    }

    ParseAppConfig(ctx, appConfigFilePath);
    
    RtcEngineAuthInfo authInfo;
    authInfo.appId = ctx->appId.c_str();
    authInfo.channelId = ctx->channelName.c_str();
    authInfo.userId = ctx->userId.c_str();
    int statusCode = RequestAuthInfo(ctx->appServer, ctx->appId, ctx->userId, ctx->userName, ctx->channelName, ctx->env, ctx->passwd, authInfo);
    if (statusCode != 0 && statusCode != 200) {
        TLOG_PRINT("RequestAuthInfo error: %d", statusCode);
        return -1;
    }

    TLOG_PRINT("xxx Request RtcEngineAuthInfo result: ");
    TLOG_PRINT("    authInfo.appId: %s", authInfo.appId.c_str());
    TLOG_PRINT("    authInfo.channelId: %s", authInfo.channelId.c_str());
    TLOG_PRINT("    authInfo.userId: %s", authInfo.userId.c_str());
    TLOG_PRINT("    authInfo.token: %s", authInfo.token.c_str());
    TLOG_PRINT("    authInfo.gslbServer: %s", authInfo.gslbServer.c_str());

    RtcEngine* engine = InitRtcEngine(ctx);

    int joinLeaveCount = ctx->joinLeaveCount > 0 ? ctx->joinLeaveCount : 1;
    for (int i = 0; i < joinLeaveCount; i++) {
        JoinAndLeaveChannel(ctx, authInfo);
        if (g_signum) {
            ctx->quit = true;
            break;
        }
    }

    RtcEngine::Destroy(engine);
    
    if (ctx->listener) {
        delete ctx->listener;
    }
    if (ctx->audioFrameObserver) {
        delete ctx->audioFrameObserver;
    }
    // if (ctx->videoFrameObserver) {
    //     delete ctx->videoFrameObserver;
    // }
    if (ctx->externalAudioThread.joinable()) {
        ctx->externalAudioThread.join();
    }
    if (ctx->externalVideoThread.joinable()) {
        ctx->externalVideoThread.join();
    }

    if (ctx) {
        delete ctx;
        ctx = nullptr;
    }

    TLOG_PRINT("App exit.");

    return 0;
}
