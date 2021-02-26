/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.qces.khronos;

/**
 * Java wrapper around an EGL sync object
 * @author slaver
 */
public class EGLSyncKHR {

    @SuppressWarnings("unused")
    private int mEGLSync;

    /**
     * Creates a Java wrapper around an EGL sync object
     * @param eglSync EGL object handle, from {@link EGLKHRFenceSync#
     *      eglCreateSyncKHR(EGLDisplay, int, int[])}
     */
    public EGLSyncKHR(int eglSync) {
        mEGLSync = eglSync;
    }
}
