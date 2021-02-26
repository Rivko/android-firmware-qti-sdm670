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
 * This renderer provides a sample of how to apply effects to tracked objects
 */
public class EffectRenderer extends BaseTrackedObjectRenderer {
    private static final String TAG = "EffectRenderer";

    private final GrayscaleTextureQuad backgroundQuad;
    private final HashMap<TrackedObject, CartoonTextureQuad> foregroundQuads =
            new HashMap<TrackedObject, CartoonTextureQuad>();

    private float[] inputTransform;
    private int[] inputTexture;

    /**
     * Construct an {@link EffectRenderer}. See {@link BaseRenderer#BaseRenderer(int, int, int, int,
     * TextureFormat)} for a description of shared parameters.
     */
    public EffectRenderer(CoordinateSpace textureSpace, CoordinateSpace viewportSpace,
            TextureFormat textureFormat) {
        super(textureSpace, viewportSpace, textureFormat);

        backgroundQuad = new GrayscaleTextureQuad(textureFormat);
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

        inputTransform = transform;
        inputTexture = texture;

        backgroundQuad.setTexture(texture, transform, textureSpace.width, textureSpace.height, 0, 0,
                textureSpace.width, textureSpace.height);
    }

    /** {@inheritDoc} */
    @Override
    protected void onTrackedObjectsAcquired(Collection<TrackedObject> objects) {
        for (TrackedObject t : objects) {
            Log.v(TAG, "Creating foreground quad for TrackedObject " + t.toString());
            CartoonTextureQuad quad = new CartoonTextureQuad(textureFormat);
            foregroundQuads.put(t, quad);
            addQuad(quad);
            quad.setCoordinateSpace(orthographicTextureProjection);
        }
    }

    /** {@inheritDoc} */
    @Override
    protected void onTrackedObjectsReleased(Collection<TrackedObject> objects) {
        for (TrackedObject t : objects) {
            CartoonTextureQuad quad = foregroundQuads.get(t);
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
            CartoonTextureQuad quad = foregroundQuads.get(e.getKey());
            if (quad != null) {
                TrackedObject.Position p = e.getValue();
                switch (p.state) {
                case ACQUIRED:
                    quad.setPosition(p.position.left, textureSpace.height - p.position.bottom,
                            p.position.width(), p.position.height());
                    quad.setTexture(inputTexture, inputTransform,
                            textureSpace.width, textureSpace.height,
                            p.position.left, textureSpace.height - p.position.bottom,
                            p.position.width(), p.position.height());
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
