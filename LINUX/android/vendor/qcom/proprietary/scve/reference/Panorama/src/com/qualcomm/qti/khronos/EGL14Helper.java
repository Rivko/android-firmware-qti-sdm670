/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.khronos;

import android.annotation.TargetApi;
import android.opengl.EGL14;
import android.opengl.EGLObjectHandle;
import android.os.Build;

/**
 * This class provides helper methods for working with {@link EGL14}
 */
public class EGL14Helper {
    // Not constructable
    private EGL14Helper() {}

    /**
     * In Android L, to support 64-bit platforms, {@link EGLObjectHandle#getHandle()} was deprecated
     * in favor of {@link EGLObjectHandle#getNativeHandle()}. This helper allows retrieving the
     * handle as a long, regardless of target platform version.
     * @param eglObject the {@link EGLObjectHandle} for which to retrieve the native handle
     * @return the native handle for eglObject
     */
    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    @SuppressWarnings("deprecation")
    public static long getEGLObjectHandle(EGLObjectHandle eglObject) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            return eglObject.getNativeHandle();
        } else {
            return eglObject.getHandle();
        }
    }
}
