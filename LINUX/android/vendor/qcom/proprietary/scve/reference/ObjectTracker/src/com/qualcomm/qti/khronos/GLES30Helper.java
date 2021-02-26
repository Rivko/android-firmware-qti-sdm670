/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.khronos;

import android.graphics.Bitmap;
import android.opengl.GLES30;

/**
 * Helper functions for {@link GLES30}
 */
public class GLES30Helper {
    /**
     * Token class to save the state of the current FBO before replacing it
     * with a new configuration
     */
    public static class SavedFboToken {
        private final int[] fbo = new int[2];
        private final int[] viewport = new int[4];
    }

    /**
     * Not constructable
     */
    private GLES30Helper() {
    }

    static {
        System.loadLibrary("objecttrackerref_khr_gles_extensions");
        nClassInit();
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

    /**
     * Helper function to create an FBO with the specified texture bound to the color attachment
     * @param colorTarget Binding target for the color buffer handle (eg
     *      {@link GLES30#GL_TEXTURE_2D}, {@link GLES30#GL_RENDERBUFFER}, etc). Must be a valid
     *      binding target.
     * @param color Object name (of type appropriate for colorTarget) to bind to the color
     *      attachment point
     * @param depthTarget Binding target for the depth buffer handle, or 0 if no depth buffer should
     *      be attached
     * @param depth If depthTarget is non-zero, the object name (of type appropriate for
     *      depthTarget) to bind to the depth attachment point
     * @param stencilTarget Binding target for the stencil buffer handle, or 0 if no stencil buffer
     *      should be attached
     * @param stencil If stencilTarget is non-zero, the object name (of type appropriate for
     *      stencilTarget) to bind to the stencil attachment point
     * @return a new FBO handle, with the specified attachments
     */
    public static int generateFbo(int colorTarget, int color, int depthTarget, int depth,
            int stencilTarget, int stencil) {
        if (colorTarget != GLES30.GL_TEXTURE_2D && colorTarget != GLES30.GL_RENDERBUFFER) {
            throw new IllegalArgumentException("colorTarget [" + colorTarget + "] not supported");
        } else if (color == 0) {
            throw new IllegalArgumentException("color cannot be 0");
        } else if (depthTarget != 0 && depthTarget != GLES30.GL_TEXTURE_2D &&
                depthTarget != GLES30.GL_RENDERBUFFER) {
            throw new IllegalArgumentException("depthTarget [" + depthTarget + "] not supported");
        } else if (depthTarget != 0 && depth == 0) {
            throw new IllegalArgumentException("depth cannot be 0 when depthTarget is non-zero");
        } else if (stencilTarget != 0 && stencilTarget != GLES30.GL_TEXTURE_2D &&
                stencilTarget != GLES30.GL_RENDERBUFFER) {
            throw new IllegalArgumentException("stencilTarget [" + stencilTarget + "] not supported");
        } else if (stencilTarget != 0 && stencil == 0) {
            throw new IllegalArgumentException("stencil cannot be 0 when stencilTarget is non-zero");
        }

        final int[][] attachmentData = new int[][] {
                new int[] { GLES30.GL_COLOR_ATTACHMENT0, colorTarget, color },
                new int[] { GLES30.GL_DEPTH_ATTACHMENT, depthTarget, depth },
                new int[] { GLES30. GL_STENCIL_ATTACHMENT, stencilTarget, stencil },
        };

        int[] savedBinding = new int[1];
        GLES30.glGetIntegerv(GLES30.GL_DRAW_FRAMEBUFFER_BINDING, savedBinding, 0);

        int[] fbo = new int[1];
        GLES30.glGenFramebuffers(1, fbo, 0);
        GLES30.glBindFramebuffer(GLES30.GL_DRAW_FRAMEBUFFER, fbo[0]);

        for (int[] attachment : attachmentData) {
            switch (attachment[1]) {
            case 0:
                // Bind nothing to this attachment point
                break;

            case GLES30.GL_TEXTURE_2D:
                GLES30.glFramebufferTexture2D(GLES30.GL_DRAW_FRAMEBUFFER, attachment[0],
                        GLES30.GL_TEXTURE_2D, attachment[2], 0);
                break;

            case GLES30.GL_RENDERBUFFER:
                GLES30.glFramebufferRenderbuffer(GLES30.GL_DRAW_FRAMEBUFFER, attachment[0],
                        GLES30.GL_RENDERBUFFER, attachment[2]);
                break;

            default:
                throw new RuntimeException("Unexpected binding target");
            }
        }

        GLES30.glBindFramebuffer(GLES30.GL_DRAW_FRAMEBUFFER, savedBinding[0]);

        return fbo[0];
    }

    /**
     * Helper function to convert a colour in Android standard format (0xAARRGGBB) to GL format
     * (4 separate floats, 0.0..1.0)
     * @param argb Colour in 0xAARRGGBB format to convert
     * @return Array of 4 floats representing colour in GL format
     */
    public static float[] androidToGLColour(int argb) {
        return new float[] {
            (float)((argb >> 16) & 0xFF) / 0xFF,
            (float)((argb >>  8) & 0xFF) / 0xFF,
            (float)((argb      ) & 0xFF) / 0xFF,
            (float)((argb >> 24) & 0xFF) / 0xFF
        };
    }

    /**
     * Set a new FBO configuration, and return a token that can be used to restore the previous
     * configuration.
     * @param fbo FBO ID to bind
     * @param x X origin of viewport
     * @param y Y origin of viewport
     * @param width Width of viewport
     * @param height Height of viewport
     * @return A {@link SavedFboToken} which can be used to restore the previous FBO configuration
     */
    public static SavedFboToken setFboConfiguration(int fbo, int x, int y, int width, int height) {
        SavedFboToken token = new SavedFboToken();
        GLES30.glGetIntegerv(GLES30.GL_DRAW_FRAMEBUFFER_BINDING, token.fbo, 0);
        GLES30.glGetIntegerv(GLES30.GL_READ_FRAMEBUFFER_BINDING, token.fbo, 1);
        GLES30.glGetIntegerv(GLES30.GL_VIEWPORT, token.viewport, 0);

        GLES30.glBindFramebuffer(GLES30.GL_FRAMEBUFFER, fbo);
        if (GLES30.glCheckFramebufferStatus(GLES30.GL_FRAMEBUFFER) !=
                GLES30.GL_FRAMEBUFFER_COMPLETE) {
            throw new RuntimeException("Framebuffer not complete, bind failed");
        }

        GLES30.glViewport(x, y, width, height);

        GLES30.glClearColor(0f, 0f, 0f, 1f);
        GLES30.glClearDepthf(1f);
        GLES30.glClearStencil(0);
        GLES30.glClear(GLES30.GL_COLOR_BUFFER_BIT | GLES30.GL_DEPTH_BUFFER_BIT |
                GLES30.GL_STENCIL_BUFFER_BIT);

        return token;
    }

    /**
     * Restore a previous FBO configuration
     * @param token {@link SavedFboToken} from {@link #pushFboConfiguration(int,
     *      int, int, int, int, int)}
     */
    public static void restoreFboConfiguration(SavedFboToken token) {
        GLES30.glBindFramebuffer(GLES30.GL_DRAW_FRAMEBUFFER, token.fbo[0]);
        GLES30.glBindFramebuffer(GLES30.GL_READ_FRAMEBUFFER, token.fbo[1]);
        GLES30.glViewport(token.viewport[0], token.viewport[1], token.viewport[2],
                token.viewport[3]);
    }

    /**
     * Copy pixels from the specified {@link Bitmap} to the provided pixel buffer object (PBO). The
     * caller must ensure that the PBO is of sufficient size to receive the pixels.
     * @param bitmap {@link Bitmap} from which to get pixels to write
     * @param pbo GLES PBO handle into which to write the pixels
     * @throws IllegalArgumentException if bitmap is null, or if pbo is 0
     */
    public static void copyBitmapToPbo(Bitmap bitmap, int pbo) {
        if (bitmap == null) {
            throw new IllegalArgumentException("bitmap cannot be null");
        } else if (pbo == 0) {
            throw new IllegalArgumentException("pbo cannot be 0");
        }

        nCopyBitmapToPbo(bitmap, pbo);
    }

    /**
     * Copy pixels from the specified pixel buffer object (PBO) to the provided {@link Bitmap}. The
     * caller must ensure that the PBO contains sufficient pixels to populate the {@link Bitmap}.
     * @param pbo GLES PBO handle from which to read the pixels
     * @param bitmap {@link Bitmap} to store the pixels into
     * @throws IllegalArgumentException if pbo is 0, or if bitmap is null
     */
    public static void copyPboToBitmap(int pbo, Bitmap bitmap) {
        if (pbo == 0) {
            throw new IllegalArgumentException("pbo cannot be 0");
        } else if (bitmap == null) {
            throw new IllegalArgumentException("bitmap cannot be null");
        }

        nCopyPboToBitmap(pbo, bitmap);
    }

    /////////////////////////////////////// NATIVE INTERFACE ///////////////////////////////////////

    private static native void nClassInit();
    private static native void nCopyBitmapToPbo(Bitmap bitmap, int pbo);
    private static native void nCopyPboToBitmap(int pbo, Bitmap bitmap);
}
