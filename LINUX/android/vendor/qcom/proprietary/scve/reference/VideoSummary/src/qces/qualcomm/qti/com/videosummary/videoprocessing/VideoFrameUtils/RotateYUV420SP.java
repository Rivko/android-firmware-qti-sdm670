/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.videoprocessing.VideoFrameUtils;

import java.nio.ByteBuffer;
import java.util.Objects;

/**
 * This class provides helpers for rotating {@link FrameYUV420SP}-encoded frames
 */
public class RotateYUV420SP {
    static {
        System.loadLibrary("VideoFrameUtils");
    }

    // Not constructable
    private RotateYUV420SP() {}

    /**
     * Rotate the provided NV-21 encoded source image, storing the result into the provided
     * destination buffer
     * @param src the {@link FrameYUV420SP} to rotate
     * @param dst the {@link FrameYUV420SP} into which to store the rotated frame
     * @param degrees the clockwise angle by which src should be rotated
     * @throws NullPointerException if src or dst are null
     * @throws IllegalArgumentException if degrees is not 0/90/180/270, or if dst width/height do
     *         not match the post-rotation width/height of src
     */
    public static void rotate(FrameYUV420SP src, FrameYUV420SP dst, int degrees) {
        Objects.requireNonNull(src, "src may not be null");
        Objects.requireNonNull(dst, "dst may not be null");

        if (degrees == 0 || degrees == 180) {
            if (src.width != dst.width || src.height != dst.height) {
                throw new IllegalArgumentException("src and dst must have same width/height");
            }
        } else if (degrees == 90 || degrees == 270) {
            if (src.width != dst.height || src.height != dst.width) {
                throw new IllegalArgumentException("src and dst must have swapped width/height");
            }
        } else {
            throw new IllegalArgumentException("degrees must be 0, 90, 180, or 270");
        }

        nRotate(src.buf, src.width, src.height, src.offsetY, src.strideY, src.offsetUV,
                src.strideUV, dst.buf, dst.offsetY, dst.strideY, dst.offsetUV, dst.strideUV,
                degrees);
    }

    private static native void nRotate(ByteBuffer src, int srcWidth, int srcHeight, int srcOffsetY,
                                       int srcStrideY, int srcOffsetUV, int srcStrideUV,
                                       ByteBuffer dst, int dstOffsetY, int dstStrideY,
                                       int dstOffsetUV, int dstStrideUV, int degrees);
}
