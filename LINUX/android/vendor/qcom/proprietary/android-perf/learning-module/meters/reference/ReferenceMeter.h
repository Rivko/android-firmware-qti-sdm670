/******************************************************************************
  @file    ReferenceMeter.h
  @brief   Implementation of Android Framework Learning Module

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef REFERENCEMETER_H
#define REFERENCEMETER_H

#include <ctime>
#include "Meter.h"
#include "DebugLog.h"
#include "MeterRegistry.h"

class ReferenceMeter: public Meter{
private:
    std::time_t mTimeField;
public:
    ReferenceMeter(std::string meterName);
    ReferenceMeter(MeterInfo meterInfo);

    ~ReferenceMeter(){}
    void start();
    std::time_t getTime() {
        return mTimeField;
    }
    static MeterRegistry<ReferenceMeter> registerMeter;
};

#endif /* REFERENCEMETER_H */
