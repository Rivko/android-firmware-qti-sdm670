/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.objecttracker.misc;

import android.graphics.Matrix;
import android.graphics.PointF;
import android.graphics.RectF;

/**
 * This class provides a coordinate space translation facility between two coordinate spaces. It
 * handles horizontal reflection (for front-facing cameras), rotation, scaling, and centering of
 * the content.
 */
public class CoordinateTranslator {
    private CoordinateSpace source;
    private CoordinateSpace destination;
    private boolean mirrored;
    private Orientation relativeOrientation; // to be applied _after_ mirroring
    private final Matrix transform = new Matrix();
    private final Matrix transformInverse = new Matrix();
    private final float[] mapPointWorkingBuffer = new float[4];
    private final PointF[] mapRectWorkingBuffer = new PointF[] { new PointF(), new PointF() };

    /**
     * Construct a {@link CoordinateTranslator} with {@link CoordinateSpace#EMPTY} source and
     * destination coordinate spaces
     */
    public CoordinateTranslator() {
        this(CoordinateSpace.EMPTY, CoordinateSpace.EMPTY);
    }

    /**
     * Construct a {@link CoordinateTranslator} with the specified {@link CoordinateSpace}s
     * @param source the source {@link CoordinateSpace}
     * @param destination the destination {@link CoordinateSpace}
     * @throws IllegalArgumentException if source or destination are null
     */
    public CoordinateTranslator(CoordinateSpace source, CoordinateSpace destination) {
        if (source == null) {
            throw new IllegalArgumentException("source cannot be null");
        } else if (destination == null) {
            throw new IllegalArgumentException("destination cannot be null");
        }

        this.source = source;
        this.destination = destination;
        updateTransform();
    }

    /**
     * Specify the source {@link CoordinateSpace}
     * @param space the source {@link CoordinateSpace}
     * @throws IllegalArgumentException if space is null
     */
    public synchronized void setSourceCoordinateSpace(CoordinateSpace space) {
        if (space == null) {
            throw new IllegalArgumentException("space cannot be null");
        }

        source = space;
        updateTransform();
    }

    /**
     * Specify the destination {@link CoordinateSpace}
     * @param space the destination {@link CoordinateSpace}
     * @throws IllegalArgumentException if space is null
     */
    public synchronized void setDestinationCoordinateSpace(CoordinateSpace space) {
        if (space == null) {
            throw new IllegalArgumentException("space cannot be null");
        }

        destination = space;
        updateTransform();
    }

    /**
     * Get the source {@link CoordinateSpace}, as previously set by
     * {@link #setSourceCoordinateSpace(CoordinateSpace)}
     * @return the source {@link CoordinateSpace}, or {@link CoordinateSpace#EMPTY} if no source
     *      coordinate space has been set
     */
    public synchronized CoordinateSpace getSourceCoordinateSpace() {
        return source;
    }

    /**
     * Get the destination {@link CoordinateSpace}, as previously set by
     * {@link #setDestinationCoordinateSpace(CoordinateSpace)}
     * @return the destination {@link CoordinateSpace}, or {@link CoordinateSpace#EMPTY} if no
     *      destination coordinate space has been set
     */
    public synchronized CoordinateSpace getDestinationCoordinateSpace() {
        return destination;
    }

    /**
     * Gets whether the destination coordinate space is mirrored with respect to the source
     * coordinate space.
     * @return true if the destination coordinate space is mirrored with respect to the source
     *      coordinate space, else false
     */
    public synchronized boolean isMirrored() {
        return mirrored;
    }

    /**
     * Gets the relative {@link Orientation} between the source and destination coordinate spaces.
     * This is the amount that the source coordinate space needs to be rotated clockwise to match
     * the destination coordinate space. This rotation is to be applied <b>after</b> horizontal
     * reflection (if required).
     * @return an {@link Orientation} value
     * @see #isMirrored()
     */
    public synchronized Orientation getRelativeOrientation() {
        return relativeOrientation;
    }

