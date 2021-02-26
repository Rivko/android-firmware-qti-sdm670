/**
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.wrapper;

import android.os.SystemProperties;

public class SystemProperties_Wrapper {

    public static boolean getBoolean(String key, boolean def) {
        return SystemProperties.getBoolean(key, def);
    }

    public static void set(String key, String val) {
        SystemProperties.set(key, val);
    }

    public static String get(String key) {
        return SystemProperties.get(key);
    }
}
