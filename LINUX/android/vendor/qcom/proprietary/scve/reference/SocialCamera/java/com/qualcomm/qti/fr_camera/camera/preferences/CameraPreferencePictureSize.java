/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.fr_camera.camera.preferences;

import android.content.Context;
import android.graphics.ImageFormat;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CaptureRequest;
import android.hardware.camera2.params.StreamConfigurationMap;
import android.util.Size;

import com.qualcomm.qti.fr_camera.R;

import java.util.ArrayList;
import java.util.Iterator;

/**
 * This class exposes the supported {@link CameraDevice} picture resolutions
 */
/*package*/ class CameraPreferencePictureSize extends CameraPreferenceFromResources {
    public CameraPreferencePictureSize(Context context, String cameraId,
                                       CameraCharacteristics characteristics) {
        super(context, cameraId, characteristics, R.string.camera_pref_picture_size_name,
                R.array.camera_pref_picture_size_desc, R.array.camera_pref_picture_size_values, 0);
    }

    /**
     * Get the users requested picture {@link Size}
     * @return a {@link Size} containing the requested picture size
     */
    public Size getSize() {
        final int val = Integer.decode(getValue());
        final int width = (val >> 16) & 0xFFFF;
        final int height = val & 0xFFFF;
        return new Size(width, height);
    }

    @Override
    protected String getPreferenceKeyName() {
        return "picture_size";
    }

    @Override
    protected void filterSettings(ArrayList<Setting> settings) {
        final StreamConfigurationMap scm = characteristics.get(
                CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP);
        assert scm != null;
        final Size[] sizes = scm.getOutputSizes(ImageFormat.JPEG);
        assert sizes != null;

        Iterator<Setting> it = settings.iterator();
        while (it.hasNext()) {
            Setting s = it.next();
            final int val = Integer.decode(s.value);
            final int width = (val >> 16) & 0xFFFF;
            final int height = val & 0xFFFF;

            boolean found = false;
            for (Size size : sizes) {
                if (size.getWidth() == width && size.getHeight() == height) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                it.remove();
            }
        }
    }

    @Override
    protected void doApply(CaptureRequest.Builder request, boolean isPreviewRequest, String value) {
        // N.B. the picture size is not applied to the CaptureRequest; rather, it is applied to an
        // ImageReader when the CameraCaptureSession is created
    }
}
