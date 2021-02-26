/*
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.objecttrackerdemo.renderer.primitives;

import java.util.ArrayList;

import android.opengl.GLES30;
import android.opengl.Matrix;

import com.qualcomm.qti.objecttracker.misc.CoordinateSpace;
import com.qualcomm.qti.objecttracker.misc.CoordinateTranslator;
import com.qualcomm.qti.objecttracker.misc.Orientation;

/**
 * This class provides the framework for an OpenGL renderer, which consumes an input texture and
 * renders a set of {@link BaseQuad}s to the bound framebuffer or FBO.
 */
/*package*/ abstract class BaseRenderer implements Renderer {
    protected static final float[] IDENTITY_TRANSFORM = new float[16];
    static {
        Matrix.setIdentityM(IDENTITY_TRANSFORM, 0);
    }

    // N.B. these fields should be considered read-only be subclasses
    protected final CoordinateSpace textureSpace;
    protected final CoordinateSpace viewportSpace;
    protected final CoordinateTranslator textureToViewportTranslator;
    protected final boolean mirror;
    protected final Orientation orientation; // CCW rotatation (post-mirroring) to apply to input texture
    protected final TextureFormat textureFormat;
    protected final float[] orthographicTextureProjection = new float[16];
    protected boolean initialized;

    private final ArrayList<BaseQuad> quads = new ArrayList<BaseQuad>();
    private float[] inputTransform;
    private int[] inputTextures;
    private boolean inputTexturesDirty;

    /**
     * Construct a {@link BaseRenderer}
     * @param textureSpace {@link CoordinateSpace} describing the properties of the texture
     *      coordinate space
     * @param viewportSpace {@link CoordinateSpace} describing the properties of the viewport
     *      coordinate space
     * @param textureFormat Format of the input texture
     * @throws IllegalArgumentException if textureSpace, viewportSpace, or textureFormat is null
     */
    public BaseRenderer(CoordinateSpace textureSpace, CoordinateSpace viewportSpace,
            TextureFormat textureFormat) {
        if (textureSpace == null) {
            throw new IllegalArgumentException("textureSpace may not be null");
        } else if (viewportSpace == null) {
            throw new IllegalArgumentException("viewportSpace may not be null");
        } else if (textureFormat == null) {
            throw new IllegalArgumentException("Texture format cannot be null");
        }

        this.viewportSpace = viewportSpace;
        this.textureSpace = textureSpace;
        this.textureFormat = textureFormat;

        textureToViewportTranslator = new CoordinateTranslator(textureSpace, viewportSpace);
        mirror = textureToViewportTranslator.isMirrored();
        orientation = textureToViewportTranslator.getRelativeOrientation();
        calculateOrthographicProjection();
    }

    /** {@inheritDoc} */
    @Override
    public final void init() {
        if (initialized) {
            throw new RuntimeException("Cannot reinitialize while already initialized");
        }

        initialized = true;
        inputTextures = null;
        inputTransform = IDENTITY_TRANSFORM;
        inputTexturesDirty = true;

        // Initialize any Quads added before this function was invoked
        for (BaseQuad quad : quads) {
            quad.init();
        }

        onInit();
    }

    /** {@inheritDoc} */
    @Override
    public final void release() {
        if (!initialized) {
            throw new RuntimeException("Cannot release when uninitialized");
        }

        initialized = false;

        onRelease();

        // Cleanup any Quads that were not removed in onRelease
        for (BaseQuad quad : quads) {
            quad.cleanup();
        }
        quads.clear();
    }

    /** {@inheritDoc} */
    @Override
    public final void setInputTexture(float[] transform, int ... texture) {
        if (transform != null && transform.length != 16) {
            throw new IllegalArgumentException("If transform is non-null, it must be a 4x4 matrix");
        } else if (!initialized) {
            throw new RuntimeException("Cannot invoke setInputTexture when uninitialized");
        }

        inputTransform = (transform != null ? transform : IDENTITY_TRANSFORM);
        inputTextures = texture;
        inputTexturesDirty = true;
    }

    /** {@inheritDoc} */
    @Override
    public final void render() {
        if (!initialized) {
            throw new RuntimeException("Cannot invoke render when uninitialized");
        }

        if (inputTexturesDirty) {
            if (inputTextures == null) {
                throw new IllegalStateException("Cannot render without input textures");
            }

            inputTexturesDirty = false;
            onSetInputTexture(inputTransform, inputTextures);
        }

        onPreRender();

        GLES30.glDisable(GLES30.GL_BLEND);
        GLES30.glBlendEquation(GLES30.GL_FUNC_ADD);
        GLES30.glBlendFunc(GLES30.GL_SRC_ALPHA, GLES30.GL_ONE_MINUS_SRC_ALPHA);

        GLES30.glDisable(GLES30.GL_DEPTH_TEST);
        GLES30.glDisable(GLES30.GL_STENCIL_TEST);

        GLES30.glCullFace(GLES30.GL_BACK);
        GLES30.glEnable(GLES30.GL_CULL_FACE);

        for (BaseQuad quad : quads) {
            quad.render();
        }
    }

    /**
     * Add the indicated {@link BaseQuad} to the set of quads owned by this renderer
     * @param quad {@link BaseQuad} to be initialized & rendered via this {@link BaseRenderer}
     * @throws IllegalArgumentException if quad is null
     */
    protected final void addQuad(BaseQuad quad) {
        if (quad == null) {
            throw new IllegalArgumentException("quad cannot be null");
        }

        if (!quads.contains(quad)) {
            quads.add(quad);
            if (initialized) {
                quad.init();
            }
        }
    }

    /**
     * Remove the indicated {@link BaseQuad} from the set of quads owned by this renderer
     * @param quad {@link BaseQuad} to be released & removed from this {@link BaseRenderer}
     * @throws IllegalArgumentException if quad is null
     */
    protected final void removeQuad(BaseQuad quad) {
        if (quad == null) {
            throw new IllegalArgumentException("quad cannot be null");
        }

        if (quads.remove(quad) && initialized) {
            quad.cleanup();
        }
    }

    /**
     * Subclasses may implement this method to take any steps required to initialize rendering.
     * This method will be invoked when {@link #init()} is called.
     */
    protected void onInit() { }

    /**
     * Subclasses may implement this method to take any steps required to release this renderer.
     * This method will be invoked when {@link #release()} is called.
     */
    protected void onRelease() { }

    /**
     * Subclasses may implement this method to take any steps required when a new input texture
     * handle is provided. This method will be invoked when {@link #render()} is called, before
     * {@link #onPreRender()}.
     * @param transform A 4x4 affine transform to apply to texture coordinates referencing this
     *      texture. May be null, in which case an identity transform will be applied.
     * @param texture A valid set of GL texture handles. This array will be at least 1 element in
     *      length.
     */
    protected void onSetInputTexture(float[] transform, int[] texture) { }

    /**
     * Subclasses may implement this method to take any steps required immediately before a render
     * is performed. This method will be invoked when {@link #render()} is called, immediately after
     * {@link #onSetInputTexture(float[], int[])}.
     */
    protected void onPreRender() { }

    private void calculateOrthographicProjection() {
        final boolean rotated = orientation.isRightAngle();
        final int rotatedTextureWidth = (rotated ? textureSpace.height : textureSpace.width);
        final int rotatedTextureHeight = (rotated ? textureSpace.width : textureSpace.height);

        // Set up an orthographic projection such that a quad running from (0,0) to
        // (texture width, texture height) will be scaled to fit and centered in the viewport.
        // Note that the OpenGL matrix class uses pre-multiplication for operations, so all matrix
        // operations should be read from bottom to top.
        float textureAspect = (float)rotatedTextureWidth / (float)rotatedTextureHeight;
        float viewportAspect = (float)viewportSpace.width / (float)viewportSpace.height;
        if (textureAspect >= viewportAspect) {
            float offset = (rotatedTextureWidth / viewportAspect - rotatedTextureHeight) / 2f;
            Matrix.orthoM(orthographicTextureProjection, 0, 0, rotatedTextureWidth, -offset,
                    rotatedTextureHeight + offset, -1f, 1f);
        } else {
            float offset = (rotatedTextureHeight * viewportAspect - rotatedTextureWidth) / 2f;
            Matrix.orthoM(orthographicTextureProjection, 0, -offset, rotatedTextureWidth + offset,
                    0, rotatedTextureHeight, -1f, 1f);
        }

        Matrix.translateM(orthographicTextureProjection, 0, rotatedTextureWidth / 2f,
                rotatedTextureHeight / 2f, 0f);
        Matrix.rotateM(orthographicTextureProjection, 0, orientation.degrees, 0f, 0f, 1f);
        Matrix.scaleM(orthographicTextureProjection, 0, mirror ? -1f : 1f, 1f, 1f);
        Matrix.translateM(orthographicTextureProjection, 0, -textureSpace.width / 2f,
                -textureSpace.height / 2f, 0f);
    }
}
