/**
 * 功能接口类型
 */
const AliEngineInterfaceIdType = {
	AliEngineInterfaceAudioDeviceManger: 0,
	AliEngineInterfaceVideoDeviceManger: 1,
	AliEngineInterfaceMediaEngine: 2
}
/**
 * 频道类型
 */
const AliEngineChannelProfile = {
	AliEngineCommunication: 0,
	AliEngineInteractiveLive: 1,
	AliEngineInteractiveWithLowLatencyLive: 2,
}

/**
 * 用户角色
 */
const AliEngineClientRole = {
	AliEngineClientRoleInteractive: 1,
	AliEngineClientRoleLive: 2,
}

/**
 * @brief 网络状况
 */
const AliEngineNetworkQuality = {
	AliEngineNetworkQualityExcellent: 0,
	AliEngineNetworkQualityGood: 1,
	AliEngineNetworkQualityPoor: 2,
	AliEngineNetworkQualityBad: 3,
	AliEngineNetworkQualityVeryBad: 4,
	AliEngineNetworkQualityDisconnect: 5,
	AliEngineNetworkQualityUnknow: 6,
}

/**
 * @brief 视频窗口镜像模式
 */
const AliEngineRenderMirrorMode = {
	AliEngineRenderMirrorModeOnlyFrontMirror: 0,
	AliEngineRenderMirrorModeAllMirror: 1,
	AliEngineRenderMirrorModeAllNoMirror: 2,
	AliEngineRenderMirrorModeNoChange: 99
}

/**
 * @brief camera方向
 */
const AliEngineCameraDirection = {
	AliEngineCameraDirectionInvalid: -1,
	AliEngineCameraDirectionBack: 0,
	AliEngineCameraDirectionFront: 1
}


/**
 * @brief 采集偏好
 */
const AliEngineCaptureOutputPreference = {
	AliEngineCaptureOutputPreferenceAuto: 0,
	/* 根据设置的publsh profile 分辨率和帧率，选择最接近的摄像头输出参数 */
	AliEngineCaptureOutputPreferencePerformance: 1,
	/* 选择较高的摄像头输出参数 */
	AliEngineCaptureOutputPreferencePreview: 2,
}

/**
 * @brief 视频比例
 */
const AliEngineVideoScale = {
	AliEngineVideoScale_16_9: 0,
	AliEngineVideoScale_4_3: 1,
}

/**
 * @brief 日志级别
 */
const AliEngineLogLevel = {
	AliEngineLogLevelDump: 0,
	AliEngineLogLevelDebug: 1,
	AliEngineLogLevelVerbose: 2,
	AliEngineLogLevelInfo: 3,
	AliEngineLogLevelWarn: 4,
	AliEngineLogLevelError: 5,
	AliEngineLogLevelFatal: 6,
	AliEngineLogLevelNone: 7,
}

/**
 * @brief 设备方向
 */
const AliEngineOrientationMode = {
	AliEngineOrientationModePortrait: 0,
	AliEngineOrientationModeLandscapeLeft: 1,
	AliEngineOrientationModePortraitUpsideDown: 2,
	AliEngineOrientationModeLandscapeRight: 3,
	AliEngineOrientationModeAuto: 4,
}

/**
 * @brief 本地静音模式
 */
const AliEngineMuteLocalAudioMode = {
	AliEngineMuteLocalAudioModeDefault: 0,
	AliEngineMuteLocalAudioModeMuteAll: 1,
	AliEngineMuteLocalAudioModeMuteOnlyMic: 2,
}

/**
 * @brief AVAudioSession控制权限设置
 */
const AliEngineAudioSessionOperationRestriction = {
	AliEngineAudioSessionOperationRestrictionNone: 0,
	AliEngineAudioSessionOperationRestrictionSetCategory: 1,
	AliEngineAudioSessionOperationRestrictionConfigureSession: 2,
	AliEngineAudioSessionOperationRestrictionDeactivateSession: 3,
}

/**
 * @brief 变声音效模式
 */
