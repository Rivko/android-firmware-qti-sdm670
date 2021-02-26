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
 * This class implements a {@link CameraPreference} to control scene mode
 */
@TargetApi(Build.VERSION_CODES.LOLLIPOP)
public class Camera2PreferenceSceneMode extends CameraPreferenceFromResources implements
        Camera2CameraPreference {
    private static final String TAG = Camera2PreferenceSceneMode.class.getSimpleName();
    private static final int CONTROL_SCENE_MODE_AUTO = -1;

    private final CameraCharacteristics characteristics;

    /**
     * Construct a {@link Camera2PreferenceSceneMode}
     * @see CameraPreferenceFromResources#CameraPreferenceFromResources(Context, int, int, int, int)
     */
    public Camera2PreferenceSceneMode(Context context, int cameraId,
            CameraCharacteristics characteristics) {
        super(context, cameraId, R.string.camera_settings_scene_mode,
                R.array.camera_settings_scene_mode_entries,
                R.array.camera_settings_scene_mode_values_camera2);
        this.characteristics = characteristics;
    }

    /** {@inheritDoc} */
    @Override
    protected String getBaseKey() {
        return "camerapref_scene_mode";
    }

    /** {@inheritDoc} */
    @Override
    protected String[] getSupportedValues() {
        int[] supported = characteristics.get(CameraCharacteristics.CONTROL_AVAILABLE_SCENE_MODES);
        if (supported != null) {
            String[] supportedModes = new String[supported.length + 1];
            for (int i = 0; i < supported.length; i++) {
                supportedModes[i] = Integer.toString(supported[i]);
            }
            supportedModes[supportedModes.length - 1] = Integer.toString(CONTROL_SCENE_MODE_AUTO);
            return supportedModes;
        } else {
            return null;
        }
    }

    /** {@inheritDoc} */
    @Override
    public void apply(CaptureRequest.Builder builder) {
        int val = Integer.parseInt(getValue());

        if (val == CONTROL_SCENE_MODE_AUTO) {
            builder.set(CaptureRequest.CONTROL_MODE, CaptureRequest.CONTROL_MODE_AUTO);
            Log.i(TAG, "Set scene mode to [AUTO]");
        } else {
            builder.set(CaptureRequest.CONTROL_MODE, CaptureRequest.CONTROL_MODE_USE_SCENE_MODE);
            builder.set(CaptureRequest.CONTROL_SCENE_MODE, val);
            Log.i(TAG, "Set scene mode to [" + val + "]");
        }
    }
}
