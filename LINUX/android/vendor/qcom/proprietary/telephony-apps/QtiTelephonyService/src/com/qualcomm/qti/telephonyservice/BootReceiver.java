/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.telephonyservice;

import android.app.ActivityManager;
import android.app.ActivityManager.RunningServiceInfo;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

public class BootReceiver extends BroadcastReceiver {
    private static final String TAG = "QtiTelephonyService BootReceiver";
    private static final boolean DBG = true;
    private static final String mClassName = QtiTelephonyService.class.getName();

    @Override
    public void onReceive(Context context, Intent intent) {
        if (DBG) Log.d(TAG, "onReceive: " + intent);

        if (intent.getAction().equals(Intent.ACTION_BOOT_COMPLETED) ||
                intent.getAction().equals(Intent.ACTION_LOCKED_BOOT_COMPLETED)) {
            if (isServiceRunning(context)) {
                Log.i(TAG, mClassName + " is already running. " + intent + " ignored.");
                return;
            }

            Intent serviceIntent = new Intent(context, QtiTelephonyService.class);
            ComponentName serviceComponent = context.startService(serviceIntent);
            if (serviceComponent == null) {
                Log.e(TAG, "Could not start service");
            } else {
                Log.d(TAG, "Successfully started service");
            }
        } else {
            Log.e(TAG, "Received unsupported intent");
        }
    }

    private boolean isServiceRunning(Context context) {
        ActivityManager manager =
            (ActivityManager) context.getSystemService(Context.ACTIVITY_SERVICE);
        for (RunningServiceInfo service : manager.getRunningServices(Integer.MAX_VALUE)) {
            if (mClassName.equals(service.service.getClassName())) {
                return true;
            }
        }
        return false;
    }
}