const AliEngineAudioEffectVoiceChangerMode = {
	AliEngineAudioEffectVoiceChangerOff: 0,     		/* 关闭 */
	AliEngineAudioEffectVoiceChangerOldMan: 1,      /* 老人 */
	AliEngineAudioEffectVoiceChangerBabyBoy: 2,     /* 男孩 */
	AliEngineAudioEffectVoiceChangerBabGirl: 3,     /* 女孩 */
	AliEngineAudioEffectVoiceChangerRobot: 4,       /* 机器人 */
	AliEngineAudioEffectVoiceChangerDaimo: 5,       /* 大魔王 */
	AliEngineAudioEffectVoiceChangerKTV: 6,         /* KTV */
	AliEngineAudioEffectVoiceChangerEcho: 7,        /* 回声 */
}

/**
 * @brief 音效模式
 */
const AliEngineAudioEffectReverbMode = {
	AliEngineAudioEffectReverbOff: 0,								 /* 关闭 */
	AliEngineAudioEffectReverbVocal_I: 1,            /* 人声 I */
	AliEngineAudioEffectReverbVocal_II: 2,           /* 人声 II */
	AliEngineAudioEffectReverbBathroom: 3,           /* 澡堂 */
	AliEngineAudioEffectReverbSmallRoomBright: 4,    /* 明亮小房间 */
	AliEngineAudioEffectReverbSmallRoomDark: 5,      /* 黑暗小房间 */
	AliEngineAudioEffectReverbMediumRoom: 6,         /* 中等房间 */
	AliEngineAudioEffectReverbLargeRoom: 7,          /* 大房间 */
	AliEngineAudioEffectReverbChurchHall: 8,         /* 教堂走廊 */
}

/**
 * @brief 音效混响模式
 */
const AliEngineAudioEffectReverbParamType = {
	AliEngineAudioEffectReverbRoomSize: 0, /* 房间大小 [0, 100] */
	AliEngineAudioEffectReverbPreDelay: 1,          /* 预延时ms [0, 200] */
	AliEngineAudioEffectReverbReverberance: 2,      /* 混响感[0,100] */
	AliEngineAudioEffectReverbHfDamping: 3,         /* 消声 [0,100] */
	AliEngineAudioEffectReverbToneLow: 4,           /* 低音调[0,100] */
	AliEngineAudioEffectReverbToneHigh: 5,          /* 高音调[0,100] */
	AliEngineAudioEffectReverbDryGain: 6,           /* 干增益[-20,10] */
	AliEngineAudioEffectReverbWetGain: 7,           /* 湿增益[-20,10] */
}

/**
 * @brief 录制类型
 */
const AliEngineRecordType = {
	AliEngineRecordTypeAudio: 0,
	// AliEngineRecordTypeVideo: 1,
	// AliEngineRecordTypeBoth: 2,
}

/**
 * @brief 录制格式
 */
const AliEngineRecordFormat = {
	AliEngineRecordFormatAAC: 0,
	AliEngineRecordFormatWAV: 1,
	// AliEngineRecordFormatMP4: 2,
}

/**
 * @brief 录制音频质量
 */
const AliEngineAudioQuality = {
	AliEngineAudioQualityLow: 0,
	AliEngineAudioQualityMidium: 1,
	AliEngineAudioQualityHigh: 2,
}

/**
 * @brief 录制视频质量
 */
const AliEngineVideoQuality = {
	AliEngineVideoQualityDefault: 0,
}

/**
* @brief 录制视频布局
*/
const AliEngineRecordVideoLayoutMode = {
	AliEngineRecordVideoLayoutModeGrid: 0,
	AliEngineRecordVideoLayoutModeSpeaker: 1,
	AliEngineRecordVideoLayoutModeUnique: 2,
	AliEngineRecordVideoLayoutModeCustom: 3,
}

/**
 * @brief OnBye返回类型
 */
const AliEngineOnByeType = {
	AliEngineOnByeBeKickOut: 1,
	AliEngineOnByeChannelTerminated: 2,
	AliEngineOnByeUserReplaced: 3,
}

