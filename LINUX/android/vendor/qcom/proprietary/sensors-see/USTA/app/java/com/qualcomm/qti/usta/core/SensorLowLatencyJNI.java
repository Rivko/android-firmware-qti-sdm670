/*============================================================================
  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  @file SensorLowLatencyJNI.java
  @brief
   SensorLowLatencyJNI class implementation
============================================================================*/
package com.qualcomm.qti.usta.core;

import android.os.Build;

public class SensorLowLatencyJNI {
  private static native int getchannel(int bufferSize);
  private static native void start(long[] sensorSuid, int samplePeriodUs, int flags, int streamHandle);
  private static native void stop(int streamHandle);
  private static native void closechannel(int streamHandle);

  private static native int getmaxrate(long[] sensorSuid);

  private static final int defaultCircBufferSize = 0x1000;
  public static final int defaultSamplePeriodUs = 2500;

  private static int supportedRates;
  private static final int SNS_DIRECT_RATE_NORMAL_SAMPLE_US    = 20000;
  private static final int SNS_DIRECT_RATE_FAST_SAMPLE_US      = 5000;
  private static final int SNS_DIRECT_RATE_VERY_FAST_SAMPLE_US = 1250;

  private static final int RATE_STOP = 0;
  private static final int RATE_NORMAL = 1;
  private static final int RATE_FAST = 2;
  private static final int RATE_VERY_FAST = 3;
  private static final float RATE_NORMAL_NOMINAL = 50;
  private static final float RATE_FAST_NOMINAL = 200;
  private static final float RATE_VERY_FAST_NOMINAL = 800;

  public static int lowLatEnable(long[] sensorSuid, int samplePeriodUs, int flags) {
    int streamHandle = -1;
    if (!Build.MODEL.contains("Android SDK")) {
      streamHandle = lowLatGetChannel(defaultCircBufferSize);
      lowLatStart(streamHandle, sensorSuid, samplePeriodUs, flags);
    } else {
      USTALog.i("SensorLowLatencyJNI - lowLatEnable");
    }
    return streamHandle;
  }

  public static void lowLatDisable(int streamHandle) {
    if (!Build.MODEL.contains("Android SDK")) {
      try {
        lowLatStop(streamHandle);
      } catch (IllegalArgumentException e) {
        USTALog.e(e.getMessage());
        e.printStackTrace();
      } catch (IllegalStateException e) {
        USTALog.e(e.getMessage());
        e.printStackTrace();
      }
      try {
        lowLatCloseChannel(streamHandle);
      } catch (IllegalArgumentException e) {
        USTALog.e(e.getMessage());
        e.printStackTrace();
      } catch (IllegalStateException e) {
        USTALog.e(e.getMessage());
        e.printStackTrace();
      }
    } else {
      USTALog.i("SensorLowLatencyJNI - lowLatDisable");
    }
  }

  public static int lowLatGetChannel(int bufferSize) {
    int streamHandle = -1;
    if (!Build.MODEL.contains("Android SDK")) {
      try {
        streamHandle = getchannel(bufferSize);
        USTALog.i("SensorLowLatencyJNI - lowLatInit");
      } catch (IllegalArgumentException e) {
        USTALog.e(e.getMessage());
        e.printStackTrace();
      } catch (IllegalStateException e) {
        USTALog.e(e.getMessage());
        e.printStackTrace();
      } catch (NullPointerException e) {
        USTALog.e(e.getMessage());
        e.printStackTrace();
      }
    } else {
      USTALog.i("SensorLowLatencyJNI - lowLatInit");
    }
    return streamHandle;
  }

  public static void lowLatStart(int streamHandle, long[] sensorSuid, int samplePeriodUs, int flags) {
    if (!Build.MODEL.contains("Android SDK")) {
      try {
        start(sensorSuid, samplePeriodUs, flags, streamHandle);
        USTALog.i("SensorLowLatencyJNI - lowLatStart with handle " + streamHandle);
        USTALog.i("SensorLowLatencyJNI - lowLatStart with suid low "
                          + sensorSuid[0]+ "suid high" + sensorSuid[1]);
        USTALog.i("SensorLowLatencyJNI - lowLatStart with samplePeriodUs " + samplePeriodUs);
        USTALog.i("SensorLowLatencyJNI - lowLatStart with flags " + flags);
      } catch (IllegalArgumentException e) {
        USTALog.e(e.getMessage());
        e.printStackTrace();
      } catch (IllegalStateException e) {
        USTALog.e(e.getMessage());
        e.printStackTrace();
      }
    } else {
      USTALog.i("SensorLowLatencyJNI - lowLatStart");
    }
  }

  public static void lowLatStop(int streamHandle) {
    if (!Build.MODEL.contains("Android SDK")) {
      stop(streamHandle);
      USTALog.i("SensorLowLatencyJNI - lowLatStop");
    } else {
      USTALog.i("SensorLowLatencyJNI - lowLatStop");
    }
  }

  public static void lowLatCloseChannel(int streamHandle) {
    if (!Build.MODEL.contains("Android SDK")) {
      closechannel(streamHandle);
      USTALog.i("SensorLowLatencyJNI - lowLatCloseChannel");
    } else {
      USTALog.i("SensorLowLatencyJNI - lowLatCloseChannel");
    }
  }

  public static int lowLatChannelSampleRate(int rate_level) {
    int sample_period_us = -1;
    switch(rate_level) {
      case RATE_NORMAL:
        sample_period_us = SNS_DIRECT_RATE_NORMAL_SAMPLE_US;
        break;
      case RATE_FAST:
        sample_period_us = SNS_DIRECT_RATE_FAST_SAMPLE_US;
        break;
      case RATE_VERY_FAST:
        sample_period_us = SNS_DIRECT_RATE_VERY_FAST_SAMPLE_US;
        break;
      case RATE_STOP:
        sample_period_us = 0;
      default:
        USTALog.e("SensorLowLatencyJNI - unsupported sample rate");
        break;
    }
    USTALog.i("lowLatChannelSampleRate samplerate selected " + sample_period_us);
    return sample_period_us;
  }

  public static int lowLatGetMaxRate(long[] suid) {
    int low_lat_rates =  getmaxrate(suid);
    if ( RATE_VERY_FAST_NOMINAL < low_lat_rates )
      return RATE_VERY_FAST;
    else if ( RATE_FAST_NOMINAL < low_lat_rates )
      return RATE_FAST;
    else if ( RATE_NORMAL_NOMINAL < low_lat_rates )
      return RATE_NORMAL;
    else
      return RATE_STOP;
  }

}
