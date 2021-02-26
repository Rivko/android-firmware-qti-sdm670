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
 * This class exposes the {@link CaptureRequest#CONTROL_AF_MODE} preference
 */
/*package*/ class CameraPreferenceAutoFocus extends SimpleCameraPreferenceFromResources {
    private final boolean hasVariableFocus;

    public CameraPreferenceAutoFocus(Context context, String cameraId,
                                     CameraCharacteristics characteristics) {
        super(context, cameraId, characteristics, R.string.camera_pref_af_name,
                R.array.camera_pref_af_desc, R.array.camera_pref_af_values,
                R.string.camera_pref_af_default_value,
                CameraCharacteristics.CONTROL_AF_AVAILABLE_MODES, CaptureRequest.CONTROL_AF_MODE);

        final Float minFocusDistance = characteristics.get(
                CameraCharacteristics.LENS_INFO_MINIMUM_FOCUS_DISTANCE);
        hasVariableFocus = (minFocusDistance != null && minFocusDistance > 0f);
    }

    @Override
    protected void doApply(CaptureRequest.Builder request, boolean isPreviewRequest, int value) {
        super.doApply(request, isPreviewRequest, value);

        // If AF is off (ie fixed focus) and lens has variable focus, set focus distance to infinity
        if (value == CaptureRequest.CONTROL_AF_MODE_OFF && hasVariableFocus) {
            request.set(CaptureRequest.LENS_FOCUS_DISTANCE, 0f);
        }
    }
}
