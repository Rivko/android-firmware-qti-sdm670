/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.clevercapture.data;

/**
 * This class define all the device speed related state and threshold value
 *
 */
public class DeviceSpeed {

    public static final int HIGH_SPEED_DELTA_THRESHOLD = 100;
    public static final int MEDIUM_SPEED_DELTA_THRESHOLD = 50;

    /**
     * Private constructor to prevent this utility-like class being sub-classed
     */
    private DeviceSpeed() {}

    public enum DeviceSpeedState {
        HIGH,
        MEDIUM,
        NORMAL,
        UNKNOWN
    }
}
