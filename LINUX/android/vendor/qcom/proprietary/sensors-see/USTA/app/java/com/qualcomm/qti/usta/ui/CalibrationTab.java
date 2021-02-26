/*============================================================================
  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  @file CalibrationTab.java
  @brief
   CalibrationTab class implementation
============================================================================*/
package com.qualcomm.qti.usta.ui;

import android.app.Fragment;
import android.graphics.Color;
import android.graphics.Typeface;
import android.os.Bundle;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;

import com.qualcomm.qti.usta.R;
import com.qualcomm.qti.usta.core.AccelCalibrationJNI;
import com.qualcomm.qti.usta.core.USTALog;

public class CalibrationTab extends Fragment implements Runnable {

  private View calTabView;
  private Button calReadButton;
  private Button calStopButton;
  private int calSensorHandle;
  boolean isThreadRunning = true;
  public static boolean isDisplaySamplesEnabled = true;
  private TextView eventMessageTextView;
  private static Thread samplesThread;


  @Override
  public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
    calTabView = inflater.inflate(R.layout.calibration_tab, container, false);
    setUpCalreaddButton();
    setUpCalStopRequest();
    eventMessageTextView = (TextView) calTabView.findViewById(R.id.sensor_event_cal_id);
    samplesThread = new Thread(this);
    samplesThread.start();
    return calTabView;

  }


  private void setUpCalreaddButton() {

    calReadButton = (Button) calTabView.findViewById(R.id.cal_read_button);

    calReadButton.setOnClickListener(new View.OnClickListener() {
      @Override
      public void onClick(View v) {
        USTALog.i("You clicked the cal button ");
        AccelCalibrationJNI.startCalibrationWrapper();
      }
    });
  }

  private void setUpCalStopRequest(){
    calStopButton = (Button) calTabView.findViewById(R.id.cal_stop_button);
    calStopButton.setOnClickListener(new View.OnClickListener() {
      @Override
      public void onClick(View v) {
        USTALog.i("going to stop accel cal  ");
        AccelCalibrationJNI.stopCalibrationWrapper();
      }
    });
  }

  public void displaySamplesCallback(final String inSensorEventData) {

    if (isDisplaySamplesEnabled == true) {
      if (getActivity() != null) {
        getActivity().runOnUiThread(new Runnable() {
          @Override
          public void run() {

            if (inSensorEventData != null && inSensorEventData.length() != 0) {
              eventMessageTextView.setText(inSensorEventData);
              eventMessageTextView.setTextColor(Color.BLUE);
              eventMessageTextView.setTypeface(null, Typeface.BOLD_ITALIC);
            } else {
              eventMessageTextView.setText("");
            }
          }
        });
      }
    } else {
      if (getActivity() != null) {
        getActivity().runOnUiThread(new Runnable() {
          @Override
          public void run() {
            eventMessageTextView.setText("");
          }
        });
      }
    }
  }


  @Override
  public void run() {
    while (isThreadRunning) {
      if (isDisplaySamplesEnabled == true) {
        String inDisplayString = AccelCalibrationJNI.getAccelCalFromNativeWrapper();
        if (inDisplayString != null && inDisplayString.length() != 0)
          displaySamplesCallback(inDisplayString);
      }

    }
  }
}
