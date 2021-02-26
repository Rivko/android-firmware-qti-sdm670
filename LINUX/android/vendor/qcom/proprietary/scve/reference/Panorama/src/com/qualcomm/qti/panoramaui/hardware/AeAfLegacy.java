/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.panoramaui.hardware;

import java.util.ArrayList;
import java.util.List;

import android.graphics.PointF;
import android.graphics.Rect;
import android.hardware.Camera;
import android.hardware.Camera.AutoFocusCallback;
import android.hardware.Camera.Parameters;
import android.util.Log;

import com.qualcomm.qti.panoramaui.AeAfUIHandler;

/**
 * class to manage AE AF settings
 */
@SuppressWarnings("deprecation")
public class AeAfLegacy extends AeAfHandler {
    private static final String TAG = AeAfLegacy.class.getSimpleName();
    private static final int CAMERA_AREA_SIZE = 2000;

    private static final float FOCUS_RECT_MULTIPLIER = 0.1f;// percentage
    private static final float METERING_RECT_MULTIPLIER = 0.15f;// percentage

    public final boolean meteringRegionEnable;
    public final boolean focusRegionEnable;
    private Rect focusRect = null;
    private Rect meteringRect = null;

    /**
     * Constructor for the class
     *
     * @param aeAfUIHandler
     *            Ae AF UI Handler
     * @param cameraParameters
     *            camera parameters
     * @param shouldLock3A
     *            flag for locking support
     */
    public AeAfLegacy(AeAfUIHandler aeAfUIHandler, Parameters cameraParameters,
            boolean shouldLock3A, AeAfCameraListener cameraListener) {
        super(aeAfUIHandler, shouldLock3A, cameraListener);

        focusRegionEnable = (cameraParameters.getMaxNumFocusAreas() > 0) ? true : false;
        meteringRegionEnable = (cameraParameters.getMaxNumMeteringAreas() > 0) ? true : false;
    }

    /** {@inheritDoc} */
    @Override
    public void setRegionAt(PointF touch, int viewWidth, int viewHeight, int angle) {
        aeAfUIHandler.autoFocusPosition(touch);

        MappedArea rotatedArea = getRotatedArea(touch, viewWidth, viewHeight, angle);
        focusRect = calcCameraArea(rotatedArea, FOCUS_RECT_MULTIPLIER * CAMERA_AREA_SIZE);
        meteringRect = calcCameraArea(rotatedArea, METERING_RECT_MULTIPLIER * CAMERA_AREA_SIZE);
    }

    private Rect calcCameraArea(MappedArea rotatedArea, float sensorRegionSize) {
        float scaleX = CAMERA_AREA_SIZE / rotatedArea.width;
        float scaleY = CAMERA_AREA_SIZE / rotatedArea.height;

        float sensorLeft = rotatedArea.x * scaleX;
        if (sensorLeft < 0) {
            sensorLeft = 0;
        } else if (sensorLeft + sensorRegionSize > CAMERA_AREA_SIZE) {
            sensorLeft = CAMERA_AREA_SIZE - sensorRegionSize;
        }
        sensorLeft -= CAMERA_AREA_SIZE / 2;

        float sensorTop = rotatedArea.y * scaleY;
        if (sensorTop < 0) {
            sensorTop = 0;
        } else if (sensorTop + sensorRegionSize > CAMERA_AREA_SIZE) {
            sensorTop = CAMERA_AREA_SIZE - sensorRegionSize;
        }
        sensorTop -= CAMERA_AREA_SIZE / 2;

        return new Rect((int) sensorLeft, (int) sensorTop, (int) (sensorLeft + sensorRegionSize),
                (int) (sensorTop + sensorRegionSize));
    }

    private void lockAndFocus(Camera camera) {
        if (type == AeAfType.LOCK_AND_FOCUS) {
            Log.d(TAG, "Same type:" + AeAfType.LOCK_AND_FOCUS);
            return;
        }
        Parameters cameraParameters = camera.getParameters();
        if (cameraParameters.isAutoExposureLockSupported()) {
            cameraParameters.setAutoExposureLock(true);
        }
        if (cameraParameters.isAutoWhiteBalanceLockSupported()) {
            cameraParameters.setAutoWhiteBalanceLock(true);
        }
        camera.setParameters(cameraParameters);

        if (type == AeAfType.CONTINUOUS) {
            Log.v(TAG, "Requesting autofocus in continuous autofocus mode to lock focus");
            camera.autoFocus(null);
        }
        type = AeAfType.LOCK_AND_FOCUS;
    }

    private void lockAndFocusRegion(Camera camera) {
        if (!meteringRegionEnable) {
            Log.d(TAG, "Focus on region not supported");
            return;
        }
        clearFocusExpiredTimer();
        camera.cancelAutoFocus();
        Parameters cameraParameters = camera.getParameters();
        cameraParameters.setFocusMode(Parameters.FOCUS_MODE_AUTO);

        List<Camera.Area> focusAreaList = new ArrayList<Camera.Area>();
        Camera.Area area = new Camera.Area(focusRect, AeAfHandler.TOUCH_FOCUS_AREA_WEIGHT);
        focusAreaList.add(area);
        cameraParameters.setFocusAreas(focusAreaList);

        List<Camera.Area> meteringAreaList = new ArrayList<Camera.Area>();
        area = new Camera.Area(meteringRect, AeAfHandler.TOUCH_FOCUS_AREA_WEIGHT);
        meteringAreaList.add(area);
        cameraParameters.setMeteringAreas(meteringAreaList);

        camera.setParameters(cameraParameters);
        camera.autoFocus(new AutoFocusCallback() {
            /** {@inheritDoc} */
            @Override
            public void onAutoFocus(boolean success, Camera camera) {
                if (success) {
                    aeAfUIHandler.autoFocusComplete(true);
                    startFocusExpiredTimer();

                } else {
                    Log.d(TAG, "Camera failed to focus");
                    aeAfUIHandler.autoFocusComplete(false);
                    if (cameraListener != null) {
                        cameraListener.onAutoFocusFailed();
                    }
                }
            }
        });

        aeAfUIHandler.autoFocusStarted();
        type = AeAfType.LOCK_AND_FOCUS_REGION;
    }

    private void applyContinuous(Camera camera) {
        if (type == AeAfType.CONTINUOUS) {
            Log.d(TAG, "Same type:" + AeAfType.CONTINUOUS);
            return;
        }
        camera.cancelAutoFocus();
        Parameters cameraParameters = camera.getParameters();
        if (cameraParameters.isAutoExposureLockSupported()) {
            cameraParameters.setAutoExposureLock(false);
        }
        if (cameraParameters.isAutoWhiteBalanceLockSupported()) {
            cameraParameters.setAutoWhiteBalanceLock(false);
        }

        cameraParameters.setFocusMode(Parameters.FOCUS_MODE_CONTINUOUS_PICTURE);
        camera.setParameters(cameraParameters);
        type = AeAfType.CONTINUOUS;
        Log.d(TAG, "Focus mode: continuous");
    }

    /**
     * Configure the ae af parameters
     *
     * @param camera
     *            camera reference
     * @param newType
     *            ae af type
     */
    public void configure(Camera camera, AeAfType newType) {
        if (newType == AeAfType.LOCK_AND_FOCUS) {
            lockAndFocus(camera);

        } else if (newType == AeAfType.LOCK_AND_FOCUS_REGION) {
            lockAndFocusRegion(camera);

        } else if (newType == AeAfType.CONTINUOUS) {
            applyContinuous(camera);
        }
    }
}
