/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.objecttrackerdemo.renderer.primitives;

/**
 * This Quad renders the input texture using a non-photo-realistic technique similar to cel shading
 */
class CartoonTextureQuad extends BaseTextureQuad {
    private static final String FRAGMENT_SHADER_CARTOON =
            "vec3 RGBtoHSV(in float r, in float g, in float b) {" +
            "    vec3 res = vec3(0.0);" +
            "    float K = 0.0;" +
            "    if (g < b) {" +
            "        float tmp = g;" +
            "        g = b;" +
            "        b = tmp;" +
            "        K = -1.0;" +
            "    }" +
            "    if (r < g) {" +
            "        float tmp = r;" +
            "        r = g;" +
            "        g = tmp;" +
            "        K = -2.0 / 6.0 - K;" +
            "    }" +
            "    float chroma = r - min(g, b);" +
            "    res.x = abs(K + (g - b) / (6.0 * chroma + 1e-20));" +
            "    res.y = chroma / (r + 1e-20);" +
            "    res.z = r;" +
            "    return res;" +
            "}" +
            "vec3 HSVtoRGB(in float h, in float s, in float v) {" +
            "    vec3 tmp = vec3(-1.0 + abs(6.0 * h - 3.0), " +
            "                     2.0 - abs(6.0 * h - 2.0), " +
            "                     2.0 - abs(6.0 * h - 4.0)); " +
            "    return v * mix(vec3(1.0), clamp(tmp, 0.0, 1.0), s);" +
            "}" +
            "float intensity(in vec3 rgbPixel) {" +
            "    return (rgbPixel.r + rgbPixel.g + rgbPixel.b) / 3.0;" +
            "}" +
            "bool IsEdge(in vec3 curRGB) {" +
            "    vec3 left = sampleTexture(vTransformedTextureCoord + " +
            "        vec2(-uTextureStepSize.x * 2.0, 0.0)).rgb;" +
            "    vec3 down = sampleTexture(vTransformedTextureCoord + " +
            "        vec2(0.0, -uTextureStepSize.y * 2.0)).rgb;" +
            "    float delta = max(intensity(abs(curRGB - left)), " +
            "        intensity(abs(curRGB - down)));" +
            "    return (delta > 0.07);" +
            "}" +
            "vec4 shade_fragment(void) {" +
            "    vec4 orig = sampleTexture(vTransformedTextureCoord);" +
            "    vec3 vHSV =  RGBtoHSV(orig.r, orig.g, orig.b);" +
            "    vHSV = min(vec3(1.0), ceil(vHSV * " +
            "        vec3(6.0, 6.6666666667, 3.3333333333)) * " +
            "        vec3(0.16666667, 0.15, 0.3));" +
            "    vec3 vRGB = HSVtoRGB(vHSV.x, vHSV.y, vHSV.z) - step(0.5, " +
            "        float(IsEdge(orig.rgb)));" +
            "    return vec4(vRGB, orig.a);" +
            "}";

    /**
     * Construct a {@link CartoonTextureQuad}
     * @param format {@link TextureFormat} of the input texture
     */
    public CartoonTextureQuad(TextureFormat format) {
        super(format);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected String getFragmentShader() {
        return super.getFragmentShader() + FRAGMENT_SHADER_CARTOON;
    }
}
