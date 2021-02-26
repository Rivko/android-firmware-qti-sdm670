/*=============================================================================
Copyright (c) 2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package com.qualcomm.qti.securemsm.mdtp.MdtpDemo;

import android.app.Activity;
import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.content.pm.PackageManager;
import android.content.ComponentName;
import android.content.Context;

import android.app.admin.DevicePolicyManager;
import android.app.admin.DeviceAdminReceiver;
import android.app.AlarmManager;
import android.os.PowerManager;
import android.os.PowerManager.WakeLock;
import android.app.PendingIntent;


public class LockReceiver extends DeviceAdminReceiver {
    private static String TAG = "LockReceiver";

    @Override
    public void onReceive(Context context, Intent intent) {
        Log.d(TAG,"LockReceiver - onReceive mdtp = " + intent.getAction());
        if ((intent.getAction().equals(Intent.ACTION_SCREEN_OFF))) {
            Log.d(TAG,"MDTP LockReceiver - screen is off");
            PowerManager pm = (PowerManager) context.getSystemService(Context.POWER_SERVICE);
            WakeLock wakeLock = pm.newWakeLock(PowerManager.SCREEN_DIM_WAKE_LOCK | PowerManager.ACQUIRE_CAUSES_WAKEUP | PowerManager.ON_AFTER_RELEASE, "TEST");
            wakeLock.acquire();

            AlarmManager alarmMgr = (AlarmManager) context.getSystemService(Context.ALARM_SERVICE);
            Intent inten = new Intent(context, LockScreenActivity.class);
            PendingIntent pi = PendingIntent.getActivity(context, 0, inten, 0);
            alarmMgr.set(AlarmManager.ELAPSED_REALTIME_WAKEUP,  100, pi);
        }
    }


    /**
    * Generates a {@link ComponentName} that is used throughout the app.
    * @return a {@link ComponentName}
    */
    public static ComponentName getComponentName(Context context) {
        Log.d(TAG,"LockReceiver - getComponentName mdtp");
        return new ComponentName(context.getApplicationContext(), LockReceiver.class);
    }

}
