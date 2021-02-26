/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.threedr;

public class VOConfig {
    public boolean BATTERY_OVERRIDE = false;
    public boolean USE_LIVE_TRACKING     = !false;
    public boolean USE_MOTION_SENSOR     = !false;
    public boolean USE_HR_IMG            = !false;
    public boolean USE_AUTO_HR_IMG       = !false;
    public boolean USE_MOTION_CONSTRAINT = !false;
    public boolean USE_PREV_FRAME_KF     = false;
    public boolean IS_DUMP_IMGS          = false;
    public boolean USE_TSDF              = false;
    public boolean USE_TUTORIAL_SCREENS  = !false;
    public boolean USE_DEVELOPER_MODE    = false;
    public boolean USE_ENABLE_GUIDANCE = true;

    public int     hrImgRes[]          = new int[2];

    public VOConfig()
    {
        hrImgRes[0] = 1280;
        hrImgRes[1] = 720;
    }

    public boolean[] ToArray()
    {
        boolean[] result = new boolean[12];
        result[VO_Configuration.ConfigName.BATTERY_OVERRIDE] = BATTERY_OVERRIDE;
        result[VO_Configuration.ConfigName.USE_LIVE_TRACKING] = USE_LIVE_TRACKING;
        result[VO_Configuration.ConfigName.USE_MOTION_SENSOR] = USE_MOTION_SENSOR;
        result[VO_Configuration.ConfigName.USE_HR_IMG] = USE_HR_IMG;
        result[VO_Configuration.ConfigName.USE_AUTO_HR_IMG] = USE_AUTO_HR_IMG;
        result[VO_Configuration.ConfigName.USE_MOTION_CONSTRAINT] = USE_MOTION_CONSTRAINT;
        result[VO_Configuration.ConfigName.USE_PREV_FRAME_KF] = USE_PREV_FRAME_KF;
        result[VO_Configuration.ConfigName.IS_DUMP_IMGS] = IS_DUMP_IMGS;
        result[VO_Configuration.ConfigName.USE_TSDF] = USE_TSDF;
        result[VO_Configuration.ConfigName.USE_TUTORIAL_SCREENS] = USE_TUTORIAL_SCREENS;
        result[VO_Configuration.ConfigName.USE_DEVELOPER_MODE] = USE_DEVELOPER_MODE;
        result[VO_Configuration.ConfigName.USE_ENABLE_GUIDANCE] = USE_ENABLE_GUIDANCE;
        return result;
    }
}

