/**
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.wrapper;

import android.telephony.SubscriptionManager;

public class SubscriptionManager_Wrapper {

    public static int[] getSubId(int slotId) {
        return SubscriptionManager.getSubId(slotId);
    }

}
