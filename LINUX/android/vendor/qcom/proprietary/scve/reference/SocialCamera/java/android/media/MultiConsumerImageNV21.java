/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package android.media;

import android.graphics.Rect;

import java.nio.ByteBuffer;
import java.util.Objects;

/**
 * This class wraps an {@link ImageNV21} and allows multiple read-only copies of it to be created.
 * It is closely related to {@link MultiConsumerImage} (and uses a {@link MultiConsumerImage}
 * internally). See the {@link MultiConsumerImage} class for more details on the functionality of
 * {@link MultiConsumerImageNV21}.
 */
public class MultiConsumerImageNV21 extends ImageNV21 {
    private final MultiConsumerImage im;
    private final ByteBuffer nv21Buffer;
    private final int nv21Stride;
    private boolean closed;

    /**
     * Construct a new {@link MultiConsumerImageNV21} for the given {@link ImageNV21}. See
     * {@link MultiConsumerImage#MultiConsumerImage(Image)} for more details.
     * @param im the {@link ImageNV21} to wrap
     * @throws NullPointerException if im is null
     */
    public MultiConsumerImageNV21(ImageNV21 im) {
        Objects.requireNonNull(im, "im may not be null");

        this.im = new MultiConsumerImage(im);
        this.nv21Buffer = im.getNV21ImageBuffer().asReadOnlyBuffer();
        nv21Stride = im.getNV21Stride();
    }

    private MultiConsumerImageNV21(MultiConsumerImageNV21 other) {
        im = other.im.duplicate();
        nv21Buffer = other.nv21Buffer.duplicate();
        nv21Stride = other.nv21Stride;
    }

    /**
     * Duplicate a {@link MultiConsumerImageNV21}, producing another {@link MultiConsumerImageNV21}
     * which shares the same underlying {@link ImageNV21}. See
     * {@link MultiConsumerImage#duplicate()} for more details.
     * @return a new {@link MultiConsumerImageNV21} which shares the same underlying
     *         {@link ImageNV21} as this instance
     */
    public MultiConsumerImageNV21 duplicate() {
        return new MultiConsumerImageNV21(this);
    }

    //@Override
    public void setTimestamp(long timestamp) {
        im.setTimestamp(timestamp);
    }

    @Override
    public Rect getCropRect() {
        return im.getCropRect();
    }

    @Override
    public void setCropRect(Rect cropRect) {
        im.setCropRect(cropRect);
    }

    @Override
    public int getFormat() {
        return im.getFormat();
    }

    @Override
    public int getWidth() {
        return im.getWidth();
    }

    @Override
    public int getHeight() {
        return im.getHeight();
    }

    @Override
    public long getTimestamp() {
        return im.getTimestamp();
    }

    @Override
    public Plane[] getPlanes() {
        return im.getPlanes();
    }

    @Override
    public void close() {
        im.close();
        closed = true;
    }

    @Override
    public ByteBuffer getNV21ImageBuffer() {
        if (closed) {
            throw new IllegalStateException("Image is closed");
        }

        return nv21Buffer;
    }

    @Override
    public int getNV21Stride() {
        if (closed) {
            throw new IllegalStateException("Image is closed");
        }

        return nv21Stride;
    }
}
