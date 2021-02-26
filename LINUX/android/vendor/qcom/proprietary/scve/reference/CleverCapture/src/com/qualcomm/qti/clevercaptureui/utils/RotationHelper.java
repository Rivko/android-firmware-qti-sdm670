/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.clevercaptureui.utils;

import android.graphics.Matrix;

/**
 * This class provides methods which help handle rotation of images
 */
public class RotationHelper {
    // Not constructable
    private RotationHelper() {
    }

    /**
     * Calculate a rotation matrix for an image of the given dimensions by the specified rotation,
     * such that the rotated image's origin is still at (0, 0)
     * @param m {@link Matrix} to populate with the calculated rotation matrix
     * @param width Width of the image, in px
     * @param height Height of the image, in px
     * @param rotation Degrees clockwise to rotate the image
     * @throws IllegalArgumentException if m is null, or if width or height is < 0
     */
    public static void rotateImage(Matrix m, int width, int height, int rotation) {
        if (m == null) {
            throw new IllegalArgumentException("m cannot be null");
        } else if (width < 0 || height < 0) {
            throw new IllegalArgumentException("width and height must be non-negative");
        }

        final double rotationRadians = Math.toRadians(rotation);
        final float absSinRotation = (float)Math.abs(Math.sin(rotationRadians));
        final float absCosRotation = (float)Math.abs(Math.cos(rotationRadians));

        m.setTranslate(-width / 2f, -height / 2f);
        m.postRotate(rotation, 0f, 0f);
        m.postTranslate((width * absCosRotation + height * absSinRotation) / 2f,
                (width * absSinRotation + height * absCosRotation) / 2f);
    }

    /**
     * Calculate a rotation matrix for an image of the given dimensions by the specified rotation,
     * such that the rotated image's origin is still at (0, 0)
     * @param width Width of the image, in px
     * @param height Height of the image, in px
     * @param rotation Degrees clockwise to rotate the image
     * @return a new {@link Matrix} representing the calculated rotation matrix
     * @throws IllegalArgumentException if width or height is < 0
     */
    public static Matrix rotateImage(int width, int height, int rotation) {
        Matrix m = new Matrix();
        rotateImage(m, width, height, rotation);
        return m;
    }
}
