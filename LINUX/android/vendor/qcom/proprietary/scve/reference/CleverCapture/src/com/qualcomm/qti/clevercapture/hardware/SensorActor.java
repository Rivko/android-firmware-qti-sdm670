/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.clevercapture.hardware;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.util.Log;
import android.view.OrientationEventListener;

/**
 * This class handles all the functionality related to camera as well as the
 * device tilt sensors.
 *
 */
public class SensorActor implements SensorEventListener {
    private static final String LOG_TAG = SensorActor.class.getSimpleName();

    private static final int DEVICE_MOVEMENT_THRESHHOLD = 2;

    private SensorManager sensorManager = null;
    private Sensor accelerometerSensor = null;
    private Sensor magneticSensor = null;
    private Sensor linearAccelerationSensor = null;

    private float[] gravityValues = null;
    private float[] magneticValues = null;
    float rotationMatrix[] = new float[9];
    float inclinationMatrix[] = new float[9];
    private Callbacks callbacks = null;
    private OrientationEventListener orientationSensor;

    private float mPrevTiltY = 0;

    /**
     * Interface for the callbacks from Camera View Actor
     */
    public interface Callbacks {
        /**
         * This callback gets fire on getting the tilt information from the
         * sensors
         *
         * @param tilt
         *            New changed/updated y tilt value
         */
        public void onDeviceTiltY(float tilt);

        /**
         * This callback gets fire on getting the tilt information from the
         * sensors
         *
         * @param tilt
         *            New changed/updated z tilt value
         */
        public void onDeviceTiltZ(float tilt);
    }

    /**
     * This method initializes the CamerView Actor Object
     *
     * @param context
     *            Context of the main activity
     * @param callback
     *            Callback object
     * @return true if initialized properly, false otherwise
     */
    public boolean initialize(Context context, Callbacks callback) {
        this.callbacks = callback;
        return initializeSensors(context);
    }

    /**
     * This method initializes the sensors
     *
     * @param context
     *            Main activity context
     * @return true if initialized properly, false otherwise
     */
    private boolean initializeSensors(Context context) {
        sensorManager = (SensorManager) context
                .getSystemService(Context.SENSOR_SERVICE);

        // getDefaultSensor function will return null if a sensor is not available
        accelerometerSensor = sensorManager
                .getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
        linearAccelerationSensor = sensorManager.getDefaultSensor(Sensor.TYPE_LINEAR_ACCELERATION);
        magneticSensor = sensorManager
                .getDefaultSensor(Sensor.TYPE_MAGNETIC_FIELD);
        orientationSensor = new OrientationEventListener(context) {
            @Override
            public void onOrientationChanged(int newOrientation) {
                callbacks.onDeviceTiltZ(newOrientation);
            }
        };

        return enableSensors(true);
    }

    /**
     * Register or unregister for device tilt listeners based on argument value
     *
     * @param enable
     *            true if application needs to register for tilt listener, false
     *            otherwise
     */
    private boolean enableSensors(boolean enable) {
        if (sensorManager == null) {
            return false;
        }
        if (enable) {
            if (accelerometerSensor != null ) {
                if (!sensorManager.registerListener(this, accelerometerSensor,
                        SensorManager.SENSOR_DELAY_UI)) {
                    Log.d(LOG_TAG, "Accelerometer Sensor failed");
                    return false;
                }
            }
            if (linearAccelerationSensor != null) {
                if (!sensorManager.registerListener(this, linearAccelerationSensor,
                        SensorManager.SENSOR_DELAY_UI)) {
                    Log.d(LOG_TAG, "Liean Acceleration Sensor failed");
                    return false;
                }
            }
            if (magneticSensor != null ) {
                if (!sensorManager.registerListener(this, magneticSensor,
                        SensorManager.SENSOR_DELAY_UI)) {
                    Log.d(LOG_TAG, "Magnetic Sensor failed");
                    return false;
                }
            }
            if (orientationSensor.canDetectOrientation()) {
                // Log.d(LOG_TAG, "Enabling Orientation detection");
                orientationSensor.enable();
            } else {
                Log.d(LOG_TAG, "Not able to detect Orientation");
                return false;
            }
        } else {
            if (accelerometerSensor != null) {
                // AccelerometerSensor has successfully registered
                sensorManager.unregisterListener(this, accelerometerSensor);
                accelerometerSensor = null;
            }
            if (linearAccelerationSensor != null) {
                // AccelerometerSensor has successfully registered
                sensorManager.unregisterListener(this, linearAccelerationSensor);
                linearAccelerationSensor = null;
            }
            if (magneticSensor != null) {
                // MegneticSensor has successfully registered
                sensorManager.unregisterListener(this, magneticSensor);
                magneticSensor = null;
            }
            orientationSensor.disable();
        }
        return true;
    }

    /**
     * clean up method for camera view actor
     */
    public void wrapUp() {
        Log.d(LOG_TAG, "Sensor Actor Wrapped");
        enableSensors(false);
        sensorManager = null;
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
    public void onSensorChanged(SensorEvent event) {

        float[] passedInEvent = event.values.clone();
        double sumOfSquares = 0.0d;
        double acceleration = 0.0d;

        /*
         * It is recommended to use software Sensor over hardward Sensor.
         * Sensor.TYPE_LINEAR_ACCELERATION isolates acceleration data by pass accelerometer
         * data through a high-pass filter
         * From book: Make sense of Android sensors
         * https://docs.google.com/presentation/d/1mGzkcZF7C4QyemRvbAizraPjjqabgulIv7X04ABkEuU/edit?pli=1#slide=id.g4571614f_1_15
         */
        if (event.sensor.getType() == Sensor.TYPE_LINEAR_ACCELERATION) {
            passedInEvent = highPass(passedInEvent[0], passedInEvent[1], passedInEvent[2]);

            sumOfSquares = (passedInEvent[0] * passedInEvent[0])
                    + (passedInEvent[1] * passedInEvent[1])
                    + (passedInEvent[2] * passedInEvent[2]);
            acceleration = Math.sqrt(sumOfSquares);
        }

        if (event.sensor.getType() == Sensor.TYPE_ACCELEROMETER) {
            gravityValues = event.values;
        }
        if (event.sensor.getType() == Sensor.TYPE_MAGNETIC_FIELD) {
            magneticValues = event.values;
        }

        if (gravityValues != null && magneticValues != null && acceleration != 0.0d) {
            boolean success = SensorManager.getRotationMatrix(rotationMatrix,
                    inclinationMatrix, gravityValues, magneticValues);
            if (success && acceleration >= DEVICE_MOVEMENT_THRESHHOLD ) {
                int yTilt = (int) Math.round(Math.toDegrees(Math
                        .asin(rotationMatrix[8])));

                if (mPrevTiltY != yTilt) {
                    mPrevTiltY = yTilt;
                    callbacks.onDeviceTiltY(yTilt);
                }
            } else {
                Log.d(LOG_TAG, "Get Rotation failed");
            }
        }
    }

    private float[] highPass(float x, float y, float z) {

        final float ALPHA = 0.8f;
        float[] gravity = {0,0,0};

        float[] filteredValues = new float[3];

        gravity[0] = ALPHA * gravity[0] + (1 - ALPHA) * x;
        gravity[1] = ALPHA * gravity[1] + (1 - ALPHA) * y;
        gravity[2] = ALPHA * gravity[2] + (1 - ALPHA) * z;

        filteredValues[0] = x - gravity[0];
        filteredValues[1] = y - gravity[1];
        filteredValues[2] = z - gravity[2];

        return filteredValues;
    }
}
