/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.biometrics.voiceprinttest;

import android.app.Application;
import android.util.Log;

public class VoicePrintTestApp extends Application {
    private static final String TAG = "VoicePrintTestApp";

    @Override
    public void onCreate() {
        Log.d(TAG, "onCreate");
        super.onCreate();
    }
}
