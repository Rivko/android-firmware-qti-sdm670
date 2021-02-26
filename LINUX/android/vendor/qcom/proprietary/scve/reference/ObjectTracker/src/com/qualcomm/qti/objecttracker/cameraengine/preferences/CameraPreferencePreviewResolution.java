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
import android.hardware.Camera.Size;
import android.util.Log;

/**
 * This class implements a {@link CameraPreference} to control preview resolution
 */
@SuppressWarnings("deprecation")
/*package*/ class CameraPreferencePreviewResolution extends CameraPreferenceFromResources {

    private static final String TAG = "CameraPreferencePreviewResolution";

    /**
     * Construct a {@link CameraPreferencePreviewResolution}
     * @see CameraPreference#CameraPreference(Context, int, Parameters)
     */
    public CameraPreferencePreviewResolution(Context context, int cameraId, Parameters p) {
        super(context, cameraId, p, R.string.camera_settings_preview_resolution,
                R.array.camera_settings_preview_resolution_entries,
                R.array.camera_settings_preview_resolution_values);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected String getBaseKey() {
        return "camerapref_preview_resolution";
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected String[] getSupportedValues(Parameters p) {
        List<Size> sizes = p.getSupportedPreviewSizes();
        String[] values = new String[sizes.size()];
        int idx = 0;
        for (Size size : sizes) {
            values[idx++] = size.width + "x" + size.height;
        }
        return values;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void doApply(String val, Parameters p) {
        int x = -1;
        if (val != null && !val.isEmpty()) {
            x = val.indexOf("x");
        }

        if (x != -1) {
            int w = Integer.parseInt(val.substring(0, x));
            int h = Integer.parseInt(val.substring(x + 1));
            p.setPreviewSize(w, h);
            Log.i(TAG, "Set preview size to [" + w + "x" + h + "]");
        }
    }
}
