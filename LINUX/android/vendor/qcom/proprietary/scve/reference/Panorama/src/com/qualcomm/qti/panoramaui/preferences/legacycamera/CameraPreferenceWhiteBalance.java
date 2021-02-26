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
 * This class implements a {@link CameraPreference} to control white balance
 */
@SuppressWarnings("deprecation")
public class CameraPreferenceWhiteBalance extends CameraPreferenceFromResources
        implements LegacyCameraPreference {

    private static final String TAG = "CameraPreferenceWhiteBalance";

    private final Parameters mParameters;

    /**
     * Construct a {@link CameraPreferenceWhiteBalance}
     * @see CameraPreference#CameraPreference(Context, int, Parameters)
     */
    public CameraPreferenceWhiteBalance(Context context, int cameraId, Parameters p) {
        super(context, cameraId, R.string.camera_settings_white_balance,
                R.array.camera_settings_white_balance_entries,
                R.array.camera_settings_white_balance_values);
        mParameters = p;
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
    protected String[] getSupportedValues() {
        List<String> supported = mParameters.getSupportedWhiteBalance();
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
        mParameters.setWhiteBalance(val);
        Log.i(TAG, "Set white balance to [" + val + "]");
    }
}
