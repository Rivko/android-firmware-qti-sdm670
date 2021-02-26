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

import java.util.ArrayList;

/**
 * This class exposes the {@link CaptureRequest#SHADING_MODE} preference
 */
/*package*/ class CameraPreferenceLensShadingCorrection
        extends SimpleCameraPreferenceFromResources {
    public CameraPreferenceLensShadingCorrection(Context context, String cameraId,
                                                 CameraCharacteristics characteristics) {
        super(context, cameraId, characteristics, R.string.camera_pref_lens_shading_name,
                R.array.camera_pref_lens_shading_desc, R.array.camera_pref_lens_shading_values,
                R.string.camera_pref_lens_shading_default_value, null, CaptureRequest.SHADING_MODE);
    }

    @Override
    protected void filterSettings(ArrayList<Setting> settings) {
        final Integer supportedHardwareLevel = characteristics.get(
                CameraCharacteristics.INFO_SUPPORTED_HARDWARE_LEVEL);
        assert supportedHardwareLevel != null;

        // Only supported on FULL devices
        if (supportedHardwareLevel != CameraCharacteristics.INFO_SUPPORTED_HARDWARE_LEVEL_FULL) {
            settings.clear();
            return;
        }

        super.filterSettings(settings);
    }
}
