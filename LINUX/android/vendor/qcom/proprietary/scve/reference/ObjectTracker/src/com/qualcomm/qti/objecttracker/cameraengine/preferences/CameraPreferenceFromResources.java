/*
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.objecttracker.cameraengine.preferences;

import android.content.Context;
import android.content.res.Resources;
import android.hardware.Camera.Parameters;

/**
 * This class extends {@link CameraPreference} to allow specifying title, entries, and values from
 * application resources
 */
@SuppressWarnings("deprecation")
/*package*/ abstract class CameraPreferenceFromResources extends CameraPreference {

    private final String mTitle;
    private final String[] mEntries;
    private final String[] mValues;

    /**
     * Construct a {@link CameraPreferenceFromResources}
     * @param titleResId Resource ID of a {@link String} to use as the preference title
     * @param entriesResId Resource ID of an array of {@link String}s to use as the preference
     *      entries
     * @param valuesResId Resource ID of an array of {@link String}s to use as the preference
     *      entry values
     * @see CameraPreference#CameraPreference(Context, int, Parameters)
     */
    public CameraPreferenceFromResources(Context context, int cameraId, Parameters p,
            int titleResId, int entriesResId, int valuesResId) {
        super(context, cameraId, p);

        Resources res = context.getResources();
        mTitle = res.getString(titleResId);
        mEntries = res.getStringArray(entriesResId);
        mValues = res.getStringArray(valuesResId);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected String getTitle() {
        return mTitle;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected String[] getEntries() {
        return mEntries;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected String[] getValues() {
        return mValues;
    }
}
