/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.panoramaui.preferences.camera2;

import java.text.DecimalFormat;
import java.text.DecimalFormatSymbols;
import java.util.Locale;

import com.qualcomm.qti.panoramaui.R;
import com.qualcomm.qti.panoramaui.preferences.CameraPreference;

import android.annotation.TargetApi;
import android.content.Context;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CaptureRequest;
import android.os.Build;
import android.util.Log;
import android.util.Range;
import android.util.Rational;

/**
 * This class implements a {@link CameraPreference} to control exposure compensation
 */
@TargetApi(Build.VERSION_CODES.LOLLIPOP)
public class Camera2PreferenceExposureCompensation extends CameraPreference implements
        Camera2CameraPreference {
    private static final String TAG = Camera2PreferenceExposureCompensation.class.getSimpleName();

    private final String mTitle;
    private final String[] mEntries;
    private final String[] mValues;

    /**
     * Construct a {@link Camera2PreferenceExposureCompensation}.
     * @see CameraPreference#CameraPreference(Context, int)
     */
    public Camera2PreferenceExposureCompensation(Context context, int cameraId,
            CameraCharacteristics characteristics) {
        super(context, cameraId);

        mTitle = context.getResources().getString(R.string.camera_settings_exposure_compensation);

        DecimalFormat df = new DecimalFormat("+#0.##;-#", DecimalFormatSymbols.getInstance(Locale.US));
        Range<Integer> compensationRange = characteristics.get(
                CameraCharacteristics.CONTROL_AE_COMPENSATION_RANGE);
        Rational step = characteristics.get(CameraCharacteristics.CONTROL_AE_COMPENSATION_STEP);
        int numEntries = compensationRange.getUpper() - compensationRange.getLower() + 1;
        mEntries = new String[numEntries];
        mValues = new String[numEntries];
        for (int i = 0; i < numEntries; i++) {
            float val = (compensationRange.getLower() + i) * step.floatValue();
            if (Math.abs(val) <= 0.005f) {
                mEntries[i] = "0";
            } else {
                mEntries[i] = df.format(val);
            }
            mValues[i] = Integer.toString(compensationRange.getLower() + i);
        }
    }

    /** {@inheritDoc} */
    @Override
    protected String getBaseKey() {
        return "camerapref_exposure_comp";
    }

    /** {@inheritDoc} */
    @Override
    protected String getTitle() {
        return mTitle;
    }

    /** {@inheritDoc} */
    @Override
    protected String[] getEntries() {
        return mEntries;
    }

    /** {@inheritDoc} */
    @Override
    protected String[] getValues() {
        return mValues;
    }

    /** {@inheritDoc} */
    @Override
    protected String[] getSupportedValues() {
        return null;
    }

    /** {@inheritDoc} */
    @Override
    protected String getDefaultValue() {
        return "0";
    }

    /** {@inheritDoc} */
    @Override
    public void apply(CaptureRequest.Builder builder) {
        int idx = Integer.parseInt(getValue());
        builder.set(CaptureRequest.CONTROL_AE_EXPOSURE_COMPENSATION, idx);
        Log.i(TAG, "Set exposure compensation idx to [" + idx + "]");
    }
}
