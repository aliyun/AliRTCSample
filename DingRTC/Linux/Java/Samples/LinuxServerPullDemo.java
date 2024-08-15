package samples;

import com.ding.rtc.DingRtcEngine;
import com.ding.rtc.DingRtcEngineListener;
import org.json.JSONObject;

import java.io.*;
import java.net.HttpURLConnection;
import java.net.URL;
import java.net.URLEncoder;
import java.nio.charset.StandardCharsets;
import java.util.Random;

public class LinuxServerPullDemo {

    static class RtcContext {
        DingRtcEngine engine = null;
        DingRtcEngineListener listener = null;

        volatile boolean joinFinished = false;
        int joinResult = -1;

        // for audio
        volatile boolean isPullAudioStarted = false;
        AudioObserver audioObserver = null;

        // for video
        volatile boolean isPullVideoStarted = false;
        VideoObserver videoObserver = null;

        void reset() {
            log("context reset");
            if (engine != null) {
                engine.destroy();
            }
            engine = null;
            listener = null;
            audioObserver = null;
            videoObserver = null;
        }
    }

    static class RtcListener extends DingRtcEngineListener {

        private final RtcContext mRtcContext;

        RtcListener(RtcContext context) {
            mRtcContext = context;
        }

        @Override
        public void onJoinChannelResult(int result, String channel, String userId, int elapsed) {
            log("[Callback] onJoinChannelResult : " + result + ", channel : " + channel + ", uid : " + userId + ", elapsed: " + elapsed);
            if (mRtcContext != null) {
                mRtcContext.joinFinished = true;
                mRtcContext.joinResult = result;
            }
        }

        @Override
        public void onLeaveChannelResult(int result) {
            log("[Callback] onLeaveChannelResult : " + result);
        }


    }

    static class AudioObserver extends DingRtcEngine.DingRtcAudioFrameObserver {
        private final RtcContext mRtcContext;
        private FileOutputStream mFile;

        public AudioObserver(RtcContext context) {
            mRtcContext = context;
        }

        @Override
        public boolean onCapturedAudioFrame(DingRtcEngine.DingRtcAudioFrame frame) {
            return false;
        }

        @Override
        public boolean onProcessCapturedAudioFrame(DingRtcEngine.DingRtcAudioFrame frame) {
            return false;
        }

        @Override
        public boolean onPublishAudioFrame(DingRtcEngine.DingRtcAudioFrame frame) {
            return false;
        }

        @Override
        public boolean onPlaybackAudioFrame(DingRtcEngine.DingRtcAudioFrame frame) {
            // 拉流播放数据
            // TODO do save or other logic, ex: asr
            return false;
        }
    }

    static class VideoObserver extends DingRtcEngine.DingRtcVideoFrameObserver {

        private final RtcContext mRtcContext;
        private FileOutputStream mFile;

        public VideoObserver(RtcContext context) {
            mRtcContext = context;
        }

        @Override
        public DingRtcEngine.DingRtcVideoFormat getVideoFormatPreference() {
            // 期望数据格式，目前仅支持I420格式
            return DingRtcEngine.DingRtcVideoFormat.DingRtcVideoI420;
        }

        @Override
        public boolean onCaptureVideoFrame(DingRtcEngine.DingRtcVideoSample videoSample) {
            return false;
        }

        @Override
        public boolean onPreEncodeVideoFrame(DingRtcEngine.DingRtcVideoTrack track, DingRtcEngine.DingRtcVideoSample videoSample) {
            return false;
        }

        @Override
        public boolean onRemoteVideoFrame(String uid, DingRtcEngine.DingRtcVideoTrack track, DingRtcEngine.DingRtcVideoSample videoSample) {
            // 远端用户视频数据回调
            // TODO do save or other logic
            return false;
        }

    }

    private static void log(Object... args) {
        if (args != null && args.length > 0) {
            for (Object o : args) {
                if (o != null) {
                    System.out.print(o);
                }
            }
            System.out.println();
        }
    }

