/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.Notification;

import android.app.AlarmManager;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;

import java.util.Calendar;

import qces.qualcomm.qti.com.videosummary.Util.Constants;
import qces.qualcomm.qti.com.videosummary.Util.Utils;

/**
 * Created by mithun on 11/5/15.
 */
public class AutoPendingNotificationTrigger extends BroadcastReceiver {
    public static AutoPendingNotificationTrigger instance = null;
    private final int ALARM_REQUEST_CODE = 2;
    private static final String TAG = AutoPendingNotificationTrigger.class.getSimpleName();


    public static synchronized AutoPendingNotificationTrigger getInstance() {
        if (instance == null) {
            instance = new AutoPendingNotificationTrigger();
        }
        return instance;
    }

    @Override
    public void onReceive(Context context, Intent intent) {
        Utils.sendMessage(context, TAG, "AutoPendingNotificationTrigger alarm receiver");

        long timestampMillis = intent.getLongExtra(Constants.EXTRA_TIME_STAMP_MILLIS, 0);
        handleActionPostPendingNotifications(context, timestampMillis);
    }

    private void handleActionPostPendingNotifications(Context context, long timestampMillis) {
        Notification notification = new Notification(context, timestampMillis);
        notification.showNotification(false);
    }


    public void cancel(Context context) {
        Utils.sendMessage(context, TAG, "AutoPendingNotificationTrigger cancelling alarm");
        Intent intent = new Intent(context, AutoPendingNotificationTrigger.class);
        PendingIntent pendingIntent = PendingIntent.getBroadcast(context.getApplicationContext(), ALARM_REQUEST_CODE, intent, PendingIntent.FLAG_UPDATE_CURRENT);
        AlarmManager alarmManager = (AlarmManager) context.getSystemService(context.getApplicationContext().ALARM_SERVICE);
        alarmManager.cancel(pendingIntent);
    }

    /**
     * Will set the alarm upcoming morning 9 am.
     *
     * @param context
     * @param timestampMillisKey
     */
    public void setAlarmMorning(Context context, long timestampMillisKey) {
        //cancel any existing alarms and then set a new one
        cancel(context);

        Calendar calendar = Calendar.getInstance();
        calendar.set(Calendar.HOUR_OF_DAY, Constants.AFTER_HOURS_END);
        calendar.set(Calendar.MINUTE, 0);
        calendar.set(Calendar.SECOND, 0);
        calendar.set(Calendar.MILLISECOND, 0);

        if (calendar.compareTo(Calendar.getInstance()) < 0) {
            //this means we are trying to set alarm time which is already expired, so add 24Hrs or 1 day to this and then set alarm
            calendar.add(Calendar.DATE, 1);
        }

        Intent alarmIntent = new Intent(context, AutoPendingNotificationTrigger.class);

        Bundle bundle = new Bundle();
        bundle.putLong(Constants.EXTRA_TIME_STAMP_MILLIS, timestampMillisKey);
        alarmIntent.putExtras(bundle);

        AlarmManager alarmManager = (AlarmManager) context.getSystemService(context.getApplicationContext().ALARM_SERVICE);
        alarmManager.setExact(AlarmManager.RTC_WAKEUP,
                calendar.getTimeInMillis(),
                PendingIntent.getBroadcast(context.getApplicationContext(), ALARM_REQUEST_CODE, alarmIntent, PendingIntent.FLAG_UPDATE_CURRENT));
    }

}
