/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qti.csm.securityLog;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

import com.qti.csm.permission.utils.PermissionUtils;

public class SystemInfo extends BroadcastReceiver {
    private static final String TAG = "SystemInfo";
    private DataProc sysData;

    @Override
    public void onReceive(Context context, Intent intent) {
        if (intent.getAction().equals(Intent.ACTION_BOOT_COMPLETED)) {
            Log.i(TAG, "boot");
            Settings.confUpdate(context);
            Settings.updateTask(context);
            PermissionUtils.initStat(context);
        } else if (intent.getAction().equals(Intent.ACTION_SHUTDOWN)) {
            Log.i(TAG, "down");
        }
    }
}