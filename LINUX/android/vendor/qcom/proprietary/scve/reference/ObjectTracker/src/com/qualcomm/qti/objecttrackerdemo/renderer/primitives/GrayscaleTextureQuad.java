/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.objecttrackerdemo.renderer.primitives;

/**
 * This Quad renders the input texture as grayscale
 */
class GrayscaleTextureQuad extends BaseTextureQuad {
    private static final String FRAGMENT_SHADER_GRAYSCALE =
            "const vec3 RGB_TO_GRAYSCALE = vec3(0.2126, 0.7152, 0.0722);" +
            "vec4 shade_fragment() {" +
            "  vec4 sample = sampleTexture(vTransformedTextureCoord);" +
            "  float grayscale = dot(sample.xyz, RGB_TO_GRAYSCALE);" +
            "  return vec4(vec3(grayscale), sample.w);" +
            "}";

    /**
     * Construct a {@link GrayscaleTextureQuad}
     * @param format {@link TextureFormat} of the input texture
     */
    public GrayscaleTextureQuad(TextureFormat format) {
        super(format);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected String getFragmentShader() {
        return super.getFragmentShader() + FRAGMENT_SHADER_GRAYSCALE;
    }
}
