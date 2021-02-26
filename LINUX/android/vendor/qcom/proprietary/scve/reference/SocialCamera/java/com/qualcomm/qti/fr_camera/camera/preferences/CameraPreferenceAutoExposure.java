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
 * This class exposes the {@link CaptureRequest#CONTROL_AE_MODE} preference
 */
/*package*/ class CameraPreferenceAutoExposure extends SimpleCameraPreferenceFromResources {
    public CameraPreferenceAutoExposure(Context context, String cameraId,
                                        CameraCharacteristics characteristics) {
        super(context, cameraId, characteristics, R.string.camera_pref_ae_name,
                R.array.camera_pref_ae_desc, R.array.camera_pref_ae_values,
                R.string.camera_pref_ae_default_value,
                CameraCharacteristics.CONTROL_AE_AVAILABLE_MODES, CaptureRequest.CONTROL_AE_MODE);
    }
}
