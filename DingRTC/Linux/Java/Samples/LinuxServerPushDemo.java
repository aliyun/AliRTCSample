package samples;

import com.ding.rtc.DingRtcEngine;
import com.ding.rtc.DingRtcEngineListener;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.RandomAccessFile;
import java.io.UnsupportedEncodingException;
import java.net.HttpURLConnection;
import java.net.URL;
import java.net.URLEncoder;
import java.nio.charset.StandardCharsets;
import java.util.Random;

public class LinuxServerPushDemo {

    static class RtcContext {
        DingRtcEngine engine = null;
        DingRtcEngineListener listener = null;

        volatile boolean joinFinished = false;
        int joinResult = -1;

        volatile boolean quit = false;

        // audio config
        String pcmFilePath;
        int pcmSampleRate = 16000;
        int pcmChannels = 1;
        int pcmReadFreq = 40;
        volatile boolean isPushAudioStarted = false;
        volatile boolean externalAudioFinished = false;
        Thread externalAudioThread;

        // video config
        String videoYUVFilePath;
        DingRtcEngine.DingRtcVideoFormat videoPixelFormat = DingRtcEngine.DingRtcVideoFormat.DingRtcVideoI420;
        int videoWidth = 0;
        int videoHeight = 0;
        int videoRotation = 0;
        int videoFps = 25;
        volatile boolean isPushVideoStarted = false;
        volatile boolean externalVideoFinished = false;
        Thread externalVideoThread;

        void reset() {
            if (engine != null) {
                engine.destroy();
            }
            engine = null;
            listener = null;
            if (externalAudioThread != null) {
                try {
                    externalAudioThread.join();
                } catch (InterruptedException e) {
                    log("[Demo] reset audio thread error:", e.toString());
                }
            }
            if (externalVideoThread != null) {
                try {
                    externalVideoThread.join();
                } catch (InterruptedException e) {
                    log("[Demo] reset video thread error:", e.toString());
                }
            }
        }

    }

    static class RtcListener extends DingRtcEngineListener {

        private final RtcContext mRtcContext;

        RtcListener(RtcContext context) {
            mRtcContext = context;
        }

        @Override
        public void onJoinChannelResult(int result, String channel, String userId, int elapsed) {
            log("[Callback] onJoinChannelResult : " + result + ", channel : " + channel + ", uid : " + userId);
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
        LinuxServerPushDemo demo = new LinuxServerPushDemo();
        demo.run(args);
        log("[Demo] main finish");
    }

    public void run(String[] args) {
        log("************** [Demo] begin   **************  \n");

        /**
         * 配置音频、视频推流资源文件和相关参数
         * 该示例音视频源为文件，其他形式资源流程基本一致
         */
        RtcContext context = new RtcContext();
        // 音频输入资源
        context.pcmFilePath = "32000.pcm";
        context.pcmSampleRate = 32000;
        context.pcmChannels = 1;
        // 一次读取40ms数据，控制io读取频率
        context.pcmReadFreq = 25;

        // 视频输入资源
        context.videoYUVFilePath = "720_960.yuv";
        context.videoPixelFormat = DingRtcEngine.DingRtcVideoFormat.DingRtcVideoI420;
        context.videoWidth = 720;
        context.videoHeight = 960;
        context.videoRotation = 0;
        context.videoFps = 25;


        // 1. 初始化SDK
        step_1_init_sdk(context);

        // 2. 入会
        step_2_join(context);

        // 为了演示方便，同步等待join成功，等onJoinChannelResult回调触发
        if (!wait_join_result(context)) {
            // join fail
            return;
        }

        // 3. 开始推音视频流
        step_3_start_push_audio_video_data(context);

        // 为了演示方便，同步等10s后，主动停止推流
        sleep(10 * 1000);
        // 设置结束flag，结束推流线程
        context.quit = true;

        // 4. 停止推音视频流
        step_4_stop_push_audio_video_data(context);

        // 5. 离会
        step_5_leave(context);

        // 6. 销毁
        step_6_destroy(context);

        context.reset();
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
    }

    private void step_2_join(RtcContext context) {
        DingRtcEngine.DingRtcAuthInfo authInfo = getAuthInfoFromServer(context);
        if (context.engine != null) {
            context.engine.joinChannel(authInfo, "张三");
        }
    }

