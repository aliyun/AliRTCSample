import base64
import io
import struct
import time

import util.sign_utils
import util.bytes_utils
import util.compress_utils
import random

from app_token_options import AppTokenOptions
from service import Service

VERSION_LENGTH = 3
VERSION_0 = "000"


class AppToken:
    def __init__(self, app_id, app_key, timestamp, issue_timestamp=None, salt=None,
                 service=None,
                 options=None, signature=None):
        self.app_id = app_id
        self.app_key = app_key
        self.timestamp = timestamp
        self.issue_timestamp = int(time.time()) if issue_timestamp is None else issue_timestamp
        self.salt = random.randint(1, self.issue_timestamp) if salt is None else salt
        self.service = service
        self.options = options
        self.signature = signature

    def set_service(self, service):
        self.service = service

    def set_options(self, options):
        self.options = options

    def build(self):
        if self.app_key is None:
            raise ValueError('missing secretKey')
        if self.service is None:
            raise ValueError('missing service')

        signKey = util.sign_utils.generate_sign(self.app_key, self.issue_timestamp, self.salt)

        bufBody = io.BytesIO()

        app_id_bytes = self.app_id.encode('utf-8')
        bufBody.write(struct.pack('>I', len(app_id_bytes)))
        bufBody.write(app_id_bytes)
        bufBody.write(struct.pack('>I', self.issue_timestamp))
        bufBody.write(struct.pack('>I', self.salt))
        bufBody.write(struct.pack('>I', self.timestamp))

        bufBody.write(self.service.pack())

        if self.options is None:
            self.options = AppTokenOptions()
        bufBody.write(self.options.pack())

        # FIXME
        bufBodyFixedLength = util.bytes_utils.get_fixed_length_bytes(bufBody.getvalue())
        signature = util.sign_utils.sign(signKey, bufBodyFixedLength)

        buf = io.BytesIO()
        buf.write(struct.pack('>I', len(signature)))
        buf.write(signature)
        buf.write(bufBodyFixedLength)

        bufFixedLength = util.bytes_utils.get_fixed_length_bytes(buf.getvalue())

        return "{}{}".format(VERSION_0, base64.b64encode(util.compress_utils.compress(bufFixedLength)).decode('utf-8'))

    @classmethod
    def parse(cls, token):
        if token is None:
            raise ValueError('empty appToken')

        if not token.startswith(VERSION_0):
            raise ValueError('unsupported version')

        data = util.compress_utils.decompress(base64.b64decode(token[VERSION_LENGTH:]))

        buf = io.BytesIO(data)

        signature_length = struct.unpack('>I', buf.read(4))[0]
        signature = buf.read(signature_length)

        app_id_length = struct.unpack('>I', buf.read(4))[0]
        app_id = buf.read(app_id_length).decode('utf-8')

        issue_timestamp = struct.unpack('>I', buf.read(4))[0]
        salt = struct.unpack('>I', buf.read(4))[0]
        timestamp = struct.unpack('>I', buf.read(4))[0]

        service = Service.unpack(buf)
        options = AppTokenOptions.unpack(buf)

        return AppToken(app_id, None, timestamp, issue_timestamp, salt, service, options, signature)

if __name__ == "__main__":
    # find appId in your RTC console (https://rtc.console.aliyun.com/#/manage/list)
    app_id = "replace_your_appId"
    # find appKey in your RTC console 
    app_key = "replace_your_appKey"
    # Token is valid for a maximum of 24 hours. This example uses 12 hours, adjust according to your needs.
    expired_ts = int(time.time()) + 12 * 60 * 60
    channel_id = 'replace_your_channelId'
    user_id = 'replace_your_userId'

    appToken = AppToken(app_id, app_key, expired_ts)

    # By default, all privileges are allowed. You can control audio/video/screen privileges individually as shown in the example below.
    # Please check(https://help.aliyun.com/document_detail/2689025.html) for more detail privilege informations. 

    # Example0: full privileges as default
    service = Service(channel_id, user_id)
    appToken.set_service(service)
    token = appToken.build()
    print(token)

    # Example1: only allow audio
    # service = Service(channel_id, user_id)
    # service.add_audio_publish_privilege()
    # appToken.set_service(service)
    # token = appToken.build()
    # print(token)

    # Example2: only allow audio and video
    # service = Service(channel_id, user_id)
    # service.add_audio_publish_privilege()
    # service.add_video_publish_privilege()
    # appToken.set_service(service)
    # token = appToken.build()
    # print(token)

    # By default, no more additional options are set. You can set more options individually as shown in the example below.
    # Please check(https://help.aliyun.com/document_detail/2689025.html) for more detail options informations.
    # Example1: add more options
    # service = Service(channel_id, user_id)
    # appToken.set_service(service)
    # engine_options={
    #     "duration_per_channel": "400",
    #     "delay_close_per_channel": "60"
    # }
    # token_options = AppTokenOptions(engine_options)
    # appToken.set_options(token_options)
    # token = appToken.build()
    # print(token)