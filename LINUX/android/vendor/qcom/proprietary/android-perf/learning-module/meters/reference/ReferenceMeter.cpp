/******************************************************************************
  @file    ReferenceMeter.cpp
  @brief   Implementation of Android Framework Learning Module

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#include "ReferenceMeter.h"
using namespace std;
MeterRegistry<ReferenceMeter> ReferenceMeter::registerMeter("RefMeter");

ReferenceMeter::ReferenceMeter(string meterName) : Meter(meterName) {

}

ReferenceMeter::ReferenceMeter(MeterInfo meterInfo): Meter(meterInfo) {

}

void ReferenceMeter::start() {
    std::time_t timeNow = std::time(nullptr);
    if (timeNow != (std::time_t)(-1)) {
        DEBUGV("ReferenceMeter","time is %ld", timeNow);
        mTimeField = timeNow;
    } else {
        mTimeField = 0;
    }
}