    private void step_3_start_push_audio_video_data(RtcContext context) {
        startPushAudio(context);
        startPushVideo(context);
    }

    private void step_4_stop_push_audio_video_data(RtcContext context) {
        stopPushAudio(context);
        stopPushVideo(context);
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
     * 停止推音频流数据
     */
    private void stopPushAudio(RtcContext context) {
        if (!context.isPushAudioStarted) {
            return;
        }
        context.isPushAudioStarted = false;
        if (context.engine != null) {
            // 关闭推流
            context.engine.publishLocalAudioStream(false);
            // 关闭设置音频PCM推流
            context.engine.setExternalAudioSource(false, context.pcmSampleRate, context.pcmChannels);
        }
        if (context.externalAudioThread != null) {
            try {
                context.externalAudioThread.join();
            } catch (InterruptedException e) {
                log("[Demo] doStopAudio thread error:", e.toString());
            }
        }
    }

    /**
     * 开始推音频流数据
     */
    private void startPushAudio(RtcContext context) {
        if (context.isPushAudioStarted) {
            return;
        }
        context.isPushAudioStarted = true;
        if (context.engine != null) {
            // 开启推流
            context.engine.publishLocalAudioStream(true);
            // 设置音频PCM推流
            context.engine.setExternalAudioSource(true, context.pcmSampleRate, context.pcmChannels);
        }
        context.externalAudioThread = new Thread(() -> {
            DingRtcEngine.DingRtcAudioFrame frame = new DingRtcEngine.DingRtcAudioFrame();
            try (RandomAccessFile file = new RandomAccessFile(context.pcmFilePath, "r")) {
                long sampleCount = 0;
                // 16-bit pcm, 2byte
                int bytesPerSample = 2;
                int samplesToRead = context.pcmSampleRate / (1000 / context.pcmReadFreq);
                int bufferSize = samplesToRead * context.pcmChannels * bytesPerSample;
                byte[] buffer = new byte[bufferSize];
                long delay = 0;
                long startClock = System.currentTimeMillis();
                long lastStatsClock = startClock;
                while (!context.quit) {
                    long lastPos = file.getFilePointer();
                    int readBytes = file.read(buffer);
                    if (readBytes != bufferSize) {
                        // 读完文件，直接退出
                        // break;

                        // 如果想循环读
                        file.seek(0);
                        continue;
                    }
                    frame.bytesPerSample = bytesPerSample;
                    frame.samplesPerSec = context.pcmSampleRate;
                    frame.numChannels = context.pcmChannels;
                    frame.buffer = buffer;
                    frame.numSamples = readBytes / frame.bytesPerSample / frame.numChannels;
                    frame.timestamp = sampleCount * 1000 / context.pcmSampleRate;

                    delay = frame.timestamp;
                    long elapsed = System.currentTimeMillis() - startClock;
                    if (delay - elapsed > 5) {
                        sleep(delay - elapsed);
                    }

                    if (frame.numSamples > 0) {
                        int ret = context.engine.pushExternalAudioFrame(frame);
                        if (ret == 0) {
                            sampleCount += frame.numSamples;
                        } else {
                            file.seek(lastPos);
                            // sdk buffer full, wait some time ,ex: 40ms
                            if (ret == DingRtcEngine.DingRtcErrorCode.DingRtcErrorAudioBufferFull.getValue()) {
                                sleep(40);
                            }
                        }
                    }

                    long elpasedStats = System.currentTimeMillis() - lastStatsClock;
                    if (elpasedStats >= 2000) {
                        log("[Demo] audioPushThread pushExternalAudioFrame sampleCount:", sampleCount);
                        lastStatsClock = System.currentTimeMillis();
                    }
                }
            } catch (Exception e) {
                log("[Demo] pushAudio error:", e.toString());
            } finally {
                context.externalAudioFinished = true;
            }
        }, "pub-audio");
        context.externalAudioThread.start();
    }

    /**
     * 停止推视频流数据
     */
    private void stopPushVideo(RtcContext context) {
        if (!context.isPushVideoStarted) {
            return;
        }
        context.isPushVideoStarted = false;
        if (context.engine != null) {
            // 关闭推流
            context.engine.publishLocalVideoStream(false);
            context.engine.setExternalVideoSource(false, DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackCamera);
        }
        if (context.externalVideoThread != null) {
            try {
                context.externalVideoThread.join();
            } catch (InterruptedException e) {
                log("[Demo] doStopAudio thread error:", e.toString());
            }
        }
    }

    /**
     * 开始推视频流数据
     */
    private void startPushVideo(RtcContext context) {
        if (context.isPushVideoStarted) {
            return;
        }
        context.isPushVideoStarted = true;
        if (context.engine != null) {
            // 开启推流
            context.engine.publishLocalVideoStream(true);
            // 设置音频PCM推流
            context.engine.setExternalVideoSource(true, DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackCamera);
        }
        context.externalVideoThread = new Thread(() -> {
            try (RandomAccessFile file = new RandomAccessFile(context.videoYUVFilePath, "r")) {
                long frameCount = 0;
                DingRtcEngine.DingRtcVideoFormat pixelFormat = context.videoPixelFormat;
                int bufferSize = calcBufferSize(pixelFormat, context.videoWidth, context.videoHeight);

                long delay = 0;
                long startClock = System.currentTimeMillis();
                long lastStatsClock = startClock;
                byte[] buffer = new byte[bufferSize];
                DingRtcEngine.DingRtcVideoFrame frame = new DingRtcEngine.DingRtcVideoFrame();
                while (!context.quit) {
                    long readStartClock = System.currentTimeMillis();
                    long lastPos = file.getFilePointer();
                    int readBytes = file.read(buffer);
                    if (readBytes != bufferSize) {
                        // 读完文件，直接退出
                        // break;

                        // 如果想循环读
                        file.seek(0);
                        continue;
                    }
                    frame.format = pixelFormat;
                    frame.width = context.videoWidth;
                    frame.height = context.videoHeight;
                    frame.lineSize[0] = context.videoWidth;
                    frame.lineSize[1] = context.videoWidth >> 1;
                    frame.lineSize[2] = context.videoWidth >> 1;
                    frame.rotation = context.videoRotation;
                    frame.videoFrameLength = bufferSize;
                    frame.buffer = buffer;
                    frame.timestamp = frameCount * 1000 / context.videoFps;

                    delay = frame.timestamp;
                    long elapsed = System.currentTimeMillis() - startClock;
                    if (delay - elapsed > 5) {
                        sleep(delay - elapsed);
                    }

                    int ret = context.engine.pushExternalVideoFrame(frame, DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackCamera);
                    if (ret == 0) {
                        frameCount++;
                    } else {
                        file.seek(lastPos);
                        if (ret == DingRtcEngine.DingRtcErrorCode.DingRtcErrorVideoBufferFull.getValue()) {
                            // sdk buffer full, wait some time ,ex: 40ms
                            sleep(40);
                        }
                    }

                    long elpasedStats = System.currentTimeMillis() - lastStatsClock;
                    if (elpasedStats >= 5000) {
                        log("[Demo] videoPushThread pushExternalVideoFrame frameCount:", frameCount);
                        lastStatsClock = System.currentTimeMillis();
                    }
                }
            } catch (Exception e) {
                log("[Demo] pushVideo error:", e.toString());
            } finally {
                context.externalVideoFinished = true;
            }
        }, "pub-video");
        context.externalVideoThread.start();
    }

    /**
     * 计算视频一帧数据大小
     */
    int calcBufferSize(DingRtcEngine.DingRtcVideoFormat pixel_format, int width, int height) {
        int buffer_size = 0;
        switch (pixel_format) {
            case DingRtcVideoI420:
            case DingRtcVideoNV12:
            case DingRtcVideoNV21: {
                int half_width = (width + 1) >> 1;
                int half_height = (height + 1) >> 1;
                buffer_size = width * height + half_width * half_height * 2;
                break;
            }
            case DingRtcVideoBGRA:
            case DingRtcVideoARGB:
            case DingRtcVideoRGBA:
            case DingRtcVideoABGR:
                buffer_size = width * height * 4;
                break;
            default:
                break;
        }
        return buffer_size;
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



