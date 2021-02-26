/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.objecttrackerdemo.renderer.primitives;

import java.util.Collection;
import java.util.Iterator;
import java.util.Map;

import android.graphics.Color;
import android.graphics.RectF;

import com.qualcomm.qti.objecttracker.misc.CoordinateSpace;
import com.qualcomm.qti.objecttracker.tracker.TrackedObject;

/**
 * This {@link Renderer} renders a Picture-in-Picture window containing the contents of the first
 * {@link TrackedObject}
 */
public class PipRenderer extends BaseTrackedObjectRenderer {
    private static final float PIP_MARGIN_RATIO = 0.008f;
    private static final float PIP_SIZE_RATIO = 0.22f;
    private static final float PIP_BORDER_INSET = 0.0025f;
    private static final float PIP_CONTENT_INSET = 0.0025f;

    private final TextureQuad backgroundQuad;
    private final ColorQuad pipBorderQuad;
    private final ColorQuad pipFrameQuad;
    private final TextureQuad pipQuad;
    private final RectF pipBorder = new RectF();
    private final RectF pipFrame = new RectF();
    private final RectF pipContentFrame = new RectF();

    /**
     * Construct a new {@link PipRenderer}.
     * @see BaseTrackedObjectRenderer#
     *      BaseTrackedObjectRenderer(CoordinateSpace, CoordinateSpace, TextureFormat)
     */
    public PipRenderer(CoordinateSpace textureSpace, CoordinateSpace viewportSpace,
            TextureFormat textureFormat) {
        super(textureSpace, viewportSpace, textureFormat);

        backgroundQuad = new TextureQuad(textureFormat);
        pipBorderQuad = new ColorQuad(Color.WHITE);
        pipFrameQuad = new ColorQuad(Color.BLACK);
        pipQuad = new TextureQuad(textureFormat);
    }

    /** {@inheritDoc} */
    @Override
    protected void onInit() {
        super.onInit();

        calculatePipPosition();

        addQuad(backgroundQuad);
        backgroundQuad.setCoordinateSpace(orthographicTextureProjection);
        backgroundQuad.setPosition(0, 0, textureSpace.width, textureSpace.height);

        addQuad(pipBorderQuad);
        pipBorderQuad.setCoordinateSpace(orthographicTextureProjection);
        pipBorderQuad.setPosition(pipBorder.left, textureSpace.height - pipBorder.bottom,
                pipBorder.width(), pipBorder.height());

        addQuad(pipFrameQuad);
        pipFrameQuad.setCoordinateSpace(orthographicTextureProjection);
        pipFrameQuad.setPosition(pipFrame.left, textureSpace.height - pipFrame.bottom,
                pipFrame.width(), pipFrame.height());

        addQuad(pipQuad);
        pipQuad.setCoordinateSpace(orthographicTextureProjection);
    }

    /** {@inheritDoc} */
    @Override
    protected void onSetInputTexture(float[] transform, int[] texture) {
        super.onSetInputTexture(transform, texture);

        backgroundQuad.setTexture(texture, transform, textureSpace.width, textureSpace.height,
                0, 0, textureSpace.width, textureSpace.height);
    }

    /** {@inheritDoc} */
    @Override
    protected void onTrackedObjectsAcquired(Collection<TrackedObject> objects) {
    }

    /** {@inheritDoc} */
    @Override
    protected void onTrackedObjectsReleased(Collection<TrackedObject> objects) {
    }

    /** {@inheritDoc} */
    @Override
    protected void onTrackedObjectsUpdate(Map<TrackedObject, TrackedObject.Position> positions,
            int[] texture, float[] textureTransform) {
        TrackedObject.Position pos = null;

        Iterator<TrackedObject.Position> it = positions.values().iterator();
        if (it.hasNext()) {
            pos = it.next();
        }

        if (pos == null || pos.state != TrackedObject.State.ACQUIRED) {
            pipQuad.setPosition(0f, 0f, 0f, 0f);
            pipQuad.setTexture(texture, textureTransform, 0, 0, 0f, 0f, 0f, 0f);
        } else {
            final float scaledWidth;
            final float scaledHeight;
            if (pipContentFrame.width() * pos.position.height() >=
                    pipContentFrame.height() * pos.position.width()) {
                // Content frame has a wider aspect than content; fill vertically
                scaledHeight = pipContentFrame.height();
                scaledWidth = scaledHeight / pos.position.height() * pos.position.width();
            } else {
                // Content has a wider aspect than content frame; fill horizontally
                scaledWidth = pipContentFrame.width();
                scaledHeight = scaledWidth / pos.position.width() * pos.position.height();
            }

            pipQuad.setPosition(pipContentFrame.left + (pipContentFrame.width() - scaledWidth) / 2f,
                    textureSpace.height - pipContentFrame.bottom +
                    (pipContentFrame.height() - scaledHeight) / 2f, scaledWidth, scaledHeight);
            pipQuad.setTexture(texture, textureTransform, textureSpace.width, textureSpace.height,
                    pos.position.left, textureSpace.height - pos.position.bottom,
                    pos.position.width(), pos.position.height());
        }
    }

    private void calculatePipPosition() {
        final int maxTextureDimension = Math.max(viewportSpace.width, viewportSpace.height);
        final float pipMargin = maxTextureDimension * PIP_MARGIN_RATIO;
        final float pipFrameSize = maxTextureDimension * PIP_SIZE_RATIO;
        final float pipBorderInset = maxTextureDimension * PIP_BORDER_INSET;
        final float pipContentInset = maxTextureDimension * PIP_CONTENT_INSET;

        RectF pos = new RectF(pipMargin, viewportSpace.height - pipMargin - pipFrameSize,
                pipMargin + pipFrameSize, viewportSpace.height - pipMargin);
        textureToViewportTranslator.mapRectInverse(pos, pipBorder);
        pos.inset(pipBorderInset, pipBorderInset);
        textureToViewportTranslator.mapRectInverse(pos, pipFrame);
        pos.inset(pipContentInset, pipContentInset);
        textureToViewportTranslator.mapRectInverse(pos, pipContentFrame);
    }
}
