/*
 * Copyright (c) 2015,2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.fr_camera.camera.preferences;

import android.content.Context;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CaptureRequest;
import android.preference.PreferenceManager;
import android.util.Log;

import com.qualcomm.qti.fr_camera.R;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Iterator;

/**
 * This class controls the camera flash (via the {@link CaptureRequest#FLASH_MODE} and
 * {@link CaptureRequest#CONTROL_AE_MODE} settings)
 */
/*package*/ class CameraPreferenceFlash extends CameraPreferenceFromResources {
    private static final String TAG = "SocialCamera";

    public CameraPreferenceFlash(Context context, String cameraId,
                                 CameraCharacteristics characteristics) {
        super(context, cameraId, characteristics, R.string.camera_pref_flash_name,
                R.array.camera_pref_flash_desc, R.array.camera_pref_flash_values,
                R.string.camera_pref_flash_default_value);
    }

    /**
     * Get the current {@link FlashMode}
     * @return the current {@link FlashMode}, or {@link FlashMode#NOT_SUPPORTED} if this
     *         {@link CameraDevice} does not have a flash
     */
    public FlashMode getFlashMode() {
        return (isSupported() ? FlashMode.valueOf(getValue()) : FlashMode.NOT_SUPPORTED);
    }

    /**
     * Toggle the {@link CameraPreferenceFlash} to the next supported {@link FlashMode}
     * @return the newly selected {@link FlashMode}, or {@link FlashMode#NOT_SUPPORTED} if this
     *         {@link CameraDevice} does not have a flash
     */
    public FlashMode toggleFlashMode() {
        if (!isSupported()) {
            return FlashMode.NOT_SUPPORTED;
        }

        Collection<Setting> settings = getSettings();
        String val = getValue();

        Iterator<Setting> it = settings.iterator();
        while (it.hasNext()) {
            Setting s = it.next();
            if (val.equals(s.value)) {
                break;
            }
        }

        Setting s;
        if (it.hasNext()) {
            s = it.next();
        } else {
            s = settings.iterator().next();
        }

        PreferenceManager.getDefaultSharedPreferences(context).edit().putString(
                getPreferenceKey(), s.value).apply();

        return FlashMode.valueOf(s.value);
    }

    @Override
    protected String getPreferenceKeyName() {
        return "flash";
    }

    @Override
    protected void filterSettings(ArrayList<Setting> settings) {
        final Boolean flashAvailable = characteristics.get(
                CameraCharacteristics.FLASH_INFO_AVAILABLE);
        assert flashAvailable != null;

        if (!flashAvailable) {
            settings.clear();
        }
    }

    @Override
    protected void doApply(CaptureRequest.Builder request, boolean isPreviewRequest, String value) {
        final FlashMode mode = FlashMode.valueOf(value);
        final Integer aeMode = request.get(CaptureRequest.CONTROL_AE_MODE);
        assert aeMode != null;

        if (aeMode == CaptureRequest.CONTROL_AE_MODE_ON) {
            int val = CaptureRequest.CONTROL_AE_MODE_ON;
            if (mode == FlashMode.ON) {
                val = CaptureRequest.CONTROL_AE_MODE_ON_ALWAYS_FLASH;
            } else if (mode == FlashMode.AUTO) {
                val = CaptureRequest.CONTROL_AE_MODE_ON_AUTO_FLASH;
            }

            Log.v(TAG, "Setting [" + CaptureRequest.CONTROL_AE_MODE.getName() + "] to [" +
                    val + "]");
            request.set(CaptureRequest.CONTROL_AE_MODE, val);
        } else if (!isPreviewRequest) {
            int val = (mode == FlashMode.ON ? CaptureRequest.FLASH_MODE_SINGLE :
                    CaptureRequest.FLASH_MODE_OFF);
            Log.v(TAG, "Setting [" + CaptureRequest.FLASH_MODE.getName() + "] to [" + val + "]");
            request.set(CaptureRequest.FLASH_MODE, val);
        }
    }
}
