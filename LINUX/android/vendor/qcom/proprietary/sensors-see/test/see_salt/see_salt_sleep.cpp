/* ===================================================================
** Copyright (c) 2017 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: see_salt_sleep.cpp
** DESC: sleep_and_awake() called by main thread to sleep for
**       milliseconds then if necessary wakeup the apps processor
**       from suspend mode and resume.
** ================================================================ */
#include <pthread.h>
#include <stdbool.h>
#include "LocTimer.h"

typedef struct rendez {
    pthread_mutex_t mutx;
    pthread_cond_t  cond;
    bool wait = false;
} rendezvous;

/*
** rv - inter-thread rendevous
*/
rendezvous rv;

static void InitRV( rendezvous *rv)
{
    pthread_mutex_init( &rv->mutx, NULL );
    pthread_cond_init( &rv->cond, NULL );
}

/**
 * @name    PostRv
 * @brief   post completion event
 * @param   pointer to rendevous
 */
static void PostRV( rendezvous *rv)
{
    pthread_mutex_lock( &rv->mutx);
    rv->wait = false;
    pthread_cond_signal( &rv->cond);
    pthread_mutex_unlock( &rv->mutx);
}

/**
 * @name    WaitRv
 * @brief   wait for completion event
 * @param   pointer to rendevous
 */
static int WaitRV( rendezvous *rv)
{
    pthread_mutex_lock( &rv->mutx);
    rv->wait = true;
    int rc = 0;
    while ( rc == 0 && rv->wait) {
        rc = pthread_cond_wait( &rv->cond, &rv->mutx);
    }
    pthread_mutex_unlock( &rv->mutx);
    return rc;
}

class Timer : public LocTimer {
public:
    virtual void timeOutCallback();
};

void Timer::timeOutCallback( void)
{
   PostRV( &rv);
}

/**
 * @brief sleep_and_awake() called by main thread to sleep for milliseconds then
 *        if necessary, wakeup the apps processor from suspend mode
 * @param milliseconds
 */
void sleep_and_awake( uint32_t milliseconds )
{
   Timer lt;

   InitRV( &rv);
   if ( lt.start( milliseconds, true)) {
      WaitRV( &rv);
      lt.stop();
   }
}


