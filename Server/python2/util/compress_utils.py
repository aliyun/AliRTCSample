import base64
import zlib


def compress(data) :
    compressor = zlib.compressobj(zlib.Z_DEFAULT_COMPRESSION, zlib.DEFLATED, zlib.MAX_WBITS)
    compressed_data = compressor.compress(data)
    compressed_data += compressor.flush()
    return compressed_data


def decompress(data) :
    decompressed_data = zlib.decompress(data, zlib.MAX_WBITS)
    return decompressed_data


if __name__ == "__main__":
    original_data = b"hello world!"
    compressed_data = base64.b64encode(compress(original_data))
    print(compressed_data.decode())
