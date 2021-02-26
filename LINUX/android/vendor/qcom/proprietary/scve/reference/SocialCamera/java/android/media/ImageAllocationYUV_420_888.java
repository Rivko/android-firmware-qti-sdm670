/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package android.media;

import android.graphics.Bitmap;
import android.graphics.ImageFormat;
import android.renderscript.Allocation;
import android.renderscript.RenderScript;

import java.nio.ByteBuffer;
import java.util.Objects;
import java.util.concurrent.atomic.AtomicInteger;

/**
 * This {@link Image} wraps a set of {@link RenderScript} allocations which describe an
 * {@link ImageFormat#YUV_420_888} image
 */
public class ImageAllocationYUV_420_888 extends Image {
    private final int width;
    private final int height;
    private final Plane[] planes;
    private boolean closed;
    private long timestamp;

    /**
     * Construct a new {@link ImageAllocationYUV_420_888}
     * @param rs {@link RenderScript} context with which this instance should be associated. If rs
     *           is destroyed, this {@link ImageAllocationYUV_420_888} instance becomes undefined.
     * @param width width of this image, in px
     * @param height height of this image, in px
     * @throws NullPointerException if rs is null
     * @throws IllegalArgumentException if width or height are <= 0
     */
    public ImageAllocationYUV_420_888(RenderScript rs, int width, int height) {
        super();

        Objects.requireNonNull(rs, "rs may not be null");

        if (width < 1 || height < 1) {
            throw new IllegalArgumentException("width and height must be positive");
        }

        this.width = width;
        this.height = height;

        planes = new Plane[3];
        planes[0] = new Plane(rs, width, height);
        planes[1] = new Plane(rs, (width + 1) / 2, (height + 1) / 2);
        planes[2] = new Plane(rs, (width + 1) / 2, (height + 1) / 2);
    }

    /**
     * Create a new {@link ImageAllocationYUV_420_888} from an existing Image. This new Image
     * references the same memory as the existing Image; changes to one will be observed by the
     * other. The underlying memory for an {@link ImageAllocationYUV_420_888} will be retained until
     * all Images referencing it have been {@link #close()}d
     * @param copy a {@link ImageAllocationYUV_420_888} to copy
     * @throws NullPointerException if copy is null
     * @throws IllegalArgumentException if copy has been {@link #close()}d
     */
    protected ImageAllocationYUV_420_888(ImageAllocationYUV_420_888 copy) {
        super();

        Objects.requireNonNull(copy, "copy may not be null");

        if (copy.closed) {
            throw new IllegalArgumentException("Image has been closed, cannot copy");
        }

        width = copy.width;
        height = copy.height;
        planes = copy.planes;
        for (Plane p : planes) {
            if (p.refCount.getAndIncrement() == 0) {
                p.refCount.set(0);
                throw new IllegalArgumentException("Plane has been closed, cannot copy");
            }
        }
    }

    @Override
    public int getFormat() {
        if (closed) {
            throw new IllegalStateException("Image is closed");
        }

        return ImageFormat.YUV_420_888;
    }

    @Override
    public int getWidth() {
        if (closed) {
            throw new IllegalStateException("Image is closed");
        }

        return width;
    }

    @Override
    public int getHeight() {
        if (closed) {
            throw new IllegalStateException("Image is closed");
        }

        return height;
    }

    //@Override
    public void setTimestamp(long timestamp) {
        if (closed) {
            throw new IllegalStateException("Image is closed");
        }

        this.timestamp = timestamp;
    }

    @Override
    public long getTimestamp() {
        if (closed) {
            throw new IllegalStateException("Image is closed");
        }

        return timestamp;
    }

    @Override
    public Plane[] getPlanes() {
        if (closed) {
            throw new IllegalStateException("Image is closed");
        }

        return planes.clone();
    }

    /**
     * Get the set of {@link Allocation}s, 1 for each plane (Y, U, and V). The producer of this
     * {@link ImageAllocationYUV_420_888} is responsible for rendering into these allocations.
     * @return an {@link Allocation}[] containing 1 allocation for each plane
     */
    public Allocation[] getAllocations() {
        return new Allocation[] { planes[0].a, planes[1].a, planes[2].a };
    }

    /**
     * Sync the {@link Image.Plane} buffers with the contents of the underlying {@link Allocation}s
     */
    public void syncAllocations() {
        for (Plane p : planes) {
            p.syncToBitmap();
        }
    }

    @Override
    public void close() {
        if (!closed) {
            onClose();

            closed = true;
            for (Plane p : planes) {
                p.close();
            }
        }
    }

    /**
     * Subclasses may override this method to provide custom handling when this
     * {@link ImageAllocationYUV_420_888} is {@link #close()}d, before the {@link Plane}s are
     * closed
     */
    protected void onClose() {
    }

    protected static class Plane extends Image.Plane {
        static {
            System.loadLibrary("allocationimage");
        }

        // N.B. Keep a copy of rs, so that it isn't GC'ed, which would make our Allocations invalid
        @SuppressWarnings("unused")
        private final RenderScript rs;

        private final AtomicInteger refCount;
        private final Bitmap b;
        private final Allocation a;
        private final ByteBuffer buf;
        private final int rowStride;

        private Plane(RenderScript rs, int width, int height) {
            this.rs = rs;
            refCount = new AtomicInteger(1);
            b = Bitmap.createBitmap(width, height, Bitmap.Config.ALPHA_8);
            a = Allocation.createFromBitmap(rs, b);
            buf = nLockBitmap(b);
            rowStride = b.getRowBytes();

            if (buf == null) {
                throw new RuntimeException("Failed to lock bitmap pixels");
            }
        }

        @Override
        public int getRowStride() {
            return rowStride;
        }

        @Override
        public int getPixelStride() {
            return 1;
        }

        @Override
        public ByteBuffer getBuffer() {
            return buf;
        }

        private void syncToBitmap() {
            a.copyTo(b);
        }

        private void close() {
            int remaining = refCount.decrementAndGet();

            if (remaining < 0) {
                throw new IllegalStateException("Plane closed too many times");
            } else if (remaining == 0) {
                a.destroy();
                nUnlockBitmap(b);
                b.recycle();
            }
        }

        private static native ByteBuffer nLockBitmap(Bitmap b);
        private static native void nUnlockBitmap(Bitmap b);
    }
}
