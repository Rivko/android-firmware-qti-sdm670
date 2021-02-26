/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.extsettings;

import android.app.Application;

import com.qualcomm.qti.extsettings.sta.StaMonitor;


public class ExtSettingsApplication extends Application {

    private StaMonitor mStaMonitor;

    @Override
    public void onCreate() {
        super.onCreate();
        mStaMonitor = new StaMonitor(this);
        mStaMonitor.startMonitor();
    }

    @Override
    public void onTerminate() {
        super.onTerminate();
        mStaMonitor.stopMonitor();
    }

    public StaMonitor getStaMonitor() {
        return mStaMonitor;
    }
}
