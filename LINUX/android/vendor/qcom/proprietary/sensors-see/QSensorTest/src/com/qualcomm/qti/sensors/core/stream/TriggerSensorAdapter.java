/*============================================================================
@file TriggerSensorAdapter.java

@brief
Trigger sensor wrapper API for View and Controller to access or modify the
QSensor Model.

Copyright (c) 2013, 2015-2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/
package com.qualcomm.qti.sensors.core.stream;

import android.annotation.SuppressLint;
import android.hardware.Sensor;
import android.hardware.SensorManager;
import android.util.Log;
import android.widget.Toast;

import com.qualcomm.qti.sensors.core.qsensortest.QSensorContext;
import com.qualcomm.qti.sensors.core.qsensortest.QSensorService;

public class TriggerSensorAdapter extends EventSensorAdapter {

  public TriggerSensorAdapter(Sensor sensor, SensorManager sensorManager) {
    super(sensor, sensorManager);
    this.accuracyIs(SensorManager.SENSOR_STATUS_ACCURACY_HIGH);
  }

  @Override
  @SuppressLint("NewApi")
  public synchronized void streamRateIs(int streamRate, int reportRate, boolean clear){
    this.sensorManager.cancelTriggerSensor(this.sensorListener, this.sensor());
    this.sensorListener = null;

    if(-1 != streamRate) {
      this.sensorListener = new SensorListener(this);
      if(!this.sensorManager.requestTriggerSensor(this.sensorListener, this.sensor())) {
        streamRate = -1;
        Log.e(QSensorContext.TAG, "Unable to request trigger");
        Toast.makeText(QSensorContext.getContext(), "Unable to request trigger", Toast.LENGTH_LONG).show();
      }
    }

    if(clear)
      this.sensorEventsClear();

    Log.d(QSensorContext.TAG, "Change sensor rate for " + this.sensor().getType() +
        " from " + this.streamRate() + " (" + this.reportRate() + ") to " +
        streamRate + " (" + reportRate + ")");

    this.streamRateIs(streamRate);
    this.reportRateIs(reportRate);
  }

  public synchronized void eventIs(SensorSample sensorSample){
    if(!QSensorContext.optimizePower)
      super.eventIs(sensorSample);

    if(QSensorContext.autoRetryTrigger)
      this.streamRateIs(this.streamRate(), this.reportRate(), false);
    else
      this.streamRateIs(-1, -1, false);
  }
}
