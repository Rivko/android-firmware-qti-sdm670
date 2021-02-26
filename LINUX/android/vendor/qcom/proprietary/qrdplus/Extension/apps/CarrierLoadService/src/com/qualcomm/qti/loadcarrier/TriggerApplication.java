/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.loadcarrier;

import android.app.Application;
import android.app.ActivityManager;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.provider.Settings;
import android.util.Log;

import java.util.ArrayList;
import java.util.List;
import com.android.internal.telephony.TelephonyIntents;

public class TriggerApplication extends Application {
    private static final String TAG = "TriggerApplication";

    private static final String ACTION_PHONE_READY = "com.android.phone.ACTION_PHONE_READY";
    private static final String INTENTFILTER_SCHEME = "android_secret_code";

    BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            //unregisterReceiver(mReceiver);
            if (Utils.DEBUG) Log.i(TAG, "TriggerApplication onReceive. intent.getAction()="
                    + intent.getAction());
            // If the trigger function has been enabled, we will start the service.
            boolean defaultValue = context.getResources().getBoolean(R.bool.trigger_enabled);
            String currentCarrierName = Utils.getCurrentCarriersName(Utils.getCurrentCarriers());
            if (Utils.getValue(Utils.PROP_KEY_TRIGGER, defaultValue)
                    && (Utils.getValue(Utils.PROP_KEY_TRIGGER_FIRST_INSERT, false) == false
                            || (Utils.getValue(Utils.PROP_KEY_TRIGGER_FIRST_INSERT, false)
                                    && currentCarrierName.equals("Default")))
                    && Settings.Global.getInt(context.getContentResolver(),
                            Settings.Global.AIRPLANE_MODE_ON, 0) == 0) {
                if(!isServiceRunning(context,"com.qualcomm.qti.loadcarrier.TriggerService")){
                    if (Utils.DEBUG) Log.i(TAG, "Start the trigger service.");
                    context.startService(new Intent(context, TriggerService.class));
                }
            }
        }
    };

    BroadcastReceiver mTriggerActionReceiver = new TriggerActionReceiver();

    @Override
    public void onCreate() {
        super.onCreate();

        IntentFilter filter = new IntentFilter();
        filter.addAction(ACTION_PHONE_READY);
        filter.addAction(TelephonyIntents.ACTION_SIM_STATE_CHANGED);
        filter.setPriority(1000);
        registerReceiver(mReceiver, filter);
        Log.d(TAG, "register receiver to start TriggerService");

        filter = new IntentFilter();
        filter.addAction(TriggerActionReceiver.ACTION_SET_TRIGGER_PROPERTY);
        filter.setPriority(1000);
        registerReceiver(mTriggerActionReceiver, filter);
        filter = new IntentFilter();
        filter.addAction(TelephonyIntents.SECRET_CODE_ACTION);
        filter.addDataScheme(INTENTFILTER_SCHEME);
        filter.addDataAuthority(TriggerActionReceiver.HOST_TRIGGER_Y, null);
        filter.addDataAuthority(TriggerActionReceiver.HOST_TRIGGER_N, null);
        filter.addDataAuthority(TriggerActionReceiver.HOST_TRIGGER_VALUE, null);
        filter.addDataAuthority(TriggerActionReceiver.HOST_TRIGGER_START, null);
        filter.addDataAuthority(TriggerActionReceiver.HOST_FILE_BASED_Y, null);
        filter.addDataAuthority(TriggerActionReceiver.HOST_FILE_BASED_N, null);
        filter.addDataAuthority(TriggerActionReceiver.HOST_BLOCK_BASED_Y, null);
        filter.addDataAuthority(TriggerActionReceiver.HOST_BLOCK_BASED_N, null);
        filter.setPriority(1000);
        registerReceiver(mTriggerActionReceiver, filter);
        filter = new IntentFilter();
        filter.addAction(TriggerActionReceiver.ACTION_CARRIER_PREFERRED_SIM_SWAPPED);
        filter.setPriority(1000);
        registerReceiver(mTriggerActionReceiver, filter);
        Log.d(TAG, "register TriggerActionReceiver");
    }
    /**
        * if specified service is running
        * @param context
        * @param className; service name
        * @return true running, false not running
        */
       public static boolean isServiceRunning(Context mContext,String className) {
           boolean isRunning = false;
           ActivityManager activityManager = (ActivityManager)mContext.getSystemService(Context.ACTIVITY_SERVICE);
           List<ActivityManager.RunningServiceInfo> serviceList
           = activityManager.getRunningServices(40);
          if (!(serviceList.size()>0)) {
               return false;
           }
           for (int i=0; i<serviceList.size(); i++) {
                //Log.d(TAG, "...service name="+serviceList.get(i).service.getClassName());
                if (serviceList.get(i).service.getClassName().equals(className) == true) {
                    isRunning = true;
                    break;
                }
           }
           return isRunning;
       }

    @Override
    public void onTerminate() {
        // run when the process terminated
        Log.d(TAG, "onTerminate");
        unregisterReceiver(mReceiver);
        super.onTerminate();
    }
    @Override
    public void onLowMemory() {
        // run when system has low memory
        Log.d(TAG, "onLowMemory");
        //unregisterReceiver(mReceiver);
        super.onLowMemory();
    }

    @Override
    public void onTrimMemory(int level) {
        // run when system execute memory clean
        Log.d(TAG, "onTrimMemory");
        super.onTrimMemory(level);
    }
}
