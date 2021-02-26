/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.panoramaui.preferences.legacycamera;

import java.util.List;

import com.qualcomm.qti.panoramaui.R;
import com.qualcomm.qti.panoramaui.preferences.CameraPreference;
import com.qualcomm.qti.panoramaui.preferences.CameraPreferenceFromResources;

import android.content.Context;
import android.hardware.Camera.Parameters;
import android.util.Log;

/**
 * This class implements a {@link CameraPreference} to control exposure compensation
 */
@SuppressWarnings("deprecation")
public class CameraPreferenceSceneMode extends CameraPreferenceFromResources
        implements LegacyCameraPreference {

    private static final String TAG = "CameraPreferenceSceneMode";

    private final Parameters mParameters;

    /**
     * Construct a {@link CameraPreferenceSceneMode}
     * @see CameraPreference#CameraPreference(Context, int, Parameters)
     */
    public CameraPreferenceSceneMode(Context context, int cameraId, Parameters p) {
        super(context, cameraId, R.string.camera_settings_scene_mode,
                R.array.camera_settings_scene_mode_entries,
                R.array.camera_settings_scene_mode_values);
        mParameters = p;
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
    protected String[] getSupportedValues() {
        List<String> supported = mParameters.getSupportedSceneModes();
        if (supported != null) {
            String[] s = new String[supported.size()];
            return supported.toArray(s);
        } else {
            return null;
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void apply() {
        String val = getValue();
        mParameters.setSceneMode(val);
        Log.i(TAG, "Set scene mode to [" + val + "]");
    }
}
