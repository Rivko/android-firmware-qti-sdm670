/*******************************************************************************
@file    PowerOffAlarmBroadcastReceiver.java
@brief   Receive "org.codeaurora.poweroffalarm.action.SET_ALARM" action to set
         power off alarm and receive "org.codeaurora.poweroffalarm.action.CANCEL_ALARM"
         action to cancel alarm.
---------------------------------------------------------------------------

Copyright (c) 2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

---------------------------------------------------------------------------
******************************************************************************/

package com.qualcomm.qti.poweroffalarm;

import android.app.AlarmManager;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.SystemProperties;
import android.os.UserHandle;
import android.util.Log;

import java.io.File;

public class PowerOffAlarmBroadcastReceiver extends BroadcastReceiver {
    private static final String TAG = "PowerOffAlarm";

    private static final String ACTION_SET_POWEROFF_ALARM =
            "org.codeaurora.poweroffalarm.action.SET_ALARM";

    private static final String ACTION_CANCEL_POWEROFF_ALARM =
            "org.codeaurora.poweroffalarm.action.CANCEL_ALARM";

    private static final String ACTION_UPDATE_ALARM_STATUS =
            "org.codeaurora.poweroffalarm.action.UPDATE_ALARM";

    private static final String ENCRYPTING_STATE = "trigger_restart_min_framework";

    private static final String PERSIST_ALARM_FOLDER = "/persist/alarm/";

    private static final String ALARM_BOOT_PROP = "ro.alarm_boot";

    private static final String STATUS = "status";

    private static final String TIME = "time";

    private static final String SNOOZE_TIME = "snooze_time";

    private static final int FAILURE = -1;

