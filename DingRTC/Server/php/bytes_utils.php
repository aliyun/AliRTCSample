<?php

function nextMultiple(int $n, int $baseValue): int {
    if ($baseValue <= 0 || $n <= 0) {
        return 0;
    }
    $result = $baseValue;
    while ($result < $n) {
        $result *= 2;
    }
    return $result;
}

function getFixedLengthBytes(string $srcBytes, int $fixedSize, string $paddingByte = "\x00"): string {
    $currentLength = strlen($srcBytes);
    if ($currentLength < $fixedSize) {
        return $srcBytes . str_repeat($paddingByte, $fixedSize - $currentLength);
    } else {
        return substr($srcBytes, 0, $fixedSize);
    }
}

function getFixedLengthBytesAuto(string $srcBytes): string {
    $nextMultipleOf256 = nextMultiple(strlen($srcBytes), 256);
    return getFixedLengthBytes($srcBytes, $nextMultipleOf256);
}
?>