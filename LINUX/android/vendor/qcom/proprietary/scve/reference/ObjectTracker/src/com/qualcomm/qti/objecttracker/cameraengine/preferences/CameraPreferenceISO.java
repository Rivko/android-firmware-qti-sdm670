/*
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.objecttracker.cameraengine.preferences;

import com.qualcomm.qti.objecttrackerdemo.R;

import android.content.Context;
import android.hardware.Camera.Parameters;
import android.util.Log;

/**
 * This class implements a {@link CameraPreference} to control exposure compensation
 */
@SuppressWarnings("deprecation")
/*package*/ class CameraPreferenceISO extends CameraPreferenceFromResources {

    private static final String TAG = "CameraPreferenceISO";
    private static final String CAMERA_PARAMETERS_KEY_ISO = "iso";
    private static final String CAMERA_PARAMETERS_KEY_ISO_SUPPORTED = "iso-values";

    /**
     * Construct a {@link CameraPreferenceISO}
     * @see CameraPreference#CameraPreference(Context, int, Parameters)
     */
    public CameraPreferenceISO(Context context, int cameraId, Parameters p) {
        super(context, cameraId, p, R.string.camera_settings_iso,
                R.array.camera_settings_iso_entries,
                R.array.camera_settings_iso_values);
    }

    /**
     * Test if {@link CameraPreferenceISO} is supported for the specified camera
     * @param p {@link Parameters} for the current camera
     * @return true if the specified camera supports ISO settings, else false
     */
    public static boolean isSupported(Parameters p) {
        return (p.get(CAMERA_PARAMETERS_KEY_ISO_SUPPORTED) != null);
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
    protected String[] getSupportedValues(Parameters p) {
        return p.get(CAMERA_PARAMETERS_KEY_ISO_SUPPORTED).split(",");
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void doApply(String val, Parameters p) {
        p.set(CAMERA_PARAMETERS_KEY_ISO, val);
        Log.i(TAG, "Set ISO to [" + val + "]");
    }
}
