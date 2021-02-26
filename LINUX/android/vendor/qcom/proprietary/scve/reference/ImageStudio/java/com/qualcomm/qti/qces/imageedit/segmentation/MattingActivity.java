/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.imageedit.segmentation;

import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.graphics.Rect;
import android.net.Uri;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.View;
import android.widget.ImageButton;

import com.qualcomm.qti.qces.imageedit.softcut.R;
import com.qualcomm.qti.qces.util.ImageSharingIntentExtras;

public class MattingActivity extends AbstractSegmentationActivity {

    public static final String prefName = "com.qualcomm.qti.qces.imageedit.segmentation";

    private MattingModel model;
    private MattingController controller;
    private Uri imageUri;
    private Uri maskUri;

    private ImageButton strokeWidthSmall;
    private ImageButton strokeWidthMedium;
    private ImageButton strokeWidthLarge;

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        model = new MattingModel(this);
        controller = new MattingController(this, model);
        super.setModel(model);
        super.setController(controller);

        super.onCreate(savedInstanceState);
        setContentView(R.layout.matting_layout);

        initializeViews();
        initializeMaskAlphaView();

        SharedPreferences prefs = this.getSharedPreferences(prefName,
                Context.MODE_PRIVATE);
        controller.checkShowHelp(prefs);

    }

    @Override
    protected void onStart() {
        super.onStart();
        imageUri = maskUri = null;
        Intent intent = getIntent();
        String imageUriString = intent
                .getStringExtra(ImageSharingIntentExtras.EXTRA_TARGET_IMAGE_URI);
        if (imageUriString != null) {
            imageUri = Uri.parse(imageUriString);
        }
        String maskUriString = intent.getStringExtra(ImageSharingIntentExtras.EXTRA_MASK_IMAGE_URI);
        if (maskUriString != null) {
            maskUri = Uri.parse(maskUriString);
        }
        nextActivity = intent.getStringExtra(ImageSharingIntentExtras.NEXT_ACTIVITY);
        isFullMP = intent.getBooleanExtra(ImageSharingIntentExtras.FULL_MP_MODE, true);
        controller.setStateFromBundle(intent.getBundleExtra("softCutStateBundle"));

        if (imageUri != null && maskUri != null) {
            controller.initialize(imageUri, maskUri);
        }
    }

    @Override
    protected void onStop() {

        super.onStop();
        controller.onStop();
    }

    @Override
    public boolean dispatchTouchEvent(MotionEvent event) {
        if (event.getActionMasked() == MotionEvent.ACTION_DOWN) {
            // we don't dismiss the hint if user is pressing cancel/back button
            ImageButton cancelButton = (ImageButton)findViewById(R.id.matting_back_button);
            Rect buttonRect = new Rect();
            cancelButton.getGlobalVisibleRect(buttonRect);
            if (buttonRect.contains((int)event.getX(), (int)event.getY()) == false)
                controller.dismissHint();
        }
        return super.dispatchTouchEvent(event);
    }

    void initializeViews() {

        initializeSpinner(R.id.matting_spinner, R.id.matting_spinner_background);
        initializeImageView(R.id.matting_preview_image);
        initializeMaskAlphaView(R.id.matting_mask_alpha_view);
        initializeOutputMaskView(R.id.matting_output_mask_view);
        initializeOutputMaskViewColors(new float[] { 0, 0, 0, 0, 255f, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, -0.5f, 128f });

        strokeWidthSmall = (ImageButton)findViewById(R.id.matting_small_stroke);
        strokeWidthMedium = (ImageButton)findViewById(R.id.matting_medium_stroke);
        strokeWidthLarge = (ImageButton)findViewById(R.id.matting_large_stroke);

        initializeHelpButton(R.id.matting_help_button);
        initializeCancelButton(R.id.matting_back_button);
        initializeUndoButton(R.id.matting_undo_button);
        initializeRedoButton(R.id.matting_redo_button);
        initializeDoneButton(R.id.done_button);

    }

    void refreshStrokeButtons() {
        switch (model.strokeSizeSelected) {
        case SMALL:
            getMaskAlphaView().setMattingStrokeSize(
                    getResources().getDimension(R.dimen.stroke_width_small));// Stroke.SMALL_MATTING_STROKE
            strokeWidthSmall.setSelected(true);
            strokeWidthMedium.setSelected(false);
            strokeWidthLarge.setSelected(false);
            break;
        case MEDIUM:
            getMaskAlphaView().setMattingStrokeSize(
                    getResources().getDimension(R.dimen.stroke_width_medium));// Stroke.MEDIUM_MATTING_STROKE
            strokeWidthSmall.setSelected(false);
            strokeWidthMedium.setSelected(true);
            strokeWidthLarge.setSelected(false);
            break;
        case LARGE:
            getMaskAlphaView().setMattingStrokeSize(
                    getResources().getDimension(R.dimen.stroke_width_large));// Stroke.LARGE_MATTING_STROKE
            strokeWidthSmall.setSelected(false);
            strokeWidthMedium.setSelected(false);
            strokeWidthLarge.setSelected(true);
            break;
        }
    }

    public void smallStrokeClicked(View view) {
        controller.smallStrokeClicked();
    }

    public void mediumStrokeClicked(View view) {
        controller.mediumStrokeClicked();
    }

    public void largeStrokeClicked(View view) {
        controller.largeStrokeClicked();
    }

    void showHint() {
        if (isActivityInActiveState()) {
            controller.showHint();
        }
    }

    void dismissHint() {
        controller.dismissHint();
    }

    void foregroundBackgroundSeparationCompleted(Uri foregroundUri, Uri backgroundUri) {
        Intent intent = getIntent();
        Uri imageUri = null;
        String imageUriString = intent
                .getStringExtra(ImageSharingIntentExtras.EXTRA_TARGET_IMAGE_URI);
        if (imageUriString != null) {
            imageUri = Uri.parse(imageUriString);
        }
        controller.foregroundBackgroundSeparationCompleted(foregroundUri, backgroundUri, imageUri);
    }

}
