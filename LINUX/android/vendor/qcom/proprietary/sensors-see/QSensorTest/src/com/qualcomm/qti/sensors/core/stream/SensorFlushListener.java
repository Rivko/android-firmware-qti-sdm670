/*============================================================================
@file QSensorFlushListener.java

@brief
Receives flush complete events from the Android Sensors Manager.

Copyright (c) 2013, 2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/
package com.qualcomm.qti.sensors.core.stream;

import android.hardware.Sensor;
import android.hardware.SensorEventListener2;
import android.util.Log;
import android.widget.Toast;

import com.qualcomm.qti.sensors.core.qsensortest.QSensorContext;

public class SensorFlushListener extends SensorListener implements SensorEventListener2 {

  public SensorFlushListener(SensorAdapter sensorAdapter) {
    super(sensorAdapter);
  }

  @Override
  public void onFlushCompleted(Sensor sensor) {
    Toast.makeText(QSensorContext.getContext(), "Flush complete for:\n" + this.sensorAdapter.sensor().getName(),
          Toast.LENGTH_SHORT).show();
    Log.d(QSensorContext.TAG, "Flush complete event received for: " + this.sensorAdapter.sensor().getName());
  }
}