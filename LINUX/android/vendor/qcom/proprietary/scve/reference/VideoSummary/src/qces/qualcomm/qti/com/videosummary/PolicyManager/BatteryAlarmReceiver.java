/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.PolicyManager;

import android.app.AlarmManager;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;

import qces.qualcomm.qti.com.videosummary.Util.Utils;

/**
 * Created by mithun on 7/26/2015.
 */
public class BatteryAlarmReceiver extends BroadcastReceiver {
    public static BatteryAlarmReceiver instance = null;
    private final int ALARM_REQUEST_CODE = 1;
    private static final String TAG = BatteryAlarmReceiver.class.getSimpleName();


    public static synchronized BatteryAlarmReceiver getInstance() {
        if (instance == null) {
            instance = new BatteryAlarmReceiver();
        }
        return instance;
    }

    @Override
    public void onReceive(Context context, Intent intent) {
        Utils.sendMessage(context,TAG, "alarm receiver");
        BatteryChangeEventReceiver.registerForBatteryChangeEvents(context);
    }


    public void cancel(Context context) {
        Utils.sendMessage(context,TAG,"cancelling alarm");
        Intent intent = new Intent(context, BatteryAlarmReceiver.class);
        PendingIntent pendingIntent = PendingIntent.getBroadcast(context.getApplicationContext(), ALARM_REQUEST_CODE, intent, 0);
        AlarmManager alarmManager = (AlarmManager) context.getSystemService(context.getApplicationContext().ALARM_SERVICE);
        alarmManager.cancel(pendingIntent);
    }

    public void setAlarmAtTime(Context context, float timeSeconds) {
        Intent alarmIntent = new Intent(context, BatteryAlarmReceiver.class);
        AlarmManager alarmManager = (AlarmManager) context.getSystemService(context.getApplicationContext().ALARM_SERVICE);
        alarmManager.setExact(AlarmManager.RTC_WAKEUP,
                System.currentTimeMillis() + (long) (timeSeconds * 1000),
                PendingIntent.getBroadcast(context.getApplicationContext(), ALARM_REQUEST_CODE, alarmIntent, 0));
    }

}
