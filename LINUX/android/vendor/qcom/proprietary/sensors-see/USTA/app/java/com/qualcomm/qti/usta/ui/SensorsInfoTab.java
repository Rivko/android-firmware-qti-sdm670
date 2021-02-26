/*============================================================================
  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  @file SensorsInfoTab.java
  @brief
   SensorsInfoTab class implementation for displaying the sensors attributes
============================================================================*/

package com.qualcomm.qti.usta.ui;

import android.app.Fragment;
import android.graphics.Color;
import android.graphics.Typeface;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Spinner;
import android.widget.TextView;


import com.qualcomm.qti.usta.R;
import com.qualcomm.qti.usta.core.ModeType;
import com.qualcomm.qti.usta.core.SensorContext;
import com.qualcomm.qti.usta.core.SensorContextJNI;
import com.qualcomm.qti.usta.core.USTALog;


public class SensorsInfoTab extends Fragment {

  private SensorContext sensorContext;
  private View sensorsInfoTabView;
  private TextView sensorInfoTextView;
  private String sensorsInfoString;
  final String NEWLINE = "\n";
  private Spinner sensorSpinnerList;

  @Override
  public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
    sensorsInfoTabView =  inflater.inflate(R.layout.sensors_info_tab, container, false);

    sensorContext = SensorContext.getSensorContext(ModeType.USTA_MODE_TYPE_UI);

    if(sensorContext == null){
      USTALog.e("sensorContext is null ");
      return null;
    }
    sensorInfoTextView = (TextView) sensorsInfoTabView.findViewById(R.id.sensor_info_text);
    setUpSensorsInfoListSpinner();

    return sensorsInfoTabView;
  }

  @Override
  public void onCreate( Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setHasOptionsMenu(true);
  }

  @Override
  public void onPrepareOptionsMenu(Menu menu) {
    menu.findItem(R.id.disable_qmi_connection_id).setEnabled(false);
  }

  private void setUpSensorsInfoListSpinner(){
    int defaultPosition = 0;
    sensorSpinnerList = (Spinner) sensorsInfoTabView.findViewById(R.id.sensor_info_handle);
    ArrayAdapter<String> sensorSpinnerListAdapter = new ArrayAdapter<>(getActivity().getApplicationContext(), android.R.layout.simple_list_item_1, sensorContext.getSensorNames());
    sensorSpinnerList.setAdapter(sensorSpinnerListAdapter);
    sensorSpinnerList.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
      @Override
      public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {

        sensorsInfoString = "";
        updateSensorsInfo(sensorSpinnerList.getSelectedItemPosition());

        if(sensorsInfoString != null && !sensorsInfoString.isEmpty()) {
          sensorInfoTextView.setText(sensorsInfoString);
          sensorInfoTextView.setTextColor(Color.BLUE);
          sensorInfoTextView.setTypeface(null, Typeface.BOLD_ITALIC);
        }

      }

      @Override
      public void onNothingSelected(AdapterView<?> parent) {

      }
    });
    sensorSpinnerList.setSelection(defaultPosition);

  }

  private void updateSensorsInfo(int sensorIndex) {
    if (sensorContext == null)
      return;
    if (sensorContext.getSensors() == null)
      return;
    if (sensorIndex > sensorContext.getSensors().size())
      return;

    sensorsInfoString += "Sensor Count : (" + Integer.toString(sensorIndex) + ")";
    sensorsInfoString += NEWLINE;

    sensorsInfoString += "Sensor Name: " + sensorContext.getSensors().get(sensorIndex).getSensorName();
    sensorsInfoString += NEWLINE;

    sensorsInfoString += "SUID Low: " + sensorContext.getSensors().get(sensorIndex).getSensorSUIDLow();
    sensorsInfoString += NEWLINE;

    sensorsInfoString += "SUID High: " + sensorContext.getSensors().get(sensorIndex).getSensorSUIDHigh();
    sensorsInfoString += NEWLINE;

    sensorsInfoString += "Attributes Information : ";
    sensorsInfoString += NEWLINE;

    sensorsInfoString += SensorContextJNI.getSensorAttributesNativeWrapper(ModeType.USTA_MODE_TYPE_UI, sensorIndex);

    sensorsInfoString += NEWLINE;
    sensorsInfoString += NEWLINE;
    sensorsInfoString += NEWLINE;
  }



}