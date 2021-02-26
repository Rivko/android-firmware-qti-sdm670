/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.threedr;

import android.content.Context;
import android.content.SharedPreferences;
import android.preference.PreferenceManager;

public class Preferences {

    public static VOConfig getVOConfig(Context context) {
        VOConfig voConfig = new VOConfig();
        SharedPreferences sharedPreferences = PreferenceManager.getDefaultSharedPreferences(context);

        //live tracking is inverted to "run offline mode" in settings screen so inverting its value here
        voConfig.BATTERY_OVERRIDE = sharedPreferences.getBoolean(context.getString(R.string.prefs_batteryOverride), voConfig.BATTERY_OVERRIDE);
        voConfig.USE_LIVE_TRACKING = !sharedPreferences.getBoolean(context.getString(R.string.prefs_liveTracking), !voConfig.USE_LIVE_TRACKING);
        voConfig.USE_MOTION_SENSOR = sharedPreferences.getBoolean(context.getString(R.string.prefs_motionSensor), voConfig.USE_MOTION_SENSOR);
        voConfig.USE_HR_IMG = sharedPreferences.getBoolean(context.getString(R.string.prefs_hires), voConfig.USE_HR_IMG);
        voConfig.USE_AUTO_HR_IMG = sharedPreferences.getBoolean(context.getString(R.string.prefs_autoHrImage), voConfig.USE_AUTO_HR_IMG);
        voConfig.USE_MOTION_CONSTRAINT = sharedPreferences.getBoolean(context.getString(R.string.prefs_motionConstraint), voConfig.USE_MOTION_CONSTRAINT);
        voConfig.USE_PREV_FRAME_KF = sharedPreferences.getBoolean(context.getString(R.string.prefs_previousFrameKF), voConfig.USE_PREV_FRAME_KF);
        voConfig.IS_DUMP_IMGS = sharedPreferences.getBoolean(context.getString(R.string.prefs_dumpImages), voConfig.IS_DUMP_IMGS);
        voConfig.USE_TSDF = sharedPreferences.getBoolean(context.getString(R.string.prefs_tsdf), voConfig.USE_TSDF);
        voConfig.USE_TUTORIAL_SCREENS = sharedPreferences.getBoolean(context.getString(R.string.prefs_tutorialScreens), voConfig.USE_TUTORIAL_SCREENS);
        voConfig.USE_DEVELOPER_MODE = sharedPreferences.getBoolean(context.getString(R.string.prefs_developerMode), voConfig.USE_DEVELOPER_MODE);
        voConfig.USE_ENABLE_GUIDANCE = sharedPreferences.getBoolean(context.getString(R.string.prefs_enableGuidance), voConfig.USE_ENABLE_GUIDANCE);
        return voConfig;
    }

    public static int getPrefCannedDatasetType(Context context) {
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(context);
        return Integer.parseInt(prefs.getString(context.getResources().getString(R.string.canned_dataset_type), "0"));
    }
}
