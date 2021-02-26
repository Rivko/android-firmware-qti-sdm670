/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.objecttrackerdemo.renderer.primitives;

/**
 * Render a Quad textured with an input texture (or some subset thereof)
 */
class TextureQuad extends BaseTextureQuad {
    private static final String FRAGMENT_SHADER_TEXTURE =
            "vec4 shade_fragment() {" +
            "  return sampleTexture(vTransformedTextureCoord);" +
            "}";

    /**
     * Construct a {@link TextureQuad}
     * @param format {@link TextureFormat} of the input texture
     */
    public TextureQuad(TextureFormat format) {
        super(format);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected String getFragmentShader() {
        return super.getFragmentShader() + FRAGMENT_SHADER_TEXTURE;
    }
}
