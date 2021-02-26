/*
 * Copyright (c) 2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.utils;

import android.app.Activity;
import android.app.KeyguardManager;
import android.content.Context;
import android.content.SharedPreferences;
import android.os.PowerManager;
import android.os.PowerManager.WakeLock;
import android.os.SystemProperties;
import android.preference.PreferenceManager;
import android.view.KeyEvent;
import android.view.WindowManager;

import com.qualcomm.qti.qmmi.bean.TestCase;
import com.qualcomm.qti.qmmi.framework.BaseActivity;
import com.qualcomm.qti.qmmi.framework.BaseService;

import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.List;

public class Utils {
    public static String CONFIG_FILE_NAME = "config_file_name";
    public static final String BASE_DIR = "/data/FTM_AP/";
    public static final String RES_RAW_SOUND_WAV = "qualsound.wav";
    public static final String RES_RAW_TONE_1KHZ_WAV = "tone_single_1khz.wav";
    public static final String SDCARD_DIR = "/sdcard/";

    public static final String SOUND_FILE_PATH = BASE_DIR + RES_RAW_SOUND_WAV;
    public static final String TONE_1KHZ_FILE_PATH = BASE_DIR + RES_RAW_TONE_1KHZ_WAV;

    //Broadcast for Qmmi
    public static final String ACTION_UPDATE_MESSAGE =
            "qualcomm.qti.qmmi.UPDATE_MESSAGE";
    public static final String ACTION_UPDATE_FOREGROUND_CASE_RESULT =
            "qualcomm.qti.qmmi.UPDATE_FOREGROUND_CASE_RESULT";
    public static final String ACTION_UPDATE_BACKGROUND_CASE_RESULT =
            "qualcomm.qti.qmmi.UPDATE_BACKGROUND_CASE_RESULT";

    public static final String ACTION_DIAG_UPDATE_TESTCAST_LIST =
            "qualcomm.qti.qmmi.DIAG_UPDATE_TESTCASE_LIST";
    public static final String ACTION_DIAG_START_TESTCAST =
            "qualcomm.qti.qmmi.DIAG_START_TESTCAST";
    public static final String ACTION_DIAG_UPDATE_TESTCAST_RESULT =
            "qualcomm.qti.qmmi.DIAG_UPDATE_TESTCASE_RESULT";

    public static final String ACTION_UPDATE_FM_STATE =
            "qualcomm.qti.qmmi.UPDATE_FM_STATE";
    public static final String ACTION_FM_RECEIVER_ENABLE =
            "qualcomm.qti.qmmi.FM_RECEIVER_ENABLE";

    //Bundle Key for Update result broadcast/start service
    public static final String BUNDLE_KEY_RESULT = "result";
    public static final String BUNDLE_KEY_CASE_NAME = "case_name";
    public static final String BUNDLE_KEY_RUN_FUNCTIOIN = "run_function";
    public static final String BUNDLE_KEY_MESSAGE = "msg";

    /**
     * In N and earlier, powers of 1024 are used instead, with KB = 1024 bytes, MB = 1,048,576 bytes, etc.
     *
     * As of Android O, the prefixes are used in their standard meanings in the SI system,
     * so kB = 1000 bytes, MB = 1,000,000 bytes, etc.
     *
     * QMMI support from Android O, so use the unit: 1000.
     */
    public static final int GIGABYTE_BIT = 1000 * 1000 * 1000;

    public static void parseCases(final String in, List<String> out) {
        String[] strs = in.split(",");
        for (String substr : strs) {
            out.add(substr);
        }
    }


    public static String getSavedString(Context context, String key, String defult) {
        SharedPreferences mSharedPreferences = PreferenceManager
                .getDefaultSharedPreferences(context);
        return mSharedPreferences.getString(key, defult);
    }

    public static void setSavedString(Context context, String key, String value) {
        SharedPreferences mSharedPreferences = PreferenceManager
                .getDefaultSharedPreferences(context);
        SharedPreferences.Editor editor = mSharedPreferences.edit();
        editor.putString(key, value);
        editor.commit();
    }

    public static String getResultFileName(Context context) {
        String configFileName = getSavedString(context, CONFIG_FILE_NAME, "test_case_config.xml");
        String resultFileName;
        if (configFileName != null && !configFileName.equals("test_case_config.xml")) {
            String str = configFileName.substring(0, configFileName.indexOf("."));
            resultFileName = str + "_res.xml";
        } else {
            resultFileName = "mmi_res.xml";
        }
        return resultFileName;
    }

    public static Class<?> getActivityClass(String caseKey) {
        try {
            Class<?> clazz = Class.forName("com.qualcomm.qti.qmmi.testcase." + caseKey + "." +
                    caseKey + "Activity");
            if (!BaseActivity.class.isAssignableFrom(clazz)) {
                return null;
            }
            return clazz;
        } catch (ClassNotFoundException e) {
            return null;
        }
    }

    public static Class<?> getServiceClass(String caseKey) {
        try {
            Class<?> clazz = Class.forName("com.qualcomm.qti.qmmi.testcase." + caseKey + "." +
                    caseKey + "Service");
            if (!BaseService.class.isAssignableFrom(clazz)) {
                return null;
            }
            return clazz;
        } catch (ClassNotFoundException e) {
            return null;
        }
    }

    public static String getSystemProperties(String key, String defaultValue) {
        if (key == null)
            return null;
        String property = SystemProperties.get(key, defaultValue);
        LogUtils.logi(key + "=" + property);
        return property;
    }


    public static Date getCurrentTime() {
        SimpleDateFormat formatter = new SimpleDateFormat("yyyy-MM-dd hh:mm:ss");
        Date curDate = new Date(System.currentTimeMillis());//get current time
        String time = formatter.format(curDate);
        LogUtils.logi( "currentTime:" + time + ",data:" + curDate);
        return curDate;
    }

    public static void keepScreenOn(Activity context){
        context.getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
    }

    public static String byteTo16(byte bt){
        String[] strHex={"0","1","2","3","4","5","6","7","8","9","a","b","c","d","e","f"};
        String resStr="";
        int low =(bt & 15);
        int high = bt>>4 & 15;
        resStr = strHex[high]+strHex[low];
        return resStr;
    }

    public static StringBuilder byteTo16(byte[] bts){
        StringBuilder builder = new StringBuilder();
        for(byte bt: bts){
            builder.append(byteTo16(bt));
        }
        return builder;
    }

    public static void wakeUpAndKeepScreenOn(Activity context){
        WakeLock screenLock = ((PowerManager)context.getSystemService(context.POWER_SERVICE)).newWakeLock(
            PowerManager.SCREEN_BRIGHT_WAKE_LOCK | PowerManager.ACQUIRE_CAUSES_WAKEUP, "TAG");
        screenLock.acquire();
        keepScreenOn(context);
        screenLock.release();
    }
}

