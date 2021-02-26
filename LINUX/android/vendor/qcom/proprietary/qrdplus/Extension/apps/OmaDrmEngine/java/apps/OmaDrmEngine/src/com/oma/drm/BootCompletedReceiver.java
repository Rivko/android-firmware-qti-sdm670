/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc
 */

package com.oma.drm;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

public class BootCompletedReceiver extends BroadcastReceiver {

    private static final String TAG = "OmaDrm:BootCompletedrec";
    private static final boolean debug = true;

    @Override
    public void onReceive(Context context, Intent intent) {
        // Start DrmManagerClientService
        if (debug) {
            Log.d(TAG, "Starting DrmManagerClientService");
        }
        Intent startServiceIntent = new Intent(context,
                DrmManagerClientService.class);
        context.startService(startServiceIntent);
    }

}
