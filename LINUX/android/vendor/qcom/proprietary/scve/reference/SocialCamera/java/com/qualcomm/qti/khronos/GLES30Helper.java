/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.khronos;

import android.opengl.GLES30;

/**
 * Helper functions for {@link GLES30}
 */
public class GLES30Helper {
    /**
     * Not constructable
     */
    private GLES30Helper() {
    }

    /**
     * Helper function to compile a GLES 3.0 shader from source
     * @param type Type of shader being compiled (e.g. {@link GLES30#GL_VERTEX_SHADER}, etc)
     * @param shaderSource GLSL source code for the shader
     * @return GLES handle to the compiled shader object
     * @throws IllegalArgumentException if type is not {@link GLES30#GL_VERTEX_SHADER} or
     *      {@link GLES30#GL_FRAGMENT_SHADER}
     * @throws RuntimeException if shaderSource fails to compile
     */
    public static int compileShader(int type, String shaderSource) {
        if (type != GLES30.GL_VERTEX_SHADER && type != GLES30.GL_FRAGMENT_SHADER) {
            throw new IllegalArgumentException("Unexpected shader type");
        }

        int shader = GLES30.glCreateShader(type);
        GLES30.glShaderSource(shader, shaderSource);
        GLES30.glCompileShader(shader);

        final int[] compileStatus = new int[1];
        GLES30.glGetShaderiv(shader, GLES30.GL_COMPILE_STATUS, compileStatus, 0);
        if (compileStatus[0] != GLES30.GL_TRUE) {
            throw new RuntimeException("Failed compiling " + type + " shader! Log:" +
                    GLES30.glGetShaderInfoLog(shader));
        }

        return shader;
    }

    /**
     * Helper function to generate a GL texture name and set default texture parameters.
     * @param texTarget GL texture target for which to generate a texture name
     * @return A GL name for a new texture, with GL_LINEAR minification and magnification filtering,
     *      and GL_CLAMP_TO_EDGE wrap behaviour
     */
    public static int generateTexture(int texTarget) {
        int[] tex = new int[1];
        GLES30.glGenTextures(1, tex, 0);
        GLES30.glBindTexture(texTarget, tex[0]);
        GLES30.glTexParameteri(texTarget, GLES30.GL_TEXTURE_MIN_FILTER, GLES30.GL_LINEAR);
        GLES30.glTexParameteri(texTarget, GLES30.GL_TEXTURE_MAG_FILTER, GLES30.GL_LINEAR);
        GLES30.glTexParameteri(texTarget, GLES30.GL_TEXTURE_WRAP_S, GLES30.GL_CLAMP_TO_EDGE);
        GLES30.glTexParameteri(texTarget, GLES30.GL_TEXTURE_WRAP_T, GLES30.GL_CLAMP_TO_EDGE);
        return tex[0];
    }
}
