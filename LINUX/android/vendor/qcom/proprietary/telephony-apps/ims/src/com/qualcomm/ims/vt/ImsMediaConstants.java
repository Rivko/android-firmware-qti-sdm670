/* Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.ims.vt;

public class ImsMediaConstants {
    // Use QVGA as default resolution
    public static final int DEFAULT_WIDTH = 240;
    public static final int DEFAULT_HEIGHT = 320;
    public static final int DEFAULT_FPS = 20;

    // UI Orientation Modes
    public static final int LANDSCAPE_MODE = 1;
    public static final int PORTRAIT_MODE = 2;
    public static final int DYNAMIC_MODE = 3;

    // Identifiers for front or back cameras
    public static final int CAMERA_FACING_BACK = 0;
    public static final int CAMERA_FACING_FRONT = 1;

    // Following values 0, 1, and 2 are from the IMS VT API documentation
    public static final int VIDEO_QUALITY_UNKNOWN = -1;
    public static final int VIDEO_QUALITY_LOW = 0;
    public static final int VIDEO_QUALITY_MEDIUM = 1;
    public static final int VIDEO_QUALITY_HIGH = 2;
}
