/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package android.media;

import android.graphics.ImageFormat;

import java.nio.ByteBuffer;

/**
 * This class extends {@link Image} to add the capability to directly retrieve an NV21-formatted
 * direct {@link ByteBuffer}. Note that, implicitly, all {@link ImageNV21}s will
 * {@link ImageFormat#YUV_420_888} from {@link Image#getFormat()}.
 */
public abstract class ImageNV21 extends Image {
    /**
     * Get an NV21-formatted direct {@link ByteBuffer} for this {@link Image}. The stride of this
     * buffer can be obtained from {@link #getNV21Stride()}. The VU plane directly follows the Y
     * plane, with no intermediate padding.
     * @return an NV21-formatted direct {@link ByteBuffer}
     */
    public abstract ByteBuffer getNV21ImageBuffer();

    /**
     * Get the row stride for the NV21-formatted direct {@link ByteBuffer}. The stride of the Y and
     * VU planes are guaranteed to be identical.
     * @return the row stride for the NV21-formatted direct {@link ByteBuffer}
     */
    public abstract int getNV21Stride();
}
