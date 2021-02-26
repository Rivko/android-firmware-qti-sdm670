/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.videoprocessing.VideoFrameUtils;

import java.nio.ByteBuffer;
import java.util.Objects;

/**
 * This class wraps a buffer containing YUV 4:2:0 semi-planar with metadata describing that image.
 * It can either allocate a new buffer for the image, or it can wrap an existing buffer. The byte
 * order of the chroma plane (UV/VU) is unspecified, and depends on the usage of the buffer.
 */
public class FrameYUV420SP {
    /** A direct {@link ByteBuffer} containing the encoded frame */
    public final ByteBuffer buf;

    /** Width of the frame, in px. This will always be > 0. */
    public final int width;

    /** Height of the frame, in px. This will always be > 0. */
    public final int height;

    /**
     * Offset of the start of the Y plane from the start of buf, in bytes. This will always be >= 0.
     */
    public final int offsetY;

    /**
     * Distance between the start of one row and the start of the next in the Y plane, in bytes.
     * This will always be >= {@link #width}.
     */
    public final int strideY;

    /** Width of the UV plane, in px. This will always be ceil({@link #width} / 2f). */
    public final int widthUV;

    /** Height of the UV plane, in px. This will always be ceil({@link #width} / 2f). */
    public final int heightUV;

    /**
     * Offset of the start of the UV plane from the start of the Y plane, in bytes. This will always
     * be >= ({@link #strideY} * {@link #height}).
     */
    public final int offsetUV;

    /**
     * Distance between the start of one row and the start of the next in the UV plane, in bytes.
     * This will always be >= ({@link #widthUV} * 2).
     */
    public final int strideUV;

    /**
     * Construct a new {@link FrameYUV420SP} and corresponding {@link ByteBuffer}. {@link #strideY}
     * and {@link #strideUV} will be width rounded up to the next multiple of 16. {@link #offsetY}
     * will be 0, and {@link #offsetUV} will be ({@link #strideY} * {@link #height}).
     * @param width width of the frame, in px
     * @param height height of the frame, in px
     * @throws IllegalArgumentException if width or height are <= 0
     */
    public FrameYUV420SP(int width, int height) {
        this(width, height, roundUpMult16(width));
    }

    /**
     * Construct a new {@link FrameYUV420SP} and corresponding {@link ByteBuffer}. Both planes will
     * have the specified stride. {@link #offsetY} will be 0, and {@link #offsetUV} will be
     * (stride * {@link #height}).
     * @param width width of the frame, in px
     * @param height height of the frame, in px
     * @param stride  stride of the Y and UV planes, in bytes
     * @throws IllegalArgumentException if width or height are <= 0, or if stride is <
     *         (ceil(width / 2) * 2)
     */
    public FrameYUV420SP(int width, int height, int stride) {
        this(ByteBuffer.allocateDirect(stride * (height + (height + 1) / 2)), width, height,
                0, stride, stride * height, stride);
    }

    /**
     * Construct a new {@link FrameYUV420SP}, wrapping the specified {@link ByteBuffer}
     * @param buf a direct {@link ByteBuffer} for the encoded frame data
     * @param width width of the frame, in px
     * @param height height of the frame, in px
     * @param offsetY offset of the start of the Y plane from the start of buf, in bytes
     * @param strideY distance between the start of one row and the start of the next in the Y
     *                plane, in bytes
     * @param offsetUV offset of the start of the UV plane from the start of the Y plane, in bytes
     * @param strideUV distance between the start of one row and the start of the next in the UV
     *                 plane, in bytes
     * @throws NullPointerException if buf is null
     * @throws IllegalArgumentException if buf is not direct,  if width or height are <= 0, if
     *         offsetY is < 0, if strideY < width, if offsetUV < (strideY * height), if strideUV <
     *         (ceil(width / 2) * 2), or if the capacity of buf is not large enough to contain the
     *         specified frame
     */
    public FrameYUV420SP(ByteBuffer buf, int width, int height, int offsetY, int strideY,
                         int offsetUV, int strideUV) {
        Objects.requireNonNull("buf", "buf may not be null");

        if (!buf.isDirect()) {
            throw new IllegalArgumentException("buf must be a direct ByteBuffer");
        } else if (width <= 0 || height <= 0) {
            throw new IllegalArgumentException("width and height must be > 0");
        } else if (offsetY < 0) {
            throw new IllegalArgumentException("offsetY must be >= 0");
        } else if (strideY < width) {
            throw new IllegalArgumentException("strideY must be >= width");
        }

        this.buf = buf;
        this.width = width;
        this.height = height;
        this.offsetY = offsetY;
        this.strideY = strideY;

        widthUV = (width + 1) / 2;
        heightUV = (height + 1) / 2;

        if (offsetUV < (strideY * height)) {
            throw new IllegalArgumentException("offsetUV must be >= strideY * height");
        } else if (strideUV < (widthUV * 2)) {
            throw new IllegalArgumentException("strideUV must be >= ceil(width / 2) * 2");
        }

        this.offsetUV = offsetUV;
        this.strideUV = strideUV;

        if (buf.capacity() < (offsetY + offsetUV + strideUV * heightUV)) {
            throw new IllegalArgumentException("buf is too small to contain the specified image");
        }
    }

    private static int roundUpMult16(int n) {
        return ((n + 15) & ~15);
    }
}
