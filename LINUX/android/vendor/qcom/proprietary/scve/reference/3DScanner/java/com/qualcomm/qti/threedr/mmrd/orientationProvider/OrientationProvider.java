/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.threedr.mmrd.orientationProvider;

import java.util.ArrayList;
import java.util.List;
import com.qualcomm.qti.threedr.mmrd.sensorfusioncv.MotionData;

import android.hardware.Sensor;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.util.Log;

/**
 * Classes implementing this interface provide an orientation of the device either by directly accessing hardware, using
 * Android sensor fusion or fusing sensors itself.
 * 
 * The orientation can be provided as rotation matrix or quaternion.
 * 
 */
public abstract class OrientationProvider implements SensorEventListener {
    /**
     * Sync-token for syncing read/write to sensor-data from sensor manager and fusion algorithm
     */
    protected final Object          syncToken = new Object();   
    protected List<Sensor>          sensorList = new ArrayList<Sensor>();          
    protected SensorManager         sensorManager;    
    protected ArrayList<MotionData> gyroData;
    protected final int             SensorBufferSize = 20;
    private   String                sensorName;    

    // Construcotr        
    /**
     * Initialises a new OrientationProvider
     * 
     * @param sensorManager The android sensor manager
     */    
    public OrientationProvider(SensorManager sensorManager) {
        this.sensorManager = sensorManager;
        // init sensor readings        
        gyroData = new ArrayList<MotionData>();
    }
    
    // MMethods

    /**
     * Starts the sensor fusion (e.g. when resuming the activity)
     */
    public void start() {
        // enable our sensor when the activity is resumed, ask for
        // 10 ms updates.      
        for (Sensor sensor : sensorList) {
            Log.d("MotionSensorManager", " sensor "+ sensor.getName() + " vendor " + sensor.getVendor());
            // enable our sensors when the activity is resumed, ask for
            // 20 ms updates (Sensor_delay_game)
            //sensorManager.registerListener(this, sensor, SensorManager.SENSOR_DELAY_GAME);
            sensorManager.registerListener(this, sensor, SensorManager.SENSOR_DELAY_FASTEST);
            sensorName = sensor.getName();
        }
    }

    /**
     * Stops the sensor fusion (e.g. when pausing/suspending the activity)
     */
    public void stop() {
        // make sure to turn our sensors off when the activity is paused
        for (Sensor sensor : sensorList) {
            sensorManager.unregisterListener(this, sensor);
        }
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {
        // Not doing anything
    }       
    
    public boolean isDataAvailable() {
      Log.i("MotionSensorManager", "gyro data size " + gyroData.size());
      return gyroData.size() > 0;
    }
    
    public String getSensorName()
    {
      return sensorName;
    }
    public void flushReadings(ArrayList<MotionData> out)   
    {
      synchronized (syncToken) {
        out.addAll(gyroData);        
        gyroData.clear();
      }
    }   
}

