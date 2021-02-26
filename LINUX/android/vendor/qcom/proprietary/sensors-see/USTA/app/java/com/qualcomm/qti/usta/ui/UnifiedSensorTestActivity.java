/*============================================================================
  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  @file UnifiedSensorTestActivity.java
  @brief
   UnifiedSensorTestActivity class implementation
============================================================================*/
package com.qualcomm.qti.usta.ui;


import android.app.ActionBar;
import android.app.ActionBar.Tab;
import android.app.Activity;
import android.app.Fragment;
import android.app.FragmentTransaction;
import android.app.FragmentManager;
import android.os.Build;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Toast;
import com.qualcomm.qti.usta.R;
import com.qualcomm.qti.usta.core.USTALog;
import com.qualcomm.qti.usta.core.SensorContextJNI;
import android.content.pm.PackageManager;

public class UnifiedSensorTestActivity extends Activity {

  static public View viewForStreamingStatus;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);

    System.loadLibrary("USTANative");
    System.loadLibrary("SensorCalLibNative");
    if(getPackageManager().hasSystemFeature(PackageManager.FEATURE_WATCH) == false){
      System.loadLibrary("sensor_low_lat");
    }

    setContentView(R.layout.activity_unified_sensor_test);

    ActionBar actionbar = getActionBar();
    actionbar.setNavigationMode(ActionBar.NAVIGATION_MODE_TABS);
    actionbar.setDisplayShowTitleEnabled(true);

    Tab tab = actionbar.newTab().setText("USTA-Tab").setTabListener(new TabListener<SensorRequestFragment>(this, "USTA", SensorRequestFragment.class));
    actionbar.addTab(tab);

    tab = actionbar.newTab().setText("Registry-Tab").setTabListener(new TabListener<RegistryTab>(this, "Registry", RegistryTab.class));
    actionbar.addTab(tab);

    tab = actionbar.newTab().setText("Sensors-Info").setTabListener(new TabListener<SensorsInfoTab>(this, "SensorsInfo", SensorsInfoTab.class));
    actionbar.addTab(tab);

    tab = actionbar.newTab().setText("Cal-Tab").setTabListener(new TabListener<CalibrationTab>(this, "Calibration", CalibrationTab.class));
    actionbar.addTab(tab);

    tab = actionbar.newTab().setText("DRM-Tab").setTabListener(new TabListener<SensorLowLatency>(this, "DRM", SensorLowLatency.class));
    actionbar.addTab(tab);

  }

  @Override
  public boolean onKeyDown(int keyCode, KeyEvent event) {
    if(keyCode == KeyEvent.KEYCODE_BACK) {
      moveTaskToBack(true);
      return true;
    }
    return super.onKeyDown(keyCode, event);
  }

  @Override
  public boolean onCreateOptionsMenu(Menu menu) {
    super.onCreateOptionsMenu(menu);
    getMenuInflater().inflate(R.menu.usta_menu, menu);
    return true;
  }

  @Override
  public boolean onOptionsItemSelected(MenuItem item) {
    if(item.getItemId() == R.id.disable_logging) {
      item.setChecked(!item.isChecked());
      USTALog.isLoggingDisabled = item.isChecked();
      Toast.makeText(getApplicationContext(), (USTALog.isLoggingDisabled ? "USTA logging is disabled":"USTA logging is enabled"), Toast.LENGTH_LONG).show();
      SensorContextJNI.updateLoggingFlagWrapper();
      return true;
    }
    else if(item.getItemId() == R.id.update_screen){
      item.setChecked(!item.isChecked());
      SensorEventDisplayFragment.isDisplaySamplesEnabled = item.isChecked();
      Toast.makeText(getApplicationContext(), (item.isChecked() ? "Streaming Status is enabled":"Streaming Status is disabled"), Toast.LENGTH_SHORT).show();
      return true;
    }
    else if(item.getItemId() == R.id.disable_qmi_connection_id){
      if(item.isCheckable() == true) {
        item.setChecked(!item.isChecked());
        SensorPayloadFragment.isClientDisableRequestEnabled = item.isChecked();
        Toast.makeText(getApplicationContext(), (item.isChecked() ? "Enable qmi API is enabled" : "Enable qmi API is disabled"), Toast.LENGTH_SHORT).show();
        item.setEnabled(false);
      }
      return true;
    }
    else if(item.getItemId() == R.id.app_force_close_id){
      item.setChecked(!item.isChecked());
      this.finishAndRemoveTask();
      return true;
    }
    else{
      return super.onOptionsItemSelected(item);
    }
  }

  private static class TabListener<T extends Fragment> implements ActionBar.TabListener {
    private Fragment fragment;
    private final Activity activity;
    private final String tag;
    private final Class<T> fragmentClass;

    /** Constructor used each time a new tab is created.
     *
     * @param activity  The host Activity, used to instantiate the fragment
     * @param tag  The identifier tag for the fragment
     * @param fragmentClass  The fragment's Class, used to instantiate the fragment
     */
    public TabListener(Activity activity, String tag, Class<T> fragmentClass) {
      this.activity = activity;
      this.tag = tag;
      this.fragmentClass = fragmentClass;
    }
    @Override
    public void onTabSelected(Tab tab, FragmentTransaction ft) {

      USTALog.d(tag + " Tab is selected ");
        if (null == this.fragment) {
          this.fragment = Fragment.instantiate(this.activity, this.fragmentClass.getName());
          ft.add(android.R.id.content, this.fragment, this.tag);
        } else {
          if(tag.equals("USTA")){
            this.fragment = Fragment.instantiate(this.activity, this.fragmentClass.getName());
            ft.add(android.R.id.content, this.fragment, this.tag);
          }else
            ft.attach(this.fragment);
        }
    }
    @Override
    public void onTabUnselected(Tab tab, FragmentTransaction ft) {
      if (this.fragment != null)
        ft.detach(this.fragment);
    }
    @Override
    public void onTabReselected(Tab tab, FragmentTransaction ft) {
      USTALog.d(tag + " Tab is re-selected ");
    }
  }
}
