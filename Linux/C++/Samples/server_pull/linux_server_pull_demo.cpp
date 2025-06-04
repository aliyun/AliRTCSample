#include <string>
#include <iostream>
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

    // for audio
    volatile bool isPullAudioStarted = false;
    AudioObserver *audioObserver = nullptr;

    // for video
    volatile bool isPullVideoStarted = false;
    VideoObserver *videoObserver = nullptr;
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

class AudioObserver : public ding::rtc::RtcEngineAudioFrameObserver {
public:
    AudioObserver() {}

    ~AudioObserver() {}

    void OnCapturedAudioFrame(ding::rtc::RtcEngineAudioFrame &frame) override {
    }

    void OnProcessCapturedAudioFrame(ding::rtc::RtcEngineAudioFrame &frame) override {
    }

    void OnPublishAudioFrame(ding::rtc::RtcEngineAudioFrame &frame) override {
    }

    void OnPlaybackAudioFrame(ding::rtc::RtcEngineAudioFrame &frame) override {
        std::cout << "OnPlaybackAudioFrame  sampleRate: " << frame.samplesPerSec << ", channels: " << frame.channels << std::endl;
        // 拉流播放数据（多路合并后数据）
        // TODO do save or other logic, ex: asr
    }

    void OnRemoteUserAudioFrame(const char *uid, ding::rtc::RtcEngineAudioFrame &frame) override {
        std::cout << "OnRemoteUserAudioFrame uid: " << uid << ", sampleRate: " << frame.samplesPerSec << ", channels: " << frame.channels << std::endl;
        // 远端用户音频数据回调
        // TODO do save or other logic, ex: asr
    }
};

class VideoObserver : public ding::rtc::RtcEngineVideoFrameObserver {
public:
    VideoObserver() {}

    ~VideoObserver() {}

    ding::rtc::RtcEngineVideoPixelFormat GetVideoFormatPreference() override {
        return ding::rtc::RtcEngineVideoI420;
    }

    bool OnCaptureVideoFrame(ding::rtc::RtcEngineVideoFrame &frame) override {
        // 本地采集视频数据
        return false;
    }

    bool OnPreEncodeVideoFrame(ding::rtc::RtcEngineVideoTrack track, ding::rtc::RtcEngineVideoFrame &frame) override {
        // 本地编码前视频数据
        return false;
    }

