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
 * This class implements a {@link CameraPreference} to control white balance
 */
@TargetApi(Build.VERSION_CODES.LOLLIPOP)
public class Camera2PreferenceWhiteBalance extends CameraPreferenceFromResources implements
        Camera2CameraPreference {
    private static final String TAG = Camera2PreferenceWhiteBalance.class.getSimpleName();

    private final CameraCharacteristics characteristics;

    /**
     * Construct a {@link Camera2PreferenceWhiteBalance}
     * @see CameraPreferenceFromResources#CameraPreferenceFromResources(Context, int, int, int, int)
     */
    public Camera2PreferenceWhiteBalance(Context context, int cameraId,
            CameraCharacteristics characteristics) {
        super(context, cameraId, R.string.camera_settings_white_balance,
                R.array.camera_settings_white_balance_entries,
                R.array.camera_settings_white_balance_values_camera2);
        this.characteristics = characteristics;
    }

    /** {@inheritDoc} */
    @Override
    protected String getBaseKey() {
        return "camerapref_white_balance";
    }

    /** {@inheritDoc} */
    @Override
    protected String[] getSupportedValues() {
        int[] supported = characteristics.get(CameraCharacteristics.CONTROL_AWB_AVAILABLE_MODES);
        if (supported != null) {
            String[] supportedModes = new String[supported.length];
            for (int i = 0; i < supported.length; i++) {
                supportedModes[i] = Integer.toString(supported[i]);
            }
            return supportedModes;
        } else {
            return null;
        }
    }

    /** {@inheritDoc} */
    @Override
    public void apply(CaptureRequest.Builder builder) {
        int val = Integer.parseInt(getValue());
        builder.set(CaptureRequest.CONTROL_AWB_MODE, val);
        Log.i(TAG, "Set white balance to [" + val + "]");
    }
}
