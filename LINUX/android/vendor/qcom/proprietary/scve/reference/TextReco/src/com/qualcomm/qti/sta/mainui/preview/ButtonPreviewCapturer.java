/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.sta.mainui.preview;

import android.content.Context;
import android.util.Log;

import com.qualcomm.qti.sta.device.CameraHandler;
import com.qualcomm.qti.sta.device.CameraHandler.CameraListener;
import com.qualcomm.qti.sta.mainui.ImageCapturer;
import com.qualcomm.qti.sta.mainui.CameraActivity.CaptureType;

/**
 * class for the button-preview capturer
 */
public class ButtonPreviewCapturer extends ImageCapturer {
    private static final String LOG_TAG = "ButtonPreviewCapturer";

    /**
     * Constructor for the class
     *
     * @param context
     *            application context
     * @param camera
     *            camera object
     * @param cameraListener
     *            camera listener
     */
    public ButtonPreviewCapturer(Context context, CameraListener cameraListener) {
        super(CaptureType.BUTTON_PREVIEW, context, cameraListener);

        Log.d(LOG_TAG, "State: UNKNOWN");
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean start(CameraHandler cameraHandler) {
        if (captureStatus) {
            Log.d(LOG_TAG, "Capturing already on");
            return false;
        }
        captureStatus = true;
        if (cameraListener != null) {
            cameraListener.onFrameSessionStarted();
        }
        return true;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void stop() {
        if (!captureStatus) {
            Log.d(LOG_TAG, "Capturing already off");
            return;
        }
        captureStatus = false;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void wrapUp() {
        stop();
        Log.d(LOG_TAG, "Image capturer wrapped");
    }
}
