/*
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.khronos;

import android.annotation.TargetApi;
import android.graphics.SurfaceTexture;
import android.opengl.EGLConfig;
import android.opengl.EGLContext;
import android.opengl.EGLDisplay;
import android.opengl.EGLObjectHandle;
import android.opengl.EGLSurface;
import android.os.Build;

/**
 * This class provides utility methods to assist in working with {@link EGL15}
 */
public class EGL15Helper {
    /**
     * An {@link EGLContextToken} wraps all the members related to a specific {@link EGLContext},
     * and provides the unit on which the {@link EGL15Helper} class operates
     */
    public static class EGLContextToken {
        public EGLDisplay display;
        public EGLSurface surface;
        public EGLContext context;

        private SurfaceTexture dummyOutputTexture;

        @Override
        protected void finalize() throws Throwable {
            super.finalize();
            releaseEGLContext(this); // N.B. safe to call even if context is already released
        }
    }

    /**
     * Not constructable
     */
    private EGL15Helper() {
    }

    /**
     * Create and initialize a GLES30 EGL context for rendering to the provided surface. If no
     * surface is provided, the default surface bound to this EGL context will be a dummy 1x1
     * surface, which should never actually be used for any sort of real rendering. The returned
     * context is not initially current on any thread.
     * @param window The window against which to render. May be null, in which case a 1x1 dummy
     *      surface will be created. If not null, it should be an instance of {@link Surface},
     *      {@link SurfaceHolder}, or {@link SurfaceTexture}.
     * @param recordable true if the provided window was produced by {@link MediaCodec} for
     *      recording, else false
     * @return a new {@link EGLContextToken} representing the GLES30 EGL context. It can be used
     *      to make the context current on the calling thread, or to release the resources
     *      associated with the context.
     */
    public static EGLContextToken initializeGLES30EGLContext(Object window, boolean recordable) {
        EGLContextToken token = new EGLContextToken();

        // N.B. This config won't actually be used for rendering; it points to a dummy surface.
        final int[] CONFIG_ATTRIBUTES = new int[] {
            EGL15.EGL_RENDERABLE_TYPE, EGL15.EGL_OPENGL_ES3_BIT,
            EGL15.EGL_SURFACE_TYPE, EGL15.EGL_WINDOW_BIT,
            EGL15.EGL_COLOR_BUFFER_TYPE, EGL15.EGL_RGB_BUFFER,
            EGL15.EGL_RED_SIZE, 8,
            EGL15.EGL_GREEN_SIZE, 8,
            EGL15.EGL_BLUE_SIZE, 8,
            EGL15.EGL_ALPHA_SIZE, 0,
            EGLAndroidRecordable.EGL_RECORDABLE_ANDROID,
                    (window != null && recordable ? EGL15.EGL_TRUE : EGL15.EGL_DONT_CARE),
            EGL15.EGL_NONE
        };

        final int[] CONTEXT_ATTRIBUTES = new int[] {
            EGL15.EGL_CONTEXT_CLIENT_VERSION, 3,
            EGL15.EGL_NONE
        };

        final int[] EMPTY_ATTRIBUTES = new int[] {
            EGL15.EGL_NONE
        };

        token.display = EGL15.eglGetDisplay(EGL15.EGL_DEFAULT_DISPLAY);
        if (token.display == null || token.display.equals(EGL15.EGL_NO_DISPLAY)) {
            throw new RuntimeException("Failed initializing EGL display");
        }

        // N.B. Android has a modified version of eglInitialize, which reference-counts the
        // number of EGL initializations. It is thus required for every thread which uses
        // EGL to initialize it.
        int[] major = new int[1];
        int[] minor = new int[1];
        if (!EGL15.eglInitialize(token.display, major, 0, minor, 0)) {
            throw new RuntimeException("Failed initializing EGL display connection");
        }

        int[] numConfigs = new int[1];
        EGLConfig[] configs = new EGLConfig[1];
        if (!EGL15.eglChooseConfig(token.display, CONFIG_ATTRIBUTES, 0, configs, 0, configs.length,
                numConfigs, 0)) {
            throw new RuntimeException("Failed querying configs");
        } else if (numConfigs[0] != 1) {
            throw new RuntimeException("No matching EGL configurations found");
        }

        if (window == null) {
            token.dummyOutputTexture = new SurfaceTexture(0);
            token.dummyOutputTexture.detachFromGLContext();
            token.dummyOutputTexture.setDefaultBufferSize(1, 1);
            window = token.dummyOutputTexture;
        }

        token.surface = EGL15.eglCreateWindowSurface(token.display, configs[0], window,
                EMPTY_ATTRIBUTES, 0);
        if (token.surface == null || token.surface.equals(EGL15.EGL_NO_SURFACE)) {
            throw new RuntimeException("Failed creating EGL surface from Android SurfaceTexture");
        }

        token.context = EGL15.eglCreateContext(token.display, configs[0], EGL15.EGL_NO_CONTEXT,
                CONTEXT_ATTRIBUTES, 0);
        if (token.context == null || token.context.equals(EGL15.EGL_NO_CONTEXT)) {
            throw new RuntimeException("Failed creating OpenGL ES3 context");
        }

        return token;
    }

