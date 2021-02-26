/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.fr_camera.ui;

import android.animation.Animator;
import android.animation.AnimatorSet;
import android.animation.ObjectAnimator;
import android.animation.PropertyValuesHolder;
import android.animation.ValueAnimator;
import android.graphics.PointF;
import android.view.TextureView;
import android.view.View;
import android.view.animation.LinearInterpolator;
import android.widget.ImageView;

import com.qualcomm.qti.fr_camera.R;

import java.util.Objects;

/**
 * This class manages an autofocus indicator {@link ImageView}. It is intended to work closely in
 * conjunction with the {@link R.drawable#camera_preview_focus_indicator} drawable. The indicator
 * {@link ImageView} should be in a container with the same placement as the preview
 * {@link TextureView}.
 */
public class AutoFocusIndicatorController {
    private static final int AUTOFOCUS_RUNNING = 0;
    private static final int AUTOFOCUS_FAILED = 1;
    private static final int AUTOFOCUS_SUCCEEDED = 2;
    private static final int[][] AUTOFOCUS_INDICATOR_STATE = new int[][] {
            null,
            new int[] { R.attr.state_focus_complete },
            new int[] { R.attr.state_focus_complete, R.attr.state_focus_succeeded }
    };

    private final ImageView autoFocusIndicator;

    private AnimatorSet autoFocusAnimation;
    private int autoFocusState;
    private PointF autoFocusPos = new PointF();

    /**
     * Construct a new {@link AutoFocusIndicatorController}
     * @param autoFocusIndicator the autofocus indicator {@link ImageView}. It should have the
     *                           {@link R.drawable#camera_preview_focus_indicator} as its drawable.
     * @throws NullPointerException if autoFocusIndicator is null
     */
    public AutoFocusIndicatorController(ImageView autoFocusIndicator) {
        Objects.requireNonNull(autoFocusIndicator, "autoFocusIndicator may not be null");

        this.autoFocusIndicator = autoFocusIndicator;
        autoFocusIndicator.setVisibility(View.INVISIBLE);
        autoFocusIndicator.addOnLayoutChangeListener(autoFocusIndicatorLayoutChangeListener);
        autoFocusPosition(null);

        // N.B. This is a bug in LayerDrawable when cloning an existing drawable (such as when the
        // device is rotated). The LayerDrawable is reset to level 0, but the children maintain
        // their existing level (from the cloned drawable). Set the LayerDrawable's level to a non-
        // zero value, such that the setLevel(0) call during autoFocusStarted is guaranteed not to
        // be ignored (since the LayerDrawable thinks it is already 0).
        autoFocusIndicator.setImageLevel(10000);
    }

    /**
     * Set the position (in display coordinates) for the autofocus indicator
     * @param pos a {@link PointF} representing the desired location of the center of the autofocus
     *            indicator. If null, the location will be set to (0,0).
     */
    public void autoFocusPosition(PointF pos) {
        if (pos != null) {
            autoFocusPos.set(pos);
        } else {
            autoFocusPos.set(0, 0);
        }
        updateAutoFocusIndicatorPosition();
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
                PropertyValuesHolder.ofFloat("scaleY", 1.5f, 0.98f))
                .setDuration(200);
        ObjectAnimator stage2Anim = ObjectAnimator.ofPropertyValuesHolder(autoFocusIndicator,
                PropertyValuesHolder.ofFloat("scaleX", 1f),
                PropertyValuesHolder.ofFloat("scaleY", 1f))
                .setDuration(100);
        ObjectAnimator stage3Anim = ObjectAnimator.ofInt(autoFocusIndicator, "imageLevel", 0, 10000)
                .setDuration(600);
        stage3Anim.addUpdateListener(new ValueAnimator.AnimatorUpdateListener() {
            private boolean playingForward = true;
            private int bounceCount;

            @Override
            public void onAnimationUpdate(ValueAnimator animation) {
                float fraction = animation.getAnimatedFraction();

                if (autoFocusState == AUTOFOCUS_RUNNING && playingForward && fraction > 0.95f &&
                        bounceCount++ < 5) {
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
            }

            @Override
            public void onAnimationEnd(Animator animation) {
                if (autoFocusState == AUTOFOCUS_RUNNING) {
                    autoFocusState = AUTOFOCUS_FAILED;
                }

                autoFocusIndicator.setImageState(AUTOFOCUS_INDICATOR_STATE[autoFocusState], true);
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
                PropertyValuesHolder.ofFloat("scaleY", 0.97f))
                .setDuration(100);
        stage4Anim.setStartDelay(100);
        ObjectAnimator stage5Anim = ObjectAnimator.ofPropertyValuesHolder(autoFocusIndicator,
                PropertyValuesHolder.ofFloat("alpha", 0f),
                PropertyValuesHolder.ofFloat("scaleX", 0f),
                PropertyValuesHolder.ofFloat("scaleY", 0f))
                .setDuration(100);

        autoFocusAnimation = new AnimatorSet();
        autoFocusAnimation.playSequentially(stage1Anim, stage2Anim, stage3Anim, stage4Anim, stage5Anim);
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
        autoFocusIndicator.setImageLevel(0);
        autoFocusIndicator.setImageState(AUTOFOCUS_INDICATOR_STATE[autoFocusState], true);
        autoFocusAnimation.start();
    }

    /**
     * Cancel any existing autofocus indicator animation
     */
    public void autoFocusCancelled() {
        if (autoFocusAnimation != null) {
            autoFocusAnimation.cancel();
            autoFocusAnimation = null;
        }
    }

    /**
     * Indicate that the active autofocus operation is complete. If an autofocus indicator animation
     * is running, it will be flagged to finish as soon as possible.
     * @param success true if the autofocus operation completed successfully (ie the focus is now
     *                sharp), else false
     */
    public void autoFocusComplete(boolean success) {
        if (autoFocusState == AUTOFOCUS_RUNNING) {
            autoFocusState = (success ? AUTOFOCUS_SUCCEEDED : AUTOFOCUS_FAILED);
        }
    }

    private void updateAutoFocusIndicatorPosition() {
        autoFocusIndicator.setTranslationX(autoFocusPos.x - autoFocusIndicator.getWidth() / 2f);
        autoFocusIndicator.setTranslationY(autoFocusPos.y - autoFocusIndicator.getHeight() / 2f);
    }

    @SuppressWarnings("FieldCanBeLocal")
    private final View.OnLayoutChangeListener autoFocusIndicatorLayoutChangeListener =
            new View.OnLayoutChangeListener() {
        @Override
        public void onLayoutChange(View v, int left, int top, int right, int bottom,
                                   int oldLeft, int oldTop, int oldRight, int oldBottom) {
            updateAutoFocusIndicatorPosition();
        }
    };
}
