/*============================================================================
@file SensorListener.java

@brief
SensorEvent listener.  Updates the associated QSensor upon event receipt.

Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/
package com.qualcomm.qti.sensors.core.stream;

import android.annotation.SuppressLint;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.TriggerEvent;

import com.qualcomm.qti.sensors.core.qsensortest.QSensorContext;
import com.qualcomm.qti.sensors.core.qsensortest.QSensorEventListener;

@SuppressLint("NewApi")
public class SensorListener extends QSensorEventListener implements SensorEventListener {
  protected SensorAdapter sensorAdapter;

  public SensorListener(SensorAdapter sensorAdapter) {
    this.sensorAdapter = sensorAdapter;
  }

  @Override
  public void onAccuracyChanged(Sensor sensor, int accuracy) {
    if(!QSensorContext.optimizePower)
      this.sensorAdapter.accuracyIs(accuracy);
  }

  @Override
  public void onSensorChanged(SensorEvent event) {
    if(!QSensorContext.optimizePower)
      this.sensorAdapter.eventIs(new SensorSample(event, this.sensorAdapter));
  }

  @Override
  public void onTrigger(TriggerEvent event) {
      this.sensorAdapter.eventIs(new SensorSample(event));
  }
}
