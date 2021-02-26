/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.threedr.mmrd.speedProvider;

import java.util.ArrayList;
import java.util.List;
import com.qualcomm.qti.threedr.mmrd.sensorfusioncv.MotionData;

import android.hardware.Sensor;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.util.Log;

public abstract class SpeedProvider implements SensorEventListener {
    protected final Object syncToken = new Object();
    protected List<Sensor> sensorList = new ArrayList<Sensor>();
    protected SensorManager sensorManager;
    protected boolean running;
    protected ArrayList<MotionData> acceData;
    protected final int     SensorBufferSize = 20;
    protected long          difUnixTimestamp;
    
    public SpeedProvider(SensorManager sensorManager) {
        this.sensorManager = sensorManager;
        running = false;
        // init sensor readings
        acceData = new ArrayList<MotionData>();
        
    }
    
    public long getDifUnixTimestamp()
    {
      return difUnixTimestamp;
    }
    
    public void start() {
        if(running)
            return;        
        difUnixTimestamp = 0;
        for (Sensor sensor : sensorList) {
            //sensorManager.registerListener(this, sensor, SensorManager.SENSOR_DELAY_GAME);
            sensorManager.registerListener(this, sensor, SensorManager.SENSOR_DELAY_FASTEST);
        }
        running = true;
    }
    
    public void stop() {
        if(!running)
            return;
        for (Sensor sensor : sensorList) {
            sensorManager.unregisterListener(this, sensor);
        }
        running = false;
    }      
    
    public void flushReadings(ArrayList<MotionData> out)   
    {
      synchronized (syncToken) {
        out.addAll(acceData);
        acceData.clear();
      }
    }

    public boolean isDataAvailable() {
      Log.i("MotionSensorManager", "acce data size " + acceData.size());
      return acceData.size() > 0;
    }
    
    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {
        // Not doing anything
    }
}
