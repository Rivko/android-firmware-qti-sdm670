/*============================================================================
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  @file AccelCalibrationJNI.java
  @brief
   AccelCalibrationJNI class implementation
============================================================================*/
package com.qualcomm.qti.usta.core;

import android.os.Build;

public class AccelCalibrationJNI {
  private static native void startCalibration();
  private static native String getAccelCalFromNative();
  private static native void stopCalibration();

  public static void startCalibrationWrapper() {
    if (!Build.MODEL.contains("Android SDK")) {
      startCalibration();
    }
    else{
      USTALog.i("startCalibrationWrapper - Stub");
    }
  }
  public static void stopCalibrationWrapper(){
    if (!Build.MODEL.contains("Android SDK")) {
      stopCalibration();
    }
    else{
      USTALog.i("stopCalibrationWrapper - Stub");
    }
  }

  public static String getAccelCalFromNativeWrapper() {
    if (!Build.MODEL.contains("Android SDK")) {
      return getAccelCalFromNative();
    } else {
      return "getAccelCalFromNativeWrapper - TestStub ";
    }
  }
}
