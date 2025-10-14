#ifndef __TLOG_CHECK_H__
#define __TLOG_CHECK_H__

#include "tlog.h"

#define DCHECK(condition) \
    if (!(condition)) TLOGF("Check failed: " # condition);

#endif // __TLOG_CHECK_H__
