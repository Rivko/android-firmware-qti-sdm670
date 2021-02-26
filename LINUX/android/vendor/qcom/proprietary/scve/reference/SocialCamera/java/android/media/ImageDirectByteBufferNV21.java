/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package android.media;

import android.graphics.ImageFormat;
import android.graphics.Rect;

import java.nio.ByteBuffer;
import java.util.Objects;

/**
 * An implementation of {@link ImageNV21} which wraps a NV21-formatted direct {@link ByteBuffer},
 * and exposes it (as either read-only {@link Image.Plane}s or as a read-only {@link ByteBuffer}).
 */
public class ImageDirectByteBufferNV21 extends ImageNV21 {
    private final ByteBuffer nv21BufferRO;
    private final Plane[] planes = new Plane[3];
    private final int width;
    private final int height;
    private final int stride;
    private boolean closed;
    private long timestamp;
    private Rect cropRect;

    /**
     * Construct a new {@link ImageDirectByteBufferNV21} instance from the specified direct
     * {@link ByteBuffer}. Note that this class only supports NV21 images with identical strides for
     * the Y and VU planes.
     * @param nv21Buffer a direct {@link ByteBuffer} whose contents are an NV21-formatted image.
     *                   The buffer limit must be at least stride * height.
     * @param width the width of the NV21-encoded image, in px
     * @param height the height of the NV21-encoded image, in px
     * @param stride the stride of both the Y and VU planes, in bytes. This must be at least width,
     *               rounded up to the next even integer.
     * @throws NullPointerException if nv21Buffer is null
     * @throws IllegalArgumentException if nv21Buffer is not direct, if width or height are < 0,
     *                                  if stride is not at least width rounded up to the next even
     *                                  integer, or if the limit of nv21Buffer is less than height *
     *                                  stride
     */
    public ImageDirectByteBufferNV21(ByteBuffer nv21Buffer, int width, int height, int stride) {
        Objects.requireNonNull(nv21Buffer, "nv21Buffer may not be null");

        if (!nv21Buffer.isDirect()) {
            throw new IllegalArgumentException("nv21Buffer must be a direct ByteBuffer");
        } else if (width < 0 || height < 0) {
            throw new IllegalArgumentException("width and height must be non-negative integers");
        }

        final int widthVU = (width + 1) / 2;
        final int heightVU = (height + 1) / 2;

        if (stride < (2 * widthVU)) {
            throw new IllegalArgumentException("stride must be at least twice the width of the " +
                    "VU plane (which is equal to half of width, rounded up)");
        } else if (nv21Buffer.limit() < ((height + heightVU) * stride)) {
            throw new IllegalArgumentException("nv21Buffer too small to contain the specified " +
                    "NV21 image");
        }

        this.width = width;
        this.height = height;
        this.stride = stride;

        nv21BufferRO = nv21Buffer.asReadOnlyBuffer();

        nv21BufferRO.position(height * stride);
        ByteBuffer bufferV = nv21BufferRO.slice();
        bufferV.limit(stride * heightVU - 1);

        nv21BufferRO.position(height * stride + 1);
        ByteBuffer bufferU = nv21BufferRO.slice();
        bufferU.limit(stride * heightVU - 1);

        nv21BufferRO.position(0);
        ByteBuffer bufferY = nv21BufferRO.slice();
        bufferY.limit(stride * height);

        planes[0] = new Plane(bufferY, false);
        planes[1] = new Plane(bufferV, true);
        planes[2] = new Plane(bufferU, true);
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

    @Override
    public Plane[] getPlanes() {
        if (closed) {
            throw new IllegalStateException("Image is closed");
        }

        return planes;
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
    public void setCropRect(Rect cropRect) {
        if (closed) {
            throw new IllegalStateException("Image is closed");
        }

        if (this.cropRect == null) {
            this.cropRect = new Rect();
        }

        if (cropRect == null) {
            this.cropRect.setEmpty();
            return;
        }

        this.cropRect.set(cropRect);
        if (!this.cropRect.intersect(0, 0, width, height)) {
            this.cropRect.setEmpty();
        }
    }

    @Override
    public Rect getCropRect() {
        if (closed) {
            throw new IllegalStateException("Image is closed");
        }

        if (cropRect == null) {
            cropRect = new Rect(0, 0, width, height);
        }

        return new Rect(cropRect);
    }

    @Override
    public void close() {
        if (!closed) {
            closed = true;
            onClosed();
        }
    }

    @Override
    public ByteBuffer getNV21ImageBuffer() {
        if (closed) {
            throw new IllegalStateException("Image is closed");
        }

        return nv21BufferRO;
    }

    @Override
    public int getNV21Stride() {
        if (closed) {
            throw new IllegalStateException("Image is closed");
        }

        return stride;
    }

    protected void onClosed() {
    }

    @Override
    protected void finalize() throws Throwable {
        super.finalize();
        close();
    }

    // N.B. Don't make static; a reference to a Plane should keep the ImageBufferNV21 from being
    // finalized
    private class Plane extends Image.Plane {
        private final ByteBuffer buf;
        private boolean isVUPlane;

        public Plane(ByteBuffer buf, boolean isVUPlane) {
            this.buf = buf;
            this.isVUPlane = isVUPlane;
        }

        @Override
        public int getRowStride() {
            return stride;
        }

        @Override
        public int getPixelStride() {
            return (isVUPlane ? 2 : 1);
        }

        @Override
        public ByteBuffer getBuffer() {
            return buf;
        }
    }
}
