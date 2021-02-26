/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.threedr;

import android.app.Activity;

public class VO_Configuration extends Activity {
  public final static String Extra_hrImgRes          = "com.qti.qualcomm.vointerface.hrImgRes";
  public final static String Extra_vo_config         = "com.qti.qualcomm.vointerface.vo_config";
  public static class ConfigName {
    public static final int USE_LIVE_TRACKING     = 0;
    public static final int USE_MOTION_SENSOR     = 1;
    public static final int USE_HR_IMG            = 2;
    public static final int USE_AUTO_HR_IMG       = 3;
    public static final int USE_MOTION_CONSTRAINT = 4;
    public static final int USE_PREV_FRAME_KF     = 5;
    public static final int IS_DUMP_IMGS          = 6;
    public static final int USE_TSDF              = 7;
    public static final int USE_TUTORIAL_SCREENS  = 8;
    public static final int USE_DEVELOPER_MODE    = 9;
    public static final int USE_ENABLE_GUIDANCE   = 10;
    public static final int BATTERY_OVERRIDE   = 11;

  }
  private boolean vo_config[]         = new boolean[12];
  private int     hrImgRes[]          = new int[2];

}
