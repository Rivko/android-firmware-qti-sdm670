/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.panoramaui.hardware;

import java.util.Arrays;
import java.util.HashSet;

import com.qualcomm.qti.panoramaui.utils.IirFilter;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.util.Log;
import android.view.Surface;
import android.view.WindowManager;

/**
 * This class interfaces with the device sensors and provides processed acceleration and rotation
 * data. The coordinate system used for all values returned by {@link SensorActor}:<br>
 * With the device held upright, X points out the right side of the device, Y points out the top
 * of the device, and Z points out the screen towards the user. The Z axis points in an arbitrary
 * (but consistent) direction; it may not necessarily be magnetic north.
 */
public class SensorActor {
    /**
     * Classes may implement this interface to receive acceleration updates
     */
    public static interface AccelerationListener {
        void onAccelerationUpdate(float x, float y, float z);
    }

    /**
     * Classes may implement this interface to receive rotation updates
     */
    public static interface RotationListener {
        void onRotationUpdate(float x, float y, float z);
    }

    private static final String TAG = SensorActor.class.getSimpleName();

    // Note: Rotation quaternion is stored as [(x, y, z), w]
    private static final float[] ROTATE_AROUND_X_BY_MINUS_PI_2 = new float[4];
    static {
        axisAngleToQuaternion(new float[] { 1f, 0f, 0f }, (float)(-Math.PI / 2d),
                ROTATE_AROUND_X_BY_MINUS_PI_2);
    }

    private final HashSet<AccelerationListener> accelerationListeners =
            new HashSet<AccelerationListener>();
    private final HashSet<RotationListener> rotationListeners = new HashSet<RotationListener>();
    private final float[] lastAcceleration = new float[3];
    private final float[] lastRotation = new float[3];
    private final float[] referenceFrameQuat = new float[4];
    private final float[] intermediateQuat = new float[4]; // Used for per-event calculations
    private final float[] orientationQuat = new float[4]; // Used for per-event calculations
    private final IirFilter[] accelerationFilter;
    private final IirFilter[] rotationFilter;
    private final SensorManager sensorManager;
    private final Sensor accelerationSensor;
    private final Sensor rotationSensor;
    private boolean running;

    /**
     * Construct a new {@link SensorActor}
     * @param context the application {@link Context}
     */
    public SensorActor(Context context) {
        sensorManager = (SensorManager)context.getSystemService(Context.SENSOR_SERVICE);

        accelerationSensor = sensorManager.getDefaultSensor(Sensor.TYPE_LINEAR_ACCELERATION);
        if (accelerationSensor == null) {
            Log.w(TAG, "No linear acceleration sensor, position info will not be available");
        }

        Sensor rotationSensor;
        rotationSensor = sensorManager.getDefaultSensor(Sensor.TYPE_ROTATION_VECTOR);
        if (rotationSensor == null) {
            rotationSensor = sensorManager.getDefaultSensor(Sensor.TYPE_GAME_ROTATION_VECTOR);
        }
        if (rotationSensor == null) {
            rotationSensor = sensorManager.getDefaultSensor(
                    Sensor.TYPE_GEOMAGNETIC_ROTATION_VECTOR);
        }
        this.rotationSensor = rotationSensor;
        if (this.rotationSensor == null) {
            Log.w(TAG, "No rotation sensor, rotation info will not be available");
        }

        // Filtering of accelerometer signals with:
        // Chebyshev type 2 lowpass filter, 4th order, cutoff freq=0.5*(Fs/2),
        // stopband attenuation=-30dB
        final float[] yA = new float[] { 0.0995903507443931f, 0.1640311659319415f,
                0.2226137251933492f, 0.1640311659319415f, 0.0995903507443931f };
        final float[] xA = new float[] { 1.0000000000000000f, -0.9055263176087963f,
                0.8127598454006713f, -0.2015798505409148f, 0.0442030812950586f };

        accelerationFilter = new IirFilter[3];
        for (int i = 0; i < accelerationFilter.length; i++) {
            accelerationFilter[i] = new IirFilter(yA, xA);
        }

        // If no gyroscope present, magnetometer is the only source of heading, and its stability
        // is not particularly high. Add some filtering to rotation values (especially Y).
        if (rotationSensor != null &&
                rotationSensor.getType() == Sensor.TYPE_GEOMAGNETIC_ROTATION_VECTOR) {
            rotationFilter = new IirFilter[3];

            // Filter rotation around Y with:
            // Bessel lowpass filter, 3rd order, cutoff freq=0.18*(Fs/2)
            final float[] yRY = new float[] { 0.0126435374254435f, 0.0379306122763306f,
                    0.0379306122763306f, 0.0126435374254435f };
            final float[] xRY = new float[] { 1.0000000000000000f, -1.7659624819702828f,
                    1.1129591126475655f, -0.2458483312737345f };
            rotationFilter[1] = new IirFilter(yRY, xRY);

            // Filter rotation around X/Z with:
            // Bessel lowpass filter, 1st order, cutoff freq=0.5*(Fs/2)
            final float[] yR = new float[] { 0.5f, 0.5f };
            final float[] xR = new float[] { 1f, 0f };
            rotationFilter[0] = new IirFilter(yR, xR);
            rotationFilter[2] = new IirFilter(yR, xR);
        } else {
            rotationFilter = null;
        }

        // Calculate a quaternion to adjust the rotation vector for the device orientation
        int displayRotation = ((WindowManager)context.getSystemService(Context.WINDOW_SERVICE))
                .getDefaultDisplay().getRotation();
        float rotationZ;
        switch (displayRotation) {
        case Surface.ROTATION_90:
            rotationZ = (float)(-Math.PI / 2d);
            break;

        case Surface.ROTATION_180:
            rotationZ = (float)(-Math.PI);
            break;

        case Surface.ROTATION_270:
            rotationZ = (float)(-3d * Math.PI / 2d);
            break;

        case Surface.ROTATION_0:
        default:
            rotationZ = 0;
            break;
        }
        axisAngleToQuaternion(new float[] { 0f, 0f, 1f }, rotationZ, referenceFrameQuat);
    }

