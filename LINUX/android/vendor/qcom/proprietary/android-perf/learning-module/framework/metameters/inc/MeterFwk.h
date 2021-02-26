/******************************************************************************
  @file    Meter.h
  @brief   Implementation of Android Framework Learning Module

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef FWK_METERFWK_H
#define FWK_METERFWK_H

#include <string>

#include "AsyncData.h"
#include "DebugLog.h"
#include "MeterReceiver.h"

class ThreadHelper;
class Meter;

class MeterFwk {
    ThreadHelper* meterHelper;
    MeterReceiver* receiver;
    AsyncTriggerPayload* asyncData;

    // This variable will be accessed in two different thread contexts.
    // 1. Meters will call hasStopNotification() after every sample collection
    // 2. Will be written from
    //      1) set to false(reset) in MeterMgr::start while starting meters.
    //      2) set to true, From MetaMeter ( LM Thread ) when there is a new request and we want to stop the current on going request.
    // 3. This only write protected.
    // 4. Read is intentially left unprotected as a tradeoff b/w
    //      1) Frequent access by every meter after each sample collection.
    //      2) Worst case overhead is one extra sample collection.
    bool stopMeter;

public:
    MeterFwk();
    virtual ~MeterFwk() {}

    virtual bool wakeUp(void *data);
    void setThreadHelper(ThreadHelper* helper) { meterHelper = helper;}
    void setReceiver(MeterReceiver* dataReceiver) { receiver = dataReceiver; }
    ThreadHelper* getThreadHelper() { return meterHelper; }
    MeterReceiver* getReceiver() {return receiver; }
};
#endif
