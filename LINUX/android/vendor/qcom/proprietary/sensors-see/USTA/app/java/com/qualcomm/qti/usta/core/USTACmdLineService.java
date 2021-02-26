/*============================================================================
  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  @file USTACmdLineService.java
  @brief
   USTACmdLineService class implementation which extends service
============================================================================*/

package com.qualcomm.qti.usta.core;

import android.app.Notification;
import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.content.pm.PackageManager;
import com.qualcomm.qti.usta.R;
import com.qualcomm.qti.usta.ui.UnifiedSensorTestActivity;
import com.qualcomm.qti.usta.core.SensorLowLatencyJNI;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;

public class USTACmdLineService extends Service {

  private USTACmdLineReceiver USTACmdLineReceiverReceiver;
  private static SensorContext sensorContext = null;
  private static SensorLowLatencyJNI sensorDrmContext = null;

  static public SensorContext getSensorContext() {
    return sensorContext;
  }

  static public SensorLowLatencyJNI getDrmSensorContext() {
    return sensorDrmContext;
  }

  public String getLoggingStatus(){
    Process process = null;
    try {
      process = new ProcessBuilder().command("/system/bin/getprop" , "sensors.log.disable").redirectErrorStream(true).start();
    } catch (IOException e) {
      USTALog.i("sensors.log.disable was not found ");
      return null;
    }

    InputStream in = process.getInputStream();

    BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(process.getInputStream()));

    StringBuilder log = new StringBuilder();
    String line ;
    try {
      line = bufferedReader.readLine();
    } catch (IOException e) {
      return null;
    }

    process.destroy();
    return line;
  }
  @Override
  public void onCreate() {
    USTALog.i("onCreate 1 ");
    System.loadLibrary("USTANative");
    if(getPackageManager().hasSystemFeature(PackageManager.FEATURE_WATCH) == false) {
      System.loadLibrary("sensor_low_lat");
    }

    String value = getLoggingStatus();

    if(value == null ) {
      USTALog.i("value is null and isLoggingDisabled false");
      USTALog.isLoggingDisabled = false;
      SensorContextJNI.updateLoggingFlagWrapper();
    }

    if (value.length() != 0) {
      int loggingStatus = Integer.parseInt(value);

      if(loggingStatus == 0){
        USTALog.isLoggingDisabled = false;
        SensorContextJNI.updateLoggingFlagWrapper();
      }
      else if(loggingStatus == 1){
        USTALog.isLoggingDisabled = true;
        SensorContextJNI.updateLoggingFlagWrapper();
      }

    }
    sensorContext = SensorContext.getSensorContext(ModeType.USTA_MODE_TYPE_COMMAND_LINE);

    USTALog.i("create command line drm interface");
    sensorDrmContext = new SensorLowLatencyJNI();
    if (null == sensorDrmContext) {
        USTALog.e("create sensorDrmContext failed");
    }

    if (null == USTACmdLineReceiverReceiver) {

      USTACmdLineReceiverReceiver = new USTACmdLineReceiver();
      try {
        this.registerReceiver(USTACmdLineReceiverReceiver, USTACmdLineReceiverReceiver.getIntentFilter());
      } catch (Exception e) {
        USTALog.e(e.toString());
      }
    }


  }

  @Override
  public int onStartCommand(Intent intent, int flags, int startId) {

    super.onStartCommand(intent, flags, startId);

    return START_STICKY;
  }

  @Override
  public void onDestroy() {
    stopForeground(true);
    super.onDestroy();
    if (null != USTACmdLineReceiverReceiver)
      this.unregisterReceiver(USTACmdLineReceiverReceiver);
  }

  @Override
  public IBinder onBind(Intent intent) {
    // TODO: Return the communication channel to the service.
    throw new UnsupportedOperationException("Not yet implemented");
  }

}
