import os
import sys
import time
import threading
from typing import Optional, Callable, Any

# Add parent directory to path
#sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

import dingrtc
import dingrtc_wrapper
import requests
import signal
import asyncio
import weakref
from weakref import ReferenceType
import queue

class MyRtcEventListener(dingrtc_wrapper.RtcEngineEventListener):
  """
  获得DingRTC引擎的各种消息通知。详细的文档，见 engine_interface.h。
  """
  def __init__(self, join_event: asyncio.Event):
    super().__init__()
    self.join_event:asyncio.Event = join_event

  def OnJoinChannelResult(self, result:int, channel:str, userId:str, elapsed:int):
    print(f"Python MyRtcEventListener OnJoinChannelResult {result}")
    # Notify main() that join channel completed. event.set() not work here.
    #self.join_event.set()
    self.join_event._loop.call_soon_threadsafe(self.join_event.set)

class MyAudioFrameObserver(dingrtc_wrapper.RtcEngineAudioFrameObserver):
  def __init__(self, input_stream_ref: Optional[ReferenceType[Any]] = None):
    super().__init__()
    self.input_stream_ref = input_stream_ref

  def OnRemoteUserAudioFrame(self, uid:str, frame:dingrtc.RtcEngineAudioFrame):
    if self.input_stream_ref:
      input_stream = self.input_stream_ref()
      if input_stream and hasattr(input_stream, 'enqueue_audio_data'):
        success = input_stream.enqueue_audio_data(frame)
        if not success:
          print(f"Failed to write audio frame for user {uid}")
      else:
        print("Input stream not available or doesn't support enqueue_audio_data")
    else:
      # Fallback: just print for debugging
      print(f"Received audio frame from user {uid}, size: {len(frame.buffer) if frame.buffer else 0}")

class MyVideoFrameObserver(dingrtc_wrapper.RtcEngineVideoFrameObserver):
  def __init__(self, input_stream_ref: Optional[ReferenceType[Any]] = None):
    super().__init__()
    self.input_stream_ref = input_stream_ref

  def OnRemoteVideoFrameEx(self, uid:str, track, frame:dingrtc.RtcEngineVideoFrame):
    if self.input_stream_ref:
      input_stream = self.input_stream_ref()
      if input_stream and hasattr(input_stream, 'enqueue_frame'):
        success = input_stream.enqueue_frame(frame)
        if not success:
          print(f"Failed to write video frame for user {uid}")
          return False
      else:
        print("Input stream not available or doesn't support enqueue_frame")
        return False
    else:
      # Fallback: just print for debugging
      print(f"Received video frame from user {uid}")

    return True

  def OnPreDecodeVideoFrameEx(self, uid:str, track, frame:dingrtc.RtcEngineEncodedVideoFrame):
    # frame: RtcEngineEncodedVideoFrame
    print(f"Python MyVideoFrameObserver OnPreDecodeVideoFrame from user {uid}")

class MyRtmEventListener(dingrtc_wrapper.RtmEventListener):
  """
  获得DingRTC引擎的各种消息通知。详细的文档，见 engine_rtm.h。
  """
  def __init__(self, sessionId:str, rtmClient:dingrtc_wrapper.RtmClient):
    super().__init__()
    self.rtmClient:dingrtc_wrapper.RtmClient = rtmClient
    self.sessionId:str = sessionId
    self.readyToUse:bool = False

  def OnRtmServerStateChanged(self, state, erroCode:int):
    print(f"Python MyRtmEventListener OnRtmServerStateChanged {state} {erroCode}")
    # 当state == 1 (Available)时，服务器可用。
    if state == 1:
      # 重新加入session
      # TODO: 锁保护rtmClient
      rc = self.rtmClient.JoinSession(self.sessionId)
      if rc != 0:
        print(f"Failed to join texting session {self.sessionId}, code {rc}")
      # else, check OnJoinSessionResult
    else:
      # 服务不稳定，中途不可用
      self.readyToUse = False

  def OnJoinSessionResultEx(self, sessionId:str, result:int):
    print(f"Python MyRtmEventListener OnJoinSessionResult {result}")
    # 当result == 0时，加入会话成功。
    if result == 0:
      self.readyToUse = True

  def OnMessageEx(self, sessionId:str, fromUid:str, broadcast:bool, data:bytes):
    message_str = data.decode('utf-8', errors='replace')
    print(f"Python MyRtmEventListener OnMessage {sessionId} {fromUid} {broadcast} {message_str}")

