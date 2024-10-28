def _next_multiple(n, base_value) :
    if base_value <= 0 or n <= 0:
        return 0
    result = base_value
    while result < n:
        result *= 2
    return result


def _get_fixed_length_bytes(src_bytes, fixed_size, padding_byte=b'\x00'):
    if len(src_bytes) < fixed_size:
        return src_bytes + padding_byte * (fixed_size - len(src_bytes))
    else:
        return src_bytes[:fixed_size]


def get_fixed_length_bytes(src_bytes):
    return _get_fixed_length_bytes(src_bytes, _next_multiple(len(src_bytes), 256))


if __name__ == '__main__':
    original_data = b"hello world!"
    print(get_fixed_length_bytes(original_data))