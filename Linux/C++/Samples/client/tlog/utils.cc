#include "utils.h"

#include "config.h"

#if defined(TLOG_OS_POSIX)
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <arpa/inet.h>
#include <unistd.h>
#if defined(TLOG_OS_LINUX)
#include <linux/limits.h>
#endif
#if defined(TLOG_OS_MAC)
#include <mach/mach_time.h>
#endif
#endif

#include <errno.h>
#include <stdarg.h>
#include <string.h>

#include <chrono>
#include <iomanip>
#include <sstream>
#include <random>

namespace tlog {

// ---------- os info utilities ----------
std::string OsName() {
#if defined(TLOG_OS_POSIX)
    struct utsname info;
    if (uname(&info) < 0) {
        return std::string();
    }
    return std::string(info.sysname);
#else
#error "TODO: implements OsName"
    return std::string();
#endif
}

std::string OsVersion() {
#if defined(TLOG_OS_POSIX)
    struct utsname info;
    if (uname(&info) < 0) {
        return std::string();
    }
    return std::string(info.release);
#else
#error "TODO: implements OsName"
    return std::string();
#endif
}

// ---------- time utilities ----------

int64_t SystemTimeNanos() {
    int64_t ticks;
#if defined(TLOG_OS_MAC)
    static mach_timebase_info_data_t timebase;
    if (timebase.denom == 0) {
        // Get the timebase if this is the first time we run.
        // Recommended by Apple's QA1398.
        if (mach_timebase_info(&timebase) != KERN_SUCCESS) {
            //      RTC_DCHECK_NOTREACHED();
        }
    }
    // Use timebase to convert absolute time tick units into nanoseconds.
    const auto mul = [](uint64_t a, uint32_t b) -> int64_t {
        //TLOG_DCHECK_NE(b, 0);
        return static_cast<int64_t>(a * b);
    };
    ticks = mul(mach_absolute_time(), timebase.numer) / timebase.denom;
#elif defined(TLOG_OS_POSIX)
    struct timespec ts;
    // TODO(deadbeef): Do we need to handle the case when CLOCK_MONOTONIC is not
    // supported?
    clock_gettime(CLOCK_MONOTONIC, &ts);
    ticks = kNumNanosecsPerSec * static_cast<int64_t>(ts.tv_sec) + static_cast<int64_t>(ts.tv_nsec);
#elif defined(WINUWP)
    ticks = WinUwpSystemTimeNanos();
#elif defined(TLOG_OS_WIN)
    // TODO(webrtc:14601): Fix the volatile increment instead of suppressing the
    // warning.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-volatile"
    static volatile LONG last_timegettime = 0;
    static volatile int64_t num_wrap_timegettime = 0;
    volatile LONG* last_timegettime_ptr = &last_timegettime;
    DWORD now = timeGetTime();
    // Atomically update the last gotten time
    DWORD old = InterlockedExchange(last_timegettime_ptr, now);
    if (now < old) {
        // If now is earlier than old, there may have been a race between threads.
        // 0x0fffffff ~3.1 days, the code will not take that long to execute
        // so it must have been a wrap around.
        if (old > 0xf0000000 && now < 0x0fffffff) {
            num_wrap_timegettime++;
        }
    }
    ticks = now + (num_wrap_timegettime << 32);
    // TODO(deadbeef): Calculate with nanosecond precision. Otherwise, we're
    // just wasting a multiply and divide when doing Time() on Windows.
    ticks = ticks * kNumNanosecsPerMillisec;
#pragma clang diagnostic pop
#else
#error Unsupported platform.
#endif
    return ticks;
}

std::tm localtime(const std::time_t &time_tt) {
    std::tm tm;
#ifdef _WIN32
    localtime_s(&tm, &time_tt);
#else
    localtime_r(&time_tt, &tm);
#endif
    return tm;
}

std::string DateTimePretty(bool high_precision) {
    auto now_time = std::chrono::system_clock::now();
    auto tm_time = localtime(std::chrono::system_clock::to_time_t(now_time));
    std::ostringstream oss;
    oss << std::put_time(&tm_time, "%Y-%m-%d %H:%M:%S.");
    if (high_precision) {
        auto usecs = std::chrono::duration_cast<std::chrono::microseconds>(now_time.time_since_epoch()).count() % 1000000;
        oss << std::setfill('0') << std::setw(6) << usecs;
    } else {
        auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(now_time.time_since_epoch()).count() % 1000;
        oss << std::setfill('0') << std::setw(3) << millis;
    }
    return oss.str();
}

#if 1
std::string DateTimeFormat(const char *format) {
    auto now_time = std::chrono::system_clock::now();
    auto tm_time = localtime(std::chrono::system_clock::to_time_t(now_time));
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(now_time.time_since_epoch()).count() % 1000;
    std::ostringstream oss;
    oss << std::put_time(&tm_time, format);
    return oss.str();
}
#else
std::string DateTimeFormat(const char *format) {
    std::time_t now = std::time(NULL);
    char buf[64] = {0};
    std::strftime(buf, sizeof(buf), format, localtime(now));
    return std::string(buf);
}
#endif

// ---------- string utilities ----------

#ifdef _MSC_VER
    #ifndef va_copy
        // Define va_copy for MSVC. This is a hack, assuming va_list is simply a
        // pointer into the stack and is safe to copy.
        #define va_copy(dest, src) ((dest) = (src))
    #endif
#endif

void StringAppendV(std::string* dst, const char* format, va_list ap) {
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

std::string StringPrintf(const char* format, ...) {
    va_list ap;
    va_start(ap, format);
    std::string result;
    StringAppendV(&result, format, ap);
    va_end(ap);
    return result;
}

std::vector<std::string> SplitString(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    if (str.empty())
        return result;

    size_t start = 0;
    while (start != std::string::npos) {
        size_t end = str.find_first_of(delimiter, start);

        std::string part;
        if (end == std::string::npos) {
            part = str.substr(start);
            start = std::string::npos;
        } else {
            part = str.substr(start, end - start);
            start = end + 1;
        }

        result.push_back(part);
    }
    return result;
}

// Return the filename portion of the string (that following the last slash).
const char* FilenameFromPath(const char* file) {
    const char* end1 = ::strrchr(file, '/');
    const char* end2 = ::strrchr(file, '\\');
    if (!end1 && !end2)
        return file;
    else
        return (end1 > end2) ? end1 + 1 : end2 + 1;
}

bool CreateDirectory(const char *path) {
    struct stat st;
    int ret = stat(path, &st);
    if (ret != 0) {
        ret = mkdir(path, 0755);
        if (ret != 0) {
            fprintf(stderr, "Error creating directory %s: %s\n", path, strerror(errno));
            return false;
        }
    } else if (!S_ISDIR(st.st_mode)) {
        fprintf(stderr, "%s exists but is not a directory\n", path);
        return false;
    }
    return true;
}

// /a/b/
// /a/b
// a/b/c/
// a/b/c

bool CreateDirectoryTree(const char *path) {
    char buf[1024] = {0};
    strncpy(buf, path, sizeof(buf));
    buf[sizeof(buf) - 1] = '\0';

    // add trailing slash
    int len = strlen(path);
    if (buf[len - 1] != '/') {
        buf[len] = '/';
    }

    for (char *p = buf + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            if (!CreateDirectory(buf)) {
                fprintf(stderr, "CreateDirectory failed, %s\n", buf);
                return false;
            } else {
                // printf("CreateDirectory ok, %s\n", buf);
            }
            *p = '/';
        }
    }
    return true;
}

std::string CurrentWorkingDirectory() {
    char cwd[PATH_MAX] = {0};
#if defined(TLOG_OS_POSIX)
    getcwd(cwd, sizeof(cwd));
#endif
    return std::string(cwd);
}

std::string GetExecutablePath() {
#if defined(TLOG_OS_POSIX)
    char exe_path[PATH_MAX + 1] = {0};
    ssize_t len = readlink("/proc/self/exe", exe_path, PATH_MAX);
    if (len > 0 && len <= PATH_MAX) {
        exe_path[len] = '\0';
        return std::string(exe_path);
    }
#endif
    return std::string();
}

// ---------- Other utilities ----------
int RandomInt(int range_start, int range_end) {
    // invalid range
    if (range_end <= range_start) {
        return range_start;
    }
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(range_start, range_end - 1);
    return distrib(gen);
}

std::string RandomString(int length) {
    static const char kPossibleChars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

    std::string random_string;
    for(int i = 0; i < length; ++i) {
        int index = RandomInt(0, arraysize(kPossibleChars));
        random_string.push_back(kPossibleChars[index]);
    }
    return random_string;
}

bool IsValidIPv4Address(const char* ip_str) {
    struct in_addr addr;
    return inet_pton(AF_INET, ip_str, &addr) == 1;
}

bool IsValidPort(int port) {
    return port > 0 && port < 65536;
}

} // namespace tlog