/**
 * @brief SDK错误码
*/
const AliEngineErrorCode = {
	AliEngineErrorJoinBadAppId: 0x02010201,
	AliEngineErrorJoinInvaildAppId: 0x02010202,
	AliEngineErrorJoinBadChannel: 0x02010204,
	AliEngineErrorJoinInvaildChannel: 0x02010203,
	AliEngineErrorJoinBadToken: 0x02010205,
	AliEngineErrorJoinTimeout: 0x01020204,
	AliEngineErrorJoinBadParam: 0x01030101,
	AliEngineErrorJoinChannelFailed: 0x01030202,

	AliEngineErrorMicOpenFail: 0x01040404,
	AliEngineErrorSpeakerOpenFail: 0x01040405,
	AliEngineErrorMicInterrupt: 0x01040406,
	AliEngineErrorSpeakerInterrupt: 0x01040407,
	AliEngineErrorMicAuthFail: 0x01040408,
	AliEngineErrorMicNotAvailable: 0x01040409,
	AliEngineErrorSpeakerNotAvailable: 0x01040410,
	AliEngineErrorCameraOpenFail: 0x01040104,
	AliEngineErrorCameraInterrupt: 0x01040106,
	AliEngineErrorVideoDisplayOpenFail: 0x01040201,
	AliEngineErrorVideoDisplayInterrupt: 0x01040202,
	AliEngineErrorAudioBufferFull: 0x01070101,
	AliEngineErrorVideoBufferFull: 0x01070102,
	AliEngineErrorAudioStreamError: 0x01080101,
	AliEngineErrorAudioFileOpenFail: 0x01080102,
	AliEngineErrorAudioFormatNotSupported: 0x01080103,

	AliEngineErrorIceConnectionConnectFail: 0x01050201,
	AliEngineErrorIceConnectionReconnectFail: 0x01050202,
	AliEngineErrorIceConnectionHeartbeatTimeout: 0x0102020C,

	AliEngineErrorLowLatencyLiveCommunicationFail: 0x01070003,
	AliEngineErrorLowLatencyLiveMediaSettingFail: 0x01070004,
	AliEngineErrorLowLatencyLiveNoneBroadcaster: 0x01070005,
	AliEngineErrorLowLatencyLiveNeedRestart: 0x01070006,
	AliEngineErrorLowLatencyLiveNeedRestartAgainLater: 0x01070007,
	AliEngineErrorLowLatencyLiveServiceUnavailable: 0x01070008,
	AliEngineErrorLowLatencyLiveMediaConnectionInterrruption: 0x01070009,

	AliEngineErrorUpdateRoleChannel: 0x01070102,

	AliEngineErrorPublishInvaild: 0x01030305,
	AliEngineErrorPublishNotJoinChannel: 0x01010406,
	AliEngineErrorPublishAudioStreamFailed: 0x01010450,
	AliEngineErrorPublishVideoStreamFailed: 0x01010451,
	AliEngineErrorPublishDualStreamFailed: 0x01010452,
	AliEngineErrorPublishScreenShareFailed: 0x01010453,
	AliEngineErrorPublishScreenShareConfigError: 0x01010454,

	AliEngineErrorSubscribeInvaild: 0x01030404,
	AliEngineErrorSubscribeNotJoinChannel: 0x01010550,
	AliEngineErrorSubscribeAudioStreamFailed: 0x01010551,
	AliEngineErrorSubscribeVideoStreamFailed: 0x01010552,
	AliEngineErrorSubscribeDualStreamFailed: 0x01010553,
	AliEngineErrorSubscribeScreenShareFailed: 0x01010554,

	AliEngineErrorInvaildState: 0x01030204,
	AliEngineErrorSessionRemoved: 0x02010105,
	AliEngineErrorInvaildArgument: 0x01030103,
	AliEngineErrorFailedToJoinChannelDueToInvaildChannelProfile: 0x02010903,
	AliEngineErrorFailedToJoinChannelDueToAlreadyInChannel: 0x01010301,
	AliEngineErrorFailedToLeaveChannelDueToAlreadyLeaved: 0x01010302,
	AliEngineErrorFailedToUpdateRole: 0x01020110,
	AliEngineErrorFailedToPublishDueToWrongRole: 0x0103030d,
	
	AliEngineErrorInner: -1,
}

/**
 * SDK反馈问题类型
 */
