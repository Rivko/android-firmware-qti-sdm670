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
 * Java wrapper around an EGL sync object
 * @author slaver
 */
public class EGLSyncKHR extends EGLObjectHandle {
    @SuppressWarnings("deprecation")
    private EGLSyncKHR(int handle) {
        super(handle);
    }

    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    private EGLSyncKHR(long handle) {
        super(handle);
    }

    /**
     * Construct an {@link EGLSyncKHR} for the given EGL object
     * @param handle EGL handle to an EGLSyncKHR
     * @return an {@link EGLSyncKHR}
     */
    public static EGLSyncKHR create(long handle) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            return new EGLSyncKHR(handle);
        } else {
            return new EGLSyncKHR((int)handle);
        }
    }
}
