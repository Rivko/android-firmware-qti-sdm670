/*===========================================================================
                           keep_alive_driver_proxy.h

 DESCRIPTION: Interface to Keep Alive Driver Proxy

 Copyright (c) 2015 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 =============================================================================*/

#ifndef __KEEP_ALIVE_DRIVER_PROXY_H__
#define __KEEP_ALIVE_DRIVER_PROXY_H__

/*-----------------------------------------------------------------------------
  Include Files
-----------------------------------------------------------------------------*/
#include "usf_proximity_driver.h"
#include <pthread.h>

/*-----------------------------------------------------------------------------
  Classes
 -----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
  Classes
 -----------------------------------------------------------------------------*/

/**
  Keep Alive Driver Proxy:
  Acts as a proxy of the driver. Listens for callbacks from the driver, which
  are intended to originate from the daemon, to be "kept alive". If in this
  keep-alive state, a "stop" will not really stop the underlying driver. Later,
  if in this "delayed stop" state and the keep-alive becomes false, the
  driver will really be stopped.

  This class must also interpret the poll command to send back the "first
  proximity" data when re-started in keep-alive state, since the driver won't
  really be started and so the first event must be simulated.
*/
class KeepAliveDriverProxy : public DaemonDriver {

enum DriverState{
    RUNNING = 0,
    STOPPED = 1
};

public:
    KeepAliveDriverProxy(UsfProximityDriver *pDriver);
    virtual ~KeepAliveDriverProxy();
    virtual int start(int sensorHandle);
    virtual int stop();
    virtual int poll(int *distance);
    virtual int flush();
private:
    static void keepAliveHook(void *pThis, int keepAliveValue);
    void onKeepAliveChanged(int keepAliveValue);
    void ensurePollThreadStarted();
    static void *pollThreadHook(void *pThis);
    void pollThreadTask();
    static void *stopDriverHook(void *pThis);
    void stopDriverTask();
    UsfProximityDriver *proxiedDriver;
    bool keepDriverAlive;
    DriverState requestedDriverState;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    pthread_t pollThread;
    bool hasUnreadProximityData;
    int lastProximityData;
    bool flushRequested;
};

#endif /* __KEEP_ALIVE_DRIVER_PROXY_H__ */
