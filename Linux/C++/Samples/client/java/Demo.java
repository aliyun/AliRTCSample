package demo;

import com.ding.rtc.DingRtcEngine;
import com.ding.rtc.DingRtcEngineListener;
import com.ding.rtc.DingRtmClient;
import com.ding.rtc.DingRtmEventListener;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.*;
import java.net.HttpURLConnection;
import java.net.URL;
import java.net.URLEncoder;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Paths;

/*
 *  demo run : ./demo.sh
 */
public class Demo {

    static String CMD_JOIN = "1";
    static String CMD_LEAVE = "2";
    static String CMD_AUDIO_START = "3";
    static String CMD_AUDIO_STOP = "4";
    static String CMD_VIDEO_START = "5";
    static String CMD_VIDEO_STOP = "6";
    static String CMD_RTM_SEND = "7";
    static String CMD_VIDEO_BITRATE = "8";
    static String CMD_VIDEO_FPS = "9";
    static String CMD_VIDEO_DIMEN = "10";
    static String CMD_EXIT = "0";

    static class ManualState {
        boolean isJoined = false;
        boolean isAudioStarted = false;
        boolean isVideoStarted = false;
    }

    static class RtcContext {
        DingRtcEngine engine = null;
        DingRtcEngineListener listener = null;
        AudioObserver audioFrameObserver = null;

        String appServer;
        String appId;
        String userId;
        String userName;
        String channelName;
        String env;
        String passwd;

        int joinLeaveCount = 1;
        long sleepMsBeforeLeave = 3000;

        boolean publishAudio = true;
        boolean publishVideo = true;
        boolean subscribeAllAudio = true;
        boolean subscribeAllVideo = true;

        boolean pushExternalAudio = true;
        String pcmFilePath;
        int pcmSampleRate = 16000;
        int pcmChannels = 1;
        int pcmReadFreq = 40;

        boolean pushExternalVideo = true;
        String videoFilePath;
        String videoPixelFormat = "I420";
        int videoWidth = 1920;
        int videoHeight = 1080;
        int videoFps = 30;
        int videoRotation = 0;
        int videoBitrate = 0;

        long loopPlayCount = 0;

        boolean joinResponsed = false;
        int joinResult = -1;

        volatile boolean quit = false;

        volatile boolean externalAudioFinished = false;
        Thread externalAudioThread;

        volatile boolean externalVideoFinished = false;
        Thread externalVideoThread;

        boolean manualControl = false;
        ManualState manualState = new ManualState();

        // rtm
        int rtmSendCount = 0;
        String rtmSessionId = "";
        String rtmRemoteUid = "";

        public boolean parseConfig(JSONObject json) {
            try {
                appServer = getString(json, "appServer", "");
                appId = getString(json, "appId", "");
                userId = getString(json, "userId", "");
                userName = getString(json, "userName", "");
                channelName = getString(json, "channelName", "");
                env = getString(json, "env", "");
                passwd = getString(json, "passwd", "");

                joinLeaveCount = getInt(json, "joinLeaveCount", 1);
                sleepMsBeforeLeave = getInt(json, "sleepMsBeforeLeave", 3000);

                publishAudio = getBoolean(json, "publishAudio", true);
                publishVideo = getBoolean(json, "publishVideo", true);
                subscribeAllAudio = getBoolean(json, "subscribeAllAudio", false);
                subscribeAllVideo = getBoolean(json, "subscribeAllVideo", false);

                pushExternalAudio = getBoolean(json, "pushExternalAudio", true);
                pcmFilePath = getString(json, "pcmFilePath", "");
                pcmSampleRate = getInt(json, "pcmSampleRate", 48000);
                pcmChannels = getInt(json, "pcmChannels", 2);
                pcmReadFreq = getInt(json, "pcmReadFreq", 40);

                pushExternalVideo = getBoolean(json, "pushExternalVideo", true);
                videoFilePath = getString(json, "videoFilePath", "");
                videoPixelFormat = getString(json, "videoPixelFormat", "I420");
                videoWidth = getInt(json, "videoWidth", 640);
                videoHeight = getInt(json, "videoHeight", 480);
                videoFps = getInt(json, "videoFps", 20);
                videoRotation = getInt(json, "videoRotation", 0);
                videoBitrate = getInt(json, "videoBitrate", 0);
                loopPlayCount = getInt(json, "loopPlayCount", 1);
                rtmSessionId = getString(json, "rtmSessionId", "");
                rtmSendCount = 0;
                return true;
            } catch (JSONException e) {
                log("[Demo] parseConfig error:", e.toString());
                return false;
            }
        }

        String getString(JSONObject json, String field, String defaultValue) {
            if (json.has(field)) {
                return json.getString(field);
            }
            return defaultValue;
        }

