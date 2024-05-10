<?php

function compress(string $data): string {
    $compressedData = gzcompress($data);
    return $compressedData;
}

function decompress(string $data): string {
    $decompressedData = gzuncompress($data);
    return $decompressedData;
}
?>