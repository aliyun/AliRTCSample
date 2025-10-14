//
//  tlog_print.h
//  logging
//
//  Created by yuqilin.
//

#ifndef tlog_print_h
#define tlog_print_h

#ifndef ENABLE_TLOG
#define ENABLE_TLOG 1
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64) || defined(__CYGWIN__)
# define TLOG_OS_WIN
#elif defined(linux) || defined(__linux) || defined(__linux__)
# define TLOG_OS_LINUX
# if defined(__ANDROID__)
#  define TLOG_OS_ANDROID
# endif
#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
# define TLOG_OS_MAC
# include <TargetConditionals.h>
# if TARGET_OS_IPHONE == 1
#  define TLOG_OS_IOS
# endif
#else
# warning "Unknown OS"
#endif

#if defined(TLOG_OS_LINUX) || defined(TLOG_OS_MAC)
#define TLOG_OS_POSIX
#endif

#if defined(TLOG_OS_WIN)
#include <windows.h>
typedef DWORD PlatformThreadId;
#elif defined(TLOG_OS_POSIX)
#include <pthread.h>
#include <sys/types.h>
#include <sys/syscall.h>
typedef pid_t PlatformThreadId;
#endif

#include <stdio.h>
#include <time.h>
#include <iomanip>
#include <sstream>

inline struct tm localtime(const time_t &time_tt) {
    struct tm tm;
#ifdef _WIN32
    localtime_s(&tm, &time_tt);
#else
    localtime_r(&time_tt, &tm);
#endif
    return tm;
}

inline std::string SysTimeToString(bool use_usecs = true) {
    auto now_time = std::chrono::system_clock::now();
    auto tm_time = localtime(std::chrono::system_clock::to_time_t(now_time));
    auto usecs = std::chrono::duration_cast<std::chrono::microseconds>(now_time.time_since_epoch()).count() % 1000000;
    std::ostringstream oss;
    oss << std::put_time(&tm_time, "%F %T.")
        << std::setfill('0')
        << (use_usecs ? std::setw(6) : std::setw(3))
        << (use_usecs ? usecs : usecs / 1000);
    return oss.str();
}

inline PlatformThreadId CurrentThreadId() {
#if defined(TLOG_OS_WIN)
  return GetCurrentThreadId();
#elif defined(TLOG_OS_POSIX)
# if defined(TLOG_OS_MAC) || defined(TLOG_OS_IOS)
  return pthread_mach_thread_np(pthread_self());
# elif defined(TLOG_OS_ANDROID)
  return gettid();
# elif defined(TLOG_OS_LINUX)
  return syscall(__NR_gettid);
# else
  // Default implementation for nacl and solaris.
  return reinterpret_cast<pid_t>(pthread_self());
# endif
#endif
}

inline const char* FilenameFromPath(const char* file) {
    const char* end1 = ::strrchr(file, '/');
    const char* end2 = ::strrchr(file, '\\');
    if (!end1 && !end2)
        return file;
    else
        return (end1 > end2) ? end1 + 1 : end2 + 1;
}

inline void StringAppendV(std::string* dst, const char* format, va_list ap) {
    // First try with a small fixed size buffer
    static const int kSpaceLength = 1024;
    char space[kSpaceLength];

    // It's possible for methods that use a va_list to invalidate
    // the data in it upon use.  The fix is to make a copy
    // of the structure before using it and use that copy instead.
    va_list backup_ap;
    va_copy(backup_ap, ap);
    int result = vsnprintf(space, kSpaceLength, format, backup_ap);
    va_end(backup_ap);

    if (result < kSpaceLength) {
        if (result >= 0) {
            // Normal case -- everything fit.
            dst->append(space, result);
            return;
        }

#ifdef _MSC_VER
        {
            // Error or MSVC running out of space.  MSVC 8.0 and higher
            // can be asked about space needed with the special idiom below:
            va_copy(backup_ap, ap);
            result = vsnprintf(nullptr, 0, format, backup_ap);
            va_end(backup_ap);
        }
#endif

        if (result < 0) {
            // Just an error.
            return;
        }
    }

    // Increase the buffer size to the size requested by vsnprintf,
    // plus one for the closing \0.
    int length = result + 1;
    char* buf = new char[length];

    // Restore the va_list before we use it again
    va_copy(backup_ap, ap);
    result = vsnprintf(buf, length, format, backup_ap);
    va_end(backup_ap);

    if (result >= 0 && result < length) {
        // It fit
        dst->append(buf, result);
    }
    delete[] buf;
}

inline std::string StringPrintf(const char* format, ...) {
    va_list ap;
    va_start(ap, format);
    std::string result;
    StringAppendV(&result, format, ap);
    va_end(ap);
    return result;
}

#if ENABLE_TLOG

// log format: [timestamp][thread_id][level](filename:line): user_msg
//#define LOG_FMT "[%s][%llu][%s](%s:%d): %s\n"

#define TLOG_FORMAT "[%s][%ld]: %s"

#define TEXT(x) #x

#ifdef __PRETTY_FUNCTION__
# define TLOG_FUNCTION __PRETTY_FUNCTION__
#else
# define TLOG_FUNCTION __FUNCTION__
#endif

#if defined(TLOG_OS_ANDROID)
#include <android/log.h>
#define TLOG_TAG "tlog"
#endif

#if defined(__ANDROID__)
#define _TLOG_PRINT(level, ...) \
    __android_log_print(ANDROID_LOG_##level, TLOG_TAG, TLOG_FORMAT, SysTimeToString().c_str(), (long)CurrentThreadId(), StringPrintf(__VA_ARGS__).c_str());
#else
#define _TLOG_PRINT(level, ...) \
    printf(TLOG_FORMAT "\n", SysTimeToString().c_str(), (long)CurrentThreadId(), StringPrintf(__VA_ARGS__).c_str());
#endif

#define TLOG_PRINT(...) _TLOG_PRINT(INFO, __VA_ARGS__)

#else

#define TLOG_PRINT(...)

#endif

#endif /* tlog_print_h */
