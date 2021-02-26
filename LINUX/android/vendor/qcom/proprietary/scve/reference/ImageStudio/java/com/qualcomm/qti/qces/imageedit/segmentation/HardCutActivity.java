/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.imageedit.segmentation;

import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.animation.ValueAnimator;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.graphics.Bitmap;
import android.graphics.Matrix;
import android.graphics.drawable.Drawable;
import android.net.Uri;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnTouchListener;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.ImageButton;
import android.widget.ImageView;

import com.qualcomm.qti.qces.imageedit.softcut.R;
import com.qualcomm.qti.qces.util.ImageSharingIntentExtras;

public class HardCutActivity extends AbstractSegmentationActivity {

    public static final String prefName = "com.qualcomm.qti.qces.imageedit.segmentation";

    private HardCutModel model;
    private HardCutController controller;

    private Button foregroundButton;
    private Button backgroundButton;
    private ImageButton previewButton;
    private ImageButton enhanceEdgesButton;

    private ImageView tempImgView;
    private FrameLayout hardcutFrameLayout;

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        model = new HardCutModel(this);
        controller = new HardCutController(this, model);
        super.setModel(model);
        super.setController(controller);

        super.onCreate(savedInstanceState);


        setContentView(R.layout.hard_cut_layout);
        initializeViews();

        initializeMaskAlphaView();
        initializeListeners();

