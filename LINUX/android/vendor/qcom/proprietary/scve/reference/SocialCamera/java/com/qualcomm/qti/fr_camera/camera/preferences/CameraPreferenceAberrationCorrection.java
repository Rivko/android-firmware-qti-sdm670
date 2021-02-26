/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.fr_camera.camera.preferences;

import android.content.Context;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CaptureRequest;

import com.qualcomm.qti.fr_camera.R;

/**
 * This class exposes the {@link CaptureRequest#COLOR_CORRECTION_ABERRATION_MODE} preference
 */
/*package*/ class CameraPreferenceAberrationCorrection extends SimpleCameraPreferenceFromResources {
    public CameraPreferenceAberrationCorrection(Context context, String cameraId,
                                                CameraCharacteristics characteristics) {
        super(context, cameraId, characteristics, R.string.camera_pref_aberration_name,
                R.array.camera_pref_aberration_desc, R.array.camera_pref_aberration_values,
                R.string.camera_pref_aberration_default_value,
                CameraCharacteristics.COLOR_CORRECTION_AVAILABLE_ABERRATION_MODES,
                CaptureRequest.COLOR_CORRECTION_ABERRATION_MODE);
    }
}
