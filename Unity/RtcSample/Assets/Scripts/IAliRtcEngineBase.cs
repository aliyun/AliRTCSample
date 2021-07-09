using UnityEngine;
using System;
using System.Globalization;
using System.Runtime.InteropServices;
using System.Collections;
using System.Collections.Generic;
using AOT;

namespace ali_unity_rtc
{
    public enum AliRTCVideoTrack {
        VIDEO_TRACK_NONE = 0,
        VIDEO_TRACK_CAMERA,
        VIDEO_TRACK_SCREEN,
        VIDEO_TRACK_BOTH,
    };

    public enum AliRTCAudioTrack {
        AUDIO_TRACK_NONE = 0,
        AUDIO_TRACK_MIC,
    };

    public enum AliRTCNetworkQuality {
        NETWORK_QUALITY_EXCELLENT = 0,
        NETWORK_QUALITY_GOOD,
        NETWORK_QUALITY_POOR,
        NETWORK_QUALITY_BAD,
        NETWORK_QUALITY_VERY_BAD,
        NETWORK_QUALITY_VERY_DISCONNECT,
        NETWORK_QUALITY_UNKNOWN,
    };

    public enum AliRTCVideoProfile {
        VIDEO_PROFILE_DEFAULT= 0,
        VIDEO_PROFILE_180_320P_15,
        VIDEO_PROFILE_180_320P_30,
        VIDEO_PROFILE_360_640P_15,
        VIDEO_PROFILE_360_640P_30,
        VIDEO_PROFILE_720_1280P_15,
        VIDEO_PROFILE_720_1280P_30,
        VIDEO_PROFILE_Max
    };

    public enum AliRTCRenderMirror {
        RENDER_MIRROR_FRONT_PREVIEW_ENABLED = 0,
        RENDER_MIRROR_ALL_ENABLED,
        RENDER_MIRROR_ALL_DISABLED,
    };

    public enum AliRTCLogLevel{
        LOG_LEVEL_DUMP = 0,
        LOG_LEVEL_DEBUG,
        LOG_LEVEL_VERBOSE,
        LOG_LEVEL_INFO,
        LOG_LEVEL_WARN,
        LOG_LEVEL_ERROR,
        LOG_LEVEL_FATAL,
        LOG_LEVEL_NONE,
    };

    public enum AliRTCnByeType
    {
        ON_BYE_KICK_OUT = 1,
        ON_BYE_CHANNEL_TERMINATED = 2,
        ON_BYE_USER_REPLACED = 3,
    };

    public enum AliRTCChannelProfile
    {
        CHANNEL_PROFILE_COMMUNICATION = 0,
        CHANNEL_PROFILE_INTERACTIVELIVE = 1,
    };

    public enum AliRTCClientRole
    {
        CLIENT_ROLE_INTERACTIVE = 0,
        CLIENT_ROLE_LIVE = 1,
    };

    public struct AliRTCAuthInfo {
        public string channel;
        public string userId;
        public string appId;
        public string nonce;
        public string token;
        public string gslbArray;
        public string agentArray;
        public long timestamp;
    };

#if UNITY_ANDROID || UNITY_IOS || UNITY_IPHONE
    public struct AliRTCTextureInfo
    {
        public int textureId;
        public int flip;
    };
#endif

