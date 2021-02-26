/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.panoramaui.preferences.camera2;

import android.annotation.TargetApi;
import android.content.Context;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CaptureRequest;
import android.os.Build;
import android.util.Log;

import com.qualcomm.qti.panoramaui.R;
import com.qualcomm.qti.panoramaui.preferences.CameraPreference;
import com.qualcomm.qti.panoramaui.preferences.CameraPreferenceFromResources;

/**
 * This class implements a {@link CameraPreference} to control focus mode
 */
@TargetApi(Build.VERSION_CODES.LOLLIPOP)
public class Camera2PreferenceFocusMode extends CameraPreferenceFromResources implements
        Camera2CameraPreference {
    private static final String TAG = Camera2PreferenceFocusMode.class.getSimpleName();
    private static final int CONTROL_AF_MODE_FIXED = -1;

    private final CameraCharacteristics characteristics;

    /**
     * Construct a {@link Camera2PreferenceFocusMode}
     * @see CameraPreferenceFromResources#CameraPreferenceFromResources(Context, int, int, int, int)
     */
    public Camera2PreferenceFocusMode(Context context, int cameraId,
            CameraCharacteristics characteristics) {
        super(context, cameraId, R.string.camera_settings_focus_mode,
                R.array.camera_settings_focus_mode_entries,
                R.array.camera_settings_focus_mode_values_camera2);
        this.characteristics = characteristics;
    }

    /** {@inheritDoc} */
    @Override
    protected String getBaseKey() {
        return "camerapref_focus_mode";
    }

    /** {@inheritDoc} */
    @Override
    protected String[] getSupportedValues() {
        // Check for fixed-focus lens
        Float minFocusDistance = characteristics.get(
                CameraCharacteristics.LENS_INFO_MINIMUM_FOCUS_DISTANCE);
        if (minFocusDistance != null && minFocusDistance == 0f) {
            return new String[] { Integer.toString(CONTROL_AF_MODE_FIXED) };
        }

        int[] supported = characteristics.get(CameraCharacteristics.CONTROL_AF_AVAILABLE_MODES);
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

        if (val == CONTROL_AF_MODE_FIXED) {
            Log.i(TAG, "Camera focus is [FIXED], not setting focus mode");
        } else {
            builder.set(CaptureRequest.CONTROL_AF_MODE, val);
            if (val == CaptureRequest.CONTROL_AF_MODE_OFF) {
                builder.set(CaptureRequest.LENS_FOCUS_DISTANCE, 0f);
            }
            Log.i(TAG, "Set focus mode to [" + val + "]");
        }
    }
}
