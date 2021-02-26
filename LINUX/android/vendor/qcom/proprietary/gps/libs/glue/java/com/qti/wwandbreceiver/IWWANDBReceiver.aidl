/* ======================================================================
*  Copyright (c) 2017 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*  ====================================================================*/

package com.qti.wwandbreceiver;

import java.com.qti.wwandbreceiver.IWWANDBReceiverResponseListener;
import java.com.qti.wwandbreceiver.BSLocationData;
import java.com.qti.wwandbreceiver.BSSpecialInfo;
import java.util.List;

interface IWWANDBReceiver {

    boolean registerResponseListener(in IWWANDBReceiverResponseListener callback);

    int requestBSList (in int expireInDays);

    int pushWWANDB (in List<BSLocationData> locationData,
                    in List<BSSpecialInfo> specialData,
                    in int daysValid);

    void removeResponseListener(in IWWANDBReceiverResponseListener callback);
}