        boolean getBoolean(JSONObject json, String field, boolean defaultValue) {
            if (json.has(field)) {
                return json.getBoolean(field);
            }
            return defaultValue;
        }

        int getInt(JSONObject json, String field, int defaultValue) {
            if (json.has(field)) {
                return json.getInt(field);
            }
            return defaultValue;
        }

        void reset() {
            if (engine != null) {
                engine.destroy();
            }
            listener = null;
            if (audioFrameObserver != null) {
                audioFrameObserver.close();
            }
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
        public void onError(int errorCode) {
            log("[Callback] onError : " + errorCode);
        }

        @Override
        public void onWarning(int warningCode) {
            log("[Callback] onWarning : " + warningCode);
        }

        @Override
        public void onJoinChannelResult(int result, String channel, String userId, int elapsed) {
            log("[Callback] onJoinChannelResult : " + result + ", channel : " + channel + ", uid : " + userId);
            if (mRtcContext != null) {
                mRtcContext.joinResponsed = true;
                mRtcContext.joinResult = result;
            }
        }

        @Override
        public void onLeaveChannelResult(int result) {
            log("[Callback] onLeaveChannelResult : " + result);
        }

        @Override
        public void onRemoteUserOnLineNotify(String uid, int elapsed) {
            log("[Callback] onRemoteUserOnLineNotify : " + uid);
        }

        @Override
        public void onRemoteUserOffLineNotify(String uid, DingRtcEngine.DingRtcUserOfflineReason reason) {
            log("[Callback] onRemoteUserOffLineNotify : " + uid + ", reason : " + reason);
        }

        @Override
        public void onAudioPublishStateChanged(DingRtcEngine.DingRtcPublishState oldState, DingRtcEngine.DingRtcPublishState newState, int i, String s) {
            log("[Callback] onAudioPublishStateChanged : " + oldState + ", " + newState);
        }

        @Override
        public void onVideoPublishStateChanged(DingRtcEngine.DingRtcPublishState oldState, DingRtcEngine.DingRtcPublishState newState, int i, String s) {
            log("[Callback] onVideoPublishStateChanged : " + oldState + ", " + newState);
        }

        @Override
        public void onFirstAudioPacketSend(int i) {
            log("[Callback] onFirstAudioPacketSend : " + i);
        }

        @Override
        public void onFirstVideoPacketSend(DingRtcEngine.DingRtcVideoTrack dingRtcVideoTrack, int i) {
            log("[Callback] onFirstVideoPacketSend : " + dingRtcVideoTrack);
        }

        @Override
        public void onConnectionStatusChanged(DingRtcEngine.DingRtcConnectionStatus status, DingRtcEngine.DingRtcConnectionStatusChangeReason reason) {
            log("[Callback] onConnectionStatusChange : " + status + ", " + reason);
        }

        @Override
        public void onBye(DingRtcEngine.DingRtcOnByeType dingRtcOnByeType) {
            log("[Callback] onBye : " + dingRtcOnByeType);
        }

        @Override
        public void onNetworkQualityChanged(String s, DingRtcEngine.DingRtcNetworkQuality dingRtcNetworkQuality, DingRtcEngine.DingRtcNetworkQuality dingRtcNetworkQuality1) {
            log("[Callback] onNetworkQualityChanged : " + s + ", " + dingRtcNetworkQuality + ", " + dingRtcNetworkQuality1);
        }

        @Override
        public void onRemoteTrackAvailableNotify(String s, DingRtcEngine.DingRtcAudioTrack dingRtcAudioTrack, DingRtcEngine.DingRtcVideoTrack dingRtcVideoTrack) {
            log("[Callback] onRemoteTrackAvailableNotify : " + s + ", " + dingRtcAudioTrack + ", " + dingRtcVideoTrack);
        }

        @Override
        public void onUserAudioMuted(String s, boolean b) {
            log("[Callback] onUserAudioMuted : " + s + ", " + b);
        }

        @Override
        public void onUserVideoMuted(String s, boolean b, DingRtcEngine.DingRtcVideoTrack dingRtcVideoTrack) {
            log("[Callback] onUserVideoMuted : " + s + ", " + b + ", " + dingRtcVideoTrack);
        }

        @Override
        public void onFirstVideoPacketReceived(String s, DingRtcEngine.DingRtcVideoTrack dingRtcVideoTrack, int i) {
            log("[Callback] onFirstVideoPacketReceived : " + s + ", " + dingRtcVideoTrack);
        }

        @Override
        public void onFirstVideoFrameReceived(String s, DingRtcEngine.DingRtcVideoTrack dingRtcVideoTrack, int i) {
            log("[Callback] onFirstVideoFrameReceived : " + s + ", " + dingRtcVideoTrack);
        }

        @Override
        public void onFirstAudioPacketReceived(String s, int i) {
            log("[Callback] onFirstAudioPacketReceived : " + s + ", " + i);
        }

        @Override
        public void onAudioSubscribeStateChanged(String s, DingRtcEngine.DingRtcSubscribeState dingRtcSubscribeState, DingRtcEngine.DingRtcSubscribeState dingRtcSubscribeState1, int i, String s1) {
            log("[Callback] onAudioSubscribeStateChanged : " + s + ", " + dingRtcSubscribeState + ", " + dingRtcSubscribeState1);
        }

        @Override
        public void onVideoSubscribeStateChanged(String s, DingRtcEngine.DingRtcSubscribeState dingRtcSubscribeState, DingRtcEngine.DingRtcSubscribeState dingRtcSubscribeState1, int i, String s1) {
            log("[Callback] onVideoSubscribeStateChanged : " + s + ", " + dingRtcSubscribeState + ", " + dingRtcSubscribeState1);
        }

        @Override
        public void onScreenShareSubscribeStateChanged(String uid, DingRtcEngine.DingRtcSubscribeState oldState, DingRtcEngine.DingRtcSubscribeState newState, int elapseSinceLastState, String channel) {
            System.out.println("[Callback] onScreenShareSubscribeStateChanged : " + uid + ", " + oldState + ", " + newState);
        }

        @Override
        public void onSubscribeStreamTypeChanged(String s, DingRtcEngine.DingRtcVideoStreamType dingRtcVideoStreamType, DingRtcEngine.DingRtcVideoStreamType dingRtcVideoStreamType1, int i, String s1) {
            log("[Callback] onSubscribeStreamTypeChanged : " + s + ", " + dingRtcVideoStreamType + ", " + dingRtcVideoStreamType1);
        }

        @Override
        public void onLocalAudioStats(DingRtcEngine.DingRtcLocalAudioStats dingRtcLocalAudioStats) {
            log("[Callback] onLocalAudioStats : " + dingRtcLocalAudioStats.toString());
        }

        @Override
        public void onLocalVideoStats(DingRtcEngine.DingRtcLocalVideoStats dingRtcLocalVideoStats) {
            log("[Callback] onLocalVideoStats : " + dingRtcLocalVideoStats.toString());
        }

        @Override
        public void onRemoteAudioStats(DingRtcEngine.DingRtcRemoteAudioStats dingRtcRemoteAudioStats) {
            log("[Callback] onRemoteAudioStats : " + dingRtcRemoteAudioStats.toString());
        }

        @Override
        public void onRemoteVideoStats(DingRtcEngine.DingRtcRemoteVideoStats dingRtcRemoteVideoStats) {
            log("[Callback] onRemoteVideoStats : " + dingRtcRemoteVideoStats.toString());
        }

        @Override
        public void onStats(DingRtcEngine.DingRtcStats dingRtcStats) {
            log("[Callback] onStats : " + dingRtcStats.toString());
        }

        @Override
        public void onSnapshotComplete(String uid, DingRtcEngine.DingRtcVideoTrack track, String filePath, int width, int height, boolean success) {
            super.onSnapshotComplete(uid, track, filePath, width, height, success);
            log("[Callback] onSnapshotComplete : " + uid + ", " + track + ", " + filePath + ", " + width + " x " + height + ", " + success);
        }
    }

