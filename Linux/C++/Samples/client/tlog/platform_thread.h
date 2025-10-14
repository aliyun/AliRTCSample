#ifndef TLOG_PLATFORM_THREAD_H_
#define TLOG_PLATFORM_THREAD_H_

#include "config.h"

#if defined(TLOG_OS_WIN)
#include <windows.h>
#elif defined(TLOG_OS_POSIX)
#include <pthread.h>
#include <unistd.h>
#elif defined(TLOG_OS_MAC)
#include <pthread_spis.h>
#endif

namespace tlog {

#if defined(TLOG_OS_WIN)
typedef DWORD PlatformThreadId;
#elif defined(TLOG_OS_POSIX)
typedef pid_t PlatformThreadId;
#endif

// Retrieve the ID of the current thread.
PlatformThreadId CurrentThreadId();

// Sets the current thread name.
void SetCurrentThreadName(const char* name);

} // namespace tlog

#endif // TLOG_PLATFORM_THREAD_H_
