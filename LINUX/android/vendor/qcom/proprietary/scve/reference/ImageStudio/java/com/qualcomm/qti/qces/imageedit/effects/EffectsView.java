/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.imageedit.effects;

import android.graphics.Bitmap;
import android.graphics.Matrix;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.SeekBar;
import android.widget.TextView;

import com.qualcomm.qti.qces.customui.AnimatedImageView;
import com.qualcomm.qti.qces.imageedit.softcut.R;
import com.qualcomm.qti.qces.util.TranslateScaleController;

/*package*/class EffectsView {

    private EffectsActivity activity;
    private AnimatedImageView spinner;
    private ImageView foregroundImage;
    private ImageView backgroundImage;
    private SeekBar effectIntensitySeekBar;
    private TextView effectIntensityTextView;
    private TextView effectNameTextView;
    private ImageButton undoButton;
    private ImageButton redoButton;
    private ImageButton doneButton;
    private ImageButton separateButton;
    private ImageButton helpButton;
    private ViewGroup foregroundEffectNone;
    private ViewGroup foregroundEffectGrayscale;
    private ViewGroup foregroundEffectBlur;
    private ViewGroup foregroundEffectDarken;
    private ViewGroup foregroundEffectLighten;
    private ViewGroup foregroundEffectSepia;
    private ViewGroup foregroundEffectRecolor;
    private ViewGroup backgroundEffectNone;
    private ViewGroup backgroundEffectGrayscale;
    private ViewGroup backgroundEffectBlur;
    private ViewGroup backgroundEffectDarken;
    private ViewGroup backgroundEffectLighten;
    private ViewGroup backgroundEffectSepia;
    private ViewGroup backgroundEffectRecolor;

    private static final float MAX_SCALE_FACTOR = 10f;
    private TranslateScaleController translateScaleController;

    public EffectsView(EffectsActivity effectsActivity) {
        this.activity = effectsActivity;

        initializeViews();
        initializeTranslationScaling();
    }

    void initializeViews() {
        foregroundImage = (ImageView)getViewForId(R.id.effects_foreground_image);
        backgroundImage = (ImageView)getViewForId(R.id.effects_background_image);
        spinner = (AnimatedImageView)getViewForId(R.id.effects_spinner);
        effectIntensitySeekBar = (SeekBar)getViewForId(R.id.effects_effect_intensity_seekbar);
        effectIntensityTextView = (TextView)getViewForId(R.id.effects_current_intensity_textview);
        effectNameTextView = (TextView)getViewForId(R.id.effects_current_effect_textview);
        undoButton = (ImageButton)getViewForId(R.id.effects_undo_button);
        redoButton = (ImageButton)getViewForId(R.id.effects_redo_button);
        doneButton = (ImageButton)getViewForId(R.id.done_button);
        separateButton = (ImageButton)getViewForId(R.id.effects_separate_button);
        helpButton = (ImageButton)getViewForId(R.id.effects_help_button);
        foregroundEffectNone = (ViewGroup)getViewForId(R.id.effects_foreground_none);
        foregroundEffectGrayscale = (ViewGroup)getViewForId(R.id.effects_foreground_grayscale);
        foregroundEffectBlur = (ViewGroup)getViewForId(R.id.effects_foreground_blur);
        foregroundEffectDarken = (ViewGroup)getViewForId(R.id.effects_foreground_darken);
        foregroundEffectLighten = (ViewGroup)getViewForId(R.id.effects_foreground_lighten);
        foregroundEffectSepia = (ViewGroup)getViewForId(R.id.effects_foreground_sepia);
        foregroundEffectRecolor = (ViewGroup)getViewForId(R.id.effects_foreground_recolor);
        backgroundEffectNone = (ViewGroup)getViewForId(R.id.effects_background_none);
        backgroundEffectGrayscale = (ViewGroup)getViewForId(R.id.effects_background_grayscale);
        backgroundEffectBlur = (ViewGroup)getViewForId(R.id.effects_background_blur);
        backgroundEffectDarken = (ViewGroup)getViewForId(R.id.effects_background_darken);
        backgroundEffectLighten = (ViewGroup)getViewForId(R.id.effects_background_lighten);
        backgroundEffectSepia = (ViewGroup)getViewForId(R.id.effects_background_sepia);
        backgroundEffectRecolor = (ViewGroup)getViewForId(R.id.effects_background_recolor);

        effectIntensitySeekBar.setOnSeekBarChangeListener(intensityChangeListener);
    }

    void initializeTranslationScaling() {
        translateScaleController = new TranslateScaleController(foregroundImage, MAX_SCALE_FACTOR) {
            @Override
            public void onTranslateScaleChanged(float x, float y, float scale) {
                Matrix m = new Matrix();
                m.postTranslate(-x, -y);
                m.postScale(scale, scale);
                foregroundImage.setImageMatrix(m);
                backgroundImage.setImageMatrix(m);
            }
        };
    }

    void updateContentSize() {
        int contentWidth = foregroundImage.getDrawable().getIntrinsicWidth();
        int contentHeight = foregroundImage.getDrawable().getIntrinsicHeight();
        translateScaleController.setContentSize(contentWidth, contentHeight);
    }

    private View getViewForId(int viewId) {
        return activity.findViewById(viewId);
    }

    void enableTouchMaskImageView() {
    }

    void showSpinner() {
        spinner.setVisibility(View.VISIBLE);
    }

    void hideSpinner() {
        spinner.setVisibility(View.INVISIBLE);
    }

    void setForegroundBitmap(Bitmap foregroundBitmap) {
        foregroundImage.setImageBitmap(foregroundBitmap);
    }

    void setBackgroundBitmap(Bitmap backgroundBitmap) {
        backgroundImage.setImageBitmap(backgroundBitmap);
    }

    void setEffectName(String effectLabel) {
        effectNameTextView.setText(effectLabel);
    }

    void showEffectIntensitySlider() {
        effectIntensitySeekBar.setVisibility(View.VISIBLE);
        //effectIntensityTextView.setVisibility(View.VISIBLE);//Commenting this line to hide the % text as requested by UI spec team
        effectNameTextView.setVisibility(View.VISIBLE);
    }

    void hideEffectIntensitySlider() {
        effectIntensitySeekBar.setVisibility(View.INVISIBLE);
        effectIntensityTextView.setVisibility(View.INVISIBLE);
        effectNameTextView.setVisibility(View.INVISIBLE);
    }

    void setEffectIntensity(int intensity) {
        effectIntensitySeekBar.setProgress(intensity);
    }

    void enableUndo() {
        undoButton.setEnabled(true);
    }

    void disableUndo() {
        undoButton.setEnabled(false);
    }

    void enableRedo() {
        redoButton.setEnabled(true);
    }

    void disableRedo() {
        redoButton.setEnabled(false);
    }

    void enableDone() {
        doneButton.setEnabled(true);
    }

    void disableDone() {
        doneButton.setEnabled(false);
    }

    boolean isSeparateButtonEnabled() {
        return separateButton.isEnabled();
    }

    boolean isHelpPressed() {
        return helpButton.isPressed();
    }

    void selectForegroundEffectNone() {
        unselectAllForegroundEffects();
        foregroundEffectNone.setSelected(true);
    }

    void selectForegroundEffectGrayscale() {
        unselectAllForegroundEffects();
        foregroundEffectGrayscale.setSelected(true);
    }

    void selectForegroundEffectBlur() {
        unselectAllForegroundEffects();
        foregroundEffectBlur.setSelected(true);
    }

    void selectForegroundEffectDarken() {
        unselectAllForegroundEffects();
        foregroundEffectDarken.setSelected(true);
    }

    void selectForegroundEffectLighten() {
        unselectAllForegroundEffects();
        foregroundEffectLighten.setSelected(true);
    }

    void selectForegroundEffectSepia() {
        unselectAllForegroundEffects();
        foregroundEffectSepia.setSelected(true);
    }

    void selectForegroundEffectRecolor() {
        unselectAllForegroundEffects();
        foregroundEffectRecolor.setSelected(true);
    }

    void selectBackgroundEffectNone() {
        unselectAllBackgroundEffects();
        backgroundEffectNone.setSelected(true);
    }

    void selectBackgroundEffectGrayscale() {
        unselectAllBackgroundEffects();
        backgroundEffectGrayscale.setSelected(true);
    }

    void selectBackgroundEffectBlur() {
        unselectAllBackgroundEffects();
        backgroundEffectBlur.setSelected(true);
    }

    void selectBackgroundEffectDarken() {
        unselectAllBackgroundEffects();
        backgroundEffectDarken.setSelected(true);
    }

    void selectBackgroundEffectLighten() {
        unselectAllBackgroundEffects();
        backgroundEffectLighten.setSelected(true);
    }

    void selectBackgroundEffectSepia() {
        unselectAllBackgroundEffects();
        backgroundEffectSepia.setSelected(true);
    }

    void selectBackgroundEffectRecolor() {
        unselectAllBackgroundEffects();
        backgroundEffectRecolor.setSelected(true);
    }

    private void unselectAllForegroundEffects() {
        foregroundEffectNone.setSelected(false);
        foregroundEffectGrayscale.setSelected(false);
        foregroundEffectBlur.setSelected(false);
        foregroundEffectDarken.setSelected(false);
        foregroundEffectLighten.setSelected(false);
        foregroundEffectSepia.setSelected(false);
        foregroundEffectRecolor.setSelected(false);
    }

    private void unselectAllBackgroundEffects() {
        backgroundEffectNone.setSelected(false);
        backgroundEffectGrayscale.setSelected(false);
        backgroundEffectBlur.setSelected(false);
        backgroundEffectDarken.setSelected(false);
        backgroundEffectLighten.setSelected(false);
        backgroundEffectSepia.setSelected(false);
        backgroundEffectRecolor.setSelected(false);
    }

    private final SeekBar.OnSeekBarChangeListener intensityChangeListener = new SeekBar.OnSeekBarChangeListener() {
        @Override
        public void onStartTrackingTouch(SeekBar seekBar) {
        }

        @Override
        public void onStopTrackingTouch(SeekBar seekBar) {
        }

        @Override
        public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
            if (fromUser) {
                activity.effectIntensityChanged(progress);
            }
            effectIntensityTextView.setText(progress + "%");
        }
    };
}
