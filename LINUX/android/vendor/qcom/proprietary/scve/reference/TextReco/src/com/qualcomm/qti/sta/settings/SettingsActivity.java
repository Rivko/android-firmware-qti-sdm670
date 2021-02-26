/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies, Inc. Confidential and Proprietary.
 */
package com.qualcomm.qti.sta.settings;

import java.util.Set;

import android.app.ActionBar;
import android.app.FragmentManager;
import android.app.FragmentTransaction;
import android.os.Bundle;
import android.preference.EditTextPreference;
import android.preference.ListPreference;
import android.preference.MultiSelectListPreference;
import android.preference.Preference;
import android.preference.PreferenceActivity;
import android.preference.PreferenceFragment;
import android.preference.PreferenceManager;
import android.preference.PreferenceScreen;
import android.view.MenuItem;
import android.widget.Toast;

import com.qualcomm.qti.sta.R;
import com.qualcomm.qti.sta.data.Utils;
import com.qualcomm.qti.sta.engine.JniConstants;
import com.qualcomm.qti.sta.engine.JniManager;

/**
 * Activity for showing application preferences
 */
public class SettingsActivity extends PreferenceActivity {
    //private static final String LOG_TAG = "SettingsActivity";

    // instance for avoiding ocr manager getting wrapped up
    private JniManager ocrManager = null;

    /**
     * {@inheritDoc}
     */
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        String localeSettings = getIntent().getStringExtra(Utils.INTENT_KEY_LOCALE);

        FragmentManager fragmentManager = getFragmentManager();
        FragmentTransaction fragmentTransaction = fragmentManager.beginTransaction();
        AppPreferencesFragment fragment = new AppPreferencesFragment();
        fragment.setData(this, localeSettings);
        fragmentTransaction.replace(android.R.id.content, fragment);
        fragmentTransaction.commit();
        PreferenceManager.setDefaultValues(this, R.xml.preferences, false);

        ActionBar actionBar = getActionBar();
        actionBar.setDisplayHomeAsUpEnabled(true);

