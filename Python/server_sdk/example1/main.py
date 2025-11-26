import os
import sys
# Add parent directory to path
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import dingrtc
import dingrtc_wrapper
import asyncio
import requests
from audio_pcm_generator import PcmFileReader
from audio_wav_recorder import MultiUserAudioWavRecorder
from video_yuv_generator import YuvFrameReader
from text_generator import call_every
import signal

def shut_down(exit_event):
  exit_event.set()

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
  def __init__(self, recorder:MultiUserAudioWavRecorder):
    super().__init__()
    self.recorder:MultiUserAudioWavRecorder = recorder

  def OnRemoteUserAudioFrame(self, uid:str, frame:dingrtc.RtcEngineAudioFrame):
    if self.recorder:
      # frame.buffer should contain the PCM data
      success = self.recorder.write_audio_frame_for_user(uid, frame)
      if not success:
          print(f"Failed to write audio frame for user {uid}")

class MyVideoFrameObserver(dingrtc_wrapper.RtcEngineVideoFrameObserver):
  def __init__(self, recorder):
    super().__init__()
    self.recorder = recorder

  def OnRemoteVideoFrameEx(self, uid:str, track, frame:dingrtc.RtcEngineVideoFrame):
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

def destroyEngine(engine:dingrtc_wrapper.RtcEngine):
  if engine:
    engine.Destroy()

