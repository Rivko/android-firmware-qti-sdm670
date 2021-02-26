/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.fr_camera.camera.preferences;

import android.content.Context;
import android.content.SharedPreferences;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CaptureRequest;
import android.preference.ListPreference;
import android.preference.Preference;
import android.preference.PreferenceManager;

import java.util.Collection;
import java.util.Objects;

/**
 * Base class for all {@link android.hardware.camera2} camera preferences
 */
/*package*/ abstract class CameraPreference {
    private static final String PREFERENCE_KEY_PREFIX = "camera_pref";

    /**
     * This class encapsulates a single setting for this {@link CameraPreference}
     */
    protected static class Setting {
        public final String value;
        public final CharSequence description;

        public Setting(String value, CharSequence description) {
            this.value = value;
            this.description = description;
        }
    }

    protected final Context context;
    protected final CameraCharacteristics characteristics;

    private final String cameraId;

    /**
     * Construct a new {@link CameraPreference}
     * @param context the active {@link Context}
     * @param cameraId the camera identifier of the {@link CameraDevice} for which to configure
     *                 preferences
     * @param characteristics the {@link CameraCharacteristics} for the specified
     *                        {@link CameraDevice}
     * @throws NullPointerException if context, cameraId, or characteristics are null
     * @throws IllegalArgumentException if cameraId is an empty string
     */
    public CameraPreference(Context context, String cameraId,
                            CameraCharacteristics characteristics) {
        Objects.requireNonNull(context, "context may not be null");
        Objects.requireNonNull(cameraId, "cameraId may not be null");
        Objects.requireNonNull(characteristics, "characteristics may not be null");
        if (cameraId.isEmpty()) {
            throw new IllegalArgumentException("cameraId may not be an empty string");
        }

        this.context = context;
        this.cameraId = cameraId;
        this.characteristics = characteristics;
    }

    /**
     * Tests if this {@link CameraPreference} is supported for this camera
     * @return true if this {@link CameraPreference} is supported (ie can be configured, and has at
     *         least 2 possible settings), else false
     */
    public boolean isSupported() {
        return getSettings().size() > 1;
    }

    /**
     * Create a {@link Preference} for the current {@link CameraPreference}
     * @return a {@link Preference} object, or null if {@link #isSupported()} returns false for this
     *         {@link CameraPreference}
     */
    public Preference createPreference() {
        if (!isSupported()) {
            return null;
        }

        Collection<Setting> settings = getSettings();
        CharSequence[] descriptions = new CharSequence[settings.size()];
        String[] values = new String[descriptions.length];
        int i = 0;
        for (Setting s : settings) {
            descriptions[i] = s.description;
            values[i++] = s.value;
        }

        ListPreference lp = new ListPreference(context);
        lp.setKey(getPreferenceKey());
        lp.setTitle(getName());
        lp.setEntries(descriptions);
        lp.setEntryValues(values);
        lp.setDefaultValue(getDefaultSetting().value);
        lp.setSummary("%s");

        return lp;
    }

    /**
     * Apply this {@link CameraPreference} to the provided {@link CaptureRequest.Builder}
     * @param request the {@link CaptureRequest.Builder} to apply this {@link CameraPreference} to
     * @param isPreviewRequest if request is for camera preview
     * @throws NullPointerException if request is null
     */
    public void apply(CaptureRequest.Builder request, boolean isPreviewRequest) {
        Objects.requireNonNull(request, "request may not be null");

        if (!isSupported()) {
            return;
        }

        doApply(request, isPreviewRequest, getValue());
    }

    /**
     * Get the {@link SharedPreferences} key which will be read/written by this
     * {@link CameraPreference}
     * @return a String containing the {@link SharedPreferences} key
     */
    /*package*/ String getPreferenceKey() {
        return PREFERENCE_KEY_PREFIX + "_" + cameraId + "_" + getPreferenceKeyName();
    }

    /**
     * Get the current value of this {@link CameraPreference}
     * @return a String containing the value of this setting, or null if {@link #isSupported()}
     *         returns false for this {@link CameraPreference}
     */
    protected String getValue() {
        return PreferenceManager.getDefaultSharedPreferences(context).getString(
                getPreferenceKey(), getDefaultSetting().value);
    }

    /**
     * Subclasses should implement this to provide the base name of the preference key in which this
     * preference should be saved. It will be decorated with a prefix and the specified camera ID
     * @return a String containing the preference key base name
     */
    protected abstract String getPreferenceKeyName();

    /**
     * Subclasses should implement this to provide the human-readable name of this
     * {@link CameraPreference}
     * @return a {@link CharSequence} containing the human-readable name
     */
    protected abstract CharSequence getName();

    /**
     * Subclasses should implement this to provide a {@link Collection} of the supported
     * {@link Setting}s for this {@link CameraPreference}
     * @return a {@link Collection} of the supported settings
     */
    protected abstract Collection<Setting> getSettings();

    /**
     * Subclasses should implement this to provide the default {@link Setting} for this
     * {@link CameraPreference}. The returned {@link Setting} must be one of the values returned by
     * {@link #getSettings()}.
     * @return the default {@link Setting}
     */
    protected abstract Setting getDefaultSetting();

    /**
     * Subclasses should implement this to apply this {@link CameraPreference} to the provided
     * {@link CaptureRequest.Builder}
     * @param request the {@link CaptureRequest.Builder} to apply this {@link CameraPreference} to
     * @param isPreviewRequest if request is for camera preview
     * @param value the {@link CameraPreference} value to apply. Must be one of the values returned
     *              in the {@link Collection} returned by {@link #getSettings()}
     */
    protected abstract void doApply(CaptureRequest.Builder request, boolean isPreviewRequest,
                                    String value);
}
