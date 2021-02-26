/******************************************************************************
  @file    MeterReceiver.h
  @brief   Implementation of Android Framework Learning Module

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef FWK_METER_RECEIVER_H
#define FWK_METER_RECEIVER_H

class Meter;

class MeterReceiver {
public:
    virtual void notifyReceiver(Meter *m)=0;
    /**
     * Destroy the MeterReceiver.
     */
    virtual ~MeterReceiver(){};
};


#endif