    /**
     * Test if the device has the required sensors for full {@link SensorActor} functionality
     * @return true if all required sensors are present, else false
     */
    public boolean hasRequiredSensors() {
        return (accelerationSensor != null && rotationSensor != null);
    }

    /**
     * Start this instance (if not already running). When started, registered listeners will begin
     * receiving updates.
     */
    public void start() {
        if (running) {
            Log.w(TAG, "SensorActor is already running");
            return;
        }

        running = true;
        Arrays.fill(lastAcceleration, 0f);
        Arrays.fill(lastRotation, 0f);
        accelerationFilter[0].resetWithInput(0f);
        accelerationFilter[1].resetWithInput(0f);
        accelerationFilter[2].resetWithInput(0f);
        if (rotationFilter != null) {
            rotationFilter[0].resetWithInput(0f);
            rotationFilter[1].resetWithInput(0f);
            rotationFilter[2].resetWithInput(0f);
        }

        if (accelerationSensor != null) {
            sensorManager.registerListener(listener, accelerationSensor,
                    SensorManager.SENSOR_DELAY_GAME);
        }

        if (rotationSensor != null) {
            sensorManager.registerListener(listener, rotationSensor,
                    SensorManager.SENSOR_DELAY_GAME);
        } else {
            Log.w(TAG, "No rotation sensor, rotation info will not be available");
        }
    }

    /**
     * Stop this instance (if running). After stop() is invoked, registered listeners will not
     * receive any more updates.
     */
    public void stop() {
        if (!running) {
            Log.w(TAG, "SensorActor is not running");
            return;
        }

        running = false;

        sensorManager.unregisterListener(listener);
    }

    /**
     * Register an {@link AccelerationListener}
     * @param listener {@link AccelerationListener} to register
     */
    public void registerAccelerationListener(AccelerationListener listener) {
        if (!accelerationListeners.contains(listener)) {
            accelerationListeners.add(listener);
        }
    }

    /**
     * Unregister an {@link AccelerationListener}
     * @param listener Previously registered {@link AccelerationListener} to unregister
     */
    public void unregisterAccelerationListener(AccelerationListener listener) {
        accelerationListeners.remove(listener);
    }

