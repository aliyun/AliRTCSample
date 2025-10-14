#ifndef __EXTERNAL_SOURCE_PUSHER_H__
#define __EXTERNAL_SOURCE_PUSHER_H__

#include <thread>
#include "raw_file_config.h"

namespace ding {
namespace rtc {
class RtcEngine;
}
}

class ExternalSourcePusher {
public:
    // NOTION: ensure the engine is alive when pusher start and stop
    ExternalSourcePusher(ding::rtc::RtcEngine *engine);
    ~ExternalSourcePusher();
    
    // loop_count == -1 means loop indefinitely
    bool StartPushRawAuidoFile(const RawAudioFileConfig &config, int64_t loop_count = -1);
    void StopPushRawAuidoFile();
    bool IsRawAudioPushing() { return raw_audio_pushing_; }
    
    // loop_count == -1 means loop indefinitely
    bool StartPushRawVideoFile(const RawVideoFileConfig &config, int64_t loop_count = -1);
    void StopPushRawVideoFile();
    bool IsRawVideoPushing() { return raw_video_pushing_; }

    void StopAll() {
        StopPushRawAuidoFile();
        StopPushRawVideoFile();
    }

private:
    ding::rtc::RtcEngine *engine_{nullptr};

    bool raw_audio_quit_{false};
    bool raw_audio_pushing_{false};
    std::thread raw_audio_thread_;

    bool raw_video_quit_{false};
    bool raw_video_pushing_{false};
    std::thread raw_video_thread_;
};

#endif  // __EXTERNAL_SOURCE_PUSHER_H__
