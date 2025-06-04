#include <string>
#include <iostream>
#include <chrono>
#include <thread>
#include "engine_interface.h"


class RtcContext;
class RtcListener;
class AudioObserver;
class VideoObserver;

bool local_test_mode{false};
std::string local_test_appid;
std::string local_test_channelid;
std::string local_test_userid;
std::string local_test_token;
std::string local_test_gslb;

/**
 * TODO request from app server
 */
ding::rtc::RtcEngineAuthInfo GetAuthInfoFromServer(RtcContext *ctx) {
    ding::rtc::RtcEngineAuthInfo authInfo;
    if (local_test_mode) {
        authInfo.appId = local_test_appid.c_str();
        authInfo.channelId = local_test_channelid.c_str();
        authInfo.userId = local_test_userid.c_str();
        authInfo.token = local_test_token.c_str();
        // 注意 gslbServer 字段推荐填空串
        authInfo.gslbServer = local_test_gslb.c_str();
    } else {
        // request from app server
        authInfo.appId = "";
        authInfo.channelId = "";
        authInfo.userId = "";
        authInfo.token = "";
        // 注意 gslbServer 字段推荐填空串
        authInfo.gslbServer = "";
    }
    return authInfo;
};

void Sleep(long milliseconds) {
    struct timespec sleep_time;
    sleep_time.tv_sec = milliseconds / 1000;
    sleep_time.tv_nsec = (milliseconds % 1000) * 1000000;
    while (nanosleep(&sleep_time, &sleep_time) != 0 && errno == EINTR) {
    }
};


struct RtcContext {
    ding::rtc::RtcEngine *engine = nullptr;
    ding::rtc::RtcEngineEventListener *listener = nullptr;

    volatile bool joinFinished = false;
    int joinResult = -1;

    volatile bool quit = false;

    // audio config
    std::string pcmFilePath;
    int pcmSampleRate = 16000;
    int pcmChannels = 1;
    int pcmReadFreq = 40;
    volatile bool isPushAudioStarted = false;
    volatile bool externalAudioFinished = false;
    std::thread *externalAudioThread;

    // video config
    std::string videoYUVFilePath;
    ding::rtc::RtcEngineVideoPixelFormat videoPixelFormat = ding::rtc::RtcEngineVideoI420;
    int videoWidth = 0;
    int videoHeight = 0;
    int videoRotation = 0;
    int videoFps = 25;
    volatile bool isPushVideoStarted = false;
    volatile bool externalVideoFinished = false;
    std::thread *externalVideoThread;
};

class RtcListener : public ding::rtc::RtcEngineEventListener {
private:
    RtcContext *ctx_;
public:
    RtcListener(RtcContext *ctx) {
        ctx_ = ctx;
    }

    ~RtcListener() {}

    void OnJoinChannelResult(int result, const char *channel, const char *userId, int elapsed) override {
        std::cout << "OnJoinChannelResult result: " << result << ", channel: " << channel << ", uid : " << userId << ", elapsed: " << elapsed << std::endl;
        if (ctx_) {
            ctx_->joinFinished = true;
            ctx_->joinResult = result;
        }
    }

    void OnLeaveChannelResult(int result, ding::rtc::RtcEngineStats stats) override {
        std::cout << "OnLeaveChannelResult result: " << result << std::endl;
    }

    void OnRemoteUserOnLineNotify(const char *uid, int elapsed) override {
        std::cout << "OnRemoteUserOnLineNotify uid: " << uid << ", elapsed: " << elapsed << std::endl;
    }

    void OnRemoteUserOffLineNotify(const char *uid, ding::rtc::RtcEngineUserOfflineReason reason) override {
        std::cout << "OnRemoteUserOffLineNotify uid: " << uid << ", reason: " << reason << std::endl;
    }
};


void DestroyRtcContext(RtcContext *ctx) {
    if (!ctx) {
        return;
    }
    if (ctx->engine) {
       ding::rtc::RtcEngine::Destroy(ctx->engine);
    }
    ctx->engine = nullptr;
    if (ctx->listener) {
        delete ctx->listener;
    }
    ctx->listener = nullptr;
    if (ctx->externalAudioThread) {
        if (ctx->externalAudioThread->joinable()) {
            ctx->externalAudioThread->join();
        }
        delete ctx->externalAudioThread;
    }
    ctx->externalAudioThread = nullptr;
    if (ctx->externalVideoThread) {
        if (ctx->externalVideoThread->joinable()) {
            ctx->externalVideoThread->join();
        }
        delete ctx->externalVideoThread;
    }
    ctx->externalVideoThread = nullptr;
    ctx->quit = false;
    ctx->joinFinished = false;
    ctx->joinResult = -1;
    ctx->isPushAudioStarted = false;
    ctx->isPushVideoStarted = false;
};


