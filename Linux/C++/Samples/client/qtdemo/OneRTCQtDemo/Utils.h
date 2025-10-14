#ifndef UTILS_H
#define UTILS_H

#include <pthread.h>

#include <QString>

#ifndef COUNTOF
#define COUNTOF(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif


QString GetRandomString(int randomStringLength);



typedef pid_t PlatformThreadId;

PlatformThreadId CurrentThreadId();

#endif // UTILS_H