    @Override
    public void onReceive(Context context, Intent intent) {
        Log.d(TAG, "Receive " + intent.getAction() + " intent. ");

        String action = intent.getAction();
        String cryptState = SystemProperties.get("vold.decrypt");
        PowerOffAlarmPersistData persistData = new PowerOffAlarmPersistData();

        if (Intent.ACTION_LOCKED_BOOT_COMPLETED.equals(action)) {

            // When it is the first boot completed of PowerOffAlarm app,
            // delete the persist files.
            if (PowerOffAlarmUtils.isFirstBoot(context) &&
                            !ENCRYPTING_STATE.equals(cryptState)) {
                File file = new File(PERSIST_ALARM_FOLDER);
                if (file.exists()) {
                    String[] fileList = file.list();
                    for (int i = 0; i < fileList.length; i++) {
                        File tempFile = new File(PERSIST_ALARM_FOLDER + fileList[i]);
                        tempFile.delete();
                    }
                }

                PowerOffAlarmUtils.setFirstBootCompleted(context);
            }

            long alarmTime = persistData.getAlarmTime();
            // When alarmTime is 0, it means there is no alarm need to be handled,
            // just return.
            if (alarmTime == PowerOffAlarmUtils.DEFAULT_ALARM_TIME)
                return;

            int alarmStatus = persistData.getAlarmStatus();
            long currentTime = System.currentTimeMillis();
            if (ENCRYPTING_STATE.equals(cryptState)) {
                // If it is min framework mode, set a pending intent to AlarmManager
                // to start alarm activity as device is boot up 1 minutes earlier than
                // the real alarm time.
                long snoozeTime = persistData.getSnoozeTime();

                long time = 0L;

                if (alarmStatus == PowerOffAlarmUtils.ALARM_STATUS_TO_FIRE) {
                    time = alarmTime;
                } else if (alarmStatus == PowerOffAlarmUtils.ALARM_STATUS_SNOOZE) {
                    time = snoozeTime;
                } else if (alarmStatus == PowerOffAlarmUtils.ALARM_STATUS_DISMISS ||
                        alarmStatus == PowerOffAlarmUtils.ALARM_STATUS_NONE) {
                    return;
                }

                if (time >= currentTime) {
                    AlarmManager am = (AlarmManager) context.
                            getSystemService(Context.ALARM_SERVICE);
                    Intent activityIntent = new Intent(context, PowerOffAlarmActivity.class);
                    PendingIntent pendingIntent = PendingIntent.getActivity(context, 1,
                            activityIntent, PendingIntent.FLAG_UPDATE_CURRENT);
                    am.setExactAndAllowWhileIdle(AlarmManager.RTC_WAKEUP, time,
                            pendingIntent);
                } else {
                    Log.d(TAG, "Time is smaller than current time "+ persistData.toString());

                    persistData.setAlarmStatus(PowerOffAlarmUtils.ALARM_STATUS_NONE);
                    persistData.setSnoozeTime(PowerOffAlarmUtils.DEFAULT_ALARM_TIME);
                    persistData.setAlarmTime(PowerOffAlarmUtils.DEFAULT_ALARM_TIME);
                    persistData.writeDataToFile();
                }

            } else {
                // If it is normal mode and the alarm is handled in min framework, send
                // update status broadcast. Clock app will receiver this broadcast and
                // update the alarm status.

                if (alarmStatus > PowerOffAlarmUtils.ALARM_STATUS_TO_FIRE) {
                    // send update broadcast
                    Log.d(TAG, "Send update broadcast");

                    Intent updateIntent = new Intent(ACTION_UPDATE_ALARM_STATUS);
                    updateIntent.addFlags(Intent.FLAG_RECEIVER_FOREGROUND);
                    updateIntent.putExtra(TIME, alarmTime);
                    updateIntent.putExtra(STATUS, alarmStatus);
                    if (alarmStatus == PowerOffAlarmUtils.ALARM_STATUS_SNOOZE) {
                        updateIntent.putExtra(SNOOZE_TIME, persistData.getSnoozeTime());
                    }
                    context.sendBroadcast(updateIntent);
                }
            }
        }

        // Set or cancel power off alarm
        if (ACTION_SET_POWEROFF_ALARM.equals(action)) {
            long alarmTime = (long) intent.getExtra(TIME);
            long alarmInPref = PowerOffAlarmUtils.getAlarmFromPreference(context);

            if (alarmTime != alarmInPref) {
                PowerOffAlarmUtils.saveAlarmToPreference(context, alarmTime);
            }

        } else if (ACTION_CANCEL_POWEROFF_ALARM.equals(action)){
            long alarmTime = (long) intent.getExtra(TIME);
            long alarmInPref = PowerOffAlarmUtils.getAlarmFromPreference(context);

            if (alarmTime == alarmInPref) {
                PowerOffAlarmUtils.saveAlarmToPreference(context, PowerOffAlarmUtils.DEFAULT_ALARM_TIME);
            }
        }

        // If it is shut down broadcast, set alarm to rtc register or
        // cancel alarm in rtc register.
        if (Intent.ACTION_SHUTDOWN.equals(action) &&
                !ENCRYPTING_STATE.equals(cryptState)) {
            long alarmTime = PowerOffAlarmUtils.getAlarmFromPreference(context);

            if (alarmTime != PowerOffAlarmUtils.DEFAULT_ALARM_TIME) {
                long alarmTimeToRtc = PowerOffAlarmUtils.setAlarmToRtc(alarmTime);
                if (alarmTimeToRtc != FAILURE) {
                    // Save alarm time in persist file for alarm in min frameowork.
                    persistData.setAlarmTime(alarmTime);
                    persistData.setAlarmStatus(PowerOffAlarmUtils.ALARM_STATUS_TO_FIRE);
                    persistData.setSnoozeTime(PowerOffAlarmUtils.DEFAULT_ALARM_TIME);
                    persistData.writeDataToFile();

                    PowerOffAlarmUtils.saveRtcAlarmToPreference(context, alarmTimeToRtc);
                }
            } else {
                PowerOffAlarmUtils.saveRtcAlarmToPreference(context, PowerOffAlarmUtils.DEFAULT_ALARM_TIME);
                int rtc = PowerOffAlarmUtils.cancelAlarmInRtc();
                if (rtc < 0) {
                    Log.d(TAG, "Cancel alarm time in rtc failed ");
                }

                persistData.setAlarmStatus(PowerOffAlarmUtils.ALARM_STATUS_NONE);
                persistData.setSnoozeTime(PowerOffAlarmUtils.DEFAULT_ALARM_TIME);
                persistData.setAlarmTime(PowerOffAlarmUtils.DEFAULT_ALARM_TIME);
                persistData.writeDataToFile();
            }
        }
    }

}
