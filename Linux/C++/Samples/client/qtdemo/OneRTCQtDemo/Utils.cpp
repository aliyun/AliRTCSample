#include "Utils.h"

#include <sys/syscall.h>
#include <unistd.h>

#include <QtGlobal>

#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
#include <QRandomGenerator>
#else
#include "tlog/utils.h"
#endif

QString GetRandomString(int randomStringLength)
{
    const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");

    QString randomString;
    for(int i=0; i<randomStringLength; ++i)
    {
    #if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
        int index = QRandomGenerator::global()->bounded(0, possibleCharacters.length());
    #else
        int index = tlog::RandomInt(0, possibleCharacters.length());
    #endif
        QChar nextChar = possibleCharacters.at(index);
        randomString.append(nextChar);
    }
    return randomString;
}

PlatformThreadId CurrentThreadId()
{
    return syscall(__NR_gettid);
}
