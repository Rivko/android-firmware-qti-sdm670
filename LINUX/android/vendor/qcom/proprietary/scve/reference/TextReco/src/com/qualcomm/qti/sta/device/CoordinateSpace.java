/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.sta.device;



import android.hardware.Camera;
import android.view.Display;

/**
 * This class abstracts the properties of a coordinate space into a container. It defines the origin
 * of a coordinate space to be in the upper-left. It can be used with {@link CoordinateTranslator}
 * to express translations between two coordinate spaces.
 */
public class CoordinateSpace {
    /** An empty coordinate space, with no width, height, rotation, or horizontal reflection */
    public static final CoordinateSpace EMPTY = new CoordinateSpace(0, 0,
            Orientation.ORIENTATION_0, false);

    /** Width of the coordinate space, in local units */
    public final int width;

    /** Height of the coordinate space, in local units */
    public final int height;

    /** Orientation of the coordinate space from the device baseline */
    public final Orientation orientation;

    /** Whether or not this coordinate space is horizontally reflected vs. the device baseline */
    public final boolean horizontallyReflected;

    /**
     * Construct a new {@link CoordinateSpace}
     * @param width Width of the coordinate space, in local units
     * @param height Height of the coordinate space, in local units
     * @param orientation Orientation of the coordinate space from the device baseline
     * @param horizontallyReflected Whether or not this coordinate space is horizontally reflected
     *      vs. the device baseline
     * @throws IllegalArgumentException if width or height is < 0, or if orientation is null
     */
    public CoordinateSpace(int width, int height, Orientation orientation,
            boolean horizontallyReflected) {
        if (width < 0 || height < 0) {
            throw new IllegalArgumentException("Width and height must be >= 0");
        } else if (orientation == null) {
            throw new IllegalArgumentException("orientation may not be null");
        }

        this.width = width;
        this.height = height;
        this.orientation = orientation;
        this.horizontallyReflected = horizontallyReflected;
    }

    /**
     * Construct a {@link CoordinateSpace} representing a {@link Camera}
     * @param width Width of camera frame, in px
     * @param height Height of camera frame, in px
     * @param cameraInfo Metadata about the {@link Camera} which is producing frames
     * @return a new {@link CoordinateSpace}
     * @throws IllegalArgumentException if cameraInfo is null
     */
    public static CoordinateSpace forCamera(int width, int height, Camera.CameraInfo cameraInfo) {
        if (cameraInfo == null) {
            throw new IllegalArgumentException("cameraInfo cannot be null");
        }

        return new CoordinateSpace(width, height, Orientation.fromDegrees(cameraInfo.orientation),
                cameraInfo.facing == Camera.CameraInfo.CAMERA_FACING_FRONT);
    }

    /**
     * Construct a {@link CoordinateSpace} representing an area of the specified display
     * @param width Width of the display area, in px
     * @param height Height of the display area, in px
     * @param display Metadata for the {@link Display}
     * @return a new {@link CoordinateSpace}
     * @throws IllegalArgumentException if display is null
     */
    public static CoordinateSpace forDisplay(int width, int height, Display display) {
        if (display == null) {
            throw new IllegalArgumentException("display cannot be null");
        }

        return new CoordinateSpace(width, height, Orientation.fromSurfaceRotation(
                display.getRotation()), false);
    }
}