        SharedPreferences prefs = this.getSharedPreferences(prefName,
                Context.MODE_PRIVATE);
        controller.checkShowHelp(prefs);


    }

    public void setPreviewButtonEnable(boolean enabled) {
        model.previewEnabled = enabled;
        previewButton.setEnabled(enabled);
    }

    @Override
    public void onResume() {
        super.onResume();
        controller.onResume();
    }

    void initializeViews() {

        foregroundButton = (Button) findViewById(R.id.foreground_button);
        backgroundButton = (Button) findViewById(R.id.background_button);

        enhanceEdgesButton = (ImageButton) findViewById(R.id.enhance_button);
        previewButton = (ImageButton) findViewById(R.id.preview_button);
        setPreviewButtonEnable(model.previewEnabled);

        initializeSpinner(R.id.spinner, R.id.spinner_background);
        initializeImageView(R.id.preview_image);
        initializeMaskAlphaView(R.id.mask_alpha_view);
        initializeOutputMaskView(R.id.output_mask_view);
        initializeOutputMaskViewColors(new float[]{0, 0, 0, 0, 255f, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, -128f, 256f});
        hideOutputMaskView();

        initializeCancelButton(R.id.matting_cancel_button);
        initializeHelpButton(R.id.help_button);
        initializeUndoButton(R.id.undo_button);
        initializeRedoButton(R.id.redo_button);
        initializeDoneButton(R.id.done_button);

        controller.setupMaskView();
    }

    private void initializeListeners() {
        handleForegroundButtonClicked();
        handleBackgroundButtonClicked();
        handleEnhanceEdgesButtonClicked();
        handlePreviewButtonClicked();
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent intent) {
        super.onActivityResult(requestCode, resultCode, intent);
        if (intent == null) {
            return;
        }
        if (requestCode == 1) {
            Bundle softCutBundle = intent.getBundleExtra("softCutStateBundle");
            model.softCutStateBundle = softCutBundle;
        }

    }

    @Override
    protected void onStart() {
        super.onStart();

        Intent intent = getIntent();
        String imageUriString = intent
                .getStringExtra(ImageSharingIntentExtras.EXTRA_TARGET_IMAGE_URI);
        Uri imageUri = null;
        if (imageUriString != null) {
            imageUri = Uri.parse(imageUriString);
        }
        nextActivity = intent.getStringExtra(ImageSharingIntentExtras.NEXT_ACTIVITY);
        isFullMP = intent.getBooleanExtra(ImageSharingIntentExtras.FULL_MP_MODE, true);
        if (imageUri != null) {
            controller.imageSelected(imageUri);
        }

        controller.onStart();
    }

    @Override
    protected void onStop() {
        super.onStop();
        controller.onStop();
    }

    @Override
    public boolean dispatchTouchEvent(MotionEvent event) {
        super.dispatchTouchEvent(event);

        /* Do not dismiss any hints for MotionEvent.ACTION_DOWN when helpButton is pressed */
        if (helpButton.isPressed()) {
            return false;
        }
        if (event.getActionMasked() == MotionEvent.ACTION_DOWN) {
            controller.dismissHint();
        }
        return false;
    }

    private void handleEnhanceEdgesButtonClicked() {
        enhanceEdgesButton.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                controller.enhanceEdgesClicked();
            }
        });
    }

    private void handlePreviewButtonClicked() {
        previewButton.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View view) {
                controller.previewClicked();
            }
        });
    }

    private void handleBackgroundButtonClicked() {
        backgroundButton.setOnTouchListener(new OnTouchListener() {

            @Override
            public boolean onTouch(View v, MotionEvent event) {
                if (event.getAction() == MotionEvent.ACTION_UP) {
                    controller.backgroundClicked();
                    v.performClick();
                }
                return false;
            }
        });
    }

    private void handleForegroundButtonClicked() {
        foregroundButton.setOnTouchListener(new OnTouchListener() {

            @Override
            public boolean onTouch(View v, MotionEvent event) {
                if (event.getAction() == MotionEvent.ACTION_UP) {
                    controller.foregroundClicked();
                    v.performClick();
                }
                return false;
            }
        });
    }

    void enableEnhanceEdgesBtn() {
        enhanceEdgesButton.setEnabled(true);
    }

    void disableEnhanceEdgesBtn() {
        enhanceEdgesButton.setEnabled(false);
    }

    void setForegroundButtonActive() {
        foregroundButton.setSelected(true);
        backgroundButton.setSelected(false);
    }

    void setBackgroundButtonActive() {
        foregroundButton.setSelected(false);
        backgroundButton.setSelected(true);
    }

    void foregroundBackgroundSeparationCompleted(Uri foregroundUri, Uri backgroundUri) {
        if (!isActivityInActiveState()) {
            return;
        }
        Intent intent = getIntent();
        Uri imageUri = null;
        String imageUriString = intent
                .getStringExtra(ImageSharingIntentExtras.EXTRA_TARGET_IMAGE_URI);
        if (imageUriString != null) {
            imageUri = Uri.parse(imageUriString);
        }
        controller.foregroundBackgroundSeparationCompleted(foregroundUri, backgroundUri, imageUri);
    }

    public void doSegmentationPreview(Bitmap fgBitmap, Bitmap bgBitmap) {

        hideMaskView();

        final ImageView imageView = getImageView();

        Drawable drawable = getResources().getDrawable(R.drawable.checkerboard_tile_bitmap);

        imageView.setImageBitmap(fgBitmap);
        imageView.setBackground(drawable);

        hardcutFrameLayout = (FrameLayout) findViewById(R.id.hardcut_frame_layout);
        tempImgView = new ImageView(this);
        tempImgView.setScaleType(ImageView.ScaleType.MATRIX);
        hardcutFrameLayout.addView(tempImgView);
        tempImgView.setImageBitmap(bgBitmap);

        Matrix MM = getOutputMaskView().getImageMatrix();
        tempImgView.setImageMatrix(MM);

        getOutputMaskView().bringToFront();

        ValueAnimator animation = ValueAnimator.ofInt(0, 100, 100, 100, 0);
        animation.setDuration(4000);

        animation.addUpdateListener(new ValueAnimator.AnimatorUpdateListener() {
            @Override
            public void onAnimationUpdate(ValueAnimator animation) {
                int value = (Integer) animation.getAnimatedValue();

                //fade out the red outputmaskview
                float maskFade = value / (float) 100;
                setOutputMaskViewOpacity(1.0f - maskFade);
                tempImgView.setAlpha(1.0f - maskFade);

                //fade in the tiled background
                float fade2 = maskFade * 255f;
                getImageView().getBackground().setAlpha((int) fade2);


            }
        });
        animation.addListener(new AnimatorListenerAdapter() {
            @Override
            public void onAnimationEnd(Animator animation) {

                final ImageView imageView = getImageView();

                setPreviewButtonEnable(true);
                showMaskView();

                hardcutFrameLayout.removeView(tempImgView);

                imageView.setAlpha(1.0f);
                imageView.setImageBitmap(getImageViewBitmap());
                imageView.setBackground(null);
                imageView.invalidate();
            }
        });

        animation.start();

    }


}
