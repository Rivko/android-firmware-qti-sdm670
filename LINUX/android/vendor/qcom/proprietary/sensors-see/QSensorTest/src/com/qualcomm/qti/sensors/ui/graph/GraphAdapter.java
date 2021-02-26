/*============================================================================
@file GraphAdapter.java

@brief
Sensor listener, data storage, and manager of the graph view.

Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/
package com.qualcomm.qti.sensors.ui.graph;

import java.util.concurrent.CopyOnWriteArrayList;

import android.content.Context;
import android.graphics.Color;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Handler;
import android.util.Log;
import android.view.View;
import android.widget.TabHost.TabContentFactory;
import android.widget.Toast;

import com.qualcomm.qti.sensors.core.stream.QSensor;
import com.qualcomm.qti.sensors.core.stream.SensorSample;
import com.qualcomm.qti.sensors.core.qsensortest.QSensorContext;
import com.qualcomm.qti.sensors.ui.qsensortest.TabControl;

public class GraphAdapter implements TabContentFactory, SensorEventListener {
  static private int graphCount = 0;
  static final long eventWindow = 500;
  static final int maxAxisCnt = 16;

  private CopyOnWriteArrayList<SensorSample> sensorData;
  private GraphView graphView;
  private Context context;
  private QSensor sensor;
  protected SensorManager sensorManager;
  private String tabTitle;
  private int axisCnt;

  public GraphAdapter(Context context)
  {
    GraphAdapter.graphCount++;

    this.tabTitle = Integer.toString(GraphAdapter.graphCount);
    this.sensor = null;
    this.axisCnt = 0;
    this.sensorManager = QSensorContext.sensorManager();
    this.sensorData = new CopyOnWriteArrayList<SensorSample>();
    this.graphView = new GraphView(context, this);
    this.graphView.setBackgroundColor(Color.WHITE);
  }

  public GraphView graphView() { return this.graphView; }
  // PEND: Find better way of synchronizing data access, other than using CopyOnWriteArrayList
  public synchronized CopyOnWriteArrayList<SensorSample> sensorData() { return this.sensorData; }
  public QSensor sensor() { return this.sensor; }
  public String tabTitle() { return this.tabTitle; }
  public int axisCnt() { return this.axisCnt; }

  public void cancel() {
    Log.d(SensorGraphActivity.TAG, "Canceling stream associated with graph " + this.tabTitle());
    this.sensorManager.unregisterListener(this, this.sensor().sensor());
    for(int i = 0; i < GraphAdapter.maxAxisCnt; i++)
      this.sensorData.clear();
    this.graphView = null;
  }

  public synchronized void sensorEventIs(SensorSample sensorSample) {
    if(0 == this.axisCnt)
      this.axisCnt = sensorSample.values().length;

    for(int j = 0; j < this.axisCnt(); j++){
      this.sensorData.add(sensorSample);
      if(this.sensorData.size() > eventWindow)
        this.sensorData.remove(0);
    }
  }

  @Override
  public void onAccuracyChanged(Sensor sensor, int accuracy) {}

  @Override
  public void onSensorChanged(SensorEvent event) {
    this.sensorEventIs(new SensorSample(event, sensor));
  }

  public void streamRateIs(QSensor sensor, int streamRate, int reportRate) {
    Handler handler = new Handler(SensorGraphActivity.handlerThread.getLooper());
    this.sensor = sensor;

    Log.d(SensorGraphActivity.TAG, "Registering sensor listener.  Sensor: " + sensor.sensor().getType() +
        " sampling rate: " + streamRate + ", batch rate: " + reportRate);

    if(-1 == reportRate) {
      if(!this.sensorManager.registerListener(this,
          sensor.sensor(), streamRate < 4 ? streamRate : streamRate * 1000, handler)) {
        streamRate = -1;
        Toast.makeText(TabControl.getContext(),
            "Unable to register new listener", Toast.LENGTH_LONG).show();
      }
    }
    else {
      if(!this.sensorManager.registerListener(this, sensor.sensor(),
          streamRate < 4 ? streamRate : streamRate * 1000, reportRate * 1000, handler)) {
        streamRate = -1;
        reportRate = -1;
        Toast.makeText(TabControl.getContext(),
          "Unable to register new listener", Toast.LENGTH_LONG).show();
      }
    }
  }

  @Override
  public View createTabContent(String tag) {
    return this.graphView();
  }
}