    static class AudioObserver extends DingRtcEngine.DingRtcAudioFrameObserver {
        private final RtcContext mRtcContext;
        private FileOutputStream mFile;

        public AudioObserver(RtcContext context) {
            mRtcContext = context;
        }

        @Override
        public void onCapturedAudioFrame(DingRtcEngine.DingRtcAudioFrame frame) {
            //log("[Callback] [Audio] onCapturedAudioFrame : " + frame.buffer.length + ", " + frame.timestamp);
            return ;
        }

        @Override
        public void onProcessCapturedAudioFrame(DingRtcEngine.DingRtcAudioFrame frame) {
            //log("[Callback] [Audio] onProcessCapturedAudioFrame : " + frame.buffer.length + ", " + frame.timestamp);
            return ;
        }

        @Override
        public void onPublishAudioFrame(DingRtcEngine.DingRtcAudioFrame frame) {
            //log("[Callback] [Audio] onPublishAudioFrame : " + frame.buffer.length + ", " + frame.timestamp);
            return ;
        }

        @Override
        public void onPlaybackAudioFrame(DingRtcEngine.DingRtcAudioFrame frame) {
            //log("[Callback] [Audio] onPlaybackAudioFrame : " + frame.buffer.length + ", " + frame.timestamp);
            if (mFile == null) {
                String dir = System.getProperty("user.dir");
                String file = String.format("audio_playback_%d_%d_%s.pcm", frame.samplesPerSec, frame.numChannels, System.currentTimeMillis());
                try {
                    mFile = new FileOutputStream(dir + File.separator + file);
                } catch (Exception e) {
                    log("[Demo] audio observer output create error:", e.toString());
                }
            }
            if (mFile != null) {
                try {
                    if (frame.buffer != null && frame.buffer.length > 0) {
                        mFile.write(frame.buffer);
                    }
                } catch (IOException ignored) {
                }
            }
            return ;
        }

