/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.threedr.mmrd.speedProvider;

import com.qualcomm.qti.threedr.mmrd.sensorfusioncv.MotionData;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorManager;
import android.util.Log;
public class LinearAccelerometerSpeedProvider extends SpeedProvider {    
    private long timestamp;
    private long sensorInitializationTime = 15; // mili-second. This is an empirical value that
                                                // tells you the first sensor measurement since start()
                                                // Maybe it should be a function of the sensor sampling rate?
    
    public LinearAccelerometerSpeedProvider(SensorManager sensorManager) {
        super(sensorManager);
        
        sensorList.add(sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER));
    }

    @Override
    public void start() {
        timestamp        = 0;
        difUnixTimestamp = 0; // assume dif between two timestamp are the same
        super.start();
        
        difUnixTimestamp = (System.nanoTime()) /1000L +
                            sensorInitializationTime*1000L; // get an approximation of 
        // of the time difference
    }

    @Override
    public void onSensorChanged(SensorEvent event) {
        if (event.sensor.getType() == Sensor.TYPE_ACCELEROMETER) {            
            if (timestamp != 0) {
              
              // ===== debug =====
              //Log.d("MotionSensorManager","dif time" + (timestamp - event.timestamp)/1000L);
              
                // record data
                synchronized (syncToken) {
                  timestamp = event.timestamp;///1000;
                  
                  // push the information into vector
                  acceData.add(new MotionData(event.values, timestamp));                                     
                }               
                
            }
            else {
              synchronized (syncToken) {
                timestamp        = event.timestamp;///1000;
                //difUnixTimestamp = timestamp/1000L - difUnixTimestamp; // get an approximation of
                // of the time difference
                Log.d("Scan3D","MotionSensorManager: Guess unix time is " + (System.nanoTime()) /1000L +
                      " difUnix" + difUnixTimestamp);
              }
            }
        }
    }

}
