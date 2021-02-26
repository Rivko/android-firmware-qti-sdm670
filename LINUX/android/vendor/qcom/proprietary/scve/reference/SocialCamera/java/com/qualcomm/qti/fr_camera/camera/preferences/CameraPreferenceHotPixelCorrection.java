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
 * This class exposes the {@link CaptureRequest#HOT_PIXEL_MODE} preference
 */
/*package*/ class CameraPreferenceHotPixelCorrection extends SimpleCameraPreferenceFromResources {
    public CameraPreferenceHotPixelCorrection(Context context, String cameraId,
                                              CameraCharacteristics characteristics) {
        super(context, cameraId, characteristics, R.string.camera_pref_hot_pixel_name,
                R.array.camera_pref_hot_pixel_desc, R.array.camera_pref_hot_pixel_values,
                R.string.camera_pref_hot_pixel_default_value,
                CameraCharacteristics.HOT_PIXEL_AVAILABLE_HOT_PIXEL_MODES,
                CaptureRequest.HOT_PIXEL_MODE);
    }
}
