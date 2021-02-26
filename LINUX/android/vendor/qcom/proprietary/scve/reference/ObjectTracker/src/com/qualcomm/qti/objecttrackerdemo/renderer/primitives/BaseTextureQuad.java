/*
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.objecttrackerdemo.renderer.primitives;

import java.util.Arrays;
import java.util.Locale;

import android.opengl.GLES30;
import android.opengl.Matrix;

/**
 * An {@link BaseTextureQuad} is a {@link BaseQuad} which applies a texture-based effect to a Quad
 */
abstract class BaseTextureQuad extends BaseQuad {
    private static final String VERTEX_SHADER_BASE =
            "uniform mat4 uTextureTransformMatrix;" +
            "varying vec2 vTransformedTextureCoord;" +
            "void shade_vertex() {" +
            "  vTransformedTextureCoord = (uTextureTransformMatrix * " +
            "    vec4(aTextureCoord, 0, 1)).xy;" +
            "}";

    private static final String FRAGMENT_SHADER_EXTENSION_DIRECTIVES =
            "#extension GL_OES_EGL_image_external : require\n";

    private static final String FRAGMENT_SHADER_BASE =
            "uniform %s uTextureUnit[%d];" +
            "uniform vec2 uTextureStepSize;" +
            "varying vec2 vTransformedTextureCoord;";

    private static final String FRAGMENT_SHADER_BASE_RGB =
            FRAGMENT_SHADER_BASE +
            "vec4 sampleTexture(vec2 coord) {" +
            "  return texture2D(uTextureUnit[0], coord).%s;" +
            "}";

    private static final String FRAGMENT_SHADER_BASE_YUV_SEMIPLANAR =
            FRAGMENT_SHADER_BASE +
            "const vec3 YUV_OFFSETS = vec3(-0.062746, -0.501961, -0.501961);" +
            "uniform mat3 uYuvToRgb;" +
            "vec4 sampleTexture(vec2 coord) {" +
            "  vec3 yuv = vec3(texture2D(uTextureUnit[0], coord).%s, " +
            "      texture2D(uTextureUnit[1], coord).%s);" +
            "  return vec4(uYuvToRgb * (yuv + YUV_OFFSETS), 1);" +
            "}";

    // N.B. CCIR.601 specifies YUV to RGB conversion matrix for SD content (ie height <= 576).
    // BT.709 specifies YUV to RGB conversion matrix for HD content (in our case, anything not SD)
    private static final int CCIR_601_HEIGHT_UPPER_BOUND = 576;
    private static final float[] YUV_TO_RGB_CCIR_601 = new float[] {
        1.164f, 1.164f, 1.164f, 0f, -0.392f, 2.017f, 1.596f, -0.813f, 0f };
    private static final float[] YUV_TO_RGB_BT_709 = new float[] {
        1.164f, 1.164f, 1.164f, 0f, -0.213f, 2.112f, 1.793f, -0.533f, 0f };

    protected final TextureFormat textureFormat;

    private int vsuTextureTransformMatrix;
    private int fsuTextureStepSize;
    private int fsuYuvToRgb = -1;
    private final int[] textureUnit;
    private final int[] texture;

    private final float[] textureTransformMatrix = new float[16];
    private final float[] textureStepSize = new float[2];
    private float[] yuvToRgbMatrix = YUV_TO_RGB_CCIR_601;
    private boolean textureUniformsDirty;

