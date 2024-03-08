package com.ding.rtc.demo.basicvideocall;

import com.ding.rtc.DingRtcEngine;

public interface EventHandler {

    void onJoinChannelResult(int result, String channel, String userId, int elapsed);

    void onLeaveChannelResult(int result, DingRtcEngine.DingRtcStats stats);

    void onRemoteUserOnLineNotify(String uid, int elapsed);

    void onRemoteUserOffLineNotify(String uid, DingRtcEngine.DingRtcUserOfflineReason reason);

    void onRemoteTrackAvailableNotify(String uid, DingRtcEngine.DingRtcAudioTrack audioTrack, DingRtcEngine.DingRtcVideoTrack videoTrack);

}
