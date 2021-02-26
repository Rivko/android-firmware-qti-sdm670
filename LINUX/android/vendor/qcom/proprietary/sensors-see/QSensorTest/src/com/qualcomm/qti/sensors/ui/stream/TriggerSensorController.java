/*============================================================================
@file SensorTriggerController.java

@brief
Handles button-clicks for trigger-mode sensors.

Copyright (c) 2013, 2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/
package com.qualcomm.qti.sensors.ui.stream;

import android.hardware.SensorManager;
import android.view.View;

import com.qualcomm.qti.sensors.core.stream.SensorAdapter;

public class TriggerSensorController extends EventSensorController {

  public TriggerSensorController(SensorManager sensorManager,
      SensorAdapter sensorAdapter) {
    super(sensorManager, sensorAdapter);
    // TODO Auto-generated constructor stub
  }

  @Override
  public boolean onLongClick(View view) {
     return true;
  }
}
