<?php

class Service {
    const WILDCARD_CHARACTERS = "*";
    const ENABLE_PRIVILEGE = 1;
    const ENABLE_AUDIO_PRIVILEGE = 2;
    const ENABLE_VIDEO_PRIVILEGE = 4;

    private $channel_id;
    private $user_id;
    private $privilege;

    public function __construct(string $channel_id, string $user_id, $privilege = null) {
        $this->channel_id = $channel_id;
        $this->user_id = $user_id;
        $this->privilege = $privilege;
    }

    public static function create_service_only_with_user_id(string $user_id): self {
        return new self(self::WILDCARD_CHARACTERS, $user_id);
    }

    public static function create_service_only_with_channel_id(string $channel_id): self {
        return new self($channel_id, self::WILDCARD_CHARACTERS);
    }

    public function add_audio_publish_privilege() {
        if ($this->privilege === null) {
            $this->privilege = 0 | self::ENABLE_PRIVILEGE;
        }
        $this->privilege |= self::ENABLE_AUDIO_PRIVILEGE;
    }

    public function add_video_publish_privilege() {
        if ($this->privilege === null) {
            $this->privilege = 0 | self::ENABLE_PRIVILEGE;
        }
        $this->privilege |= self::ENABLE_VIDEO_PRIVILEGE;
    }

    public function pack(): string {
        $buf = '';

        $channel_id_bytes = $this->channel_id;
        $buf .= pack('N', strlen($channel_id_bytes));
        $buf .= $channel_id_bytes;
        
        $user_id_bytes = $this->user_id;
        $buf .= pack('N', strlen($user_id_bytes));
        $buf .= $user_id_bytes;
        
        if ($this->privilege !== null) {
            $buf .= pack('C', 1);
            $buf .= pack('N', $this->privilege);
        } else {
            $buf .= pack('C', 0);
        }
        return $buf;
    }

    public static function unpack($buffer): self {
        $channel_id_length = unpack('N', fread($buffer,4))[1];
        $channel_id = fread($buffer,$channel_id_length);

        $user_id_length = unpack('N', fread($buffer,4))[1];
        $user_id = fread($buffer,$user_id_length);


        $has_privilege = unpack('C', fread($buffer,1))[1];
        $privilege = null;
        if ($has_privilege === 1) {
            $privilege = unpack('N', fread($buffer,4))[1];
        }

        return new self($channel_id, $user_id, $privilege);
    }
}
?>