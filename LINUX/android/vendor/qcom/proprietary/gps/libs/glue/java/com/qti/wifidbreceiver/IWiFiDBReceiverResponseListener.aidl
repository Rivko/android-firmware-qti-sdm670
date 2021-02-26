/* ======================================================================
*  Copyright (c) 2017 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*  ====================================================================*/

package com.qti.wifidbreceiver;

import java.com.qti.wifidbreceiver.APInfo;
import java.util.List;

oneway interface IWiFiDBReceiverResponseListener {

    void onAPListAvailable(in List<APInfo> apInfoList);

    void onStatusUpdate(in boolean isSuccess, in String error);

    void onServiceRequest();
}
