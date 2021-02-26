/*============================================================================
@file QSensorEventListener.java

@brief
Call-back API for threshold JNI.

Copyright (c) 2012-2013, 2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/
package com.qualcomm.qti.sensors.core.qsensortest;

import android.annotation.SuppressLint;
import android.hardware.SensorEvent;
import android.hardware.TriggerEvent;
import android.hardware.TriggerEventListener;

@SuppressLint("NewApi")
public class QSensorEventListener extends TriggerEventListener {
   public void onSensorChanged(SensorEvent event) {
   }

   @Override
   public void onTrigger(TriggerEvent event) {}
}