async def main(exit_event:asyncio.Event):
  result = getAppToken('a4zfr1hn', 'room124', 'pytutan', 'pyuser')
  if not result:
    return

  # set log path & level ahead of engine creation
  dingrtc_wrapper.RtcEngine.SetLogLevel(dingrtc.RtcEngineLogLevelInfo)
  curdir = os.path.dirname(os.path.abspath(__file__))
  print("Current directory: ", curdir)
  dingrtc_wrapper.RtcEngine.SetLogDirPath(curdir + '/log')

  #################################################
  # create rtc engine
  #################################################

  quit:bool = False

  engine:dingrtc_wrapper.RtcEngine = dingrtc_wrapper.RtcEngine()
  if not engine.Create(""):
    print("Failed to create RtcEngine")
    return
  # Create event for join notification
  join_event = asyncio.Event()
  listener = MyRtcEventListener(join_event)
  engine.SetEngineEventListener(listener)

  #################################################
  # join channel
  #################################################
  authInfo = dingrtc.RtcEngineAuthInfo(
    appId=result['appid'],
    gslbServer=result['gslbServer'],
    channelId=result['channel'],
    userId=result['userid'],
    token=result['token']
  )
  rc = engine.JoinChannel(authInfo, result['username'])
  if rc != 0:
    print("Failed to call JoinChannel failed", rc)
    quit = True
  else:
    try:
      # Wait for either join completion or exit signal with timeout
      done, pending = await asyncio.wait(
          [join_event.wait(), exit_event.wait()],
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
      if exit_event.is_set():
        print("Exit signal received while joining channel")
        quit = True
      elif join_event.is_set():
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
    listener = None
    destroyEngine(engine)
    join_event = None
    return
  #################################################
  # publish
  #################################################

  # 1. publish audio stream
  file_path = "./music_stereo_48kHz.pcm"
  sampleRate = 48000
  channels = 2
  engine.SetExternalAudioSource(True, sampleRate, channels)
  engine.PublishLocalAudioStream(True)

  # Create and start PCM reader for audio publishing
  def send_audio_frame(pcmData):
    # Assert pcmData is a byte buffer
    assert isinstance(pcmData, (bytes, bytearray)), f"pcmData must be bytes or bytearray, got {type(pcmData).__name__}"

    frame_data = dingrtc.RtcEngineAudioFrame()
    frame_data.type=dingrtc.RtcEngineAudioFramePcm16
    frame_data.bytesPerSample=2
    frame_data.samplesPerSec=sampleRate
    frame_data.channels=channels
    frame_data.samples=len(pcmData) // (channels * (16 // 8))
    frame_data.buffer=pcmData
    frame_data.timestamp=0
    engine.PushExternalAudioFrame(frame_data)
    return True  # Continue processing

  pcm_reader = PcmFileReader(
    pcm_file_path=file_path,
    sample_rate=sampleRate,
    bits_per_sample=16,
    channels=channels,
    process_function=send_audio_frame
  )

  # Start audio streaming
  pcm_reader.Start(rewind=True)

  # 2. publish video stream
  video_file_path = "./parisqcif-000.yuv"
  video_width = 176
  video_height = 144
  video_frame_rate = 15.0
  engine.SetExternalVideoSource(True, dingrtc.RtcEngineVideoTrackCamera)
  engine.PublishLocalVideoStream(True)

  # Create and start YUV reader for video publishing
  def send_video_frame(yuvData):
      # Create video frame and send to engine
      video_frame = dingrtc.RtcEngineVideoFrame()
      video_frame.frameType = dingrtc.RtcEngineVideoFrameRaw
      video_frame.pixelFormat = dingrtc.RtcEngineVideoI420
      video_frame.timestamp = 0
      video_frame.width = video_width
      video_frame.height = video_height
      video_frame.rotation = 0
      video_frame.mirror = False
      video_frame.count = 1
      ysize = video_width * video_height
      csize = ysize // 4
      video_frame.set_offset(0, ysize)
      video_frame.set_offset(1, csize)
      video_frame.set_offset(2, csize)
      video_frame.set_stride(0, video_width)
      video_frame.set_stride(1, video_width // 2)
      video_frame.set_stride(2, video_width // 2)
      video_frame.data = yuvData
      video_frame.dataLength = 0
      video_frame.textureId = 0
      engine.PushExternalVideoFrame(video_frame, dingrtc.RtcEngineVideoTrackCamera)
      return True  # Continue processing

  yuv_reader = YuvFrameReader(
      yuv_file_path=video_file_path,
      width=video_width,
      height=video_height,
      frame_rate=video_frame_rate,
      process_function=send_video_frame
  )

  # Start video streaming
  yuv_reader.Start(rewind=True)
  #################################################
  # subscribe
  #################################################

  # Subscribe audio stream

  recorder = None #MultiUserAudioWavRecorder()
  audioSink = MyAudioFrameObserver(recorder)
  engine.SubscribeAllRemoteAudioStreams(True)
  engine.RegisterAudioFrameObserver(audioSink)
  # 令SDK回调时自动将audio frame按照指定的采样率和声道数进行转换
  config = dingrtc.RtcEngineAudioFrameObserverConfig()
  config.sampleRate = dingrtc.RtcEngineAudioSampleRate_48000
  config.channels = dingrtc.RtcEngineMonoAudio
  config.permission = dingrtc.RtcEngineAudioFrameReadOnly
  engine.EnableAudioFrameObserver(True, dingrtc.RtcEngineAudioPositionRemoteUser, config)

  # Subscribe video stream
  videoSink = MyVideoFrameObserver(None)
  engine.SubscribeAllRemoteVideoStreams(True, True)
  engine.RegisterVideoFrameObserver(videoSink)
  engine.EnableVideoFrameObserver(True, dingrtc.RtcEnginePositionPreRender)

  #################################################
  # texting
  # 可以在JoinChannel之前设置好
  #################################################
  textSession:str = "AI agent texting"
  rtmClient = engine.GetRtmClient()
  textListener = MyRtmEventListener(textSession, rtmClient)
  rtmClient.SetListener(textListener)

  # 消息发生器
  def sendText():
    if not textListener.readyToUse:
      return
    text:bytes = b"Hello, I'm an AI agent. How can I help you?"
    rtmClient.BroadcastData(textSession, text)
    #rtmClient.SendData(textSession, remoteUser, data)
  timer = call_every(3.0, sendText)

  #################################################
  # Let it run for 30 seconds
  #################################################

  # 等待过程中若用户ctrl-c则立即清理并推出频道
  try:
    await asyncio.wait_for(exit_event.wait(), timeout=30)
    print("Exit signal received, cleaning up...")
  except asyncio.TimeoutError:
    print("30 seconds timeout reached, cleaning up...")

  #################################################
  # 停止texting
  #################################################
  print("Stopping text streaming...")
  if timer:
    timer.stop()
    timer = None
  if rtmClient:
    rtmClient.LeaveSession(textSession)
    rtmClient.SetListener(None) # 破除循环引用
    rtmClient = None
  if textListener:
    textListener.rtmClient = None
    textListener = None

  #################################################
  # Stop 音视频 streaming (publish & subscribe)
  #################################################
  print("Stopping audio streaming...")
  if recorder:
    recorder.stop_all_recordings()
  if pcm_reader:
    pcm_reader.Stop()
    pcm_reader = None
  engine.PublishLocalAudioStream(False)
  engine.SubscribeAllRemoteAudioStreams(False)
  engine.RegisterAudioFrameObserver(None)
  engine.EnableAudioFrameObserver(False, 0xffffffff, config)

  print("Stopping video streaming...")
  if yuv_reader:
    yuv_reader.Stop()
    yuv_reader = None
  engine.PublishLocalVideoStream(False)
  engine.SubscribeAllRemoteVideoStreams(False, False)
  engine.RegisterVideoFrameObserver(None)
  engine.EnableVideoFrameObserver(False, 0xffffffff)

  #################################################
  # quit
  #################################################
  print("destroy engine...")
  destroyEngine(engine)
  engine = None
  print("Test done")

if __name__ == "__main__":
  # 打印版本号，检查DingRTC动态库是否可访问
  print('DingRTC version ', dingrtc_wrapper.RtcEngine.GetSDKVersion())
  exit_event = asyncio.Event() # 用来捕获用户输入的Ctrl-C，若检测到，则结束demo
  loop = asyncio.get_event_loop()
  loop.add_signal_handler(signal.SIGINT, shut_down, exit_event)
  loop.run_until_complete(main(exit_event))
  # 结束循环
  loop.close()
  exit_event = None
  print('Demo quit')

