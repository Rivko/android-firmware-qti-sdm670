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
 * This class exposes the {@link CaptureRequest#CONTROL_AWB_MODE} preference
 */
/*package*/ class CameraPreferenceAutoWhiteBalance extends SimpleCameraPreferenceFromResources {
    public CameraPreferenceAutoWhiteBalance(Context context, String cameraId,
                                            CameraCharacteristics characteristics) {
        super(context, cameraId, characteristics, R.string.camera_pref_awb_name,
                R.array.camera_pref_awb_desc, R.array.camera_pref_awb_values,
                R.string.camera_pref_awb_default_value,
                CameraCharacteristics.CONTROL_AWB_AVAILABLE_MODES, CaptureRequest.CONTROL_AWB_MODE);
    }
}
