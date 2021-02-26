/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.qces.khronos;

/**
 * This class adds core GLES30 methods not found in {@link android.opengl.GLES30}
 */
public class GLES30Ext {
    /**
     * Not constructable
     */
    private GLES30Ext() {
    }

    static {
        System.loadLibrary("clevercapture_khr_gles_extensions");
        nClassInit();
    }

    public static void glReadPixels(int x, int y, int width, int height, int format, int type,
            int offset) {
        nGlReadPixels(x, y, width, height, format, type, offset);
    }

    /////////////////////////////////////// NATIVE INTERFACE ///////////////////////////////////////

    private static native void nClassInit();
    private static native void nGlReadPixels(int x, int y, int width, int height, int format,
            int type, int offset);
}
