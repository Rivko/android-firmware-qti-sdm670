/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qti.csm.securityLog;

import android.app.ActivityManager;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.util.Log;

import java.util.List;

class Settings {
    private static final String SETTING = "SET";
    private static final String CON_INF = "con";
    private static final String TAG = "Settings";

    private static boolean boolGet(Context context, String name) {
        SharedPreferences settings = context.getApplicationContext()
                .getSharedPreferences(CON_INF, 0);
        return settings.getBoolean(name, false);
    }

    private static String setGet(Context context, String name) {
        SharedPreferences settings = context.getApplicationContext()
                .getSharedPreferences(CON_INF, 0);
        return settings.getString(name, null);
    }

    private static int intGet(Context context, String name) {
        SharedPreferences settings = context.getApplicationContext()
                .getSharedPreferences(CON_INF, 0);
        return settings.getInt(name, 0);
    }

    public static int getSet(Context context) {
        if (context == null) {
            Log.e(TAG, "null Context");
            return 0;
        }
        return intGet(context, SETTING);
    }

    private static void boolSet(Context context, String name, boolean value) {
        SharedPreferences settings = context.getApplicationContext()
                .getSharedPreferences(CON_INF, 0);
        SharedPreferences.Editor editor = settings.edit();
        editor.putBoolean(name, value);
        editor.commit();
    }

    private static void strSet(Context context, String name, String value) {
        SharedPreferences settings = context.getApplicationContext()
                .getSharedPreferences(CON_INF, 0);
        SharedPreferences.Editor editor = settings.edit();
        editor.putString(name, value);
        editor.commit();
    }

    private static void intSet(Context context, String name, int value) {
        SharedPreferences settings = context.getApplicationContext()
                .getSharedPreferences(CON_INF, 0);
        SharedPreferences.Editor editor = settings.edit();
        editor.putInt(name, value);
        editor.commit();
    }

    public static void setDef(Context context, int value) {
        if (context == null) {
            Log.e(TAG, "err context");
            return;
        }
        intSet(context, SETTING, value);
    }

    public static void confUpdate(Context context) {
        DataProc.mSet = getSet(context);
    }

    public static void runTask(Context context) {
        Log.i(TAG, "task");
        if (checkRun(context)) {
            Log.i(TAG, "Running");
        } else {
            Intent s = new Intent(context, RunTask.class);
            context.startService(s);
        }
    }

    public static void updateTask(Context context) {
        Log.i(TAG, "update");
        if (checkRun(context)) {
            Intent s = new Intent(context, RunTask.class);
            context.stopService(s);
        }
        if (checkRun(context)) {
            Log.i(TAG, "Run");
        } else {
            Intent s = new Intent(context, RunTask.class);
            context.startService(s);
        }
    }

    public static boolean checkRun(Context context) {
        ActivityManager am = (ActivityManager) context
                .getSystemService(Context.ACTIVITY_SERVICE);
        List<ActivityManager.RunningServiceInfo> list = am.getRunningServices(Integer.MAX_VALUE);
        for (ActivityManager.RunningServiceInfo info : list) {
            if (info.service.getClassName().equals(
                    "com.qti.csm.securityLog.RunTask")) {
                return true;
            }
        }
        return false;
    }
}