    /**
     * Register a {@link RotationListener}
     * @param listener {@link RotationListener} to register
     */
    public void registerRotationListener(RotationListener listener) {
        if (!rotationListeners.contains(listener)) {
            rotationListeners.add(listener);
        }
    }

    /**
     * Unregister a {@link RotationListener}
     * @param listener Previously registered {@link RotationListener} to unregister
     */
    public void unregisterRotationListener(RotationListener listener) {
        rotationListeners.remove(listener);
    }

    /**
     * Get the last reported acceleration along the X axis.<br><i>Note: this value is only valid
     * if the {@link SensorActor} is {@link #start()}ed.</i>
     * @return the last reported acceleration along the X axis
     */
    public float getAccelerationX() {
        return lastAcceleration[0];
    }

    /**
     * Get the last reported acceleration along the Y axis.<br><i>Note: this value is only valid
     * if the {@link SensorActor} is {@link #start()}ed.</i>
     * @return the last reported acceleration along the Y axis
     */
    public float getAccelerationY() {
        return lastAcceleration[1];
    }

    /**
     * Get the last reported acceleration along the Z axis.<br><i>Note: this value is only valid
     * if the {@link SensorActor} is {@link #start()}ed.</i>
     * @return the last reported acceleration along the Z axis
     */
    public float getAccelerationZ() {
        return lastAcceleration[2];
    }

    /**
     * Get the last reported acceleration values.<br><i>Note: these values are only valid if the
     * {@link SensorActor} is {@link #start()}ed</i>
     * @param arr if not null and arr.length >= 3, this array will be populated with the last
     *      reported acceleration values and returned. Otherwise, a new float[] will be allocated
     *      and returned.
     * @return a float[] containing the last reported acceleration values
     */
    public float[] getAcceleration(float[] arr) {
        if (arr == null || arr.length < lastAcceleration.length) {
            arr = new float[lastAcceleration.length];
        }

        System.arraycopy(lastAcceleration, 0, arr, 0, lastAcceleration.length);
        return arr;
    }

    /**
     * Get the last reported rotation around the X axis.<br><i>Note: this value is only valid
     * if the {@link SensorActor} is {@link #start()}ed.</i>
     * @return the last reported rotation around the X axis
     */
    public float getRotationX() {
        return lastRotation[0];
    }

    /**
     * Get the last reported rotation around the Y axis.<br><i>Note: this value is only valid
     * if the {@link SensorActor} is {@link #start()}ed.</i>
     * @return the last reported rotation around the Y axis
     */
    public float getRotationY() {
        return lastRotation[1];
    }

    /**
     * Get the last reported rotation around the Z axis.<br><i>Note: this value is only valid
     * if the {@link SensorActor} is {@link #start()}ed.</i>
     * @return the last reported rotation around the Z axis
     */
    public float getRotationZ() {
        return lastRotation[2];
    }

    /**
     * Get the last reported rotation values.<br><i>Note: these values are only valid if the
     * {@link SensorActor} is {@link #start()}ed</i>
     * @param arr if not null and arr.length >= 3, this array will be populated with the last
     *      reported rotation values and returned. Otherwise, a new float[] will be allocated
     *      and returned.
     * @return a float[] containing the last reported rotation values
     */
    public float[] getRotation(float[] arr) {
        if (arr == null || arr.length < lastRotation.length) {
            arr = new float[lastRotation.length];
        }

        System.arraycopy(lastRotation, 0, arr, 0, lastRotation.length);
        return arr;
    }

    private static void axisAngleToQuaternion(float[] axis, float angle, float[] q) {
        // Quaternion [(x, y, z), w] = [(axis(X)/||axis||*sin(angle/2),
        // axis(Y)/||axis||*sin(angle/2), axis(Z)/||axis||*sin(angle/2)), cos(angle/2)]
        float norm = (float)Math.sqrt(axis[0] * axis[0] + axis[1] * axis[1] + axis[2] * axis[2]);
        float sinAngle = (float)Math.sin(angle / 2f);
        q[0] = axis[0] / norm * sinAngle;
        q[1] = axis[1] / norm * sinAngle;
        q[2] = axis[2] / norm * sinAngle;
        q[3] = (float)Math.cos(angle / 2f);
    }

