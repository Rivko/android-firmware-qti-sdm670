/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.panoramaui;

import java.util.Objects;

import android.animation.Animator;
import android.animation.AnimatorSet;
import android.animation.ObjectAnimator;
import android.animation.PropertyValuesHolder;
import android.animation.ValueAnimator;
import android.content.res.Resources;
import android.graphics.PointF;
import android.util.Log;
import android.view.TextureView;
import android.view.View;
import android.view.animation.LinearInterpolator;
import android.widget.ImageView;

import com.qualcomm.qti.panoramaui.R;
import com.qualcomm.qti.panoramaui.AfIndicator.IndicatorState;

/**
 * This class manages an autofocus indicator {@link ImageView}. It is intended to work closely in
 * conjunction with the {@link R.drawable#camera_preview_focus_indicator} drawable. The indicator
 * {@link ImageView} should be in a container with the same placement as the preview
 * {@link TextureView}.
 */
public class AeAfUIHandler {
    private static final String LOG_TAG = "AeAfUIHandler";

    private static final int AUTOFOCUS_RUNNING = 0;
    private static final int AUTOFOCUS_FAILED = 1;
    private static final int AUTOFOCUS_SUCCEEDED = 2;

    private final ImageView autoFocusIndicator;
    private final AfIndicator afIndicator;

    private AnimatorSet autoFocusAnimation;
    private int autoFocusState;
    private final ImageView recordButton;
    private boolean touchToFocus = false;

    /**
     * Construct a new {@link AeAfUIHandler}
     *
     * @param autoFocusIndicator
     *            the autofocus indicator {@link ImageView}. It should have the
     *            {@link R.drawable#camera_preview_focus_indicator} as its drawable.
     * @param recordButton
     *            record button reference
     */
    public AeAfUIHandler(Resources resources, ImageView autoFocusIndicator, ImageView recordButton) {
        Objects.requireNonNull(autoFocusIndicator, "autoFocusIndicator may not be null");

        this.recordButton = recordButton;
        this.autoFocusIndicator = autoFocusIndicator;

        int progressColor = resources.getColor(R.color.autofocus_progress);
        int doneColor = resources.getColor(R.color.autofocus_done);
        float outerCircleThick = resources.getDimension(R.dimen.autofocus_outer_circle_stroke);
        float arcThick = resources.getDimension(R.dimen.autofocus_inner_circle_stroke);
        float outerRadius = resources.getDimension(R.dimen.autofocus_outer_circle_radius);
        float innerRadius = resources.getDimension(R.dimen.autofocus_inner_circle_radius);
        afIndicator = new AfIndicator(progressColor, doneColor, outerCircleThick, arcThick,
                outerRadius, innerRadius);

        autoFocusIndicator.setVisibility(View.INVISIBLE);
        autoFocusIndicator.setImageDrawable(afIndicator);
    }

    /**
     * Set the position (in display coordinates) for the autofocus indicator
     *
     * @param pos
     *            a {@link PointF} representing the desired location of the center of the autofocus
     *            indicator. If null, the location will be set to (0,0).
     */
    public void autoFocusPosition(PointF pos) {
        if (pos != null) {
            afIndicator.setLocation(pos.x, pos.y);
        } else {
            afIndicator.setLocation(0, 0);
        }
    }

