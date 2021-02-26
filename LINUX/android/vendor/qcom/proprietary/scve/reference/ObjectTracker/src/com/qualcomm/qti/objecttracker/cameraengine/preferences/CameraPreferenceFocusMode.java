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
 * This class implements a {@link CameraPreference} to control focus mode
 */
@SuppressWarnings("deprecation")
/*package*/ class CameraPreferenceFocusMode extends CameraPreferenceFromResources {

    private static final String TAG = "CameraPreferenceFocusMode";

    /**
     * Construct a {@link CameraPreferenceFocusMode}
     * @see CameraPreference#CameraPreference(Context, int, Parameters)
     */
    public CameraPreferenceFocusMode(Context context, int cameraId, Parameters p) {
        super(context, cameraId, p, R.string.camera_settings_focus_mode,
                R.array.camera_settings_focus_mode_entries,
                R.array.camera_settings_focus_mode_values);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected String getBaseKey() {
        return "camerapref_focus_mode";
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected String[] getSupportedValues(Parameters p) {
        List<String> supported = p.getSupportedFocusModes();
        String[] s = new String[supported.size()];
        return supported.toArray(s);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void doApply(String val, Parameters p) {
        p.setFocusMode(val);
        Log.i(TAG, "Set focus mode to [" + val + "]");
    }
}
