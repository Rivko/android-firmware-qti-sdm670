/*===========================================================================
                           usf_proximity_driver.h

 DESCRIPTION: Interface to USF Proximity Driver

 Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 =============================================================================*/

#ifndef __USF_PROXIMITY_DRIVER_H__
#define __USF_PROXIMITY_DRIVER_H__

/*-----------------------------------------------------------------------------
  Include Files
-----------------------------------------------------------------------------*/
#include <sys/un.h>
#include <sys/types.h>
#include <pthread.h>
#include "daemon_driver.h"
#include "usf_socket_receiver.h"

/*-----------------------------------------------------------------------------
  Macros and constants
-----------------------------------------------------------------------------*/
static const int SENSOR_HANDLE = 251;
static const int SENSOR_POCKET_HANDLE = 252;

/*-----------------------------------------------------------------------------
  Classes
 -----------------------------------------------------------------------------*/

/**
  USF Proximity Driver:
  Responsible for starting and stopping the proximity daemon, initiating a
  socket listening thread, and updating the blocking poll request from sensor
  HAL when there is new data.
*/
class UsfProximityDriver : public DaemonDriver {
public:
    UsfProximityDriver(UnSocketReceiver *socketArg);
    virtual ~UsfProximityDriver();
    virtual int start(int sensorHandle);
    virtual int stop();
    virtual int poll(int *distance);
    virtual int flush();
    virtual void registerKeepAliveCallback(void (callback)(void *, int), void *arg);
    static const int FAR_DISTANCE_MARKER; // output from poll when there's no proximity
protected:
    virtual int blockOnSocketOrWakeEvent(int sockFd);
    virtual void startDaemon();
    virtual void stopDaemon();
    virtual void doUsleep(useconds_t useconds);
    virtual void getDaemonState(char *state);
private:
    static void *socketReaderThreadHook(void *pThis);
    void onNewProximityValue(int newProximityValue);
    void socketReaderThread();
    int startDaemonAndConnectToSocket();
    void tearDownThread();
    int changeCfgLink();
    bool allowChangeCfgLink();

    UnSocketReceiver *socket;
    bool shutdownRequested;
    bool threadRunning;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int wakeSocketThreadEvent;
    int lastProximityValue;
    bool hasUnreadProximityData;
    pthread_t thread;
    void (*keepAliveCallback)(void *, int);
    void *keepAliveCallbackArg;
    bool flushRequested;
    static int sSensorHandle;
};

#endif /* __USF_PROXIMITY_DRIVER_H__ */
