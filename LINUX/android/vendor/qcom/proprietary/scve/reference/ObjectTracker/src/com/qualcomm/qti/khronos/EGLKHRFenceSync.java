/*
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.khronos;

import android.annotation.TargetApi;
import android.opengl.EGL14;
import android.opengl.EGLDisplay;
import android.opengl.EGLObjectHandle;
import android.os.Build;

/**
 * This class exposes the EGL 1.2 EGL_KHR_fence_sync extension in Java.
 */
public class EGLKHRFenceSync {
    /**
     * This class acts as an opaque token wrapping an EGL KHR fence sync object. It is created and
     * consumed by the {@link EGLKHRFenceSync} class.
     */
    public static class EGLSyncKHR extends EGLObjectHandle {
        private static EGLSyncKHR create(long handle) {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
                return new EGLSyncKHR(handle);
            } else {
                return new EGLSyncKHR((int)handle);
            }
        }

        @SuppressWarnings("deprecation")
        protected EGLSyncKHR(int handle) {
            super(handle);
        }

        @TargetApi(Build.VERSION_CODES.LOLLIPOP)
        protected EGLSyncKHR(long handle) {
            super(handle);
        }

        @Override
        public boolean equals(Object o) {
            if (this == o) return true;
            if (!(o instanceof EGLSyncKHR)) return false;
            return (EGL15Helper.getEGLObjectHandle(this) ==
                    EGL15Helper.getEGLObjectHandle((EGLSyncKHR)o));
        }
    }

    public static final String EXTENSION_NAME = "EGL_KHR_fence_sync";

    public static final EGLSyncKHR EGL_NO_SYNC = EGLSyncKHR.create(0);

    public static final int EGL_SYNC_FENCE_KHR = 0x30F9;
    public static final int EGL_SYNC_TYPE_KHR = 0x30F7;
    public static final int EGL_SYNC_STATUS_KHR = 0x30F1;
    public static final int EGL_SYNC_CONDITION_KHR = 0x30F8;
    public static final int EGL_SIGNALED_KHR = 0x30F2;
    public static final int EGL_UNSIGNALED_KHR = 0x30F3;
    public static final int EGL_SYNC_PRIOR_COMMANDS_COMPLETE_KHR = 0x30F0;
    public static final int EGL_SYNC_FLUSH_COMMANDS_BIT_KHR = 0x0001;
    public static final int EGL_TIMEOUT_EXPIRED_KHR = 0x30F5;
    public static final int EGL_CONDITION_SATISFIED_KHR = 0x30F6;
    public static final long EGL_FOREVER_KHR = 0xFFFFFFFFFFFFFFFFl;

    static {
        String extensions = EGL14.eglQueryString(EGL14.eglGetDisplay(EGL14.EGL_DEFAULT_DISPLAY),
                EGL14.EGL_EXTENSIONS);
        if (!extensions.contains(EXTENSION_NAME)) {
            throw new RuntimeException(EXTENSION_NAME + " not supported by EGL implementation");
        }

        System.loadLibrary("objecttrackerref_khr_egl_extensions");
        nClassInit();
    }

    // Not constructable
    private EGLKHRFenceSync() {}

    public static EGLSyncKHR eglCreateSyncKHR(EGLDisplay dpy, int type, int[] attrib_list) {
        if (dpy == null) {
            throw new IllegalArgumentException("dpy cannot be null");
        } else if (!validateAttribList(attrib_list)) {
            throw new IllegalArgumentException("attrib_list is not valid");
        }

        long eglSync = nEglCreateSyncKHR(EGL15Helper.getEGLObjectHandle(dpy), type, attrib_list);
        if (eglSync == 0) {
            return EGL_NO_SYNC;
        }
        return EGLSyncKHR.create(eglSync);
    }

    public static boolean eglDestroySyncKHR(EGLDisplay dpy, EGLSyncKHR sync) {
        if (dpy == null) {
            throw new IllegalArgumentException("dpy cannot be null");
        } else if (sync == null) {
            throw new IllegalArgumentException("sync cannot be null");
        }

        return nEglDestroySyncKHR(EGL15Helper.getEGLObjectHandle(dpy),
                EGL15Helper.getEGLObjectHandle(sync));
    }

    public static int eglClientWaitSyncKHR(EGLDisplay dpy, EGLSyncKHR sync, int flags,
            long timeout) {
        if (dpy == null) {
            throw new IllegalArgumentException("dpy cannot be null");
        } else if (sync == null) {
            throw new IllegalArgumentException("sync cannot be null");
        }

        return nEglClientWaitSyncKHR(EGL15Helper.getEGLObjectHandle(dpy),
                EGL15Helper.getEGLObjectHandle(sync), flags, timeout);
    }

    public static boolean eglGetSyncAttribKHR(EGLDisplay dpy, EGLSyncKHR sync, int attribute,
            int[] value) {
        if (dpy == null) {
            throw new IllegalArgumentException("dpy cannot be null");
        } else if (sync == null) {
            throw new IllegalArgumentException("sync cannot be null");
        } else if (value == null || value.length != 1) {
            throw new IllegalArgumentException("value must be an int array of length 1");
        }

        return nEglGetSyncAttribKHR(EGL15Helper.getEGLObjectHandle(dpy),
                EGL15Helper.getEGLObjectHandle(sync), attribute, value);
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
    private static native long nEglCreateSyncKHR(long dpy, int type, int[] attrib_list);
    private static native boolean nEglDestroySyncKHR(long dpy, long sync);
    private static native int nEglClientWaitSyncKHR(long dpy, long sync, int flags, long timeout);
    private static native boolean nEglGetSyncAttribKHR(long dpy, long sync, int attribute,
            int[] value);
}
