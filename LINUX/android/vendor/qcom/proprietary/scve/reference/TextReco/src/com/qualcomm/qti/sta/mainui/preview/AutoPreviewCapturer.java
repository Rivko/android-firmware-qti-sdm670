/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.sta.mainui.preview;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Handler;
import android.util.Log;

import com.qualcomm.qti.sta.device.CameraHandler;
import com.qualcomm.qti.sta.device.CameraHandler.CameraListener;
import com.qualcomm.qti.sta.mainui.ImageCapturer;
import com.qualcomm.qti.sta.mainui.CameraActivity.CaptureType;
import com.qualcomm.qti.sta.settings.PreferenceData;

/**
 * Class for capturing preview frames using auto capturing method
 */
public class AutoPreviewCapturer extends ImageCapturer implements SensorEventListener {
    private static final String LOG_TAG = "AutoPreviewCapturer";

    private enum State {
        UNKNOWN, SENSOR_ROI, CAPTURE;
    }

    private enum SensorState {
        DISABLED, ENABLED_NOT_INITIALIZED, INITIALIZED;
    }

    private State state = State.UNKNOWN;

    // sensor parameters
    private Handler captureHandler = new Handler();
    private final long steadyDuration;// ms
    private final float moveThreshold;
    private final SensorManager sensorManager;
    private final Sensor accelerometerSensor;
    private SensorState sensorState = SensorState.DISABLED;
    private float previousX = 0;
    private float previousY = 0;
    private float previousZ = 0;

    final Runnable previewReadyRunnable = new Runnable() {
        /**
         * {@inheritDoc}
         */
        @Override
        public void run() {
            loadCaptureState();
        }
    };

    /**
     * Constructor for the class
     *
     * @param context
     *            application context
     * @param cameraListener
     *            camera listener
     */
    public AutoPreviewCapturer(Context context, CameraListener cameraListener) {
        super(CaptureType.AUTO_PREVIEW, context, cameraListener);

        moveThreshold = PreferenceData.getCameraMoveThreshold(context);
        steadyDuration = PreferenceData.getCameraSteadyDuration(context);

        sensorManager = (SensorManager) context.getSystemService(Context.SENSOR_SERVICE);
        accelerometerSensor = sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);

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
        loadSensorRoiState();
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

        captureHandler.removeCallbacksAndMessages(null);
        captureStatus = false;
        enableSensor(false);
        state = State.UNKNOWN;
    }

    private void loadSensorRoiState() {
        if (state == State.SENSOR_ROI) {
            Log.d(LOG_TAG, "State rejected: already on " + state);
            return;
        }
        Log.d(LOG_TAG, "State changed: " + state + " => " + State.SENSOR_ROI);
        state = State.SENSOR_ROI;
        enableSensor(true);

        captureHandler.postDelayed(previewReadyRunnable, steadyDuration);
    }

    private void loadCaptureState() {
        if (state == State.CAPTURE) {
            Log.d(LOG_TAG, "State rejected: already on " + state);
            return;
        }
        Log.d(LOG_TAG, "State changed: " + state + " => " + State.CAPTURE);
        state = State.CAPTURE;
        enableSensor(false);

        if (cameraListener != null) {
            cameraListener.onFrameSessionStarted();
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void wrapUp() {
        stop();
        Log.d(LOG_TAG, "Image capturer wrapped");
    }

    private boolean enableSensor(boolean enable) {
        if (sensorManager == null) {
            Log.d(LOG_TAG, "Sensor manager null");
            return false;
        }
        if (enable) {
            if (sensorState == SensorState.DISABLED) {
                if (!sensorManager.registerListener(this, accelerometerSensor,
                        SensorManager.SENSOR_DELAY_UI)) {
                    Log.d(LOG_TAG, "Accelerometer Sensor failed");
                    return false;
                }
                sensorState = SensorState.ENABLED_NOT_INITIALIZED;
                Log.i(LOG_TAG, "Accelerometer initialized");
            }

        } else if (!enable) {
            if (sensorState != SensorState.DISABLED) {
                sensorManager.unregisterListener(this, accelerometerSensor);
                sensorState = SensorState.DISABLED;
                Log.d(LOG_TAG, "Accelerometer released");
            }
        }
        return true;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onSensorChanged(SensorEvent event) {
        // this check is for the amazon fire device which sometimes inspite of
        // unregister call does not un registers the sensors.
        if (sensorState == SensorState.DISABLED) {
            Log.w(LOG_TAG, "Sensors disabled but still getting events");
            return;
        } else if (sensorState == SensorState.ENABLED_NOT_INITIALIZED) {
            previousX = event.values[0];
            previousY = event.values[1];
            previousZ = event.values[2];
            sensorState = SensorState.INITIALIZED;
            return;
        }

        float xDistance = Math.abs(previousX - event.values[0]);
        float yDistance = Math.abs(previousY - event.values[1]);
        float zDistance = Math.abs(previousZ - event.values[2]);

        if (xDistance > moveThreshold || yDistance > moveThreshold || zDistance > moveThreshold) {
            Log.d(LOG_TAG, "device moved " + xDistance + "," + yDistance + "," + zDistance);
            captureHandler.removeCallbacksAndMessages(null);
            captureHandler.postDelayed(previewReadyRunnable, steadyDuration);
        }
        previousX = event.values[0];
        previousY = event.values[1];
        previousZ = event.values[2];
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onRoiChanged(CameraHandler cameraHandler) {
        if (state == State.SENSOR_ROI) {
            // Log.d(LOG_TAG, "ROI changed");
            captureHandler.removeCallbacksAndMessages(null);
            captureHandler.postDelayed(previewReadyRunnable, steadyDuration);

        } else if (state == State.CAPTURE) {
            stop();
            if (cameraListener != null) {
                cameraListener.onFrameSessionCancelled();
            }
            start(cameraHandler);
        }
    }
}
