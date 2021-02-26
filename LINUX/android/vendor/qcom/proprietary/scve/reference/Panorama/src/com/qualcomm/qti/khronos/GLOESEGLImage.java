/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.khronos;

import android.opengl.GLES30;

/**
 * This class exposes the GLES 2.0+ GL_OES_EGL_image and
 * GL_OES_EGL_image_external extensions in Java. It is intended to work in
 * conjunction with {@link GLES30}.
 * @author slaver
 */
public class GLOESEGLImage {

    public static final String EXTENSION_NAMES[] = {
        "GL_OES_EGL_image", "GL_OES_EGL_image_external" };

    public static final int GL_TEXTURE_EXTERNAL_OES = 0x8D65;

    /**
     * Not constructable
     */
    public GLOESEGLImage() {
    }

    protected static native void _nativeClassInit();
    static {
        String extensions = GLES30.glGetString(GLES30.GL_EXTENSIONS);
        for (String extension : EXTENSION_NAMES) {
            if (!extensions.contains(extension)) {
                throw new RuntimeException(extension +
                        " not supported by GLES 3.0 implementation");
            }
        }

        System.loadLibrary("panoramaref_khr_gles_extensions");
        _nativeClassInit();
    }

    public static native void EGLImageTargetTexture2DOES(int target,
            EGLImageKHR image);
    public static native void EGLImageTargetRenderbufferStorageOES(int target,
            EGLImageKHR image);
}
