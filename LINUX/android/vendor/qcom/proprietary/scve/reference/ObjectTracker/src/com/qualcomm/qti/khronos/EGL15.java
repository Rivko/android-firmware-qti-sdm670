/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.khronos;

import android.opengl.EGL14;

/**
 * This class provides a subset of EGL 1.5 functionality, for use in conjunction with {@link EGL14}
 */
public class EGL15 extends EGL14 {
    public static final int EGL_DONT_CARE = -1;
    public static final int EGL_OPENGL_ES3_BIT = 0x40;

    // Not constructable
    private EGL15() {}
}
