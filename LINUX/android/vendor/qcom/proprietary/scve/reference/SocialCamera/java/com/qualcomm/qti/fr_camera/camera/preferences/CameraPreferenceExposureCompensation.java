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
import android.util.Range;
import android.util.Rational;

import com.qualcomm.qti.fr_camera.R;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.Locale;

/**
 * This class exposes the {@link CaptureRequest#CONTROL_AE_EXPOSURE_COMPENSATION} preference
 */
/*package*/ class CameraPreferenceExposureCompensation extends CameraPreference {
    private static final String TAG = "SocialCamera";

    private final ArrayList<Setting> settings = new ArrayList<>();
    private final Collection<Setting> settingsUnmodifiable =
            Collections.unmodifiableCollection(settings);
    private Setting defaultSetting;

    public CameraPreferenceExposureCompensation(Context context, String cameraId,
                                                CameraCharacteristics characteristics) {
        super(context, cameraId, characteristics);
        populateSettings();
    }

    @Override
    protected String getPreferenceKeyName() {
        return "exposure_comp";
    }

    @Override
    protected CharSequence getName() {
        return context.getText(R.string.camera_pref_exposure_comp_name);
    }

    @Override
    protected Collection<Setting> getSettings() {
        return settingsUnmodifiable;
    }

    @Override
    protected Setting getDefaultSetting() {
        return defaultSetting;
    }

    @Override
    protected void doApply(CaptureRequest.Builder request, boolean isPreviewRequest, String value) {
        int val = Integer.parseInt(value);
        Log.v(TAG, "Setting [" + CaptureRequest.CONTROL_AE_EXPOSURE_COMPENSATION.getName() +
                "] to [" + val + "]");
        request.set(CaptureRequest.CONTROL_AE_EXPOSURE_COMPENSATION, val);
    }

    private void populateSettings() {
        final Range<Integer> compRange = characteristics.get(
                CameraCharacteristics.CONTROL_AE_COMPENSATION_RANGE);
        final Rational compStep = characteristics.get(
                CameraCharacteristics.CONTROL_AE_COMPENSATION_STEP);
        assert compRange != null;
        assert compStep != null;

        if (!compStep.isFinite()) {
            // Invalid compensation step, cannot configure a compensation range
            return;
        }

        String desc = context.getString(R.string.camera_pref_exposure_comp_desc);
        final int defaultValue = (compRange.contains(0) ? 0 :
                (compRange.getLower() + compRange.getUpper()) / 2);

        for (int i = compRange.getLower(); i <= compRange.getUpper(); i++) {
            Setting s = new Setting(Integer.toString(i), String.format(Locale.US, desc,
                    (float)(i * compStep.getNumerator()) / (float)compStep.getDenominator()));
            settings.add(s);

            if (i == defaultValue) {
                defaultSetting = s;
            }
        }
    }
}