    private static void sleep(long millis) {
        try {
            Thread.sleep(millis);
        } catch (InterruptedException e) {
            throw new RuntimeException(e);
        }
    }

    public static void main(String[] args) {
        log("[Demo] main start");
        LinuxServerPullDemo demo = new LinuxServerPullDemo();
        demo.run(args);
        log("[Demo] main finish");
    }

    public void run(String[] args) {
        log("************** [Demo] begin   **************  \n");
        log("[Demo] current dir : " + System.getProperty("user.dir"));
        RtcContext context = new RtcContext();

        // 1. 初始化SDK
        step_1_init_sdk(context);

        // 2. 入会
        step_2_join(context);

        // 为了演示方便，同步等待join成功，等onJoinChannelResult回调触发
        if (!wait_join_result(context)) {
            // join fail
            return;
        }

        // 3. 开始拉音视频流
        step_3_start_pull_audio_video_data(context);

        // 为了演示方便，同步等10s后，主动停止拉流
        sleep(10 * 1000);

        // 4. 停止拉音频、视频裸数据
        step_4_stop_pull_audio_video_data(context);

        // 5. 离会
        step_5_leave(context);

        // 6. 销毁
        step_6_destroy(context);

        context.reset();
        log("************** [Demo] end   **************  \n");
    }

    private boolean wait_join_result(RtcContext context) {
        while (!context.joinFinished) {
            sleep(500);
        }
        if (context.joinResult != 0) {
            // 入会失败，退出
            // 需要检查authinfo是否请求正确
            log("[Demo] join failed, return");
            context.reset();
            return false;
        }
        return true;
    }

    private void step_1_init_sdk(RtcContext context) {
        initRtcEngine(context);
        if (context.engine != null) {
            // 设置是否订阅音频流，默认会自动订阅，可以不调用
            // note:目前音频流仅支持合流订阅，单人订阅后续支持
            context.engine.subscribeAllRemoteAudioStreams(true);

            // 设置是否订阅视频流，默认会自动订阅，可以不调用
            // 也可以仅订阅单人 context.engine.subscribeRemoteVideoStream(userId, DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackCamera, true);
            context.engine.subscribeAllRemoteVideoStreams(true);
        }
    }

    private void step_2_join(RtcContext context) {
        DingRtcEngine.DingRtcAuthInfo authInfo = getAuthInfoFromServer(context);
        if (context.engine != null) {
            context.engine.joinChannel(authInfo, "张三");
        }
    }

    private void step_3_start_pull_audio_video_data(RtcContext context) {
        startPullAudioData(context);
        startPullVideoData(context);
    }

    private void step_4_stop_pull_audio_video_data(RtcContext context) {
        stopPullAudioData(context);
        stopPullVideoData(context);
    }

    private void step_5_leave(RtcContext context) {
        if (context.engine != null) {
            context.engine.leaveChannel();
        }
    }

    private void step_6_destroy(RtcContext context) {
        if (context.engine != null) {
            context.engine.destroy();
            context.engine = null;
        }
    }

    /**
     * 停止音频拉流
     */
    private void stopPullAudioData(RtcContext context) {
        if (!context.isPullAudioStarted) {
            return;
        }
        if (context.engine != null) {
            // 1. 关闭音频数据订阅位置
            context.engine.enableAudioFrameObserver(false, DingRtcEngine.DintRtcAudioObservePosition.RtcEngineAudioPositionPlayback);

            // 2. 取消注册observer
            context.engine.registerAudioFrameObserver(null);

            // 3. 关闭音量提示和说话人提示功能，interval <= 0 代表关闭
            // 相关信息见回调： onAudioVolumeIndication
            context.engine.enableAudioVolumeIndication(0, 3, 1);
        }
        if (context.audioObserver != null) {
            context.audioObserver = null;
        }
        context.isPullAudioStarted = false;
    }

