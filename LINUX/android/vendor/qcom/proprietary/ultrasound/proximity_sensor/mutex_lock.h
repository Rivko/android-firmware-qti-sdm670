/*===========================================================================
                           mutex_lock.h

DESCRIPTION: Utility class for managing pthread mutex locks.

Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/
#ifndef __MUTEX_LOCK_H__
#define __MUTEX_LOCK_H__

#include "check_call.h"
#include <pthread.h>

/*-----------------------------------------------------------------------------
  Classes
-----------------------------------------------------------------------------*/
/**
  Utility class for managing pthread mutex via stack object.
*/
class MutexLock {
public:
    MutexLock(pthread_mutex_t &_mutex) :
            mutex(_mutex) {
        CHECK_CALL(pthread_mutex_lock(&mutex));
    }
    ~MutexLock() {
        CHECK_CALL(pthread_mutex_unlock(&mutex));
    }
private:
    pthread_mutex_t &mutex;
};

#endif /* __MUTEX_LOCK_H__ */
