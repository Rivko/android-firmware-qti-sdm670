/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.khronos;

import android.opengl.EGL14;

/**
 * This class exposes the EGL_ANDROID_recordable extension in Java
 */
public class EGLAndroidRecordable {
    public static final String EXTENSION_NAME = "EGL_ANDROID_recordable";

    public static final int EGL_RECORDABLE_ANDROID = 0x3142;

    /**
     * Not constructable
     */
    private EGLAndroidRecordable() {
    }

    static {
        String extensions = EGL14.eglQueryString(EGL14.eglGetDisplay(EGL14.EGL_DEFAULT_DISPLAY),
                EGL14.EGL_EXTENSIONS);
        if (!extensions.contains(EXTENSION_NAME)) {
            throw new RuntimeException(EXTENSION_NAME + " not supported by EGL implementation");
        }
    }
}
