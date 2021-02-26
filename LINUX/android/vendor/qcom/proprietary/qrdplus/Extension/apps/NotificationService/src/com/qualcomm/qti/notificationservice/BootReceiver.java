/*
 * Copyright (c) 2015, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.notificationservice;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

public class BootReceiver extends BroadcastReceiver {
    private final static String TAG = "BootReceiver";

    @Override
    public void onReceive(Context context, Intent intent) {
        Log.d(TAG, "action:" + intent.getAction());
        boolean mShowBatteryOverTempratureWarning;
        mShowBatteryOverTempratureWarning = context.getResources().getBoolean(
                R.bool.config_showBatteryOverTemperatureWarning);
        String action = intent.getAction();
        if (mShowBatteryOverTempratureWarning && action.equals(Intent.ACTION_BOOT_COMPLETED)) {
            Intent service = new Intent(context, BootService.class);
            context.startService(service);
            Log.d(TAG, "Boot Complete. Starting BootService...");
        }
    }
}
