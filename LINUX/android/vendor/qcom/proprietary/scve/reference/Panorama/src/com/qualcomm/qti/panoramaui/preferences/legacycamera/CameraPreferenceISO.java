/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.panoramaui.preferences.legacycamera;

import android.content.Context;
import android.hardware.Camera.Parameters;
import android.util.Log;

import com.qualcomm.qti.panoramaui.R;
import com.qualcomm.qti.panoramaui.preferences.CameraPreference;
import com.qualcomm.qti.panoramaui.preferences.CameraPreferenceFromResources;

/**
 * This class implements a {@link CameraPreference} to control exposure compensation
 */
@SuppressWarnings("deprecation")
public class CameraPreferenceISO extends CameraPreferenceFromResources
        implements LegacyCameraPreference {

    private static final String TAG = "CameraPreferenceISO";
    private static final String CAMERA_PARAMETERS_KEY_ISO = "iso";
    private static final String CAMERA_PARAMETERS_KEY_ISO_SUPPORTED = "iso-values";

    private final Parameters mParameters;

    /**
     * Construct a {@link CameraPreferenceISO}
     * @see CameraPreference#CameraPreference(Context, int, Parameters)
     */
    public CameraPreferenceISO(Context context, int cameraId, Parameters p) {
        super(context, cameraId, R.string.camera_settings_iso,
                R.array.camera_settings_iso_entries,
                R.array.camera_settings_iso_values,
                R.string.camera_settings_iso_default);
        mParameters = p;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected String getBaseKey() {
        return "camerapref_iso";
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected String[] getSupportedValues() {
        String supportedValues = mParameters.get(CAMERA_PARAMETERS_KEY_ISO_SUPPORTED);
        return (supportedValues != null ? supportedValues.split(",") : null);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void apply() {
        String val = getValue();
        mParameters.set(CAMERA_PARAMETERS_KEY_ISO, val);
        Log.i(TAG, "Set ISO to [" + val + "]");
    }
}
