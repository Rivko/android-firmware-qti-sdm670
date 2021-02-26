/*============================================================================
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  @file SensorEventDisplayFragment.java
  @brief
   SensorEventDisplayFragment class implementation
============================================================================*/

package com.qualcomm.qti.usta.ui;

import android.app.Fragment;
import android.graphics.Color;
import android.graphics.Typeface;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.qualcomm.qti.usta.R;
import com.qualcomm.qti.usta.core.SensorContextJNI;

public class SensorEventDisplayFragment extends Fragment implements Runnable {


  private View SensorEventDisplayFrgmentView;
  private TextView eventMessageTextView;
  public static boolean isDisplaySamplesEnabled = true;
  boolean isThreadRunning = true;
  private static Thread samplesThread;

  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
  }

  @Nullable
  @Override
  public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {

    SensorEventDisplayFrgmentView = inflater.inflate(R.layout.sensor_event_payload_display, container, false);
    eventMessageTextView = (TextView) SensorEventDisplayFrgmentView.findViewById(R.id.sensor_event_payload_textview_id);
    return SensorEventDisplayFrgmentView;
  }

  public static SensorEventDisplayFragment CreateDisplaySamplesInstance() {
    SensorEventDisplayFragment SensorEventDisplayFragmentInstance = new SensorEventDisplayFragment();
    samplesThread = new Thread(SensorEventDisplayFragmentInstance);
    samplesThread.start();
    return SensorEventDisplayFragmentInstance;
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

  public void run() {

    while (isThreadRunning) {
      String inDisplayString = SensorContextJNI.getSamplesFromNativeWrapper(false);
      if(inDisplayString != null && inDisplayString.length() != 0)
        displaySamplesCallback(inDisplayString);
    }
  }

}
