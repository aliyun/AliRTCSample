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


ExternalSourcePusher::ExternalSourcePusher(ding::rtc::RtcEngine *engine)
    : engine_(engine) {
	TLOGI("[%p] ExternalSourcePusher ctor", this);
}

ExternalSourcePusher::~ExternalSourcePusher() {
	TLOGI("[%p] ExternalSourcePusher dtor", this);
    StopAll();
}

bool ExternalSourcePusher::StartPushRawAuidoFile(const RawAudioFileConfig &config, int64_t loop_count) {
    TLOGI("[%p] StartPushRawAuidoFile: engine_: %p, config: %s, loop_count: %" PRId64, this, engine_, to_string(config).c_str(), loop_count);
    if (!engine_) {
        return false;
    }

    int ret = engine_->SetExternalAudioSource(true, config.sample_rate, config.channels);
    if (ret < 0) {
        TLOGI("engine enable external audio source failed, ret: %d", ret);
        return false;
    }

    raw_audio_pushing_ = true;
    raw_audio_thread_ = std::thread([=] {
        FILE *fp = fopen(config.file_path.c_str(), "rb");
        if (!fp) {
            TLOGI("open audio file failed");
            raw_audio_pushing_ = false;
            return;
        }

        int64_t sample_count = 0;
        int64_t delay_ms = 0;
        auto start_clock = std::chrono::high_resolution_clock::now();
        auto last_stats_clock = std::chrono::steady_clock::now();

        int samples_to_read = config.sample_rate / (1000 / config.freq);
        int buffer_size = samples_to_read * config.channels * kBytePerSamplePcm16;
        std::unique_ptr<uint8_t[]> buffer(new uint8_t[buffer_size]);

        int64_t loop = loop_count;
        while (!raw_audio_quit_) {
            size_t read_bytes = fread(buffer.get(), 1, buffer_size, fp);
            if (read_bytes == 0) {
                TLOGI("reading pcm EOF, loop: %" PRId64, loop);
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

            engine_->PushExternalAudioFrame(&frame);
            sample_count += frame.samples;

            int64_t stats_elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - last_stats_clock).count();
            if (stats_elapsed_ms >= kStatsIntervalMs) {
                TLOGI("[%p] pushed pcm sample_count: %" PRId64, this, sample_count);
                last_stats_clock = std::chrono::steady_clock::now();
            }
        }

        fclose(fp);
        raw_audio_pushing_ = false;
    });

    return true;
}

void ExternalSourcePusher::StopPushRawAuidoFile() {
    TLOGI("[%p] StopPushRawAuidoFile start", this);
    raw_audio_quit_ = true;
    if (raw_audio_thread_.joinable()) {
        raw_audio_thread_.join();
    }
    raw_audio_quit_ = false;
    if (engine_) {
        engine_->SetExternalAudioSource(false, 0, 0);
    }
    TLOGI("[%p] StopPushRawAuidoFile end", this);
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

bool ExternalSourcePusher::StartPushRawVideoFile(const RawVideoFileConfig &config, int64_t loop_count) {
    TLOGI("[%p] StartPushRawVideoFile: engine_: %p, config: %s, loop_count: %" PRId64, this, engine_, to_string(config).c_str(), loop_count);
    if (!engine_) {
        return false;
    }
    
    int ret = engine_->SetExternalVideoSource(true, RtcEngineVideoTrackCamera);
    if (ret < 0) {
        TLOGI("engine enable external video source failed, ret: %d", ret);
        return false;
    }

    raw_video_pushing_ = true;
    raw_video_thread_ = std::thread([=] {
        FILE *fp = fopen(config.file_path.c_str(), "rb");
        if (!fp) {
            TLOGI("open video file failed");
            raw_video_pushing_ = false;
            return;
        }

        int64_t frame_count = 0;
        int64_t delay_ms = 0;
        auto start_clock = std::chrono::high_resolution_clock::now();
        auto last_stats_clock = std::chrono::steady_clock::now();

        RtcEngineVideoPixelFormat pixel_format = ConvertPixelFormat(config.pixel_format);
        int buffer_size = CalcBufferSize(pixel_format, config.width, config.height);
        std::unique_ptr<uint8_t[]> buffer(new uint8_t[buffer_size]);
        
        int64_t loop = loop_count;
        while (!raw_video_quit_) {
            // auto readStartClock = std::chrono::high_resolution_clock::now();
            size_t read_bytes = fread(buffer.get(), 1, buffer_size, fp);
            // int64_t readCostTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - readStartClock).count();
            // TLOGI("videoReadThread Reading yuv read_bytes:%zu, buffer_size:%d, readCostTime:%ld", read_bytes, buffer_size, readCostTime);
            if (read_bytes == 0) {
                TLOGI("reading yuv EOF, loop: %" PRId64, loop);
                if (loop == -1 || --loop > 0) {
                    fseek(fp, 0, SEEK_SET);
                    continue;
                } else {
                    break;
                }
            }

            // 配置外部视频流参数
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
            frame.timestamp = frame_count * 1000 / config.fps;

            delay_ms = frame.timestamp;
            int64_t elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_clock).count();
            if (delay_ms - elapsed_ms > 5) {
                std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms - elapsed_ms));
            }
            
            engine_->PushExternalVideoFrame(&frame, RtcEngineVideoTrackCamera);
            frame_count++;

            int64_t stats_elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - last_stats_clock).count();
            if (stats_elapsed_ms >= kStatsIntervalMs) {
                TLOGI("[%p] push video frame_count: %" PRId64, this, frame_count);
                last_stats_clock = std::chrono::steady_clock::now();
            }
        }

        fclose(fp);
        raw_video_pushing_ = false;
    });

    return false;
}

void ExternalSourcePusher::StopPushRawVideoFile() {
    TLOGI("[%p] StopPushRawVideoFile start", this);

    raw_video_quit_ = true;
    if (raw_video_thread_.joinable()) {
        raw_video_thread_.join();
    }
    raw_video_quit_ = false;
    if (engine_) {
        engine_->SetExternalVideoSource(false, RtcEngineVideoTrackCamera);
    }
    TLOGI("[%p] StopPushRawVideoFile end", this);
}
