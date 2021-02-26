/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.objecttrackerdemo.renderer.primitives;

import android.opengl.GLES30;

import com.qualcomm.qti.khronos.GLOES2EGLImage;

/**
 * {@link TextureFormat} describes the types of textures that can be rendered by an {@link
 * BaseTextureQuad}
 */
public enum TextureFormat {
    /**
     * RGBA: In-memory order R, G, B, A
     */
    RGBA(GLES30.GL_TEXTURE_2D, "xyzw"),

    /**
     * RGBA: In-memory order B, G, R, A
     */
    BGRA(GLES30.GL_TEXTURE_2D, "zyxw"),

    /**
     * NV12: In-memory order is Y-plane (8-bit) followed by interleaved UV plane (2x 8-bit). The
     * UV plane is subsampled by a factor of 2 both horizontally and vertically.
     */
    NV12(GLES30.GL_TEXTURE_2D, "x", "xy"),

    /**
     * NV21: In-memory order is Y-plane (8-bit) followed by interleaved VU plane (2x 8-bit). The
     * VU plane is subsampled by a factor of 2 both horizontally and vertically.
     */
    NV21(GLES30.GL_TEXTURE_2D, "x", "yx"),

    /**
     * External: described by the OES_EGL_image_external GL extension
     */
    EXTERNAL(GLOES2EGLImage.GL_TEXTURE_EXTERNAL_OES, "xyzw");

    /**
     * {@link GLES30} texture target for this texture type
     */
    public final int glTextureTarget;

    /**
     * GLSL sampler name for this texture type
     */
    /*package*/ final String samplerName;

    /**
     * Colour component order to use when sampling from samplerName
     */
    /*package*/ final String[] channelSwizzle;

    private TextureFormat(int glTextureTarget, String ... channelSwizzle) {
        this.glTextureTarget = glTextureTarget;
        this.channelSwizzle = channelSwizzle;

        switch (glTextureTarget) {
        case GLES30.GL_TEXTURE_2D:
            this.samplerName = "sampler2D";
            break;

        case GLOES2EGLImage.GL_TEXTURE_EXTERNAL_OES:
            this.samplerName = "samplerExternalOES";
            break;

        default:
            throw new RuntimeException("Unexpected texture target " + this.glTextureTarget);
        }
    }
}
