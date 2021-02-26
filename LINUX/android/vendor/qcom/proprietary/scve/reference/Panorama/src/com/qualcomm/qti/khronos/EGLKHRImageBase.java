/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.khronos;

import android.opengl.EGL14;
import android.opengl.EGLContext;
import android.opengl.EGLDisplay;

/**
 * This class exposes the EGL 1.2 EGL_KHR_image_base extension in Java. It is
 * intended to work in conjunction with {@link EGL14}.
 * @author slaver
 */
public class EGLKHRImageBase {

    public static final String EXTENSION_NAME = "EGL_KHR_image_base";

    public static final EGLImageKHR EGL_NO_IMAGE_KHR = EGLImageKHR.create(0);

    public static final int EGL_IMAGE_PRESERVED_KHR = 0x30D2;

    /**
     * Class not constructable
     */
    private EGLKHRImageBase() {
    }

    protected static native void _nativeClassInit();
    static {
        String extensions = EGL14.eglQueryString(EGL14.eglGetDisplay(
                EGL14.EGL_DEFAULT_DISPLAY), EGL14.EGL_EXTENSIONS);
        if (!extensions.contains(EXTENSION_NAME)) {
            throw new RuntimeException(EXTENSION_NAME + " not supported by EGL implementation");
        }

        System.loadLibrary("panoramaref_khr_egl_extensions");
        _nativeClassInit();
    }

    public static EGLImageKHR eglCreateImageKHR(EGLDisplay dpy, EGLContext ctx,
            int target, int buffer, int[] attrib_list) {
        long im = _eglCreateImageKHR(EGL14Helper.getEGLObjectHandle(dpy),
                EGL14Helper.getEGLObjectHandle(ctx), target, buffer, attrib_list);
        return (im == 0 ? EGL_NO_IMAGE_KHR : EGLImageKHR.create(im));
    }

    public static boolean eglDestroyImageKHR(EGLDisplay dpy, EGLImageKHR image) {
        return _eglDestroyImageKHR(EGL14Helper.getEGLObjectHandle(dpy),
                EGL14Helper.getEGLObjectHandle(image));
    }

    protected static native long _eglCreateImageKHR(long dpy, long ctx, int target, int buffer,
            int[] attrib_list);
    protected static native boolean _eglDestroyImageKHR(long dpy, long image);
}
