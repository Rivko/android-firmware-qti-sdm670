/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.khronos;

import java.nio.ByteBuffer;

import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;

/**
 * Utilities for images stored in direct {@link ByteBuffer}s
 */
public class ImageBufferUtils {

    static {
        System.loadLibrary("panoramaref_imagebufferutils");
    }

    /**
     * Not constructable
     */
    private ImageBufferUtils() {
    }

    /**
     * Pack the provided buffer (ie make the stride equal to the width)
     * @param buf Direct {@link ByteBuffer} to pack. It will be modified in-place.
     * @param fmt {@link ColourFormat} of the buffer
     * @param width Width of image in buffer, in px
     * @param height Height of image in buffer, in px
     * @param stride Stride of image in buffer, in bytes
     */
    public static void packBuffer(ByteBuffer buf, ColourFormat fmt, int width, int height,
            int stride) {
        if (buf == null) {
            throw new IllegalArgumentException("buf cannot be null");
        } else if (!buf.isDirect()) {
            throw new IllegalArgumentException("buf must be direct");
        } else if (fmt == null) {
            throw new IllegalArgumentException("fmt cannot be null");
        } else if (stride < width * fmt.bytesPerPixel) {
            throw new IllegalArgumentException("stride must be >= width*bytesPerPixel");
        }

        switch (fmt) {
        case ARGB_8888:
        case ABGR_8888:
            nPackBuffer(buf, fmt.bytesPerPixel, width, height, stride);
            break;

        case NV12:
        case NV21:
            nPackBuffer(buf, fmt.bytesPerPixel, width, (height * 3) / 2, stride);
            break;

        default:
            throw new IllegalArgumentException("Unknown colour format");
        }
    }

    /**
     * Populate the provided {@link Bitmap} with the contents of the {@link ByteBuffer}.
     * @param bmp {@link Bitmap} to populate. It must be mutable.
     * @param buf Direct {@link ByteBuffer} containing source pixel data
     * @param width Width of image in buffer, in px
     * @param height Height of image in buffer, in px
     * @param stride Stride of image in buffer, in bytes
     */
    public static void populateBitmapFromABGR8888(Bitmap bmp, ByteBuffer buf, int width, int height,
            int stride) {
        if (bmp == null) {
            throw new IllegalArgumentException("bmp cannot be null");
        } else if (!bmp.isMutable()) {
            throw new IllegalArgumentException("bmp must be mutable");
        } else if (bmp.getConfig() != Config.ARGB_8888) {
            throw new IllegalArgumentException("bmp must be Config.ARGB_8888");
        } else if (buf == null) {
            throw new IllegalArgumentException("buf cannot be null");
        } else if (!buf.isDirect()) {
            throw new IllegalArgumentException("buf must be direct");
        } else if (bmp.getWidth() != width || bmp.getHeight() != height) {
            throw new IllegalArgumentException("bmp width and height must match width and height");
        } else if (stride < width * ColourFormat.ARGB_8888.bytesPerPixel) {
            throw new IllegalArgumentException("stride must be >= width*bytesPerPixel");
        }

        nPopulateBitmapFromABGR8888(bmp, buf, width, height, stride);
    }

    /**
     * Convert the provided buffer from NV12 format to NV21 format
     * @param buf a buffer to convert
     * @param width Width of image in buffer, in px
     * @param height Height of image in buffer, in px
     * @param stride Stride of image in buffer, in bytes
     */
    public static void convertNV12toNV21(byte[] buf, int width, int height, int stride) {
        if (buf == null) {
            throw new IllegalArgumentException("buf cannot be null");
        } else if (width < 0 || height < 0) {
            throw new IllegalArgumentException("Width and height must be >= 0");
        } else if (stride < width * ColourFormat.NV12.bytesPerPixel) {
            throw new IllegalArgumentException("stride must be >= width*bytesPerPixel");
        } else if (buf.length < (height * stride  * 3) / 2) {
            throw new IllegalArgumentException("buffer too small to contain image");
        }

        nConvertNV12toNV21(buf, width, height, stride);
    }

    //==========================================================================
    // Native interface below here
    //==========================================================================
    private static native void nPackBuffer(ByteBuffer buf, int bytesPerPixel, int width, int height,
            int stride);
    private static native void nPopulateBitmapFromABGR8888(Bitmap bmp, ByteBuffer buf, int width,
            int height, int stride);
    private static native void nConvertNV12toNV21(byte[] buf, int width, int height, int stride);
}
