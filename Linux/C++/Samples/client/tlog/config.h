#ifndef TLOG_CONFIG_H_
#define TLOG_CONFIG_H_

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64) || defined(__CYGWIN__)
    #define TLOG_OS_WIN
#elif defined(linux) || defined(__linux) || defined(__linux__)
    #define TLOG_OS_LINUX
    #if defined(__ANDROID__)
        #define TLOG_OS_ANDROID
    #endif
#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
    #define TLOG_OS_MAC
    #include <TargetConditionals.h>
    #if TARGET_OS_IPHONE // iOS, tvOS, watchOS, bridgeOS
        #if TARGET_OS_IOS
            #define TLOG_OS_IOS
        #else
            #warning "Not supported OS?"
        #endif
    #elif TARGET_OS_OSX // macOS
        #define TLOG_OS_OSX
    #endif
#else
    #warning "Unknown OS"
#endif

#if defined(TLOG_OS_LINUX) || defined(TLOG_OS_MAC)
    #define TLOG_OS_POSIX
#endif

#if defined(TLOG_SHARED_LIB)
    #if defined(_WIN32)
        #ifdef TLOG_EXPORTS
            #define TLOG_API __declspec(dllexport)
        #else  // !defined(TLOG_EXPORTS)
            #define TLOG_API __declspec(dllimport)
        #endif
    #else  // !defined(_WIN32)
        #define TLOG_API __attribute__((visibility("default")))
    #endif
#else  // !defined(TLOG_SHARED_LIB)
    #define TLOG_API
#endif

#endif  // TLOG_CONFIG_H_
