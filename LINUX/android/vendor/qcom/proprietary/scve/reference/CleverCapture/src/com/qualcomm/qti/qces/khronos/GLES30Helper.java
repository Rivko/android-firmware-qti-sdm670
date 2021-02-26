/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.qces.khronos;

import android.opengl.GLES30;

/**
 * Helper functions for {@link GLES30}
 */
public class GLES30Helper {

    /**
     * The maximum number of color attachments supported by {@link GLES30Helper}. OpenGL ES 3.0
     * guarantees that all implementations will allow at least 4 color attachments, thus we use
     * that as our maximum.
     */
    public static final int MAX_COLOR_ATTACHMENTS = 4;

    /**
     * The set of color attachments which may be bound by the FBO methods in this class. Will always
     * contain exactly {@link #MAX_COLOR_ATTACHMENTS} elements, starting from
     * {@link GLES30#GL_COLOR_ATTACHMENT0}.
     */
    public static final int[] GL_COLOR_BUFFERS = new int[] { GLES30.GL_COLOR_ATTACHMENT0,
            GLES30.GL_COLOR_ATTACHMENT1, GLES30.GL_COLOR_ATTACHMENT2, GLES30.GL_COLOR_ATTACHMENT3 };

    /**
     * Token class to save the state of the current FBO before replacing it
     * with a new configuration
     */
    public static class SavedFboToken {
        private final int[] fbo = new int[2];
        private final int[] viewport = new int[4];
    }

    /**
     * This class wraps a GL FBO with additional metadata describing the size and number of
     * attachments to the FBO
     */
    public static class FboDescriptor {
        private final int fbo;
        private final int x;
        private final int y;
        private final int width;
        private final int height;
        private final int numColorAttachments;

        private FboDescriptor(int fbo, int x, int y, int width, int height,
                int numColorAttachments) {
            this.fbo = fbo;
            this.x = x;
            this.y = y;
            this.width = width;
            this.height = height;
            this.numColorAttachments = numColorAttachments;
        }
    }

    /**
     * Not constructable
     */
    private GLES30Helper() {
    }

    /*
     * Helper function to compile a GLES 2.0 shader from source
     * @param type Type of shader being compiled.
     * @param shaderSource Source code of shader
     * @return GLES handle to shader
     */
    public static int compileShader(int type, String shaderSource) {
        if (type != GLES30.GL_VERTEX_SHADER && type != GLES30.GL_FRAGMENT_SHADER) {
            throw new RuntimeException("Unexpected shader type");
        }

        int shader = GLES30.glCreateShader(type);
        GLES30.glShaderSource(shader, shaderSource);
        GLES30.glCompileShader(shader);

        final int[] compileStatus = new int[1];
        GLES30.glGetShaderiv(shader, GLES30.GL_COMPILE_STATUS, compileStatus, 0);
        if (compileStatus[0] != GLES30.GL_TRUE) {
            throw new RuntimeException("Failed compiling " + GLES30.GL_VERTEX_SHADER +
                    " shader! Log:" + GLES30.glGetShaderInfoLog(shader));
        }

        return shader;
    }

    /**
     * Helper function to generate a GL texture name and set default texture
     * parameters.
     * @return A GL name for a new texture, with GL_LINEAR minification and
     *     magnification filtering, and GL_CLAMP_TO_EDGE wrap behaviour
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

    /**
     * Helper function to convert a colour in Android standard format
     * (0xAARRGGBB) to GL format (4 separate floats, 0.0..1.0)
     * @param argb Colour in 0xAARRGGBB format to convert
     * @return Array of 4 floats representing colour in GL format
     */
    public static float[] androidToGLColour(int argb) {
        return new float[] { (float)((argb >> 16) & 0xFF) / 0xFF,
                (float)((argb >>  8) & 0xFF) / 0xFF,
                (float)((argb      ) & 0xFF) / 0xFF,
                (float)((argb >> 24) & 0xFF) / 0xFF };
    }

