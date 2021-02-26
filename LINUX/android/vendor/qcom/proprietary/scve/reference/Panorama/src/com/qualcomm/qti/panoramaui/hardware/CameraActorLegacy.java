/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.panoramaui.hardware;

import java.io.IOException;
import java.util.List;

import android.app.Activity;
import android.graphics.ImageFormat;
import android.graphics.Point;
import android.graphics.PointF;
import android.hardware.Camera;
import android.hardware.Camera.CameraInfo;
import android.hardware.Camera.Parameters;
import android.hardware.Camera.Size;
import android.util.Log;

import com.qualcomm.qti.panoramaui.R;
import com.qualcomm.qti.panoramaui.AeAfUIHandler;
import com.qualcomm.qti.panoramaui.CameraPreviewView;
import com.qualcomm.qti.panoramaui.data.AppError;
import com.qualcomm.qti.panoramaui.hardware.AeAfHandler.AeAfType;
import com.qualcomm.qti.panoramaui.preferences.CameraPreferenceManager;

/**
 * This class handles all the camera interactions, using the legacy {@link Camera} APIs
 */
@SuppressWarnings("deprecation")
public class CameraActorLegacy extends CameraActor {
    private static final String LOG_TAG = CameraActorLegacy.class.getSimpleName();
    private static final String KEY_QC_ZSL = "zsl";
    private static final String SUPPORTED_VALUES_SUFFIX = "-values";
    private static final String ZSL_ON = "on";
    private static final String KEY_QC_ZSL_BURST_LOOKBACK = "capture-burst-retroactive";

    private Camera camera;
    private final CameraInfo cameraInfo = new CameraInfo();
    private Parameters cameraParameters;
    private CameraPreviewView cameraView;
    private int cameraDisplayOrientation;
    private Point previewSize = new Point();
    private float focalLength;

    private AeAfLegacy aeAfHandler = null;

    // ZSL mode may not supported on some devices

    /**
     * Construct a {@link CameraActorLegacy} instance, which implements the {@link CameraActor}
     * contract with the legacy Android {@link Camera} APIs
     *
     * @param parentActivity
     *            {@link Activity} which owns this {@link CameraActorLegacy}
     * @param aeAfEventListener
     *            AeAFEvent listener
     */
    public CameraActorLegacy(Activity parentActivity, AeAfUIHandler aeAfUIHandler) {
        super(parentActivity, aeAfUIHandler);
    }

    /** {@inheritDoc} */
    @Override
    public AppError startCamera(boolean zslEnabled) {
        int jpegQuality = CameraPreferenceManager.getImageCompressLevel(parentActivity);
        int cameraId = CameraPreferenceManager.getSelectedCameraId(parentActivity);
        zslOn = zslEnabled;

        try {
            camera = Camera.open(cameraId);
        } catch (Exception e) {
            Log.d(LOG_TAG, "Camera open failed: " + e.getMessage());
            camera = null;
        }
        if (camera == null) {
            Log.d(LOG_TAG, "Camera failed to get initialized");
            return AppError.CAMERA_FAILED;
        }

        Camera.getCameraInfo(cameraId, cameraInfo);
        cameraDisplayOrientation = getCameraDisplayOrientation(cameraInfo.orientation,
                cameraInfo.facing == CameraInfo.CAMERA_FACING_FRONT);

        camera.enableShutterSound(false);
        cameraParameters = camera.getParameters();
        CameraPreferenceManager.applyLegacyCameraPreferences(parentActivity, cameraId,
                cameraParameters);
        if (zslOn) {
        String supportedZslModes = cameraParameters.get(KEY_QC_ZSL + SUPPORTED_VALUES_SUFFIX);

        if (supportedZslModes != null && supportedZslModes.contains(ZSL_ON)) {

            cameraParameters.set(KEY_QC_ZSL, ZSL_ON);
            Log.i(LOG_TAG, "Set ZSL to [" + ZSL_ON + "]");
            cameraParameters.set(KEY_QC_ZSL_BURST_LOOKBACK, 2); // N+1 == 1 frame of lookback
            Log.i(LOG_TAG, "Set ZSL_BURST_LOOKBACK to [" + 2 + "]");
            } else {
                return AppError.ZSL_FAILED;
            }
        }

        // Match preview to selected picture size with best try effort
        Size pictureSize = cameraParameters.getPictureSize();
        List<Size> supportedPreviewSizes = cameraParameters.getSupportedPreviewSizes();
        previewSize = selectPreviewSize(supportedPreviewSizes, pictureSize);
        cameraParameters.setPreviewSize(previewSize.x, previewSize.y);
        Log.i(LOG_TAG, "Set picture size to [" + pictureSize.width + "x" + pictureSize.height + "]");
        Log.i(LOG_TAG, "Set preview size to [" + previewSize.x + "x" + previewSize.y + "]");

        cameraParameters.setJpegQuality(jpegQuality);
        cameraParameters.setRotation(cameraDisplayOrientation);
        cameraParameters.setPreviewFormat(ImageFormat.NV21);
        camera.setParameters(cameraParameters);

        cameraView = (CameraPreviewView) parentActivity.findViewById(R.id.camera_preview_view);
        cameraView.setPreviewFrameRotation(cameraDisplayOrientation);
        cameraView.setPreviewFrameSize(getWidth(), getHeight());

        try {
            camera.setPreviewTexture(cameraView.getSurfaceTexture());
        } catch (IOException e) {
            Log.e(LOG_TAG, "Failed setting camera preview SurfaceTexture", e);
            throw new RuntimeException("Failed setting camera preview SurfaceTexture", e);
        }

        boolean shouldLock3A = CameraPreferenceManager.getEnable3A(parentActivity);
        aeAfHandler = new AeAfLegacy(aeAfUIHandler, cameraParameters, shouldLock3A, this);
        aeAfHandler.configure(camera, AeAfType.CONTINUOUS);

        camera.startPreview();

        focalLength = cameraParameters.getFocalLength();
        Log.i(LOG_TAG, "Lens focal length is [" + focalLength + "]");

        return AppError.NONE;
    }

