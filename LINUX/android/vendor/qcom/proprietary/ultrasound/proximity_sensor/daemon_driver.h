/*===========================================================================
                           daemon_driver.h

DESCRIPTION: Interface for daemon drivers

Copyright (c) 2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef __DAEMON_DRIVER_H__
#define __DAEMON_DRIVER_H__

/*-----------------------------------------------------------------------------
  Classes
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
  Macros and constants
-----------------------------------------------------------------------------*/
static const int POLL_RETVAL_DATA = 0;
static const int POLL_RETVAL_FLUSH = 1;
/**
  Interface for daemon drivers
*/
class DaemonDriver {
public:
    virtual int start(int sensorHandle) = 0;
    virtual int stop() = 0;
    virtual int poll(int *distance) = 0;
    virtual int flush() = 0;
    virtual ~DaemonDriver() { }
};

#endif /* __DAEMON_DRIVER_H__ */
