/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.threedr.ui3d;

import android.animation.Animator;
import android.animation.AnimatorSet;
import android.animation.ObjectAnimator;
import android.animation.PropertyValuesHolder;
import android.view.View;
import android.view.animation.LinearInterpolator;
import android.widget.ImageView;

import java.util.Objects;

/**
 * This class manages a Target Indicator {@link ImageView}. It is intended to work closely in
 * conjunction with the scan_target drawable.
 */
public class TargetIndicatorController {

    private final ImageView targetHitIndicator;

    private AnimatorSet targetHitAnimation;

    public TargetIndicatorController(ImageView targetIndicator) {
        Objects.requireNonNull(targetIndicator, "targetIndicator may not be null");

        this.targetHitIndicator = targetIndicator;
        targetHitIndicator.setVisibility(View.INVISIBLE);

    }

    /**
     * This method indicates the start of an autofocus operation. It will start the autofocus
     * indicator animation. If an existing autofocus indicator animation is already running, it will
     * be cancelled.
     */
    public void targetHitStarted() {

        ObjectAnimator stage1Anim = ObjectAnimator.ofPropertyValuesHolder(targetHitIndicator,
                PropertyValuesHolder.ofFloat("alpha", 0f, 1f),
                PropertyValuesHolder.ofFloat("scaleX", 0.5f, 1.2f),
                PropertyValuesHolder.ofFloat("scaleY", 0.5f, 1.2f))
                .setDuration(200);

        ObjectAnimator stage2Anim = ObjectAnimator.ofPropertyValuesHolder(targetHitIndicator,
                PropertyValuesHolder.ofFloat("scaleX", 1.0f),
                PropertyValuesHolder.ofFloat("scaleY", 1.0f))
                .setDuration(100);

        ObjectAnimator stage3Anim = ObjectAnimator.ofPropertyValuesHolder(targetHitIndicator,
                PropertyValuesHolder.ofFloat("alpha", 0f),
                PropertyValuesHolder.ofFloat("scaleX", 0.4f),
                PropertyValuesHolder.ofFloat("scaleY", 0.4f))
                .setDuration(200);

        stage3Anim.setStartDelay(400);

        targetHitAnimation = new AnimatorSet();
        targetHitAnimation.playSequentially(stage1Anim, stage2Anim, stage3Anim);
        targetHitAnimation.setInterpolator(new LinearInterpolator());

        targetHitAnimation.addListener(new Animator.AnimatorListener() {
            @Override
            public void onAnimationStart(Animator animation) {
                targetHitIndicator.setVisibility(View.VISIBLE);
            }

            @Override
            public void onAnimationEnd(Animator animation) {
                targetHitIndicator.setVisibility(View.INVISIBLE);
                targetHitAnimation = null;
            }

            @Override
            public void onAnimationCancel(Animator animation) {
            }

            @Override
            public void onAnimationRepeat(Animator animation) {
            }
        });

        targetHitAnimation.start();
    }


}
