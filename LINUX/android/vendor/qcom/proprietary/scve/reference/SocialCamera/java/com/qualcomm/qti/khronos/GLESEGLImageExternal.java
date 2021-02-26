/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.khronos;

import android.opengl.GLES30;

/**
 * This class exposes the GL_OES_EGL_image_external extensions in Java.
 */
public class GLESEGLImageExternal {
    public static final String[] EXTENSION_NAMES = { "GL_OES_EGL_image_external",
            "GL_OES_EGL_image_external_essl3" };

    public static final int GL_TEXTURE_EXTERNAL_OES = 0x8D65;

    // Not constructable
    public GLESEGLImageExternal() {}

    static {
        String extensions = GLES30.glGetString(GLES30.GL_EXTENSIONS);
        for (String extension : EXTENSION_NAMES) {
            if (!extensions.contains(extension)) {
                throw new RuntimeException(extension + " not supported by GLES implementation");
            }
        }
    }
}
