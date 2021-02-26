/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.clevercaptureui.preferences.legacycamera;

import java.util.List;

import android.content.Context;
import android.hardware.Camera.Parameters;
import android.hardware.Camera.Size;
import android.util.Log;

import com.qualcomm.qti.clevercaptureui.R;
import com.qualcomm.qti.clevercaptureui.preferences.CameraPreference;
import com.qualcomm.qti.clevercaptureui.preferences.CameraPreferenceFromResources;

/**
 * This class implements a {@link CameraPreference} to control preview size
 */
@SuppressWarnings("deprecation")
public class CameraPreferencePreviewSize extends CameraPreferenceFromResources
        implements LegacyCameraPreference {

    private static final String TAG = "CameraPreferencePreviewSize";

    private final Parameters mParameters;

    /**
     * Construct a {@link CameraPreferencePreviewResolution}
     * @see CameraPreference#CameraPreference(Context, int, Parameters)
     */
    public CameraPreferencePreviewSize(Context context, int cameraId, Parameters p) {
        super(context, cameraId, R.string.camera_settings_preview_size,
                R.array.camera_settings_preview_size_entries,
                R.array.camera_settings_preview_size_values);
        mParameters = p;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected String getBaseKey() {
        return "camerapref_preview_size";
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected String[] getSupportedValues() {
        List<Size> sizes = mParameters.getSupportedPreviewSizes();
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
    public void apply() {
        String val = getValue();
        int x = -1;
        if (val != null && !val.isEmpty()) {
            x = val.indexOf("x");
        }

        if (x != -1) {
            int w = Integer.parseInt(val.substring(0, x));
            int h = Integer.parseInt(val.substring(x + 1));
            mParameters.setPreviewSize(w, h);
            Log.i(TAG, "Set preview size to [" + w + "x" + h + "]");
        }
    }
}
