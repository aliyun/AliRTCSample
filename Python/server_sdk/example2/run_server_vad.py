import os
import cv2
import numpy as np
import base64
import signal
import sys
import time
#import pyaudio
# Add parent directory to path
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from fake_audio import FakePyAudio as PyAudio
import dashscope
from dashscope.audio.qwen_omni import *

from B64PCMPlayer import B64PCMPlayer

voice = 'Chelsie'

pya = None
mic_stream = None
camera_stream = None
b64_player = None
conversation = None

DO_VIDEO_TEST = True

#if DO_VIDEO_TEST:
#    fake_video = None
#    with open('data/cat_480p.jpg', 'rb') as image_file:
#        fake_video = base64.b64encode(image_file.read()).decode('ascii')

class YUVProcessor:
    def __init__(self):
        pass

    def yuv_frame_to_base64(self, yuv_data:bytes, width:int, height:int, yuv_format='I420', saveToFile=None):
        """
        Convert YUV frame data directly to base64 JPEG for API transmission

        Args:
            yuv_data: Raw YUV frame data
            width: Frame width
            height: Frame height
            yuv_format: YUV format type

        Returns:
            str: Base64 encoded JPEG image
        """
        try:
            # Convert YUV to numpy array
            yuv_array = np.frombuffer(yuv_data, dtype=np.uint8)

            # Handle different YUV formats
            if yuv_format == 'I420':
                yuv_image = yuv_array.reshape((height * 3 // 2, width))
                bgr_image = cv2.cvtColor(yuv_image, cv2.COLOR_YUV2BGR_I420)
            elif yuv_format == 'NV12':
                yuv_image = yuv_array.reshape((height * 3 // 2, width))
                bgr_image = cv2.cvtColor(yuv_image, cv2.COLOR_YUV2BGR_NV12)
            elif yuv_format == 'NV21':
                yuv_image = yuv_array.reshape((height * 3 // 2, width))
                bgr_image = cv2.cvtColor(yuv_image, cv2.COLOR_YUV2BGR_NV21)
            else:
                yuv_image = yuv_array.reshape((height * 3 // 2, width))
                bgr_image = cv2.cvtColor(yuv_image, cv2.COLOR_YUV2BGR_I420)

            # Encode to JPEG
            success, jpeg_buffer = cv2.imencode('.jpg', bgr_image, 
                                              [int(cv2.IMWRITE_JPEG_QUALITY), 85])

            if success:
                # Convert to numpy array and save
                if isinstance(saveToFile, str) and saveToFile != '':
                    try:
                        cv2.imwrite(saveToFile, bgr_image)
                    except:
                        pass

                # Convert to base64 string
                jpeg_base64 = base64.b64encode(jpeg_buffer).decode('ascii')
                return jpeg_base64
            else:
                raise Exception("Failed to encode image")

        except Exception as e:
            print(f"Error converting YUV to JPEG: {e}")
            return None

def init_dashscope_api_key():
    """
        Set your DashScope API-key. More information:
        https://github.com/aliyun/alibabacloud-bailian-speech-demo/blob/master/PREREQUISITES.md
    """

    if 'DASHSCOPE_API_KEY' in os.environ:
        dashscope.api_key = os.environ[
            'DASHSCOPE_API_KEY']  # load API-key from environment variable DASHSCOPE_API_KEY
    else:
        dashscope.api_key = '<your-dashscope-api-key>'  # set API-key manually


class MyCallback(OmniRealtimeCallback):
    def on_open(self) -> None:
        global pya
        global mic_stream
        global camera_stream
        global b64_player
        print('connection opened, init microphone')
        pya = PyAudio() # pyaudio.PyAudio()
        mic_stream = pya.open(format=PyAudio.paInt16,
                        channels=1,
                        rate=16000,
                        input=True)
        b64_player = B64PCMPlayer(pya)
        camera_stream = pya.open_camera()

    def on_close(self, close_status_code, close_msg) -> None:
        print('connection closed with code: {}, msg: {}, destroy microphone'.format(close_status_code, close_msg))
        sys.exit(0)

    def on_event(self, response: str) -> None:
        try:
            global conversation
            global b64_player
            type = response['type']
            if 'session.created' == type:
                print('start session: {}'.format(response['session']['id']))
            elif 'conversation.item.input_audio_transcription.completed' == type:
                print('question: {}'.format(response['transcript']))
            elif 'response.audio_transcript.delta' == type:
                text = response['delta']
                print("got llm response delta: {}".format(text))
            elif 'response.audio.delta' == type:
                recv_audio_b64 = response['delta']
                b64_player.add_data(recv_audio_b64)
            elif 'input_audio_buffer.speech_started' == type:
                print('======VAD Speech Start======')
                b64_player.cancel_playing()
                print('cancel playing done')
            elif 'response.done' == type:
                print('======RESPONSE DONE======')
                print('[Metric] response: {}, first text delay: {}, first audio delay: {}'.format(
                                conversation.get_last_response_id(), 
                                conversation.get_last_first_text_delay(), 
                                conversation.get_last_first_audio_delay(),
                                ))
        except Exception as e:
            print('[Error] {}'.format(e))
            return


if __name__  == '__main__':
    init_dashscope_api_key()

    print('Initializing ...')

    #record_pcm_file = open('data/record_16khz.pcm', 'wb')

    callback = MyCallback()

    conversation = OmniRealtimeConversation(
        model='qwen-omni-turbo-realtime-latest',
        callback=callback, 
        )

    conversation.connect()

    conversation.update_session(
        output_modalities=[MultiModality.AUDIO, MultiModality.TEXT],
        voice=voice,
        input_audio_format=AudioFormat.PCM_16000HZ_MONO_16BIT,
        output_audio_format=AudioFormat.PCM_24000HZ_MONO_16BIT,
        enable_input_audio_transcription=True,
        input_audio_transcription_model='gummy-realtime-v1',
        enable_turn_detection=True,
        turn_detection_type='server_vad',
    )

    yuv_processor = YUVProcessor()

    # After connecting and updating session, send an initial message
    # conversation.append_text("Hello", role="user")
    # 没有这个方法

    def signal_handler(sig, frame):
        print('Ctrl+C pressed, stop recognition ...')
        # Stop recognition
        conversation.close()
        if b64_player:
            b64_player.shutdown()
        # 显式的销毁PyAudio对象，结束DingRTC
        if pya:
            print('destroy pyaudio')
            pya.terminate()

        print('omni realtime stopped.')
        # Forcefully exit the program
        sys.exit(0)

    signal.signal(signal.SIGINT, signal_handler)
    print("Press 'Ctrl+C' to stop conversation...")

    #模拟多轮交互，在server_vad模式下，服务会自动处理打断，用户可以持续发送静音
    last_photo_time = time.time()*1000
    picNum = 0
    while True:
        if mic_stream and conversation:
            audio_data = mic_stream.read(3200, exception_on_overflow=False)
            #record_pcm_file.write(audio_data)
            audio_b64 = base64.b64encode(audio_data).decode('ascii')
            try:
                conversation.append_audio(audio_b64)
            except Exception as e:
                print('[Error] {}'.format(e))
                break
            if DO_VIDEO_TEST and time.time()*1000 - last_photo_time > 500:
                # 每500ms发送一张图片
                # 截取一帧yuv数据
                result = camera_stream.read_frame()
                if result is not None and len(result) == 3 and result[0] is not None:
                    yuv_frame_data, width, height = result
                    fileName = None
                    # if picNum % 10 == 0:
                    #     fileName = 'pic{}.jpg'.format(picNum)
                    picNum += 1
                    jpeg_base64 = yuv_processor.yuv_frame_to_base64(yuv_frame_data, width, height, 'I420', fileName)
                    if jpeg_base64:
                        conversation.append_video(jpeg_base64)
                last_photo_time = time.time()*1000
        else:
            break

    # 体面的退出
    if b64_player:
        b64_player.shutdown()
    # 显式的销毁PyAudio对象，结束DingRTC
    if pya:
        print('destroy pyaudio')
        pya.terminate()

    print('omni realtime stopped.')

