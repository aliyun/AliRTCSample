# dingrtc_wrapper.py 是在dingrtc.py基础上的一层封装，屏蔽
# 不需要的接口，简化参数的使用。

import dingrtc

class RtmClient:
  def __init__(self):
    self.rtmClient:dingrtc.RtmClient = None

  def JoinSession(self, sessionId:str) -> int:
    if not self.rtmClient:
      return -1
    return self.rtmClient.JoinSession(dingrtc.String(sessionId))

  def LeaveSession(self, sessionId:str) -> int:
    if not self.rtmClient:
      return -1
    return self.rtmClient.LeaveSession(dingrtc.String(sessionId))

  def SetListener(self, listener:'RtmEventListener') -> None:
    if not self.rtmClient:
      return
    self.rtmClient.SetListener(listener)

  def BroadcastData(self, sessionId:str, data:bytes) -> int:
    if not self.rtmClient:
      return -1
    return self.rtmClient.BroadcastDataEx(sessionId, data, len(data))

  def SendData(self, sessionId:str, toUid:str, data:bytes) -> int:
    if not self.rtmClient:
      return -1
    return self.rtmClient.SendDataEx(sessionId, toUid, data, len(data))

class RtcEngine:
  """
  DingRTC引擎的简易接口封装。
  """
  def __init__(self):
    self.engine:dingrtc.RtcEngine = None
    self.listener = None
    self.audio_observer = None
    self.video_observer = None
    self.rtmClient:RtmClient = RtmClient()

  @staticmethod
  def GetSDKVersion() -> str:
    """
    Get the SDK version.
    @return str: The version string of the SDK.
    """
    return dingrtc.RtcEngine.GetSDKVersion()

  @staticmethod
  def SetLogLevel(level: int) -> None:
    """
    Set the log level.
    @param level (int): Log level to set.
    """
    dingrtc.RtcEngine.SetLogLevel(level)

  @staticmethod
  def SetLogDirPath(path: str) -> None:
    """
    Set the log directory path.
    @param path (str): Path to the log directory.
    """
    dingrtc.RtcEngine.SetLogDirPath(path)

  def Create(self, context: str = "") -> bool:
    """
    Create an RtcEngine instance.
    @param context (str): Context for engine creation.
    @return bool: True if the engine is created successfully, False otherwise.
    @note 只有Create成功之后，才能调用engine的其他非静态接口。
    """
    if self.engine:
      return False

    self.engine = dingrtc.RtcEngine.Create(context)
    return True

  def Destroy(self) -> None:
    """
    Destroy an RtcEngine instance.
    """
    if self.engine:
      self.engine.PublishLocalAudioStream(False)
      self.engine.SubscribeAllRemoteAudioStreams(False)
      self.engine.RegisterAudioFrameObserver(None)
      self.engine.EnableAudioFrameObserver(False, 0xffffffff)
      self.engine.PublishLocalVideoStream(False)
      self.engine.SubscribeAllRemoteVideoStreams(False, False)
      self.engine.RegisterVideoFrameObserver(None)
      self.engine.EnableVideoFrameObserver(False, 0xffffffff)

      self.engine.LeaveChannel()

      dingrtc.RtcEngine.Destroy(self.engine)
      self.engine = None
    self.listener = None
    self.audio_observer = None
    self.video_observer = None

  def SetEngineEventListener(self, listener:'RtcEngineEventListener') -> int:
    """
    Set the RtcEngineEventListener.
    @param listener (RtcEngineEventListener): RtcEngineEventListener instance.
    """
    if self.engine:
      result = self.engine.SetEngineEventListener(listener)
      if result == 0:
        self.listener = listener
      return result
    else:
      return -1

  def JoinChannel(self, authInfo:dingrtc.RtcEngineAuthInfo, userName:str) -> int:
    """
    Join a channel.
    @param authInfo (RtcEngineAuthInfo): Auth info for joining channel.
    @return int: Error code. 0表示api调用成功，是否入会成功等待
                 OnJoinChannelResultEx消息。<0表示api调用失败，无
                 OnJoinChannelResultEx消息通知。
    通知。
    """
    if not self.engine:
      return -1
    return self.engine.JoinChannel(authInfo, userName)

  def LeaveChannel(self) -> int:
    """
    Leave the current channel.
    """
    if not self.engine:
      return -1
    return self.engine.LeaveChannel()

  def SetExternalAudioSource(self, enabled:bool, sampleRate:int, channels:int) -> int:
    """
    设置外部音频源参数。
    @param enabled (bool): Whether to enable the external audio source.
    @param sampleRate (int): Sample rate of the external audio source, 8000, 16000, 320000, 44100, 48000。
    @param channels (int): Number of channels of the external audio source。取值1:单声道，2:双声道。
    @return int: Error code. 0表示api调用成功，<0表示api调用失败。
    """
    if not self.engine:
      return -1
    return self.engine.SetExternalAudioSource(enabled, sampleRate, channels)

  def PushExternalAudioFrame(self, frame:dingrtc.RtcEngineAudioFrame) -> int:
    """
    Push an external audio frame.
    @param frame (RtcEngineAudioFrame): External audio frame.
    @return int: Error code. 0表示api调用成功，<0表示api调用失败。
    """
    if not self.engine:
      return -1
    return self.engine.PushExternalAudioFrame(frame)

  def PublishLocalAudioStream(self, enabled:bool) -> int:
    """
    设置是否启用外部音频源。
    @param enabled (bool): 是否启用外部音频源。
    @return int: Error code. 0表示api调用成功，<0表示api调用失败。
    @note SDK默认设置不推送音频流，在加入频道前也可以调用此接口修改默认值，并在加入频道成功时生效。
    """
    if not self.engine:
      return -1
    return self.engine.PublishLocalAudioStream(enabled)

  def PublishLocalVideoStream(self, enabled:bool) -> int:
    """
    设置是否启用外部视频源。
    @param enabled (bool): 是否启用外部视频源。
    @return int: Error code. 0表示api调用成功，<0表示api调用失败。
    @note SDK默认设置不推送视频流，在加入频道前也可以调用此接口修改默认值，并在加入频道成功时生效。
    """
    if not self.engine:
      return -1
    return self.engine.PublishLocalVideoStream(enabled)

  def SetExternalVideoSource(self, enabled:bool, track:int) -> int:
    """
    设置外部视频源参数。
    @param enabled (bool): 是否启用外部视频源。
    @param track (int): dingrtc.RtcEngineVideoTrackCamera or dingrtc.RtcEngineVideoTrackScreen。
    @return int: Error code. 0表示api调用成功，<0表示api调用失败。
    """
    if not self.engine:
      return -1
    return self.engine.SetExternalVideoSource(enabled, track)

  def PushExternalVideoFrame(self, frame:dingrtc.RtcEngineVideoFrame, track:int) -> int:
    """
    Push an external video frame.
    @param frame (RtcEngineVideoFrame): External video frame.
    @param track (int): dingrtc.RtcEngineVideoTrackCamera or dingrtc.RtcEngineVideoTrackScreen。
    @return int: Error code. 0表示api调用成功，<0表示api调用失败。
    """
    if not self.engine:
      return -1
    return self.engine.PushExternalVideoFrame(frame, track)

  def SubscribeAllRemoteAudioStreams(self, enabled:bool) -> int:
    """
    设置是否订阅所有远端用户的音频流。
    @param enabled (bool): 是否订阅所有远端用户的音频流。
    @return int: Error code. 0表示api调用成功，<0表示api调用失败。
    """
    if not self.engine:
      return -1
    return self.engine.SubscribeAllRemoteAudioStreams(enabled)

  def SubscribeAllRemoteVideoStreams(self, allCameraTracks:bool, allScreenShareTracks:bool) -> int:
    """
    设置是否订阅所有远端用户的视频流。
    @param allCameraTracks (bool): 是否订阅所有远端用户的摄像头视频流。
    @param allScreenShareTracks (bool): 是否订阅所有远端用户的屏幕共享视频流。
    @return int: Error code. 0表示api调用成功，<0表示api调用失败。
    """
    if not self.engine:
      return -1
    return self.engine.SubscribeAllRemoteVideoStreams(allCameraTracks, allScreenShareTracks)

  def RegisterAudioFrameObserver(self, observer:dingrtc.RtcEngineAudioFrameObserver) -> int:
    """
    注册音频帧观察者。
    @param observer (RtcEngineAudioFrameObserver): 音频帧观察者。
    @return int: Error code. 0表示api调用成功，<0表示api调用失败。
    """
    if not self.engine:
      return -1
    result = self.engine.RegisterAudioFrameObserver(observer)
    if result == 0:
      self.audio_observer = observer
    return result

  def EnableAudioFrameObserver(self, enabled:bool, position:int, audioFrameConfig:dingrtc.RtcEngineAudioFrameObserverConfig) -> int:
    """
    设置是否启用音频帧观察者。
    @param enabled (bool): 是否启用音频帧观察者。
    @return int: Error code. 0表示api调用成功，<0表示api调用失败。
    """
    if not self.engine:
      return -1
    return self.engine.EnableAudioFrameObserver(enabled, position, audioFrameConfig)

  def RegisterVideoFrameObserver(self, observer:dingrtc.RtcEngineVideoFrameObserver) -> int:
    """
    注册视频帧观察者。
    @param observer (RtcEngineVideoFrameObserver): 视频帧观察者。
    @return int: Error code. 0表示api调用成功，<0表示api调用失败。
    """
    if not self.engine:
      return -1

    result = self.engine.RegisterVideoFrameObserver(observer)
    if result == 0:
      self.video_observer = observer
    return result

  def EnableVideoFrameObserver(self, enabled:bool, position:int) -> int:
    """
    设置是否启用音频帧观察者。
    @param enabled (bool): 是否启用音频帧观察者。
    @return int: Error code. 0表示api调用成功，<0表示api调用失败。
    """
    if not self.engine:
      return -1
    return self.engine.EnableVideoFrameObserver(enabled, position)

  def GetRtmClient(self) -> dingrtc.RtmClient:
    """
    获取RtmClient实例。
    @return RtmClient: RtmClient实例。
    """
    if not self.engine:
      return None
    if not self.rtmClient.rtmClient:
      self.rtmClient.rtmClient = self.engine.GetRtmClient()
    return self.rtmClient

