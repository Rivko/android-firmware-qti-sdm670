/*
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.khronos;

import android.annotation.TargetApi;
import android.opengl.EGL14;
import android.opengl.EGLContext;
import android.opengl.EGLDisplay;
import android.opengl.EGLObjectHandle;
import android.os.Build;

/**
 * This class exposes the EGL 1.2 EGL_KHR_image_base extension in Java.
 */
public class EGLKHRImageBase {
    /**
     * This class acts as an opaque token wrapping an EGL KHR image object. It is created and
     * consumed by the {@link EGLKHRImageBase} class.
     */
    public static class EGLImageKHR extends EGLObjectHandle {
        private static EGLImageKHR create(long handle) {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
                return new EGLImageKHR(handle);
            } else {
                return new EGLImageKHR((int)handle);
            }
        }

        @SuppressWarnings("deprecation")
        protected EGLImageKHR(int handle) {
            super(handle);
        }

        @TargetApi(Build.VERSION_CODES.LOLLIPOP)
        protected EGLImageKHR(long handle) {
            super(handle);
        }

        @Override
        public boolean equals(Object o) {
            if (this == o) return true;
            if (!(o instanceof EGLImageKHR)) return false;
            return (EGL15Helper.getEGLObjectHandle(this) ==
                    EGL15Helper.getEGLObjectHandle((EGLImageKHR)o));
        }
    }

    public static final String EXTENSION_NAME = "EGL_KHR_image_base";

    public static final EGLImageKHR EGL_NO_IMAGE_KHR = EGLImageKHR.create(0);

    public static final int EGL_IMAGE_PRESERVED_KHR = 0x30D2;

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
    private EGLKHRImageBase() {}

    public static EGLImageKHR eglCreateImageKHR(EGLDisplay dpy, EGLContext ctx, int target,
            int buffer, int[] attrib_list) {
        if (dpy == null) {
            throw new IllegalArgumentException("dpy cannot be null");
        } else if (ctx == null) {
            throw new IllegalArgumentException("ctx cannot be null");
        } else if (!validateAttribList(attrib_list)) {
            throw new IllegalArgumentException("attrib_list is not valid");
        }

        long eglImage = nEglCreateImageKHR(EGL15Helper.getEGLObjectHandle(dpy),
                EGL15Helper.getEGLObjectHandle(ctx), target, buffer, attrib_list);
        if (eglImage == 0) {
            return EGL_NO_IMAGE_KHR;
        }
        return EGLImageKHR.create(eglImage);
    }

    public static boolean eglDestroyImageKHR(EGLDisplay dpy, EGLImageKHR image) {
        if (dpy == null) {
            throw new IllegalArgumentException("dpy cannot be null");
        } else if (image == null) {
            throw new IllegalArgumentException("image cannot be null");
        }
        return nEglDestroyImageKHR(EGL15Helper.getEGLObjectHandle(dpy),
                EGL15Helper.getEGLObjectHandle(image));
    }

    private static boolean validateAttribList(int[] attrib_list) {
        if (attrib_list == null) {
            return true;
        } else if (attrib_list.length % 2 != 1) {
            return false;
        } else {
            return (attrib_list[attrib_list.length - 1] == EGL14.EGL_NONE);
        }
    }

    /////////////////////////////////////// NATIVE INTERFACE ///////////////////////////////////////

    private static native void nClassInit();
    private static native long nEglCreateImageKHR(long dpy, long ctx, int target, int buffer,
            int[] attrib_list);
    private static native boolean nEglDestroyImageKHR(long dpy, long image);
}
