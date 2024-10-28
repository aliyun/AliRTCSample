import base64
import zlib


def compress(data: bytes) -> bytes:
    compressor = zlib.compressobj(wbits=zlib.MAX_WBITS)
    compressed_data = compressor.compress(data)
    compressed_data += compressor.flush()
    return compressed_data


def decompress(data: bytes) -> bytes:
    decompressed_data = zlib.decompress(data, zlib.MAX_WBITS)
    return decompressed_data


if __name__ == "__main__":
    original_data = b"hello world!"
    compressed_data = base64.b64encode(compress(original_data))
    print(compressed_data.decode())
