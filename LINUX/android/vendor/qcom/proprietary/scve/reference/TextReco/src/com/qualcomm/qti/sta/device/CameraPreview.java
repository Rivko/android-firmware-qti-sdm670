/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.sta.device;

import android.content.Context;
import android.util.AttributeSet;
import android.view.SurfaceView;

/**
 * This class handles the show and hide of the camera preview
 */
public class CameraPreview extends SurfaceView {
    // private static final String LOG_TAG = "CameraPreviewView";

    /**
     * Constructor for the camera
     *
     * @param context
     *            application context
     * @param attributes
     *            attributes
     * @param defStyle
     *            style
     */
    public CameraPreview(Context context, AttributeSet attributes, int defStyle) {
        super(context, attributes, defStyle);
    }

    /**
     * Constructor for the camera
     *
     * @param context
     *            application context
     * @param attributes
     *            attribute set
     */
    public CameraPreview(Context context, AttributeSet attributes) {
        super(context, attributes);
    }

    /**
     * Constructor for the camera
     *
     * @param context
     *            application context
     */
    public CameraPreview(Context context) {
        super(context);
    }
}
