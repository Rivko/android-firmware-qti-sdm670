/**
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.wrapper;

import android.telephony.TelephonyManager;

public class TelephonyManager_Wrapper {
    private static TelephonyManager_Wrapper sInstance = new TelephonyManager_Wrapper();

    private TelephonyManager_Wrapper() {}

    public static TelephonyManager_Wrapper getDefaultWrapperInstance() {
        return sInstance;
    }

    public boolean isMultiSimEnabled() {
        return TelephonyManager.getDefault().isMultiSimEnabled();
    }

    public int getSimState(int slotIdx) {
        return TelephonyManager.getDefault().getSimState(slotIdx);
    }

    public void setDataEnabled(boolean enable) {
        TelephonyManager.getDefault().setDataEnabled(enable);
    }

    public static TelephonyManager getDefault() {
        return TelephonyManager.getDefault();
    }

}
