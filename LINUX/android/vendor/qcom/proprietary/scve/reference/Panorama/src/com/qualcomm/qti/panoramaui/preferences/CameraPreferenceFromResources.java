/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.panoramaui.preferences;

import android.content.Context;
import android.content.res.Resources;

/**
 * This class extends {@link CameraPreference} to allow specifying title, entries, and values from
 * application resources
 */
public abstract class CameraPreferenceFromResources extends CameraPreference {

    private final String mTitle;
    private final String[] mEntries;
    private final String[] mValues;
    private String mDefaultValue;

    /**
     * Construct a {@link CameraPreferenceFromResources}
     * @param titleResId Resource ID of a {@link String} to use as the preference title
     * @param entriesResId Resource ID of an array of {@link String}s to use as the preference
     *      entries
     * @param valuesResId Resource ID of an array of {@link String}s to use as the preference
     *      entry values
     * @see CameraPreference#CameraPreference(Context, int)
     */
    public CameraPreferenceFromResources(Context context, int cameraId, int titleResId,
            int entriesResId, int valuesResId) {
        this(context, cameraId, titleResId, entriesResId, valuesResId, 0);
    }

    /**
     * Construct a {@link CameraPreferenceFromResources}
     * @param titleResId Resource ID of a {@link String} to use as the preference title
     * @param entriesResId Resource ID of an array of {@link String}s to use as the preference
     *      entries
     * @param valuesResId Resource ID of an array of {@link String}s to use as the preference
     *      entry values
     * @param defaultResId Resource ID of a default value
     * @see CameraPreference#CameraPreference(Context, int)
     */
    public CameraPreferenceFromResources(Context context, int cameraId, int titleResId,
            int entriesResId, int valuesResId, int defaultResId) {
        super(context, cameraId);

        Resources res = context.getResources();
        mTitle = res.getString(titleResId);
        mEntries = res.getStringArray(entriesResId);
        mValues = res.getStringArray(valuesResId);
        if (defaultResId == 0) {
            mDefaultValue = null;
        } else {
            mDefaultValue = res.getString(defaultResId);
        }
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

    /**
     * {@inheritDoc}
     */
    protected String getDefaultValue() {
        return mDefaultValue;
    }
}
