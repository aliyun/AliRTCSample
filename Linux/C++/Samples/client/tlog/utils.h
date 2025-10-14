#ifndef __TLOG_UTILS_H__
#define __TLOG_UTILS_H__

#include <stdint.h>

#include <chrono>
#include <string>
#include <vector>

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

template<typename T, size_t N>
char (&ArraySizeHelper(T (&array)[N]))[N];

template <typename T, size_t N>
char (&ArraySizeHelper(const T (&array)[N]))[N];

#define arraysize(array) (sizeof(ArraySizeHelper(array)))

namespace tlog {

// ---------- os info utilities ----------
std::string OsName();
std::string OsVersion();

// ---------- time utilities ----------

static const int64_t kNumMillisecsPerSec = INT64_C(1000);
static const int64_t kNumMicrosecsPerSec = INT64_C(1000000);
static const int64_t kNumNanosecsPerSec = INT64_C(1000000000);

static const int64_t kNumMicrosecsPerMillisec = kNumMicrosecsPerSec / kNumMillisecsPerSec;
static const int64_t kNumNanosecsPerMillisec = kNumNanosecsPerSec / kNumMillisecsPerSec;
static const int64_t kNumNanosecsPerMicrosec = kNumNanosecsPerSec / kNumMicrosecsPerSec;

// for monotonic time measurements, not current system time
int64_t SystemTimeNanos();

inline int64_t TimeMillis() {
    return SystemTimeNanos() / kNumNanosecsPerMillisec;
}

inline int64_t TimeDiff(int64_t later, int64_t earlier) {
    return later - earlier;
}

inline int64_t TimeAfter(int64_t elapsed) {
    return TimeMillis() + elapsed;
}

inline int64_t TimeSince(int64_t earlier) {
    return TimeMillis() - earlier;
}

// The number of milliseconds that will elapse between now and 'later'.
inline int64_t TimeUntil(int64_t later) {
    return later - TimeMillis();
}

inline int64_t CurrentSystemTimeMillis() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

// if high_precision, suffix with us, else with ms
std::string DateTimePretty(bool high_precision = true);
std::string DateTimeFormat(const char *format = "%Y%m%d%H%M%S");


// ---------- string utilities ----------

void StringAppendV(std::string* dst, const char* format, va_list ap);
std::string StringPrintf(const char* format, ...);

std::vector<std::string> SplitString(const std::string& str, char delimiter);

// Return the filename portion of the string (that following the last slash).
const char* FilenameFromPath(const char* file);

// ---------- file system utilities ----------

bool CreateDirectory(const char *path);
bool CreateDirectoryTree(const char *path);

std::string CurrentWorkingDirectory();
std::string GetExecutablePath();

// ---------- byte io utilities ----------
inline uint16_t ReadU16BE(const uint8_t *data) {
    return ((uint16_t)data[0] << 8) |
           ((uint16_t)data[1]);
}

inline uint16_t ReadU16LE(const uint8_t* data) {
    return ((uint16_t)data[1] << 8) |
           ((uint16_t)data[0]);
}

inline uint32_t ReadU32BE(const uint8_t *data) {
    return ((uint32_t)data[0] << 24) |
           ((uint32_t)data[1] << 16) |
           ((uint32_t)data[2] << 8) |
           ((uint32_t)data[3]);
}

inline uint32_t ReadU32LE(const uint8_t* data) {
    return ((uint32_t)data[3] << 24) |
           ((uint32_t)data[2] << 16) |
           ((uint32_t)data[1] << 8) |
           ((uint32_t)data[0]);
}

inline uint64_t ReadU64BE(const uint8_t *data) {
    return ((uint64_t)data[0] << 56) |
           ((uint64_t)data[1] << 48) |
           ((uint64_t)data[2] << 40) |
           ((uint64_t)data[3] << 32) |
           ((uint64_t)data[4] << 24) |
           ((uint64_t)data[5] << 16) |
           ((uint64_t)data[6] << 8) |
           ((uint64_t)data[7]);
}

inline uint64_t ReadU64LE(const uint8_t* data) {
    return ((uint64_t)data[7] << 56) |
           ((uint64_t)data[6] << 48) |
           ((uint64_t)data[5] << 40) |
           ((uint64_t)data[4] << 32) |
           ((uint64_t)data[3] << 24) |
           ((uint64_t)data[2] << 16) |
           ((uint64_t)data[1] << 8) |
           ((uint64_t)data[0]);
}

inline void WriteU32BE(uint8_t* data, uint32_t val) {
    data[0] = val >> 24;
    data[1] = val >> 16;
    data[2] = val >> 8;
    data[3] = val;
}

inline void WriteU32LE(uint8_t* data, uint32_t val) {
    data[3] = val >> 24;
    data[2] = val >> 16;
    data[1] = val >> 8;
    data[0] = val;
}

inline void WriteU64BE(uint8_t* data, uint64_t val) {
    data[0] = val >> 56;
    data[1] = val >> 48;
    data[2] = val >> 40;
    data[3] = val >> 32;
    data[4] = val >> 24;
    data[5] = val >> 16;
    data[6] = val >> 8;
    data[7] = val;
}

inline void WriteU64LE(uint8_t* data, uint64_t val) {
    data[7] = val >> 56;
    data[6] = val >> 48;
    data[5] = val >> 40;
    data[4] = val >> 32;
    data[3] = val >> 24;
    data[2] = val >> 16;
    data[1] = val >> 8;
    data[0] = val;
}

// ---------- Other utilities ----------
// generate a random integer in range [a, b)
int RandomInt(int range_start, int range_end);
std::string RandomString(int length);

// network
bool IsValidIPv4Address(const char* ip_str);
bool IsValidPort(int port);

} // namespace tlog

#endif // __TLOG_UTILS_H__
