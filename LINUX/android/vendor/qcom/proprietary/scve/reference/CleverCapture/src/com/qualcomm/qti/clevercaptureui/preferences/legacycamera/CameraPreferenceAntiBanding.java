/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.clevercaptureui.preferences.legacycamera;

import java.util.List;

import com.qualcomm.qti.clevercaptureui.R;
import com.qualcomm.qti.clevercaptureui.preferences.CameraPreferenceFromResources;

import android.content.Context;
import android.hardware.Camera.Parameters;
import android.util.Log;

/**
 * This class implements a {@link CameraPreference} to control anti banding
 */
@SuppressWarnings("deprecation")
public class CameraPreferenceAntiBanding extends CameraPreferenceFromResources
        implements LegacyCameraPreference {

    private static final String TAG = "CameraPreferenceAntiBanding";

    private final Parameters mParameters;

    /**
     * Construct a {@link CameraPreferenceAntiBanding}
     * @see CameraPreference#CameraPreference(Context, int, Parameters)
     */
    public CameraPreferenceAntiBanding(Context context, int cameraId, Parameters p) {
        super(context, cameraId, R.string.camera_settings_antibanding,
                R.array.camera_settings_antibanding_entries,
                R.array.camera_settings_antibanding_values);
        mParameters = p;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected String getBaseKey() {
        return "ccpref_antibanding";
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected String[] getSupportedValues() {
        List<String> supported = mParameters.getSupportedAntibanding();
        String[] s = new String[supported.size()];
        return supported.toArray(s);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void apply() {
        String val = getValue();
        mParameters.setAntibanding(val);
        Log.i(TAG, "Set anti banding to [" + val + "]");
    }
}
