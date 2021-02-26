/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.stage;

import android.app.Activity;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.view.View;

import com.qualcomm.qti.qces.imageedit.home.HomeActivity;
import com.qualcomm.qti.qces.imageedit.softcut.R;
import com.qualcomm.qti.qces.util.ImageSharingIntentExtras;

public class StageActivity extends Activity {

    private Uri stageUri;
    private StageView view;
    private StageController controller;
    public Boolean isFullMP;

    private static final String TAG = StageActivity.class.getSimpleName();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.d(TAG, "stage onCreate");

        setContentView(R.layout.stage_layout);
        initialize();
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent intent) {

        if (requestCode == R.integer.clone_intro_code) {
            controller.cloneDialogFinished(stageUri);
        }
    }

    private void initialize() {
        view = new StageView(this);
        controller = new StageController(this, view);

        Intent intent = getIntent();

        String stageUriString = intent
                .getStringExtra(ImageSharingIntentExtras.EXTRA_TARGET_IMAGE_URI);
        if (stageUriString != null) {
            stageUri = Uri.parse(stageUriString);
        }
        isFullMP = intent.getBooleanExtra(ImageSharingIntentExtras.FULL_MP_MODE, true);
        Log.i(TAG, "Stage activity : isFull MP = " + isFullMP);
        if (stageUri != null) {
            controller.initializeByImageUri(stageUri);
        }
    }

    public void newClicked(View view) {
        Intent galleryActivity = new Intent(this, HomeActivity.class);
        galleryActivity.putExtra(ImageSharingIntentExtras.FULL_MP_MODE, isFullMP);
        startActivity(galleryActivity);
    }

    public void saveClicked(View view) {
        controller.saveClicked();
    }

    public void effectsClicked(View view) {
        controller.effectsClicked(stageUri);
    }

    public void shareClicked(View view) {
        controller.shareClicked(stageUri);
    }

    public void cloneClicked(View view) {
        controller.cloneClicked(stageUri);
    }

    public void removeClicked(View view) {
        controller.removeClicked(stageUri);
    }
}
