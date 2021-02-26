/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.fr_camera.coordinate;

import android.graphics.Matrix;
import android.graphics.PointF;
import android.graphics.RectF;

import java.util.Objects;

/**
 * This class provides a coordinate space translation facility between two coordinate spaces. It
 * handles rotation, scaling, and centering of the content.
 */
public class CoordinateTranslator {
    public static final CoordinateTranslator IDENTITY =
            new CoordinateTranslator(CoordinateSpace.EMPTY, CoordinateSpace.EMPTY);

    /**
     * The source {@link CoordinateSpace} that this {@link CoordinateTranslator} will translate from
     */
    public final CoordinateSpace source;

    /**
     * The destination {@link CoordinateSpace} that this {@link CoordinateTranslator} will translate
     * to
     */
    public final CoordinateSpace destination;

    /**
     * The relative orientation of the source {@link CoordinateSpace}, with respect to the
     * destination {@link CoordinateSpace}. If not {@link Orientation#O_0}, the rotation operation
     * is applied before scaling & centering of the content.
     */
    public final Orientation relativeOrientation;

    private final Matrix transform = new Matrix();
    private final Matrix transformInverse = new Matrix();

    /**
     * Construct a {@link CoordinateTranslator} with the specified {@link CoordinateSpace}s
     * @param source the source {@link CoordinateSpace}
     * @param destination the destination {@link CoordinateSpace}
     * @throws NullPointerException if source or destination are null
     */
    public CoordinateTranslator(CoordinateSpace source, CoordinateSpace destination) {
        Objects.requireNonNull(source, "source may not be null");
        Objects.requireNonNull(destination, "destination may not be null");

        this.source = source;
        this.destination = destination;
        relativeOrientation = source.orientation.subtract(destination.orientation);

        calculateTransform();
    }

    /**
     * Map a point from the source coordinate space to the destination coordinate space
     * @param sourcePoint Specifies the coordinate in source space
     * @param destinationPoint Receives the coordinate in destination space. May be the same
     *                         {@link PointF} as sourcePoint.
     */
    public void mapPoint(PointF sourcePoint, PointF destinationPoint) {
        float[] buf = new float[] { sourcePoint.x, sourcePoint.y };
        transform.mapPoints(buf);
        destinationPoint.x = buf[0];
        destinationPoint.y = buf[1];
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
     * @param sourcePoint Receives the coordinate in source space. May be the same {@link PointF} as
     *                    as destinationPoint.
     */
    public void mapPointInverse(PointF destinationPoint, PointF sourcePoint) {
        float[] buf = new float[] { destinationPoint.x, destinationPoint.y };
        transformInverse.mapPoints(buf);
        sourcePoint.x = buf[0];
        sourcePoint.y = buf[1];
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
     * @param destinationRect Receives the rectangle in destination space. May be the same
     *                        {@link RectF} as sourceRect.
     */
    public void mapRect(RectF sourceRect, RectF destinationRect) {
        transform.mapRect(destinationRect, sourceRect);
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
     * @param sourceRect Receives the rectangle in source space. May be the same {@link RectF} as
     *                   destinationRect.
     */
    public void mapRectInverse(RectF destinationRect, RectF sourceRect) {
        transformInverse.mapRect(sourceRect, destinationRect);
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

    private void calculateTransform() {
        transform.reset();
        transformInverse.reset();

        if (destination.width == 0 || destination.height == 0) {
            return;
        }

        transform.setTranslate(-source.width / 2f, -source.height / 2f);

        // N.B. since the CoordinateSpace defines the origin to be in the upper-left, a clockwise
        // rotation is represented by a positive angle.
        transform.postRotate(relativeOrientation.degrees);

        final boolean rotated = relativeOrientation.isRightAngle;
        final int rotatedSourceWidth = (rotated ? source.height : source.width);
        final int rotatedSourceHeight = (rotated ? source.width : source.height);
        final float rotatedSourceAspect = (rotated ? 1f / source.aspect : source.aspect);
        final float scale;
        if (destination.aspect >= rotatedSourceAspect) {
            scale = (float)destination.height / (float)rotatedSourceHeight;
        } else {
            scale = (float)destination.width / (float)rotatedSourceWidth;
        }
        transform.postScale(scale, scale);

        transform.postTranslate(destination.width / 2f, destination.height / 2f);

        transform.invert(transformInverse);
    }
}