        public void close() {
            if (mFile != null) {
                try {
                    mFile.close();
                } catch (IOException ignored) {
                }
            }
        }
    }

    static class VideoObserver extends DingRtcEngine.DingRtcVideoFrameObserver {

        private final RtcContext mRtcContext;

        public VideoObserver(RtcContext context) {
            mRtcContext = context;
        }

        @Override
        public DingRtcEngine.DingRtcVideoFormat getVideoFormatPreference() {
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
            return false;
        }
    }

    static class RtmListener extends DingRtmEventListener {

        private final RtcContext ctx;

        RtmListener(RtcContext c) {
            ctx = c;
        }

        public void onRtmServerStateChanged(DingRtmClient.DingRtmServerState state, int reason) {
            log("[Callback][RTM] onRtmServerStateChanged state: " + state + ", reason: " + reason);
            if (state == DingRtmClient.DingRtmServerState.Available) {
                ctx.engine.getRtmClient().joinSession(ctx.rtmSessionId);
            }
        }

        public void onJoinSessionResult(String sessionId, int result) {
            log("[Callback][RTM] onJoinSessionResult sessionId: " + sessionId + ", result: " + result);
        }

        public void onLeaveSessionResult(String sessionId, int reason) {
            log("[Callback][RTM] onLeaveSessionResult sessionId: " + sessionId + ", reason: " + reason);
        }

        public void onCloseSessionResult(String sessionId, int result) {
            log("[Callback][RTM] onCloseSessionResult sessionId: " + sessionId + ", result: " + result);
        }

        public void onRemovedFromSession(String sessionId, int reason) {
            log("[Callback][RTM] onRemovedFromSession sessionId: " + sessionId + ", reason: " + reason);
        }

        public void onSessionCreate(String sessionId) {
            log("[Callback][RTM] onSessionCreate sessionId: " + sessionId);
        }

        public void onSessionClose(String sessionId) {
            log("[Callback][RTM] onSessionClose sessionId: " + sessionId);
        }

        public void onSessionRemoteUserJoin(String sessionId, String uid) {
            log("[Callback][RTM] onSessionRemoteUserJoin sessionId: " + sessionId + ", uid: " + uid);
            if ("".equals(ctx.rtmRemoteUid)) {
                ctx.rtmRemoteUid = uid;
            }
        }

        public void onSessionRemoteUserLeave(String sessionId, String uid) {
            log("[Callback][RTM] onSessionRemoteUserLeave sessionId: " + sessionId + ", uid: " + uid);
        }