    private static void hamiltonProduct(float[] q1, float[] q2, float[] q) {
        // [(x1, y1, z1), w1] * [(x2, y2, z2), w2] = [(x1, y1, z1)X(x2, y2, z2) +
        // w1*(x2, y2, z2) + w2*(x1, y1, z1), w1*w2 - (x1, y1, z1).(x2, y2, z2)
        q[3] = q1[3] * q2[3] - q1[0] * q2[0] - q1[1] * q2[1] - q1[2] * q2[2];
        q[0] = q1[3] * q2[0] + q1[0] * q2[3] + q1[1] * q2[2] - q1[2] * q2[1];
        q[1] = q1[3] * q2[1] - q1[0] * q2[2] + q1[1] * q2[3] + q1[2] * q2[0];
        q[2] = q1[3] * q2[2] + q1[0] * q2[1] - q1[1] * q2[0] + q1[2] * q2[3];
    }

    private final SensorEventListener listener = new SensorEventListener() {
        @Override
        public void onSensorChanged(SensorEvent event) {
            if (!running) {
                return;
            }

            if (event.sensor == accelerationSensor) {
                lastAcceleration[0] = accelerationFilter[0].add(event.values[0]);
                lastAcceleration[1] = accelerationFilter[1].add(event.values[1]);
                lastAcceleration[2] = accelerationFilter[2].add(event.values[2]);

                // N.B. Take a snapshot of listeners, in case one of the callbacks registers or
                // unregisters a listener and modifies the set
                AccelerationListener[] snapshot = accelerationListeners.toArray(
                        new AccelerationListener[accelerationListeners.size()]);
                for (AccelerationListener l : snapshot) {
                    l.onAccelerationUpdate(lastAcceleration[0], lastAcceleration[1],
                            lastAcceleration[2]);
                }
            } else if (event.sensor == rotationSensor) {
                // First, rotate the reference frame to adjust for device orientation. Then, apply
                // the current event rotation. Finally, rotate by -PI/2 around the X axis, to match
                // the upright device coordinate space that represents the normal case for this app.
                hamiltonProduct(event.values, referenceFrameQuat, intermediateQuat);
                hamiltonProduct(ROTATE_AROUND_X_BY_MINUS_PI_2, intermediateQuat, orientationQuat);

                // Calculate the independent axis Euler angle (see the 1977 NASA "Euler Angles,
                // Quaternions, and Transformation Matrics" paper, appendix A, for the derivation
                // of these equations)
                final float m21 = 2 * (orientationQuat[0] * orientationQuat[1] +
                        orientationQuat[3] * orientationQuat[2]);
                final float m23 = 2 * (-orientationQuat[0] * orientationQuat[3] +
                        orientationQuat[1] * orientationQuat[2]);
                final float m31 = 2 * (orientationQuat[0] * orientationQuat[2] -
                        orientationQuat[3] * orientationQuat[1]);

                lastRotation[0] = (float)Math.toDegrees(
                        Math.atan2(-m23, Math.sqrt(1 - m23 * m23))); // Euler YXZ
                lastRotation[1] = (float)Math.toDegrees(
                        Math.atan2(-m31, Math.sqrt(1 - m31 * m31))); // Euler ZYX
                lastRotation[2] = (float)(float)Math.toDegrees(
                        Math.atan2(m21, Math.sqrt(1 - m21 * m21))); // Euler YZX

                if (rotationFilter != null) {
                    lastRotation[0] = rotationFilter[0].add(lastRotation[0]);
                    lastRotation[1] = rotationFilter[1].add(lastRotation[1]);
                    lastRotation[2] = rotationFilter[2].add(lastRotation[2]);
                }

                // N.B. Take a snapshot of listeners, in case one of the callbacks registers or
                // unregisters a listener and modifies the set
                RotationListener[] snapshot = rotationListeners.toArray(
                        new RotationListener[rotationListeners.size()]);
                for (RotationListener l : snapshot) {
                    l.onRotationUpdate(lastRotation[0], lastRotation[1], lastRotation[2]);
                }
            }
        }

        @Override
        public void onAccuracyChanged(Sensor sensor, int accuracy) {
        }
    };
}
