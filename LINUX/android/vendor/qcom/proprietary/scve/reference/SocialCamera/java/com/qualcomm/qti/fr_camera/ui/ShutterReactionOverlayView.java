/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.fr_camera.ui;

import android.animation.Animator;
import android.animation.AnimatorInflater;
import android.content.Context;
import android.util.AttributeSet;
import android.view.View;

import com.qualcomm.qti.fr_camera.R;

/**
 * This class animates an overlay view to provide a shutter reaction animation for still image
 * capture
 */
public class ShutterReactionOverlayView extends View {
    private Animator shutterReactionAnimation;

    /**
     * Construct a new {@link ShutterReactionOverlayView}
     */
    public ShutterReactionOverlayView(Context context) {
        this(context, null, 0, 0);
    }

    /**
     * Construct a new {@link ShutterReactionOverlayView}
     */
    public ShutterReactionOverlayView(Context context, AttributeSet attrs) {
        this(context, attrs, 0, 0);
    }

    /**
     * Construct a new {@link ShutterReactionOverlayView}
     */
    public ShutterReactionOverlayView(Context context, AttributeSet attrs, int defStyleAttr) {
        this(context, attrs, defStyleAttr, 0);
    }

    /**
     * Construct a new {@link ShutterReactionOverlayView}
     */
    public ShutterReactionOverlayView(Context context, AttributeSet attrs, int defStyleAttr,
                                      int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
    }

    /**
     * Start the shutter reaction animation
     */
    public void doShutterReaction() {
        if (shutterReactionAnimation == null) {
            shutterReactionAnimation = AnimatorInflater.loadAnimator(getContext(),
                    R.animator.camera_preview_shutter_reaction);
            shutterReactionAnimation.setTarget(this);
            shutterReactionAnimation.addListener(new Animator.AnimatorListener() {
                @Override
                public void onAnimationStart(Animator animation) {
                    setVisibility(View.VISIBLE);
                }

                @Override
                public void onAnimationEnd(Animator animation) {
                    setVisibility(View.INVISIBLE);
                }

                @Override
                public void onAnimationCancel(Animator animation) {
                    setVisibility(View.INVISIBLE);
                }

                @Override
                public void onAnimationRepeat(Animator animation) {
                }
            });
        }

        shutterReactionAnimation.start();
    }

    @Override
    protected void onAttachedToWindow() {
        super.onAttachedToWindow();
        setVisibility(View.INVISIBLE);
    }
}