        public void onMessage(String sessionId, String fromUid, boolean broadcast, byte[] data) {
            String msg = new String(data, StandardCharsets.UTF_8);
            log("[Callback][RTM] onMessage sessionId: " + sessionId + ", fromUid: " + fromUid + ", broadcast: " + broadcast + ", data: " + msg);
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
        log("************** [Demo] begin   **************  \n");
        log("[Demo] current dir : " + System.getProperty("user.dir"));
        String appConfigPath = "app_config.json";
        RtcContext context = new RtcContext();
        String fifoPath = "/tmp/linux_java_pipe";
        if (args != null) {
            if (args.length >= 1) {
                fifoPath = args[0];
                context.manualControl = true;
                log("[Demo] is in manual mode, " + fifoPath);
            } else {
                log("[Demo] is in auto mode");
            }
        }
        if (parseAppConfig(context, appConfigPath)) {
            initRtcEngine(context);
            if (!context.manualControl) {
                DingRtcEngine.DingRtcAuthInfo authInfo = getAuthInfo(context);
                int joinLeaveCount = context.joinLeaveCount > 0 ? context.joinLeaveCount : 1;
                for (int i = 0; i < joinLeaveCount; i++) {
                    joinAndLeaveChannel(context, authInfo);
                }
            } else {
                log("[Demo] manual case");
                while (true) {
                    try (BufferedReader in = new BufferedReader(new FileReader(fifoPath))) {
                        String data = in.readLine();
                        log("Command received : " + data);
                        if (CMD_EXIT.equals(data)) {
                            doStopAudio(context);
                            doStopVideo(context);
                            doLeave(context);
                            sleep(200);
                            break;
                        } else if (CMD_JOIN.equals(data)) {
                            doJoin(context);
                        } else if (CMD_LEAVE.equals(data)) {
                            doStopAudio(context);
                            doStopVideo(context);
                            doLeave(context);
                        } else if (CMD_AUDIO_START.equals(data)) {
                            doStartAudio(context);
                        } else if (CMD_AUDIO_STOP.equals(data)) {
                            doStopAudio(context);
                        } else if (CMD_VIDEO_START.equals(data)) {
                            doStartVideo(context);
                        } else if (CMD_VIDEO_STOP.equals(data)) {
                            doStopVideo(context);
                        } else if (CMD_RTM_SEND.equals(data)) {
                            doRtmSend(context);
                        } else if (data.startsWith(CMD_VIDEO_BITRATE)) {
                            String[] ss = data.split("-");
                            if (ss != null && ss.length == 2) {
                                try {
                                    context.videoBitrate = Integer.parseInt(ss[1]);
                                    doChangeVideoConfig(context);
                                } catch (Exception e) {
                                    e.printStackTrace();
                                }
                            }
                        } else if (data.startsWith(CMD_VIDEO_FPS)) {
                            String[] ss = data.split("-");
                            if (ss != null && ss.length == 2) {
                                try {
                                    context.videoFps = Integer.parseInt(ss[1]);
                                    doChangeVideoConfig(context);
                                } catch (Exception e) {
                                    e.printStackTrace();
                                }
                            }
                        } else if (data.startsWith(CMD_VIDEO_DIMEN)) {
                            String[] ss = data.split("-");
                            if (ss != null && ss.length == 3) {
                                try {
                                    context.videoWidth = Integer.parseInt(ss[1]);
                                    context.videoHeight = Integer.parseInt(ss[2]);
                                    doChangeVideoConfig(context);
                                } catch (Exception e) {
                                    e.printStackTrace();
                                }
                            }
                        }
                        
                        sleep(500);
                    } catch (Exception e) {
                        e.printStackTrace();
                        log("[Demo] pipe error, finish");
                    }
                }
            }
            context.reset();
        } else {
            log("[Demo] config error~!!");
        }
        log("************** [Demo] end   **************  \n");
    }

    private static void doRtmSend(RtcContext context) {
        if (context != null && context.engine != null) {
            String msg = "linux_java_data_" + context.rtmSendCount++;
            int ret = context.engine.getRtmClient().broadcastData(context.rtmSessionId, msg.getBytes(StandardCharsets.UTF_8));
            log("[Demo] doRtmSend " + msg + ", ret: " + ret);
            msg = "linux_java_data_to_user_" + context.rtmSendCount++;
            ret = context.engine.getRtmClient().sendData(context.rtmSessionId, context.rtmRemoteUid, msg.getBytes(StandardCharsets.UTF_8));
            log("[Demo] doRtmSend " + msg + ", ret: " + ret);
        }
    }

    private static void doJoin(RtcContext context) {
        log("[Demo] doJoin");
        if (context.manualState.isJoined) {
            log("[Demo] doJoin had joined");
            return;
        }
        DingRtcEngine engine = context.engine;
        if (!"".equals(context.rtmSessionId)) {
            engine.getRtmClient().setListener(new RtmListener(context));
        }
        engine.subscribeAllRemoteAudioStreams(context.subscribeAllAudio);
        engine.subscribeAllRemoteVideoStreams(context.subscribeAllVideo);
        if (context.subscribeAllAudio) {
            context.audioFrameObserver = new AudioObserver(context);
            engine.registerAudioFrameObserver(context.audioFrameObserver);
            engine.enableAudioFrameObserver(true, DingRtcEngine.DingRtcAudioObservePosition.RtcEngineAudioPositionPlayback);
        }
        DingRtcEngine.DingRtcVideoEncoderConfiguration videoEncoderConfig = new DingRtcEngine.DingRtcVideoEncoderConfiguration();
        videoEncoderConfig.dimensions = new DingRtcEngine.DingRtcVideoDimensions(context.videoWidth, context.videoHeight);
        videoEncoderConfig.frameRate = context.videoFps;
        videoEncoderConfig.bitrate = context.videoBitrate;
        engine.setVideoEncoderConfiguration(videoEncoderConfig);
        DingRtcEngine.DingRtcAuthInfo authInfo = getAuthInfo(context);
        engine.joinChannel(authInfo, context.userName);
        while (!context.joinResponsed) {
            sleep(100);
        }
        if (context.joinResult == 0) {
            context.manualState.isJoined = true;
        }
    }

