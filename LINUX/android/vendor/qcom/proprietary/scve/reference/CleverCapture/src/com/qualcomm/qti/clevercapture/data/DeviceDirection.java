/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.clevercapture.data;

/**
 * This enum defines the direction in which device can move
 */
public enum DeviceDirection {
    UNKNOWN(-1),
    STILL(0),
    RIGHT (1),
    LEFT(2),
    TOP(3),
    BOTTOM(4);

    private final int direction;

    /**
     * returns the direction
     *
     * @return direction id
     */
    public int getDirectionId() {
        return direction;
    }
    /**
     * sets the direction
     *
     * @param directionId
     *         direction to be set
     */
    private DeviceDirection(int direction) {
        this.direction = direction;
    }
}