    /**
     * Create and initialize a GLES30 EGL context. The default surface bound to this EGL context is
     * a dummy 1x1 surface, which should never actually be used for any sort of real rendering. The
     * returned context is not initially current on any thread.
     * @return a new {@link EGLContextToken} representing the GLES30 EGL context. It can be used
     *      to make the context current on the calling thread, or to release the resources
     *      associated with the context.
     */
    public static EGLContextToken initializeGLES30EGLContext() {
        return initializeGLES30EGLContext(null, false);
    }

    /**
     * Makes the provided EGL context current on the calling thread. All normal rules about context
     * selection & concurrency apply.
     * @param token an {@link EGLContextToken} detailing the EGL context to make current
     * @throws IllegalArgumentException if token is null
     */
    public static void makeEGLContextCurrent(EGLContextToken token) {
        if (token == null) {
            throw new IllegalArgumentException("token cannot be null");
        }

        if (!EGL15.eglMakeCurrent(token.display, token.surface, token.surface, token.context)) {
            throw new RuntimeException("Failed making context current");
        }
    }

    /**
     * Release the resources for the provided EGL context
     * @param token an {@link EGLContextToken} detailing the EGL context to release. After invoking
     *      this method, all members of token will be null.
     * @throws IllegalArgumentException if token is null
     */
    public static void releaseEGLContext(EGLContextToken token) {
        if (token == null) {
            throw new IllegalArgumentException("token cannot be null");
        }

        if (token.display != null) {
            if (token.context != null) {
                EGLContext currentContext = EGL15.eglGetCurrentContext();
                if (token.context.equals(currentContext)) {
                    EGL15.eglMakeCurrent(token.display, EGL15.EGL_NO_SURFACE, EGL15.EGL_NO_SURFACE,
                            EGL15.EGL_NO_CONTEXT);
                }

                EGL15.eglDestroyContext(token.display, token.context);
                token.context = null;
            }
            if (token.surface != null) {
                EGL15.eglDestroySurface(token.display, token.surface);
                token.surface = null;
            }

            // N.B. Android has a modified version of eglTerminate, which reference-counts the
            // number of EGL initializations. It is thus appropriate for every thread which uses
            // EGL to terminate when it is complete.
            EGL15.eglTerminate(token.display);
            token.display = null;
        }

        if (token.dummyOutputTexture != null) {
            token.dummyOutputTexture.release();
            token.dummyOutputTexture = null;
        }
    }

    /**
     * In Android L, to support 64-bit platforms, {@link EGLObjectHandle#getHandle()} was deprecated
     * in favor of {@link EGLObjectHandle#getNativeHandle()}. This helper allows retrieving the
     * handle as a long, regardless of target platform version.
     * @param eglObject the {@link EGLObjectHandle} for which to retrieve the native handle
     * @return the native handle for eglObject
     */
    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    @SuppressWarnings("deprecation")
    public static long getEGLObjectHandle(EGLObjectHandle eglObject) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            return eglObject.getNativeHandle();
        } else {
            return eglObject.getHandle();
        }
    }
}