    private static void doLeave(RtcContext context) {
        log("[Demo] doLeave");
        if (!context.manualState.isJoined) {
            log("[Demo] doLeave had joined");
            return;
        }
        if (context.engine != null) {
            context.engine.leaveChannel();
        }
        context.manualState.isJoined = false;
    }

    private static void doStartAudio(RtcContext context) {
        log("[Demo] doStartAudio");
        if (!context.manualState.isJoined) {
            log("[Demo] doStartAudio not join");
            return;
        }
        if (context.manualState.isAudioStarted) {
            log("[Demo] doStartAudio had started");
            return;
        }
        context.manualState.isAudioStarted = true;
        if (context.engine != null) {
            context.engine.publishLocalAudioStream(true);
            context.engine.setExternalAudioSource(true, context.pcmSampleRate, context.pcmChannels);
        }
        pushAudio(context);
    }

    private static void doStopAudio(RtcContext context) {
        log("[Demo] doStopAudio");
        if (!context.manualState.isJoined) {
            log("[Demo] doStopAudio not join");
            return;
        }
        if (!context.manualState.isAudioStarted) {
            log("[Demo] doStopAudio had stopped");
            return;
        }
        context.manualState.isAudioStarted = false;
        if (context.engine != null) {
            context.engine.publishLocalAudioStream(false);
        }
        if (context.externalAudioThread != null) {
            try {
                context.externalAudioThread.join();
            } catch (InterruptedException e) {
                log("[Demo] doStopAudio thread error:", e.toString());
            }
        }
    }

    private static void doStartVideo(RtcContext context) {
        log("[Demo] doStartVideo");
        if (!context.manualState.isJoined) {
            log("[Demo] doStartVideo not join");
            return;
        }
        if (context.manualState.isVideoStarted) {
            log("[Demo] doStartVideo had started");
            return;
        }
        context.manualState.isVideoStarted = true;
        if (context.engine != null) {
            context.engine.publishLocalVideoStream(true);
            context.engine.setExternalVideoSource(true, DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackCamera);
        }
        pushVideo(context);
    }

    private static void doStopVideo(RtcContext context) {
        log("[Demo] doStopVideo");
        if (!context.manualState.isJoined) {
            log("[Demo] doStopVideo not join");
            return;
        }
        if (!context.manualState.isVideoStarted) {
            log("[Demo] doStartVideo had stopped");
            return;
        }
        context.manualState.isVideoStarted = false;
        if (context.engine != null) {
            context.engine.publishLocalVideoStream(false);
            context.engine.setExternalVideoSource(false, DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackCamera);
        }
        if (context.externalVideoThread != null) {
            try {
                context.externalVideoThread.join();
            } catch (InterruptedException e) {
                log("[Demo] doStopVideo thread error:", e.toString());
            }
        }
    }

    private static void doChangeVideoConfig(RtcContext context) {
        if (context != null && context.engine != null) {
            DingRtcEngine.DingRtcVideoEncoderConfiguration videoEncoderConfig = new DingRtcEngine.DingRtcVideoEncoderConfiguration();
            videoEncoderConfig.dimensions = new DingRtcEngine.DingRtcVideoDimensions(context.videoWidth, context.videoHeight);
            videoEncoderConfig.frameRate = context.videoFps;
            videoEncoderConfig.bitrate = context.videoBitrate;
            context.engine.setVideoEncoderConfiguration(videoEncoderConfig);
        }
    }

    private static DingRtcEngine initRtcEngine(RtcContext context) {
        context.listener = new RtcListener(context);
        String dir = System.getProperty("user.dir");
        String extra = "";
        if (context.subscribeAllVideo) {
            JSONObject json = new JSONObject();
            json.put("videoDumpPath", dir);
            extra = json.toString();
        }
        DingRtcEngine.setLogDirPath(dir);
        DingRtcEngine engine = DingRtcEngine.createInstance(context.listener, extra);
        context.engine = engine;
        return engine;
    }

    private static boolean parseAppConfig(RtcContext context, String appConfigPath) {
        if (appConfigPath == null || appConfigPath.isEmpty()) {
            log("[Demo] parseAppConfig config is null or empty");
            return false;
        }
        try (BufferedReader in = new BufferedReader(new InputStreamReader(Files.newInputStream(Paths.get(appConfigPath)), StandardCharsets.UTF_8))) {
            String inputLine;
            StringBuilder content = new StringBuilder();
            while ((inputLine = in.readLine()) != null) {
                content.append(inputLine);
            }
            JSONObject json = new JSONObject(content.toString());
            return context.parseConfig(json);
        } catch (IOException e) {
            log("[Demo] parseAppConfig config exception ", e.toString());
            return false;
        }
    }