def getAppToken(appid:str, channel:str, userid:str, username:str) -> dict:
  """
  向AppServer请求入会需要的token
  Returns:
    入会需要的完整的参数，包含token，appid，频道名等。
  Remarks:
    客户支持时加入客户的频道，不能通过AppServer来获取token。此时客户会
    提供频道名，token等必要参数。重载 getAppToken() 即可，比如：

    def getAppToken(appid, channel, userid, username):
      # 读取配置文件，获取客户的appid, channel, userid, username, token
      return {'appid':appid, 'channel':channel, 'userid':userid, 'username':username, 'token':token}
  """

  appserver_key = ''
  if 'APPSERVER_KEY' in os.environ:
    appserver_key = os.environ[
            'APPSERVER_KEY']  # load appserver key from environment variable APPSERVER_KEY
  else:
    appserver_key = 'your-appserver-key'  # set appserver key manually
  if appid == 'a4zfr1hn': # 线上使用的appid
    appsrv = 'https://onertc-demo-app-server.dingtalk.com/login?passwd=' + appserver_key + '&env=onertcOnline'
  else: # 其他是预发环境使用的appid
    appsrv = 'https://pre-onertc-demo-app-server.dingtalk.com/login?passwd=' + appserver_key + '&env=onertcPre'
  appsrv += '&appid=' + appid + '&userid=' + userid + '&user=' + username + '&room=' + channel + '&tokensid=false'

  r = requests.get(appsrv)
  if r.status_code != 200:
    print(f'failed to access appserver, code {r.status_code}')
    return {}
  data = r.json()
  token = data['data']['token']
  return {'appid':appid, 'channel':channel, 'userid':userid, 'username':username, 'token':token, 'gslbServer':''}

def createEngine() -> dingrtc_wrapper.RtcEngine:
    engine:dingrtc_wrapper.RtcEngine = dingrtc_wrapper.RtcEngine()
    engine.Create("")
    return engine

def destroyEngine(engine:dingrtc_wrapper.RtcEngine):
    if engine:
        engine.Destroy()

async def joinChannel(engine:dingrtc_wrapper.RtcEngine, appToken:dict) -> bool:
  authInfo:dingrtc.RtcEngineAuthInfo = dingrtc.RtcEngineAuthInfo(
    appId      = appToken['appid'],
    gslbServer = appToken['gslbServer'],
    channelId  = appToken['channel'],
    userId     = appToken['userid'],
    token      = appToken['token']
  )

  # Create event for join notification
  join_event = asyncio.Event()
  listener = MyRtcEventListener(join_event)
  engine.SetEngineEventListener(listener)

  quit = False

  rc = engine.JoinChannel(authInfo, appToken['username'])
  if rc != 0:
    print("Failed to call JoinChannel failed", rc)
    quit = True
  else:
    try:
      # Wait for either join completion or exit signal with timeout
      done, pending = await asyncio.wait(
          [join_event.wait()],
          timeout=10.0,
          return_when=asyncio.FIRST_COMPLETED
      )

      # Cancel any pending tasks
      for task in pending:
        task.cancel()
        try:
          await task
        except asyncio.CancelledError:
          pass

      # Check which event completed
      if join_event.is_set():
        print("Join channel completed successfully")
      else:
        print("Timeout: Failed to join channel within 10 seconds")
        quit = True

    except asyncio.TimeoutError:
      print("Timeout: Failed to join channel within 10 seconds")
      quit = True
    except asyncio.CancelledError:
      print("Operation cancelled while waiting to join channel")
      quit = True
    except Exception as e:
      print(f"Unexpected error while waiting to join channel: {e}")
      quit = True

  if quit:
    engine.SetEngineEventListener(None)
    destroyEngine(engine)
    return False
  else:
    return True

