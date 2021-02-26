/*============================================================================
@file QSensorController.java

@brief
Handler for all user input.

Copyright (c) 2013, 2015-2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/
package com.qualcomm.qti.sensors.ui.stream;

import android.annotation.SuppressLint;
import android.hardware.SensorManager;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnLongClickListener;
import android.widget.EditText;
import android.widget.Toast;
import android.content.Context;
import android.util.Log;
import android.graphics.Color;

import com.qualcomm.qti.sensors.core.stream.SensorAdapter;
import com.qualcomm.qti.sensors.qsensortest.R;
import com.qualcomm.qti.sensors.ui.qsensortest.TabControl;
import com.qualcomm.qti.sensors.ui.stream.WearStreamingActivity;

public class SensorController implements OnClickListener, OnLongClickListener {
  protected SensorManager sensorManager;
  protected SensorAdapter sensorAdapter;
  protected SensorDialog activeDialog;  // The dialog that is now active
  public Context mContext;
  String wearReportRateField;
  String wearSampleRateField;
  boolean flageWearReportRate = true;
  public  Color color = new Color();
  public SensorController(SensorManager sensorManager, SensorAdapter sensorAdapter) {
    this.sensorAdapter = sensorAdapter;
    this.sensorManager = sensorManager;
    this.activeDialog = null;
    mContext = TabControl.getContext();
    if (TabControl.EnableWearQSTP) {
        mContext = WearStreamingActivity.getContext();
    }
  }

  @SuppressLint("NewApi")
  @Override
  public boolean onLongClick(View view) {

  mContext = TabControl.getContext();
    if (TabControl.EnableWearQSTP) {
          mContext = WearStreamingActivity.getContext();
    }

    if(R.id.stream_data_column1 == view.getId() || R.id.stream_data_column2 == view.getId() ||
       R.id.stream_sensor_title == view.getId() || R.id.stream_sensor_rate == view.getId()) {
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
    SensorAdapter.isDelayButtonSubmit = false;
    if(R.id.stream_data_column1 == view.getId() || R.id.stream_data_column2 == view.getId() ||
       R.id.stream_sensor_title == view.getId() || R.id.stream_sensor_rate == view.getId()) {
      this.sensorAdapter.streamRateIs(SensorManager.SENSOR_DELAY_NORMAL, -1, true);
    }
    else if(R.id.stream_button_cancel == view.getId()){
      this.sensorAdapter.streamRateIs(-1, -1, false);
    }
    else if(R.id.delay_button_normal == view.getId()){
      this.sensorAdapter.streamRateIs(SensorManager.SENSOR_DELAY_NORMAL, -1, true);
      activeDialog.cancel();
    }
    else if(R.id.delay_button_ui == view.getId()){
      this.sensorAdapter.streamRateIs(SensorManager.SENSOR_DELAY_UI, -1, true);
      activeDialog.cancel();
    }
    else if(R.id.delay_button_game == view.getId()){
      this.sensorAdapter.streamRateIs(SensorManager.SENSOR_DELAY_GAME, -1, true);
      activeDialog.cancel();
    }
    else if(R.id.delay_button_fastest == view.getId()){
      this.sensorAdapter.streamRateIs(SensorManager.SENSOR_DELAY_FASTEST, -1, true);
      activeDialog.cancel();
    }
    else if(R.id.delay_button_submit == view.getId()){
      EditText sampleField = (EditText) activeDialog.findViewById(R.id.delay_field_sample);
      EditText reportField = (EditText) activeDialog.findViewById(R.id.delay_field_report);
      int batchRate;

      try{
        batchRate = Integer.parseInt(reportField.getText().toString());
      } catch(NumberFormatException e){
        batchRate = -1;
      }

      try{
        int rate = Integer.parseInt(sampleField.getText().toString());
        SensorAdapter.isDelayButtonSubmit = true;
        this.sensorAdapter.streamRateIs(rate, batchRate, true);
      } catch(NumberFormatException e){
        SensorAdapter.isDelayButtonSubmit = false;
        Toast.makeText(TabControl.getContext(), "Invalid number entry", Toast.LENGTH_SHORT).show();
      }
      activeDialog.cancel();
    }
    else if(R.id.delay_button_cancel == view.getId()){
      this.activeDialog.cancel();
    }
    else if(R.id.delay_button_flush == view.getId()){
      this.sensorAdapter.flush(this.sensorManager);
      this.activeDialog.cancel();
    }
    else if(R.id.wear_delay_button_ui == view.getId()){
      this.sensorAdapter.streamRateIs(SensorManager.SENSOR_DELAY_UI, -1, true);
      activeDialog.cancel();
    }
    else if(R.id.wear_delay_button_fastest == view.getId()){
      this.sensorAdapter.streamRateIs(SensorManager.SENSOR_DELAY_FASTEST, -1, true);
      activeDialog.cancel();
    }
    else if(R.id.wear_delay_button_game == view.getId()){
      this.sensorAdapter.streamRateIs(SensorManager.SENSOR_DELAY_GAME, -1, true);
      activeDialog.cancel();
    }
    else if(R.id.wear_delay_button_normal == view.getId()){
      this.sensorAdapter.streamRateIs(SensorManager.SENSOR_DELAY_NORMAL, -1, true);
      activeDialog.cancel();
    }
    else if(R.id.wear_delay_button_cancel == view.getId()){
      this.activeDialog.cancel();
    }
    else if(R.id.wear_delay_button_batch == view.getId()) {
      this.activeDialog.fieldReportDilog();
    }
    else if(R.id.wear_delay_button_zero == view.getId()) {
      if (this.activeDialog.isReportText) {
          if (wearReportRateField == null) {
              wearReportRateField = "0";
          } else {
              if (wearReportRateField.length() < 10) {
                  wearReportRateField += "0";
              } else {
                  Toast.makeText(TabControl.getContext(), "Max 10 digits only", Toast.LENGTH_SHORT).show();
              }
          }
      } else {
          if (wearSampleRateField == null) {
              wearSampleRateField = "0";
          } else {
              if (wearSampleRateField.length() < 10) {
                  wearSampleRateField += "0";
              } else {
                  Toast.makeText(TabControl.getContext(), "Max 10 digits only", Toast.LENGTH_SHORT).show();
              }
          }
      }
   }
   else if(R.id.wear_delay_button_one == view.getId()) {
      if (this.activeDialog.isReportText) {
          if (wearReportRateField == null) {
              wearReportRateField = "1";
          } else {
              if (wearReportRateField.length() < 10) {
                  wearReportRateField += "1";
              } else {
                  Toast.makeText(TabControl.getContext(), "Max 10 digits only", Toast.LENGTH_SHORT).show();
              }
          }
      } else {
          if (wearSampleRateField == null) {
              wearSampleRateField = "1";
          } else {
              if (wearSampleRateField.length() < 10) {
                  wearSampleRateField += "1";
              } else {
                  Toast.makeText(TabControl.getContext(), "Max 10 digits only", Toast.LENGTH_SHORT).show();
              }
          }
      }
   }
   else if(R.id.wear_delay_button_two == view.getId()) {
      if (this.activeDialog.isReportText) {
          if (wearReportRateField == null) {
              wearReportRateField = "2";
          } else {
              if (wearReportRateField.length() < 10) {
                  wearReportRateField += "2";
              } else {
                  Toast.makeText(TabControl.getContext(), "Max 10 digits only", Toast.LENGTH_SHORT).show();
              }
          }
      } else {
          if (wearSampleRateField == null) {
              wearSampleRateField = "2";
          } else {
              if (wearSampleRateField.length() < 10) {
                  wearSampleRateField += "2";
              } else {
                  Toast.makeText(TabControl.getContext(), "Max 10 digits only", Toast.LENGTH_SHORT).show();
              }
          }
      }
   }
   else if(R.id.wear_delay_button_three == view.getId()) {
      if (this.activeDialog.isReportText) {
          if (wearReportRateField == null){
              wearReportRateField = "3";
          } else {
              if (wearReportRateField.length() < 10) {
                  wearReportRateField += "3";
              } else {
                  Toast.makeText(TabControl.getContext(), "Max 10 digits only", Toast.LENGTH_SHORT).show();
              }
          }
      } else {
          if (wearSampleRateField == null) {
              wearSampleRateField = "3";
          } else {
              if (wearSampleRateField.length() < 10) {
                  wearSampleRateField += "3";
              } else {
                  Toast.makeText(TabControl.getContext(), "Max 10 digits only", Toast.LENGTH_SHORT).show();
              }
          }
      }
   }
   else if(R.id.wear_delay_button_four == view.getId()) {
      if (this.activeDialog.isReportText) {
          if (wearReportRateField == null) {
              wearReportRateField = "4";
          } else {
              if (wearReportRateField.length() < 10) {
                  wearReportRateField += "4";
              } else {
                  Toast.makeText(TabControl.getContext(), "Max 10 digits only", Toast.LENGTH_SHORT).show();
              }
          }
      } else {
          if (wearSampleRateField == null) {
              wearSampleRateField = "4";
          } else {
              if (wearSampleRateField.length() < 10) {
                  wearSampleRateField += "4";
              } else {
                  Toast.makeText(TabControl.getContext(), "Max 10 digits only", Toast.LENGTH_SHORT).show();
              }
          }
      }
   }
   else if(R.id.wear_delay_button_five == view.getId()) {
      if (this.activeDialog.isReportText) {
          if (wearReportRateField == null) {
              wearReportRateField = "5";
          } else {
              if (wearReportRateField.length() < 10) {
                  wearReportRateField += "5";
              } else {
                  Toast.makeText(TabControl.getContext(), "Max 10 digits only", Toast.LENGTH_SHORT).show();
              }
          }
      } else {
          if (wearSampleRateField == null) {
              wearSampleRateField = "5";
          } else {
              if (wearSampleRateField.length() < 10) {
                  wearSampleRateField += "5";
              } else {
                  Toast.makeText(TabControl.getContext(), "Max 10 digits only", Toast.LENGTH_SHORT).show();
              }
          }
      }
   }
   else if(R.id.wear_delay_button_six == view.getId()) {
      if (this.activeDialog.isReportText) {
          if (wearReportRateField == null) {
              wearReportRateField = "6";
          } else {
              if (wearReportRateField.length() < 10) {
                  wearReportRateField += "6";
              } else {
                  Toast.makeText(TabControl.getContext(), "Max 10 digits only", Toast.LENGTH_SHORT).show();
              }
          }
      } else {
          if (wearSampleRateField == null) {
              wearSampleRateField = "6";
          } else {
              if (wearSampleRateField.length() < 10) {
                  wearSampleRateField += "6";
              } else {
                  Toast.makeText(TabControl.getContext(), "Max 10 digits only", Toast.LENGTH_SHORT).show();
              }
          }
      }
   }
   else if(R.id.wear_delay_button_seven == view.getId()) {
      if (this.activeDialog.isReportText) {
          if (wearReportRateField == null) {
              wearReportRateField = "7";
          } else {
              if (wearReportRateField.length() < 10) {
                  wearReportRateField += "7";
              } else {
                  Toast.makeText(TabControl.getContext(), "Max 10 digits only", Toast.LENGTH_SHORT).show();
              }
          }
      } else {
          if (wearSampleRateField == null) {
              wearSampleRateField = "7";
          } else {
              if (wearSampleRateField.length() < 10) {
                  wearSampleRateField += "7";
              } else {
                  Toast.makeText(TabControl.getContext(), "Max 10 digits only", Toast.LENGTH_SHORT).show();
              }
          }
      }
   }
   else if(R.id.wear_delay_button_eight == view.getId()) {
      if (this.activeDialog.isReportText) {
          if (wearReportRateField == null) {
              wearReportRateField = "8";
          } else {
              if (wearReportRateField.length() < 10) {
                  wearReportRateField += "8";
              } else {
                  Toast.makeText(TabControl.getContext(), "Max 10 digits only", Toast.LENGTH_SHORT).show();
              }
          }
      } else {
          if (wearSampleRateField == null) {
              wearSampleRateField = "8";
          } else {
              if (wearSampleRateField.length() < 10) {
                  wearSampleRateField += "8";
              } else {
                  Toast.makeText(TabControl.getContext(), "Max 10 digits only", Toast.LENGTH_SHORT).show();
              }
          }
      }
   }
   else if(R.id.wear_delay_button_nine == view.getId()) {
      if (this.activeDialog.isReportText) {
          if (wearReportRateField == null) {
              wearReportRateField = "9";
          } else {
              if (wearReportRateField.length() < 10) {
                  wearReportRateField += "9";
              } else {
                  Toast.makeText(TabControl.getContext(), "Max 10 digits only", Toast.LENGTH_SHORT).show();
              }
          }
      } else {
          if (wearSampleRateField == null) {
              wearSampleRateField = "9";
          } else {
              if (wearSampleRateField.length() < 10) {
                  wearSampleRateField += "9";
              } else {
                  Toast.makeText(TabControl.getContext(), "Max 10 digits only", Toast.LENGTH_SHORT).show();
              }
          }
      }
   }
   else if(R.id.wear_delay_button_delet == view.getId()) {
      if (this.activeDialog.isReportText) {
          if (wearReportRateField != null) {
              if (wearReportRateField.length() != 0) {
                  wearReportRateField = wearReportRateField.substring(0,wearReportRateField.length()-1);
              }
          }
      } else {
          if (wearSampleRateField != null) {
              if (wearSampleRateField.length() != 0) {
                  wearSampleRateField = wearSampleRateField.substring(0,wearSampleRateField.length()-1);
              }
          }
      }
   }
   else if(R.id.wear_delay_button_submit == view.getId()) {
      try {
      int batchRate = Integer.parseInt(wearReportRateField);
      int sampleRate = Integer.parseInt(wearSampleRateField);
      wearReportRateField = "";
      wearSampleRateField = "";
      Log.d ("WearQSTA"," batchRate = " +batchRate +" sampleRate = " +sampleRate);
      this.sensorAdapter.streamRateIs(sampleRate, batchRate, true);
      } catch(NumberFormatException e){
        Toast.makeText(TabControl.getContext(), "Invalid number entry", Toast.LENGTH_SHORT).show();
      }
      activeDialog.cancel();
    }
    if (this.activeDialog != null && this.activeDialog.wearReportRateField != null) {
        if (this.activeDialog.isReportText) {
            this.activeDialog.wearReportRateField.setTextColor(color.RED);
            this.activeDialog.wearStreamRateField.setTextColor(color.GREEN);
        } else {
            this.activeDialog.wearReportRateField.setTextColor(color.GREEN);
            this.activeDialog.wearStreamRateField.setTextColor(color.RED);
        }
        this.activeDialog.wearReportRateField.setText(wearReportRateField);
        this.activeDialog.wearStreamRateField.setText(wearSampleRateField);
    }
  }
}
