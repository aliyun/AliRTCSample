#include "external_source_pusher_c.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <chrono>
#include <memory>

#include "tlog/tlog.h"
#include "../../../../src/c_api/dingrtc_api.h"

static const int kBytePerSamplePcm16 = 2;
static const int kStatsIntervalMs = 5000;


ExternalSourcePusherC::ExternalSourcePusherC(DingRtcHandle handle)
    : rtc_handle_(handle) {
}

ExternalSourcePusherC::~ExternalSourcePusherC() {
	TLOGI("ExternalSourcePusherC dtor");
    StopPushRawAuidoFile();
    StopPushRawVideoFile();
}

bool ExternalSourcePusherC::StartPushRawAuidoFile(const RawAudioFileConfig &config, int64_t loop_count) {
    TLOGI("StartPushRawAuidoFile: rtc_handle_: %p, config: %s, loop_count: %" PRId64, rtc_handle_, to_string(config).c_str(), loop_count);
    if (!rtc_handle_) {
        return false;
    }

    int ret = DingRtc_SetExternalAudioSource(rtc_handle_, true, config.sample_rate, config.channels);
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

            DingRtcAudioFrame frame;
            frame.type = DingRtcAudioFramePcm16;
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

            DingRtc_PushExternalAudioFrame(rtc_handle_, &frame);
            sample_count += frame.samples;

            int64_t stats_elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - last_stats_clock).count();
            if (stats_elapsed_ms >= kStatsIntervalMs) {
                TLOGI("pushed pcm sample_count: %" PRId64, sample_count);
                last_stats_clock = std::chrono::steady_clock::now();
            }
        }

        fclose(fp);
        raw_audio_pushing_ = false;
    });

    return true;
}

void ExternalSourcePusherC::StopPushRawAuidoFile() {
    TLOGI("StopPushRawAuidoFile start");
    raw_audio_quit_ = true;
    if (raw_audio_thread_.joinable()) {
        raw_audio_thread_.join();
    }
    raw_audio_quit_ = false;
    DingRtc_SetExternalAudioSource(rtc_handle_, false, 0, 0);
    TLOGI("StopPushRawAuidoFile end");
}

static DingRtcVideoPixelFormat ConvertPixelFormat(const std::string& pixel_format) {
    if (pixel_format == "I420") return DingRtcVideoI420;
    else if (pixel_format == "NV12") return DingRtcVideoNV12;
    else if (pixel_format == "BGRA") return DingRtcVideoBGRA;
    else if (pixel_format == "ARGB") return DingRtcVideoARGB;
    else if (pixel_format == "RGBA") return DingRtcVideoRGBA;
    else if (pixel_format == "ABGR") return DingRtcVideoABGR;
    else return DingRtcVideoI420;
}

static size_t CalcBufferSize(DingRtcVideoPixelFormat pixel_format, int width, int height) {
    size_t buffer_size = 0;
    switch (pixel_format) {
        case DingRtcVideoI420:
        case DingRtcVideoNV12:
        case DingRtcVideoNV21:
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
        case DingRtcVideoBGRA:
        case DingRtcVideoARGB:
        case DingRtcVideoRGBA:
        case DingRtcVideoABGR:
            buffer_size = width * height * 4;
            break;
        default:
            break;
    }
    return buffer_size;
}

bool ExternalSourcePusherC::StartPushRawVideoFile(const RawVideoFileConfig &config, int64_t loop_count) {
    TLOGI("StartPushRawVideoFile: rtc_handle_: %p, config: %s, loop_count: %" PRId64, rtc_handle_, to_string(config).c_str(), loop_count);
    if (!rtc_handle_) {
        return false;
    }
    
    int ret = DingRtc_SetExternalVideoSource(rtc_handle_, true, DingRtcVideoTrackCamera);
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

        DingRtcVideoPixelFormat pixel_format = ConvertPixelFormat(config.pixel_format);
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
            DingRtcVideoFrame frame;
            memset(&frame, 0, sizeof(frame));
            frame.frameType = DingRtcVideoFrameRaw;
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

            DingRtc_PushExternalVideoFrame(rtc_handle_, &frame, DingRtcVideoTrackCamera);
            frame_count++;

            int64_t stats_elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - last_stats_clock).count();
            if (stats_elapsed_ms >= kStatsIntervalMs) {
                TLOGI("push video frame_count: %" PRId64, frame_count);
                last_stats_clock = std::chrono::steady_clock::now();
            }
        }

        fclose(fp);
        raw_video_pushing_ = false;
    });

    return false;
}

void ExternalSourcePusherC::StopPushRawVideoFile() {
    TLOGI("StopPushRawVideoFile start");

    raw_video_quit_ = true;
    if (raw_video_thread_.joinable()) {
        raw_video_thread_.join();
    }
    raw_video_quit_ = false;
    DingRtc_SetExternalVideoSource(rtc_handle_, false, DingRtcVideoTrackCamera);
    TLOGI("StopPushRawVideoFile end");
}
