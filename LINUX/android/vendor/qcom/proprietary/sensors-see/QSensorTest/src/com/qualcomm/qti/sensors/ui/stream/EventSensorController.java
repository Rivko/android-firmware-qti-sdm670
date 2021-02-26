/*============================================================================
@file SensorEventController.java

@brief
Handles button-clicks for event-sensors.

Copyright (c) 2013, 2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/
package com.qualcomm.qti.sensors.ui.stream;

import android.annotation.SuppressLint;
import android.hardware.SensorManager;
import android.view.View;
import android.content.Context;

import com.qualcomm.qti.sensors.qsensortest.R;
import com.qualcomm.qti.sensors.core.stream.SensorAdapter;
import com.qualcomm.qti.sensors.ui.qsensortest.TabControl;
import com.qualcomm.qti.sensors.ui.stream.WearStreamingActivity;

public class EventSensorController extends SensorController{
  public Context mContext;
  public EventSensorController(SensorManager sensorManager, SensorAdapter sensorAdapter) {
    super(sensorManager, sensorAdapter);
     mContext = TabControl.getContext();
     if (TabControl.EnableWearQSTP) {
          mContext = WearStreamingActivity.getContext();
     }
  }

  @SuppressLint("NewApi")
  @Override
  public boolean onLongClick(View view) {
     if (TabControl.EnableWearQSTP) {
          mContext = WearStreamingActivity.getContext();
     }
     if(R.id.stream_event_column1 == view.getId() || R.id.stream_event_column2 == view.getId() ||
        R.id.stream_sensor_title == view.getId()) {
        boolean batchSupport = this.sensorAdapter.sensor().getFifoMaxEventCount() > 0;
        this.activeDialog = new SensorDialog(this, this.sensorAdapter.streamRate(),
            this.sensorAdapter.reportRate(), batchSupport,mContext);
        this.activeDialog.show();
     }

     return true;
  }

  @SuppressLint("NewApi")
  @Override
  public void onClick(View view) {
    if(R.id.stream_event_column1 == view.getId() || R.id.stream_event_column2 == view.getId() ||
        R.id.stream_sensor_title == view.getId()){
       boolean batchSupport = this.sensorAdapter.sensor().getFifoMaxEventCount() > 0;
       this.activeDialog = new SensorDialog(this, this.sensorAdapter.streamRate(),
           this.sensorAdapter.reportRate(), batchSupport,mContext);
       this.sensorAdapter.streamRateIs(SensorManager.SENSOR_DELAY_NORMAL, -1, true);
    }
    else if(R.id.stream_button_cancel == view.getId()){
      this.sensorAdapter.streamRateIs(-1, -1, false);
    }
    else {
       super.onClick(view);
    }
  }
}
