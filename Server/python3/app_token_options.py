import io
import struct


class AppTokenOptions:

    def __init__(self, engine_options={}):
        self.engine_options = engine_options

    def set_engine_options(self, engine_options: dict):
        self.engine_options = engine_options if engine_options is not None else {}

    def pack(self) -> bytes:
        buf = io.BytesIO()

        if self.engine_options is None:
            buf.write(struct.pack('>?', False))
            return buf.getvalue()
        else:
            buf.write(struct.pack('>?i', True, len(self.engine_options)))

        sorted_items = sorted(self.engine_options.items())

        for key, value in sorted_items:
            if key is None or value is None:
                raise ValueError('illegal engineOptions entry')

            buf.write(struct.pack('>I', len(key)))
            buf.write(key.encode('utf-8'))
            buf.write(struct.pack('>I', len(value)))
            buf.write(value.encode('utf-8'))

        return buf.getvalue()

    @staticmethod
    def unpack(buf: io.BytesIO):
        has_engine_options = struct.unpack('>?', buf.read(1))[0]
        if not has_engine_options:
            return AppTokenOptions()

        engine_options = {}
        size = struct.unpack('>I', buf.read(4))[0]

        for _ in range(size):
            key_length = struct.unpack('>I', buf.read(4))[0]
            key = buf.read(key_length).decode('utf-8')
            value_length = struct.unpack('>I', buf.read(4))[0]
            value = buf.read(value_length).decode('utf-8')

            engine_options[key] = value

        return AppTokenOptions(engine_options)


if __name__ == '__main__':
    options = AppTokenOptions()
    optionsPack = options.pack()

    optionsUnpack = AppTokenOptions.unpack(io.BytesIO(optionsPack))

    assert optionsUnpack.engine_options == options.engine_options

    options.set_engine_options({'k1': 'v1', 'k2': 'v2'})

    optionsPack = options.pack()

    optionsUnpack = AppTokenOptions.unpack(io.BytesIO(optionsPack))

    assert optionsUnpack.engine_options == options.engine_options
