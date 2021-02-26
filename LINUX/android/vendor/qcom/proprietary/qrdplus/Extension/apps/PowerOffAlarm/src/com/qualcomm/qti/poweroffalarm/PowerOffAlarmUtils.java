/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

package com.qualcomm.qti.poweroffalarm;

import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.SystemProperties;
import android.preference.PreferenceManager;
import android.util.Log;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;

import vendor.qti.hardware.alarm.V1_0.IAlarm;

public class PowerOffAlarmUtils {
    public static final String PREF_KEY_IS_FIRST_BOOT = "is_first_boot";

    public static final String PREF_KEY_RTC_ALARM_TIME = "rtc_alarm_time";

    public static final String PREF_KEY_ALARM_TIME = "alarm_time";

    public static final long DEFAULT_ALARM_TIME = 0L;

    public static final int ALARM_STATUS_NONE = 0;

    public static final int ALARM_STATUS_TO_FIRE = 1;

    public static final int ALARM_STATUS_SNOOZE = 2;

    public static final int ALARM_STATUS_DISMISS = 3;

    private static final int SUCCESS = 0;

    private static final int FAILURE = -1;

    public static final long MS_IN_ONE_MIN = 60000L;

    private static final long SEC_TO_MS = 1000L;

    private static final String TAG = "PowerOffAlarm";

    private static final String ALARM_BOOT_PROP = "ro.alarm_boot";

    /**
     * Write value to power off alarm files
     *
     */
    public static void writePowerOffAlarmFile(String fileName, String value) {
        FileWriter writer = null;
        try {
            writer = new FileWriter(fileName, false);
            writer.write(value);
            writer.flush();
        } catch (Exception e) {
            Log.e(TAG, e.toString());
        } finally {
            if (writer != null) {
                try {
                    writer.close();
                } catch (Exception e1) {
                    Log.e(TAG, e1.toString());
                }
            }
        }
    }

    /**
     * Read value from power off alarm files
     *
     */
    public static String readPowerOffAlarmFile(String fileName) {
        BufferedReader reader = null;
        String line = null;
        try {
            reader = new BufferedReader(new FileReader(fileName));
            line = reader.readLine();
        } catch (Exception e) {
            Log.e(TAG, e.toString());
        } finally {
            if (reader != null) {
                try {
                    reader.close();
                } catch (Exception e1) {
                    Log.e(TAG, e1.toString());
                }
            }
        }
        return line;
    }

    /**
     * Set alarm time to rtc register
     *
     * @param time alarm time based on current time (ms)
     * @return set result -- Fail, return FAILURE; Success,
     *         return the alarm time to rtc
     */
    public static long setAlarmToRtc(long alarmTime) {
        Log.d(TAG, "setAlarmToRtc time: " + alarmTime);

        long currentTime = System.currentTimeMillis();
        long alarmInRtc = getAlarmFromRtc();
        long rtcTime = getRtcTime();

        // calculate the alarm to rtc
        long timeDelta = alarmTime - currentTime - MS_IN_ONE_MIN;

        // alarm in two minute is not supported
        if (timeDelta <= 0) {
            Log.d(TAG, "setAlarmToRtc failed: alarm time is in one minute");
            return FAILURE;
        }

        long alarmTimeToRtc = timeDelta/SEC_TO_MS + rtcTime;

        try {
            IAlarm mProxy = IAlarm.getService();
            int ret = mProxy.setAlarm(alarmTimeToRtc);

            if (ret == SUCCESS) {
                return alarmTimeToRtc;
            } else {
                return FAILURE;
            }
        } catch (Exception e) {
            Log.d(TAG, e.toString());
            return FAILURE;
        }
    }

    /**
     * Get rtc time in rtc register(seconds)
     *
     * @return rtc time
     */
    public static long getRtcTime() {
        try {
            IAlarm mProxy = IAlarm.getService();
            long time = mProxy.getRtcTime();
            Log.d(TAG, "getRtcTime: " + time);
            return time;
        } catch (Exception e) {
            Log.d(TAG, e.toString());
            return FAILURE;
        }
    }