    private static void joinAndLeaveChannel(RtcContext context, DingRtcEngine.DingRtcAuthInfo authInfo) {
        log("[Demo] joinAndLeaveChannel");
        DingRtcEngine engine = context.engine;

        engine.publishLocalAudioStream(context.publishAudio);
        engine.publishLocalVideoStream(context.publishVideo);

        engine.subscribeAllRemoteAudioStreams(context.subscribeAllAudio);
        engine.subscribeAllRemoteVideoStreams(context.subscribeAllVideo);

        if (context.subscribeAllAudio) {
            context.audioFrameObserver = new AudioObserver(context);
            engine.registerAudioFrameObserver(context.audioFrameObserver);
            engine.enableAudioFrameObserver(true, DingRtcEngine.DingRtcAudioObservePosition.RtcEngineAudioPositionPlayback);
        }

        DingRtcEngine.DingRtcVideoEncoderConfiguration videoEncoderConfig = new DingRtcEngine.DingRtcVideoEncoderConfiguration();
        videoEncoderConfig.dimensions = new DingRtcEngine.DingRtcVideoDimensions(context.videoWidth, context.videoHeight);
        videoEncoderConfig.frameRate = context.videoFps;
        videoEncoderConfig.bitrate = context.videoBitrate;
        engine.setVideoEncoderConfiguration(videoEncoderConfig);

        if (context.pushExternalVideo) {
            engine.setExternalVideoSource(true, DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackCamera);
        }

        engine.joinChannel(authInfo, context.userName);

        while (!context.joinResponsed) {
            sleep(100);
        }

        if (context.joinResult == 0) {
            context.externalAudioFinished = !context.pushExternalAudio;
            context.externalVideoFinished = !context.pushExternalVideo;

            // 入会成功后，开启外部音视频推流
            if (context.pushExternalAudio) {
                pushAudio(context);
            }
            if (context.pushExternalVideo) {
                pushVideo(context);
            }

            while (!context.externalAudioFinished || !context.externalVideoFinished) {
                sleep(1000);
            }

            if (context.pushExternalAudio) {
                engine.setExternalAudioSource(false, context.pcmSampleRate, context.pcmChannels);
            }
            if (context.pushExternalVideo) {
                engine.setExternalVideoSource(false, DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackCamera);
            }

            if (context.sleepMsBeforeLeave > 0) {
                sleep(context.sleepMsBeforeLeave);
            }

            log("[Demo] leaveChannel...");
            engine.leaveChannel();
        } else {
            log("[Demo] join false");
        }
    }