const AliEngineFeedbackType = {
	AliEngineFeedbackVideoDeviceFail: 101,
	AliEngineFeedbackVideoNotRender: 102,
	AliEngineFeedbackAudioDeviceFail: 201,
	AliEngineFeedbackAudioNotRender: 202,
	AliEngineFeedbackAudioEchoError: 203,
	AliEngineFeedbackChannelTypeError: 301,
	AliEngineFeedbackSDKTypeError: 302,
	AliEngineFeedbackNetworkUnfluent: 401,
	AliEngineFeedbackVideoBlurring: 402,
	AliEngineFeedbackUnkonw: -1,
}

/**
 * @brief 音乐伴奏播放状态
 */
const AliEngineAudioPlayingType = {
	AliEngineAudioPlayingStarted: 100,
	AliEngineAudioPlayingStopped: 101,
	AliEngineAudioPlayingPaused: 102,
	AliEngineAudioPlayingResumed: 103,
	AliEngineAudioPlayingEnded: 104,
	AliEngineAudioPlayingBuffering: 105,
	AliEngineAudioPlayingBufferingEnd: 106,
	AliEngineAudioPlayingFailed: 107,
}

/**
 * @brief 音乐伴奏播放错误码
 */
const AliEngineAudioPlayingErrorCode = {
	AliEngineAudioPlayingNoError: 0,
	AliEngineAudioPlayingOpenFailed: -100,
	AliEngineAudioPlayingDecodeFailed: -101,
}

/**
 * @brief 网络连接状态
 */
const AliEngineConnectionStatus = {
	AliEngineConnectionInit: 0,       /* 初始化完成 */
	AliEngineConnectionDisconnected: 1,   /* 网络连接断开 */
	AliEngineConnectionConnecting: 2,     /* 建立网络连接中 */
	AliEngineConnectionConnected: 3,      /* 网络已连接 */
	AliEngineConnectionReconnecting: 4,   /* 重新建立网络连接中 */
	AliEngineConnectionFailed: 5,         /* 网络连接失败 */
}

/**
 * @brief 网络连接状态变更原因
*/
const AliEngineConnectionStatusChangeReason = {
	AliEngineConnectionChangedDummyReason: 0,
	AliEngineConnectionMediaPathChanged: 1,
	AliEngineConnectionSignalingHeartbeatTimeout: 2,
	AliEngineConnectionSignalingHeartbeatAlive: 3,
	AliEngineConnectionSignalingHttpdnsResolved: 4,
	AliEngineConnectionSignalingHttpdnsFailure: 5,
	AliEngineConnectionSignalingGslbFailure: 6,
	AliEngineConnectionSignalingGslbSucccess: 7,
	AliEngineConnectionSignalingJoinChannelFailure: 8,
	AliEngineConnectionSignalingJoinChannelSuccess: 9,
	AliEngineConnectionSignalingLeaveChannel: 10,
	AliEngineConnectionSignalingConnecting: 11,
}

/**
 * @brief 桌面分享类型
 */
const AliEngineScreenShareType = {
	AliEngineScreenShareDesktop: 0,      /* 桌面分享 */
	AliEngineScreenShareWindow: 1,       /* 窗口分享 */
}

/**
 * @brief 外接设备类型
*/
const AliEngineExternalDeviceType = {
	AliEngineExternalTypeUnknown: -1,       /* 未知类型 */
	AliEngineExternalTypeAudioCapture: 0,   /* 音频采集设备 */
	AliEngineExternalTypeAudioPlayout: 1,   /* 音频播放设备 */
	AliEngineExternalTypeVideoCapture: 2,   /* 视频采集设备 */
}

/**
 * @brief 外接设备状态
 */
const AliEngineExternalDeviceState = {
	AliEngineExternalDeviceAdd: 0,      /* 新增外接设备 */
	AliEngineExternalDeviceRemove: 1,   /* 移除外接设备 */
}

/**
 * @brief 语音路由类型
*/
const AliEngineAudioRouteType = {
	AliEngineAudioRouteTypeDefault: 0, /* 默认的语音路由 */
	AliEngineAudioRouteTypeHeadset: 1, /* 耳机 */
	AliEngineAudioRouteTypeEarpiece: 2, /* 听筒 */
	AliEngineAudioRouteTypeHeadsetNoMic: 3, /* 不带麦的耳机 */
	AliEngineAudioRouteTypeSpeakerphone: 4, /* 手机扬声器 */
	AliEngineAudioRouteTypeLoudSpeaker: 5, /* 外接扬声器 */
	AliEngineAudioRouteTypeBlueTooth: 6, /* 蓝牙耳机 */
}

