/******************************************************************************
  @file    Meter.h
  @brief   Implementation of Android Framework Learning Module

  DESCRIPTION
        Meter is an abstract base class of Meter
        Every meter should implement start() method for this class.
        start() indicates the start of the meter will be invoked in
        a separate thread context asynchronously.

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef FWK_METER_H
#define FWK_METER_H

#include <string>
#include <vector>

#include "AsyncData.h"
#include "DebugLog.h"
#include "FeatureInfo.h"

class ThreadHelper;
class Meter;
class MeterFwk;

/**
 * Meter is an Abstract class.
 * Every Meter should extend this class & implement start() method.
 * Ex: A memory meter can extend this class & implement start() method to collect the required stats.
 * The start() method will be triggered when needed based on Async triggers and feature currently choosen for learning.
 * The start() method when triggered will have a dedicated execution context.
 */
class Meter {
private:
    std::string mMeterName;
    MeterFwk* mMeterFwk;
    std::vector<AsyncTriggerPayload> asyncUpdates;
protected:
    bool hasStop;
public:
    MeterInfo mInfo;
    /**
     * Default construction.
     */
    Meter();

    /**
     * Constructing Meter via this method can only provide a separate execution.
     * This method can not send the collected stats to MeterMgr which uses Meter statistics.
     * @param meterName - Name of the meter.
     */
    Meter(std::string meterName);

    /**
     * Constructing Meter via this method can only provide a separate execution.
     * This method can not send the collected stats to MeterMgr which uses Meter statistics.
     * @param meterInfo - MeterInfo object containing the meter details.
     */
    Meter(MeterInfo meterInfo);

    /**
     * Abstarct Method. Must be implemented by appropriate Meter classes.
     * Meters should implement logic to collect the required stats in this method
     * The collected stats should not released untill the same start() method is triggered again / destructor is invoked.
     */
    virtual void start()=0;

    /**
     * All Async updates for a given Meter are delivered here.
     * Also acts as a stop notification by issuing default Async Trigger
     * The default implementation will just append all the AsyncTriggerPayload and will be returned in getAsyncUpdates()
     * If Meter's choose to override this method for some state updates, they should return immediately as this run in the context of LM core thread.
     */
     // TODO :: Invok this method in a separate context.
    virtual void asyncTriggerUpdate(const AsyncTriggerPayload& asyncUpdate);

    /**
     * Returns the list of AsyncUpdates available on this Meter.
     */
    virtual std::vector<AsyncTriggerPayload>& getAsyncUpdates();
    /**
     * An intermitent check point.
     * Should be used by the Meters to check if they should stop collecting logs, do the necessary cleanup and return from start().
     * Usually this method can be invoked from start() method to stop the current execution.
     */
    bool hasStopNotification();

    /**
     * Destroy the Meter. Will be invoked by LM.
     */
    virtual ~Meter();
    MeterFwk* getMeterFwk() { return mMeterFwk;}
    std::string getMeterName(){return mMeterName;}
};


#endif
