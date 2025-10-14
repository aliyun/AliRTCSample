#include "external_source_pusher.h"

#include <inttypes.h>
#include <string.h>
#include <chrono>
#include <memory>

#include "engine_interface.h"
#include "tlog/tlog.h"

using namespace ding::rtc;

static const int kBytePerSamplePcm16 = 2;
static const int kStatsIntervalMs = 5000;
static const int kDefaultAudioFreqMillis = 40;
static const int kDefaultVideoFps = 20;

ExternalSourcePusher::ExternalSourcePusher(ding::rtc::RtcEngine *engine)
    : engine_(engine) {
	TLOGI("[%p] ExternalSourcePusher ctor", this);
}

ExternalSourcePusher::~ExternalSourcePusher() {
	TLOGI("[%p] ExternalSourcePusher dtor", this);
    StopAll();
}

bool ExternalSourcePusher::StartPushExternalAudioSource(const RawAudioFileConfig &config, int loop_count) {
    return StartPushRawAudioFile(config, kExternalAudioSource, loop_count);
}

void ExternalSourcePusher::StopPushExternalAudioSource() {
    StopPushRawAudioFile(kExternalAudioSource);
}

bool ExternalSourcePusher::StartPushExternalAudioRender(const RawAudioFileConfig &config, int loop_count) {
    return StartPushRawAudioFile(config, kExternalAudioRender, loop_count);
}

void ExternalSourcePusher::StopPushExternalAudioRender() {
    StopPushRawAudioFile(kExternalAudioRender);
}

bool ExternalSourcePusher::StartPushRawAudioFile(const RawAudioFileConfig &config, ExternalAudioType audio_type, int loop_count) {
    TLOGI("[%p] StartPushRawAuidoFile: engine_:%p, config:%s, audio_type:%d, loop_count: %" PRId64, this, engine_, to_string(config).c_str(), audio_type, loop_count);
    if (!engine_) {
        return false;
    }
    if (config.file_path.empty() || config.sample_rate <= 0 || config.channels <= 0) {
        return false;
    }

    int ret = 0;
    if (audio_type == kExternalAudioSource) {
        ret = engine_->SetExternalAudioSource(true, config.sample_rate, config.channels);
    } else if (audio_type == kExternalAudioRender) {
        ret = engine_->SetExternalAudioRender(true, config.sample_rate, config.channels);
    } else {
        return false;
    }
    if (ret < 0) {
        TLOGI("engine enable external audio failed, ret: %d", ret);
        return false;
    }

    auto push_thread_func = [this, config, audio_type, loop_count] {
        FILE *fp = fopen(config.file_path.c_str(), "rb");
        if (fp) {
            int64_t sample_count = 0;
            int64_t delay_ms = 0;
            auto start_clock = std::chrono::high_resolution_clock::now();
            auto last_stats_clock = std::chrono::steady_clock::now();

            int freq = config.freq > 0 ? config.freq : kDefaultAudioFreqMillis;
            int samples_to_read = config.sample_rate / (1000 / freq);
            int buffer_size = samples_to_read * config.channels * kBytePerSamplePcm16;
            std::unique_ptr<uint8_t[]> buffer(new uint8_t[buffer_size]);

            int64_t loop = loop_count;
            while (true) {
                if (IsAudioQuit(audio_type)) {
                    break;
                }
                size_t read_bytes = fread(buffer.get(), 1, buffer_size, fp);
                if (read_bytes == 0) {
                    TLOGI("audio_type:%d, reading pcm EOF, loop: %" PRId64, audio_type, loop);
                    if (loop == -1 || --loop > 0) {
                        fseek(fp, 0, SEEK_SET);
                        continue;
                    } else {
                        break;
                    }
                }

                RtcEngineAudioFrame frame;
                frame.type = RtcEngineAudioFramePcm16;
                frame.bytesPerSample = kBytePerSamplePcm16;
                frame.samplesPerSec = config.sample_rate;
                frame.channels = config.channels;
                frame.buffer = buffer.get();
                frame.samples = read_bytes / frame.bytesPerSample / frame.channels;
                frame.timestamp = sample_count * 1000 / config.sample_rate;

                delay_ms = frame.timestamp;
                int64_t elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_clock).count();
                if (delay_ms - elapsed_ms > 5) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms - elapsed_ms));
                }

                if (audio_type == kExternalAudioSource) {
                    engine_->PushExternalAudioFrame(&frame);
                } else if (audio_type == kExternalAudioRender) {
                    engine_->PushExternalAudioRenderFrame(&frame);
                }

                sample_count += frame.samples;

                int64_t stats_elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - last_stats_clock).count();
                if (stats_elapsed_ms >= kStatsIntervalMs) {
                    TLOGI("audio_type:%d, pushed pcm sample_count: %" PRId64, audio_type, sample_count);
                    last_stats_clock = std::chrono::steady_clock::now();
                }
            }

            fclose(fp);
        } else {
            TLOGE("open audio file failed");
        }
        // push thread finished
        if (audio_type == kExternalAudioSource) {
            audio_source_pushing_ = false;
        } else if (audio_type == kExternalAudioRender) {
            audio_render_pushing_ = false;
        }
    };
    
    if (audio_type == kExternalAudioSource) {
        audio_source_pushing_ = true;
        audio_source_thread_ = std::thread(push_thread_func);
    } else if (audio_type == kExternalAudioRender) {
        audio_render_pushing_ = true;
        audio_render_thread_ = std::thread(push_thread_func);
    }

    return true;
}

