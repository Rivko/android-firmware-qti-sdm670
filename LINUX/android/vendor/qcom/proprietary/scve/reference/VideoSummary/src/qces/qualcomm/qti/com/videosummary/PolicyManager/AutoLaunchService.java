/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.PolicyManager;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

/**
 * An {@link AutoLaunchService} subclass of BroadcastReceiver to receive BOOT_COMPLETED intent,
 * this receiver will start InitService which does the one time initializations/reinitializations
 * upon reboot
 * <p/>
 */
public class AutoLaunchService extends BroadcastReceiver {
    private static final String TAG = AutoLaunchService.class.getSimpleName();

    public AutoLaunchService() {
    }

    @Override
    public void onReceive(Context context, Intent intent) {
        // TODO: This method is called when the BroadcastReceiver is receiving
        // an Intent broadcast.
        //upon boot_completed
        Log.i(TAG,"onReceive of videosummary.PolicyManager.AutoLaunchService, action="+intent.getAction
                ());
        InitService.startActionInit(context);
    }
}