/**
 * 计算视频一帧数据大小
 */
int CalcBufferSize(ding::rtc::RtcEngineVideoPixelFormat pixel_format, int width, int height) {
    int buffer_size = 0;
    switch (pixel_format) {
        case ding::rtc::RtcEngineVideoI420:
        case ding::rtc::RtcEngineVideoNV12:
        case ding::rtc::RtcEngineVideoNV21: {
            int half_width = (width + 1) >> 1;
            int half_height = (height + 1) >> 1;
            buffer_size = width * height + half_width * half_height * 2;
            break;
        }
        case ding::rtc::RtcEngineVideoBGRA:
        case ding::rtc::RtcEngineVideoARGB:
        case ding::rtc::RtcEngineVideoRGBA:
        case ding::rtc::RtcEngineVideoABGR:
            buffer_size = width * height * 4;
            break;
        default:
            break;
    }
    return buffer_size;
};

/**
 * 停止推音频流数据
 */
void StopPushAudio(RtcContext *ctx) {
    if (!ctx->isPushAudioStarted) {
        return;
    }
    ctx->isPushAudioStarted = false;
    if (ctx->engine) {
        // 关闭推流
        ctx->engine->PublishLocalAudioStream(false);
        // 关闭设置音频PCM推流
        ctx->engine->SetExternalAudioSource(false, ctx->pcmSampleRate, ctx->pcmChannels);
    }
    if (ctx->externalAudioThread) {
        ctx->externalAudioThread->join();
    }
    ctx->externalAudioThread = nullptr;
}

/**
 * 开始推音频流数据
 */
void StartPushAudio(RtcContext *ctx) {
    if (ctx->isPushAudioStarted) {
        return;
    }
    ctx->isPushAudioStarted = true;
    if (ctx->engine) {
        // 开启推流
        ctx->engine->PublishLocalAudioStream(true);
        // 设置音频PCM推流
        ctx->engine->SetExternalAudioSource(true, ctx->pcmSampleRate, ctx->pcmChannels);
    }
    ctx->externalAudioThread = new std::thread([ctx]() {
        FILE *fp = fopen(ctx->pcmFilePath.c_str(), "rb"); 
        if (fp) {
            long sampleCount = 0;
            // 16-bit pcm, 2byte
            int bytesPerSample = 2;
            int samplesToRead = ctx->pcmSampleRate / (1000 / ctx->pcmReadFreq);
            int bufferSize = samplesToRead * ctx->pcmChannels * bytesPerSample;            
            long delay = 0;
            auto startClock = std::chrono::high_resolution_clock::now();
            auto lastStatsClock = std::chrono::steady_clock::now();
            std::unique_ptr<uint8_t[]> buffer(new uint8_t[bufferSize]);
            ding::rtc::RtcEngineAudioFrame frame;
            while (!ctx->quit) {
                long lastPos = ftell(fp);
                size_t readBytes = fread(buffer.get(), 1, bufferSize, fp);
                if (readBytes == 0) {
                    // 读完文件，直接退出
                    // break;

                    // 如果想循环读
                    fseek(fp, 0, SEEK_SET);
                    continue;
                }
                frame.bytesPerSample = bytesPerSample;
                frame.samplesPerSec = ctx->pcmSampleRate;
                frame.channels = ctx->pcmChannels;
                frame.buffer = buffer.get();
                frame.samples = readBytes / frame.bytesPerSample / frame.channels;
                frame.timestamp = sampleCount * 1000 / ctx->pcmSampleRate;

                delay = frame.timestamp;
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startClock).count();
                if (delay - elapsed > 5) {
                    Sleep(std::chrono::milliseconds(delay - elapsed).count());
                }

                if (frame.samples > 0) {
                    int ret = ctx->engine->PushExternalAudioFrame(&frame);
                    if (ret == 0) {
                        sampleCount += frame.samples;
                    } else {
                        fseek(fp, lastPos, SEEK_SET);
                        // sdk buffer full, wait some time ,ex: 40ms
                        if (ret == ding::rtc::RtcEngineErrorAudioBufferFull) {
                            Sleep(40);
                        }
                    }
                }

                auto elpasedStats = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastStatsClock).count();
                if (elpasedStats >= 2000) {
                    std::cout << "[Demo] audioPushThread pushExternalAudioFrame sampleCount:" << sampleCount << std::endl;
                    lastStatsClock = std::chrono::steady_clock::now();
                }
            }
        } else {
            ctx->externalAudioFinished = true;
        }
    });
};

