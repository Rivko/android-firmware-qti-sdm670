/*
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.objecttrackerdemo;

import com.qualcomm.qti.objecttracker.cameraengine.preferences.CameraPreferenceManager;

import android.hardware.Camera;
import android.hardware.Camera.Parameters;
import android.os.Bundle;
import android.preference.PreferenceActivity;
import android.preference.PreferenceScreen;
import android.view.MenuItem;

/**
 * Implement a Camera/SCVE settings screen
 */
@SuppressWarnings("deprecation")
public class CameraSettingsActivity extends PreferenceActivity {
    /**
     * {@inheritDoc}
     */
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getActionBar().setDisplayHomeAsUpEnabled(true);
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
    @Override
    protected void onPostCreate(Bundle savedInstanceState) {
        super.onPostCreate(savedInstanceState);

        int cameraId = CameraPreferenceManager.getSelectedCameraId(this);
        Camera camera = Camera.open(cameraId);
        Parameters params = camera.getParameters();
        camera.release();

        PreferenceScreen ps = getPreferenceManager().createPreferenceScreen(this);
        setPreferenceScreen(ps);
        CameraPreferenceManager.addCameraPreferenceEntries(ps, cameraId, params);
    }
}
