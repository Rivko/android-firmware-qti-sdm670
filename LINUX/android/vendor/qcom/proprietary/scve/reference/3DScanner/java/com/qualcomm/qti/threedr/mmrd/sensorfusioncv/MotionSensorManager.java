/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.threedr.mmrd.sensorfusioncv;

import android.hardware.SensorManager;
import android.util.Log;

import com.qualcomm.qti.threedr.CannedDataTest.ReadCannedIMUData;
import com.qualcomm.qti.threedr.mmrd.orientationProvider.CalibratedGyroscopeProvider;
import com.qualcomm.qti.threedr.mmrd.speedProvider.LinearAccelerometerSpeedProvider;
import com.qualcomm.qti.threedr.mmrd.speedProvider.SpeedProvider;

import java.util.ArrayList;
import java.util.Timer;
import java.util.TimerTask;

public class MotionSensorManager {
    static {
        System.loadLibrary("3DR");
        System.loadLibrary("3DRgl2jni");
    }

    private String TAG = "Scan3D";

    private final double captureFPS = 50;      // unit 1/s
    private final long MaxTimediff = 5000000; // unit nano second;
    private boolean mRunning;
    private boolean isDataAvailable;
    private boolean isMPU6050;
    private boolean isBMI160;
    private boolean isLSM6DS3;
    private SensorManager mSensorManager;
    private CalibratedGyroscopeProvider mCalibratedGyroscopeProvider[];
    private SpeedProvider mSpeedProviders[];
    private ArrayList<MotionData> acceData;
    private ArrayList<MotionData> gyroData;
    private boolean imuAligned;
    //    
    //
    private long mPrevTimeStamp = 0;
    private long mCurrTimeStamp = 0;
    private double mAvgDeltaTime = 0;
    private double mAvgDeltaCnt = 0;

    // timer classs
    private Timer mTimer;
    private MotionTimerTask mTimerTask;
    private CannedMotionTimerTask mCannedTimerTask;
    private boolean live_tracking = true;//needs to be true by default

    public MotionSensorManager(SensorManager sensorManager) {
        mSensorManager = sensorManager;
        mCalibratedGyroscopeProvider = new CalibratedGyroscopeProvider[1];
        mSpeedProviders = new SpeedProvider[1];

        mRunning = false;

        // init timer
        mAvgDeltaTime = 0;
        mAvgDeltaCnt = 0;

        // init buffer        //
        gyroData = new ArrayList<MotionData>();
        acceData = new ArrayList<MotionData>();
    }


    public MotionSensorManager() {
        //for canned data
        live_tracking = false;
    }

    public void start() {
        if (mRunning)
            return;

        if (live_tracking) {
            startRealIMUData();
        } else {
            startCannedIMUData();
        }

        mRunning = true;
        isDataAvailable = false;
        mTimer = new Timer();

        if (live_tracking) {
            mTimerTask = new MotionTimerTask();
            mTimer.scheduleAtFixedRate(mTimerTask, (long) (1000.f / captureFPS), (long) (1000.f / captureFPS)); // unit ms
        } else {
            mCannedTimerTask = new CannedMotionTimerTask();
            mTimer.scheduleAtFixedRate(mCannedTimerTask, (long) (1000.f / captureFPS), (long) (1000.f / captureFPS)); // unit ms
        }
    }

    private void startRealIMUData() {
        // init gyro sensor
        mCalibratedGyroscopeProvider[0] = new CalibratedGyroscopeProvider(mSensorManager);
        for (CalibratedGyroscopeProvider provider : mCalibratedGyroscopeProvider) {
            provider.start();
            Log.i(TAG, "start gyro");
        }
        // init accelerate meter sensor
        mSpeedProviders[0] = new LinearAccelerometerSpeedProvider(mSensorManager);
        for (SpeedProvider provider : mSpeedProviders) {
            provider.start();
            Log.i(TAG, "start accelerometer");
        }

        isMPU6050 = mCalibratedGyroscopeProvider[0].getSensorName().equals("MPU6050");
        Log.i(TAG, "is MPU6050 = " + isMPU6050 + " " + mCalibratedGyroscopeProvider[0].getSensorName());

        isBMI160 = mCalibratedGyroscopeProvider[0].getSensorName().equals("BMI160 Gyroscope") || mCalibratedGyroscopeProvider[0].getSensorName().equals("BMI160 gyroscope");
        Log.i(TAG, "is BMI160 = " + isBMI160 + " " + mCalibratedGyroscopeProvider[0].getSensorName());

        isLSM6DS3 = mCalibratedGyroscopeProvider[0].getSensorName().equals("LSM6DS3 Gyroscope");
        Log.i(TAG, "is LSM6DS3 = " + isLSM6DS3 + " " + mCalibratedGyroscopeProvider[0].getSensorName());
    }