/**
 * @brief 音频焦点类型
*/
const AliEngineAudioFocusType = {
	AliEngineAudioFocusNone: 0,
	AliEngineAudioFocusGain: 1,
	AliEngineAudioFocusGainTransient: 2,
	AliEngineAudioFocusGainTransientMayDuck: 3,
	AliEngineAudioFocusGainTransientExclusive: 4,
	AliEngineAudioFocusLoss: -1,
	AliEngineAudioFocusLossTransient: -2,
	AliEngineAudioFocusLossCanDuck: -3
}

/**
 * @brief 推流状态
*/
const AliEnginePublishState = {
	AliEngineStatsPublishIdle: 0,  /* 初始状态 */
	AliEngineStatsNoPublish: 1,    /* 未推流 */
	AliEngineStatsPublishing: 2,   /* 推流中 */
	AliEngineStatsPublished: 3,    /* 已推流 */
}

/**
 * @brief 订阅状态
*/
const AliEngineSubscribeState = {
	AliEngineStatsSubscribeIdle: 0,  /* 初始状态 */
	AliEngineStatsNoSubscribe: 1,    /* 未订阅 */
	AliEngineStatsSubscribing: 2,    /* 订阅中 */
	AliEngineStatsSubscribed: 3,     /* 已订阅 */
}

/**
 * @brief 旁路推流LiveStream状态
*/
const AliEngineTrascodingLiveStreamStatus = {
	AliEngineTrascodingStatusStart: 0,               /* 任务开始 */
	AliEngineTrascodingStatusUpdate: 1,              /* 任务更新 */
	AliEngineTrascodingStatusStop: 2,                /* 任务停止 */
	AliEngineTrascodingStatusEnd: 3,                 /* 推流未开始或已结束 */
	AliEngineTrascodingStatusConnect: 4,             /* 正在连接推流服务器和 RTMP 服务器 */
	AliEngineTrascodingStatusRunning: 5,             /* 推流正在进行 */
	AliEngineTrascodingStatusRecovering: 6,          /* 正在恢复推流 */
	AliEngineTrascodingStatusFailed: 7,              /* 推流失败 */
	AliEngineTrascodingStatusIdle: 8,                /* 当前流空闲 */
}

/**
 * @brief 旁路推流错误码
*/
const AliEngineTrascodingLiveStreamErrorCode = {
	AliEngineTrascodingLiveStreamErrorUserStop: 0,                 /* 用户停止任务 */
	AliEngineTrascodingLiveStreamErrorChannelStop: 1,              /* channel已停止 */
	AliEngineTrascodingLiveStreamErrorInvalidNetwork: 2,           /* 网络/CDN问题,直播停止 */
	AliEngineTrascodingLiveStreamErrorInvalidSteamURL: 3,          /* 直播URL问题,直播停止 */
}

/**
 * @brief 旁路推流PublsihTask状态
*/
const AliEngineTrascodingPublsihTaskStatus = {
	AliEngineTrascodingPublsihTaskStatusStart: 0,                /* 任务开始 */
	AliEngineTrascodingPublsihTaskStatusUpdate: 1,               /* 任务更新 */
	AliEngineTrascodingPublsihTaskStatusStop: 2,                 /* 任务已停止 */
}

/**
 * @brief 用户离线原因
*/
const AliEngineUserOfflineReason = {
	AliEngineUserOfflineQuit: 0,              /* 用户主动离开 */
	AliEngineUserOfflineDropped: 1,           /* 因过长时间收不到对方数据包，超时掉线 */
	AliEngineUserOfflineBecomeAudience: 2,    /* 用户身份从主播切换为观众时触发 */
}

/**
 * @brief 音频编码配置
*/
const AliEngineAudioProfile = {
	AliEngineLowQualityMode: 0x0000,               /* 1ch, 8khz,  24kbps */
	AliEngineBasicQualityMode: 0x0001,             /* 1ch, 16khz, 24kbps */
	AliEngineHighQualityMode: 0x0010,              /* 1ch, 48khz, 48kbps */
	AliEngineStereoHighQualityMode: 0x0011,        /* 2ch, 48khz, 64kbps */
	AliEngineSuperHighQualityMode: 0x0012,         /* 1ch, 48khz, 96kbps */
	AliEngineStereoSuperHighQualityMode: 0x0013,   /* 2ch, 48khz, 128kbps */
}

