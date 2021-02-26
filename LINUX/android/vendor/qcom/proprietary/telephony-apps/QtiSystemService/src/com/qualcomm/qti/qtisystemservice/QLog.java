/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qtisystemservice;

import android.telecom.Log;

public class QLog {
    private static final String TAG = "QtiSystemService";
    private static final boolean LOCAL_DEBUG = true;

    public static void d(Object objectPrefix, String format, Object... args) {
        if (LOCAL_DEBUG) {
            android.util.Log.d(TAG, format);
        } else {
            Log.d(objectPrefix, format, args);
        }
    }

    public static void i(Object objectPrefix, String format, Object... args) {
        if (LOCAL_DEBUG) {
            android.util.Log.i(TAG, format);
        } else {
            Log.i(objectPrefix, format, args);
        }
    }

    public static void v(Object objectPrefix, String format, Object... args) {
        if (LOCAL_DEBUG) {
            android.util.Log.v(TAG, format);
        } else {
            Log.v(objectPrefix, format, args);
        }
    }

    public static void w(Object objectPrefix, String format, Object... args) {
        if (LOCAL_DEBUG) {
            android.util.Log.w(TAG, format);
        } else {
            Log.w(objectPrefix, format, args);
        }
    }

    public static void e(Object objectPrefix, Throwable tr, String format, Object... args) {
        if (LOCAL_DEBUG) {
            android.util.Log.e(TAG, format);
        } else {
            Log.e(objectPrefix, tr, format, args);
        }
    }
}
