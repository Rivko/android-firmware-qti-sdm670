/*============================================================================
@file SensorSample.java

@brief
Wrapper around an Android SensorEvent.

Copyright (c) 2013-2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/
package com.qualcomm.qti.sensors.core.stream;

import java.util.Date;

import android.annotation.SuppressLint;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.TriggerEvent;

/**
 * Since SensorEvent was poorly written by Android, this class acts as
 * a proxy.
 */
public class SensorSample {
  private final float[] values;
  private Sensor sensor;
  private int accuracy;
  private long timestamp;
  private Date rcvTime;

  public SensorSample(SensorEvent sensorEvent, QSensor sensor) {
    this.rcvTime = new Date();
    this.values = sensorEvent.values.clone();
    this.sensor = sensorEvent.sensor;
    this.accuracy = sensorEvent.accuracy == 0 ?
        sensor.accuracy() : sensorEvent.accuracy;
    this.timestamp = sensorEvent.timestamp;
  }

  @SuppressLint("NewApi")
  public SensorSample(TriggerEvent triggerEvent) {
    this.rcvTime = new Date();
    this.values = triggerEvent.values.clone();
    this.sensor = triggerEvent.sensor;
    this.timestamp = triggerEvent.timestamp;
  }

  public SensorSample(Sensor sensor) {
    this.rcvTime = new Date();
    this.values = new float[3];
    this.values[0] = 0;
    this.values[1] = 0;
    this.values[2] = 0;
    this.sensor = sensor;
    this.accuracy = -1;
    this.timestamp = 0;
  }

  public float[] values() { return this.values; }
  public Sensor sensor() { return this.sensor; }
  public int accuracy() { return this.accuracy; }
  public long timestamp() { return this.timestamp; }
  public Date rcvTime() { return this.rcvTime; }
}
