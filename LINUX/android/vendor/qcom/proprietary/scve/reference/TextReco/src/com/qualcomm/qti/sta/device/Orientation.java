/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.sta.device;

import android.view.Surface;

/**
 * This class encapsulates a logical orientation value, limited to right-angle orientations (ie any
 * multiple of 90 degrees)
 */
public enum Orientation {
    /** 0 degrees counterclockwise orientation (relative to a context-specific baseline) */
    ORIENTATION_0(0),

    /** 90 degrees counterclockwise orientation (relative to a context-specific baseline) */
    ORIENTATION_90(90),

    /** 180 degrees counterclockwise orientation (relative to a context-specific baseline) */
    ORIENTATION_180(180),

    /** 270 degrees counterclockwise orientation (relative to a context-specific baseline) */
    ORIENTATION_270(270);

    /** Orientation value (in degrees counterclockwise from context-specific baseline) */
    public final int degrees;

    private Orientation(int degrees) {
        this.degrees = degrees;
    }

    /**
     * Convert from degrees to an ORIENTATION_* value
     * @param degrees Degrees counterclockwise of the orientation, relative to a context-specific
     *      baseline. This will be normalized to be between 0 and 360 degrees.
     * @return an {@link Orientation} that expresses the requested orientation
     * @throws IllegalArgumentException if degrees does not express a right-angle orientation
     */
    public static Orientation fromDegrees(int degrees) {
        degrees = ((degrees % 360) + 360) % 360;
        switch (degrees) {
        case 0:     return ORIENTATION_0;
        case 90:    return ORIENTATION_90;
        case 180:   return ORIENTATION_180;
        case 270:   return ORIENTATION_270;

        default:
            throw new IllegalArgumentException("degrees must be a multiple of 90");
        }
    }

    /**
     * Convert from one of the Surface.ROTATION_* values to the equivalent ORIENTATION_* value
     * @param surfaceRotation One of the Surface.ROTATION_* values
     * @return an {@link Orientation} that expresses the requested surfaceRotation
     * @throws IllegalArgumentException if surfaceRotation is not one of the Surface.ROTATION_*
     *      values
     */
    public static Orientation fromSurfaceRotation(int surfaceRotation) {
        // NOTE: the Surface.ROTATION_* values represent a rotation clockwise necessary to correct
        // for a counterclockwise orientation value, thus they can be directly mapped.
        switch (surfaceRotation) {
        case Surface.ROTATION_0:    return ORIENTATION_0;
        case Surface.ROTATION_90:   return ORIENTATION_90;
        case Surface.ROTATION_180:  return ORIENTATION_180;
        case Surface.ROTATION_270:  return ORIENTATION_270;

        default:
            throw new IllegalArgumentException("surfaceRotation should be one of the " +
                    "Surface.ROTATION_* values");
        }
    }

    /**
     * Calculates the relative {@link Orientation} between two {@link Orientation}s
     * @param from {@link Orientation} from which returned value will be relative
     * @param to {@link Orientation} to which returned value will be relative
     * @return a relative {@link Orientation} which, when add to "from", produces "to"
     */
    public static Orientation relativeOrientation(Orientation from, Orientation to) {
        return Orientation.fromDegrees(to.degrees - from.degrees);
    }

    /**
     * Flip this {@link Orientation} from counterclockwise to clockwise, and then re-express as a
     * counterclockwise {@link Orientation}
     * @return the flipped version of this {@link Orientation}
     */
    public Orientation flip() {
        switch (this) {
        case ORIENTATION_0:     return ORIENTATION_0;
        case ORIENTATION_90:    return ORIENTATION_270;
        case ORIENTATION_180:   return ORIENTATION_180;
        case ORIENTATION_270:   return ORIENTATION_90;

        default:
            throw new RuntimeException("Unexpected Orientation value");
        }
    }

    /**
     * Returns whether this {@link Orientation} value represents a right angle with respect to the
     * baseline.
     * @return true if this {@link Orientation} represents a right angle, else false
     */
    public boolean isRightAngle() {
        return (this == ORIENTATION_90 || this == ORIENTATION_270);
    }
}
