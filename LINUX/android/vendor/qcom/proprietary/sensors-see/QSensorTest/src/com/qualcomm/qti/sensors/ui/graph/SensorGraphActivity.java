/*============================================================================
@file SensorGraphActivity.java

@brief
Activity used to graph sensor data and provide multiple-client scenarios.

Copyright (c) 2013-2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/
package com.qualcomm.qti.sensors.ui.graph;

import java.util.Hashtable;
import java.util.Timer;
import java.util.TimerTask;

import android.app.AlertDialog;
import android.app.Fragment;
import android.content.DialogInterface;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.TabHost;
import android.widget.Toast;

import com.qualcomm.qti.sensors.core.stream.SensorAdapter;
import com.qualcomm.qti.sensors.ui.qsensortest.TabControl;
import com.qualcomm.qti.sensors.qsensortest.R;

public class SensorGraphActivity extends Fragment implements OnClickListener, android.content.DialogInterface.OnClickListener {
  static final String TAG = "SensorGraph";
  static private final int REDRAW_TIMER_MS = 100;
  static private Hashtable<String, GraphAdapter> graphAdapters;
  static public HandlerThread handlerThread;
  static {
    SensorGraphActivity.graphAdapters = new Hashtable<String, GraphAdapter>();
    SensorGraphActivity.handlerThread = new HandlerThread("graphThread");
    SensorGraphActivity.handlerThread.start();
  }

  private GraphDialog activeDialog;
  private Handler drawHandler;
  private Timer redrawTimer;
  private View view;
  private TabHost tabHost;

  public SensorGraphActivity() {
    this.redrawTimer = null;
    this.view = null;

    this.drawHandler = new Handler() {
      public void handleMessage(Message msg) {
        String tabTitle = SensorGraphActivity.this.tabHost.getCurrentTabTag();
        for(GraphAdapter graphAdapter : SensorGraphActivity.graphAdapters.values())
          if(graphAdapter.tabTitle().equals(tabTitle))
            graphAdapter.graphView().invalidate();
      }
    };
  }

  @Override
  public void onActivityCreated(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
  }

  @Override
  public void onSaveInstanceState(Bundle state) {
    super.onSaveInstanceState(state);
    //state.putSerializable("sensorEvents", sensorEvents);
  }

  @Override
  public View onCreateView (LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState){
    if(null == this.view)
      this.view = inflater.inflate(R.layout.activity_graph_layout, null);

    this.tabHost = (TabHost)this.view.findViewById(R.id.graph_tabhost);
    this.tabHost.setup();

    this.setHasOptionsMenu(true);
    return this.view;
  }

  @Override
  public void onClick(View view) {
    if(R.id.delay_button_submit == view.getId()){
      Spinner sensorSpinner = (Spinner) this.activeDialog.findViewById(R.id.sensor_spinner);
      SensorAdapter sensor = (SensorAdapter)sensorSpinner.getSelectedItem();
      GraphAdapter graphAdapter = new GraphAdapter(TabControl.getContext());

      SensorGraphActivity.graphAdapters.put(graphAdapter.tabTitle(), graphAdapter);
      this.tabHost.setup();
      this.tabHost.addTab(tabHost.newTabSpec(graphAdapter.tabTitle())
          .setIndicator(graphAdapter.tabTitle(), null)
          .setContent(graphAdapter));

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

        graphAdapter.streamRateIs(sensor, rate, batchRate);
        this.activeDialog.cancel();
      } catch(NumberFormatException e){
        Toast.makeText(TabControl.getContext(), "Invalid number entry", Toast.LENGTH_LONG).show();
      }
    }
    else if(R.id.delay_button_cancel == view.getId()){
      this.activeDialog.cancel();
    }
  }

  @Override
  public void onCreateOptionsMenu (Menu menu, MenuInflater inflater) {
    inflater.inflate(R.menu.graph_action_menu, menu);
  }

  @Override
  public void onResume() {
    super.onResume();

    if(null != this.redrawTimer) {
      this.redrawTimer.cancel();
      this.redrawTimer = null;
    }
    this.startRedrawTimer();
  }

  private class RedrawTask extends TimerTask {
    @Override
    public void run() {
      SensorGraphActivity.this.drawHandler.obtainMessage(1).sendToTarget();
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

  @Override
  public boolean onOptionsItemSelected(MenuItem item) {
    switch (item.getItemId()) {
    case R.id.set_graph_stream:
      this.activeDialog = new GraphDialog(this, -1, -1);
      this.activeDialog.show();
      return true;
    case R.id.close_graph_tab:
      AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
      builder.setMessage("Are you sure you wish to cancel this stream?")
             .setTitle("Cancel?")
             .setPositiveButton("Yes, close tab", this)
             .setNegativeButton("No", this);

      AlertDialog dialog = builder.create();
      dialog.show();
      return true;
    default:
      return super.onOptionsItemSelected(item);
    }
  }

  @Override
  public void onClick(DialogInterface arg0, int arg1) {
    if(DialogInterface.BUTTON_POSITIVE == arg1 && null != this.tabHost.getCurrentTabTag()) {
      GraphAdapter graphAdapter = SensorGraphActivity.graphAdapters.remove(this.tabHost.getCurrentTabTag());
      if(null != graphAdapter)
      {
        graphAdapter.cancel();
        this.tabHost.clearAllTabs();

        for(GraphAdapter adapterTemp : SensorGraphActivity.this.graphAdapters.values()) {
          this.tabHost.setup();
          this.tabHost.addTab(tabHost.newTabSpec(adapterTemp.tabTitle())
              .setIndicator(adapterTemp.tabTitle(), null)
              .setContent(adapterTemp));
        }
      }
    }
    else if(DialogInterface.BUTTON_NEGATIVE == arg1) { }
  }
}
