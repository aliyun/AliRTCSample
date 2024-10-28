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
    def __init__(self, app_id: str, app_key: str, timestamp: int, issue_timestamp: int = None, salt: int = None,
                 service: Service = None,
                 options: AppTokenOptions = None, signature: bytes = None):
        self.app_id = app_id
        self.app_key = app_key
        self.timestamp = timestamp
        self.issue_timestamp = int(time.time()) if issue_timestamp is None else issue_timestamp
        self.salt = random.randint(1, self.issue_timestamp) if salt is None else salt
        self.service = service
        self.options = options
        self.signature = signature

    def set_service(self, service: Service):
        self.service = service

    def set_options(self, options: AppTokenOptions):
        self.options = options

    def build(self) -> str:
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
    def parse(cls, token: str):
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

    def validate(self, app_key: str) -> bool:
        if app_key is None:
            raise ValueError('missing appKey')

        signKey = util.sign_utils.generate_sign(app_key, self.issue_timestamp, self.salt)

        buf = io.BytesIO()

        app_id_bytes = self.app_id.encode('utf-8')
        buf.write(struct.pack('>I', len(app_id_bytes)))
        buf.write(app_id_bytes)
        buf.write(struct.pack('>I', self.issue_timestamp))
        buf.write(struct.pack('>I', self.salt))
        buf.write(struct.pack('>I', self.timestamp))

        buf.write(self.service.pack())

        if self.options is None:
            self.options = AppTokenOptions()
        buf.write(self.options.pack())

        signature = util.sign_utils.sign(signKey, util.bytes_utils.get_fixed_length_bytes(buf.getvalue()))

        return signature == self.signature


if __name__ == "__main__":
    app_id = "appId"
    app_key = "appKey"
    expired_ts = int(time.time()) + 12 * 60 * 60 # expired after 12h

    appToken = AppToken(app_id, app_key, expired_ts)

    channel_id = 'channelId'
    user_id = 'userId'
    service = Service(channel_id, user_id)
    service.add_audio_publish_privilege()
    appToken.set_service(service)

    options = AppTokenOptions()
    options.set_engine_options({'k1': 'v1', 'k2': 'v2'})
    appToken.set_options(options)

    token = appToken.build()

    appToken = AppToken.parse(token)

    print(appToken.validate(app_key))
