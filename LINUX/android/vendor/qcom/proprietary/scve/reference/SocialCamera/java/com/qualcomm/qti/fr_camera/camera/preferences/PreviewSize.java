/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.fr_camera.camera.preferences;

/**
 * Supported sizes for the camera preview stream
 */
public enum PreviewSize {
    /**
     * The preview stream should be sized to match the device display resolution as closely as
     * possible, while respecting the selected picture aspect ratio
     */
    DISPLAY_RESOLUTION,

    /**
     * The preview stream should be sized as close as possible to 4MPixels, while respecting the
     * selected picture aspect ratio
     */
    HIGH_RESOLUTION
}
