#ifndef __TLOG_H__
#define __TLOG_H__

#ifndef ENABLE_TLOG
#define ENABLE_TLOG 1
#endif

#if ENABLE_TLOG

namespace tlog {

enum LogLevel {
    LOG_LEVEL_VERBOSE,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL,
    LOG_LEVEL_NONE
};

struct SourceLoc {
    SourceLoc(const char* filename_in, int line_in, const char* funcname_in)
        : filename(filename_in)
        , line(line_in)
        , funcname(funcname_in) {}

    bool empty() const { return line == 0; }
    const char* filename{nullptr};
    int line{0};
    const char* funcname{nullptr};
};

class Logger {
public:
    static Logger& Instance();

    virtual void SetLogFilePath(const char* filepath) = 0;
    virtual void SetLogLevel(LogLevel level) = 0;
    virtual bool Loggable(LogLevel level) = 0;
    virtual void EnableConsoleOutput(bool enable) = 0;
    virtual void Log(const SourceLoc& loc, LogLevel level, const char* format, ...) = 0;

protected:
    virtual ~Logger() {}
};

}  // namespace tlog

#ifdef __PRETTY_FUNCTION__
    #define TLOG_FUNCTION __PRETTY_FUNCTION__
#else
    #define TLOG_FUNCTION __FUNCTION__
#endif

#define TLOG_LOGGER_CALL(source_loc, level, ...) \
    tlog::Logger::Instance().Log(source_loc, level, __VA_ARGS__)

#define TLOG_FROM_HERE tlog::SourceLoc{__FILE__, __LINE__, TLOG_FUNCTION}
    
#define TLOG_L(level, ...) \
    if (tlog::Logger::Instance().Loggable(level)) \
        tlog::Logger::Instance().Log(TLOG_FROM_HERE, level, __VA_ARGS__)

#define TLOGV(...) TLOG_L(tlog::LOG_LEVEL_VERBOSE, __VA_ARGS__)
#define TLOGD(...) TLOG_L(tlog::LOG_LEVEL_DEBUG, __VA_ARGS__)
#define TLOGI(...) TLOG_L(tlog::LOG_LEVEL_INFO, __VA_ARGS__)
#define TLOGW(...) TLOG_L(tlog::LOG_LEVEL_WARN, __VA_ARGS__)
#define TLOGE(...) TLOG_L(tlog::LOG_LEVEL_ERROR, __VA_ARGS__)
#define TLOGF(...) TLOG_L(tlog::LOG_LEVEL_FATAL, __VA_ARGS__)

#else  // !ENABLE_TLOG

#define TLOGV(...)
#define TLOGD(...)
#define TLOGI(...)
#define TLOGW(...)
#define TLOGE(...)
#define TLOGF(...)

#endif  // ENABLE_TLOG

#endif  // __TLOG_H__
