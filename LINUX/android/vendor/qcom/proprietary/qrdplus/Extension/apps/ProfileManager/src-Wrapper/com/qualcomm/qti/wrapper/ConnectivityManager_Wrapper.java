/**
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.wrapper;

import android.content.Context;
import android.net.ConnectivityManager;

public class ConnectivityManager_Wrapper {
    private ConnectivityManager mConnectivityManager;

    public ConnectivityManager_Wrapper(Context context) {
        mConnectivityManager = (ConnectivityManager) context.getSystemService(
                Context.CONNECTIVITY_SERVICE);
    }

    public boolean getMobileDataEnabled() {
        if (mConnectivityManager != null) {
            return mConnectivityManager.getMobileDataEnabled();
        } else {
            return false;
        }
    }
}
