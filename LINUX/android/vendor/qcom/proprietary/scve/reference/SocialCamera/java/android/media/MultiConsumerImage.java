/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package android.media;

import android.annotation.TargetApi;
import android.graphics.Rect;
import android.os.Build;

import java.nio.ByteBuffer;
import java.util.Objects;
import java.util.concurrent.atomic.AtomicInteger;

/**
 * This class wraps an {@link Image} and allows multiple read-only copies of it to be created. Each
 * copy has its own lifetime and state (for the {@link ByteBuffer}s in the {@link Image.Plane}s).
 * For a given {@link Image}, both the original {@link MultiConsumerImage}s, plus all
 * {@link MultiConsumerImage}s created from it via {@link #duplicate()}, must be {@link #close()}d
 * before the underlying {@link Image} will be {@link Image#close()}d.
 */
public class MultiConsumerImage extends Image {
    private final Image im;
    private final AtomicInteger count;

    private final ClonedPlane[] planes;
    private long timestamp;
    private final Rect cropRect;
    private boolean closed;

    /**
     * Construct a new {@link MultiConsumerImage} for the given {@link Image}. Once an {@link Image}
     * is wrapped by a {@link MultiConsumerImage}, it should no longer be used directly; the
     * {@link MultiConsumerImage} class will manage the lifetime of the underlying {@link Image}.
     * @param im an {@link Image} to wrap
     * @throws NullPointerException if im is null
     */
    public MultiConsumerImage(Image im) {
        Objects.requireNonNull(im, "im may not be null");

        this.im = im;
        this.count = new AtomicInteger(1);

        Image.Plane[] srcPlanes = im.getPlanes();
        planes = new ClonedPlane[srcPlanes.length];
        for (int i = 0; i < planes.length; i++) {
            planes[i] = new ClonedPlane(srcPlanes[i]);
        }

        timestamp = im.getTimestamp();
        cropRect = im.getCropRect();
    }

    private MultiConsumerImage(MultiConsumerImage other) {
        im = other.im;
        count = other.count;

        int oldCount = count.getAndIncrement();
        if (oldCount <= 0) {
            throw new AssertionError();
        }

        planes = new ClonedPlane[other.planes.length];
        for (int i = 0; i < planes.length; i++) {
            planes[i] = new ClonedPlane(other.planes[i]);
        }

        timestamp = other.timestamp;
        cropRect = new Rect(other.cropRect);
    }

    /**
     * Duplicate a {@link MultiConsumerImage}, producing another {@link MultiConsumerImage} which
     * shares the same underlying {@link Image}. All the mutable properties of {@link Image} (such
     * as timestamp, crop rect, and {@link Image.Plane} buffer position properties) will match those
     * of this {@link MultiConsumerImage} (as of the time of duplication).
     * @return a new {@link MultiConsumerImage} which shares the same underlying {@link Image} as
     *         this instance
     */
    public MultiConsumerImage duplicate() {
        if (closed) {
            throw new IllegalStateException("Image is closed");
        }

        return new MultiConsumerImage(this);
    }

    @Override
    public long getTimestamp() {
        if (closed) {
            throw new IllegalStateException("Image is closed");
        }

        return timestamp;
    }

    //@Override
    //@TargetApi(Build.VERSION_CODES.M)
    public void setTimestamp(long timestamp) {
        if (closed) {
            throw new IllegalStateException("Image is closed");
        }

        this.timestamp = timestamp;
    }

    @Override
    public Rect getCropRect() {
        if (closed) {
            throw new IllegalStateException("Image is closed");
        }

        return new Rect(cropRect);
    }

    @Override
    public void setCropRect(Rect cropRect) {
        if (closed) {
            throw new IllegalStateException("Image is closed");
        }

        cropRect.set(cropRect);
    }

    @Override
    public int getFormat() {
        if (closed) {
            throw new IllegalStateException("Image is closed");
        }

        return im.getFormat();
    }

    @Override
    public int getWidth() {
        if (closed) {
            throw new IllegalStateException("Image is closed");
        }

        return im.getWidth();
    }

    @Override
    public int getHeight() {
        if (closed) {
            throw new IllegalStateException("Image is closed");
        }

        return im.getHeight();
    }

    @Override
    public ClonedPlane[] getPlanes() {
        if (closed) {
            throw new IllegalStateException("Image is closed");
        }

        return planes;
    }

    @Override
    public void close() {
        if (closed) {
            throw new IllegalStateException("Image is closed");
        }

        closed = true;
        int val = count.decrementAndGet();
        if (val < 0) {
            throw new AssertionError();
        } else if (val == 0) {
            im.close();
        }
    }

    @Override
    protected void finalize() throws Throwable {
        super.finalize();

        if (!closed) {
            close();
        }
    }

    // N.B. Don't make this static; it should keep this MultiConsumerImage from being finalized
    private class ClonedPlane extends Image.Plane {
        private final Image.Plane srcPlane;
        private final ByteBuffer buffer;

        public ClonedPlane(Image.Plane srcPlane) {
            this.srcPlane = srcPlane;
            buffer = srcPlane.getBuffer().asReadOnlyBuffer();
        }

        public ClonedPlane(ClonedPlane other) {
            srcPlane = other.srcPlane;
            buffer = other.buffer.duplicate();
        }

        @Override
        public int getRowStride() {
            return srcPlane.getRowStride();
        }

        @Override
        public int getPixelStride() {
            return srcPlane.getPixelStride();
        }

        @Override
        public ByteBuffer getBuffer() {
            return buffer;
        }
    }
}
