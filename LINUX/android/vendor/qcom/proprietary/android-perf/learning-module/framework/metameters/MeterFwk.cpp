/******************************************************************************
  @file    Meter.cpp
  @brief   Implementation of Android Framework Learning Module

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#include "inc/MeterFwk.h"
#include "ThreadHelper.h"

#define LOG_TAG_METER "Meter"
using namespace std;

MeterFwk::MeterFwk(){
    receiver = NULL;
    meterHelper = NULL;
    asyncData = NULL;
    stopMeter = false;
}

bool MeterFwk::wakeUp(void *data) {
    if(!meterHelper) {
        return false;
    }

    bool readiness = meterHelper->isReady();
    if(readiness) {
        meterHelper->wakeUp();
    } else {
        DEBUGE(LOG_TAG_METER," Failed to wakeUp Meter. (helper,readiness)=(%p,%d)\n", meterHelper, (int)readiness);
    }
    return readiness;
}
