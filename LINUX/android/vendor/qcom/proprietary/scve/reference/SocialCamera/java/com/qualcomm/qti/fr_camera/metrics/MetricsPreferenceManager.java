/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.fr_camera.metrics;

import android.content.Context;
import android.preference.Preference;
import android.preference.PreferenceGroup;
import android.preference.PreferenceManager;
import android.preference.SwitchPreference;

import com.qualcomm.qti.fr_camera.R;

import java.util.Objects;

/**
 * This class provides a common interface to all settings for managing {@link Metric}s
 */
public class MetricsPreferenceManager {
    private static final String DEBUG_PREF_DISPLAY_METRICS = "debug_pref_display_metrics";

    private final Context context;

    /**
     * Construct a new {@link MetricsPreferenceManager}
     * @param context the active {@link Context}
     * @throws NullPointerException if context is null
     */
    public MetricsPreferenceManager(Context context) {
        Objects.requireNonNull(context, "context may not be null");
        this.context = context;
    }

    /**
     * Add {@link Preference}s for managing {@link Metric}s to the specified {@link PreferenceGroup}
     * @param preferenceGroup the {@link PreferenceGroup} to which to add {@link Metric}s
     *                        preferences
     * @throws NullPointerException if preferenceGroup is null
     */
    public void addMetricsPreferences(PreferenceGroup preferenceGroup) {
        Objects.requireNonNull(preferenceGroup, "preferenceGroup may not be null");

        SwitchPreference pref = new SwitchPreference(context);
        pref.setKey(DEBUG_PREF_DISPLAY_METRICS);
        pref.setTitle(R.string.debug_pref_display_metrics);
        pref.setDefaultValue(context.getResources().getBoolean(
                R.bool.debug_pref_display_metrics_default_value));
        preferenceGroup.addPreference(pref);
    }

    /**
     * Get the current value of the Display Metrics setting
     * @return true if metrics should be displayed, else false
     */
    public boolean getDisplayMetrics() {
        return PreferenceManager.getDefaultSharedPreferences(context)
                .getBoolean(DEBUG_PREF_DISPLAY_METRICS, context.getResources().getBoolean(
                        R.bool.debug_pref_display_metrics_default_value));
    }
}