    public abstract class IAliRtcEngineBase
    {
        #region DllImport
#if UNITY_STANDALONE_WIN || UNITY_EDITOR || UNITY_STANDALONE_OSX
        public const string MyLibName = "AliRTCSdkCWrapper";
#elif UNITY_IOS || UNITY_IPHONE
        public const string MyLibName = "__Internal";
#elif UNITY_ANDROID
        public const string MyLibName = "alivc_sophon";
#endif

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern void AliRtcCreateEngine(string extras);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern void AliRtcDestroyEngine();

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern IntPtr AliRtcGetSdkVersion();

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern int AliRtcSetAutoPublish(bool autoPub, bool autoSub);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern int AliRtcSetAudioOnlyMode(bool audioOnly);
        
        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern void AliRtcJoinChannel(AliRTCAuthInfo authInfo, string userName);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern void AliRtcLeaveChannel();

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern void AliRtcStartPreview();

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern void AliRtcStopPreview();

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern int AliRtcMuteLocalCamera(bool mute, int track);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern void AliRtcPublish();

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern void AliRtcSetVideoProfile(int profile, int track);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern int AliRtcMuteLocalMic(bool mute);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern int AliRtcMuteRemoteAudioPlaying(string userId, bool mute);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern void AliRtcSubscribe(string userId);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern void AliRtcConfigLocalCameraPublish(bool enable);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern bool AliRtcIsLocalCameraPublishEnabled();

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern void AliRtcConfigLocalAudioPublish(bool enable);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern bool AliRtcIsLocalAudioPublishEnabled();

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern int AliRtcConfigLocalSimulcast(bool enable, int track);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern bool AliRtcIsLocalSimulcastEnabled();

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern void AliRtcConfigRemoteAudio(string userId, bool enable);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern void AliRtcConfigRemoteCameraTrack(string userId, bool master, bool enable);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern void AliRtcConfigRemoteScreenTrack(string userId, bool enable);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern bool AliRtcIsUserOnline(string userId);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        public static extern IntPtr AliRtcGetOnlineRemoteUsers();

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern void AliRtcStartAudioCapture();

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern void AliRtcStopAudioCapture();

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern void AliRtcStartAudioPlayer();

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern void AliRtcStopAudioPlayer();

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern void AliRtcSetLogLevel(int logLevel);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern bool AliRtcIsCameraOn();

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern bool AliRtcIsInCall();

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern bool AliRtcIsAutoPublish();

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern bool AliRtcIsAutoSubscribe();

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern bool AliRtcIsAudioOnly();

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern void AliRtcSetH5CompatibleMode(bool comp);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern bool AliRtcGetH5CompatibleMode();

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern int AliRtcSetCustomId(string customId);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern int AliRtcSetChannelProfile(int profile);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern int AliRtcSetClientRole(int role);

#if UNITY_ANDROID || UNITY_IOS || UNITY_IPHONE
        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern int AliRtcGenerateTexture();

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern int AliRtcSetTexture(AliRTCTextureInfo textureInfo, int track, string userId);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern void AliRtcRemoveTexture(int textureId);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern int AliRtcEnableSpeakerphone(bool enable);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern int AliRtcSwitchCamera();

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern int AliRtcSetCameraZoom(float zoom, bool flash, bool autoFocus);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern bool AliRtcIsCameraFocusPointSupported();

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern bool AliRtcIsCameraExposurePointSupported();

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern int AliRtcSetCameraFocusPoint(float x, float y);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern int AliRtcSetCameraExposurePoint(float x, float y);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern void AliRtcEnableBackGroundRecording(bool enable);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern bool AliRtcIsBackGroundRecording();

#elif UNITY_STANDALONE_WIN || UNITY_STANDALONE_OSX
        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern int AliRtcGetVideoProfile(int track);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern bool AliRtcConfigExternalVideoRendering(bool enable);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern int AliRtcGetVideoRenderData(string uid, int track, IntPtr data);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern void AliRtcConfigLocalScreenPublish(bool enable);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern bool AliRtcIsLocalScreenPublishEnabled();

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern void AliRtcSetLocalViewConfig(bool flip);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern void AliRtcSetRemoteViewConfig(string userId, bool flip);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern IntPtr AliRtcGetAudioCaptures();

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern IntPtr AliRtcGetCurrentAudioCapture();

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern void AliRtcSetCurrentAudioCapture(string deviceName);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern IntPtr AliRtcGetAudioRenderers();

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern IntPtr AliRtcGetCurrentAudioRenderer();

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        public static extern void AliRtcSetCurrentAudioRenderer(string deviceName);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern IntPtr AliRtcGetCameraList();

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern IntPtr AliRtcGetCurrentCamera();

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern void AliRtcSetCurrentCamera(string deviceName);
#endif

        public delegate void EngineEventOnJoinChannelNotifyHandler(int errorCode);

        protected delegate void EngineEventOnPublishNotifyHandler(int errorCode);

        protected delegate void EngineEventOnSubscribeNotifyHandler(string userId, int videoTrack, int audioTrack);

        protected delegate void EngineEventOnRemoteUserOnLineNotifyHandler(string userId);

        protected delegate void EngineEventOnRemoteUserOffLineNotifyHandler(string userId);

        protected delegate void EngineEventOnRemoteTrackAvailableNotifyHandler(string userId, int audioTrack, int videoTrack);

