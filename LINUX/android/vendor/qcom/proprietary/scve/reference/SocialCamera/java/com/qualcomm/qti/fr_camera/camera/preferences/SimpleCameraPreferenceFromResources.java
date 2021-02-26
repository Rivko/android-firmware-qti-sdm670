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

import java.util.ArrayList;
import java.util.Iterator;
import java.util.Objects;

/**
 * A specialization of {@link CameraPreferenceFromResources} which uses a
 * {@link CameraCharacteristics.Key} and a {@link CaptureRequest.Key} to implement sensible default
 * methods
 */
/*package*/ abstract class SimpleCameraPreferenceFromResources
        extends CameraPreferenceFromResources {
    private static final String TAG = "SocialCamera";

    private final CameraCharacteristics.Key<int[]> availableKey;
    private final CaptureRequest.Key<Integer> applyKey;

    /**
     * Construct a new {@link SimpleCameraPreferenceFromResources}
     * @param availableKey a {@link CameraCharacteristics.Key} which enumerates the available
     *                     settings for this {@link CameraPreference}
     * @param applyKey a {@link CaptureRequest.Key} to which this {@link CameraPreference} should be
     *                 applied
     * @throws NullPointerException if applyKey is null
     * @see CameraPreferenceFromResources#CameraPreferenceFromResources(Context, String,
     *      CameraCharacteristics, int, int, int, int)
     */
    public SimpleCameraPreferenceFromResources(Context context, String cameraId,
                                               CameraCharacteristics characteristics, int nameResId,
                                               int descriptionsResId, int valuesResId,
                                               int defaultValueResId,
                                               CameraCharacteristics.Key<int[]> availableKey,
                                               CaptureRequest.Key<Integer> applyKey) {
        super(context, cameraId, characteristics, nameResId, descriptionsResId, valuesResId,
                defaultValueResId);

        Objects.requireNonNull(applyKey, "applyKey may not be null");
        this.availableKey = availableKey;
        this.applyKey = applyKey;
    }

    @Override
    protected String getPreferenceKeyName() {
        return applyKey.getName();
    }

    @Override
    protected void filterSettings(ArrayList<Setting> settings) {
        if (availableKey == null) {
            return;
        }

        int[] available = characteristics.get(availableKey);

        if (available == null) {
            settings.clear();
            return;
        }

        for (Iterator<Setting> it = settings.iterator(); it.hasNext();) {
            Setting s = it.next();

            boolean found = false;
            for (int i : available) {
                if (s.value.equals(Integer.toString(i))) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                it.remove();
            }
        }
    }

    // N.B. Made final, subclasses should override the variant below (for which value is an int)
    @Override
    protected final void doApply(CaptureRequest.Builder request, boolean isPreviewRequest,
                                 String value) {
        int valueInt = Integer.parseInt(value);
        doApply(request, isPreviewRequest, valueInt);
    }

    /**
     * A variant of {@link #doApply(CaptureRequest.Builder, boolean, String)} which converts value
     * to an int
     */
    protected void doApply(CaptureRequest.Builder request, boolean isPreviewRequest, int value) {
        Log.v(TAG, "Setting [" + applyKey.getName() + "] to [" + value + "]");
        request.set(applyKey, value);
    }
}
