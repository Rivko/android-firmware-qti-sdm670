/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.StatsPollManager;

import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Intent;
import android.content.Context;
import android.util.Log;
import android.content.ComponentName;

public class StatsPollManagerReceiver extends BroadcastReceiver {

    private static final String TAG = "StatsPollManagerReceiver";

    @Override
    public void onReceive(Context context, Intent intent){
        if (intent.getAction().equals(Intent.ACTION_BOOT_COMPLETED)) {
            ComponentName com = new ComponentName(context.getPackageName(),
                                StatsPollManagerService.class.getName());
            if (com != null) {
                ComponentName service = context.startService(new Intent().setComponent(com));
                if (service == null) {
                    Log.d(TAG,"service failed to start");
                } else {
                    Log.d(TAG,"service started successfully");
                }
            } else {
                Log.d(TAG,"service does not exist");
            }
        }
    }
}
