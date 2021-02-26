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
import java.util.Iterator;

/**
 * This class exposes the {@link CaptureRequest#LENS_OPTICAL_STABILIZATION_MODE} and
 * {@link CaptureRequest#CONTROL_VIDEO_STABILIZATION_MODE} preferences
 */
/*package*/ class CameraPreferenceStabilization extends CameraPreferenceFromResources {
    private static final String TAG = "SocialCamera";
    private static final String STABILIZATION_OFF = "off";
    private static final String STABILIZATION_OPTICAL = "optical";
    private static final String STABILIZATION_DIGITAL = "digital";

    public CameraPreferenceStabilization(Context context, String cameraId,
                                         CameraCharacteristics characteristics) {
        super(context, cameraId, characteristics, R.string.camera_pref_stabilization_name,
                R.array.camera_pref_stabilization_desc, R.array.camera_pref_stabilization_values,
                R.string.camera_pref_stabilization_default_value);
    }

    @Override
    protected String getPreferenceKeyName() {
        return "stabilization";
    }

    @Override
    protected void filterSettings(ArrayList<Setting> settings) {
        Iterator<Setting> it = settings.iterator();
        while (it.hasNext()) {
            Setting s = it.next();

            if (STABILIZATION_OPTICAL.equals(s.value)) {
                int[] opticalModes = characteristics.get(
                        CameraCharacteristics.LENS_INFO_AVAILABLE_OPTICAL_STABILIZATION);
                if (opticalModes == null || opticalModes.length <= 1) {
                    it.remove();
                }
            } else if (STABILIZATION_DIGITAL.equals(s.value)) {
                int[] digitalModes = characteristics.get(
                        CameraCharacteristics.CONTROL_AVAILABLE_VIDEO_STABILIZATION_MODES);
                if (digitalModes == null || digitalModes.length <= 1) {
                    it.remove();
                }
            } else if (!STABILIZATION_OFF.equals(s.value)) {
                throw new IllegalStateException("Unexpected setting value");
            }
        }
    }

    @Override
    protected void doApply(CaptureRequest.Builder request, boolean isPreviewRequest, String value) {
        int opticalValue = (STABILIZATION_OPTICAL.equals(value) ?
                CaptureRequest.LENS_OPTICAL_STABILIZATION_MODE_ON :
                CaptureRequest.LENS_OPTICAL_STABILIZATION_MODE_OFF);
        int digitalValue = (STABILIZATION_DIGITAL.equals(value) ?
                CaptureRequest.CONTROL_VIDEO_STABILIZATION_MODE_ON :
                CaptureRequest.CONTROL_VIDEO_STABILIZATION_MODE_OFF);

        Log.v(TAG, "Setting [" + CaptureRequest.LENS_OPTICAL_STABILIZATION_MODE.getName() +
                "] to [" + value + "]");
        request.set(CaptureRequest.LENS_OPTICAL_STABILIZATION_MODE, opticalValue);

        Log.v(TAG, "Setting [" + CaptureRequest.CONTROL_VIDEO_STABILIZATION_MODE.getName() +
                "] to [" + value + "]");
        request.set(CaptureRequest.CONTROL_VIDEO_STABILIZATION_MODE, digitalValue);
    }
}
