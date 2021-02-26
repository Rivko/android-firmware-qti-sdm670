/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.khronos;

import android.opengl.EGL14;
import android.opengl.EGLDisplay;
import android.opengl.EGLSurface;

/**
 * This class exposes the EGL 1.4 EGL_ANDROID_presentation_time extension in Java.
 */
public class EGLAndroidPresentationTime {
    public static final String EXTENSION_NAME = "EGL_ANDROID_presentation_time";

    static {
        String extensions = EGL14.eglQueryString(EGL14.eglGetDisplay(
                EGL14.EGL_DEFAULT_DISPLAY), EGL14.EGL_EXTENSIONS);
        if (!extensions.contains(EXTENSION_NAME)) {
            throw new RuntimeException(EXTENSION_NAME + " not supported by EGL implementation");
        }

        System.loadLibrary("objecttrackerref_khr_egl_extensions");
        nClassInit();
    }

    // Not constructable
    private EGLAndroidPresentationTime() {}

    public static boolean eglPresentationTimeANDROID(EGLDisplay dpy, EGLSurface sur,
            long presentationTimeNs) {
        if (dpy == null) {
            throw new IllegalArgumentException("dpy cannot be null");
        } else if (sur == null) {
            throw new IllegalArgumentException("sur cannot be null");
        }

        return nEglPresentationTimeANDROID(EGL15Helper.getEGLObjectHandle(dpy),
                EGL15Helper.getEGLObjectHandle(sur), presentationTimeNs);
    }

    /////////////////////////////////////// NATIVE INTERFACE ///////////////////////////////////////

    private static native void nClassInit();
    private static native boolean nEglPresentationTimeANDROID(long dpy, long sur,
            long presentationTimeNs);
}
