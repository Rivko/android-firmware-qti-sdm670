/*
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.objecttrackerdemo.renderer.primitives;

import java.util.Collection;
import java.util.HashMap;
import java.util.Map;

import android.util.Log;

import com.qualcomm.qti.objecttracker.misc.CoordinateSpace;
import com.qualcomm.qti.objecttracker.tracker.TrackedObject;

/**
 * This renderer provides a sample of how to apply a red translucent overlay on tracked objects
 */
public class RedHighlightRenderer extends BaseTrackedObjectRenderer {
    private static final String TAG = "RedHighlightRenderer";
    private static final int HIGHLIGHT_COLOR = 0x60FF0000;

    private final TextureQuad backgroundQuad;
    private final HashMap<TrackedObject, ColorQuad> foregroundQuads =
            new HashMap<TrackedObject, ColorQuad>();

    /**
     * Construct an {@link RedHighlightRenderer}. See {@link BaseRenderer#BaseRenderer(int, int,
     * int, int, TextureFormat)} for a description of shared parameters.
     */
    public RedHighlightRenderer(CoordinateSpace textureSpace, CoordinateSpace viewportSpace,
            TextureFormat textureFormat) {
        super(textureSpace, viewportSpace, textureFormat);

        backgroundQuad = new TextureQuad(textureFormat);
    }

    /** {@inheritDoc} */
    @Override
    protected void onInit() {
        super.onInit();

        addQuad(backgroundQuad);
        backgroundQuad.setCoordinateSpace(orthographicTextureProjection);
        backgroundQuad.setPosition(0, 0, textureSpace.width, textureSpace.height);
    }

    /** {@inheritDoc} */
    @Override
    protected void onSetInputTexture(float[] transform, int[] texture) {
        super.onSetInputTexture(transform, texture);

        backgroundQuad.setTexture(texture, transform, textureSpace.width, textureSpace.height, 0, 0,
                textureSpace.width, textureSpace.height);
    }

    /** {@inheritDoc} */
    @Override
    protected void onTrackedObjectsAcquired(Collection<TrackedObject> objects) {
        for (TrackedObject t : objects) {
            Log.v(TAG, "Creating foreground quad for TrackedObject " + t.toString());
            ColorQuad quad = new ColorQuad(HIGHLIGHT_COLOR);
            foregroundQuads.put(t, quad);
            addQuad(quad);
            quad.setCoordinateSpace(orthographicTextureProjection);
        }
    }

    /** {@inheritDoc} */
    @Override
    protected void onTrackedObjectsReleased(Collection<TrackedObject> objects) {
        for (TrackedObject t : objects) {
            ColorQuad quad = foregroundQuads.get(t);
            if (quad != null) {
                removeQuad(quad);
            }
        }
    }

    /** {@inheritDoc} */
    @Override
    protected void onTrackedObjectsUpdate(Map<TrackedObject, TrackedObject.Position> positions,
            int[] texture, float[] transform) {
        for (Map.Entry<TrackedObject, TrackedObject.Position> e : positions.entrySet()) {
            ColorQuad quad = foregroundQuads.get(e.getKey());
            if (quad != null) {
                TrackedObject.Position pos = e.getValue();

                switch (pos.state) {
                case ACQUIRED:
                    quad.setPosition(pos.position.left, textureSpace.height - pos.position.bottom,
                            pos.position.width(), pos.position.height());
                    break;

                case LOST:
                case PENDING:
                case UNABLE_TO_ACQUIRE_OBJECT:
                case CLOSED:
                default:
                    quad.setPosition(0f, 0f, 0f, 0f);
                    break;
                }
            }
        }
    }
}