/**
 * @brief 音频场景
*/
const AliEngineAudioScenario = {
	AliEngineSceneDefaultMode: 0x0000,
	AliEngineSceneEducationMode: 0x0100,
	AliEngineSeneMediaMode: 0x0200,
	AliEngineSceneMusicMode: 0x0300,
}

/**
 * @brief 跨频道转发状态
*/
const AliEngineChannelRelayEvent = {
	AliEngineChannelRelayEventStart: 0,
	AliEngineChannelRelayEventUpdate: 1,
	AliEngineChannelRelayEventStop: 2,
}

/**
 * @brief 跨频道转发状态
*/
const AliEngineChannelRelayState = {
	AliEngineChannelRelayStateInit: 3,
	AliEngineChannelRelayStateConnecting: 4,
	AliEngineChannelRelayStateRunning: 5,
	AliEngineChannelRelayStateFailure: 6,
}

/**
 * @brief 跨频道转发错误码
*/
const AliEngineChannelRelayErrorCode = {
	AliEngineChannelRelayErrorStateError: 0x01200001,       /*检测到本地状态不对，没入会，角色错误，频道模式错误 */
	AliEngineChannelRelayErrorSrcNotAllow: 0x01200002,      /*服务端返回436，源通道模式不匹配 */
	AliEngineChannelRelayErrorJoinDestFailed: 0x01200003,   /*服务端返回437，目标通道不存在或者模式不匹配 */
	AliEngineChannelRelayErrorTokenInvalid: 0x01200004,     /*服务端返回438，token无效 */
	AliEngineChannelRelayErrorRoleError: 0x01200005,        /*服务端返651，session不是主播 */
	AliEngineChannelRelayErrorInvalidParam: 0x01200006,     /*服务端400，参数错误 */
	AliEngineChannelRelayErrorToSelf: 0x01200007,           /*服务端返回440，不能转推到本频道 */
}

/**
 * @brief 视频帧率
 */
const AliEngineFrameRate = {
	/** 5: 5 fps */
	AliEngineFrameRateFps5: 5,
	/** 10: 10 fps */
	AliEngineFrameRateFps10: 10,
	/** 15: 15 fps */
	AliEngineFrameRateFps15: 15,
	/** 20: 20 fps */
	AliEngineFrameRateFps20: 20,
	/** 25: 25 fps */
	AliEngineFrameRateFps25: 25,
	/** 30: 30 fps */
	AliEngineFrameRateFps30: 30,
}

/**
 * @brief 视频编码码率
 */
const AliEngineVideoEncoderBitrate = {
	AliEngineVideoStandardBitrate: 0,
}

/**
 * @brief 视频输出模式
 */
const AliEngineVideoEncoderOrientationMode = {
	/** 0: (Default) Adaptive mode.
	 */
	AliEngineVideoEncoderOrientationModeAdaptive: 0,
	/** 1: Landscape mode.

		The video encoder always sends the video in landscape mode. The video encoder rotates the original video before sending it and the rotational infomation is 0. This mode applies to scenarios involving CDN live streaming.
		*/
	AliEngineVideoEncoderOrientationModeFixedLandscape: 1,
	/** 2: Portrait mode.

		The video encoder always sends the video in portrait mode. The video encoder rotates the original video before sending it and the rotational infomation is 0. This mode applies to scenarios involving CDN live streaming.
		*/
	AliEngineVideoEncoderOrientationModeFixedPortrait: 2,
}

/**
 * @brief 视频镜像模式
 */
const AliEngineVideoMirrorMode = {
	/** 0: (Default) disable the mirror mode.
	*/
	AliEngineVideoMirrorModeDisabled: 0,//disable mirror
	/** 1: Enable mirror mode. */
	AliEngineVideoMirrorModeEnable: 1,//enabled mirror
}

/**
	 * @brief 音频track的类型
	*/
