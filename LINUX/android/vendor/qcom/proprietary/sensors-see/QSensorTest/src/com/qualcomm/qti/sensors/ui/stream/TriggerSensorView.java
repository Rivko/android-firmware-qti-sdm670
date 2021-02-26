/*============================================================================
@file TriggerSensorView.java

@brief
The view area associated with a trigger-mode SensorListItem

Copyright (c) 2013, 2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/
package com.qualcomm.qti.sensors.ui.stream;

import android.annotation.SuppressLint;
import android.hardware.SensorManager;

import com.qualcomm.qti.sensors.core.stream.SensorAdapter;

@SuppressLint("ViewConstructor")
public class TriggerSensorView extends EventSensorView {
  public TriggerSensorView(SensorAdapter sensorAdapter, SensorManager sensorManager) {
    super(sensorAdapter, sensorManager);
  }
}