    /**
     * Construct an {@link BaseTextureQuad}
     * @param format {@link TextureFormat} of the input texture
     * @throws IllegalArgumentException if format is null
     */
    public BaseTextureQuad(TextureFormat format) {
        if (format == null) {
            throw new IllegalArgumentException("fmt cannot be null");
        }

        textureFormat = format;
        texture = new int[textureFormat.channelSwizzle.length];
        textureUnit = new int[texture.length];
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected String getVertexShader() {
        return super.getVertexShader() + VERTEX_SHADER_BASE;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected String getFragmentShaderExtensionDirectives() {
        return super.getFragmentShaderExtensionDirectives() + FRAGMENT_SHADER_EXTENSION_DIRECTIVES;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected String getFragmentShader() {
        String fragmentShader;

        switch (textureFormat) {
        case RGBA:
        case BGRA:
        case EXTERNAL:
            fragmentShader = String.format(Locale.US, FRAGMENT_SHADER_BASE_RGB,
                    textureFormat.samplerName, texture.length, textureFormat.channelSwizzle[0]);
            break;

        case NV12:
        case NV21:
            fragmentShader = String.format(Locale.US, FRAGMENT_SHADER_BASE_YUV_SEMIPLANAR,
                    textureFormat.samplerName, texture.length, textureFormat.channelSwizzle[0],
                    textureFormat.channelSwizzle[1]);
            break;

        default:
            throw new RuntimeException("Unexpected texture format");
        }

        return super.getFragmentShader() + fragmentShader;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void onInit(int programName) {
        int fsuTextureUnit = GLES30.glGetUniformLocation(programName, "uTextureUnit");
        for (int i = 0; i < textureUnit.length; i++) {
            textureUnit[i] = allocateTextureUnit(textureFormat.glTextureTarget);
        }
        GLES30.glUniform1iv(fsuTextureUnit, textureUnit.length, textureUnit, 0);

        vsuTextureTransformMatrix = GLES30.glGetUniformLocation(programName,
                "uTextureTransformMatrix");
        fsuTextureStepSize = GLES30.glGetUniformLocation(programName, "uTextureStepSize");

        if (textureFormat == TextureFormat.NV12 || textureFormat == TextureFormat.NV21) {
            fsuYuvToRgb = GLES30.glGetUniformLocation(programName, "uYuvToRgb");
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void onCleanup() {
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void onRender() {
        for (int unit : textureUnit) {
            GLES30.glActiveTexture(GLES30.GL_TEXTURE0 + unit);
            GLES30.glBindTexture(textureFormat.glTextureTarget, texture[unit]);
        }

        if (textureUniformsDirty) {
            GLES30.glUniformMatrix4fv(vsuTextureTransformMatrix, 1, false, textureTransformMatrix,
                    0);
            GLES30.glUniform2fv(fsuTextureStepSize, 1, textureStepSize, 0);
            if (fsuYuvToRgb != -1) {
                GLES30.glUniformMatrix3fv(fsuYuvToRgb, 1, false, yuvToRgbMatrix, 0);
            }
            textureUniformsDirty = false;
        }
    }

    /**
     * Sets the texture to render this quad with.
     * <i>NOTE: {@link #init()} must be called before calling this method</i>
     * @param texture GL texture handle
     * @param textureTransform 4x4 affine transform matrix to apply to texture coordinates. Needed
     *      if the texture does not conform to the GL coord conventions (ie it's (0,0) position is
     *      in the upper left).
     * @param textureWidth Width of texture, in texels
     * @param textureHeight Height of texture, in texels
     * @param s S coord (in texels) of the origin of the texture subregion to render
     * @param t T coord (in texels) of the origin of the texture subregion to render
     * @param renderWidth Width (in texels) of the texture subregion to render
     * @param renderHeight Height (in texels) of the texture subregion to render
     */
    public void setTexture(int[] texture, float[] textureTransform, int textureWidth,
            int textureHeight, float s, float t, float renderWidth, float renderHeight) {
        if (texture.length != this.texture.length) {
            throw new IllegalArgumentException("Incorrect # of texture planes, expected " +
                    this.texture.length);
        } else if (textureTransform.length != 16) {
            throw new IllegalArgumentException("textureTranform must be a 4x4 matrix");
        }

        System.arraycopy(texture, 0, this.texture, 0, texture.length);
        Arrays.fill(this.texture, texture.length, this.texture.length, 0);

        // Calculate an affine transform matrix to apply to the default Quad
        // texture coords ((0,0) in the lower-left, (1,1) in the upper-right)
        float[] transform = new float[16];
        Matrix.translateM(transform, 0, textureTransform, 0, s / textureWidth, t / textureHeight, 0);
        Matrix.scaleM(transform, 0, renderWidth / textureWidth, renderHeight / textureHeight, 1f);

        float textureStepWidth = 1f / textureWidth;
        float textureStepHeight = 1f / textureHeight;
        if (textureStepWidth != textureStepSize[0] || textureStepHeight != textureStepSize[1] ||
                !Arrays.equals(transform, textureTransformMatrix)) {
            textureStepSize[0] = textureStepWidth;
            textureStepSize[1] = textureStepHeight;
            System.arraycopy(transform, 0, textureTransformMatrix, 0, transform.length);
            if (fsuYuvToRgb != -1) {
                yuvToRgbMatrix = (textureHeight <= CCIR_601_HEIGHT_UPPER_BOUND ?
                        YUV_TO_RGB_CCIR_601 : YUV_TO_RGB_BT_709);
            }
            textureUniformsDirty = true;
        }
    }
}
