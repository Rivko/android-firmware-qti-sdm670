/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.fr_camera.camera.preferences;

import android.content.Context;
import android.content.res.Resources;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraDevice;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.Iterator;

/**
 * A specialization of {@link CameraPreference} that retrieves the list of supported values (and
 * the corresponding human-readable descriptions) from {@link Resources}
 */
/*package*/ abstract class CameraPreferenceFromResources extends CameraPreference {
    private final int nameResId;
    private final int descriptionsResId;
    private final int valuesResId;
    private final int defaultValueResId;

    private boolean loaded;
    private CharSequence name;
    private final ArrayList<Setting> settings = new ArrayList<>();
    private final Collection<Setting> settingsUnmodifiable =
            Collections.unmodifiableCollection(settings);
    private Setting defaultSetting;

    /**
     * Construct a new {@link CameraPreferenceFromResources}
     * @param nameResId the resource ID of a String specifying the preference name
     * @param descriptionsResId the resource ID of a CharSequence[] specifying the preference
     *                          setting descriptions
     * @param valuesResId the resource ID of a String[] specifying the preference setting values
     * @param defaultValueResId the resource ID of a String specifying the default preference
     *                          setting value, or 0 to use the first available setting as the
     *                          default
     * @see CameraPreference#CameraPreference(Context, String, CameraCharacteristics)
     */
    public CameraPreferenceFromResources(Context context, String cameraId,
                                         CameraCharacteristics characteristics, int nameResId,
                                         int descriptionsResId, int valuesResId,
                                         int defaultValueResId) {
        super(context, cameraId, characteristics);

        this.nameResId = nameResId;
        this.descriptionsResId = descriptionsResId;
        this.valuesResId = valuesResId;
        this.defaultValueResId = defaultValueResId;
    }

    @Override
    protected CharSequence getName() {
        loadPreferenceData();
        return name;
    }

    @Override
    protected Collection<Setting> getSettings() {
        loadPreferenceData();
        return settingsUnmodifiable;
    }

    @Override
    protected Setting getDefaultSetting() {
        loadPreferenceData();
        return defaultSetting;
    }

    /**
     * Subclasses should implement this to filter the provided {@link Setting}s and remove those
     * that aren't supported by the current {@link CameraDevice}
     * @param settings the {@link Collection} of {@link Setting}s to filter
     */
    protected abstract void filterSettings(ArrayList<Setting> settings);

    private void loadPreferenceData() {
        if (!loaded) {
            loaded = true;
            Resources res = context.getResources();
            name = res.getText(nameResId);
            String[] values = res.getStringArray(valuesResId);
            CharSequence[] descriptions = res.getTextArray(descriptionsResId);

            boolean hasDefaultValue = (defaultValueResId != 0);
            String defaultValue = (hasDefaultValue ? res.getString(defaultValueResId) : null);

            if (values.length != descriptions.length) {
                throw new IllegalStateException("Length of values array resource does not match " +
                        "length of descriptions array resource");
            }

            for (int i = 0; i < values.length; i++) {
                settings.add(new Setting(values[i], descriptions[i]));
            }

            filterSettings(settings);

            if (!settings.isEmpty()) {
                if (hasDefaultValue) {
                    Iterator<Setting> it = settings.iterator();
                    while (it.hasNext()) {
                        Setting s = it.next();
                        if (defaultValue.equals(s.value)) {
                            it.remove();
                            settings.add(0, s);
                            break;
                        }
                    }
                }

                defaultSetting = settings.get(0);
            }
        }
    }
}