const AliEngineAudioTrack = {
	AliEngineAudioTrackNo: 0,
	AliEngineAudioTrackMic: 1,
	AliEngineAudioTrackEnd: 0xffffffff
}

/**
 * @brief 视频track的类型
 */
const AliEngineVideoTrack = {
	AliEngineVideoTrackNo: 0,
	AliEngineVideoTrackCamera: 1,
	AliEngineVideoTrackScreen: 2,
	AliEngineVideoTrackBoth: 3
}


/**
 * @brief 相机流类型
 */
const AliEngineVideoStreamType = {
	AliEngineVideoStreamTypeNone: 0,
	AliEngineVideoStreamTypeHigh: 1,
	AliEngineVideoStreamTypeLow: 2
}

/**
 * @brief 视频数据源
 */
const AliEngineVideoSource = {
	AliEngineVideoSourceCamera: 0,
	AliEngineVideoSourceScreenShare: 1,
	AliEngineVideosourceTypeMax: 2,
}

/**
 * @brief 录制视频数据源
*/
const AliEngineVideoRecordSource = {
	AliEngineVideoRecordSourceCamera: 0,
	AliEngineVideoRecordSourceScreenShare: 1,
	AliEngineVideoRecordSourceBoth: 2,
	AliEngineVideoRecordSourceTypeMax: 3,
}

/**
 * @brief 视频数据类型
 */
const AliEngineVideoFormat = {
	AliEngineVideoFormatUnknow: -1,
	AliEngineVideoFormatBGRA: 0,
	AliEngineVideoFormatI420: 1,
	AliEngineVideoFormatNV21: 2,
	AliEngineVideoFormatNV12: 3,
	AliEngineVideoFormatRGBA: 4,
	AliEngineVideoFormatI422: 5,
	AliEngineVideoFormatARGB: 6,
	AliEngineVideoFormatABGR: 7,
	AliEngineVideoFormatRGB24: 8,
	AliEngineVideoFormatBGR24: 9,
	AliEngineVideoFormatRGB565: 10,
	AliEngineVideoFormatTextureOES: 11,
	AliEngineVideoFormatTexture2D: 12,
}

/**
 * @brief 视频数据格式
 */
const AliEngineBufferType = {
	AliEngineBufferTypeRawData: 0,
	AliEngineBufferTypeTexture: 1,
}


/**
 * @brief 视频纹理格式
 */
const AliEngineTextureType = {
	AliEngineTextureTypeUnknown: -1,
	AliEngineTextureTypePre: 0,
	AliEngineTextureTypePost: 1,
}

/**
 * @brief 视频窗口填充方式
 */
const AliEngineRenderMode = {
	AliEngineRenderModeAuto: 0,
	AliEngineRenderModeStretch: 1,
	AliEngineRenderModeFill: 2,
	AliEngineRenderModeCrop: 3,
	AliEngineRenderModeScroll: 4,
	AliEngineRenderModeNoChange: 99,
}

/**
 * @brief 视频旋转角度
 */
const AliEngineRotationMode = {
	AliEngineRotationMode_0: 0,
	AliEngineRotationMode_90: 90,
	AliEngineRotationMode_180: 180,
	AliEngineRotationMode_270: 270,
}

/**
 * @brief 音频声道类型
*/
const AliEngineAudioNumChannelType = {
	AliEngineMonoAudio: 1,
	AliEngineStereoAudio: 2,
}

/**
 * @brief 音频采样率类型
 */
const AliEngineAudioSampleRate = {
	AliEngineAudioSampleRate_8000: 0,
	AliEngineAudioSampleRate_11025: 1,
	AliEngineAudioSampleRate_16000: 2,
	AliEngineAudioSampleRate_22050: 3,
	AliEngineAudioSampleRate_32000: 4,
	AliEngineAudioSampleRate_44100: 5,
	AliEngineAudioSampleRate_48000: 6,
}

/**
 * @brief 音频数据源
 */
const AliEngineAudioSource = {
	AliEngineAudiosourcePub: 0,
	AliEngineAudiosourceSub: 1,
	AliEngineAudiosourceRawData: 2,
}

/**
 * @brief 视频数据输出位置
 */
