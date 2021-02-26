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
 * This class exposes the {@link CaptureRequest#EDGE_MODE} preference
 */
/*package*/ class CameraPreferenceEdgeEnhancement extends SimpleCameraPreferenceFromResources {
    public CameraPreferenceEdgeEnhancement(Context context, String cameraId,
                                           CameraCharacteristics characteristics) {
        super(context, cameraId, characteristics, R.string.camera_pref_edge_enhancement_name,
                R.array.camera_pref_edge_enhancement_desc,
                R.array.camera_pref_edge_enhancement_values,
                R.string.camera_pref_edge_enhancement_default_value,
                CameraCharacteristics.EDGE_AVAILABLE_EDGE_MODES,
                CaptureRequest.EDGE_MODE);
    }
}