bool ExternalSourcePusher::StopPushRawAudioFile(ExternalAudioType audio_type) {
    TLOGI("[%p] StopPushRawAuidoFile type:%d start", this, audio_type);
    if (audio_type == kExternalAudioSource) {
        audio_source_quit_ = true;
        if (audio_source_thread_.joinable()) {
            audio_source_thread_.join();
        }
        audio_source_quit_ = false;
        if (engine_) {
            engine_->SetExternalAudioSource(false, 0, 0);
        }
    } else if (audio_type == kExternalAudioRender) {
        audio_render_quit_ = true;
        if (audio_render_thread_.joinable()) {
            audio_render_thread_.join();
        }
        audio_render_quit_ = false;
        if (engine_) {
            engine_->SetExternalAudioRender(false, 0, 0);
        }
    }
    TLOGI("[%p] StopPushRawAuidoFile type:%d end", this, audio_type);
}

bool ExternalSourcePusher::IsAudioQuit(ExternalAudioType audio_type) {
    if (audio_type == kExternalAudioSource) {
        return audio_source_quit_;
    } else if (audio_type == kExternalAudioRender) {
        return audio_render_quit_;
    }
    return false;
}

static RtcEngineVideoPixelFormat ConvertPixelFormat(const std::string& pixel_format) {
    if (pixel_format == "I420") return RtcEngineVideoI420;
    else if (pixel_format == "NV12") return RtcEngineVideoNV12;
    else if (pixel_format == "BGRA") return RtcEngineVideoBGRA;
    else if (pixel_format == "ARGB") return RtcEngineVideoARGB;
    else if (pixel_format == "RGBA") return RtcEngineVideoRGBA;
    else if (pixel_format == "ABGR") return RtcEngineVideoABGR;
    else return RtcEngineVideoI420;
}

static size_t CalcBufferSize(RtcEngineVideoPixelFormat pixel_format, int width, int height) {
    size_t buffer_size = 0;
    switch (pixel_format) {
        case RtcEngineVideoI420:
        case RtcEngineVideoNV12:
        case RtcEngineVideoNV21:
            // case VideoType::kIYUV:
            // case VideoType::kYV12:
            {
                int half_width = (width + 1) >> 1;
                int half_height = (height + 1) >> 1;
                buffer_size = width * height + half_width * half_height * 2;
                break;
            }
        // case VideoType::kARGB4444:
        // case VideoType::kRGB565:
        // case VideoType::kARGB1555:
        // case VideoType::kYUY2:
        // case VideoType::kUYVY:
        //   buffer_size = width * height * 2;
        //   break;
        // case VideoType::kRGB24:
        //   buffer_size = width * height * 3;
        //   break;
        case RtcEngineVideoBGRA:
        case RtcEngineVideoARGB:
        case RtcEngineVideoRGBA:
        case RtcEngineVideoABGR:
            buffer_size = width * height * 4;
            break;
        default:
            break;
    }
    return buffer_size;
}

bool ExternalSourcePusher::StartPushExternalVideoSource(const RawVideoFileConfig &config, int64_t loop_count) {
    return StartPushRawVideoFile(config, RtcEngineVideoTrackCamera, loop_count);
}

void ExternalSourcePusher::StopPushExternalVideoSource() {
    StopPushRawVideoFile(RtcEngineVideoTrackCamera);
}

bool ExternalSourcePusher::StartPushExternalScreenSource(const RawVideoFileConfig &config, int64_t loop_count) {
    return StartPushRawVideoFile(config, RtcEngineVideoTrackScreen, loop_count);
}

void ExternalSourcePusher::StopPushExternalScreenSource() {
    StopPushRawVideoFile(RtcEngineVideoTrackScreen);
}

