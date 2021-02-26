/*
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.khronos;

import android.opengl.GLES20;

/**
 * This class exposes the GLES GL_OES_EGL_image and GL_OES_EGL_image_external extensions in Java.
 */
public class GLOES2EGLImage {
    public static final String[] EXTENSION_NAMES = {
        "GL_OES_EGL_image", "GL_OES_EGL_image_external" };

    public static final int GL_TEXTURE_EXTERNAL_OES = 0x8D65;

    // Not constructable
    public GLOES2EGLImage() {}

    static {
        String extensions = GLES20.glGetString(GLES20.GL_EXTENSIONS);
        for (String extension : EXTENSION_NAMES) {
            if (!extensions.contains(extension)) {
                throw new RuntimeException(extension + " not supported by GLES implementation");
            }
        }

        System.loadLibrary("objecttrackerref_khr_gles_extensions");
        nClassInit();
    }

    public static void glEGLImageTargetTexture2DOES(int target, EGLKHRImageBase.EGLImageKHR image) {
        if (image == null) {
            throw new IllegalArgumentException("image cannot be null");
        }
        nGlEGLImageTargetTexture2DOES(target, EGL15Helper.getEGLObjectHandle(image));
    }

    public static void glEGLImageTargetRenderbufferStorageOES(int target,
            EGLKHRImageBase.EGLImageKHR image) {
        if (image == null) {
            throw new IllegalArgumentException("image cannot be null");
        }
        nGlEGLImageTargetRenderbufferStorageOES(target, EGL15Helper.getEGLObjectHandle(image));
    }

    /////////////////////////////////////// NATIVE INTERFACE ///////////////////////////////////////

    private static native void nClassInit();
    private static native void nGlEGLImageTargetTexture2DOES(int target, long image);
    private static native void nGlEGLImageTargetRenderbufferStorageOES(int target, long image);
}
