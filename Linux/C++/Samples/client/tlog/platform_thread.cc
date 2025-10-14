#include "platform_thread.h"

#if defined(TLOG_OS_WIN)
#elif defined(TLOG_OS_LINUX)
#include <unistd.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#endif

namespace tlog {

PlatformThreadId CurrentThreadId() {
#if defined(TLOG_OS_WIN)
    return GetCurrentThreadId();
#elif defined(TLOG_OS_POSIX)
#if defined(TLOG_OS_MAC) || defined(TLOG_OS_IOS)
    return pthread_mach_thread_np(pthread_self());
#elif defined(TLOG_OS_ANDROID)
    return gettid();
#elif defined(TLOG_OS_LINUX)
    return syscall(__NR_gettid);
#else
    // Default implementation for nacl and solaris.
    return reinterpret_cast<pid_t>(pthread_self());
#endif
#endif // defined(TLOG_OS_POSIX)
}

void SetCurrentThreadName(const char* name) {
#if defined(TLOG_OS_WIN)
    typedef HRESULT(WINAPI* Func_SetThreadDescription)(HANDLE hThread,
                                                  PCWSTR lpThreadDescription);
  // The SetThreadDescription API works even if no debugger is attached.
  // The names set with this API also show up in ETW traces. Very handy.
  static auto set_thread_description_func =
      reinterpret_cast<Func_SetThreadDescription>(::GetProcAddress(
          ::GetModuleHandleA("Kernel32.dll"), "SetThreadDescription"));
  if (set_thread_description_func) {
    // Convert from ASCII to UTF-16.
    wchar_t wide_thread_name[64];
    for (size_t i = 0; i < arraysize(wide_thread_name) - 1; ++i) {
      wide_thread_name[i] = name[i];
      if (wide_thread_name[i] == L'\0')
        break;
    }
    // Guarantee null-termination.
    wide_thread_name[arraysize(wide_thread_name) - 1] = L'\0';
    set_thread_description_func(::GetCurrentThread(), wide_thread_name);
  }

  // For details see:
  // https://docs.microsoft.com/en-us/visualstudio/debugger/how-to-set-a-thread-name-in-native-code
#pragma pack(push, 8)
  struct {
    DWORD dwType;
    LPCSTR szName;
    DWORD dwThreadID;
    DWORD dwFlags;
  } threadname_info = {0x1000, name, static_cast<DWORD>(-1), 0};
#pragma pack(pop)

#pragma warning(push)
#pragma warning(disable : 6320 6322)
  __try {
    ::RaiseException(0x406D1388, 0, sizeof(threadname_info) / sizeof(ULONG_PTR),
                     reinterpret_cast<ULONG_PTR*>(&threadname_info));
  } __except (EXCEPTION_EXECUTE_HANDLER) {  // NOLINT
  }
#pragma warning(pop)
#elif defined(TLOG_OS_LINUX) || defined(TLOG_OS_ANDROID)
  prctl(PR_SET_NAME, reinterpret_cast<unsigned long>(name));  // NOLINT
#elif defined(TLOG_OS_MAC) || defined(TLOG_OS_IOS)
  pthread_setname_np(name);
#endif
}

}  // namespace tlog