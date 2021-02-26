/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.panoramaui;

import android.annotation.TargetApi;
import android.hardware.Camera;
import android.hardware.Camera.Parameters;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraManager;
import android.os.Build;
import android.os.Bundle;
import android.preference.PreferenceActivity;
import android.preference.PreferenceScreen;
import android.util.Log;
import android.view.MenuItem;

import com.qualcomm.qti.panoramaui.preferences.CameraPreferenceManager;

/**
 * Implement a Camera/Panorama settings screen
 */
@SuppressWarnings("deprecation")
public class CameraSettingsActivity extends PreferenceActivity {
    private static final String TAG = CameraSettingsActivity.class.getSimpleName();
    public static final String INTENT_KEY_LEGACY_CAMERA = "legacy_camera";
    private boolean isLegacyCamera = false;

    /**
     * {@inheritDoc}
     */
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getActionBar().setDisplayHomeAsUpEnabled(true);

        isLegacyCamera = getIntent().getBooleanExtra(INTENT_KEY_LEGACY_CAMERA, false);
        Log.d(TAG, "legacy: " + isLegacyCamera);
    }

    /**
     * {@inheritDoc}
     */
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

    /**
     * {@inheritDoc}
     */
    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    @Override
    protected void onPostCreate(Bundle savedInstanceState) {
        super.onPostCreate(savedInstanceState);

        PreferenceScreen ps = getPreferenceManager().createPreferenceScreen(this);
        setPreferenceScreen(ps);

        int cameraId = CameraPreferenceManager.getSelectedCameraId(this);

        if (!isLegacyCamera) {
            CameraManager cameraManager = (CameraManager) getSystemService(CAMERA_SERVICE);
            try {
                CameraCharacteristics characteristics = cameraManager
                        .getCameraCharacteristics(Integer.toString(cameraId));
                CameraPreferenceManager.addCamera2PreferenceEntries(ps, cameraId, characteristics);
            } catch (CameraAccessException e) {
                Log.e(TAG, "Failed to get characteristics for camera ID [" + cameraId + "]", e);
            }
        } else {
            Camera camera = Camera.open(cameraId);
            Parameters params = camera.getParameters();
            camera.release();
            CameraPreferenceManager.addLegacyCameraPreferenceEntries(ps, cameraId, params);
        }
    }
}
