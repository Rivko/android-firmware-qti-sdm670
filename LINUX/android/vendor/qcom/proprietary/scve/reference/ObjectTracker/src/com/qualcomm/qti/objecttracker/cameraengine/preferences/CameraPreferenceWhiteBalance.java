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
 * This class implements a {@link CameraPreference} to control white balance
 */
@SuppressWarnings("deprecation")
/*package*/ class CameraPreferenceWhiteBalance extends CameraPreferenceFromResources {

    private static final String TAG = "CameraPreferenceWhiteBalance";

    /**
     * Construct a {@link CameraPreferenceWhiteBalance}
     * @see CameraPreference#CameraPreference(Context, int, Parameters)
     */
    public CameraPreferenceWhiteBalance(Context context, int cameraId, Parameters p) {
        super(context, cameraId, p, R.string.camera_settings_white_balance,
                R.array.camera_settings_white_balance_entries,
                R.array.camera_settings_white_balance_values);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected String getBaseKey() {
        return "camerapref_white_balance";
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected String[] getSupportedValues(Parameters p) {
        List<String> supported = p.getSupportedWhiteBalance();
        String[] s = new String[supported.size()];
        return supported.toArray(s);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void doApply(String val, Parameters p) {
        p.setWhiteBalance(val);
        Log.i(TAG, "Set white balance to [" + val + "]");
    }
}
