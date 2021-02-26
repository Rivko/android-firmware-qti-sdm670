/*
 * Copyright (c) 2015,2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.fr_camera.camera.preferences;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.res.Resources;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CameraManager;
import android.preference.ListPreference;
import android.preference.Preference;
import android.preference.PreferenceManager;
import android.util.Log;

import com.qualcomm.qti.fr_camera.R;

import java.util.Objects;

/**
 * This class manages the selection of the active {@link CameraDevice}
 */
public class CameraSelectionManager {
    private static final String KEY = "camera_pref_camera_id";
    private static final String TAG = "SocialCamera";

    private final Context context;
    private final SharedPreferences sharedPreferences;
    private CharSequence[] desc;
    private String[] values;
    private EventListener listener;

    /**
     * Classes may implement this interface to be notified when the selected {@link CameraDevice}
     * changes
     */
    public interface EventListener {
        /**
         * This method will be invoked when the selected {@link CameraDevice} changes
         * @param cameraId the ID of the newly selected {@link CameraDevice}
         */
        void onSelectedCameraChanged(String cameraId);
    }

    /**
     * Construct a new {@link CameraSelectionManager}
     * @param context the active {@link Context}
     * @throws NullPointerException if context is null
     */
    public CameraSelectionManager(Context context) {
        Objects.requireNonNull(context, "context may not be null");
        this.context = context;
        sharedPreferences = PreferenceManager.getDefaultSharedPreferences(context);
        loadData();
    }

    /**
     * Set an {@link EventListener} to notify when the selected {@link CameraDevice} changes
     * @param listener the {@link EventListener} to notify, or null to clear the previously set
     *                 {@link EventListener}
     */
    public void setEventListener(EventListener listener) {
        if (this.listener != null) {
            sharedPreferences.unregisterOnSharedPreferenceChangeListener(sharedPrefsListener);
        }

        this.listener = listener;

        if (this.listener != null) {
            sharedPreferences.registerOnSharedPreferenceChangeListener(sharedPrefsListener);
        }
    }

    /**
     * Get the ID of the selected {@link CameraDevice}
     * @return the ID of the selected {@link CameraDevice}
     */
    public String getCameraId() {
        return PreferenceManager.getDefaultSharedPreferences(context).getString(KEY, values[0]);
    }

    /**
     * Create a {@link Preference} for selecting the {@link CameraDevice} to use
     * @return a new {@link Preference} for selecting the active {@link CameraDevice}
     */
    public Preference createPreference() {
        ListPreference lp = new ListPreference(context);
        lp.setKey(KEY);
        lp.setTitle(R.string.camera_pref_camera_id);
        lp.setEntries(desc);
        lp.setEntryValues(values);
        lp.setDefaultValue(values[0]);
        lp.setSummary("%s");
        return lp;
    }

    private void loadData() {
        CameraManager cm = (CameraManager)context.getSystemService(Context.CAMERA_SERVICE);
        String[] ids;
        try {
            ids = cm.getCameraIdList();
        } catch (CameraAccessException e) {
            Log.e(TAG, "Failed retrieving list of cameras", e);
            return;
        }

        Resources res = context.getResources();
        CharSequence[] desc = res.getTextArray(R.array.camera_pref_camera_id_desc);
        String[] values = res.getStringArray(R.array.camera_pref_camera_id_values);

        if (desc.length != values.length) {
            throw new IllegalStateException("Camera ID description and values arrays should be " +
                    "of equal length");
        }

        CharSequence[] filteredDesc = new CharSequence[desc.length];
        String[] filteredValues = new String[desc.length];
        int filteredIdx = 0;

        for (int i = 0; i < desc.length; i++) {
            for (String id : ids) {
                if (id.equals(values[i])) {
                    filteredDesc[filteredIdx] = desc[i];
                    filteredValues[filteredIdx++] = values[i];
                    break;
                }
            }
        }

        this.desc = new CharSequence[filteredIdx];
        this.values = new String[filteredIdx];
        System.arraycopy(filteredDesc, 0, this.desc, 0, filteredIdx);
        System.arraycopy(filteredValues, 0, this.values, 0, filteredIdx);
    }

    private final SharedPreferences.OnSharedPreferenceChangeListener sharedPrefsListener =
            new SharedPreferences.OnSharedPreferenceChangeListener() {
        @Override
        public void onSharedPreferenceChanged(SharedPreferences sharedPreferences, String key) {
            if (listener == null) {
                return;
            }

            if (KEY.equals(key)) {
                listener.onSelectedCameraChanged(getCameraId());
            }
        }
    };
}
