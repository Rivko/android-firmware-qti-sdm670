/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
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
import com.qualcomm.qti.objecttrackerdemo.renderer.primitives.BaseQuad.Mask;

/**
 * This renderer provides a sample of how to magnify tracked objects in-place
 */
public class MagnifyRenderer extends BaseTrackedObjectRenderer {
    private static final String TAG = MagnifyRenderer.class.getSimpleName();
    private static final float MAGNIFICATION_FACTOR = 1f / 1.6f;

    private final TextureQuad backgroundQuad;
    private final HashMap<TrackedObject, TextureQuad> foregroundQuads =
            new HashMap<TrackedObject, TextureQuad>();

    private float[] inputTransform;
    private int[] inputTexture;

    /**
     * Construct an {@link MagnifyRenderer}. See {@link BaseRenderer#BaseRenderer(int, int, int,
     * int, TextureFormat)} for a description of shared parameters.
     */
    public MagnifyRenderer(CoordinateSpace textureSpace, CoordinateSpace viewportSpace,
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
            TextureQuad quad = new TextureQuad(textureFormat);
            quad.setMaskType(Mask.OVAL);
            foregroundQuads.put(t, quad);
            addQuad(quad);
            quad.setCoordinateSpace(orthographicTextureProjection);
        }
    }

    /** {@inheritDoc} */
    @Override
    protected void onTrackedObjectsReleased(Collection<TrackedObject> objects) {
        for (TrackedObject t : objects) {
            TextureQuad quad = foregroundQuads.get(t);
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
            TextureQuad quad = foregroundQuads.get(e.getKey());
            if (quad != null) {
                TrackedObject.Position p = e.getValue();
                switch (p.state) {
                case ACQUIRED:
                    quad.setPosition(p.position.left, textureSpace.height - p.position.bottom,
                            p.position.width(), p.position.height());

                    float magnifiedWidth = p.position.width() * MAGNIFICATION_FACTOR;
                    float magnifiedHeight = p.position.height() * MAGNIFICATION_FACTOR;

                    quad.setTexture(inputTexture, inputTransform,
                            textureSpace.width, textureSpace.height,
                            p.position.left + magnifiedWidth / 2f,
                            textureSpace.height - p.position.bottom + magnifiedHeight / 2f,
                            magnifiedWidth, magnifiedHeight);
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