        ocrManager = JniManager.getInstance();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onStop() {
        super.onStop();

        ocrManager.release();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();
        if (id == android.R.id.home) {
            finish();
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

    /**
     * This fragment shows the preference list.
     */
    public static class AppPreferencesFragment extends PreferenceFragment {
        private SettingsActivity activity = null;
        private String localeSettings = null;

        /**
         * Constructor for the class
         *
         * @param activity
         *            activity class
         */
        public void setData(SettingsActivity activity, String localeSettings) {
            this.activity = activity;
            this.localeSettings = localeSettings;
        }

        /**
         * {@inheritDoc}
         */
        @Override
        public void onCreate(Bundle savedInstanceState) {
            super.onCreate(savedInstanceState);

            addPreferencesFromResource(R.xml.preferences);

            if (localeSettings != null) {
                PreferenceScreen screen = getPreferenceScreen();
                Preference pref = getPreferenceManager().findPreference(
                        PreferenceData.KEY_STA_LOCALE_TYPE);
                screen.removePreference(pref);
            }

            setStringListSummary(PreferenceData.KEY_TEXT_RECORD_MODE);
            //setValueNameSummary(PreferenceData.KEY_IC_REGION_EXTRACTION_MODE,
            //        R.array.setting_ic_region_ex_mode_values, R.array.setting_ic_region_ex_modes);
            setValueNameSummary(PreferenceData.KEY_IC_IMAGE_ENHANCED_MODE,
                    R.array.setting_ic_image_en_mode_values, R.array.setting_ic_image_en_modes);
            setStaTypesSummary(PreferenceData.KEY_STA_DATA_TYPES);
            if (localeSettings == null) {
                setValueNameSummary(PreferenceData.KEY_STA_LOCALE_TYPE,
                        R.array.setting_sta_locale_type_values, R.array.setting_sta_locale_type);
            }

            setEditIntegerSummary(PreferenceData.KEY_CAMERA_STEADY_DURATION,
                    R.integer.min_camera_steady_duration, R.integer.max_camera_steady_duration);
            setEditFloatSummary(PreferenceData.KEY_CAMERA_MOVE_THRESHOLD,
                    R.dimen.min_camera_move_threshold, R.dimen.max_camera_move_threshold);
        }

        private void setEditIntegerSummary(String key, final int minResourceId,
                final int maxResourceId) {
            final EditTextPreference preference = (EditTextPreference) findPreference(key);
            preference.setSummary(preference.getText());
            preference.setOnPreferenceChangeListener(new Preference.OnPreferenceChangeListener() {
                /**
                 * {@inheritDoc}
                 */
                @Override
                public boolean onPreferenceChange(Preference preference, Object newValue) {
                    int minimumValue = getResources().getInteger(minResourceId);
                    int maximumValue = getResources().getInteger(maxResourceId);
                    int newNumberValue = Integer.parseInt(newValue.toString());
                    if (newNumberValue < minimumValue || newNumberValue > maximumValue) {
                        String message = String.format(
                                getResources().getString(R.string.error_min_max_limit),
                                minimumValue, maximumValue);
                        Toast.makeText(activity, message, Toast.LENGTH_LONG).show();
                        return false;
                    } else {
                        preference.setSummary((String) newValue);
                    }
                    return true;
                }
            });
        }

        private void setEditFloatSummary(String key, final int minResourceId,
                final int maxResourceId) {
            final EditTextPreference preference = (EditTextPreference) findPreference(key);
            preference.setSummary(preference.getText());

            preference.setOnPreferenceChangeListener(new Preference.OnPreferenceChangeListener() {
                /**
                 * {@inheritDoc}
                 */
                @Override
                public boolean onPreferenceChange(Preference preference, Object newValue) {
                    float minimumValue = Utils.getFloatFromDimen(getResources(), minResourceId);
                    float maximumValue = Utils.getFloatFromDimen(getResources(), maxResourceId);
                    float newNumberValue = Float.parseFloat(newValue.toString());
                    if (newNumberValue < minimumValue || newNumberValue > maximumValue) {
                        String message = String.format(
                                getResources().getString(R.string.error_min_max_limit),
                                minimumValue, maximumValue);
                        Toast.makeText(activity, message, Toast.LENGTH_LONG).show();
                        return false;
                    } else {
                        preference.setSummary((String) newValue);
                    }
                    return true;
                }
            });
        }

        private void setStaTypesSummary(String key) {
            MultiSelectListPreference multiSelectPreference = (MultiSelectListPreference) findPreference(key);
            Set<String> values = multiSelectPreference.getValues();
            String summaryText = setSummary(values);
            multiSelectPreference.setSummary(summaryText);
            multiSelectPreference
                    .setOnPreferenceChangeListener(new Preference.OnPreferenceChangeListener() {
                        /**
                         * {@inheritDoc}
                         */
                        @Override
                        public boolean onPreferenceChange(Preference preference, Object newValue) {
                            if (newValue instanceof Set<?>) {
                                Set<?> values = (Set<?>) newValue;
                                String summaryText = setSummary(values);
                                if (!summaryText.isEmpty()) {
                                    preference.setSummary(summaryText);
                                } else {
                                    String message = getResources().getString(
                                            R.string.error_no_sta_data_selected);
                                    Toast.makeText(activity, message, Toast.LENGTH_LONG).show();
                                    return false;
                                }
                            }
                            return true;
                        }
                    });
        }

        private String setSummary(Set<?> values) {
            String[] flagNames = getResources().getStringArray(
                    R.array.setting_sta_data_type_options);
            int length = values.size();
            String summaryText = "";
            int element = 1;
            for (Object value : values) {
                if (value instanceof String) {
                    String flagName = null;
                    int flagValue = Integer.parseInt((String) value);
                    switch (flagValue) {
                    case JniConstants.DATA_TYPE_EMAIL:
                        flagName = flagNames[0];
                        break;
                    case JniConstants.DATA_TYPE_URL:
                        flagName = flagNames[1];
                        break;
                    case JniConstants.DATA_TYPE_PHONENUMBER:
                        flagName = flagNames[2];
                        break;
                    case JniConstants.DATA_TYPE_ADDRESS:
                        flagName = flagNames[3];
                        break;
                    case JniConstants.DATA_TYPE_DATETIME:
                        flagName = flagNames[4];
                        break;
                    case JniConstants.DATA_TYPE_GENERAL:
                        flagName = flagNames[5];
                        break;
                    }

                    if(flagName != null) {
                        summaryText = summaryText + flagName;
                        if (element < length) {
                            summaryText = summaryText + ", ";
                        }
                    }
                }
                element++;
            }
            return summaryText;
        }

        private void setStringListSummary(String key) {
            final ListPreference preference = (ListPreference) findPreference(key);
            preference.setSummary(preference.getValue());

            preference.setOnPreferenceChangeListener(new Preference.OnPreferenceChangeListener() {
                /**
                 * {@inheritDoc}
                 */
                @Override
                public boolean onPreferenceChange(Preference preference, Object newValue) {
                    preference.setSummary((String) newValue);
                    return true;
                }
            });
        }

        private void setValueNameSummary(String key, final int valueArrayId, final int nameArrayId) {
            final ListPreference preference = (ListPreference) findPreference(key);
            preference.setSummary(preference.getEntry().toString());

            preference.setOnPreferenceChangeListener(new Preference.OnPreferenceChangeListener() {
                /**
                 * {@inheritDoc}
                 */
                @Override
                public boolean onPreferenceChange(Preference preference, Object newValue) {
                    String[] settingValues = getResources().getStringArray(valueArrayId);
                    String[] settingTypes = getResources().getStringArray(nameArrayId);

                    for (int i = 0; i < settingValues.length; i++) {
                        String value = settingValues[i];
                        if (value.equals(newValue)) {
                            preference.setSummary(settingTypes[i]);
                            break;
                        }
                    }

                    return true;
                }
            });
        }
    }
}