        protected delegate void EngineEventOnSubscribeChangedNotifyHandler(string userId, int audioTrack, int videoTrack);

        protected delegate void EngineEventOnNetworkQualityChangedNotifyHandler(string userId, int upQuality, int downQuality);

        protected delegate void EngineEventOnConnectionLostNotifyHandler();

        protected delegate void EngineEventOnTryToReconnectNotifyHandler();

        protected delegate void EngineEventOnConnectionRecoveryNotifyHandler();

        protected delegate void EngineEventOnByeNotifyHandler(int code);

        protected delegate void EngineEventOnOccurWarningNotifyHandler(int code);

        protected delegate void EngineEventOnOccurErrorNotifyHandler(int error);

        protected delegate void EngineEventOnFirstPacketSentNotifyHandler(int audioTrack, int videoTrack);

        protected delegate void EngineEventOnFirstPacketReceivedNotifyHandler(int audioTrack, int videoTrack);

        protected delegate void EngineEventOnFirstRemoteVideoFrameDrawnNotifyHandler(string userId, int videoTrack);

        protected delegate void EngineEventOnLeaveChannelResultNotifyHandler(int result);

        protected delegate void EngineEventOnUpdateRoleNotifyHandler(int oldRole, int newRole);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern void AliRtcSetEventOnJoinChannelNotify(EngineEventOnJoinChannelNotifyHandler onJoinChannelNotify);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern void AliRtcSetEventOnPublishNotify(EngineEventOnPublishNotifyHandler onPublishNotify);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern void AliRtcSetEventOnSubscribeNotify(EngineEventOnSubscribeNotifyHandler onSubscribeNotify);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern void AliRtcSetEventOnRemoteUserOnLineNotify(EngineEventOnRemoteUserOnLineNotifyHandler OnRemoteUserOnLineNotify);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern void AliRtcSetEventOnRemoteUserOffLineNotify(EngineEventOnRemoteUserOffLineNotifyHandler OnRemoteUserOffLineNotify);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern void AliRtcSetEventOnRemoteTrackAvailableNotify(EngineEventOnRemoteTrackAvailableNotifyHandler OnRemoteTrackAvailableNotify);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern void AliRtcSetEventOnSubscribeChangedNotify(EngineEventOnSubscribeChangedNotifyHandler OnSubscribeChangedNotify);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern void AliRtcSetEventOnNetworkQualityChangedNotify(EngineEventOnNetworkQualityChangedNotifyHandler onNetworkQualityChangedNotify);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern void AliRtcSetEventOnConnectionLostNotify(EngineEventOnConnectionLostNotifyHandler onConnectionLostNotify);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern void AliRtcSetEventOnTryToReconnectNotify(EngineEventOnTryToReconnectNotifyHandler OnTryToReconnectNotify);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern void AliRtcSetEventOnConnectionRecoveryNotify(EngineEventOnConnectionRecoveryNotifyHandler onConnectionRecoveryNotify);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern void AliRtcSetEventOnByeNotify(EngineEventOnByeNotifyHandler EngineEventOnByeNotifyHandler);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern void AliRtcSetEventOnOccurWarningNotify(EngineEventOnOccurWarningNotifyHandler onOccurWarningNotify);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern void AliRtcSetEventOnOccurErrorNotify(EngineEventOnOccurErrorNotifyHandler onOccurErrorNotify);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern void AliRtcSetEventOnFirstPacketSentNotify(EngineEventOnFirstPacketSentNotifyHandler onFirstPacketSentNotify);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern void AliRtcSetEventOnFirstPacketReceivedNotify(EngineEventOnFirstPacketReceivedNotifyHandler onFirstPacketReceivedNotify);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern void AliRtcSetEventOnFirstRemoteVideoFrameDrawnNotify(EngineEventOnFirstRemoteVideoFrameDrawnNotifyHandler onFirstRemoteVideoFrameDrawnNotify);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern void AliRtcSetEventOnLeaveChannelResultNotify(EngineEventOnLeaveChannelResultNotifyHandler onLeaveChannelResultNotify);

        [DllImport(MyLibName, CharSet = CharSet.Ansi)]
        protected static extern void AliRtcSetEventOnUpdateRoleNotify(EngineEventOnUpdateRoleNotifyHandler onUpdateRoleNotify);

        #endregion
    }
}