/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.qces.khronos;

/**
 * Common colour formats definition
 */
public enum ColourFormat {
    /**
     * 32-bit RGBA format with bytes in B, G, R, A memory order
     */
    ARGB_8888(4),

    /**
     * 32-bit RGBA format with bytes in R, G, B, A memory order
     */
    ABGR_8888(4),

    /**
     * 8-bit YUV 4:2:0 semi-planar format.
     */
    NV12(1),

    /**
     * 8-bit YVU 4:2:0 semi-planar format.
     */
    NV21(1);

    public final int bytesPerPixel; // N.B. For planar formats, this is for the first plane

    private ColourFormat(int bytesPerPx) {
        bytesPerPixel = bytesPerPx;
    }
}