    // N.B. Must be called from a synchronized method
    private void updateTransform() {
        mirrored = (source.horizontallyReflected != destination.horizontallyReflected);
        relativeOrientation = Orientation.relativeOrientation(
                mirrored ? source.orientation.flip() : source.orientation, destination.orientation);

        transform.set(null);
        transformInverse.set(null);

        if (destination.width == 0 || destination.height == 0) {
            return;
        }

        transform.postTranslate(-source.width / 2f, -source.height / 2f);

        if (mirrored) {
            transform.postScale(-1f, 1f);
        }

        // N.B. since the CoordinateSpace defines the origin to be in the upper-left, a CCW
        // rotation is represented by a negative angle.
        transform.postRotate(-relativeOrientation.degrees);

        final boolean rotated = relativeOrientation.isRightAngle();
        final int rotatedSourceWidth = (rotated ? source.height : source.width);
        final int rotatedSourceHeight = (rotated ? source.width : source.height);
        final float scale;
        if (destination.width * rotatedSourceHeight >= destination.height * rotatedSourceWidth) {
            // Destination aspect ratio is wider than rotated source
            scale = (float)destination.height / (float)rotatedSourceHeight;
        } else {
            // Destination aspect ratio is taller than rotated source
            scale = (float)destination.width / (float)rotatedSourceWidth;
        }
        transform.postScale(scale, scale);

        transform.postTranslate(destination.width / 2f, destination.height / 2f);

        transform.invert(transformInverse);
    }

    /**
     * Map a point from the source coordinate space to the destination coordinate space
     * @param sourcePoint Specifies the coordinate in source space
     * @param destinationPoint Receives the coordinate in destination space
     */
    public synchronized void mapPoint(PointF sourcePoint, PointF destinationPoint) {
        mapPointWorkingBuffer[0] = sourcePoint.x;
        mapPointWorkingBuffer[1] = sourcePoint.y;
        transform.mapPoints(mapPointWorkingBuffer, 2, mapPointWorkingBuffer, 0, 1);
        destinationPoint.x = mapPointWorkingBuffer[2];
        destinationPoint.y = mapPointWorkingBuffer[3];
    }

    /**
     * Map a point from the source coordinate space to the destination coordinate space
     * @param sourcePoint Specifies the coordinate in source space
     * @return a new {@link PointF} containing the coordinate in destination space
     */
    public PointF mapPoint(PointF sourcePoint) {
        PointF destPoint = new PointF();
        mapPoint(sourcePoint, destPoint);
        return destPoint;
    }

    /**
     * Map a point from the destination coordinate space to the source coordinate space
     * @param destinationPoint Specifies the coordinate in destination space
     * @param sourcePoint Receives the coordinate in source space
     */
    public synchronized void mapPointInverse(PointF destinationPoint, PointF sourcePoint) {
        mapPointWorkingBuffer[0] = destinationPoint.x;
        mapPointWorkingBuffer[1] = destinationPoint.y;
        transformInverse.mapPoints(mapPointWorkingBuffer, 2, mapPointWorkingBuffer, 0, 1);
        sourcePoint.x = mapPointWorkingBuffer[2];
        sourcePoint.y = mapPointWorkingBuffer[3];
    }

    /**
     * Map a point from the destination coordinate space to the source coordinate space
     * @param destinationPoint Specifies the coordinate in destination space
     * @return a new {@link PointF} containing the coordinate in source space
     */
    public PointF mapPointInverse(PointF destinationPoint) {
        PointF sourcePoint = new PointF();
        mapPointInverse(destinationPoint, sourcePoint);
        return sourcePoint;
    }

