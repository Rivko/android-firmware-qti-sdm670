/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.khronos;

import android.opengl.EGLObjectHandle;

/**
 * This class exposes an EGL sync fence handle in Java
 */
public class EGLSyncKHR extends EGLObjectHandle {
    /*package*/ EGLSyncKHR(long handle) {
        super(handle);
    }
}