    private void startCannedIMUData() {
        ReadCannedIMUData.initCannedIMUData();
    }


    public void stop() {
        if (!mRunning)
            return;

        mTimer.cancel();
        mTimer = null;
        mTimerTask = null;
        mCannedTimerTask = null;
        if (live_tracking) {
            stopRealSensor();
        } else {
            //for canned IMU data do nothing
            stopCannedIMUData();
        }
        mRunning = false;
        Log.i(TAG, "MotionSensorManager.java stop() completed");
    }

    private void stopRealSensor() {
        // stop the sensor
        Log.i(TAG, "Before stop gyro");
        for (CalibratedGyroscopeProvider provider : mCalibratedGyroscopeProvider) {
            provider.stop();
        }
        Log.i(TAG, "Before stop accelerometer");
        for (SpeedProvider provider : mSpeedProviders) {
            provider.stop();
        }

        // release memory
        mCalibratedGyroscopeProvider[0] = null;
        mSpeedProviders[0] = null;
        acceData = null;
        gyroData = null;
    }

    private void stopCannedIMUData() {
        ReadCannedIMUData.deInitCannedIMUReader();
    }


    //this method is not really used
    public boolean checkDataAvailable() {

        int i = 0;

        while (i < 10) {
            if (isDataAvailable)
                break;
            else {
                try {
                    Thread.sleep(500);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
            i++;
        }

        return isDataAvailable;
    }

    public boolean checkDataAligned() {

        int i = 0;

        while (i < 10) {
            if (imuAligned)
                break;
            else {
                try {
                    Thread.sleep(100);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
            i++;
        }

        return imuAligned;
    }

    // private functions
    private class MotionTimerTask extends TimerTask {
        @Override
        public void run() {
            // collect the sensor data
            mCalibratedGyroscopeProvider[0].flushReadings(gyroData);
            mSpeedProviders[0].flushReadings(acceData);

            int size = 0;
            // check if we are using MPU 6050
            if (isMPU6050 || isBMI160 ||isLSM6DS3) {
                // sanity check, make sure we have aligned data
                if (!alignSensors()) {
                    //Log.i(TAG,"MOTION SENSOR NOT ALIGNED");
                    imuAligned = false;
                    return;
                }

                //Log.i(TAG,"MOTION SENSOR ALIGNED");
                imuAligned = true;
                size = Math.min(gyroData.size(), acceData.size());
            } else {
                final int validSize[] = new int[1];
                if (!alignSensors_v2(validSize)) {
                    //Log.i(TAG,"MOTION SENSOR NOT ALIGNED");
                    imuAligned = false;
                    return;
                } else {
                    //Log.i(TAG,"MOTION SENSOR ALIGNED");
                    imuAligned = true;
                    size = validSize[0];
                }
            }

            if (!isDataAvailable)
                isDataAvailable = true;

            if (size > 0) {
                float gyroArray[] = new float[size * 3];
                float accArray[] = new float[size * 3];
                long accTimestampArray[] = new long[size];


                // send it to JNI layer
                for (int i = 0; i < size; i++) {
                    gyroArray[i * 3] = gyroData.get(i).v[0];
                    gyroArray[i * 3 + 1] = gyroData.get(i).v[1];
                    gyroArray[i * 3 + 2] = gyroData.get(i).v[2];
                    //
                    accArray[i * 3] = acceData.get(i).v[0];
                    accArray[i * 3 + 1] = acceData.get(i).v[1];
                    accArray[i * 3 + 2] = acceData.get(i).v[2];
                    //
                    accTimestampArray[i] = acceData.get(i).t / 1000; // always using timestamp from the accelerometer
                }

                // asynchronous reading
                //Log.i(TAG,"before call JNI get sensor");
                getSensorReading(gyroArray,
                        accArray,
                        accTimestampArray,
                        mSpeedProviders[0].getDifUnixTimestamp(),
                        size);

                // erase buffer
                if (gyroData != null)
                    gyroData.subList(0, size).clear();
                if (acceData != null)
                    acceData.subList(0, size).clear();
            }

        }

        private boolean alignSensors() {
            boolean isAlign = false;
            do {
                if (gyroData.size() == 0 || acceData.size() == 0) {
                    //Log.i(TAG,"no motion data available");
                    return false;
                }

                // check if the first data is aligned or not
                long time1 = gyroData.get(0).t;
                long time2 = acceData.get(0).t;
                isAlign = isAligned(time1, time2, MaxTimediff);
                if (!isAlign) {
                    Log.i(TAG, "Re-align " + time1 + "(" + gyroData.size() + ") " +
                            time2 + "(" + acceData.size() + ") " + "dif: " + (time1 - time2) / 1e6);

                    // remove the one that has earlier timestamp
                    if (time1 < time2) {
                        gyroData.remove(0);
                    } else {
                        acceData.remove(0);
                    }
                } else {
                    //Log.i(TAG, "IMU Aligned "+time1 + "("+ gyroData.size()+") "+
                    // time2 + "("+ acceData.size()+") "+ "dif: " + (time1 - time2)/1e6);
                }
            } while (!isAlign);
            return true;
        }

        private boolean alignSensors_v2(int[] validSize) {
            int acceId = 0;
            int gyroId = 0;
            validSize[0] = 0;
            do {
                if (gyroData.size() == 0 || acceData.size() == 0) {
                    //Log.i(TAG,"no motion data available");
                    return false;
                }

                // check if the first data is aligned or not
                long gyro_t1 = gyroData.get(gyroId).t;
                long acce_t = acceData.get(acceId).t;
                long dif_t1 = gyro_t1 - acce_t;

                if (dif_t1 > 0) {
                    Log.i(TAG, "Re-align " + gyro_t1 + "(" + gyroData.size() + ") " +
                            acce_t + "(" + acceData.size() + ") " + "dif: " + (dif_t1) / 1e6);
                    acceData.remove(acceId);
                } else if ((gyroId + 1) >= gyroData.size()) {
                    //Log.i(TAG, "gyro data is not enough "+ gyroId + "/" + gyroData.size());
                    return false; // wait for more data to come
                } else {
                    // check if current acceId is within two gyro measurements
                    long gyro_t2 = gyroData.get(gyroId + 1).t;
                    long dif_t2 = gyro_t2 - acce_t;
                    if (dif_t2 < 0) {
                        //Log.i(TAG, "Remove gyro data ("+ gyroId + ")/" + gyroData.size() + " dif_2: " + ((dif_t2)/1e6));
                        gyroData.remove(gyroId);
                    } else {
                        // finally we can perform the interpolation
                        //Log.i(TAG, "IMU Aligned "+gyro_t1 + "("+ gyroData.size()+") "+
                        //    acce_t + "("+ acceData.size()+") "+ "dif: " + (dif_t1)/1e6);

                        float w1 = (float) (dif_t2) / (float) (dif_t2 - dif_t1);
                        float w2 = 1.f - w1;
                        float[] v1 = gyroData.get(gyroId).v.clone();
                        float[] v2 = gyroData.get(gyroId + 1).v.clone();
                        v1[0] = v1[0] * w1 + v2[0] * w2;
                        v1[1] = v1[1] * w1 + v2[1] * w2;
                        v1[2] = v1[2] * w1 + v2[2] * w2;
                        gyroData.get(gyroId).v = v1.clone();
                
/*                Log.i(TAG, "interpolated gyro data ("+ gyroId + ")/" + gyroData.size() + 
                                    " of acce data ("+ acceId + ")/" + acceData.size() + 
                                    " dif t1: "+(int)(dif_t1/1000) + 
                                    " dif t2: "+(int)(dif_t2/1000));
*/
                        validSize[0]++;
                        gyroId++;
                        acceId++;
                    }
                }
            } while (acceId < acceData.size());

            // Log.i(TAG, "validSize is " + validSize[0] );
            return true;
        }
    }

    private boolean isAligned(long timestamp1,
                              long timestamp2,
                              long maxTimeDiff) {
        long dif = timestamp1 - timestamp2;
        return dif < maxTimeDiff && dif > -maxTimeDiff;
    }


    // private functions
    private class CannedMotionTimerTask extends TimerTask {
        @Override
        public void run() {
            // collect the sensor data
            ReadCannedIMUData.IMUData imuData = ReadCannedIMUData.getInstance().grab();

            if (imuData == null || imuData.size == 0) {
                return;
            }
            // send it to JNI layer
            Log.i(TAG, "canneddata IMU grabbed size=" + imuData.size);

            // asynchronous reading
            //Log.i(TAG,"before call JNI get sensor");
            getSensorReading(imuData.gyroArray,
                    imuData.accArray,
                    imuData.accTimestampArray,
                    imuData.unixTimeStamp,
                    imuData.size);


        }

    }

    public native void getSensorReading(float gyro[],
                                        float quaternion[],
                                        long currTimeStamp[],
                                        long difUnixtime,
                                        int size);

}
