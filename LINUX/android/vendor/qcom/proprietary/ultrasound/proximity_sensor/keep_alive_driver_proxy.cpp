/*===========================================================================
                           keep_alive_driver_proxy.cpp

 DESCRIPTION: Proxy for driver that tries to keep it alive when requested

 Copyright (c) 2015 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 =============================================================================*/
#define LOG_TAG "keep_alive_driver_proxy"

/*-----------------------------------------------------------------------------
 Includes
 -----------------------------------------------------------------------------*/
#include "keep_alive_driver_proxy.h"
#include "check_call.h"
#include "mutex_lock.h"
#include <log/log.h>

/*-----------------------------------------------------------------------------
 Functions
 -----------------------------------------------------------------------------*/

/*============================================================================
 FUNCTION:  KeepAliveDriverProxy::KeepAliveDriverProxy
 ============================================================================*/
/**
 C'tor for KeepAliveDriverProxy. Registers for keep-alive callbacks.
 Initializes pthread objects.
 */
KeepAliveDriverProxy::KeepAliveDriverProxy(UsfProximityDriver *pDriver) :
        proxiedDriver(pDriver), keepDriverAlive(false), requestedDriverState(
                STOPPED), pollThread(0), hasUnreadProximityData(false), lastProximityData(
                -1), flushRequested(false) {
    proxiedDriver->registerKeepAliveCallback(keepAliveHook, this);
    CHECK_CALL(pthread_mutex_init(&mutex, NULL));
    CHECK_CALL(pthread_cond_init(&cond, NULL));
}

/*============================================================================
 FUNCTION:  KeepAliveDriverProxy::~KeepAliveDriverProxy
 ============================================================================*/
/**
 D'tor for KeepAliveDriverProxy.
 */
KeepAliveDriverProxy::~KeepAliveDriverProxy() {
    CHECK_CALL(pthread_mutex_destroy(&mutex));
    CHECK_CALL(pthread_cond_destroy(&cond));
}

/*============================================================================
 FUNCTION:  KeepAliveDriverProxy::keepAliveHook
 ============================================================================*/
/**
 Convenience hook for "keep-alive changed" callback
 */
void KeepAliveDriverProxy::keepAliveHook(void* pThis, int keepAliveValue) {
    ((KeepAliveDriverProxy*) pThis)->onKeepAliveChanged(keepAliveValue);
}

/*============================================================================
 FUNCTION:  KeepAliveDriverProxy::onKeepAliveChanged
 ============================================================================*/
/**
 Called from proxied driver. Conditionally starts a thread to stop the
 driver if trying to stop and no longer being kept alive.
 */
void KeepAliveDriverProxy::onKeepAliveChanged(int keepAliveValue) {
    MutexLock lock(mutex);
    keepDriverAlive = keepAliveValue == 1;
    if (!keepDriverAlive && requestedDriverState == STOPPED) {
        pthread_t stopperThread;
        CHECK_CALL(pthread_create(&stopperThread, NULL, stopDriverHook, this));
    }
}

/*============================================================================
 FUNCTION:  KeepAliveDriverProxy::stopDriverHook
 ============================================================================*/
/**
 Convenience hook for stop-driver thread.
 */
void *KeepAliveDriverProxy::stopDriverHook(void* pThis) {
    ((KeepAliveDriverProxy*) pThis)->stopDriverTask();
    return 0;
}

/*============================================================================
 FUNCTION:  KeepAliveDriverProxy::stopDriverTask
 ============================================================================*/
/**
 Launched from the keep-alive callback when trying to stop and no longer
 being kept alive. Checks conditions one more time, then really stops the
 driver.
 */
void KeepAliveDriverProxy::stopDriverTask() {
    MutexLock lock(mutex);
    if (!keepDriverAlive && requestedDriverState == STOPPED) {
        proxiedDriver->stop();
    }
}

/*============================================================================
 FUNCTION:  KeepAliveDriverProxy::start
 ============================================================================*/
/**
 Proxied start call. Really starts the driver if it wasn't running,
 otherwise signals the poll thread to allow the simulated "first proximity"
 data call.
 */
int KeepAliveDriverProxy::start(int sensorHandle) {
    MutexLock lock(mutex);
    requestedDriverState = RUNNING;
    if (keepDriverAlive) {
        CHECK_CALL(pthread_cond_broadcast(&cond));
        return 0; // driver is already running
    }
    return proxiedDriver->start(sensorHandle);
}

/*============================================================================
 FUNCTION:  KeepAliveDriverProxy::stop
 ============================================================================*/
/**
 Proxied stop call. Really stops the driver if it wasn't kept alive,
 otherwise returns success without actually stopping driver.
 */
int KeepAliveDriverProxy::stop() {
    MutexLock lock(mutex);
    requestedDriverState = STOPPED;
    if (keepDriverAlive) {
        return 0;
    } else {
        return proxiedDriver->stop();
    }
}

/*============================================================================
 FUNCTION:  KeepAliveDriverProxy::poll
 ============================================================================*/
/**
 Proxied poll call. Starts a poll loop that can return data either if it
 really comes from the driver, or if a kept-alive driver is restarted to
 simulate the "first proximity" data.
 */
int KeepAliveDriverProxy::poll(int* distance) {
    // if poll thread not started
    //   start poll thread
    // wait on:
    //   if poll result AND requestedState == RUNNING, return new result
    //
    MutexLock lock(mutex);
    ensurePollThreadStarted();
    while (requestedDriverState == STOPPED || (!hasUnreadProximityData && !flushRequested)) {
        CHECK_CALL(pthread_cond_wait(&cond, &mutex));
    }
    if (flushRequested) {
        flushRequested = false;
        return 1;
    }
    *distance = lastProximityData;
    hasUnreadProximityData = false;
    return 0;
}

/*============================================================================
 FUNCTION:  KeepAliveDriverProxy::ensurePollThreadStarted
 ============================================================================*/
/**
 Must be called with mutex held.
 Starts the poll thread if it hasn't already been started.
 */
void KeepAliveDriverProxy::ensurePollThreadStarted() {
    if (pollThread == 0) {
        CHECK_CALL(pthread_create(&pollThread, NULL, pollThreadHook, this));
    }
}

/*============================================================================
 FUNCTION:  KeepAliveDriverProxy::pollThreadHook
 ============================================================================*/
/**
 Convenience method for starting poll thread.
 */
void* KeepAliveDriverProxy::pollThreadHook(void* pThis) {
    ((KeepAliveDriverProxy*) pThis)->pollThreadTask();
    return 0;
}

/*============================================================================
 FUNCTION:  KeepAliveDriverProxy::pollThreadTask
 ============================================================================*/
/**
 Separate thread for monitoring real poll values from the proxied driver.
 Signals the poll call when data is received.
 */
void KeepAliveDriverProxy::pollThreadTask() {
    while (true) {
        int distance;
        int retval = proxiedDriver->poll(&distance);
        MutexLock lock(mutex);
        if (retval == POLL_RETVAL_DATA) {
            hasUnreadProximityData = true;
            lastProximityData = distance;
        } else if (retval == POLL_RETVAL_FLUSH) {
            flushRequested = true;
        } else {
            ALOGE("%s: Unknown retval %d from driver poll", __FUNCTION__, retval);
        }
        CHECK_CALL(pthread_cond_broadcast(&cond));
    }
}

/*============================================================================
 FUNCTION:  KeepAliveDriverProxy::flush
 ============================================================================*/
/**
 Proxied flush call.
 */
int KeepAliveDriverProxy::flush() {
    return proxiedDriver->flush();
}
