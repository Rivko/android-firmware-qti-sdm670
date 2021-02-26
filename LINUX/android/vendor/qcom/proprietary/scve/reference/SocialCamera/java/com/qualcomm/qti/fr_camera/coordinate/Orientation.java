/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.fr_camera.coordinate;

import android.view.Surface;

import java.util.Objects;

/**
 * This enumeration represents an counterclockwise orientation, relative to some predefined
 * baseline. As such, a clockwise rotation operation of an equal number of degrees can be used to
 * align the subject of this Orientation with the baseline.
 */
public enum Orientation {
    /** An orientation 0 degrees counterclockwise from the baseline */
    O_0(0),

    /** An orientation 90 degrees counterclockwise from the baseline */
    O_90(90),

    /** An orientation 180 degrees counterclockwise from the baseline */
    O_180(180),

    /** An orientation 270 degrees counterclockwise from the baseline */
    O_270(270);

    /**
     * The number of degrees represented by this {@link Orientation}. Will always be one of 0, 90,
     * 180, or 270.
     */
    public final int degrees;

    /**
     * Whether or not this {@link Orientation} is right-angled with respect to the baseline.
     * {@link #O_90} and {@link #O_270} are right-angled, and {@link #O_0} and {@link #O_180} are
     * not.
     */
    public final boolean isRightAngle;

    Orientation(int degrees) {
        this.degrees = degrees;
        isRightAngle = (degrees % 180 != 0);
    }

    /**
     * Convert from degrees to an {@link Orientation}
     * @param degrees one of 0, 90, 180, or 270
     * @return the corresponding {@link Orientation}
     * @throws IllegalArgumentException if degrees is not 0, 90, 180, or 270
     */
    public static Orientation fromDegrees(int degrees) {
        switch (degrees) {
            case 0: return O_0;
            case 90: return O_90;
            case 180: return O_180;
            case 270: return O_270;
            default: throw new IllegalArgumentException("degrees must be 0, 90, 180, or 270");
        }
    }

    /**
     * Convert from {@link Surface} ROTATION_* values to an {@link Orientation}
     * @param surfaceRotation one of the {@link Surface} ROTATION_* values
     * @return the corresponding {@link Orientation}
     * @throws IllegalArgumentException if surfaceRotation is not one of the {@link Surface}
     *         ROTATION_* values
     */
    public static Orientation fromSurfaceRotation(int surfaceRotation) {
        switch (surfaceRotation) {
            case Surface.ROTATION_0: return O_0;
            case Surface.ROTATION_90: return O_90;
            case Surface.ROTATION_180: return O_180;
            case Surface.ROTATION_270: return O_270;
            default: throw new IllegalArgumentException("surfaceRotation is not a valid " +
                    "Surface.ROTATION_* value");
        }
    }

    /**
     * Add an {@link Orientation} to this one, and return the resulting {@link Orientation}
     * @param o an {@link Orientation} to add to this one
     * @return the sum of the two {@link Orientation}s
     * @throws NullPointerException if o is null
     */
    public Orientation add(Orientation o) {
        Objects.requireNonNull(o, "o may not be null");
        return Orientation.fromDegrees((degrees + o.degrees) % 360);
    }

    /**
     * Subtract an {@link Orientation} from this one, and return the resulting {@link Orientation}
     * @param o an {@link Orientation} to subtract from this one
     * @return the difference of the two {@link Orientation}s
     * @throws NullPointerException if o is null
     */
    public Orientation subtract(Orientation o) {
        Objects.requireNonNull(o, "o may not be null");
        return Orientation.fromDegrees((360 + degrees - o.degrees) % 360);
    }

    /**
     * Reverse this {@link Orientation} (counterclockwise to clockwise), and then express the
     * reversed orientation as a counterclockwise {@link Orientation}.
     * @return the reverse of this {@link Orientation}
     */
    public Orientation reverse() {
        return Orientation.fromDegrees((360 - degrees) % 360);
    }
}
