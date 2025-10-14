#ifndef __EXTERNAL_SOURCE_PUSHER_C_H__
#define __EXTERNAL_SOURCE_PUSHER_C_H__

#include <stdint.h>
#include <thread>
#include "raw_file_config.h"
#include "../../../../src/c_api/dingrtc_types.h"

class ExternalSourcePusherC {
public:
    ExternalSourcePusherC(DingRtcHandle handle);
    ~ExternalSourcePusherC();
    
    // loop_count == -1 means loop indefinitely
    bool StartPushRawAuidoFile(const RawAudioFileConfig &config, int64_t loop_count = -1);
    void StopPushRawAuidoFile();
    bool IsRawAudioPushing() { return raw_audio_pushing_; }
    
    // loop_count == -1 means loop indefinitely
    bool StartPushRawVideoFile(const RawVideoFileConfig &config, int64_t loop_count = -1);
    void StopPushRawVideoFile();
    bool IsRawVideoPushing() { return raw_video_pushing_; }

private:
    DingRtcHandle rtc_handle_{nullptr};

    bool raw_audio_quit_{false};
    bool raw_audio_pushing_{false};
    std::thread raw_audio_thread_;

    bool raw_video_quit_{false};
    bool raw_video_pushing_{false};
    std::thread raw_video_thread_;
};

#endif  // __EXTERNAL_SOURCE_PUSHER_C_H__