/**
 * 停止推视频流数据
 */
void StopPushVideo(RtcContext *ctx) {
    if (!ctx->isPushVideoStarted) {
        return;
    }
    ctx->isPushVideoStarted = false;
    if (ctx->engine) {
        // 关闭推流
        ctx->engine->PublishLocalVideoStream(false);
        ctx->engine->SetExternalVideoSource(false, ding::rtc::RtcEngineVideoTrackCamera);
    }
    if (ctx->externalVideoThread) {
        ctx->externalVideoThread->join();
    }
    ctx->externalVideoThread = nullptr;
};

/**
 * 开始推视频流数据
 */
void StartPushVideo(RtcContext *ctx) {
    if (ctx->isPushVideoStarted) {
        return;
    }
    ctx->isPushVideoStarted = true;
    if (ctx->engine) {
        // 开启推流
        ctx->engine->PublishLocalVideoStream(true);
        // 设置开启视频外部推流
        ctx->engine->SetExternalVideoSource(true, ding::rtc::RtcEngineVideoTrackCamera);
    }
    ctx->externalVideoThread = new std::thread([ctx]() {
        FILE *fp = fopen(ctx->videoYUVFilePath.c_str(), "rb");
        if (fp) {
            long frameCount = 0;
            ding::rtc::RtcEngineVideoPixelFormat pixelFormat = ctx->videoPixelFormat;
            int bufferSize = CalcBufferSize(pixelFormat, ctx->videoWidth, ctx->videoHeight);

            long delay = 0;
            auto startClock = std::chrono::high_resolution_clock::now();
            auto lastStatsClock = startClock;
            std::unique_ptr<uint8_t[]> buffer(new uint8_t[bufferSize]);
            ding::rtc::RtcEngineVideoFrame frame;
            while (!ctx->quit) {
                auto readStartClock = std::chrono::high_resolution_clock::now();
                long lastPos = ftell(fp);
                size_t readBytes = fread(buffer.get(), 1, bufferSize, fp);
                if (readBytes != bufferSize) {
                    // 读完文件，直接退出
                    // break;

                    // 如果想循环读
                    fseek(fp, 0, SEEK_SET);
                    continue;
                }
                frame.frameType = ding::rtc::RtcEngineVideoFrameRaw;
                frame.pixelFormat = pixelFormat;
                frame.width = ctx->videoWidth;
                frame.height = ctx->videoHeight;
                frame.stride[0] = ctx->videoWidth;
                frame.stride[1] = ctx->videoWidth >> 1;
                frame.stride[2] = ctx->videoWidth >> 1;
                frame.rotation = ctx->videoRotation;
                frame.data = buffer.get();
                frame.timestamp = frameCount * 1000 / ctx->videoFps;

                delay = frame.timestamp;
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startClock).count();
                if (delay - elapsed > 5) {
                    Sleep(std::chrono::milliseconds(delay - elapsed).count());
                }

                int ret = ctx->engine->PushExternalVideoFrame(&frame, ding::rtc::RtcEngineVideoTrackCamera);
                if (ret == 0) {
                    frameCount++;
                } else {
                    fseek(fp, lastPos, SEEK_SET);
                    if (ret == ding::rtc::RtcEngineErrorVideoBufferFull) {
                        // sdk buffer full, wait some time ,ex: 40ms
                        Sleep(40);
                    }
                }

                int64_t elpasedStats = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - lastStatsClock).count();
                if (elpasedStats >= 2000) {
                    std::cout << "[Demo] videoPushThread pushExternalVideoFrame frameCount:" << frameCount << std::endl;
                    lastStatsClock = std::chrono::high_resolution_clock::now();
                }
            }
        } else {
            ctx->externalVideoFinished = true;
        }
    });
};