    /** {@inheritDoc} */
    @Override
    public int getCameraOrientation() {
        return cameraDisplayOrientation;
    }

    /** {@inheritDoc} */
    @Override
    public int getWidth() {
        return (cameraDisplayOrientation == 0 || cameraDisplayOrientation == 180 ? previewSize.x
                : previewSize.y);
    }

    /** {@inheritDoc} */
    @Override
    public int getHeight() {
        return (cameraDisplayOrientation == 0 || cameraDisplayOrientation == 180 ? previewSize.y
                : previewSize.x);
    }

    /** {@inheritDoc} */
    @Override
    public float getFocalLength() {
        return focalLength;
    }

    /** {@inheritDoc} */
    @Override
    public void wrapUp() {
        Log.d(LOG_TAG, "Camera Actor Wrapped");
        if (camera != null) {
            camera.release();
            camera = null;
        }
        if (aeAfHandler != null) {
            aeAfHandler.clear();
            aeAfHandler = null;
        }
        cameraView = null;
        previewSize.set(0, 0);

    /**
     * Return a boolean true when ZSL is supported, otherwise return false
     *
     * @return true is ZSL mode is supported
     */
    }

    /** {@inheritDoc} */
    @Override
    public void onCapture(boolean lock) {
        // Because onStop and onCaptureCancel both run on UI thread in unpredictable order
        // camera may become null at that moment.
        if (camera != null) {
            if (!aeAfHandler.is3aLockSupported() || aeAfHandler.is3aLocked() == lock) {
                return;
            }

            aeAfHandler.configure(camera, AeAfType.LOCK_AND_FOCUS);
        }
    }

    /** {@inheritDoc} */
    @Override
    public void setAeAfRegion(PointF touch) {
        if (camera == null) {
            return;
        }
        if (!isTouchValid(touch.y, cameraView.getHeight())) {
            return;
        }
        aeAfHandler.setRegionAt(touch, cameraView.getWidth(), cameraView.getHeight(),
                cameraDisplayOrientation);
        aeAfHandler.configure(camera, AeAfType.LOCK_AND_FOCUS_REGION);
    }

    /** {@inheritDoc} */
    /* package */void captureKeyFrame(final OnKeyFrameCapturedListener jpegCallback) {
        handler.post(new Runnable() {
            @Override
            public void run() {
                try {
                    camera.takePicture(null, null, null, new Camera.PictureCallback() {
                        @Override
                        public void onPictureTaken(byte[] data, Camera camera) {
                            jpegCallback.onKeyFrameCaptured(data);
                        }
                    });
                } catch (Exception e) {
                    jpegCallback.onKeyFrameCaptured(null);
                    Log.e(LOG_TAG, "Camera takePicture action failed", e);
                }
            }
        });
    }

    /**
     * See {@link #selectPreviewSize(Point[], Point)}
     */
    private Point selectPreviewSize(List<Size> supportedPreviewSizes, Size pictureSize) {
        Point[] supportedSizes = new Point[supportedPreviewSizes.size()];
        int i = 0;
        for (Size size : supportedPreviewSizes) {
            supportedSizes[i++] = new Point(size.width, size.height);
        }

        return selectPreviewSize(supportedSizes, new Point(pictureSize.width, pictureSize.height));
    }

    /** {@inheritDoc} */
    @Override
    public void onAutoFocusFailed() {
        if (camera == null) {
            Log.w(LOG_TAG, "onAutoFocusFailed but camera null");
            return;
        }
        Log.d(LOG_TAG, "Camera AF Failed, setiing back continuous");
        aeAfHandler.configure(camera, AeAfType.CONTINUOUS);
        aeAfUIHandler.autoFocusComplete(false);
    }

    /** {@inheritDoc} */
    @Override
    public void onAutoFocusExpired() {
        if (camera == null) {
            Log.w(LOG_TAG, "onAutoFocusExpired but camera null");
            return;
        }
        Log.d(LOG_TAG, "Camera AF Expired, setiing back continuous");
        aeAfHandler.configure(camera, AeAfType.CONTINUOUS);
    }

    /**
     * Check if the ZSL is supported in legacy camera apis
     *
     * @param parent
     *            activity
     * @return true if supported, false otherwise
     */
    public static boolean probeZSLSupported(Activity parent) {
        boolean isZslSupportedTmp = false;
        int cameraId = CameraPreferenceManager.getSelectedCameraId(parent);
        Camera cameraTmp = null;
        try {
            cameraTmp = Camera.open(cameraId);
        } catch (Exception e) {
            Log.d(LOG_TAG, "Camera open failed: " + e.getMessage());
            cameraTmp = null;
        }
        if (cameraTmp == null) {
            return false;
        }
        Parameters cameraParametersTmp = cameraTmp.getParameters();
        Log.i(LOG_TAG, "cameraParameters - " + cameraParametersTmp.flatten());
        String supportedZslModes = cameraParametersTmp.get(KEY_QC_ZSL + SUPPORTED_VALUES_SUFFIX);
        Log.i(LOG_TAG, "supportedZslModes " + supportedZslModes);
        if (supportedZslModes != null && supportedZslModes.contains(ZSL_ON)) {
            isZslSupportedTmp = true;
        }
        cameraTmp.release();
        return isZslSupportedTmp;
    }
}
