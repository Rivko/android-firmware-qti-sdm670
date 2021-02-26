/*
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.objecttrackerdemo.renderer.primitives;

/**
 * This interface represents an abstract renderer, which can be used to render using OpenGL ES 3.0
 * to the framebuffer or a bound FBO
 */
public interface Renderer {
    /**
     * Initialize this {@link Renderer}. This must be invoked before other methods of the renderer.
     * @throws RuntimeException if this object is currently initialized
     */
    void init();

    /**
     * Release a previously initialized (via {@link #init()}) {@link Renderer}. After invoking this
     * method, no other methods of this renderer may be invoked until it is reinitialized.
     * @throws RuntimeException if {@link #init()} has not been invoked
     */
    void release();

    /**
     * Set the input texture to use for subsequent calls to {@link #render()}
     * @param transform A 4x4 affine transform to apply to texture coordinates referencing this
     *      texture. May be null, in which case an identity transform will be applied.
     * @param texture A set of valid GL texture handles, 1 per plane in the input images. For eg,
     *      {@link TextureFormat#RGBA} images are interleaved, and have only one plane, whereas
     *      {@link TextureFormat#NV21} images are semi-planar, having one plane for the Y component
     *      and another for the UV components
     * @throws IllegalArgumentException if transform is not null and not of length 16
     * @throws RuntimeException if {@link #init()} has not been invoked
     */
    void setInputTexture(float[] transform, int ... texture);

    /**
     * Request that this {@link Renderer} render to the currently bound framebuffer or FBO
     * @throws RuntimeException if {@link #init()} has not been invoked
     * @throws IllegalStateException if the current input texture (provided via
     *      {@link #setInputTexture(Texture)}) is null
     */
    void render();
}
