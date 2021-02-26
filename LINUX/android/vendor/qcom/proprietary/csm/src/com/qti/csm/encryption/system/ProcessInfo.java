/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qti.csm.encryption.system;

import android.content.Context;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.widget.Toast;

public class ProcessInfo {
    private static final boolean INFO_ENC = false;

    public static void loge(Object s) {

        Thread thread = Thread.currentThread();
        StackTraceElement[] mStackTrace = thread.getStackTrace();
        String methodName = mStackTrace[3].getMethodName();
        String className = mStackTrace[3].getClass().getSimpleName();
        long threadId = thread.getId();

        s = "[" + threadId + ": " + className + "." + methodName + "] " + s;
        Log.e(SystemInfo.TAG, s + "");
    }

    public static void toast(final Context context, final Object s) {

        if (s == null)
            return;
        Handler handler = new Handler(Looper.getMainLooper());
        handler.post(new Runnable() {
            public void run() {
                Toast.makeText(context, s + "", Toast.LENGTH_SHORT).show();
            }
        });
    }

    public static void logd() {

        if (!INFO_ENC)
            return;
        Thread thread = Thread.currentThread();
        StackTraceElement[] mStackTrace = thread.getStackTrace();
        String methodName = mStackTrace[3].getMethodName();
        String className = mStackTrace[3].getClassName();
        className = className.substring(className.lastIndexOf('.') + 1);
        long threadId = thread.getId();
        String s = "[" + threadId + ": " + className + "." + methodName + "] ";
        Log.d(SystemInfo.TAG, s + "");
    }

    public static void logd(Object s) {

        if (!INFO_ENC)
            return;
        Thread thread = Thread.currentThread();
        StackTraceElement[] mStackTrace = thread.getStackTrace();
        String methodName = mStackTrace[3].getMethodName();
        String className = mStackTrace[3].getClassName();
        className = className.substring(className.lastIndexOf('.') + 1);
        long threadId = thread.getId();
        s = "[" + threadId + ": " + className + "." + methodName + "] " + s;
        Log.d(SystemInfo.TAG, s + "");
    }


}

