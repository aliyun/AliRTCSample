package com.ding.rtc.demo.bigconf;

import static com.ding.rtc.DingRtcEngine.DingRtcSubscribeState.DingRtcStatsNoSubscribe;
import static com.ding.rtc.DingRtcEngine.DingRtcVideoStreamType.DingRtcVideoStreamTypeLD;
import static com.ding.rtc.DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackNo;

import com.ding.rtc.DingRtcEngine;

public class BigConfUser {
    public String uid;
    public String nickName;
    public boolean isSelf;
    public boolean isAudioMuted;
    public boolean isVideoMuted;
    public boolean isScreenMuted;

    public boolean isAudioSubscribed;

    public boolean isAudioStarted;
    public boolean isVideoStarted;
    public boolean isScreenStarted;

    public boolean isCameraOn;
    public boolean isVideoEnabled;
    public boolean isVideoPreview;

    public DingRtcEngine.DingRtcVideoTrack subedStreamType = DingRtcVideoTrackNo;
    public DingRtcEngine.DingRtcSubscribeState videoSubscribeState = DingRtcStatsNoSubscribe;
    public DingRtcEngine.DingRtcSubscribeState screenSubscribeState = DingRtcStatsNoSubscribe;
    public DingRtcEngine.DingRtcVideoStreamType videoSubscribeType = DingRtcVideoStreamTypeLD;

    public String audioDeviceName = "默认的语音路由";
}
