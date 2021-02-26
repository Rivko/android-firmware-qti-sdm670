/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.sta.mainui.snapshot;

import android.content.Context;
import android.util.Log;

import com.qualcomm.qti.sta.device.CameraHandler;
import com.qualcomm.qti.sta.device.CameraHandler.CameraListener;
import com.qualcomm.qti.sta.mainui.ImageCapturer;
import com.qualcomm.qti.sta.mainui.CameraActivity.CaptureType;

/**
 * Class for capturing the camera image from snapshot
 */
public class SnapshotCapturer extends ImageCapturer {
    private static final String LOG_TAG = "SnapshotCapturer";

    /**
     * Constructor for the class
     *
     * @param mode
     *            Camera mode
     * @param context
     *            application context
     * @param cameraListener
     *            camera listener
     */
    public SnapshotCapturer(Context context, CameraListener cameraListener) {
        super(CaptureType.SNAPSHOT, context, cameraListener);

        Log.d(LOG_TAG, "Snapshot Capturer initialized");
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

        cameraHandler.startSnapshot();
        captureStatus = true;
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
