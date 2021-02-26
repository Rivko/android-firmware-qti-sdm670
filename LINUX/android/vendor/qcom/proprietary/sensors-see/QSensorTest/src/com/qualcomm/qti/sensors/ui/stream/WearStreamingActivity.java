/*============================================================================
@file WearStreamingActivity.java

@brief
Activity to streaming from sensors for Wearable target.

Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/

package com.qualcomm.qti.sensors.ui.stream;

import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

import android.annotation.SuppressLint;
import android.app.Fragment;
import android.app.Activity;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.WindowManager;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.EditText;
import android.widget.Toast;
import android.content.Context;
import android.util.Log;

import com.qualcomm.qti.sensors.core.qsensortest.QSensorContext;
import com.qualcomm.qti.sensors.qsensortest.R;
import com.qualcomm.qti.sensors.core.qsensortest.SettingsDatabase;
import com.qualcomm.qti.sensors.ui.qsensortest.TabControl;

import java.io.IOException;

public class WearStreamingActivity extends Activity {
  static final private String TAG = QSensorContext.TAG;
  static final private int REDRAW_TIMER_MS = 300;
  static private SensorListView sensorListView = null;
  static private boolean updateUI = true;

  private SensorDialog activeDialog;
  private Timer redrawTimer;
  private Handler drawHandler;
  public static Context context;
  public boolean EnableWakeLock;
  static public Context getContext() { return WearStreamingActivity.context; }
  public WearStreamingActivity() {
    this.redrawTimer = null;
    this.drawHandler = null;
    this.activeDialog = null;

    String updateScreenDef = SettingsDatabase.getSettings().getProperty("update_screen_def");
    WearStreamingActivity.updateUI = null != updateScreenDef && updateScreenDef.equals("1");

    this.drawHandler = new Handler() {
      public void handleMessage(Message msg) {
         try {
             EnableWakeLock = QSensorContext.readProperty("ro.vendor.sensors.wearqstp.lock").equals("1");
             if (EnableWakeLock && !QSensorContext.AcquireLock) {
                 QSensorContext.wakeLock().acquire();
                 QSensorContext.AcquireLock = true;
             } else {
                 if (!EnableWakeLock && QSensorContext.AcquireLock) {
                     QSensorContext.wakeLock().release();
                     QSensorContext.AcquireLock = false;
                 }
             }
         } catch(IOException e) {
            e.printStackTrace();
         }
        for(final SensorView sensorView : WearStreamingActivity.sensorListView.sensorList())
          sensorView.updateView();
        this.removeMessages(1);
      }
    };
  }

  /**
   * Called when the activity is first created.
   */

  @Override
  public void  onCreate ( Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
     context = this;
    if(null == WearStreamingActivity.sensorListView) {
      WearStreamingActivity.sensorListView = new SensorListView(context, QSensorContext.sensorManager());
    }
    else {
      ViewGroup parent = (ViewGroup)WearStreamingActivity.sensorListView.getParent();
      if(null != parent)
        parent.removeView(WearStreamingActivity.sensorListView);
    }
    setContentView(WearStreamingActivity.sensorListView);
    try {
      if(QSensorContext.readProperty("ro.vendor.sns.wqsta.screenallowdoze").equals("")) {
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
      }
    } catch(IOException e) {
      e.printStackTrace();
    }
  }

  @Override
  public void onDestroy() {
    super.onDestroy();
    if(null != WearStreamingActivity.sensorListView)
    {
      ViewGroup parent = (ViewGroup)WearStreamingActivity.sensorListView.getParent();
      if(null != parent)
        parent.removeView(WearStreamingActivity.sensorListView);
    }
  }

  @Override
  public void onResume() {
    super.onResume();

    if(null != this.redrawTimer) {
      this.redrawTimer.cancel();
      this.redrawTimer = null;
    }
    if(WearStreamingActivity.updateUI)
      this.startRedrawTimer();
  }

  private class RedrawTask extends TimerTask {
    @Override
    public void run() {
      if(null != WearStreamingActivity.sensorListView &&
         null != WearStreamingActivity.sensorListView.sensorList())
        WearStreamingActivity.this.drawHandler.obtainMessage(1).sendToTarget();
    }
  }

  private void startRedrawTimer() {
    this.redrawTimer = new Timer();
    this.redrawTimer.scheduleAtFixedRate(new RedrawTask(), 0, REDRAW_TIMER_MS);
  }

  @Override
  public void onPause() {
    super.onPause();

    if(null != this.redrawTimer) {
      this.redrawTimer.cancel();
      this.redrawTimer = null;
    }
  }

  protected void clearAllListeners(SensorManager sensorManager) {
    List<SensorView> sensorList = WearStreamingActivity.sensorListView.sensorList();

    for(SensorView sensorView : sensorList) {
      sensorView.sensorAdapter().streamRateIs(-1, -1, false);
      sensorView.updateView();
    }
  }

  protected void flushAllListeners(SensorManager sensorManager) {
    List<SensorView> sensorList = WearStreamingActivity.sensorListView.sensorList();

    for(SensorView sensorView : sensorList)
      sensorView.sensorAdapter().flush(sensorManager);
  }

  protected void setAllListeners(SensorManager sensorManager,
      int streamRate, int reportRate, boolean clear) {
    List<SensorView> sensorList = WearStreamingActivity.sensorListView.sensorList();

    for(SensorView sensorView : sensorList)
      sensorView.sensorAdapter().streamRateIs(streamRate, reportRate, clear);
  }

  private class AllSensorController implements OnClickListener {
    private SensorManager sensorManager;
    public AllSensorController(SensorManager sensorManager) {
      this.sensorManager = sensorManager;
    }

    @SuppressLint("NewApi")
    @Override
    public void onClick(View view) {
      if(R.id.delay_button_normal == view.getId())
        WearStreamingActivity.this.setAllListeners(sensorManager, SensorManager.SENSOR_DELAY_NORMAL, -1, true);
      else if(R.id.delay_button_ui == view.getId())
        WearStreamingActivity.this.setAllListeners(sensorManager, SensorManager.SENSOR_DELAY_UI, -1, true);
      else if(R.id.delay_button_game == view.getId())
        WearStreamingActivity.this.setAllListeners(sensorManager, SensorManager.SENSOR_DELAY_GAME, -1, true);
      else if(R.id.delay_button_fastest == view.getId())
        WearStreamingActivity.this.setAllListeners(sensorManager, SensorManager.SENSOR_DELAY_FASTEST, -1, true);
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
          WearStreamingActivity.this.setAllListeners(sensorManager, rate, batchRate, true);
        } catch(NumberFormatException e){
          Toast.makeText(TabControl.getContext(), "Invalid number entry", Toast.LENGTH_LONG).show();
        }
      }
      else if(R.id.delay_button_cancel == view.getId()){ }
      else if(R.id.delay_button_flush == view.getId()) {
        WearStreamingActivity.this.flushAllListeners(this.sensorManager);
      }
      WearStreamingActivity.this.activeDialog.cancel();
    }
  }
}
