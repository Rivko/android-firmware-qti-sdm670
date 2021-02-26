/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.fr_camera.camera.preferences;

import android.content.Context;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CaptureRequest;

import com.qualcomm.qti.fr_camera.R;

import java.util.ArrayList;

/**
 * This class exposes the supported {@link CameraDevice} preview resolution selection modes
 */
/*package*/ class CameraPreferencePreviewSize extends CameraPreferenceFromResources {
    public CameraPreferencePreviewSize(Context context, String cameraId,
                                       CameraCharacteristics characteristics) {
        super(context, cameraId, characteristics, R.string.camera_pref_preview_size,
                R.array.camera_pref_preview_size_desc, R.array.camera_pref_preview_size_values,
                R.string.camera_pref_preview_size_default_value);
    }

    public PreviewSize getPreviewSize() {
        return PreviewSize.valueOf(getValue());
    }

    @Override
    protected String getPreferenceKeyName() {
        return "preview_size";
    }

    @Override
    protected void filterSettings(ArrayList<Setting> settings) {
    }

    @Override
    protected void doApply(CaptureRequest.Builder request, boolean isPreviewRequest, String value) {
        // N.B. the preview size is not applied to the CaptureRequest; rather, it is applied to a
        // SurfaceTexture when the CameraCaptureSession is created
    }
}
