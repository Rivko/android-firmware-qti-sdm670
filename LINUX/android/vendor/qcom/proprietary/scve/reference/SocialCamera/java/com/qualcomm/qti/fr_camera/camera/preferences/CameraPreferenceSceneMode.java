/*
 * Copyright (c) 2015,2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.fr_camera.camera.preferences;

import android.content.Context;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CaptureRequest;
import android.util.Log;

import com.qualcomm.qti.fr_camera.R;

import java.util.ArrayList;

/**
 * A {@link CameraPreference} which handles Scene Mode configuration
 */
/*package*/ class CameraPreferenceSceneMode extends SimpleCameraPreferenceFromResources {
    private static final String TAG = "SocialCamera";

    /**
     * Construct a new {@link CameraPreferenceSceneMode}
     * @see SimpleCameraPreferenceFromResources#SimpleCameraPreferenceFromResources(Context, String,
     *      CameraCharacteristics, int, int, int, int, CameraCharacteristics.Key,
     *      CaptureRequest.Key)
     */
    public CameraPreferenceSceneMode(Context context, String cameraId,
                                     CameraCharacteristics characteristics) {
        super(context, cameraId, characteristics, R.string.camera_pref_scene_mode_name,
                R.array.camera_pref_scene_mode_desc, R.array.camera_pref_scene_mode_values,
                R.string.camera_pref_scene_mode_default_value,
                CameraCharacteristics.CONTROL_AVAILABLE_SCENE_MODES,
                CaptureRequest.CONTROL_SCENE_MODE);
    }

    @Override
    protected void filterSettings(ArrayList<Setting> settings) {
        // If any scene modes are supported, then CONTROL_SCENE_MODE_DISABLED will not be present
        // in the list. However, we use this entry to indicate Auto scene mode to the user. It will
        // always be first in the list, so temporarily remove it, and reinsert it after filtering.
        Setting auto = settings.remove(0);
        super.filterSettings(settings);
        settings.add(0, auto);
    }

    @Override
    protected void doApply(CaptureRequest.Builder request, boolean isPreviewRequest, int value) {
        super.doApply(request, isPreviewRequest, value);

        int controlMode = (value == CaptureRequest.CONTROL_SCENE_MODE_DISABLED ?
                CaptureRequest.CONTROL_MODE_AUTO : CaptureRequest.CONTROL_MODE_USE_SCENE_MODE);

        Log.v(TAG, "Setting [" + CaptureRequest.CONTROL_MODE.getName() + "] to [" +
                controlMode + "]");

        request.set(CaptureRequest.CONTROL_MODE,
                value == CaptureRequest.CONTROL_SCENE_MODE_DISABLED ?
                        CaptureRequest.CONTROL_MODE_AUTO :
                        CaptureRequest.CONTROL_MODE_USE_SCENE_MODE);
    }
}