    /**
     * This method indicates the start of an autofocus operation. It will start the autofocus
     * indicator animation. If an existing autofocus indicator animation is already running, it will
     * be cancelled.
     */
    public void autoFocusStarted() {
        autoFocusCancelled();

        ObjectAnimator stage1Anim = ObjectAnimator.ofPropertyValuesHolder(autoFocusIndicator,
                PropertyValuesHolder.ofFloat("alpha", 0f, 1f),
                PropertyValuesHolder.ofFloat("scaleX", 1.5f, 0.98f),
                PropertyValuesHolder.ofFloat("scaleY", 1.5f, 0.98f)).setDuration(200);
        ObjectAnimator stage2Anim = ObjectAnimator.ofPropertyValuesHolder(autoFocusIndicator,
                PropertyValuesHolder.ofFloat("scaleX", 1f),
                PropertyValuesHolder.ofFloat("scaleY", 1f)).setDuration(100);
        ObjectAnimator stage3Anim = ObjectAnimator
                .ofInt(autoFocusIndicator, "imageLevel", 0, 10000).setDuration(600);
        stage3Anim.addUpdateListener(new ValueAnimator.AnimatorUpdateListener() {
            private boolean playingForward = true;
            private int bounceCount;

            @Override
            public void onAnimationUpdate(ValueAnimator animation) {
                float fraction = animation.getAnimatedFraction();

                afIndicator.setProgress(fraction);
                if (autoFocusState == AUTOFOCUS_RUNNING && playingForward && fraction > 0.95f
                        && bounceCount++ < 5) {
                    playingForward = false;
                    animation.reverse();
                } else if (!playingForward && fraction < 0.6f) {
                    playingForward = true;
                    animation.reverse();
                }
            }
        });
        stage3Anim.addListener(new Animator.AnimatorListener() {
            @Override
            public void onAnimationStart(Animator animation) {
                afIndicator.setIndicatorState(IndicatorState.PROGRESS);
            }

            @Override
            public void onAnimationEnd(Animator animation) {
                if (autoFocusState == AUTOFOCUS_RUNNING) {
                    autoFocusState = AUTOFOCUS_FAILED;
                    Log.d(LOG_TAG, "Auto focus failed");
                    resetTouchToFocus();
                } else {
                    afIndicator.setIndicatorState(IndicatorState.DONE);
                }
            }

            @Override
            public void onAnimationCancel(Animator animation) {
            }

            @Override
            public void onAnimationRepeat(Animator animation) {
            }
        });
        ObjectAnimator stage4Anim = ObjectAnimator.ofPropertyValuesHolder(autoFocusIndicator,
                PropertyValuesHolder.ofFloat("alpha", 0.57f),
                PropertyValuesHolder.ofFloat("scaleX", 0.97f),
                PropertyValuesHolder.ofFloat("scaleY", 0.97f)).setDuration(100);
        stage4Anim.setStartDelay(100);
        ObjectAnimator stage5Anim = ObjectAnimator.ofPropertyValuesHolder(autoFocusIndicator,
                PropertyValuesHolder.ofFloat("alpha", 0f),
                PropertyValuesHolder.ofFloat("scaleX", 0f),
                PropertyValuesHolder.ofFloat("scaleY", 0f)).setDuration(100);

        autoFocusAnimation = new AnimatorSet();
        autoFocusAnimation.playSequentially(stage1Anim, stage2Anim, stage3Anim, stage4Anim,
                stage5Anim);
        autoFocusAnimation.setInterpolator(new LinearInterpolator());
        autoFocusAnimation.addListener(new Animator.AnimatorListener() {
            @Override
            public void onAnimationStart(Animator animation) {
                autoFocusIndicator.setVisibility(View.VISIBLE);
            }

            @Override
            public void onAnimationEnd(Animator animation) {
                autoFocusIndicator.setVisibility(View.INVISIBLE);
                autoFocusAnimation = null;
            }

            @Override
            public void onAnimationCancel(Animator animation) {
            }

            @Override
            public void onAnimationRepeat(Animator animation) {
            }
        });
        autoFocusState = AUTOFOCUS_RUNNING;
        afIndicator.setIndicatorState(IndicatorState.START);
        autoFocusAnimation.start();

        touchToFocus = true;
        recordButton.setImageResource(R.drawable.start_btn_disabled);
    }

    /**
     * Cancel any existing autofocus indicator animation
     */
    public void autoFocusCancelled() {
        if (touchToFocus) {
            touchToFocus = false;
            recordButton.setImageResource(R.drawable.start_btn);
        }

        if (autoFocusAnimation != null) {
            autoFocusAnimation.cancel();
            autoFocusAnimation = null;
        }
    }
    private void resetTouchToFocus() {
        if (touchToFocus) {
            touchToFocus = false;
            recordButton.setImageResource(R.drawable.start_btn);
        }
    }

    /**
     * Indicate that the active autofocus operation is complete. If an autofocus indicator animation
     * is running, it will be flagged to finish as soon as possible.
     *
     * @param success
     *            true if the autofocus operation completed successfully (ie the focus is now
     *            sharp), else false
     */
    public void autoFocusComplete(boolean success) {
        resetTouchToFocus();

        if (autoFocusState == AUTOFOCUS_RUNNING) {
            autoFocusState = (success ? AUTOFOCUS_SUCCEEDED : AUTOFOCUS_FAILED);
        }
    }

    /**
     * check if touch to focus in progress
     *
     * @return true if touch to focus in progress, false otherwise
     */
    public boolean isTouchToFocusOn() {
        return touchToFocus;
    }
}
