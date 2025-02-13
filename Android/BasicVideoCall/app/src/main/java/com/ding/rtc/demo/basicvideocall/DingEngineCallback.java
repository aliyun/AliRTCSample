package com.ding.rtc.demo.basicvideocall;

import com.ding.rtc.DingRtcEngine;
import com.ding.rtc.DingRtcEngineEventListener;

import java.util.ArrayList;
import java.util.List;

import static com.ding.rtc.demo.basicvideocall.ThreadUtils.runOnUiThread;

public class DingEngineCallback extends DingRtcEngineEventListener {
    private final ArrayList<EventHandler> mHandler = new ArrayList<>();

    public void addHandler(EventHandler handler) {
        mHandler.add(handler);
    }

    public void removeHandler(EventHandler handler) {
        mHandler.remove(handler);
    }

    // -------------------------- RTC Engine Callbacks --------------------------


    @Override
    public void onJoinChannelResult(int result, String channel, String userId, int elapsed) {

        runOnUiThread(()-> {
            for (EventHandler handler : mHandler) {
                handler.onJoinChannelResult(result, channel, userId, elapsed);
            }
        });
    }

    @Override
    public void onLeaveChannelResult(int result, DingRtcEngine.DingRtcStats stats) {
        runOnUiThread(()-> {
            for (EventHandler handler : mHandler) {
                handler.onLeaveChannelResult(result, stats);
            }
        });
    }

    @Override
    public void onAudioPublishStateChanged(DingRtcEngine.DingRtcPublishState oldState, DingRtcEngine.DingRtcPublishState newState, int elapseSinceLastState, String channel) {

    }

    @Override
    public void onVideoPublishStateChanged(DingRtcEngine.DingRtcPublishState oldState, DingRtcEngine.DingRtcPublishState newState, int elapseSinceLastState, String channel) {

    }

    @Override
    public void onDualStreamPublishStateChanged(DingRtcEngine.DingRtcPublishState oldState, DingRtcEngine.DingRtcPublishState newState, int elapseSinceLastState, String channel) {

    }

    @Override
    public void onScreenSharePublishStateChanged(DingRtcEngine.DingRtcPublishState oldState, DingRtcEngine.DingRtcPublishState newState, int elapseSinceLastState, String channel) {

    }

    @Override
    public void onAudioSubscribeStateChanged(String uid, DingRtcEngine.DingRtcSubscribeState oldState, DingRtcEngine.DingRtcSubscribeState newState, int elapseSinceLastState, String channel) {

    }

    @Override
    public void onVideoSubscribeStateChanged(String uid, DingRtcEngine.DingRtcSubscribeState oldState, DingRtcEngine.DingRtcSubscribeState newState, int elapseSinceLastState, String channel) {

    }

    @Override
    public void onSubscribeStreamTypeChanged(String uid, DingRtcEngine.DingRtcVideoStreamType oldStreamType, DingRtcEngine.DingRtcVideoStreamType newStreamType, int elapseSinceLastState, String channel) {

    }

    @Override
    public void onScreenShareSubscribeStateChanged(String uid, DingRtcEngine.DingRtcSubscribeState oldState, DingRtcEngine.DingRtcSubscribeState newState, int elapseSinceLastState, String channel) {

    }

    @Override
    public void onNetworkQualityChanged(String uid, DingRtcEngine.DingRtcNetworkQuality upQuality, DingRtcEngine.DingRtcNetworkQuality downQuality) {

    }

    @Override
    public void onOccurWarning(int warn, String message) {

    }

    @Override
    public void onOccurError(int error, String message) {

    }

    @Override
    public void onPerformanceLow() {
        //
    }

    @Override
    public void onPermormanceRecovery() {
        //
    }

    @Override
    public void onConnectionLost() {
        //
    }

    @Override
    public void onTryToReconnect() {
        //
    }

    @Override
    public void onConnectionRecovery() {
        //
    }

    @Override
    public void onRemoteUserOnLineNotify(String uid, int elapsed) {
        runOnUiThread(()-> {
            for (EventHandler handler : mHandler) {
                handler.onRemoteUserOnLineNotify(uid, elapsed);
            }
        });
    }

    @Override
    public void onRemoteUserOffLineNotify(String uid, DingRtcEngine.DingRtcUserOfflineReason reason) {
        runOnUiThread(()-> {
            for (EventHandler handler : mHandler) {
                handler.onRemoteUserOffLineNotify(uid, reason);
            }
        });
    }

    @Override
    public void onRemoteTrackAvailableNotify(String uid, DingRtcEngine.DingRtcAudioTrack audioTrack, DingRtcEngine.DingRtcVideoTrack videoTrack) {
        runOnUiThread(()-> {
            for (EventHandler handler : mHandler) {
                handler.onRemoteTrackAvailableNotify(uid, audioTrack, videoTrack);
            }
        });
    }

    @Override
    public void onFirstRemoteVideoFrameDrawn(String uid, DingRtcEngine.DingRtcVideoTrack videoTrack, int width, int height, int elapsed) {

    }

    @Override
    public void onFirstLocalVideoFrameDrawn(int width, int height, int elapsed) {

    }

    @Override
    public void onFirstVideoFrameReceived(String uid, DingRtcEngine.DingRtcVideoTrack videoTrack, int timeCost) {

    }

    @Override
    public void onFirstVideoPacketSend(DingRtcEngine.DingRtcVideoTrack videoTrack, int timeCost) {

    }

    @Override
    public void onFirstAudioPacketSent(String uid, int timeCost) {

    }

    @Override
    public void onFirstVideoPacketReceived(String uid, DingRtcEngine.DingRtcVideoTrack videoTrack, int timeCost) {

    }

    @Override
    public void onFirstAudioPacketReceived(String uid, int timeCost) {

    }

    @Override
    public void onBye(int code) {

    }

    @Override
    public void onDingRtcStats(DingRtcEngine.DingRtcStats DingRtcStats) {
        //
    }

    @Override
    public void onUserAudioMuted(String uid, boolean isMute) {

    }

    @Override
    public void onUserVideoMuted(String uid, boolean isMute, DingRtcEngine.DingRtcVideoTrack track) {

    }

    @Override
    public void onMediaRecordEvent(int event, String filePath) {

    }

    @Override
    public void onRtcLocalVideoStats(DingRtcEngine.DingRtcLocalVideoStats DingRtcStats) {

    }

    @Override
    public void onRtcRemoteVideoStats(DingRtcEngine.DingRtcRemoteVideoStats DingRtcStats) {
        //
    }

    @Override
    public void onRtcRemoteAudioStats(DingRtcEngine.DingRtcRemoteAudioStats DingRtcStats) {
        //
    }

    @Override
    public void onRtcLocalAudioStats(DingRtcEngine.DingRtcLocalAudioStats DingRtcStats) {
        //
    }

    @Override
    public void onAudioFocusChange(int focusChange) {
        //
    }

    @Override
    public void onAudioRouteChanged(DingRtcEngine.DingRtcAudioRouteType routing) {
        //
    }

    @Override
    public void onAudioVolumeIndication(List<DingRtcEngine.DingRtcAudioVolumeInfo> speakers) {
        //
    }


    @Override
    public void onRemoteVideoResolutionChanged(String uId, DingRtcEngine.DingRtcVideoTrack videoTrack, int oldWidth, int oldHeight, int newWidth, int newHeight) {

    }

    @Override
    public void onApiCalledExecuted(int error, String api, String result) {
        //
    }
}
