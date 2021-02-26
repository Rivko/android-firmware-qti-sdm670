/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.panoramaui.hardware;

import android.annotation.TargetApi;
import android.graphics.PointF;
import android.graphics.Rect;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CaptureRequest;
import android.hardware.camera2.CaptureResult;
import android.hardware.camera2.TotalCaptureResult;
import android.hardware.camera2.params.MeteringRectangle;
import android.os.Build;
import android.util.Log;

import com.qualcomm.qti.panoramaui.AeAfUIHandler;

/**
 * class to manage AE AF settings for camera2
 */
@TargetApi(Build.VERSION_CODES.LOLLIPOP)
public class AeAfCamera2 extends AeAfHandler {
    private static final String TAG = AeAfCamera2.class.getSimpleName();

    private static final float AE_METERING_MULTIPLIER = 0.225f;// percentage
    private static final float AF_METERING_MULTIPLIER = 0.075f;// percentage

    private final Rect sensorRect;
    private final float aeMeteringSize;
    private final float afMeteringSize;
    private final boolean aeEnabled;
    private final boolean afEnabled;

    private Integer prevState = -1;
    private MeteringRectangle[] aeRegions = new MeteringRectangle[1];
    private MeteringRectangle[] afRegions = new MeteringRectangle[1];

    /**
     * Constructor for the class
     *
     * @param aeAfUIHandler
     *            Ae Af UI handler
     * @param characteristics
     *            camera characteristics
     * @param shouldLock3A
     *            flag for locking support
     */
    public AeAfCamera2(AeAfUIHandler aeAfUIHandler, CameraCharacteristics characteristics,
            boolean shouldLock3A, AeAfCameraListener cameraListener) {
        super(aeAfUIHandler, shouldLock3A, cameraListener);

        Integer maxRegionsAe = characteristics.get(CameraCharacteristics.CONTROL_MAX_REGIONS_AE);
        Integer maxRegionsAf = characteristics.get(CameraCharacteristics.CONTROL_MAX_REGIONS_AF);
        assert maxRegionsAe != null;
        assert maxRegionsAf != null;
        aeEnabled = (maxRegionsAe != 0);
        afEnabled = (maxRegionsAf != 0);

        sensorRect = characteristics.get(CameraCharacteristics.SENSOR_INFO_ACTIVE_ARRAY_SIZE);
        int minSize = Math.min(sensorRect.width(), sensorRect.height());
        aeMeteringSize = AE_METERING_MULTIPLIER * minSize;
        afMeteringSize = AF_METERING_MULTIPLIER * minSize;

        Log.d(TAG, "ae-size:" + aeMeteringSize + " af-size:" + afMeteringSize + " sensor pixels:"
                + sensorRect.right + " X " + sensorRect.bottom);
    }

    /** {@inheritDoc} */
    @Override
    public void setRegionAt(PointF touchPoint, int viewWidth, int viewHeight, int angle) {
        aeAfUIHandler.autoFocusPosition(touchPoint);

        MappedArea mappedArea = getRotatedArea(touchPoint, viewWidth, viewHeight, angle);
        aeRegions[0] = (aeEnabled) ? getFocusRectangle(mappedArea, aeMeteringSize)
                : new MeteringRectangle(0, 0, 0, 0, 0);
        afRegions[0] = (afEnabled) ? getFocusRectangle(mappedArea, afMeteringSize)
                : new MeteringRectangle(0, 0, 0, 0, 0);
    }

    private MeteringRectangle getFocusRectangle(MappedArea rotatedArea, float sensorRegionSize) {
        float scaleX = sensorRect.right / rotatedArea.width;
        float scaleY = sensorRect.bottom / rotatedArea.height;

        // Log.d(TAG, "rotated view " + (int) rotatedArea.width + " X " + (int) rotatedArea.height);

        float sensorLeft = rotatedArea.x * scaleX;
        if (sensorLeft < 0) {
            sensorLeft = 0;
        } else if (sensorLeft + sensorRegionSize > sensorRect.right) {
            sensorLeft = sensorRect.right - sensorRegionSize;
        }

        float sensorTop = rotatedArea.y * scaleY;
        if (sensorTop < 0) {
            sensorTop = 0;
        } else if (sensorTop + sensorRegionSize > sensorRect.bottom) {
            sensorTop = sensorRect.bottom - sensorRegionSize;
        }

        // Log.d(TAG, "pixel array " + (int) sensorLeft + "," + (int) sensorTop + ","
        // + (int) sensorRegionSize + "," + (int) sensorRegionSize);
        return new MeteringRectangle((int) sensorLeft, (int) sensorTop, (int) sensorRegionSize,
                (int) sensorRegionSize, AeAfHandler.TOUCH_FOCUS_AREA_WEIGHT);
    }