    /**
     * Create an FBO, and populate it with the specified dimension & color attachment information
     * @param x X origin of viewport
     * @param y Y origin of viewport
     * @param width Width of viewport
     * @param height Height of viewport
     * @param texTarget Texture target for the specified texture
     * @param tex Texture ID(s) to attach to framebuffer color attachment point(s)
     * @return an {@link FboDescriptor} object, which wraps the GL framebuffer object and relevant
     *      metadata
     * @throws IllegalArgumentException if texTarget is not {@link GLES30#GL_TEXTURE_2D}, or if tex
     *      is null, or if tex is empty, or if tex.length is > {@link #MAX_COLOR_ATTACHMENTS}
     * @throws RuntimeException if the configured FBO is not complete
     */
    public static FboDescriptor createFboDescriptor(int x, int y, int width, int height,
            int texTarget, int... tex) {
        if (texTarget != GLES30.GL_TEXTURE_2D) {
            throw new IllegalArgumentException("Unsupported texture target");
        } else if (tex == null || tex.length == 0 || tex.length > MAX_COLOR_ATTACHMENTS) {
            throw new IllegalArgumentException("tex must contain at least one entry, and at most " +
                    MAX_COLOR_ATTACHMENTS + " entries");
        }

        int[] previousFramebufferBinding = new int[1];
        GLES30.glGetIntegerv(GLES30.GL_DRAW_FRAMEBUFFER_BINDING, previousFramebufferBinding, 0);

        int[] fbo = new int[1];
        GLES30.glGenFramebuffers(1, fbo, 0);
        GLES30.glBindFramebuffer(GLES30.GL_DRAW_FRAMEBUFFER, fbo[0]);

        for (int i = 0; i < tex.length; i++) {
            GLES30.glFramebufferTexture2D(GLES30.GL_DRAW_FRAMEBUFFER,
                    GLES30.GL_COLOR_ATTACHMENT0 + i, GLES30.GL_TEXTURE_2D, tex[i], 0);
        }

        if (GLES30.GL_FRAMEBUFFER_COMPLETE !=
                GLES30.glCheckFramebufferStatus(GLES30.GL_DRAW_FRAMEBUFFER)) {
            throw new RuntimeException("Framebuffer not complete, bind failed");
        }

        GLES30.glBindFramebuffer(GLES30.GL_DRAW_FRAMEBUFFER, previousFramebufferBinding[0]);

        return new FboDescriptor(fbo[0], x, y, width, height, tex.length);
    }

    /**
     * Destroy the GL resources associated with the specified {@link FboDescriptor}
     * @param fboDescriptor an {@link FboDescriptor} describing the FBO to destroy
     * @throws IllegalArgumentException if fboDescriptor is null
     */
    public static void destroyFboDescriptor(FboDescriptor fboDescriptor) {
        if (fboDescriptor == null) {
            throw new IllegalArgumentException("fboDescriptor cannot be null");
        }

        GLES30.glDeleteFramebuffers(1, new int[] { fboDescriptor.fbo }, 0);
    }

    /**
     * Save the current FBO configuration (including viewport configuration)
     * @return a {@link SavedFboToken} describing the current FBO & viewport configuration, for
     *      later use with {@link #restoreFboConfiguration(SavedFboToken)}
     */
    public static SavedFboToken saveFboConfiguration() {
        SavedFboToken token = new SavedFboToken();
        GLES30.glGetIntegerv(GLES30.GL_DRAW_FRAMEBUFFER_BINDING, token.fbo, 0);
        GLES30.glGetIntegerv(GLES30.GL_READ_FRAMEBUFFER_BINDING, token.fbo, 1);
        GLES30.glGetIntegerv(GLES30.GL_VIEWPORT, token.viewport, 0);
        return token;
    }

    /**
     * Restore a previously saved FBO & viewport configuration
     * @param token a {@link SavedFboToken} (from {@link #saveFboConfiguration()}) with state to
     *      restore
     */
    public static void restoreFboConfiguration(SavedFboToken token) {
        if (token == null) {
            throw new IllegalArgumentException("token cannot be null");
        }

        GLES30.glBindFramebuffer(GLES30.GL_DRAW_FRAMEBUFFER, token.fbo[0]);
        GLES30.glBindFramebuffer(GLES30.GL_READ_FRAMEBUFFER, token.fbo[1]);
        GLES30.glViewport(token.viewport[0], token.viewport[1], token.viewport[2],
                token.viewport[3]);
    }

    /**
     * Apply the FBO configuration contained within an {@link FboDescriptor} to the current GL
     * context
     * @param target Framebuffer target to which to apply the new FBO configuration
     * @param fboDescriptor an {@link FboDescriptor} (from
     *      {@link #createFboDescriptor(int, int, int, int, int, int...)}) to apply
     * @throws IllegalArgumentException if target is not one of {@link GLES30#GL_FRAMEBUFFER},
     *      {@link GLES30#GL_DRAW_FRAMEBUFFER}, or {@link GLES30#GL_READ_FRAMEBUFFER}, of if
     *      fboDescriptor is null
     */
    public static void setFboConfiguration(int target, FboDescriptor fboDescriptor) {
        if (target != GLES30.GL_FRAMEBUFFER && target != GLES30.GL_DRAW_FRAMEBUFFER &&
                target != GLES30.GL_READ_FRAMEBUFFER) {
            throw new IllegalArgumentException("Unsupported framebuffer target");
        } else if (fboDescriptor == null) {
            throw new IllegalArgumentException("fboDescriptor cannot be null");
        }

        GLES30.glBindFramebuffer(target, fboDescriptor.fbo);

        if (target != GLES30.GL_READ_FRAMEBUFFER) {
            GLES30.glDrawBuffers(fboDescriptor.numColorAttachments, GL_COLOR_BUFFERS, 0);
            GLES30.glViewport(fboDescriptor.x, fboDescriptor.y, fboDescriptor.width,
                    fboDescriptor.height);
        }
    }
}
