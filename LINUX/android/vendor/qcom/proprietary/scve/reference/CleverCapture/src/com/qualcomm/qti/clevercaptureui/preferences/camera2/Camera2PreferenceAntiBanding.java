/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.clevercaptureui.preferences.camera2;

import com.qualcomm.qti.clevercaptureui.R;
import com.qualcomm.qti.clevercaptureui.preferences.CameraPreferenceFromResources;

import android.annotation.TargetApi;
import android.content.Context;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CaptureRequest;
import android.os.Build;
import android.util.Log;

/**
 * This class implements a {@link CameraPreference} to control anti banding
 */
@TargetApi(Build.VERSION_CODES.LOLLIPOP)
public class Camera2PreferenceAntiBanding extends CameraPreferenceFromResources implements
        Camera2CameraPreference {
    private static final String TAG = Camera2PreferenceAntiBanding.class.getSimpleName();
    private final CameraCharacteristics characteristics;

    /**
     * Construct a {@link Camera2PreferenceAntiBanding}
     * @see CameraPreferenceFromResources#CameraPreferenceFromResources(Context, int, int, int, int)
     */
    public Camera2PreferenceAntiBanding(Context context, int cameraId,
            CameraCharacteristics characteristics) {
        super(context, cameraId, R.string.camera_settings_antibanding,
                R.array.camera_settings_antibanding_entries,
                R.array.camera_settings_antibanding_values_camera2);
        this.characteristics = characteristics;
    }

    /** {@inheritDoc} */
    @Override
    protected String getBaseKey() {
        return "ccpref_antibanding";
    }

    /** {@inheritDoc} */
    @Override
    protected String[] getSupportedValues() {
        int[] supported = characteristics.get(CameraCharacteristics.CONTROL_AE_AVAILABLE_ANTIBANDING_MODES);
        String[] supportedModes = new String[supported.length];
        for (int i = 0; i < supported.length; i++) {
            supportedModes[i] = Integer.toString(supported[i]);
        }
        return supportedModes;
    }

    /** {@inheritDoc} */
    @Override
    public void apply(CaptureRequest.Builder builder) {
        int val = Integer.parseInt(getValue());

        builder.set(CaptureRequest.CONTROL_AE_ANTIBANDING_MODE, val);
        Log.i(TAG, "Set anti banding to [" + val + "]");
    }
}
