/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.qces.khronos;

/**
 * Java wrapper around an EGL image object
 * @author slaver
 */
public class EGLImageKHR {

    @SuppressWarnings("unused")
    private int mEGLImage;

    /**
     * Creates a Java wrapper around an EGL image object
     * @param eglImage EGL image handle, from {@link EGLKHRImageBase#
     *      eglCreateImageKHR(EGLDisplay, EGLContext, int, int, int[])}
     */
    public EGLImageKHR(int eglImage) {
        mEGLImage = eglImage;
    }
}
