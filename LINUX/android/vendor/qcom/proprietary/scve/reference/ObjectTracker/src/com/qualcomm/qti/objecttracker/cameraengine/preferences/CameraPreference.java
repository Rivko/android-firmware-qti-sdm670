/*
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.objecttracker.cameraengine.preferences;

import java.util.ArrayList;

import android.content.Context;
import android.content.SharedPreferences;
import android.hardware.Camera;
import android.hardware.Camera.Parameters;
import android.preference.ListPreference;
import android.preference.Preference;
import android.preference.PreferenceManager;

/**
 * This class manages a single {@link Camera} setting. It both constructs an appropriate {@link
 * Preference} to allow configuration, and applies the setting to a {@link Camera}.
 */
@SuppressWarnings("deprecation")
/*package*/ abstract class CameraPreference {

    private final Context mContext;
    private final SharedPreferences mPrefs;
    private final int mCameraId;
    private final Parameters mParams;

    // Note: values in this block are only valid when mPreferenceDataCached is true
    private boolean mPreferenceDataCached;
    private String mKey;
    private String[] mEntries;
    private String[] mValues;
    private String mDefaultValue;

    /**
     * Construct a {@link CameraPreference}
     * @param context {@link Context} in which the setting will be stored
     * @param cameraId ID of the {@link Camera} to which the setting applies
     * @param p {@link Parameters} for this {@link Camera} ID
     */
    protected CameraPreference(Context context, int cameraId, Parameters p) {
        mContext = context;
        mPrefs = PreferenceManager.getDefaultSharedPreferences(mContext);
        mCameraId = cameraId;
        mParams = p;
    }

    /**
     * Apply the stored setting value (or default value, if no setting is stored) to the {@link
     * Parameters} with which this object was created.
     */
    public void apply() {
        cachePreferenceData();
        doApply(mPrefs.getString(mKey, mDefaultValue), mParams);
    }

    /**
     * Erase the stored setting value (if set)
     */
    public void clear() {
        cachePreferenceData();
        mPrefs.edit().remove(mKey).apply();
    }

    /**
     * Create a {@link Preference} representing this {@link Camera} setting
     * @return a {@link Preference} which can be used to control this setting
     */
    public Preference createPreferenceEntry() {
        cachePreferenceData();
        ListPreference lp = new ListPreference(mContext);
        lp.setKey(mKey);
        lp.setTitle(getTitle());
        lp.setEntries(mEntries);
        lp.setEntryValues(mValues);
        lp.setDefaultValue(mDefaultValue);
        lp.setSummary("%s");
        return lp;
    }

    /**
     * Helper method to load data associated with this {@link CameraPreference}
     */
    private void cachePreferenceData() {
        if (mPreferenceDataCached) {
            return;
        }
        mPreferenceDataCached = true;

        mKey = constructPreferenceKey(getBaseKey(), mCameraId);

        String[] entries = getEntries();
        String[] values = getValues();
        if (entries.length != values.length) {
            throw new RuntimeException("Entries and Values should be same length");
        }

        String[] supportedValues = getSupportedValues(mParams);
        if (supportedValues != null) {
            ArrayList<String> filteredEntries = new ArrayList<String>(entries.length);
            ArrayList<String> filteredValues = new ArrayList<String>(entries.length);
            for (int i = 0; i < entries.length; i++) {
                for (String s : supportedValues) {
                    if (values[i].equals(s)) {
                        filteredEntries.add(entries[i]);
                        filteredValues.add(values[i]);
                        break;
                    }
                }
            }
            int filteredLength = filteredEntries.size();
            entries = filteredEntries.toArray(new String[filteredLength]);
            values = filteredValues.toArray(new String[filteredLength]);
        }
        mEntries = entries;
        mValues = values;

        mDefaultValue = getDefaultValue();
        if (mDefaultValue == null || !contains(mValues, mDefaultValue)) {
            mDefaultValue = mValues[0];
        }

        String curValue = mPrefs.getString(mKey, null);
        if (curValue != null && !contains(mValues, curValue)) {
            clear();
        }
    }

    /**
     * Helper function to construct a {@link Preference} key associated with a specific {@link
     * Camera} ID
     * @param baseKey Base {@link Preference} key value
     * @param cameraId ID of the {@link Camera} for this setting
     * @return a {@link Camera} ID-specific key
     */
    public static String constructPreferenceKey(String baseKey, int cameraId) {
        return baseKey + cameraId;
    }

    /**
     * Helper function to check it a specific {@link String} is in an unsorted array
     * @param set Set of {@link String}s to search
     * @param val Value to search for in set
     * @return true if val is in set, else false
     */
    private static boolean contains(String[] set, String val) {
        for (String s : set) {
            if (val.equals(s)) {
                return true;
            }
        }
        return false;
    }

    /**
     * Subclasses should implement this method and return the base key for their setting
     * @return a {@link String} representing the base key for a setting
     * @see #constructPreferenceKey(String, int)
     */
    protected abstract String getBaseKey();

    /**
     * Subclasses should implement this method and apply the specified setting to the provided
     * {@link Parameters} instance
     * @param val Setting value to apply
     * @param p {@link Parameters} instance to apply the setting to
     */
    protected abstract void doApply(String val, Parameters p);

    /**
     * Subclasses should implement this method and return the human-readable title for this setting
     * @return a {@link String} containing the title for this setting
     */
    protected abstract String getTitle();

    /**
     * Subclasses should implement this method and return the set of human-readable entries for this
     * setting
     * @return an array of {@link String}s, each representing an entry for this setting
     */
    protected abstract String[] getEntries();

    /**
     * Subclasses should implement this method and return the set of values which correspond to the
     * entries for this setting
     * @return an array of {@link String}s, each representing the value which corresponds to the
     *      corresponding entry returned from {@link #getEntries()}. The length of this array must
     *      match that returned by {@link #getEntries()}
     */
    protected abstract String[] getValues();

    /**
     * Subclasses should implement this method and return the subset of {@link #getValues()} which
     * are currently available. This can be used to filter a list of available settings to those
     * made available by a particular {@link Parameters} instance.
     * @param p {@link Parameters} to reference when creating the subset of {@link #getValues()}
     * @return an array of {@link String}s which are a strict subset of those returned by {@link
     *      #getValues()}, or null if no filtering needs to be performed.
     */
    protected abstract String[] getSupportedValues(Parameters p);

    /**
     * Subclasses can override this method to provide a default value (from {@link #getValues()})
     * for this setting. If not implemented, the first value in {@link #getValues()} will be the
     * default.
     * @return one of the {@link String}s returned by {@link #getValues()} to be used as the default
     *      value for this setting, or null if the first entry in {@link #getValues()} should be
     *      the default.
     */
    protected String getDefaultValue() {
        return null;
    }
}
