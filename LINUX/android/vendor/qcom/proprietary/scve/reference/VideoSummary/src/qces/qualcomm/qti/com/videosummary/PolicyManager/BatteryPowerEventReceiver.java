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
import android.os.BatteryManager;
import android.widget.Toast;

import qces.qualcomm.qti.com.videosummary.Util.Utils;

/**
 * Created by mithun on 7/25/2015.
 */
public class BatteryPowerEventReceiver extends BroadcastReceiver {


    private static final String ACTION_POWER_CONNECTED_FAKE = "android.intent.action" +
            ".ACTION_POWER_CONNECTED_fake";
    private static final String ACTION_POWER_DISCONNECTED_FAKE = "android.intent.action" +
            ".ACTION_POWER_DISCONNECTED_fake";

    private static final String TAG = BatteryPowerEventReceiver.class.getSimpleName();
    private static BatteryPowerEventReceiver instance = null;
    private static boolean registered=false;

    static synchronized BatteryPowerEventReceiver getInstance() {
        if (instance == null) {
            instance = new BatteryPowerEventReceiver();
        }
        return instance;
    }

    static IntentFilter getPowerConnectedDisconnectedEventIntentFilter() {
        IntentFilter filter = new IntentFilter();
        filter.addAction(Intent.ACTION_POWER_CONNECTED);
        filter.addAction(Intent.ACTION_POWER_DISCONNECTED);

        filter.addAction(ACTION_POWER_CONNECTED_FAKE);
        filter.addAction(ACTION_POWER_DISCONNECTED_FAKE);

        return filter;
    }

    public static void registerForPowerConnectedDisconnectedEvents(Context context) {
        Utils.sendMessage(context, TAG, "trying to registerForPowerConnectedDisconnectedEvents");

        if(!registered) {
            context.getApplicationContext().registerReceiver(getInstance(),
                    getPowerConnectedDisconnectedEventIntentFilter());
            registered=true;
            Utils.sendMessage(context, TAG, "registerForPowerConnectedDisconnectedEvents " +
                    "Successful");
        }else{
            Utils.sendMessage(context, TAG, "already registerd " +
                    "ForPowerConnectedDisconnectedEvents");
        }

    }

    public static void unregisterForPowerConnectedDisconnectedEvents(Context context) {
        Utils.sendMessage(context, TAG, "trying to unregisterForPowerConnectedDisconnectedEvents");
        if(registered) {
            try {

                context.getApplicationContext().unregisterReceiver(getInstance());
                Utils.sendMessage(context, TAG, "unregisterForPowerConnectedDisconnectedEvents " +
                        "Successfully");
            } catch (IllegalArgumentException e) {
                e.printStackTrace();
                Utils.sendMessage(context, TAG, e.toString());
            }finally {
                registered = false;
            }

        }else{
            Utils.sendMessage(context, TAG, "Already " +
                    "unregister ForPowerConnectedDisconnectedEvents");
        }

    }

    @Override
    public void onReceive(Context context, Intent intent) {
        Toast.makeText(context, "onReceive: " + intent.getAction(), Toast.LENGTH_LONG).show();
        Utils.sendMessage(context, TAG, "\n\n\n\n\n\n\n\n\n\n\n");
        Utils.sendMessage(context, TAG, intent.getAction());

        if (intent.getAction().equals(Intent.ACTION_POWER_CONNECTED) || intent.getAction().equals
                (ACTION_POWER_CONNECTED_FAKE)) {
            int status = intent.getIntExtra(BatteryManager.EXTRA_STATUS, -1);
            Utils.sendMessage(context, TAG, "POWER CONNECTED "+Utils.getBatteryInfo(intent));
            if(status == BatteryManager.BATTERY_STATUS_CHARGING || status == BatteryManager
                    .BATTERY_STATUS_FULL) {
                handleActionPowerConnected(context);
            }
        }
        if (intent.getAction().equals(Intent.ACTION_POWER_DISCONNECTED) ||
                intent.getAction().equals(ACTION_POWER_DISCONNECTED_FAKE)) {
            handleActionPowerDisconnected(context);
        }
    }

    void handleActionPowerConnected(Context context) {
        /*
        wait till the status becomes charging before registering with Battery_change
         */
        Intent intent;
      /*  do {
            IntentFilter filter = new IntentFilter(Intent.ACTION_BATTERY_CHANGED);
            intent = context.getApplicationContext().registerReceiver(null, filter);
            Utils.sendMessage(context, TAG, "waiting for charging");
        } while (Utils.getBatteryStatus(intent).equals("Charging"));*/

        Utils.sendMessage(context, TAG, "action power connected so registering battery changed event");
        BatteryChangeEventReceiver.registerForBatteryChangeEvents(context);
    }

    void handleActionPowerDisconnected(Context context) {
        Utils.sendMessage(context, TAG, "action power disconnected so unregistering battery " +
                "changed event");
        BatteryChangeEventReceiver.unregisterForBatteryChangeEvents(context);
        BatteryAlarmReceiver.getInstance().cancel(context);
    }


}
