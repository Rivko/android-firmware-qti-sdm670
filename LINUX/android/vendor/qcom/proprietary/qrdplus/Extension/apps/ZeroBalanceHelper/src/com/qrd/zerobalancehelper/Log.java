/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qrd.zerobalancehelper;

/**
 * Custom Log class to enable disable Log based on flag.
 *
 */

public class Log {
    private static final boolean DEBUG_FLAG = false;

    public static void i(String tag, String msg) {
        if (DEBUG_FLAG) {
            android.util.Log.i(tag, msg);
        }
    }

    public static void w(String tag, String msg) {
        if (DEBUG_FLAG) {
            android.util.Log.w(tag, msg);
        }
    }

    public static void e(String tag, String msg) {
        // Error log should always be displayed.
        android.util.Log.e(tag, msg);
    }

    public static void d(String tag, String msg) {
        if (DEBUG_FLAG) {
            android.util.Log.d(tag, msg);
        }
    }

    public static void v(String tag, String msg) {
        if (DEBUG_FLAG) {
            android.util.Log.v(tag, msg);
        }
    }

}
