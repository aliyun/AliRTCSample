import io
import struct


class Service:
    WILDCARD_CHARACTERS = "*"
    ENABLE_PRIVILEGE = 1
    ENABLE_AUDIO_PRIVILEGE = 2
    ENABLE_VIDEO_PRIVILEGE = 4

    def __init__(self, channel_id: str, user_id: str, privilege=None):
        self.channel_id = channel_id
        self.user_id = user_id
        self.privilege = privilege

    @classmethod
    def create_service_only_with_user_id(cls, user_id: str):
        return cls(cls.WILDCARD_CHARACTERS, user_id)

    @classmethod
    def create_service_only_with_channel_id(cls, channel_id: str):
        return cls(channel_id, cls.WILDCARD_CHARACTERS)

    def add_audio_publish_privilege(self):
        if self.privilege is None:
            self.privilege = 0 | self.ENABLE_PRIVILEGE

        self.privilege |= self.ENABLE_AUDIO_PRIVILEGE

    def add_video_publish_privilege(self):
        if self.privilege is None:
            self.privilege = 0 | self.ENABLE_PRIVILEGE

        self.privilege |= self.ENABLE_VIDEO_PRIVILEGE

    def pack(self) -> bytes:
        buf = io.BytesIO()

        channel_id_bytes = self.channel_id.encode('utf-8')
        buf.write(struct.pack('>I', len(channel_id_bytes)))
        buf.write(channel_id_bytes)

        user_id_bytes = self.user_id.encode('utf-8')
        buf.write(struct.pack('>I', len(user_id_bytes)))
        buf.write(user_id_bytes)

        if self.privilege is not None:
            buf.write(struct.pack('>?I', True, self.privilege))
        else:
            buf.write(struct.pack('>?', False))
        return buf.getvalue()

    @staticmethod
    def unpack(buf: io.BytesIO):

        channel_id_length = struct.unpack('>I', buf.read(4))[0]
        channel_id = buf.read(channel_id_length).decode('utf-8')

        user_id_length = struct.unpack('>I', buf.read(4))[0]
        user_id = buf.read(user_id_length).decode('utf-8')

        has_privilege = struct.unpack('>?', buf.read(1))[0]
        privilege = struct.unpack('>I', buf.read(4))[0] if has_privilege else None
        return Service(channel_id, user_id, privilege)


if __name__ == '__main__':
    channel_id = 'channel_id'
    user_id = 'user_id'
    service = Service(channel_id, user_id)

    servicePack = service.pack()
    serviceUnpack = Service.unpack(io.BytesIO(servicePack))

    assert serviceUnpack.privilege == service.privilege
    assert serviceUnpack.channel_id == service.channel_id
    assert serviceUnpack.user_id == service.user_id

    service.add_video_publish_privilege()
    servicePack = service.pack()
    serviceUnpack = Service.unpack(io.BytesIO(servicePack))

    assert serviceUnpack.privilege == service.privilege
    assert serviceUnpack.channel_id == service.channel_id
    assert serviceUnpack.user_id == service.user_id