    /**
     * Get alarm in rtc register (seconds)
     *
     * @return alarm time
     */
    public static long getAlarmFromRtc() {
        Log.d(TAG, "getAlarmFromRtc");
        try {
            IAlarm mProxy = IAlarm.getService();
            long time = mProxy.getAlarm();
            return time;
        } catch (Exception e) {
            Log.d(TAG, e.toString());
            return FAILURE;
        }
    }

    /**
     * Cancel alarm in rtc register
     *
     * @return cancel result
     */
    public static int cancelAlarmInRtc() {
        Log.d(TAG, "cancelAlarmInRtc");
        try {
            IAlarm mProxy = IAlarm.getService();
            int ret = mProxy.cancelAlarm();
            return ret;
        } catch (Exception e) {
            Log.d(TAG, e.toString());
            return FAILURE;
        }
    }

    public static SharedPreferences getDefaultSharedPreferences(Context context) {
        Context storageContext = context.createDeviceProtectedStorageContext();
        return PreferenceManager.getDefaultSharedPreferences(storageContext);
    }

    /**
     * Save alarm time based on wall time to sharedPreference
     *
     * @param context application context
     * @param time alarm time based on wall time
     */
    public static void saveAlarmToPreference(Context context, long time) {
        getDefaultSharedPreferences(context).
                edit().putLong(PREF_KEY_ALARM_TIME, time).apply();
    }

    /**
     * Get alarm time based on wall time from sharedPreference
     *
     * @param context application context
     * @return alarm time based on wall time
     */
    public static long getAlarmFromPreference(Context context) {
        return getDefaultSharedPreferences(context).
                getLong(PREF_KEY_ALARM_TIME, DEFAULT_ALARM_TIME);
    }

    /**
     * Save alarm time based on rtc time to sharedPreference
     *
     * @param context application context
     * @param time alarm time based on rtc seconds
     */
    public static void saveRtcAlarmToPreference(Context context, long time) {
        getDefaultSharedPreferences(context).
                edit().putLong(PREF_KEY_RTC_ALARM_TIME, time).apply();
    }

    /**
     * Get alarm time based on rtc time from sharedPreference
     *
     * @param context application context
     * @return alarm time based on rtc time
     */
    public static long getRtcAlarmFromPreference(Context context) {
        return getDefaultSharedPreferences(context).
                getLong(PREF_KEY_RTC_ALARM_TIME, DEFAULT_ALARM_TIME);
    }

    /**
     * Implement reboot function immediately.
     */
    public static void reboot(Context context) {
        Intent requestReboot = new Intent(
                Intent.ACTION_REBOOT);
        requestReboot.putExtra(Intent.EXTRA_KEY_CONFIRM, false);
        requestReboot.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        context.startActivity(requestReboot);
    }


    /**
     * Implement power off function immediately.
     */
    public static void powerOff(Context context) {
        Intent requestShutdown = new Intent(
                Intent.ACTION_REQUEST_SHUTDOWN);
        requestShutdown.putExtra(Intent.EXTRA_KEY_CONFIRM, false);
        requestShutdown.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        context.startActivity(requestShutdown);
    }

    public static boolean isFirstBoot(Context context) {
        return getDefaultSharedPreferences(context)
                .getBoolean(PREF_KEY_IS_FIRST_BOOT, true);
    }

    /**
     * Marks that the first boot has been completed.
     */
    public static void setFirstBootCompleted(Context context) {
        getDefaultSharedPreferences(context).edit()
                .putBoolean(PREF_KEY_IS_FIRST_BOOT, false)
                .apply();
    }

    /**
     * Check if it is alarm boot.
     */
    public static boolean isAlarmBoot() {
        return SystemProperties.getBoolean(ALARM_BOOT_PROP, false);
    }
}
