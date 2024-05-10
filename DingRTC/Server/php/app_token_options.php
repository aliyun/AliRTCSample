<?php

class AppTokenOptions
{
    private $engine_options = [];

    public function __construct($engine_options = [])
    {
        $this->set_engine_options($engine_options);
    }

    public function set_engine_options($engine_options)
    {
        $this->engine_options = $engine_options ?? [];
    }

    public function pack(): string
    {
        $buf = '';
        if ($this->engine_options === null) {
            $buf .= pack('C', 0);
        } else {
            $buf .= pack('C', 1);
            $buf .= pack('N', count($this->engine_options));
            ksort($this->engine_options);
            foreach ($this->engine_options as $key => $value) {
                if ($key === null || $value === null) {
                    throw new \ValueError('illegal engineOptions entry');
                }
                $buf .= pack('N', strlen($key)) . $key;
                $buf .= pack('N', strlen($value)) . $value;
            }
        }
        return $buf;
    }

    public static function unpack($buffer): self
    {
        $has_engine_options = unpack('C', fread($buffer,1))[1];
        $engine_options = [];

        if ($has_engine_options) {
            $size = unpack('N', fread($buffer,4))[1];
            for ($i = 0; $i < $size; $i++) {
                $key_length = unpack('N', fread($buffer,4))[1];
                $key = fread($buffer,$key_length);
                $value_length = unpack('N', fread($buffer,4))[1];
                $value = fread($buffer,$value_length);
                $engine_options[$key] = $value;
            }
        }
        return new self($engine_options);
    }
}
