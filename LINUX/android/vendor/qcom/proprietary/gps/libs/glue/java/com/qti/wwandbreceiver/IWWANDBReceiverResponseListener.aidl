/* ======================================================================
*  Copyright (c) 2017 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*  ====================================================================*/

package com.qti.wwandbreceiver;

import java.com.qti.wwandbreceiver.BSInfo;
import java.util.List;

oneway interface IWWANDBReceiverResponseListener {

    void onBSListAvailable(in List<BSInfo> bsInfoList);

    void onStatusUpdate(in boolean isSuccess, in String error);

    void onServiceRequest();
}
