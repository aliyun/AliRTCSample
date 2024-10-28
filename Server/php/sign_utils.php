<?php
function generate_sign(string $key, int $timestamp, int $salt): string
{
    $buf_ts = pack('N', $timestamp);
    $sign_key = hash_hmac('sha256', $key, $buf_ts, true);

    $buf_salt = pack('N', $salt);
    $tmp_sign =  hash_hmac('sha256', $sign_key, $buf_salt, true);
    return $tmp_sign;
}

function sign(string $key, string $data): string
{
    return hash_hmac('sha256', $data, $key, true);
}
?>