/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.fr_camera.coordinate;

import android.graphics.Rect;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraDevice;
import android.util.Size;
import android.view.Display;

import java.util.Objects;

/**
 * This class abstracts the properties of a coordinate space into a container. It defines the origin
 * of a coordinate space to be in the upper-left. It can be used with {@link CoordinateTranslator}
 * to express translations between two coordinate spaces.
 */
@SuppressWarnings("deprecation")
public class CoordinateSpace {
    /** An empty coordinate space, with no width, height, or rotation */
    public static final CoordinateSpace EMPTY = new CoordinateSpace(0, 0, Orientation.O_0);

    /** Width of the coordinate space, in local units */
    public final int width;

    /** Height of the coordinate space, in local units */
    public final int height;

    /** Aspect ratio of the coordinate space (unitless) */
    public final float aspect;

    /** Orientation of the coordinate space from the device baseline */
    public final Orientation orientation;

    /**
     * Construct a new {@link CoordinateSpace}
     * @param width Width of the coordinate space, in local units
     * @param height Height of the coordinate space, in local units
     * @param orientation Orientation of the coordinate space from the device baseline
     * @throws NullPointerException if orientation is null
     * @throws IllegalArgumentException if width or height < 0
     */
    public CoordinateSpace(int width, int height, Orientation orientation) {
        Objects.requireNonNull(orientation, "orientation may not be null");

        if (width < 0 || height < 0) {
            throw new IllegalArgumentException("Width and height must be >= 0");
        }

        this.width = width;
        this.height = height;
        this.orientation = orientation;

        aspect = (height != 0 ? (float)width / (float)height : 0f);
    }

    /**
     * Create a new {@link CoordinateSpace} with the requested {@link Orientation} from this
     * {@link CoordinateSpace}. The width and height will be swapped if the new
     * {@link CoordinateSpace}'s {@link Orientation} is at a right angle relative to the current
     * {@link CoordinateSpace}.
     * @param orientation the {@link Orientation} of the new {@link CoordinateSpace}
     * @return a {@link CoordinateSpace} derived from this {@link CoordinateSpace}, with the
     *         requested {@link Orientation}
     * @throws NullPointerException if orientation is null
     */
    public CoordinateSpace createForOrientation(Orientation orientation) {
        Objects.requireNonNull(orientation, "orientation may not be null");

        if (this.orientation == orientation) {
            return this;
        }

        final int rotatedWidth;
        final int rotatedHeight;
        if (!this.orientation.subtract(orientation).isRightAngle) {
            rotatedWidth = width;
            rotatedHeight = height;
        } else {
            //noinspection SuspiciousNameCombination
            rotatedWidth = height;
            //noinspection SuspiciousNameCombination
            rotatedHeight = width;
        }

        return new CoordinateSpace(rotatedWidth, rotatedHeight, orientation);
    }

    /**
     * Construct a {@link CoordinateSpace} for the {@link CameraDevice} with the given frame
     * {@link Size} and {@link CameraCharacteristics}
     * @param frameSize {@link Size} of the camera frames, in px
     * @param cameraCharacteristics {@link CameraCharacteristics} for the active
     *                              {@link CameraDevice}
     * @return a new {@link CoordinateSpace}
     * @throws NullPointerException if size or cameraCharacteristics are null
     */
    public static CoordinateSpace forCameraFrame(Size frameSize,
                                            CameraCharacteristics cameraCharacteristics) {
        Objects.requireNonNull(frameSize, "frameSize may not be null");
        Objects.requireNonNull(cameraCharacteristics, "cameraCharacteristics may not be null");

        final Integer sensorOrientation = cameraCharacteristics.get(
                CameraCharacteristics.SENSOR_ORIENTATION);
        assert sensorOrientation != null;

        return new CoordinateSpace(frameSize.getWidth(), frameSize.getHeight(),
                Orientation.fromDegrees(sensorOrientation));
    }

    /**
     * Construct a {@link CoordinateSpace} for the active pixel array of the {@link CameraDevice}
     * with the provided {@link CameraCharacteristics}
     * @param cameraCharacteristics {@link CameraCharacteristics} for the active
     *                              {@link CameraDevice}
     * @return a new {@link CoordinateSpace}
     * @throws NullPointerException if cameraCharacteristics is null
     */
    public static CoordinateSpace forCameraActiveArray(
            CameraCharacteristics cameraCharacteristics) {
        Objects.requireNonNull(cameraCharacteristics, "cameraCharacteristics may not be null");

        final Rect activeArray = cameraCharacteristics.get(
                CameraCharacteristics.SENSOR_INFO_ACTIVE_ARRAY_SIZE);
        final Integer sensorOrientation = cameraCharacteristics.get(
                CameraCharacteristics.SENSOR_ORIENTATION);
        assert activeArray != null;
        assert sensorOrientation != null;

        return new CoordinateSpace(activeArray.width(), activeArray.height(),
                Orientation.fromDegrees(sensorOrientation));
    }

    /**
     * Construct a {@link CoordinateSpace} representing an area of the specified display
     * @param width Width of the display area, in px
     * @param height Height of the display area, in px
     * @param display Metadata for the {@link Display}
     * @return a new {@link CoordinateSpace}
     * @throws NullPointerException if display is null
     */
    public static CoordinateSpace forDisplay(int width, int height, Display display) {
        Objects.requireNonNull(display, "display may not be null");

        return new CoordinateSpace(width, height, Orientation.fromSurfaceRotation(
                display.getRotation()));
    }
}