    /**
     * Map a rectangle from the source coordinate space to the destination coordinate space
     * @param sourceRect Specifies the rectangle in source space
     * @param destinationRect Receives the rectangle in destination space
     */
    public synchronized void mapRect(RectF sourceRect, RectF destinationRect) {
        switch (mirrored ? Orientation.fromDegrees(90 - relativeOrientation.degrees) :
                relativeOrientation) {
        case ORIENTATION_0:
            mapRectWorkingBuffer[0].set(sourceRect.left, sourceRect.top);
            mapRectWorkingBuffer[1].set(sourceRect.right, sourceRect.bottom);
            break;

        case ORIENTATION_90:
            mapRectWorkingBuffer[0].set(sourceRect.right, sourceRect.top);
            mapRectWorkingBuffer[1].set(sourceRect.left, sourceRect.bottom);
            break;

        case ORIENTATION_180:
            mapRectWorkingBuffer[0].set(sourceRect.right, sourceRect.bottom);
            mapRectWorkingBuffer[1].set(sourceRect.left, sourceRect.top);
            break;

        case ORIENTATION_270:
            mapRectWorkingBuffer[0].set(sourceRect.left, sourceRect.bottom);
            mapRectWorkingBuffer[1].set(sourceRect.right, sourceRect.top);
            break;

        default:
            throw new RuntimeException("Unexpected rotation value");
        }

        mapPoint(mapRectWorkingBuffer[0], mapRectWorkingBuffer[0]);
        mapPoint(mapRectWorkingBuffer[1], mapRectWorkingBuffer[1]);
        destinationRect.set(mapRectWorkingBuffer[0].x, mapRectWorkingBuffer[0].y,
                mapRectWorkingBuffer[1].x, mapRectWorkingBuffer[1].y);
    }

    /**
     * Map a rectangle from the source coordinate space to the destination coordinate space
     * @param sourceRect Specifies the rectangle in source space
     * @return a new {@link RectF} containing the rectangle in destination space
     */
    public RectF mapRect(RectF sourceRect) {
        RectF destRect = new RectF();
        mapRect(sourceRect, destRect);
        return destRect;
    }

    /**
     * Map a rectangle from the destination coordinate space to the source coordinate space
     * @param destinationRect Specifies the rectangle in destination space
     * @param sourceRect Receives the rectangle in source space
     */
    public synchronized void mapRectInverse(RectF destinationRect, RectF sourceRect) {
        switch (mirrored ? Orientation.fromDegrees(relativeOrientation.degrees - 90) :
                relativeOrientation) {
        case ORIENTATION_0:
            mapRectWorkingBuffer[0].set(destinationRect.left, destinationRect.top);
            mapRectWorkingBuffer[1].set(destinationRect.right, destinationRect.bottom);
            break;

        case ORIENTATION_90:
            mapRectWorkingBuffer[0].set(destinationRect.left, destinationRect.bottom);
            mapRectWorkingBuffer[1].set(destinationRect.right, destinationRect.top);
            break;

        case ORIENTATION_180:
            mapRectWorkingBuffer[0].set(destinationRect.right, destinationRect.bottom);
            mapRectWorkingBuffer[1].set(destinationRect.left, destinationRect.top);
            break;

        case ORIENTATION_270:
            mapRectWorkingBuffer[0].set(destinationRect.right, destinationRect.top);
            mapRectWorkingBuffer[1].set(destinationRect.left, destinationRect.bottom);
            break;

        default:
            throw new RuntimeException("Unexpected rotation value");
        }

        mapPointInverse(mapRectWorkingBuffer[0], mapRectWorkingBuffer[0]);
        mapPointInverse(mapRectWorkingBuffer[1], mapRectWorkingBuffer[1]);
        sourceRect.set(mapRectWorkingBuffer[0].x, mapRectWorkingBuffer[0].y,
                mapRectWorkingBuffer[1].x, mapRectWorkingBuffer[1].y);
    }

    /**
     * Map a rectangle from the destination coordinate space to the source coordinate space
     * @param destinationRect Specifies the rectangle in destination space
     * @return a new {@link RectF} containing the rectangle in source space
     */
    public RectF mapRectInverse(RectF destinationRect) {
        RectF sourceRect = new RectF();
        mapRectInverse(destinationRect, sourceRect);
        return sourceRect;
    }
}
