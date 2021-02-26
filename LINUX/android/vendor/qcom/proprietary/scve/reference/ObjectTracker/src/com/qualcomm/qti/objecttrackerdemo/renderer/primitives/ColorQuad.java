/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.objecttrackerdemo.renderer.primitives;

import android.graphics.Color;
import android.opengl.GLES30;

/**
 * This class represents a quad which renders as a fixed color
 */
class ColorQuad extends BaseQuad {
    private static final String VERTEX_SHADER_COLOR =
            "void shade_vertex() {" +
            "}";

    private static final String FRAGMENT_SHADER_COLOR =
            "uniform vec4 uColor;" +
            "vec4 shade_fragment() {" +
            "  return uColor;" +
            "}";

    private int fsuColor;

    private int color;
    private boolean dirty;

    /**
     * Construct a transparent {@link ColorQuad}
     */
    public ColorQuad() {
        this(Color.TRANSPARENT);
    }

    /**
     * Construct a {@link ColorQuad} with the provided initial color
     * @param color {@link Color} of this {@link ColorQuad}
     */
    public ColorQuad(int color) {
        this.color = color;
        dirty = true;
    }

    /**
     * Set the color of this {@link ColorQuad}
     * @param color {@link Color} of this {@link ColorQuad}
     */
    public void setColor(int color) {
        this.color = color;
        dirty = true;
    }

    /** {@inheritDoc} */
    @Override
    protected String getVertexShader() {
        return super.getVertexShader() + VERTEX_SHADER_COLOR;
    }

    /** {@inheritDoc} */
    @Override
    protected String getFragmentShader() {
        return super.getFragmentShader() + FRAGMENT_SHADER_COLOR;
    }

    /** {@inheritDoc} */
    @Override
    protected void onInit(int programName) {
        fsuColor = GLES30.glGetUniformLocation(programName, "uColor");
    }

    /** {@inheritDoc} */
    @Override
    protected void onCleanup() {
    }

    /** {@inheritDoc} */
    @Override
    protected void onRender() {
        if (dirty) {
            GLES30.glUniform4f(fsuColor, Color.red(color) / 255f, Color.green(color) / 255f,
                    Color.blue(color) / 255f, Color.alpha(color) / 255f);
            dirty = false;
        }
    }

    /** {@inheritDoc} */
    @Override
    protected boolean isOpaque() {
        return Color.alpha(color) == 0xFF;
    }
}