def subscribeAudio(engine:dingrtc_wrapper.RtcEngine, sampleRate:int, channels:int, input_stream_ref=None):
  audioSink = MyAudioFrameObserver(input_stream_ref)
  engine.SubscribeAllRemoteAudioStreams(True)
  engine.RegisterAudioFrameObserver(audioSink)
  # 令SDK回调时自动将audio frame按照指定的采样率和声道数进行转换
  config = dingrtc.RtcEngineAudioFrameObserverConfig()
  if sampleRate == 48000:
      config.sampleRate = dingrtc.RtcEngineAudioSampleRate_48000
  elif sampleRate == 44100:
      config.sampleRate = dingrtc.RtcEngineAudioSampleRate_44100
  elif sampleRate == 32000:
      config.sampleRate = dingrtc.RtcEngineAudioSampleRate_32000
  elif sampleRate == 16000:
      config.sampleRate = dingrtc.RtcEngineAudioSampleRate_16000
  else:
      config.sampleRate = dingrtc.RtcEngineAudioSampleRate_8000
  if dingrtc.RtcEngineMonoAudio:
    config.channels = dingrtc.RtcEngineMonoAudio
  else:
    config.channels = dingrtc.RtcEngineStereoAudio
  config.permission = dingrtc.RtcEngineAudioFrameReadOnly
  engine.EnableAudioFrameObserver(True, dingrtc.RtcEngineAudioPositionRemoteUser, config)

def unsubscribeAudio(engine:dingrtc_wrapper.RtcEngine):
  engine.SubscribeAllRemoteAudioStreams(False)
  engine.RegisterAudioFrameObserver(None)
  config = dingrtc.RtcEngineAudioFrameObserverConfig()
  config.sampleRate = dingrtc.RtcEngineAudioSampleRate_48000
  config.channels = dingrtc.RtcEngineMonoAudio
  engine.EnableAudioFrameObserver(False, 0xffffffff, config)

def subscribeAllVideo(engine:dingrtc_wrapper.RtcEngine, input_stream_ref=None):
  videoSink = MyVideoFrameObserver(input_stream_ref)
  engine.SubscribeAllRemoteVideoStreams(True, True)
  engine.RegisterVideoFrameObserver(videoSink)
  engine.EnableVideoFrameObserver(True, dingrtc.RtcEnginePositionPreRender)

def unsubscribeAllVideo(engine:dingrtc_wrapper.RtcEngine):
  engine.SubscribeAllRemoteVideoStreams(False, False)
  engine.RegisterVideoFrameObserver(None)
  engine.EnableVideoFrameObserver(False, 0xffffffff)

def publishAudio(engine:dingrtc_wrapper.RtcEngine, sampleRate:int, channels:int):
  engine.SetExternalAudioSource(True, sampleRate, channels)
  engine.PublishLocalAudioStream(True)

def unpublishAudio(engine:dingrtc_wrapper.RtcEngine):
  engine.SetExternalAudioSource(False, 0, 0)
  engine.PublishLocalAudioStream(False)

def publishVideo(engine:dingrtc_wrapper.RtcEngine):
  engine.SetExternalVideoSource(True, dingrtc.RtcEngineVideoTrackCamera)
  engine.PublishLocalVideoStream(True)

def unpublishVideo(engine:dingrtc_wrapper.RtcEngine):
  engine.PublishLocalVideoStream(False)

