/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.sta.mainui;

import android.content.Context;

import com.qualcomm.qti.sta.device.CameraHandler;
import com.qualcomm.qti.sta.device.CameraHandler.CameraListener;
import com.qualcomm.qti.sta.mainui.CameraActivity.CaptureType;
import com.qualcomm.qti.sta.mainui.preview.AutoPreviewCapturer;
import com.qualcomm.qti.sta.mainui.preview.ButtonPreviewCapturer;
import com.qualcomm.qti.sta.mainui.snapshot.SnapshotCapturer;

/**
 * Base class for capturing image from camera
 */
public abstract class ImageCapturer {
    protected final Context context;
    protected final CameraListener cameraListener;
    private final CaptureType captureType;

    protected boolean captureStatus = false;

    /**
     * Constructor for the class
     *
     * @param captureType
     *            capture mode
     * @param context
     *            application context
     * @param cameraListener
     *            camera listener
     */
    protected ImageCapturer(CaptureType captureType, Context context, CameraListener cameraListener) {
        this.captureType = captureType;
        this.context = context;
        this.cameraListener = cameraListener;
    }

    /**
     * Factory method to get the image capturer based on mode
     *
     * @param captureType
     *            capture mode
     * @param context
     *            application context
     * @param cameraListener
     *            camera listener
     * @return Image capturer
     */
    public static ImageCapturer getInstance(CaptureType captureType, Context context,
            CameraListener cameraListener) {
        if (captureType == CaptureType.SNAPSHOT) {
            return new SnapshotCapturer(context, cameraListener);
        } else if (captureType == CaptureType.BUTTON_PREVIEW) {
            return new ButtonPreviewCapturer(context, cameraListener);
        } else if (captureType == CaptureType.AUTO_PREVIEW) {
            return new AutoPreviewCapturer(context, cameraListener);
        }
        return null;
    }

    /**
     * Gets the capture status
     *
     * @return capture status
     */
    public boolean isCaptureOn() {
        return captureStatus;
    }

    /**
     * Notification on ROI changed
     *
     * @param cameraHandler
     *            camera handler instance
     */
    public void onRoiChanged(CameraHandler cameraHandler) {
    }

    /**
     * checks the capture type
     *
     * @param captureType
     *            this method checks the capturer type with this specified capture type
     * @return true if capture type is same, false otherwise
     */
    public boolean isType(CaptureType captureType) {
        return (this.captureType == captureType);
    }

    /**
     * Notification on starting the capturer
     *
     * @param cameraHandler
     *            camera handler instance
     * @return true if started successfully, false otherwise
     */
    public abstract boolean start(CameraHandler cameraHandler);

    /**
     * Notification on stop-ing the capturer
     */
    public abstract void stop();

    /**
     * Wraps up the capturer
     */
    public abstract void wrapUp();
}
