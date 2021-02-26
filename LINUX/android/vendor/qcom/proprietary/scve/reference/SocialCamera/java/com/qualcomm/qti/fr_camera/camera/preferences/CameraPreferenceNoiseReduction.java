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
 * This class exposes the {@link CaptureRequest#NOISE_REDUCTION_MODE} preference
 */
/*package*/ class CameraPreferenceNoiseReduction extends SimpleCameraPreferenceFromResources {
    public CameraPreferenceNoiseReduction(Context context, String cameraId,
                                          CameraCharacteristics characteristics) {
        super(context, cameraId, characteristics, R.string.camera_pref_noise_reduction_name,
                R.array.camera_pref_noise_reduction_desc,
                R.array.camera_pref_noise_reduction_values,
                R.string.camera_pref_noise_reduction_default_value,
                CameraCharacteristics.NOISE_REDUCTION_AVAILABLE_NOISE_REDUCTION_MODES,
                CaptureRequest.NOISE_REDUCTION_MODE);
    }
}