class FakePyAudio:
    """
    PyAudio的替换类，使用DingRTC SDK实现音频输入和输出。使用该类，需要设置
    环境变量APPID, CHANNEL, USERID, USERNAME，用于DingRTC SDK加入频道的
    前置条件。
    """

    paInt16 = 0x08

    def __init__(self):
        # instance variables
        self.appid:str = os.environ.get('APPID', '')
        self.channel:str = os.environ.get('CHANNEL', '')
        self.uid:str = os.environ.get('USERID', '')
        self.userName:str = os.environ.get('USERNAME', '')
        self.engine:dingrtc_wrapper.RtcEngine = None
        # 虚拟麦克风
        self.input_stream: Optional[ReferenceType[Any]] = None # weakref.ReferenceType[FakeInputStream]
        # 虚拟扬声器
        self.output_stream: Optional[ReferenceType[Any]] = None # weakref.ReferenceType[FakeOutputStream]
        # 虚拟摄像头
        self.input_video_stream: Optional[ReferenceType[Any]] = None # weakref.ReferenceType[FakeCameraStream]

        # set log path & level ahead of engine creation
        dingrtc_wrapper.RtcEngine.SetLogLevel(dingrtc.RtcEngineLogLevelInfo)
        curdir = os.path.dirname(os.path.abspath(__file__))
        print("Current directory: ", curdir)
        dingrtc_wrapper.RtcEngine.SetLogDirPath(curdir + '/log')

    def __del__(self):
        self.terminate()

    def open(self, format=None, channels=None, rate=None, input=False, output=False, stream_callback=None):
        if not self.engine:
            # 创建 RtcEngine并JoinChannel
            self.engine = createEngine()
            appToken:dict = getAppToken(self.appid, self.channel, self.uid, self.userName)
            if not appToken:
                raise ValueError("Failed to get app token")
            # Run the async joinChannel in a synchronous context
            success = asyncio.run(joinChannel(self.engine, appToken))
            if not success:
                raise ValueError("Failed to join channel")
        if input:
            if self.input_stream and self.input_stream():
                raise ValueError("Input stream already exists")
            stream = FakeInputStream()
            stream.format = format
            stream.rate = rate
            stream.channels = channels
            stream.engine = weakref.ref(self.engine )
            self.input_stream = weakref.ref(stream)
            return stream
        elif output:
            if self.output_stream and self.output_stream():
                raise ValueError("Output stream already exists")
            stream = FakeOutputStream()
            stream.format = format
            stream.rate = rate
            stream.channels = channels
            stream.engine = weakref.ref(self.engine )
            self.output_stream = weakref.ref(stream)
            return stream
        else:
            raise ValueError("Either input or output must be True")

    def open_camera(self):
      if not self.engine:
        # 创建 RtcEngine并JoinChannel
        self.engine = createEngine()
        appToken:dict = getAppToken(self.appid, self.channel, self.uid, self.userName)
        if not appToken:
          raise ValueError("Failed to get app token")
        # Run the async joinChannel in a synchronous context
        success = asyncio.run(joinChannel(self.engine, appToken))
        if not success:
          raise ValueError("Failed to join channel")

      if self.input_video_stream and self.input_video_stream():
        raise ValueError("Output stream already exists")
      stream = FakeCameraStream()
      stream.engine = weakref.ref(self.engine )
      self.input_video_stream = weakref.ref(stream)
      return stream

    # 若app不调用 terminate()，则通过input & output stream来决定是否隐含的销毁 RtcEngine。
    def terminate(self):
        if self.input_stream is not None:
            stream = self.input_stream()
            if stream:
                stream.close()

        if self.output_stream is not None:
            stream = self.output_stream()
            if stream:
                stream.close()

        if self.input_video_stream is not None:
            stream = self.input_video_stream()
            if stream:
                stream.close()

        if self.engine:
            self.engine.LeaveChannel()
            destroyEngine(self.engine)
            self.engine = None

