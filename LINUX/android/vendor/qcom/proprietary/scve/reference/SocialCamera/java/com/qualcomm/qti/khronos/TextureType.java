/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.khronos;

import android.opengl.GLES30;

/**
 * This enum lists 2D texture types for GLES, and provides useful parameters for working with them
 */
public enum TextureType {
    /**
     * Corresponds to the {@link GLES30#GL_TEXTURE_2D} texture target
     */
    TEXTURE_2D(GLES30.GL_TEXTURE_2D, "sampler2D"),

    /**
     * Corresponds to the {@link GLESEGLImageExternal#GL_TEXTURE_EXTERNAL_OES} texture target
     */
    TEXTURE_EXTERNAL(GLESEGLImageExternal.GL_TEXTURE_EXTERNAL_OES, "samplerExternalOES");

    public final int textureTarget;
    public final String sampler;

    TextureType(int textureTarget, String sampler) {
        this.textureTarget = textureTarget;
        this.sampler = sampler;
    }
}