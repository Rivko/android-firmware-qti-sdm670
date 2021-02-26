/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.imageedit.home;

import android.app.Activity;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.os.Bundle;
import android.util.Log;
import android.view.View;

import com.qualcomm.qti.qces.imageedit.softcut.R;
import com.qualcomm.qti.qces.util.ImageSharingIntentExtras;
import com.qualcomm.qti.qces.util.ImageUtils;

public class HomeActivity extends Activity {
    private HomeController controller;
    private static final String TAG = HomeActivity.class.getSimpleName();

    public boolean isFullMP = true;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.home_layout);

        Intent intent = getIntent();
        if (intent.hasExtra(ImageSharingIntentExtras.FULL_MP_MODE)) {
            isFullMP = intent.getBooleanExtra(ImageSharingIntentExtras.FULL_MP_MODE, true);
        } else {
            try {
                ActivityInfo activityInfo = getPackageManager().getActivityInfo(
                        this.getComponentName(), PackageManager.GET_META_DATA);
                Bundle aBundle = activityInfo.metaData;
                if (aBundle != null) {
                    isFullMP = aBundle.getBoolean(
                            "com.qualcomm.qti.qces.imageedit.home.HomeActivity.isFullMP", true);
                }
            } catch (NameNotFoundException e) {
                e.printStackTrace();
            }
        }
        Log.d(TAG, "Home activity : isFull MP = " + isFullMP);

        initialize();

        // If we are entering this without saved instance state, clean up all
        // temporary files
        if (savedInstanceState == null) {
            // Initial code, later use the right api's
            ImageUtils.deleteAllTemporaryFiles(this.getExternalCacheDir());
        }
    }

    void initialize() {
        controller = new HomeController(this);
    }

    public void galleryClicked(View v) {
        controller.galleryClicked();
    }

    public void cameraClicked(View v) {
        controller.cameraClicked();
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        controller.handleOnActivityResult(requestCode, resultCode, data);
    }
}
