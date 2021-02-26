#ifndef QURT_RW_MUTEX_H
#define QURT_RW_MUTEX_H

#include "qurt_mutex.h"

/* TODO: make this a real rw lock */

#define RW_MUTEX_T                  qurt_mutex_t

#define RW_MUTEX_CTOR(mut)          qurt_mutex_init(& (mut))
                                        
#define RW_MUTEX_LOCK_READ(mut)     qurt_mutex_lock(& (mut))

#define RW_MUTEX_UNLOCK_READ(mut)   qurt_mutex_unlock(& (mut))

#define RW_MUTEX_LOCK_WRITE(mut)    qurt_mutex_lock(& (mut))

#define RW_MUTEX_UNLOCK_WRITE(mut)  qurt_mutex_unlock(& (mut))

#define RW_MUTEX_DTOR(mut)          qurt_mutex_destroy(& (mut))


#endif //QURT_RW_MUTEX_H
