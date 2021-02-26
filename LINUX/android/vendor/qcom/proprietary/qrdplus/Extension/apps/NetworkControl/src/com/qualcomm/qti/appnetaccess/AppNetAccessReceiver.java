/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
     Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

package com.qualcomm.qti.appnetaccess;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

public class AppNetAccessReceiver extends BroadcastReceiver {

    private static final String TAG = "AppNetAccessReceiver";

    @Override
    public void onReceive(Context context, Intent intent) {
        Intent service = new Intent(context, AppNetAccessService.class);
        service.putExtras(intent.getExtras());
        service.setAction(intent.getAction());
        context.startService(service);
    }
}