bool ExternalSourcePusher::StartPushRawVideoFile(const RawVideoFileConfig &config, ding::rtc::RtcEngineVideoTrack track, int64_t loop_count) {
    TLOGI("[%p] StartPushRawVideoFile: engine_:%p, config:%s, track:%d, loop_count:%" PRId64, this, engine_, to_string(config).c_str(), track, loop_count);
    if (!engine_) {
        return false;
    }
    if (config.file_path.empty() || config.width <= 0 || config.height <= 0) {
        return false;
    }
    
    int ret = engine_->SetExternalVideoSource(true, track);
    if (ret < 0) {
        TLOGI("engine enable external video source failed, ret: %d", ret);
        return false;
    }

    auto push_thread_func = [this, config, track, loop_count] {
        FILE *fp = fopen(config.file_path.c_str(), "rb");
        if (fp) {
            int64_t frame_count = 0;
            int64_t delay_ms = 0;
            auto start_clock = std::chrono::high_resolution_clock::now();
            auto last_stats_clock = std::chrono::steady_clock::now();

            RtcEngineVideoPixelFormat pixel_format = ConvertPixelFormat(config.pixel_format);
            int buffer_size = CalcBufferSize(pixel_format, config.width, config.height);
            std::unique_ptr<uint8_t[]> buffer(new uint8_t[buffer_size]);
            
            int64_t loop = loop_count;
            while (true) {
                if (IsVideoQuit(track)) {
                    break;
                }
                // auto readStartClock = std::chrono::high_resolution_clock::now();
                size_t read_bytes = fread(buffer.get(), 1, buffer_size, fp);
                // int64_t readCostTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - readStartClock).count();
                // TLOGI("videoReadThread Reading yuv read_bytes:%zu, buffer_size:%d, readCostTime:%ld", read_bytes, buffer_size, readCostTime);
                if (read_bytes == 0) {
                    TLOGI("track:%d, reading yuv EOF, loop: %" PRId64, track, loop);
                    if (loop == -1 || --loop > 0) {
                        fseek(fp, 0, SEEK_SET);
                        continue;
                    } else {
                        break;
                    }
                }

                RtcEngineVideoFrame frame;
                memset(&frame, 0, sizeof(frame));
                frame.frameType = RtcEngineVideoFrameRaw;
                frame.pixelFormat = pixel_format;
                frame.width = config.width;
                frame.height = config.height;
                frame.stride[0] = config.width;
                frame.stride[1] = frame.stride[2] = config.width >> 1;
                frame.rotation = config.rotation;
                frame.data = buffer.get();
                int fps = config.fps > 0 ? config.fps : kDefaultVideoFps;
                frame.timestamp = frame_count * 1000 / fps;

                delay_ms = frame.timestamp;
                int64_t elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_clock).count();
                if (delay_ms - elapsed_ms > 5) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms - elapsed_ms));
                }
                
                engine_->PushExternalVideoFrame(&frame, track);
                frame_count++;

                int64_t stats_elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - last_stats_clock).count();
                if (stats_elapsed_ms >= kStatsIntervalMs) {
                    TLOGI("track:%d, push video frame_count: %" PRId64, track, frame_count);
                    last_stats_clock = std::chrono::steady_clock::now();
                }
            }

            fclose(fp);
        } else {
            TLOGI("open video file failed");
        }
        // push thread finished
        if (track == RtcEngineVideoTrackCamera) {
            video_source_pushing_ = false;
        } else if (track == RtcEngineVideoTrackScreen) {
            screen_source_pushing_ = false;
        }
    };

    if (track == RtcEngineVideoTrackCamera) {
        video_source_pushing_ = true;
        video_source_thread_ = std::thread(push_thread_func);
    } else if (track == RtcEngineVideoTrackScreen) {
        screen_source_pushing_ = true;
        screen_source_thread_ = std::thread(push_thread_func);
    }

    return false;
}

void ExternalSourcePusher::StopPushRawVideoFile(ding::rtc::RtcEngineVideoTrack track) {
    TLOGI("[%p] StopPushRawVideoFile track:%d start", this, track);
    if (track == RtcEngineVideoTrackCamera) {
        video_source_quit_ = true;
        if (video_source_thread_.joinable()) {
            video_source_thread_.join();
        }
        video_source_quit_ = false;
    } else if (track == RtcEngineVideoTrackScreen) {
        screen_source_quit_ = true;
        if (screen_source_thread_.joinable()) {
            screen_source_thread_.join();
        }
        screen_source_quit_ = false;
    }
    if (engine_) {
        engine_->SetExternalVideoSource(false, track);
    }
    TLOGI("[%p] StopPushRawVideoFile track:%d end", this, track);
}

bool ExternalSourcePusher::IsVideoQuit(ding::rtc::RtcEngineVideoTrack track) {
    if (track == RtcEngineVideoTrackCamera) {
        return video_source_quit_;
    } else if (track == RtcEngineVideoTrackScreen) {
        return screen_source_quit_;
    }
    return false;
}

void ExternalSourcePusher::StopAll() {
    StopPushExternalAudioSource();
    StopPushExternalAudioRender();
    StopPushExternalVideoSource();
    StopPushExternalScreenSource();
}