    private void lockAndFocus(CaptureRequest.Builder request) {
        if (type == AeAfType.LOCK_AND_FOCUS) {
            Log.d(TAG, "Same type:" + AeAfType.LOCK_AND_FOCUS);
            return;
        }
        request.set(CaptureRequest.CONTROL_AE_LOCK, true);
        request.set(CaptureRequest.CONTROL_AWB_LOCK, true);
        Log.i(TAG, "LOCK_AND_FOCUS: AE & AWB locked");

        Integer afMode = request.get(CaptureRequest.CONTROL_AF_MODE);
        if (afMode != null
                && (afMode == CaptureRequest.CONTROL_AF_MODE_CONTINUOUS_PICTURE || afMode == CaptureRequest.CONTROL_AF_MODE_CONTINUOUS_VIDEO)) {
            Log.v(TAG, "LOCK_AND_FOCUS: trigger autofocus");
            request.set(CaptureRequest.CONTROL_AF_TRIGGER, CaptureRequest.CONTROL_AF_TRIGGER_START);
        } else {
            Log.v(TAG, "LOCK_AND_FOCUS: no more autofocus");
        }
        type = AeAfType.LOCK_AND_FOCUS;
    }

    private void lockAndFocusRegion(CaptureRequest.Builder request) {
        if (!afEnabled) {
            Log.d(TAG, "Focus on region not supported");
            return;
        }
        clearFocusExpiredTimer();
        request.set(CaptureRequest.CONTROL_AE_LOCK, true);
        request.set(CaptureRequest.CONTROL_AWB_LOCK, true);

        request.set(CaptureRequest.CONTROL_AF_REGIONS, afRegions);
        Log.i(TAG, "LOCK_AND_FOCUS_REGION: AE & AWB unlocked, trigger auto focus on region");
        if (aeEnabled) {
            request.set(CaptureRequest.CONTROL_AE_REGIONS, aeRegions);
        }

        request.set(CaptureRequest.CONTROL_AF_MODE, CaptureRequest.CONTROL_AF_MODE_AUTO);
        request.set(CaptureRequest.CONTROL_AE_MODE, CaptureRequest.CONTROL_AE_MODE_ON_AUTO_FLASH);

        request.set(CaptureRequest.CONTROL_AF_TRIGGER, CaptureRequest.CONTROL_AF_TRIGGER_CANCEL);
        request.set(CaptureRequest.CONTROL_AF_TRIGGER, CaptureRequest.CONTROL_AF_TRIGGER_START);
        request.set(CaptureRequest.CONTROL_AE_PRECAPTURE_TRIGGER,
                CaptureRequest.CONTROL_AE_PRECAPTURE_TRIGGER_START);

        aeAfUIHandler.autoFocusStarted();
        type = AeAfType.LOCK_AND_FOCUS_REGION;
    }

    private void applyContinuous(CaptureRequest.Builder request) {
        if (type == AeAfType.CONTINUOUS) {
            Log.d(TAG, "Same type:" + AeAfType.CONTINUOUS);
            return;
        }
        request.set(CaptureRequest.CONTROL_AE_LOCK, false);
        request.set(CaptureRequest.CONTROL_AWB_LOCK, false);
        Log.i(TAG, "CONTINUOUS: AE & AWB unlocked, setting continuous picture mode");

        request.set(CaptureRequest.CONTROL_AF_MODE,
                CaptureRequest.CONTROL_AF_MODE_CONTINUOUS_PICTURE);
        request.set(CaptureRequest.CONTROL_AF_TRIGGER, CaptureRequest.CONTROL_AF_TRIGGER_CANCEL);
        type = AeAfType.CONTINUOUS;
    }

    /**
     * Configure the request with the current settings
     *
     * @param request
     *            configure request
     */
    public void configure(CaptureRequest.Builder request, AeAfType newType) {
        if (newType == AeAfType.LOCK_AND_FOCUS) {
            lockAndFocus(request);

        } else if (newType == AeAfType.LOCK_AND_FOCUS_REGION) {
            lockAndFocusRegion(request);

        } else if (newType == AeAfType.CONTINUOUS) {
            applyContinuous(request);
        }
    }

    /**
     * method to process the capture result
     *
     * @param result
     *            capture result
     */
    public void processResult(TotalCaptureResult result) {
        if (type == AeAfType.LOCK_AND_FOCUS_REGION) {
            Integer afState = result.get(CaptureResult.CONTROL_AF_STATE);
            if (prevState != afState) {
                switch (afState) {
                case CaptureRequest.CONTROL_AF_STATE_FOCUSED_LOCKED:
                    aeAfUIHandler.autoFocusComplete(true);
                    startFocusExpiredTimer();
                    Log.d(TAG, "id: " + result.getSequenceId() + ", AF state locked");
                    break;

                case CaptureResult.CONTROL_AF_STATE_NOT_FOCUSED_LOCKED:
                    aeAfUIHandler.autoFocusComplete(false);
                    Log.d(TAG, "id: " + result.getSequenceId() + ", AF failed, state " + afState);
                    if (cameraListener != null) {
                        cameraListener.onAutoFocusFailed();
                    }
                    break;

                default:
                    Log.d(TAG, "id: " + result.getSequenceId() + ", AF state: " + afState);
                    break;
                }
                prevState = afState;
            }
        }
    }
}
