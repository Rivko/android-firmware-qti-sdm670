/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.objecttrackerdemo.renderer.primitives;

import com.qualcomm.qti.objecttracker.misc.CoordinateSpace;

/**
 * A simple renderer, which renders the input texture inside the viewport, scaling to fit while
 * maintaining aspect ratio
 */
public class InputTextureRenderer extends BaseRenderer {
    private final TextureQuad quad;

    /**
     * Constructs an {@link InputTextureRenderer}. See
     * {@link BaseRenderer#BaseRenderer(int, int, int, int, TextureFormat)} for parameter details.
     */
    public InputTextureRenderer(CoordinateSpace textureSpace, CoordinateSpace viewportSpace,
            TextureFormat textureFormat) {
        super(textureSpace, viewportSpace, textureFormat);

        quad = new TextureQuad(textureFormat);
    }

    /** {@inheritDoc} */
    @Override
    protected void onInit() {
        addQuad(quad);
        quad.setCoordinateSpace(orthographicTextureProjection);
        quad.setPosition(0, 0, textureSpace.width, textureSpace.height);
    }

    /** {@inheritDoc} */
    @Override
    protected void onSetInputTexture(float[] transform, int[] texture) {
        quad.setTexture(texture, transform, textureSpace.width, textureSpace.height, 0, 0,
                textureSpace.width, textureSpace.height);
    }
}
