/*
 * Copyright (c) 2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.utils;

import android.util.Log;

public class LogUtils {
    public static String TAG = "Qmmi";

    public static void logi(Object s) {
        StackTraceElement[] stackTrace = Thread.currentThread().getStackTrace();
        String fileName = stackTrace[3].getFileName();
        String methodName = stackTrace[3].getMethodName();
        s = "[" +fileName + "] " + "[" + methodName + "] " + s;
        Log.i(TAG, s + "");
    }

    public static void loge(Object s) {
        StackTraceElement[] stackTrace = Thread.currentThread().getStackTrace();
        String fileName = stackTrace[3].getFileName();
        String methodName = stackTrace[3].getMethodName();
        s = "[" +fileName + "] " + "[" + methodName + "] " + s;
        Log.e(TAG, s + "");
    }

    public static void logd(Object s) {
        StackTraceElement[] stackTrace = Thread.currentThread().getStackTrace();
        String fileName = stackTrace[3].getFileName();
        String methodName = stackTrace[3].getMethodName();
        s = "[" +fileName + "] " + "[" + methodName + "] " + s;
        Log.d(TAG, s + "");
    }
}