    private static void pushAudio(final RtcContext context) {
        log(String.format("[Demo] pushAudio, pcmFilePath:%s, sampleRate:%d, pcmChannels:%d", context.pcmFilePath, context.pcmSampleRate, context.pcmChannels));
        context.engine.setExternalAudioSource(true, context.pcmSampleRate, context.pcmChannels);
        context.externalAudioThread = new Thread(() -> {
            DingRtcEngine.DingRtcAudioFrame frame = new DingRtcEngine.DingRtcAudioFrame();
            try (RandomAccessFile file = new RandomAccessFile(context.pcmFilePath, "r");) {
                long sampleCount = 0;
                int bytesPerSample = 2;
                int samplesToRead = context.pcmSampleRate / (1000 / context.pcmReadFreq);
                int bufferSize = samplesToRead * context.pcmChannels * bytesPerSample;
                byte[] buffer = new byte[bufferSize];
                long delay = 0;
                long startClock = System.currentTimeMillis();
                long lastStatsClock = startClock;
                long loopPlayCount = context.loopPlayCount;
                while (!context.quit) {
                    if (context.manualControl && !context.manualState.isAudioStarted) {
                        break;
                    }
                    int readBytes = file.read(buffer);
                    if (readBytes != bufferSize) {
                        log("[Demo] reading pcm EOF, loopPlayCount:", loopPlayCount);
                        if (context.manualControl) {
                            file.seek(0);
                            continue;
                        } else {
                            if (--loopPlayCount > 0) {
                                file.seek(0);
                                continue;
                            } else {
                                break;
                            }
                        }
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
                        context.engine.pushExternalAudioFrame(frame);
                        sampleCount += frame.numSamples;
                    }

                    long elpasedStats = System.currentTimeMillis() - lastStatsClock;
                    if (elpasedStats >= 5000) {
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

    private static void pushVideo(RtcContext context) {
        log(String.format("[Demo] pushVideo, videoFilePath:%s, width:%d, height:%d, fps:%d", context.videoFilePath, context.videoWidth, context.videoHeight, context.videoFps));
        context.externalVideoThread = new Thread(() -> {
            try (RandomAccessFile file = new RandomAccessFile(context.videoFilePath, "r")) {
                long frameCount = 0;
                DingRtcEngine.DingRtcVideoFormat pixelFormat = convertPixelFormat(context.videoPixelFormat);
                int bufferSize = calcBufferSize(pixelFormat, context.videoWidth, context.videoHeight);

                long delay = 0;
                long startClock = System.currentTimeMillis();
                long lastStatsClock = startClock;
                byte[] buffer = new byte[bufferSize];
                long loopPlayCount = context.loopPlayCount;

                DingRtcEngine.DingRtcVideoFrame frame = new DingRtcEngine.DingRtcVideoFrame();
                while (!context.quit) {
                    if (context.manualControl && !context.manualState.isVideoStarted) {
                        break;
                    }
                    long readStartClock = System.currentTimeMillis();
                    int readBytes = file.read(buffer);
                    if (readBytes != bufferSize) {
                        log("[Demo] Reading yuv EOF, loopPlayCount:", loopPlayCount);
                        if (context.manualControl) {
                            file.seek(0);
                            continue;
                        } else {
                            if (--loopPlayCount > 0) {
                                file.seek(0);
                                continue;
                            } else {
                                break;
                            }
                        }
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

                    context.engine.pushExternalVideoFrame(frame, DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackCamera);
                    frameCount++;

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

    static DingRtcEngine.DingRtcVideoFormat convertPixelFormat(String pixelFormat) {
        if ("I420".equals(pixelFormat)) return DingRtcEngine.DingRtcVideoFormat.DingRtcVideoI420;
        else if ("NV12".equals(pixelFormat)) return DingRtcEngine.DingRtcVideoFormat.DingRtcVideoNV12;
        else if ("BGRA".equals(pixelFormat)) return DingRtcEngine.DingRtcVideoFormat.DingRtcVideoBGRA;
        else if ("ARGB".equals(pixelFormat)) return DingRtcEngine.DingRtcVideoFormat.DingRtcVideoARGB;
        else if ("RGBA".equals(pixelFormat)) return DingRtcEngine.DingRtcVideoFormat.DingRtcVideoRGBA;
        else if ("ABGR".equals(pixelFormat)) return DingRtcEngine.DingRtcVideoFormat.DingRtcVideoABGR;
        else return DingRtcEngine.DingRtcVideoFormat.DingRtcVideoI420;
    }

    static int calcBufferSize(DingRtcEngine.DingRtcVideoFormat pixel_format, int width, int height) {
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

    private static String urlEnc(String s) {
        try {
            return URLEncoder.encode(s, "utf-8");
        } catch (UnsupportedEncodingException e) {
            throw new RuntimeException(e);
        }
    }

    private static DingRtcEngine.DingRtcAuthInfo getAuthInfo(RtcContext context) {
        String server = context.appServer;
        String appId = context.appId;
        String userId = context.userId;
        String roomId = context.channelName;
        String userName = context.userName;
        String env = context.env;
        String passwd = context.passwd;

        DingRtcEngine.DingRtcAuthInfo authInfo = new DingRtcEngine.DingRtcAuthInfo();
        HttpURLConnection conn = null;
        try {
            String baseUrl = server + "/login?" +
                    "passwd=" + urlEnc(passwd) +
                    "&appid=" + urlEnc(appId) +
                    "&userid=" + urlEnc(userId) +
                    "&user=" + urlEnc(userName) +
                    "&room=" + urlEnc(roomId) +
                    "&env=" + urlEnc(env) +
                    "&tokensid=false";
            URL url = new URL(baseUrl);
            conn = (HttpURLConnection) url.openConnection();
            conn.setRequestMethod("GET");
            conn.connect();
            int responseCode = conn.getResponseCode();
            if (responseCode == HttpURLConnection.HTTP_OK) {
                try (BufferedReader in = new BufferedReader(new InputStreamReader(conn.getInputStream(), StandardCharsets.UTF_8))) {
                    String inputLine;
                    StringBuilder content = new StringBuilder();
                    while ((inputLine = in.readLine()) != null) {
                        content.append(inputLine);
                    }
                    JSONObject obj = new JSONObject(content.toString());
                    authInfo.channelId = roomId;
                    authInfo.userId = obj.getJSONObject("data").getString("userid");
                    authInfo.appId = obj.getJSONObject("data").getString("appid");
                    authInfo.token = obj.getJSONObject("data").getString("token");
                    authInfo.gslb = obj.getJSONObject("data").getJSONArray("gslb").getString(0);
                }
            } else {
                log("[Demo] Failed : HTTP error code : " + responseCode);
            }
        } catch (Exception e) {
            log("[Demo] getAuthInfo error:", e.toString());
        } finally {
            if (conn != null) {
                conn.disconnect();
            }
        }
        return authInfo;
    }

}