class RtcEngineEventListener(dingrtc.RtcEngineEventListener):
  """
  获得DingRTC引擎的各种消息通知。详细的文档，见 engine_interface.h。
  """

  def OnJoinChannelResult(self, result:int, channel:str, userId:str, elapsed:int):
    """
    当JoinChannel调用结束时，会收到此消息。注意：JoinChannel在特殊情况下不会触发
    此消息。
    @param result (int): 错误码，0表示成功，<0表示失败。
    @param channel (str): 频道名称。
    @param userId (str): 用户ID。
    @param elapsed (int): 从调用JoinChannel开始到本消息通知被触发的耗时，单位ms。
    @return None。
    @note 当result为0时，表示成功加入频道，可以和频道内其他user进行音视频通讯，收发实时
          数据等。
    """
    pass

class RtcEngineAudioFrameObserver(dingrtc.RtcEngineAudioFrameObserver):
  """
  获得DingRTC引擎的音频帧。
  """
  def OnRemoteUserAudioFrame(self, uid:str, frame:dingrtc.RtcEngineAudioFrame):
    """
    收到远端用户的音频帧。
    @param uid (str): 用户ID。
    @param frame (RtcEngineAudioFrame): 音频帧。
    @return None。
    """
    pass

class RtcEngineVideoFrameObserver(dingrtc.RtcEngineVideoFrameObserver):
  """
  获得DingRTC引擎的视频帧。
  """
  def GetVideoFormatPreference(self):
    """
    指定视频帧的格式。
    @return RtcEngineVideoFormat: 视频帧的格式。
    """
    return dingrtc.RtcEngineVideoI420

  def GetI420MemContinuous(self):
    """
    指定I420视频帧内存连续，并且ystride == width, cstride == width/2
    @return bool: 是否要求I420内存连续。
    """
    return True

  def OnRemoteVideoFrameEx(self, uid:str, track, frame:dingrtc.RtcEngineVideoFrame):
    """
    收到远端用户的视频帧。
    @param uid (str): 用户ID。
    @param track (int): 视频帧的流类型。
    @param frame (RtcEngineVideoFrame): 视频帧。
    @return None。
    """
    pass

  def OnPreDecodeVideoFrameEx(self, uid:str, track, frame:dingrtc.RtcEngineEncodedVideoFrame):
    """
    收到远端用户的视频帧。
    @param uid (str): 用户ID。
    @param track (int): 音频帧的流类型。
    @param frame (RtcEngineEncodedVideoFrame): 视频帧。
    @return None。
    """
    pass

  """ 以下方法不要继承 """
  def OnRemoteVideoFrame(self, uid:dingrtc.String, track, frame:dingrtc.RtcEngineVideoFrame):
    """
    继承类不要重写此方法，应该重写OnRemoteVideoFrameEx。
    """
    pyuid = uid.c_str()
    self.OnRemoteVideoFrameEx(pyuid, track, frame)
    return True

  def OnPreDecodeVideoFrame(self, uid:dingrtc.String, track, frame:dingrtc.RtcEngineEncodedVideoFrame):
    """
    继承类不要重写此方法，应该重写OnPreDecodeVideoFrameEx。
    """
    pyuid = uid.c_str()
    self.OnPreDecodeVideoFrameEx(pyuid, track, frame)

