/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qti.qualcomm.datastatusnotification;

import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

public class BootReceiver extends BroadcastReceiver {
    private static final String TAG = "QcDataStatusNotification BootReceiver";
    private static final boolean DBG = true;

    @Override
    public void onReceive(Context context, Intent intent) {
        if (DBG)
            Log.d(TAG, "onReceive: " + intent.toString());
        if (intent.getAction().equals(Intent.ACTION_BOOT_COMPLETED)) {
            Intent serviceIntent = new Intent(context, DataStatusNotificationService.class);
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
}
