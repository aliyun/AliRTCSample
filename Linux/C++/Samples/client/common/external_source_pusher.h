#ifndef __EXTERNAL_SOURCE_PUSHER_H__
#define __EXTERNAL_SOURCE_PUSHER_H__

#include <thread>
#include "raw_file_config.h"
#include "engine_types.h"

namespace ding {
namespace rtc {
class RtcEngine;
}
}

class ExternalSourcePusher {
public:
    enum ExternalAudioType {
        kExternalAudioSource,
        kExternalAudioRender
    };

    // NOTE: ensure the engine is alive when pusher is working
    ExternalSourcePusher(ding::rtc::RtcEngine *engine);
    ~ExternalSourcePusher();
    
    bool StartPushExternalAudioSource(const RawAudioFileConfig &config, int loop_count = -1);
    void StopPushExternalAudioSource();
    // bool IsExternalAudioSourcePushing() { return audio_source_pushing_; }

    bool StartPushExternalAudioRender(const RawAudioFileConfig &config, int loop_count = -1);
    void StopPushExternalAudioRender();
    // bool IsExternalAudioRenderPushing() { return audio_render_pushing_; }
    
    bool StartPushExternalVideoSource(const RawVideoFileConfig &config, int64_t loop_count = -1);
    void StopPushExternalVideoSource();
    // bool IsExternalVideoSourcePushing() { return video_source_pushing_; }

    bool StartPushExternalScreenSource(const RawVideoFileConfig &config, int64_t loop_count = -1);
    void StopPushExternalScreenSource();
    // bool IsExternalScreenSourcePushing() { return screen_source_pushing_; }

    void StopAll();

private:
    bool StartPushRawAudioFile(const RawAudioFileConfig &config, ExternalAudioType audio_type, int loop_count);
    bool StopPushRawAudioFile(ExternalAudioType audio_type);
    bool IsAudioQuit(ExternalAudioType audio_type);

    bool StartPushRawVideoFile(const RawVideoFileConfig &config, ding::rtc::RtcEngineVideoTrack track, int64_t loop_count);
    void StopPushRawVideoFile(ding::rtc::RtcEngineVideoTrack track);
    bool IsVideoQuit(ding::rtc::RtcEngineVideoTrack video_track);

private:
    ding::rtc::RtcEngine *engine_ = nullptr;

    bool audio_source_quit_ = false;
    bool audio_source_pushing_ = false;
    std::thread audio_source_thread_;

    bool audio_render_quit_ = false;
    bool audio_render_pushing_ = false;
    std::thread audio_render_thread_;

    bool video_source_quit_ = false;
    bool video_source_pushing_ = false;
    std::thread video_source_thread_;

    bool screen_source_quit_ = false;
    bool screen_source_pushing_ = false;
    std::thread screen_source_thread_;

};

#endif  // __EXTERNAL_SOURCE_PUSHER_H__