const AliEngineVideoObserPosition = {
	AliEnginePositionPostCapture: 1 << 0,
	AliEnginePositionPreRender: 1 << 1,
	AliEnginePositionPreEncoder: 1 << 2,
}

/**
 * @brief 视频输出宽度对齐方式
 * AliEngineAlignmentDefault：保持原有视频宽度
 * AliEngineAlignmentEven：宽度偶数对齐
 * AliEngineAlignment4：宽度是4的倍数
 * AliEngineAlignment8：宽度是8的倍数
 * AliEngineAlignment16：宽度是16的倍数
 */
const AliEngineVideoObserAlignment = {
	AliEngineAlignmentDefault: 0,
	AliEngineAlignmentEven: 1,
	AliEngineAlignment4: 2,
	AliEngineAlignment8: 3,
	AliEngineAlignment16: 4,
}

const AliEngineDeviceTransportType = {
	AliEngineDeviceTransportTypeUnknown    : 0,
	AliEngineDeviceTransportTypeBuiltIn    : 1,
	AliEngineDeviceTransportTypeBluetooth  : 2,
	AliEngineDeviceTransportTypeUSB        : 3,
	AliEngineDeviceTransportTypeAggregate  : 4,
	AliEngineDeviceTransportTypeVirtual    : 5,
	AliEngineDeviceTransportTypePCI        : 6,
	AliEngineDeviceTransportTypeFireWire   : 7,
	AliEngineDeviceTransportTypeBluetoothLE: 8,
	AliEngineDeviceTransportTypeHDMI       : 9,
	AliEngineDeviceTransportTypeDisplayPort: 10,
	AliEngineDeviceTransportTypeAirPlay    : 11,
	AliEngineDeviceTransportTypeAVB        : 12,
	AliEngineDeviceTransportTypeThunderbolt: 13
};


export {
	AliEngineInterfaceIdType,
	AliEngineCameraDirection,
	AliEngineCaptureOutputPreference,
	AliEngineOrientationMode,
	AliEngineAudioSessionOperationRestriction,
	AliEngineAudioEffectVoiceChangerMode,
	AliEngineAudioPlayingType,
	AliEngineAudioPlayingErrorCode,
	AliEngineConnectionStatus,
	AliEngineConnectionStatusChangeReason,
	AliEngineAudioRouteType,
	AliEngineAudioFocusType,
	AliEnginePublishState,
	AliEngineSubscribeState,
	AliEngineTrascodingLiveStreamStatus,
	AliEngineTrascodingLiveStreamErrorCode,
	AliEngineTrascodingPublsihTaskStatus,
	AliEngineChannelRelayEvent,
	AliEngineChannelRelayState,
	AliEngineChannelRelayErrorCode,
	AliEngineVideoTrack,
	AliEngineAudioTrack,
	AliEngineVideoStreamType,
	AliEngineVideoSource,
	AliEngineVideoRecordSource,
	AliEngineVideoFormat,
	AliEngineBufferType,
	AliEngineRenderMode,
	AliEngineRotationMode,
	AliEngineAudioNumChannelType,
	AliEngineAudioSampleRate,
	AliEngineAudioSource,
	AliEngineVideoObserPosition,
	AliEngineNetworkQuality,
	AliEngineLogLevel,
	AliEngineOnByeType,
	AliEngineUserOfflineReason,
	AliEngineScreenShareType,
	AliEngineClientRole,
	AliEngineExternalDeviceType,
	AliEngineExternalDeviceState,
	AliEngineVideoScale,
	AliEngineMuteLocalAudioMode,
	AliEngineAudioEffectReverbMode,
	AliEngineAudioEffectReverbParamType,
	AliEngineAudioProfile,
	AliEngineAudioScenario,
	AliEngineRecordType,
	AliEngineRecordFormat,
	AliEngineAudioQuality,
	AliEngineVideoQuality,
	AliEngineRecordVideoLayoutMode,
	AliEngineFrameRate,
	AliEngineVideoEncoderBitrate,
	AliEngineVideoEncoderOrientationMode,
	AliEngineVideoMirrorMode,
	AliEngineRenderMirrorMode,
	AliEngineChannelProfile,
	AliEngineFeedbackType,
	AliEngineErrorCode,
	AliEngineDeviceTransportType
}