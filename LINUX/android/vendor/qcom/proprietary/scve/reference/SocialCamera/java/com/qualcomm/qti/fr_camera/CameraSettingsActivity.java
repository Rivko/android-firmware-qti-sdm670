/*
 * Copyright (c) 2015,2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.fr_camera;

import android.os.Bundle;
import android.preference.PreferenceActivity;
import android.preference.PreferenceCategory;
import android.preference.PreferenceScreen;
import android.util.Log;
import android.view.MenuItem;

import com.qualcomm.qti.fr_camera.metrics.MetricsPreferenceManager;
import com.qualcomm.qti.fr_camera.camera.preferences.CameraPreferenceManager;
import com.qualcomm.qti.fr_camera.camera.preferences.CameraSelectionManager;
import com.qualcomm.qti.fr_camera.fr.FacePreferences;

/**
 * This activity allows the user to configure camera settings
 */
@SuppressWarnings("deprecation")
public class CameraSettingsActivity extends PreferenceActivity {
    private static final String TAG = "SocialCamera";

    private CameraSelectionManager cameraSelectionManager;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        //noinspection ConstantConditions
        getActionBar().setDisplayHomeAsUpEnabled(true);

        cameraSelectionManager = new CameraSelectionManager(this);
        cameraSelectionManager.setEventListener(listener);

        constructPreferences();
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();
        if (id == android.R.id.home) {
            // Home/Up was pressed in Action Bar. Don't use NavUtils.navigateUpFromSameTask(this),
            // it won't call onActivityResult in our parent Activity.
            finish();
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

    private void constructPreferences() {
        PreferenceScreen ps = getPreferenceManager().createPreferenceScreen(this);
        setPreferenceScreen(ps);

        PreferenceCategory cameraSettingsCategory = new PreferenceCategory(this);
        cameraSettingsCategory.setTitle(R.string.camera_settings_category_camera_settings);
        ps.addPreference(cameraSettingsCategory);

        cameraSettingsCategory.addPreference(cameraSelectionManager.createPreference());

        CameraPreferenceManager cameraPreferenceManager = new CameraPreferenceManager(this,
                cameraSelectionManager.getCameraId());
        cameraPreferenceManager.addCameraPreferences(cameraSettingsCategory);

        PreferenceCategory category = new PreferenceCategory(this);
        category.setTitle(R.string.camera_settings_category_fr_settings);
        ps.addPreference(category);

        FacePreferences.addFaceRecognizerPreferences(category);

        PreferenceCategory debugSettingsCategory = new PreferenceCategory(this);
        debugSettingsCategory.setTitle(R.string.camera_settings_category_debug_settings);
        ps.addPreference(debugSettingsCategory);

        new MetricsPreferenceManager(this).addMetricsPreferences(debugSettingsCategory);
        FacePreferences.addDebugPreferences(debugSettingsCategory);
        cameraPreferenceManager.addDebugPreferences(debugSettingsCategory);
    }

    private CameraSelectionManager.EventListener listener =
            new CameraSelectionManager.EventListener() {
        @Override
        public void onSelectedCameraChanged(String cameraId) {
            Log.v(TAG, "Selected camera changed, reconstructing camera preferences");
            constructPreferences();
        }
    };
}
