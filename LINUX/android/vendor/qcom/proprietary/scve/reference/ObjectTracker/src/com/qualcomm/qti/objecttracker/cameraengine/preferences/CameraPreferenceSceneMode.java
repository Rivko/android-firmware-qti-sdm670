/*
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.objecttracker.cameraengine.preferences;

import java.util.List;

import com.qualcomm.qti.objecttrackerdemo.R;

import android.content.Context;
import android.hardware.Camera.Parameters;
import android.util.Log;

/**
 * This class implements a {@link CameraPreference} to control exposure compensation
 */
@SuppressWarnings("deprecation")
/*package*/ class CameraPreferenceSceneMode extends CameraPreferenceFromResources {

    private static final String TAG = "CameraPreferenceSceneMode";

    /**
     * Construct a {@link CameraPreferenceSceneMode}
     * @see CameraPreference#CameraPreference(Context, int, Parameters)
     */
    public CameraPreferenceSceneMode(Context context, int cameraId, Parameters p) {
        super(context, cameraId, p, R.string.camera_settings_scene_mode,
                R.array.camera_settings_scene_mode_entries,
                R.array.camera_settings_scene_mode_values);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected String getBaseKey() {
        return "camerapref_scene_mode";
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected String[] getSupportedValues(Parameters p) {
        List<String> supported = p.getSupportedSceneModes();
        if (supported == null) {
            return null;
        }

        String[] s = new String[supported.size()];
        return supported.toArray(s);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void doApply(String val, Parameters p) {
        p.setSceneMode(val);
        Log.i(TAG, "Set scene mode to [" + val + "]");
    }
}
