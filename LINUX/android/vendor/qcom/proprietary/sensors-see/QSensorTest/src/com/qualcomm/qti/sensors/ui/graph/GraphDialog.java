/*============================================================================
@file GraphDialog.java

@brief
Dialog box for the user to select the sensor and rates to graph.

Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/
package com.qualcomm.qti.sensors.ui.graph;

import java.util.ArrayList;
import java.util.List;

import android.app.Dialog;
import android.hardware.Sensor;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Spinner;

import com.qualcomm.qti.sensors.core.qsensortest.QSensorContext;
import com.qualcomm.qti.sensors.ui.qsensortest.TabControl;
import com.qualcomm.qti.sensors.core.stream.SensorAdapter;
import com.qualcomm.qti.sensors.qsensortest.R;

public class GraphDialog extends Dialog {

  public GraphDialog(View.OnClickListener onClickListener, int currStreamRate, int currReportRate) {
    super(TabControl.getContext());

    this.setContentView(R.layout.graph_dialog);
    this.setTitle("Activate Graph Stream");

    Spinner sensorSpinner = (Spinner) this.findViewById(R.id.sensor_spinner);
    List<SensorAdapter> sensorList = new ArrayList<SensorAdapter>();
    for(Sensor sensor : QSensorContext.sensorManager().getSensorList(Sensor.TYPE_ALL))
    {
      sensorList.add(new SensorAdapter(sensor, QSensorContext.sensorManager()));
    }
    ArrayAdapter<SensorAdapter> adapter = new ArrayAdapter<SensorAdapter>(
        TabControl.getContext(), android.R.layout.simple_spinner_item, sensorList);
    adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
    sensorSpinner.setAdapter(adapter);

    String currentRate = currStreamRate == -1 ? "" : String.valueOf(currStreamRate);
    EditText streamRateField = (EditText) this.findViewById(R.id.delay_field_sample);
    streamRateField.setText(currentRate);

    currentRate = currReportRate == -1 ? "" : String.valueOf(currReportRate);
    EditText reportRateField = (EditText) this.findViewById(R.id.delay_field_report);
    reportRateField.setText(currentRate);

    Button dialogSubmitButton = (Button) this.findViewById(R.id.delay_button_submit);
    dialogSubmitButton.setOnClickListener(onClickListener);

    Button dialogCancelButton = (Button) this.findViewById(R.id.delay_button_cancel);
    dialogCancelButton.setOnClickListener(onClickListener);
  }
}
