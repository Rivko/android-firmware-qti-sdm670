/*
 * Copyright (c) 2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.android.poweroffhandlerapp;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;
import com.android.poweroffhandlerapp.PowerOffHandlerActivity;

public class PowerOffReceiver extends BroadcastReceiver {
    @Override
    public void onReceive(Context context, Intent intent) {
        if("android.intent.action.ACTION_SHUTDOWN".equals(intent.getAction()) &&
            (intent.getFlags() ==(intent.FLAG_RECEIVER_FOREGROUND
                | intent.FLAG_EXCLUDE_STOPPED_PACKAGES))){
            Intent activityIntent = new Intent();
            activityIntent.setClass(context, PowerOffHandlerActivity.class);
            activityIntent.putExtra("shutdown_handler", true);
            activityIntent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            context.startActivity(activityIntent);
        }
    }
}
