/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.clevercaptureui.preferences.camera2;

import android.annotation.TargetApi;
import android.content.Context;
import android.graphics.ImageFormat;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.params.StreamConfigurationMap;
import android.os.Build;
import android.util.Size;

import com.qualcomm.qti.clevercaptureui.R;
import com.qualcomm.qti.clevercaptureui.preferences.CameraPreference;
import com.qualcomm.qti.clevercaptureui.preferences.CameraPreferenceFromResources;

/**
 * This class implements a {@link CameraPreference} for picture resolution using the Camera2 APIs
 */
@TargetApi(Build.VERSION_CODES.LOLLIPOP)
public class Camera2PreferencePictureSize extends CameraPreferenceFromResources {

    private final CameraCharacteristics characteristics;

    /**
     * Construct a new {@link Camera2PreferencePictureSize}
     */
    public Camera2PreferencePictureSize(Context context, int cameraId, CameraCharacteristics cc) {
        super(context, cameraId, R.string.camera_settings_picture_size,
                R.array.camera_settings_picture_size_entries,
                R.array.camera_settings_picture_size_values);
        characteristics = cc;
    }

    /** {@inheritDoc} */
    @Override
    protected String getBaseKey() {
        return "camerapref_picture_size";
    }

    /** {@inheritDoc} */
    @Override
    protected String[] getSupportedValues() {
        StreamConfigurationMap map = characteristics.get(
                CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP);
        Size[] sizes = map.getOutputSizes(ImageFormat.JPEG);

        String[] result = new String[sizes.length];
        for (int i = 0; i < sizes.length; i++) {
            result[i] = sizes[i].toString();
        }

        return result;
    }

    /**
     * Get the selected picture size
     * @return a {@link Size} containing the selected picture size
     */
    public Size getPictureSize() {
        int w = 0;
        int h = 0;

        String val = getValue();
        if (val != null && !val.isEmpty()) {
            int x = val.indexOf("x");
            if (x > 0) {
                w = Integer.parseInt(val.substring(0, x));
                h = Integer.parseInt(val.substring(x + 1));
            }
        }

        return new Size(w, h);
    }
}
