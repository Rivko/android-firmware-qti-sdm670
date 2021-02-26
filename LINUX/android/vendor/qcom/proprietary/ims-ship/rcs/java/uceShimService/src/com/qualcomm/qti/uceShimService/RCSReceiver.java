/*********************************************************************
 Copyright (c) 2017 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
**********************************************************************/

package com.qualcomm.qti.uceShimService;

import java.io.BufferedReader;
import java.io.InputStreamReader;

import com.qualcomm.qti.uceShimService.RCSService;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

//secure boot complete
public class RCSReceiver extends BroadcastReceiver {

    private final static String logTAG = "RCSService BC-Receiver";

    @Override
    public void onReceive(Context context, Intent intent) {
        if (intent == null) {
            Log.e(logTAG, "Null Intent received");
            return;
        }

        final String intentActionStr = intent.getAction();
        if (intentActionStr == null) {
            Log.e(logTAG, "Null intent action received");
            return;
        }

        if (intentActionStr.equals(RCSService.INTENT_BOOT_COMPLETE_STR) ||
            intentActionStr.equals(Intent.ACTION_LOCKED_BOOT_COMPLETED)) {
                Log.d(logTAG, "Boot Complete Event Received");
                Intent startRCSServiceIntent = new Intent(context, RCSService.class);
                Log.d(logTAG, "START RCS SERVICE ON BOOTUP");
                context.startService(startRCSServiceIntent);
        } else {
            Log.d(logTAG, "UCE Service is disabled Please check Configuration");
        }
    }
}
