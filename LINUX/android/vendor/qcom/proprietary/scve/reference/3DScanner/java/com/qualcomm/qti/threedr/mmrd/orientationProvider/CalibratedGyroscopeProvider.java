/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.threedr.mmrd.orientationProvider;

import com.qualcomm.qti.threedr.mmrd.sensorfusioncv.MotionData;

import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorManager;

/**
 * The orientation provider that delivers the relative orientation from the {@link Sensor#TYPE_GYROSCOPE
 * Gyroscope}. This sensor does not deliver an absolute orientation (with respect to magnetic north and gravity) but
 * only a relative measurement starting from the point where it started.
 * 
 */
public class CalibratedGyroscopeProvider extends OrientationProvider {
    
    /**
     * The time-stamp being used to record the time when the last gyroscope event occurred.
     */
    private long timestamp;

    /**
     * Initialises a new CalibratedGyroscopeProvider
     * 
     * @param sensorManager The android sensor manager
     */
    public CalibratedGyroscopeProvider(SensorManager sensorManager) {
        super(sensorManager);

        //Add the gyroscope
        sensorList.add(sensorManager.getDefaultSensor(Sensor.TYPE_GYROSCOPE));        
        timestamp = 0;
    }

    @Override
    public void onSensorChanged(SensorEvent event) {

        // we received a sensor event. it is a good practice to check
        // that we received the proper event      
        if (event.sensor.getType() == Sensor.TYPE_GYROSCOPE) {          
            // This timestep's delta rotation to be multiplied by the current rotation
            // after computing it from the gyro sample data.
            if (timestamp != 0) {                                           
                
                // record data
                synchronized (syncToken) {
                  timestamp = event.timestamp;                 
                  // push the information into vector                  
                  gyroData.add(new MotionData(event.values,timestamp));                    
                }                         
                
                // ===== debug =====
                /*Log.d(this.getClass().getSimpleName(), 
                     " x " + event.values[0] + 
                     " y " + event.values[1] + 
                     " z " + event.values[2]);*/
            }                       
            else {            
              synchronized (syncToken) {
                // ===== debug =====
                //Log.i("CalibratedGyro","the gyro capturing at "+(event.timestamp - timestamp)*0.000001 );
                timestamp = event.timestamp;
              }     
            }
        }
        /*else if (event.accuracy == SensorManager.SENSOR_STATUS_UNRELIABLE)
        {
            Log.d("MotionSensorManager", "Accuracy unreliable");
            return;
        }*/
    }
    
    public long getTimeStamp()
    {
      long timestamp_out;
      synchronized (syncToken) {
    	timestamp_out = timestamp;
      }
      
      return timestamp_out;
    }
}

