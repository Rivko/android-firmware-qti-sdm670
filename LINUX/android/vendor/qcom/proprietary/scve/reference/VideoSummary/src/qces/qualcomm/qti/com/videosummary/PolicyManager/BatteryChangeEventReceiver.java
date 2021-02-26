/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.PolicyManager;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;

import qces.qualcomm.qti.com.videosummary.Util.Utils;

/**
 * Created by mithun on 7/26/2015.
 */
public class BatteryChangeEventReceiver extends BroadcastReceiver {
    private static BatteryChangeEventReceiver instance = null;
    private static final String TAG = BatteryChangeEventReceiver.class.getSimpleName();

    private static boolean registered=false;


    public static synchronized BatteryChangeEventReceiver getInstance() {
        if (instance == null) {
            instance = new BatteryChangeEventReceiver();
        }
        return instance;
    }

    @Override
    public void onReceive(Context context, Intent intent) {
        Utils.sendMessage(context,TAG, "BatteryChangeEventReceiver: " + intent.getAction());
        Utils.sendMessage(context, TAG,Utils.getBatteryInfo(intent));
        //register for batterylocal events for message communication

        if( !BatteryMonitoringService.runningBatteryMonitorService ) {
            Utils.sendMessage(context, TAG, "startActionBatteryMonitor ");
            BatteryMonitoringService.startActionBatteryMonitor(context, intent);
        }else{
            Utils.sendMessage(context, TAG, "Already running BatteryMonitor ");
        }
    }

    public static void registerForBatteryChangeEvents(Context context) {
        Utils.sendMessage(context, TAG, "trying to registerForBatteryChangeEvents");
        if(!registered){
        IntentFilter filter = new IntentFilter(Intent.ACTION_BATTERY_CHANGED);
        context.getApplicationContext().registerReceiver(getInstance(), filter);
            registered=true;
            Utils.sendMessage(context, TAG, "registerForBatteryChangeEvents Successful");
        }else{
            Utils.sendMessage(context, TAG, "already registerd For BatteryChangeEvents");
        }

    }

    public static void unregisterForBatteryChangeEvents(Context context) {
        Utils.sendMessage(context, TAG, "trying to unregisterForBatteryChangeEvents");
        if(registered){
        try {
            context.getApplicationContext().unregisterReceiver(getInstance());
            Utils.sendMessage(context, TAG, "unregisterForBatteryChangeEvents Successful");
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
            Utils.sendMessage(context, TAG, e.toString());
        }finally {
            registered = false;
        }
        }else{
            Utils.sendMessage(context, TAG, "already unregisterd for BatteryChangeEvents");
        }
    }
}
