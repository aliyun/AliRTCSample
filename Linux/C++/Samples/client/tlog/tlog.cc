#include "tlog.h"

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <mutex>
#include <sstream>
#include <string>

#include "platform_thread.h"
#include "utils.h"

namespace tlog {

// ---------- Logger definition ----------
class LoggerImpl : public Logger {
public:
    LoggerImpl();
    ~LoggerImpl();

    void SetLogFilePath(const char* filepath) override;
    void SetLogLevel(LogLevel level) override;
    bool Loggable(LogLevel level) override;
    void EnableConsoleOutput(bool enable) override;
    void Log(const SourceLoc& source, LogLevel level, const char* fmt, ...) override;
    LogLevel GetLogLevel() const { return log_level_; }

private:
    void LogToConsole(const std::string& msg);
    void LogToFile(const std::string& msg);
    int OpenLogFile();
    void CloseLogFile();
    void PrintLog(const SourceLoc& source, LogLevel level, const std::string& user_msg);

    bool enable_console_output_{ true };
    LogLevel log_level_{ LOG_LEVEL_INFO };
    std::mutex lock_;
    std::string log_file_path_;
    FILE* log_file_{ nullptr };
};

static const char* kLogLevelNames[] = { "VERBOSE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL", "NONE" };
static const char* kLogLevelShortNames[] = { "V", "D", "I", "W", "E", "F", "N" };

static const char* LogLevelToString(LogLevel level) {
    return kLogLevelShortNames[(int)level];
}

// ---------- Logger implementation ----------

static bool g_log_to_file = true;
static bool g_log_datetime = true;
static bool g_log_threadid = true;
static bool g_log_sourceloc = true;
static bool g_log_funcname = false;

Logger& Logger::Instance() {
    static LoggerImpl g_logger;
    return g_logger;
}

LoggerImpl::LoggerImpl() {}

LoggerImpl::~LoggerImpl() {
    std::lock_guard<std::mutex> lk(lock_);
    CloseLogFile();
}

void LoggerImpl::SetLogLevel(LogLevel level) {
    log_level_ = std::min(level, LOG_LEVEL_NONE);
}

bool LoggerImpl::Loggable(LogLevel level) {
    return (log_level_ < LOG_LEVEL_NONE) && (level >= log_level_);
}

void LoggerImpl::EnableConsoleOutput(bool enable) {
    enable_console_output_ = enable;
}

void LoggerImpl::SetLogFilePath(const char* filePath) {
    if (filePath == nullptr) return;
    std::lock_guard<std::mutex> lk(lock_);
    if (log_file_) {
        LogToConsole("Warning, log file already opend?");
        CloseLogFile();
    }
    log_file_path_ = filePath;
}

void LoggerImpl::Log(const SourceLoc& source, LogLevel level, const char* fmt, ...) {
    if (!Loggable(level)) {
        return;
    }

    std::string user_msg;
    va_list ap;
    va_start(ap, fmt);
    StringAppendV(&user_msg, fmt, ap);
    va_end(ap);

    PrintLog(source, level, user_msg);
}

void LoggerImpl::PrintLog(const SourceLoc& source, LogLevel level, const std::string& user_msg) {
    // [timestamp][thread_id][level](filename:line)[function]: user_msg
    std::lock_guard<std::mutex> lk(lock_);

    std::ostringstream oss;
    if (g_log_datetime) {
        oss << "[" << DateTimePretty() << "]";
    }

    if (g_log_threadid) {
        oss << "[" << static_cast<int>(CurrentThreadId()) << "]";
    }

    oss << "[" << LogLevelToString(level) << "]";

    if (g_log_sourceloc && !source.empty()) {
        oss << "(" << FilenameFromPath(source.filename) << ":" << source.line << ")";
    }

    if (g_log_funcname && source.funcname) {
        oss << "[" << source.funcname << "]";
    }

    oss << ": " << user_msg << "\n";

    std::string whole_msg(oss.str());

    if (enable_console_output_) {
        LogToConsole(whole_msg.c_str());
    }
    if (g_log_to_file && !log_file_path_.empty()) {
        LogToFile(whole_msg);
    }
}

void LoggerImpl::LogToConsole(const std::string& msg) {
    fprintf(stdout, "%s", msg.c_str());
    fflush(stdout);
}

void LoggerImpl::LogToFile(const std::string& msg) {
    if (log_file_ == nullptr) {
        if (OpenLogFile() < 0) {
            LogToConsole("Error, open log file FAILED!\n");
        }
    }
    if (log_file_) {
        fwrite(msg.c_str(), 1, msg.length(), log_file_);
        fflush(log_file_);
    }
}

int LoggerImpl::OpenLogFile() {
    if (log_file_path_.empty()) {
        LogToConsole("Error, log file path is empty!");
        return -1;
    }

    // create parent directories if nessesary
    auto pos = log_file_path_.find_last_of("/");
    if (pos != std::string::npos) {
        std::string dir_path = log_file_path_.substr(0, pos);
        if (!CreateDirectoryTree(dir_path.c_str())) {
            LogToConsole(std::string("Failed to create directory: ") + dir_path);
            return -1;
        }
    }

    log_file_ = fopen(log_file_path_.c_str(), "wb+");
    if (!log_file_) {
        LogToConsole(std::string("fopen failed, filepath: ") + log_file_path_ + ", errno: " + std::to_string(errno) +
                     "(" + strerror(errno) + ")");
        return -1;
    }
    return 0;
}

void LoggerImpl::CloseLogFile() {
    if (log_file_) {
        fflush(log_file_);
        fclose(log_file_);
        log_file_ = nullptr;
    }
}

}  // namespace tlog
