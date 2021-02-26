/*============================================================================
@file QSensor.java

@brief
Manages all sensor data and current listener parameters.

Copyright (c) 2013-2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/

package com.qualcomm.qti.sensors.core.stream;

import java.util.LinkedList;

import android.hardware.Sensor;

import com.qualcomm.qti.sensors.core.qsensortest.SettingsDatabase;

public class QSensor {
  static protected int effectiveRateCount;
  static protected int eventLength;

  static{
    QSensor.effectiveRateCount = Integer.parseInt(SettingsDatabase.getSettings().getProperty("stream_eff_rate_cnt"));
    QSensor.eventLength = Integer.parseInt(SettingsDatabase.getSettings().getProperty("stream_event_len"));
  }

  private Sensor sensor;  // Associated Android sensor
  private LinkedList<SensorSample> sensorEvents;
  private int streamRate; // Streaming rate requested by the user
  private int reportRate; // Report (aka batch) rate requested by the user
  private int accuracy;   // Accuracy values received by onAccuracyChanged callback

  public QSensor(Sensor sensor) {
    super();
    this.sensor = sensor;
    this.sensorEvents = new LinkedList<SensorSample>();
    this.accuracy = -1;
    this.streamRateIs(-1);
    this.reportRateIs(-1);
    this.sensorEventsClear();
  }

  public Sensor sensor() { return this.sensor; }
  public int streamRate() { return this.streamRate; }
  public int reportRate() { return this.reportRate; }
  protected int accuracy() { return this.accuracy; }
  protected LinkedList<SensorSample> sensorEvents() { return this.sensorEvents; }

  protected void reportRateIs(int reportRate) { this.reportRate = reportRate; }
  protected void streamRateIs(int streamRate) { this.streamRate = streamRate; }
  protected void accuracyIs(int accuracy) { this.accuracy = accuracy; }
  protected void sensorEventIs(SensorSample sensorEvent) {
    this.sensorEvents.addFirst(sensorEvent);

    if(this.sensorEvents().size() > QSensor.effectiveRateCount &&
       this.sensorEvents().size() > QSensor.eventLength)
      this.sensorEvents.removeLast();
  }
  protected void sensorEventsClear() {
    this.sensorEvents.clear();

    for(int i = 0; i < QSensor.eventLength || i < QSensor.effectiveRateCount; i++)
      this.sensorEventIs(new SensorSample(sensor));
  }
}
