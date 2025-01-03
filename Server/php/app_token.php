<?php

require_once  'sign_utils.php';
require_once  'compress_utils.php';
require_once  'app_token_options.php';
require_once  'bytes_utils.php';
require_once  'service.php';

class AppToken
{
    private $app_id;
    private $app_key;
    private $timestamp;
    private $issue_timestamp;
    private $salt;
    private $service;
    private $options;
    private $signature;

    const VERSION_0 = "000";

    public function __construct($app_id, $app_key, $timestamp, $issue_timestamp = null, $salt = null, $service = null, $options = null, $signature = null)
    {
        $this->app_id = $app_id;
        $this->app_key = $app_key;
        $this->timestamp = $timestamp;
        $this->issue_timestamp = $issue_timestamp ?? time();
        $this->salt = $salt ?? rand(1, $this->issue_timestamp);
        $this->service = $service;
        $this->options = $options;
        $this->signature = $signature;
    }

    public function setService($service)
    {
        $this->service = $service;
    }

    public function setOptions($options)
    {
        $this->options = $options;
    }

    public function build()
    {
        if ($this->app_key === null) {
            throw new ValueError('missing secretKey');
        }
        if ($this->service === null) {
            throw new ValueError('missing service');
        }

        $signKey = generate_sign($this->app_key, $this->issue_timestamp, $this->salt);

        $buf = '';
        $app_id_bytes = $this->app_id;
        $buf .= pack('N', strlen($app_id_bytes));
        $buf .= $app_id_bytes;
        $buf .= pack('N', $this->issue_timestamp);
        $buf .= pack('N', $this->salt);
        $buf .= pack('N', $this->timestamp);
        $buf .= $this->service->pack();
        if ($this->options === null) {
            $this->options = new AppTokenOptions();
        }
        $buf .= $this->options->pack();

        $fix_length_buf = getFixedLengthBytesAuto($buf);

        $signature = sign($signKey, $fix_length_buf);

        $token_buf = '';
        $token_buf .= pack('N', strlen($signature));
        $token_buf .= $signature;
        $token_buf .= $fix_length_buf;

        $fix_length_token_buf = getFixedLengthBytesAuto($token_buf);
        return self::VERSION_0 . base64_encode(compress($fix_length_token_buf));
    }

    public static function parse($token): self
    {
        if ($token === null) {
            throw new ValueError('empty appToken');
        }

        if (!substr($token, 0, strlen(self::VERSION_0)) == self::VERSION_0) {
            throw new ValueError('unsupported version');
        }

        $token_buf = fopen("php://memory", 'r+');

        assert($token_buf);

        fwrite($token_buf, decompress(base64_decode(substr($token, strlen(self::VERSION_0)))));
        rewind($token_buf);

        $signature_length = unpack('N', fread($token_buf, 4))[1];
        $signature = fread($token_buf, $signature_length);

        $app_id_length = unpack('N', fread($token_buf, 4))[1];
        $app_id = fread($token_buf, $app_id_length);

        $issue_timestamp = unpack('N', fread($token_buf, 4))[1];
        $salt = unpack('N', fread($token_buf, 4))[1];
        $timestamp = unpack('N', fread($token_buf,4))[1];

        $service = Service::unpack($token_buf);
        $options = AppTokenOptions::unpack($token_buf);

        fclose($token_buf);

        return new self($app_id, null, $timestamp, $issue_timestamp, $salt, $service, $options, $signature);
    }
}
