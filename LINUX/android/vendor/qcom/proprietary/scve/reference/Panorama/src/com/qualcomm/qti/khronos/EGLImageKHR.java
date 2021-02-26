/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.khronos;

import android.annotation.TargetApi;
import android.opengl.EGLObjectHandle;
import android.os.Build;

/**
 * Java wrapper around an EGL image object
 * @author slaver
 */
public class EGLImageKHR extends EGLObjectHandle {
    @SuppressWarnings("deprecation")
    private EGLImageKHR(int handle) {
        super(handle);
    }

    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    private EGLImageKHR(long handle) {
        super(handle);
    }

    /**
     * Construct an {@link EGLImageKHR} for the given EGL object
     * @param handle EGL handle to an EGLImageKHR
     * @return an {@link EGLImageKHR}
     */
    public static EGLImageKHR create(long handle) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            return new EGLImageKHR(handle);
        } else {
            return new EGLImageKHR((int)handle);
        }
    }}
