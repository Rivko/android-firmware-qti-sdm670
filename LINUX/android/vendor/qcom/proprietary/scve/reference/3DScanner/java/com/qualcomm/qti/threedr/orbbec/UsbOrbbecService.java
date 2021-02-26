/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.threedr.orbbec;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.IBinder;
import android.support.annotation.Nullable;
import android.util.Log;

public class UsbOrbbecService extends Service {
    private static final String TAG = UsbOrbbecService.class.getCanonicalName();

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        final Context context = this;
        Thread thread = new Thread(new Runnable() {
            @Override
            public void run() {
                OBNativeHelper obNativeHelper = OBNativeHelper.getInstance(context);
                obNativeHelper.openOBSensor();
                obNativeHelper.readOBCmosParams();
                obNativeHelper.closeOBSensor();
                Log.i(TAG, "complted updating calibration LR/HR info for orbbec usb plugin");
            }
        });
        thread.start();
        return super.onStartCommand(intent, flags, startId);
    }
}
