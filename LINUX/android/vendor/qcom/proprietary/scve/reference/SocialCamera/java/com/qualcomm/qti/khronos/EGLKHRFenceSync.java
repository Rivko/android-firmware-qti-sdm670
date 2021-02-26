/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.khronos;

import android.opengl.EGL14;
import android.opengl.EGLDisplay;

/**
 * This class exposes the EGL_KHR_fence_sync extension in Java. It is intended to work in
 * conjunction with {@link EGL14}.
 */
public class EGLKHRFenceSync {
    public static final String EXTENSION_NAME = "EGL_KHR_fence_sync";

    public static final EGLSyncKHR EGL_NO_SYNC = new EGLSyncKHR(0);

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

    /**
     * Not constructable
     */
    private EGLKHRFenceSync() {
    }

    static {
        String extensions = EGL14.eglQueryString(EGL14.eglGetDisplay(
                EGL14.EGL_DEFAULT_DISPLAY), EGL14.EGL_EXTENSIONS);
        if (!extensions.contains(EXTENSION_NAME)) {
            throw new RuntimeException(EXTENSION_NAME + " not supported by EGL implementation");
        }

        System.loadLibrary("socialcamera_khr_egl_extensions");
        _nativeClassInit();
    }

    public static EGLSyncKHR eglCreateSyncKHR(EGLDisplay dpy, int type, int[] attrib_list) {
        long eglSync = _eglCreateSyncKHR(dpy.getNativeHandle(), type, attrib_list);
        return (eglSync == 0 ? EGL_NO_SYNC : new EGLSyncKHR(eglSync));
    }

    public static boolean eglDestroySyncKHR(EGLDisplay dpy, EGLSyncKHR sync) {
        return _eglDestroySyncKHR(dpy.getNativeHandle(), sync.getNativeHandle());
    }

    public static int eglClientWaitSyncKHR(EGLDisplay dpy, EGLSyncKHR sync, int flags,
            long timeout) {
        return _eglClientWaitSyncKHR(dpy.getNativeHandle(), sync.getNativeHandle(), flags, timeout);
    }

    public static boolean eglGetSyncAttribKHR(EGLDisplay dpy, EGLSyncKHR sync, int attribute,
            int[] value) {
        return _eglGetSyncAttribKHR(dpy.getNativeHandle(), sync.getNativeHandle(), attribute,
                value);
    }

    protected static native void _nativeClassInit();
    protected static native long _eglCreateSyncKHR(long dpy, int type, int[] attrib_list);
    protected static native boolean _eglDestroySyncKHR(long dpy, long sync);
    protected static native int _eglClientWaitSyncKHR(long dpy, long sync, int flags, long timeout);
    protected static native boolean _eglGetSyncAttribKHR(long dpy, long sync, int attribute,
            int[] value);
}
