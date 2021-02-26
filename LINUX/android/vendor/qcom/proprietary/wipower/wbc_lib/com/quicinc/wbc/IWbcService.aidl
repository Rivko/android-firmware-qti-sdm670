/*=========================================================================
  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  =========================================================================*/

package com.quicinc.wbc;

import com.quicinc.wbc.IWbcEventListener;

interface IWbcService {
    void echo(int val);

    int getWipowerCapable();
    int getPtuPresence();
    int getWipowerCharging();
    int getChargeComplete();
    int getUSBChargingPresent();
    int getBatteryOverheat();
    void register(in IWbcEventListener listener);
    void unregister(in IWbcEventListener listener);
}