    /**
     * 开始音频拉流，获取播放PCM数据（支持配置采样率和通道数）
     * PCM数据见 AudioObserver::onPlaybackAudioFrame
     */
    private void startPullAudioData(RtcContext context) {
        if (context.isPullAudioStarted) {
            return;
        }
        if (context.audioObserver != null) {
            context.audioObserver = null;
        }
        if (context.engine != null) {
            // 1. 创建音频数据输出observer，并注册到engine中
            context.audioObserver = new AudioObserver(context);
            context.engine.registerAudioFrameObserver(context.audioObserver);

            // 2. 开启音频数据订阅位置，并设置期望输出的音频PCM数据采样率和通道数
            DingRtcEngine.DingRtcAudioFrameObserverConfig config = new DingRtcEngine.DingRtcAudioFrameObserverConfig();
            config.sampleRate = DingRtcEngine.DingRtcAudioSampleRate.DingRtcAudioSampleRate_16000;
            config.channels = DingRtcEngine.DingRtcAudioNumChannelType.DingRtcMonoAudio;
            context.engine.enableAudioFrameObserver(true, DingRtcEngine.DintRtcAudioObservePosition.RtcEngineAudioPositionPlayback, config);

            // 3. 开启音量提示和说话人提示功能，interval > 0 代表开启，<= 0 代表关闭
            // 相关信息见回调： onAudioVolumeIndication
            context.engine.enableAudioVolumeIndication(300, 3, 1);
        }
        context.isPullAudioStarted = true;
    }

    /**
     * 停止视频拉流
     */
    private void stopPullVideoData(RtcContext context) {
        if (!context.isPullVideoStarted) {
            return;
        }
        if (context.engine != null) {
            // 1. 关闭视频数据订阅位置
            context.engine.enableVideoFrameObserver(false, DingRtcEngine.DingRtcVideoObservePosition.DingRtcPositionPreRender);

            // 2. 移除视频数据回调observer
            context.engine.registerVideoFrameObserver(null);
        }
        if (context.videoObserver != null) {
            context.videoObserver = null;
        }
        context.isPullVideoStarted = false;
    }

    /**
     * 开始视频拉流，获取远端视频YUV数据
     * YUV数据见 VideoObserver::onRemoteVideoFrame
     */
    private void startPullVideoData(RtcContext context) {
        if (context.isPullVideoStarted) {
            return;
        }
        if (context.videoObserver != null) {
            context.videoObserver = null;
        }
        if (context.engine != null) {
            // 1. 创建视频数据输出observer，并注册到engine中
            context.videoObserver = new VideoObserver(context);
            context.engine.registerVideoFrameObserver(context.videoObserver);

            // 2. 开启视频数据订阅位置（支持'|'操作同时开启多个位置）
            context.engine.enableVideoFrameObserver(true,
                    DingRtcEngine.DingRtcVideoObservePosition.DingRtcPositionPreRender);
        }
        context.isPullVideoStarted = true;
    }

    /**
     * 初始化引擎
     */
    private void initRtcEngine(RtcContext context) {
        String dir = System.getProperty("user.dir");
        // 设置日志路径
        DingRtcEngine.setLogDirPath(dir);
        // 初始化回调
        RtcListener listener = new RtcListener(context);
        // 初始化引擎
        DingRtcEngine engine = DingRtcEngine.createInstance(listener, "");

        context.engine = engine;
        context.listener = listener;
    }

    /**
     * TODO request from app server
     * @return DingRtcAuthInfo
     */
    private DingRtcEngine.DingRtcAuthInfo getAuthInfoFromServer(RtcContext context) {
        DingRtcEngine.DingRtcAuthInfo authInfo = new DingRtcEngine.DingRtcAuthInfo();
        authInfo.appId = "";
        authInfo.channelId = "";
        authInfo.userId = "";
        authInfo.token = "";
        // 注意 gslb字段推荐填空串
        authInfo.gslb = "";
        return authInfo;
    }
}