    bool OnRemoteVideoFrame(ding::rtc::String uid, ding::rtc::RtcEngineVideoTrack track, ding::rtc::RtcEngineVideoFrame &frame) override {
        std::cout << "OnRemoteUserVideoFrame uid: " << uid.c_str() << ", track: " << track << ", width: " << frame.width << ", height: " << frame.height << std::endl;
        // 远端用户视频数据回调
        // TODO do save or other logic
        return false;
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
    if (ctx->audioObserver) {
        delete ctx->audioObserver;
    }
    ctx->audioObserver = nullptr;
    if (ctx->videoObserver) {
        delete ctx->videoObserver;
    }
    ctx->videoObserver = nullptr;
    delete ctx;
};

/**
 * 停止音频拉流
 */
void StopPullAudioData(RtcContext *ctx) {
    if (!ctx->isPullAudioStarted) {
        return;
    }
    if (ctx->engine) {
        // 1. 关闭音频数据订阅位置
        ctx->engine->EnableAudioFrameObserver(false, ding::rtc::RtcEngineAudioPositionPlayback);

        // 2. 取消注册observer
        ctx->engine->RegisterAudioFrameObserver(nullptr);

        // 3. 关闭音量提示和说话人提示功能，interval <= 0 代表关闭
        // 相关信息见回调： onAudioVolumeIndication
        ctx->engine->EnableAudioVolumeIndication(0, 3, 1);
    }
    if (ctx->audioObserver) {
        ctx->audioObserver = nullptr;
    }
    ctx->isPullAudioStarted = false;
};

/**
 * 开始音频拉流，获取播放PCM数据（支持配置采样率和通道数）
 * PCM数据见 AudioObserver::OnPlaybackAudioFrame || OnRemoteUserAudioFrame
 */
void StartPullAudioData(RtcContext *ctx) {
    if (ctx->isPullAudioStarted) {
        return;
    }
    if (ctx->audioObserver) {
        ctx->audioObserver = nullptr;
    }
    if (ctx->engine) {
        // 1. 创建音频数据输出observer，并注册到engine中
        ctx->audioObserver = new AudioObserver();
        ctx->engine->RegisterAudioFrameObserver(ctx->audioObserver);

        // 2. 开启音频数据订阅位置，并设置期望输出的音频PCM数据采样率和通道数，需要多个位置回调则多次调用
        ding::rtc::RtcEngineAudioFrameObserverConfig config;
        config.sampleRate = ding::rtc::RtcEngineAudioSampleRate_16000;
        config.channels = ding::rtc::RtcEngineMonoAudio;
        // 打开playback数据回调（远端全部用户音频数据)
        ctx->engine->EnableAudioFrameObserver(true, ding::rtc::RtcEngineAudioPositionPlayback, config);
        // 打开remoteUser数据回调（远端单个用户音频数据)
        ctx->engine->EnableAudioFrameObserver(true, ding::rtc::RtcEngineAudioPositionRemoteUser, config);

        // 3. 开启音量提示和说话人提示功能，interval > 0 代表开启，<= 0 代表关闭
        // 相关信息见回调： onAudioVolumeIndication
        ctx->engine->EnableAudioVolumeIndication(300, 3, 1);
    }
    ctx->isPullAudioStarted = true;
};

/**
 * 停止视频拉流
 */
void StopPullVideoData(RtcContext *ctx) {
    if (!ctx->isPullVideoStarted) {
        return;
    }
    if (ctx->engine) {
        // 1. 关闭视频数据订阅位置
        ctx->engine->EnableVideoFrameObserver(false, ding::rtc::RtcEnginePositionPreRender);

        // 2. 移除视频数据回调observer
        ctx->engine->RegisterVideoFrameObserver(nullptr);
    }
    if (ctx->videoObserver) {
        ctx->videoObserver = nullptr;
    }
    ctx->isPullVideoStarted = false;
};

/**
 * 开始视频拉流，获取远端视频YUV数据
 * YUV数据见 VideoObserver::OnRemoteVideoFrame
 */
void StartPullVideoData(RtcContext *ctx) {
    if (ctx->isPullVideoStarted) {
        return;
    }
    if (ctx->videoObserver) {
        ctx->videoObserver = nullptr;
    }
    if (ctx->engine) {
        // 1. 创建视频数据输出observer，并注册到engine中
        ctx->videoObserver = new VideoObserver();
        ctx->engine->RegisterVideoFrameObserver(ctx->videoObserver);

        // 2. 开启视频数据订阅位置(远端用户视频数据)，需要多个位置回调则多次调用
        ctx->engine->EnableVideoFrameObserver(true, ding::rtc::RtcEnginePositionPreRender);
    }
    ctx->isPullVideoStarted = true;
};

/**
 * 初始化引擎
 */
void InitRtcEngine(RtcContext *ctx) {
    // 设置日志路径
    ding::rtc::RtcEngine::SetLogDirPath("");
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
        // 也可以仅订阅单人 ctx->engine->subscribeRemoteVideoStream(userId, DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackCamera, true);
        ctx->engine->SubscribeAllRemoteVideoStreams(true);
    }
};

void Step_2_join(RtcContext *ctx) {
    ding::rtc::RtcEngineAuthInfo authInfo = GetAuthInfoFromServer(ctx);
    if (ctx->engine) {
        ctx->engine->JoinChannel(authInfo, "张三");
    }
};

void Step_3_start_pull_audio_video_data(RtcContext *ctx) {
    StartPullAudioData(ctx);
    StartPullVideoData(ctx);
};

void Step_4_stop_pull_audio_video_data(RtcContext *ctx) {
    StopPullAudioData(ctx);
    StopPullVideoData(ctx);
};

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

    //log("[Demo] main start");
    RtcContext *ctx = new RtcContext();

    // 1. 初始化SDK
    Step_1_init_sdk(ctx);

    // 2. 入会
    Step_2_join(ctx);

    // 为了演示方便，同步等待join成功，等onJoinChannelResult回调触发
    if (!Wait_join_result(ctx)) {
        // join fail
        return -1;
    }

    // 3. 开始拉音视频流
    Step_3_start_pull_audio_video_data(ctx);

    // 为了演示方便，同步等10s后，主动停止拉流
    Sleep(10 * 1000);

    // 4. 停止拉音频、视频裸数据
    Step_4_stop_pull_audio_video_data(ctx);

    // 5. 离会
    Step_5_leave(ctx);

    // 6. 销毁
    Step_6_destroy(ctx);

    DestroyRtcContext(ctx);
    return 0;
}
