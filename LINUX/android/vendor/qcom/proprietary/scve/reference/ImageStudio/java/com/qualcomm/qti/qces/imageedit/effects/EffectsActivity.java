/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.imageedit.effects;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.net.Uri;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;

import com.qualcomm.qti.qces.imageedit.softcut.R;
import com.qualcomm.qti.qces.util.ImageSharingIntentExtras;

public class EffectsActivity extends Activity {

    public static final String prefName = "com.qualcomm.qti.qces.imageedit.effects";
    public static final String dontShowAutomatically = "DontShowHelpAutoEffects";

    private Uri backgroundUri;
    private Uri foregroundUri;
    private EffectsView view;
    private EffectsModel model;
    private EffectsController controller;
    public Boolean isFullMP;

    /*
     * isOnlyForeground flag will be true when Effects was called directly from stage activity where
     * there is only one image (i.e foreground image) and there is no background image
     */
    public boolean isOnlyForeground = false;
    public boolean separateClicked = false;

    @Override
    protected void onSaveInstanceState(Bundle savedInstanceState) {
        super.onSaveInstanceState(savedInstanceState);
        controller.onSaveInstanceState(savedInstanceState);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setContentView(R.layout.effects_layout);
        initialize();

        model.enable_help_popup = getIntent().getBooleanExtra(
                "enable_help_popup", false);

        super.onCreate(savedInstanceState);
        if (savedInstanceState != null) {
            controller.onRestoreInstanceState(savedInstanceState);
        }

        SharedPreferences prefs = this.getSharedPreferences(prefName, Context.MODE_PRIVATE);
        if (prefs.getBoolean(dontShowAutomatically, true) == false && model.helpActive == false &&
                model.enable_help_popup == true) {
            controller.helpClicked();
        }

    }

    @Override
    protected void onStart() {
        super.onStart();

        backgroundUri = null;
        foregroundUri = null;

        Intent intent = getIntent();
        String backgroundUriString = intent
                .getStringExtra(ImageSharingIntentExtras.BACKGROUND_IMAGE_URI);
        if (backgroundUriString != null) {
            backgroundUri = Uri.parse(backgroundUriString);
        }
        String foregroundUriString = intent
                .getStringExtra(ImageSharingIntentExtras.FOREGROUND_IMAGE_URI);
        if (foregroundUriString != null) {
            foregroundUri = Uri.parse(foregroundUriString);
        }
        isFullMP = intent.getBooleanExtra(ImageSharingIntentExtras.FULL_MP_MODE, true);
        if (backgroundUri == null) {
            isOnlyForeground = true;
            backgroundUri = foregroundUri;
        }

        if (backgroundUri != null && foregroundUri != null) {
            controller.initialize(backgroundUri, foregroundUri);
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        controller.onResume();
    }

    @Override
    public void onBackPressed() {
        cancelClicked(null);
    }

    private void initialize() {
        view = new EffectsView(this);
        model = new EffectsModel(this);
        controller = new EffectsController(this, view, model);

        // disable all effects buttons
        enableEffectsButtons(false);
    }

    public void enableEffectsButtons(boolean enable) {
        enableForegroundEffectsButtons(enable);
        enableBackgroundEffectsButtons(enable);
    }

    void enableForegroundEffectsButtons(boolean enable) {
        ViewGroup viewGroup = (ViewGroup)findViewById(R.id.effects_foreground_container);
        enableEffectsButtons(enable, viewGroup);
    }

    void enableBackgroundEffectsButtons(boolean enable) {
        ViewGroup viewGroup = (ViewGroup)findViewById(R.id.effects_background_container);
        enableEffectsButtons(enable, viewGroup);
    }

    private void enableEffectsButtons(boolean enable, ViewGroup viewGroup) {

        for (int i = 0; i < viewGroup.getChildCount(); i++) {
            View child = viewGroup.getChildAt(i);
            child.setEnabled(enable);
            if (!enable) {
                child.setAlpha(0.5f);// to gray out when disabling
            } else {
                child.setAlpha(1.0f);
            }
            if (child instanceof ViewGroup) {
                enableEffectsButtons(enable, (ViewGroup)child);
            }
        }
    }

    public void foregroundLightenClicked(View view) {
        controller.foregroundLightenClicked();
    }

    public void foregroundDarkenClicked(View view) {
        controller.foregroundDarkenClicked();
    }

    public void foregroundSepiaClicked(View view) {
        controller.foregroundSepiaClicked();
    }

    public void foregroundGrayscaleClicked(View view) {
        controller.foregroundGrayscaleClicked();
    }

    public void foregroundNoneClicked(View view) {
        controller.foregroundNoneClicked();
    }

    public void foregroundBlurClicked(View view) {
        controller.foregroundBlurClicked();
    }

    public void foregroundRecolorClicked(View view) {
        controller.foregroundRecolorClicked();
    }

    public void backgroundLightenClicked(View view) {
        controller.backgroundLightenClicked();
    }

    public void backgroundDarkenClicked(View view) {
        controller.backgroundDarkenClicked();
    }

    public void backgroundSepiaClicked(View view) {
        controller.backgroundSepiaClicked();
    }

    public void backgroundGrayscaleClicked(View view) {
        controller.backgroundGrayscaleClicked();
    }

    public void backgroundNoneClicked(View view) {
        controller.backgroundNoneClicked();
    }

    public void backgroundBlurClicked(View view) {
        controller.backgroundBlurClicked();
    }

    public void backgroundRecolorClicked(View view) {
        controller.backgroundRecolorClicked();
    }

    public void doneClicked(View view) {
        controller.doneClicked();
    }

    public void cancelClicked(View view) {
        controller.cancelClicked();
    }

    public void undoClicked(View view) {
        controller.undoClicked();
    }

    public void redoClicked(View view) {
        controller.redoClicked();
    }

    public void separateClicked(View view) {
        separateClicked = true;
        controller.separateClicked();
    }

    public void helpClicked(View view) {
        controller.helpClicked();
    }

    public void effectIntensityChanged(int intensity) {
        controller.effectIntensityChanged(intensity);
    }

    protected Uri getBackgroundImageUri() {
        return backgroundUri;
    }

    @Override
    public boolean dispatchTouchEvent(MotionEvent event) {
        super.dispatchTouchEvent(event);

        /* Do not dismiss any hints for MotionEvent.ACTION_DOWN when helpButton is pressed */
        if (controller.isHelpPressed()) {
            return false;
        }
        if (event.getActionMasked() == MotionEvent.ACTION_DOWN) {
            controller.dismissHint();
        }
        return false;
    }
}