class RtmEventListener(dingrtc.RtmEventListener):
  """
  获得DingRTC引擎的RTM消息。详细的文档，见 engine_rtm.h。
  """
  def OnRtmServerStateChanged(self, state, erroCode:int):
    """
    RTM服务器状态改变。
    @param state (int): 状态。1表示可以使用，0表示暂时不可用。
    @param erroCode (int): 错误码。
    @return None。
    @note 当state为1时，表示可以使用，调用rtmclient.JoinSession加入消息session。
    """
    pass

  def OnJoinSessionResultEx(self, sessionId:str, result:int):
    """
    加入会话结果。
    @param sessionId (str): 会话ID。
    @param result (int): 错误码。
    @return None。
    """
    pass

  def OnMessageEx(self, sessionId:str, fromUid:str, broadcast:bool, data:bytes):
    """
    收到消息。
    @param sessionId (str): 会话ID。
    @param fromUid (str): 消息发送者ID。
    @param broadcast (bool): 是否是广播消息。
    @param data (bytes): 消息内容。
    @return None。
    """
    pass

  """ 以下方法不要继承 """
  def OnJoinSessionResult(self, sessionId:dingrtc.String, result:int):
    """
    加入会话结果。
    @param sessionId (str): 会话ID。
    @param result (int): 结果。0表示成功，<0表示失败。
    @return None。
    """
    pySessionId = sessionId.c_str()
    self.OnJoinSessionResultEx(pySessionId, result)

  def OnMessage(self, sessionId:dingrtc.String, fromUid:dingrtc.String, broadcast:bool, data:dingrtc.RtmData):
    """
    收到消息。
    @param sessionId (dingrtc.String): 会话ID。
    @param fromUid (dingrtc.String): 消息发送者ID。
    @param broadcast (bool): 是否是广播消息。
    @param data (RtmData): 消息内容。
    @return None。
    """
    pySessionId = sessionId.c_str()
    pyFromUid = fromUid.c_str()
    self.OnMessageEx(pySessionId, pyFromUid, broadcast, data.data)
