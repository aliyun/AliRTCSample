def _next_multiple(n: int, base_value: int) -> int:
    if base_value <= 0 or n <= 0:
        return 0
    result = base_value
    while result < n:
        result *= 2
    return result


def _get_fixed_length_bytes(src_bytes: bytes, fixed_size: int, padding_byte=b'\x00') -> bytes:
    if len(src_bytes) < fixed_size:
        return src_bytes + padding_byte * (fixed_size - len(src_bytes))
    else:
        return src_bytes[:fixed_size]


def get_fixed_length_bytes(src_bytes: bytes) -> bytes:
    return _get_fixed_length_bytes(src_bytes, _next_multiple(len(src_bytes), 256))