class FakeInputStream:
    """
    AI agent的麦克风流模拟，数据源：RtcEngine的音频PCM输出。
    """

    def __init__(self):
        self.is_active = False
        # format: pyaudio.paInt16 不支持float
        self.format = None
        self.rate = 48000
        self.channels = 1
        self.engine: Optional[ReferenceType[dingrtc_wrapper.RtcEngine]] = None
        self.audio_queue = queue.Queue(maxsize=20) # guarded by self.data_lock
        self.data_lock = threading.Lock()
        self.start_read_time = 0
        self.total_frames = 0

    def __del__(self):
        self.close()

    def enqueue_audio_data(self, frame: dingrtc.RtcEngineAudioFrame) -> bool:
        """
        数据源会push音频PCM数据。若队列满，则丢弃旧数据。
        """

        # Validate input
        if not frame or not frame.buffer:
            print(f"Invalid frame or buffer")
            return False

        audio_data = None

        try:
            # Convert buffer to bytes if needed
            if isinstance(frame.buffer, bytearray):
                audio_data = bytes(frame.buffer)
            elif isinstance(frame.buffer, bytes):
                audio_data = frame.buffer
            else:
                # Handle other buffer types if necessary
                audio_data = bytes(frame.buffer)
        except Exception as e:
            print(f"Error convert audio data: {e}")
            return False

        # 加入队列，若队列已满，先移除旧数据
        with self.data_lock:
            try:
                if self.audio_queue.full():
                    try:
                        self.audio_queue.get_nowait()  # Remove oldest item
                        self.audio_queue.task_done()
                    except queue.Empty:
                        pass
                # Add the new data
                self.audio_queue.put_nowait(audio_data)
                return True
            except queue.Full:
                print("Failed to add audio data to queue even after clearing space")
                return False
            except Exception as e:
                print(f"Unexpected error enqueueing audio data: {e}")
                return False

    def start_stream(self):
        # 订阅音频
        # 开始监听音频数据
        engine: Optional[dingrtc_wrapper.RtcEngine] = None
        if self.engine:
            engine = self.engine()  # Get the actual engine object from weakref
        if not engine:
            return
        subscribeAudio(engine, self.rate, self.channels, weakref.ref(self))
        self.is_active = True

    def read(self, frames: int, exception_on_overflow: bool = False):
        if not self.is_active:
            self.start_stream()

        # read data from queue。若queue发生underrun，则使用0来填充。发生
        # underrun，生产者产生的数据会累积在 audio_queue，后续消费者消费速度
        # 就不容易产生新的underrun。
        # 控制帧率，调用者没有帧率控制逻辑。
        # how long since the first read? how many frames have been read?
        now = time.time()
        if self.start_read_time == 0:
            self.start_read_time = now
        duration = self.total_frames / self.rate
        elapsed = now - self.start_read_time
        sleep_time = duration - elapsed
        if sleep_time > 0:
            time.sleep(sleep_time)
        audio_data = bytearray()
        bytes_per_frame = frames * self.channels * 2
        self.total_frames += frames

        with self.data_lock:
            try:
                while len(audio_data) < bytes_per_frame and not self.audio_queue.empty():
                  try:
                    chunk = self.audio_queue.get_nowait()
                    audio_data.extend(chunk)
                    self.audio_queue.task_done()
                  except queue.Empty:
                    break
            except Exception as e:
                print(f"Error reading from audio queue: {e}")
                return b'\x00' * (bytes_per_frame)  # Return silence on error

        # Return exactly the requested amount of data
        result = bytes(audio_data[:bytes_per_frame])

        # Pad with zeros if we don't have enough data
        if len(result) < bytes_per_frame:
            result += b'\x00' * (bytes_per_frame - len(result))

        return result

    def stop_stream(self):
        # 取消订阅
        engine: Optional[dingrtc_wrapper.RtcEngine] = None
        if self.engine:
            engine = self.engine()  # Get the actual engine object from weakref
        if engine:
            unsubscribeAudio(engine)
        self.is_active = False

    def close(self):
        self.stop_stream()
        self.engine = None

    def is_active(self):
        return self.is_active

