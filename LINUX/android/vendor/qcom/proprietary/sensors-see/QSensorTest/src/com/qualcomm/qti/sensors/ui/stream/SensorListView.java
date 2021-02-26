/*============================================================================
@file SensorListView.java

@brief
A scrolling view containing the child views associated with each of the sensors.

Copyright (c) 2011-2013, 2015-2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/
package com.qualcomm.qti.sensors.ui.stream;

import java.util.ArrayList;
import java.util.List;
import android.hardware.Sensor;

import android.annotation.SuppressLint;
import android.content.Context;
import android.hardware.SensorManager;
import android.view.LayoutInflater;
import android.widget.LinearLayout;
import android.widget.ScrollView;

import com.qualcomm.qti.sensors.core.qsensortest.QSensorContext;
import com.qualcomm.qti.sensors.core.stream.SensorAdapter;
import com.qualcomm.qti.sensors.qsensortest.R;
import com.qualcomm.qti.sensors.core.qsensortest.SettingsDatabase;
import com.qualcomm.qti.sensors.core.qsensortest.SettingsDatabase.SensorSetting;
import com.qualcomm.qti.sensors.ui.qsensortest.TabControl;

@SuppressLint("ViewConstructor")
public class SensorListView extends LinearLayout {
   protected SensorManager sensorManager;
   protected ScrollView scrollView;
   protected List<SensorView> sensorList;

   public SensorListView(Context context, SensorManager sensorManager) {
      super(context);
      this.sensorManager = sensorManager;

      this.sensorList = new ArrayList<SensorView>();

      LayoutInflater inflater = (LayoutInflater)TabControl.getContext().getSystemService(Context.LAYOUT_INFLATER_SERVICE);
      this.scrollView = (ScrollView) inflater.inflate(R.layout.activity_stream_list, null);

      LinearLayout viewList = (LinearLayout) this.scrollView.getChildAt(0);

      int index = 0;
      List<? extends SensorAdapter> sensorAdapters = this.getSensorAdapterList();
      for(SensorAdapter sensorAdapter : sensorAdapters) {
        SensorSetting settings = SettingsDatabase.getSettings().getSensorSetting(sensorAdapter.sensor());
        if(settings.getEnableStreaming()) {
          SensorView sensorView = createView(settings, sensorAdapter);

          if(null != sensorView) {
            viewList.addView(sensorView.childView(), index);
            this.sensorList.add(sensorView);
            index++;
          }
        }
      }

      this.addView(this.scrollView);
   }

   protected List<? extends SensorAdapter> getSensorAdapterList() {
       return QSensorContext.sensorAdapterList();
   }

   protected SensorView createView(SensorSetting settings, SensorAdapter sensorAdapter) {
     SensorView sensorView = null;
     Sensor sensor = sensorAdapter.sensor();
     /*RGB and CCT gives 3 values for each sample at a time unlike to other onChange Sensors*/
     if((sensor.getType() == QSensorContext.sensorTypeRGB) ||
        (sensor.getType() == QSensorContext.sensorTypeCCT)) {
         sensorView = new SensorView(sensorAdapter, sensorManager);
      }
     else if(SettingsDatabase.TriggerMode.OnChange == settings.getTriggerMode()) {
       sensorView = new EventSensorView(sensorAdapter, sensorManager);
     }
     else if(SettingsDatabase.TriggerMode.OneShot == settings.getTriggerMode()) {
       sensorView = new TriggerSensorView(sensorAdapter, sensorManager);
     } /*consider continuous mode and special mode as same to list Tilt and Stepdetector*/
     else {
       sensorView = new SensorView(sensorAdapter, sensorManager);
     }

     return sensorView;
   }

   public List<SensorView> sensorList() { return this.sensorList; }
}
