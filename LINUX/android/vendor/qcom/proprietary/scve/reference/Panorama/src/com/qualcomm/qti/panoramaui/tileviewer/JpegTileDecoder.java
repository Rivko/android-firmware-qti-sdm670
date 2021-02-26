/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.panoramaui.tileviewer;

import java.io.IOException;
import java.nio.ByteBuffer;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.BitmapRegionDecoder;
import android.graphics.Rect;

/**
 * The {@link JpegTileDecoder} is a helper for working with subsets of a JPEG image
 */
public class JpegTileDecoder {
    private final byte[] jpegDataBuf;
    private final int jpegDataOffset;
    private final BitmapRegionDecoder decoder;
    private final BitmapFactory.Options options = new BitmapFactory.Options();
    private final int width;
    private final int height;
    private final Rect workingRect = new Rect();

    /**
     * Construct a new {@link JpegTileDecoder} for the specified JPEG data
     * @param jpegData JPEG data to decode
     */
    public JpegTileDecoder(ByteBuffer jpegData) {
        if (jpegData == null) {
            throw new IllegalArgumentException("jpegData cannot be null");
        }

        if (jpegData.hasArray()) {
            jpegDataBuf = jpegData.array();
            jpegDataOffset = jpegData.arrayOffset();
        } else {
            jpegDataBuf = new byte[jpegData.limit()];
            jpegData.position(0);
            jpegData.get(jpegDataBuf);
            jpegDataOffset = 0;
        }

        try {
            decoder = BitmapRegionDecoder.newInstance(jpegDataBuf, jpegDataOffset,
                    jpegDataBuf.length - jpegDataOffset, true);
        } catch (IOException e) {
            throw new RuntimeException("Failed decoding input JPEG", e);
        }

        width = decoder.getWidth();
        height = decoder.getHeight();
    }

    /**
     * Duplicate the specified {@link JpegTileDecoder}. The new {@link JpegTileDecoder} will share
     * its underlying buffer with the original decoder.
     * @param other a {@link JpegTileDecoder} to duplicate
     */
    public JpegTileDecoder(JpegTileDecoder other) {
        jpegDataBuf = other.jpegDataBuf;
        jpegDataOffset = other.jpegDataOffset;

        try {
            decoder = BitmapRegionDecoder.newInstance(jpegDataBuf, jpegDataOffset,
                    jpegDataBuf.length - jpegDataOffset, true);
        } catch (IOException e) {
            throw new RuntimeException("Failed decoding input JPEG", e);
        }

        width = decoder.getWidth();
        height = decoder.getHeight();
    }

    /**
     * Get the width of the JPEG image at the specified level of downsampling
     * @param downsampleLog2 Binary logarithm of the downsample to apply (ie 0 = 1x scale, 1 = 1/2x
     *      scale, 2 = 1/4x scale, etc)
     * @return the width of the JPEG image at the specified level of downsampling
     */
    public int getWidth(int downsampleLog2) {
        return width / (1 << downsampleLog2);
    }

    /**
     * Get the height of the JPEG image at the specified level of downsampling
     * @param downsampleLog2 Binary logarithm of the downsample to apply (ie 0 = 1x scale, 1 = 1/2x
     *      scale, 2 = 1/4x scale, etc)
     * @return the height of the JPEG image at the specified level of downsampling
     */
    public int getHeight(int downsampleLog2) {
        return height / (1 << downsampleLog2);
    }

    /**
     * Decode a tile of the JPEG image encapsulated by this {@link JpegTileDecoder}
     * @param region Region of the image to decode. The coordinates are at the specified level of
     *      downsampling (ie from (0,0)-(getWidth(downsampleLog2),getHeight(downsampleLog2)).
     * @param downsampleLog2 Binary logarithm of the downsample to apply (ie 0 = 1x scale, 1 = 1/2x
     *      scale, 2 = 1/4x scale, etc)
     * @param tile If not null, a {@link Bitmap} to decode the tile into. It must be mutable and
     *      at least as large as region.
     * @return a {@link Bitmap} containing the requested image tile. If a tile was provided for
     *      reuse, it will be returned here. The caller should make no assumptions about the size
     *      of this {@link Bitmap}; it may be larger than the requested region.
     */
    public Bitmap decodeTile(Rect region, int downsampleLog2, Bitmap tile) {
        if (downsampleLog2 < 0) {
            throw new IllegalArgumentException("decimationPow2 must be >= 0");
        } else if (region == null) {
            throw new IllegalArgumentException("region cannot be null");
        } else if (region.left < 0 || region.top < 0 ||
                region.right > getWidth(downsampleLog2) ||
                region.bottom > getHeight(downsampleLog2)) {
            throw new IllegalArgumentException("region is outside the image dimensions for the " +
                    "specified decimation level");
        }

        if (tile != null) {
            if (tile.isRecycled()) {
                throw new IllegalArgumentException("tile is recycled");
            } else if (!tile.isMutable()) {
                throw new IllegalArgumentException("tile must be mutable");
            } else if (region.width() > tile.getWidth() || region.height() > tile.getHeight()) {
                throw new IllegalArgumentException("tile is too small to receive the decoded data");
            }

            options.inBitmap = tile;
        } else {
            options.inBitmap = null;
        }

        final int scale = (1 << downsampleLog2);
        workingRect.set(region.left * scale, region.top * scale, region.right * scale,
                region.bottom * scale);
        options.inSampleSize = scale;
        return decoder.decodeRegion(workingRect, options);
    }
}