class FakeOutputStream:
    """
    A fake output stream that writes audio data to a file.
    """

    def __init__(self):
        self.is_active = False
        self.buffer = bytearray()
        self.format = None
        self.rate = 48000
        self.channels = 1
        self.engine: Optional[ReferenceType[dingrtc_wrapper.RtcEngine]] = None
        self.start_write_time = 0
        self.total_frames = 0

    def __del__(self):
        self.close()

    def start_stream(self):
        # 推音频
        engine: Optional[dingrtc_wrapper.RtcEngine] = None
        if self.engine:
            engine = self.engine()  # Get the actual engine object from weakref
        if not engine:
            return
        publishAudio(engine, self.rate, self.channels)
        self.is_active = True

    def write(self, data: bytes):
        if not self.is_active:
            self.start_stream()

        # 帧率控制
        now = time.time()
        if self.start_write_time == 0:
            self.start_write_time = now
        duration = self.total_frames / self.rate
        elapsed = now - self.start_write_time
        sleep_time = duration - elapsed
        if sleep_time > 0:
            time.sleep(sleep_time)
        frames = len(data) / (self.channels * 2)
        self.total_frames += frames

        self._send_audio_frame(data)

    def stop_stream(self):
        engine: Optional[dingrtc_wrapper.RtcEngine] = None
        if self.engine:
            engine = self.engine()  # Get the actual engine object from weakref
        if engine:
            unpublishAudio(engine)
        self.is_active = False

    def close(self):
        self.stop_stream()
        self.engine = None

    def is_active(self):
        return self.is_active

    def _send_audio_frame(self, pcmData):
        engine: Optional[dingrtc_wrapper.RtcEngine] = None
        if self.engine:
            engine = self.engine()  # Get the actual engine object from weakref
        if not engine:
            return False

        # Assert pcmData is a byte buffer
        assert isinstance(pcmData, (bytes, bytearray)), f"pcmData must be bytes or bytearray, got {type(pcmData).__name__}"

        frame_data = dingrtc.RtcEngineAudioFrame()
        frame_data.type=dingrtc.RtcEngineAudioFramePcm16
        frame_data.bytesPerSample=2
        frame_data.samplesPerSec= self.rate
        frame_data.channels=self.channels
        frame_data.samples=len(pcmData) // (self.channels * (16 // 8))
        frame_data.buffer=pcmData
        frame_data.timestamp=0
        engine.PushExternalAudioFrame(frame_data)
        return True  # Continue processing

class FakeCameraStream:
  """
  AI agent的摄像头流模拟，数据源：RtcEngine的远端视频解码后输出。
  """
  def __init__(self):
    self.is_active = False
    self.engine: Optional[ReferenceType[dingrtc_wrapper.RtcEngine]] = None
    self.frame_queue = queue.Queue(maxsize=3) # guarded by self.data_lock
    self.data_lock = threading.Lock()
    self.start_read_time = 0
    self.total_frames = 0

  def __del__(self):
    self.close()

  def enqueue_frame(self, frame: dingrtc.RtcEngineVideoFrame) -> bool:
    # Validate input
    if not frame or not frame.data:
      print(f"Invalid frame or buffer")
      return False

    # 加入队列，若队列已满，先移除旧数据
    with self.data_lock:
      try:
        if self.frame_queue.full():
          try:
            self.frame_queue.get_nowait()  # Remove oldest item
            self.frame_queue.task_done()
          except queue.Empty:
            pass
        # Add the new data (access frame.data to turn raw buffer into bytes)
        f = (frame.data, frame.width, frame.height)
        self.frame_queue.put_nowait(f)
        return True
      except queue.Full:
        print("Failed to add video frame to queue even after clearing space")
        return False
      except Exception as e:
        print(f"Unexpected error enqueueing video frame: {e}")
        return False

  def start_stream(self):
    # 订阅所有视频 （增加接口：指定订阅某路视频）
    # 开始监听视频数据
    engine: Optional[dingrtc_wrapper.RtcEngine] = None
    if self.engine:
      engine = self.engine()  # Get the actual engine object from weakref
    if not engine:
      return
    subscribeAllVideo(engine, weakref.ref(self))
    self.is_active = True

  def read_frame(self) -> (bytes, int, int):
    '''
    返回一帧视频数据，若队列中无数据，则立即返回None.
    @return (bytes, width, height): 视频数据，宽高。
    '''
    if not self.is_active:
      self.start_stream()

    with self.data_lock:
      frame = None
      try:
        frame = self.frame_queue.get_nowait()
        self.frame_queue.task_done()
      except queue.Empty:
        return (None, 0, 0)

      if not frame:
        return (None, 0, 0)
      else:
        return frame

  def stop_stream(self):
    # 取消订阅
    engine: Optional[dingrtc_wrapper.RtcEngine] = None
    if self.engine:
      engine = self.engine()  # Get the actual engine object from weakref
    if engine:
      unsubscribeAllVideo(engine)
    self.is_active = False

  def close(self):
    self.stop_stream()
    self.engine = None

  def is_active(self):
    return self.is_active