/**
 * 初始化引擎
 */
void InitRtcEngine(RtcContext *ctx) {
    // 设置日志路径
    ding::rtc::RtcEngine::SetLogDirPath("./");
    // 初始化回调
    RtcListener *listener = new RtcListener(ctx);
    // 初始化引擎
    ding::rtc::RtcEngine *engine = ding::rtc::RtcEngine::Create("");
    engine->SetEngineEventListener(listener);
    ctx->engine = engine;
    ctx->listener = listener;
};

bool Wait_join_result(RtcContext *ctx) {
    while (!ctx->joinFinished) {
        Sleep(500);
    }
    if (ctx->joinResult != 0) {
        // 入会失败，退出
        // 需要检查authinfo是否请求正确
        //log("[Demo] join failed, return");
        DestroyRtcContext(ctx);
        return false;
    }
    return true;
};

void Step_1_init_sdk(RtcContext *ctx) {
    InitRtcEngine(ctx);
    if (ctx->engine) {
        // 设置是否订阅音频流，默认会自动订阅，可以不调用
        // note:目前音频流仅支持合流订阅，单人订阅后续支持
        ctx->engine->SubscribeAllRemoteAudioStreams(true);

        // 设置是否订阅视频流，默认会自动订阅，可以不调用
        // 也可以仅订阅单人 ctx->engine->subscribeRemoteVideoStream(userId, DingRtcengine->DingRtcVideoTrack.DingRtcVideoTrackCamera, true);
        ctx->engine->SubscribeAllRemoteVideoStreams(true);
    }
};

void Step_2_join(RtcContext *ctx) {
    ding::rtc::RtcEngineAuthInfo authInfo = GetAuthInfoFromServer(ctx);
    if (ctx->engine) {
        ctx->engine->JoinChannel(authInfo, "张三");
    }
};

void Step_3_start_push_audio_video_data(RtcContext *ctx) {
    StartPushAudio(ctx);
    StartPushVideo(ctx);
}

void Step_4_stop_push_audio_video_data(RtcContext *ctx) {
    StopPushAudio(ctx);
    StopPushVideo(ctx);
}

void Step_5_leave(RtcContext *ctx) {
    if (ctx->engine) {
        ctx->engine->LeaveChannel();
    }
};

void Step_6_destroy(RtcContext *ctx) {
    if (ctx->engine) {
        ding::rtc::RtcEngine::Destroy(ctx->engine);
        ctx->engine = nullptr;
    }
};

int main(int argc, char* argv[]) {
    if (argc >= 6) {
        local_test_mode = true;
        local_test_appid = argv[1];
        local_test_channelid = argv[2];
        local_test_userid = argv[3];
        local_test_token = argv[4];
        local_test_gslb = argv[5];
    }

    RtcContext *ctx = new RtcContext();

    /**
     * 配置音频、视频推流资源文件和相关参数
     * 该示例音视频源为文件，其他形式资源流程基本一致
     */
    // 音频输入资源
    ctx->pcmFilePath = "audio.pcm";
    ctx->pcmSampleRate = 16000;
    ctx->pcmChannels = 1;
    // 一次读取40ms数据，控制io读取频率
    ctx->pcmReadFreq = 25;

    // 视频输入资源
    ctx->videoYUVFilePath = "video.yuv";
    ctx->videoPixelFormat = ding::rtc::RtcEngineVideoI420;
    ctx->videoWidth = 320;
    ctx->videoHeight = 180;
    ctx->videoRotation = 0;
    ctx->videoFps = 20;

    // 1. 初始化SDK
    Step_1_init_sdk(ctx);

    // 2. 入会
    Step_2_join(ctx);

    // 为了演示方便，同步等待join成功，等onJoinChannelResult回调触发
    if (!Wait_join_result(ctx)) {
        // join fail
        return -1;
    }

    // 3. 开始推音视频流
    Step_3_start_push_audio_video_data(ctx);

    // 为了演示方便，同步等10s后，主动停止推流
    Sleep(10 * 1000);
    // 设置结束flag，结束推流线程
    ctx->quit = true;

    // 4. 停止推音视频流
    Step_4_stop_push_audio_video_data(ctx);

    // 5. 离会
    Step_5_leave(ctx);

    // 6. 销毁
    Step_6_destroy(ctx);

    DestroyRtcContext(ctx);
    return 0;
}
