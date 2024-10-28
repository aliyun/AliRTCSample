import base64
import hashlib
import hmac
import struct
import time


def generate_sign(key: str, timestamp: int, salt: int) -> bytes:
    buf_ts = struct.pack('>I', timestamp)
    sign_key = hmac.new(buf_ts, key.encode('utf-8'), hashlib.sha256).digest()

    buf_salt = struct.pack('>I', salt)
    return hmac.new(buf_salt, sign_key, hashlib.sha256).digest()


def sign(key: bytes, data: bytes) -> bytes:
    return hmac.new(key, data, hashlib.sha256).digest()


if __name__ == '__main__':
    appKey = 'appKey'
    timestamp = int(time.time())
    salt = int(time.time())

    signKey = generate_sign(appKey, timestamp, salt)
    print(base64.b64encode(signKey))
