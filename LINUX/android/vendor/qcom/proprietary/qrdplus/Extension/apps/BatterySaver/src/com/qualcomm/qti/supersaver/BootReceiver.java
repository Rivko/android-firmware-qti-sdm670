/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.supersaver;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;

import static com.qualcomm.qti.supersaver.Utils.logd;

public class BootReceiver extends BroadcastReceiver {

        @Override
        public void onReceive(Context context, Intent intent) {
        logd(":" + intent);
        Intent saverIntent = new Intent(context, SuperSaverService.class);
        context.startService(saverIntent);
    }
}
