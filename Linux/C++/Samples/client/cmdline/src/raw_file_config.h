#ifndef __RAW_FILE_CONFIG_H__
#define __RAW_FILE_CONFIG_H__

#include <string>
#include <sstream>

typedef struct {
    std::string file_path;
    int sample_rate;
    int channels;
    int freq; // push audio duration ms each time
} RawAudioFileConfig;

inline std::string to_string(const RawAudioFileConfig &config) {
    std::ostringstream oss;
    oss << "{"
        << "file_path:" << config.file_path << ","
        << "sample_rate:" << config.sample_rate << ","
        << "channels:" << config.channels << ","
        << "freq:" << config.freq << ","
        << "}";
    return oss.str();
}

typedef struct {
    std::string file_path;
    std::string pixel_format;
    int width;
    int height;
    int fps;
    int rotation;
} RawVideoFileConfig;

inline std::string to_string(const RawVideoFileConfig &config) {
    std::ostringstream oss;
    oss << "{"
        << "file_path:" << config.file_path << ","
        << "pixel_format:" << config.pixel_format << ","
        << "width:" << config.width << ","
        << "height:" << config.height << ","
        << "fps:" << config.fps << ","
        << "rotation:" << config.rotation << ","
        << "}";
    return oss.str();
}

#endif // __RAW_FILE_CONFIG_H__
