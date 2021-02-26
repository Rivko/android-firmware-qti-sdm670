/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.panoramaui.data;

import android.widget.ImageView;

/**
 * This class stores the Frame related information
 */
public class FrameInfo {
    public int id = -1;
    public ImageView imageView = null;

    /**
     * Constructor to initialize the Frame Info
     *
     * @param id
     *            id of the frame
     */
    public FrameInfo(int id) {
        this.id = id;
    }
}
