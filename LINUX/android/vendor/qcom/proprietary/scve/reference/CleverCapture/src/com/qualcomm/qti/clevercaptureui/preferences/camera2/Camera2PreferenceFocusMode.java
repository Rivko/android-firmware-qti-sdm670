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
 * This class implements a {@link CameraPreference} to control focus mode
 */
@TargetApi(Build.VERSION_CODES.LOLLIPOP)
public class Camera2PreferenceFocusMode extends CameraPreferenceFromResources implements
        Camera2CameraPreference {
    private static final String TAG = Camera2PreferenceFocusMode.class.getSimpleName();
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

        builder.set(CaptureRequest.CONTROL_AF_MODE, val);
        if (val == CaptureRequest.CONTROL_AF_MODE_OFF) {
            builder.set(CaptureRequest.LENS_FOCUS_DISTANCE, 0f);
        }
        Log.i(TAG, "Set focus mode to [" + val + "]");
    }
}
