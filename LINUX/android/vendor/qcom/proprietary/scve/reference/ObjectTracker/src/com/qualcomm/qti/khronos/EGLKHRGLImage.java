/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.khronos;

import android.opengl.EGL14;

/**
 * This class exposes the EGL_KHR_gl_*_image extensions in Java. It is intended to work in
 * conjunction with {@link EGLKHRImageBase}.
 */
public class EGLKHRGLImage {
    public static final String[] EXTENSION_NAMES = {
        "EGL_KHR_gl_texture_2D_image", "EGL_KHR_gl_texture_cubemap_image",
        "EGL_KHR_gl_texture_3D_image", "EGL_KHR_gl_renderbuffer_image" };

    public static final int EGL_GL_TEXTURE_2D_KHR = 0x30B1;
    public static final int EGL_GL_TEXTURE_3D_KHR = 0x30B2;
    public static final int EGL_GL_TEXTURE_CUBE_MAP_POSITIVE_X_KHR = 0x30B3;
    public static final int EGL_GL_TEXTURE_CUBE_MAP_NEGATIVE_X_KHR = 0x30B4;
    public static final int EGL_GL_TEXTURE_CUBE_MAP_POSITIVE_Y_KHR = 0x30B5;
    public static final int EGL_GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_KHR = 0x30B6;
    public static final int EGL_GL_TEXTURE_CUBE_MAP_POSITIVE_Z_KHR = 0x30B7;
    public static final int EGL_GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_KHR = 0x30B8;
    public static final int EGL_GL_RENDERBUFFER_KHR = 0x30B9;
    public static final int EGL_GL_TEXTURE_LEVEL_KHR = 0x30BC;
    public static final int EGL_GL_TEXTURE_ZOFFSET_KHR = 0x30BD;

    /**
     * Not constructable
     */
    private EGLKHRGLImage() {
    }

    static {
        String extensions = EGL14.eglQueryString(EGL14.eglGetDisplay(EGL14.EGL_DEFAULT_DISPLAY),
                EGL14.EGL_EXTENSIONS);
        for (String extension : EXTENSION_NAMES) {
            if (!extensions.contains(extension)) {
                throw new RuntimeException(extension + " not supported by EGL implementation");
            }
        }
    }
}
