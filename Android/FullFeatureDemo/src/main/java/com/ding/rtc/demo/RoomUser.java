package com.ding.rtc.demo;

import static com.ding.rtc.DingRtcEngine.DingRtcRenderMirrorMode.DingRtcRenderMirrorModeOnlyFront;
import static com.ding.rtc.DingRtcEngine.DingRtcRotationMode.DingRtcRotationMode_0;
import static com.ding.rtc.DingRtcEngine.DingRtcSubscribeState.DingRtcStatsNoSubscribe;
import static com.ding.rtc.DingRtcEngine.DingRtcVideoStreamType.DingRtcVideoStreamTypeFHD;

import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import com.ding.rtc.DingRtcEngine;

public class RoomUser {
    public boolean isSelf;
    public String userId;
    public String nickName;
    public boolean isAudioStarted;
    public boolean isVideoEnabled;
    public boolean isVideoStarted;
    public boolean isVideoPreview;
    public boolean isScreenStarted;
    public boolean isAudioMuted;
    public boolean isVideoMuted;
    public boolean isScreenMuted;
    public boolean isAudioSubscribed;
    public boolean isExternalVideo;
    public DingRtcEngine.DingRtcVideoTrack streamType;
    public boolean isExternalAudio;
    public DingRtcEngine.DingRtcSubscribeState videoSubscribeState = DingRtcStatsNoSubscribe;
    public DingRtcEngine.DingRtcSubscribeState screenSubscribeState = DingRtcStatsNoSubscribe;
    public DingRtcEngine.DingRtcVideoStreamType videoSubscribeType = DingRtcVideoStreamTypeFHD;
    public View rootView;
    public TextView nickNameView;
    public TextView statsAudioView;
    public TextView statsVideoView;
    public TextView statsScreenView;
    public TextView statsAudioCallbackView;
    public TextView statsVideoCallbackView;
    public TextView statsNetworkQualityView;
    public ViewGroup renderBigGroup;
    public ViewGroup renderSmallGroup;
    public ImageView subStatusAudio;
    public ImageView subStatusVideo;
    public ImageView subStatusScreen;
    public ImageView zoomView;
    public ImageView flashView;
    public ImageView mirrorView;
    public ImageView rotateView;
    public ImageView muteRemoteAudioView;
    public ImageView remoteUserSubView;
    public ImageView userStatView;
    public ImageView annotationView;
    public int playoutSignalVolume = 50;
    public float screenScale = 1.0f;

    public DingRtcEngine.DingRtcRenderMirrorMode mirrorMode = DingRtcRenderMirrorModeOnlyFront;
    public DingRtcEngine.DingRtcRotationMode rotationMode = DingRtcRotationMode_0;
    public boolean muteRemoteAudio = false;

//    public ImageView subStatusQuality;
    public DingRtcEngine.DingRtcVideoCanvas cameraCanvas;
    public DingRtcEngine.DingRtcVideoCanvas screenCanvas;

    public boolean isCameraOn = true;
    public String audioDeviceName = "默认的语音路由";
    public ForeBackState foreBackState = ForeBackState.Fore;

    public enum ForeBackState {
        Fore,
        Back,
        Unknown
    }
}
