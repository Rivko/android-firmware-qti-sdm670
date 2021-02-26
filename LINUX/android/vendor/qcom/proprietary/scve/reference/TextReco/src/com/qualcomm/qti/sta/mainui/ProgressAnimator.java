/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.sta.mainui;

import android.content.res.Resources;
import android.graphics.drawable.AnimationDrawable;
import android.os.Handler;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;

import com.qualcomm.qti.sta.R;

/**
 * Class for showing progress animation
 */
class ProgressAnimator {
    private static final String LOG_TAG = "ProgressAnimator";

    // TODO: add them in dimens.xml
    private static final int PROGRESS_STEP_DELAY = 42;
    private static final int PROGRESS_NEXT_LOOP_DELAY = 126;

    private View progressView = null;

    /**
     * Interface for progress listener notifications
     */
    public interface ProgressListener {
        /**
         * Notification on progress animation completion
         */
        public void onAnimationComplete();
    }

    private enum AnimationType {
        NONE, FADE_IN, FADE_OUT, DOT_CIRCLE
    }

    private class FrameAnimation extends AnimationDrawable {
        private final AnimationType animationType;
        private int currentIndex = 0;

        /**
         * Constructor for the Frame animation class
         *
         * @param animationType
         *            animation type
         */
        public FrameAnimation(AnimationType animationType) {
            this.animationType = animationType;
        }

        private void reset() {
            currentIndex = 0;
        }

        /**
         * {@inheritDoc}
         */
        @Override
        public boolean selectDrawable(int idx) {
            if(idx < currentIndex) {
                Log.d(LOG_TAG, animationType + ", Wrong index: " + currentIndex + "," + idx);
                return false;
            }
            boolean status = super.selectDrawable(idx);
            currentIndex = idx;
            if (idx != 0 && idx == getNumberOfFrames() - 1) {
                scheduleCompleteNotification();
            }
            return status;
        }

        private void scheduleCompleteNotification() {
            int delay = PROGRESS_STEP_DELAY;
            if (animationType == AnimationType.FADE_OUT) {
                delay = PROGRESS_NEXT_LOOP_DELAY;
            }
            animationHadler.postDelayed(new NextAnimationRunnable(animationType), delay);
        }
    };

    private final ProgressListener progressListener;
    private final FrameAnimation fadeInAnimation = new FrameAnimation(AnimationType.FADE_IN);
    private final FrameAnimation fadeOutAnimation = new FrameAnimation(AnimationType.FADE_OUT);
    private final FrameAnimation circleAnimation = new FrameAnimation(AnimationType.DOT_CIRCLE);
    private final ImageView baseAnimationView;
    private final ImageView circleAnimationView;
    private boolean stopProgressAnimation = false;
    private boolean circularAnimationDone = false;
    private Handler animationHadler = new Handler();

    private class NextAnimationRunnable implements Runnable {

        public final AnimationType animationType;

        private NextAnimationRunnable(AnimationType animationType) {
            this.animationType = animationType;
        }

        /**
         * {@inheritDoc}
         */
        @Override
        public void run() {
            if (fadeOutAnimation.isRunning()) {
                //Log.d(LOG_TAG, "Fade out is still running, Stopping it forcefully");
                fadeOutAnimation.stop();
            }
            if (fadeInAnimation.isRunning()) {
                //Log.d(LOG_TAG, "Fade in is still running, Stopping it forcefully");
                fadeInAnimation.stop();
            }
            animationCompleted(animationType);
        }
    };

    /**
     * Constructor for the class
     *
     * @param resources
     *            application resources
     * @param baseAnimationView
     *            view for showing progress animation
     * @param circleAnimationView
     *            view for showing final result circular animation
     * @param listener
     *            progress listener
     */
    /* package */ProgressAnimator(Resources resources, View progressView, View baseAnimationView,
            View circleAnimationView, ProgressListener listener) {
        this.baseAnimationView = (ImageView) baseAnimationView;
        this.circleAnimationView = (ImageView) circleAnimationView;
        this.progressView = progressView;
        progressListener = listener;

        progressView.setVisibility(View.GONE);
        setFadeInAnimation(resources);
        setFadeOutAnimation(resources);
        setCircleAnimation(resources);
    }

    private void setFadeInAnimation(Resources resources) {
        fadeInAnimation.addFrame(resources.getDrawable(R.drawable.anim_fadein01),
                PROGRESS_STEP_DELAY);
        fadeInAnimation.addFrame(resources.getDrawable(R.drawable.anim_fadein02),
                PROGRESS_STEP_DELAY);
        fadeInAnimation.addFrame(resources.getDrawable(R.drawable.anim_fadein03),
                PROGRESS_STEP_DELAY);
        fadeInAnimation.addFrame(resources.getDrawable(R.drawable.anim_fadein04),
                PROGRESS_STEP_DELAY);
        fadeInAnimation.addFrame(resources.getDrawable(R.drawable.anim_fadein05),
                PROGRESS_STEP_DELAY);
        fadeInAnimation.addFrame(resources.getDrawable(R.drawable.anim_fadein06),
                PROGRESS_STEP_DELAY);
        fadeInAnimation.addFrame(resources.getDrawable(R.drawable.anim_fadein07),
                PROGRESS_STEP_DELAY);
        fadeInAnimation.addFrame(resources.getDrawable(R.drawable.anim_fadein08),
                PROGRESS_STEP_DELAY);
        fadeInAnimation.addFrame(resources.getDrawable(R.drawable.anim_fadein09),
                PROGRESS_STEP_DELAY);
        fadeInAnimation.addFrame(resources.getDrawable(R.drawable.anim_fadein10),
                PROGRESS_STEP_DELAY);
        fadeInAnimation.addFrame(resources.getDrawable(R.drawable.anim_fadein11),
                PROGRESS_STEP_DELAY);
        fadeInAnimation.addFrame(resources.getDrawable(R.drawable.anim_fadein12),
                PROGRESS_STEP_DELAY);
        fadeInAnimation.addFrame(resources.getDrawable(R.drawable.anim_fadein13),
                PROGRESS_STEP_DELAY);
        fadeInAnimation.addFrame(resources.getDrawable(R.drawable.anim_fadein14),
                PROGRESS_STEP_DELAY);
        fadeInAnimation.addFrame(resources.getDrawable(R.drawable.anim_fadein15),
                PROGRESS_STEP_DELAY);
        fadeInAnimation.addFrame(resources.getDrawable(R.drawable.anim_fadein16),
                PROGRESS_STEP_DELAY);
        fadeInAnimation.addFrame(resources.getDrawable(R.drawable.anim_fadein17),
                PROGRESS_STEP_DELAY);
        fadeInAnimation.addFrame(resources.getDrawable(R.drawable.anim_fadein18),
                PROGRESS_STEP_DELAY);
        fadeInAnimation.addFrame(resources.getDrawable(R.drawable.anim_fadein19),
                PROGRESS_STEP_DELAY);
        fadeInAnimation.addFrame(resources.getDrawable(R.drawable.anim_fadein20),
                PROGRESS_STEP_DELAY);
        fadeInAnimation.addFrame(resources.getDrawable(R.drawable.anim_fadein21),
                PROGRESS_STEP_DELAY);
        fadeInAnimation.addFrame(resources.getDrawable(R.drawable.anim_fadein22),
                PROGRESS_STEP_DELAY);
        fadeInAnimation.addFrame(resources.getDrawable(R.drawable.anim_fadein23),
                PROGRESS_STEP_DELAY);
        fadeInAnimation.addFrame(resources.getDrawable(R.drawable.anim_fadein24),
                PROGRESS_STEP_DELAY);
        fadeInAnimation.addFrame(resources.getDrawable(R.drawable.anim_fadein25),
                PROGRESS_STEP_DELAY);
    }

    private void setFadeOutAnimation(Resources resources) {
        fadeOutAnimation.addFrame(resources.getDrawable(R.drawable.anim_fadeout01),
                PROGRESS_STEP_DELAY);
        fadeOutAnimation.addFrame(resources.getDrawable(R.drawable.anim_fadeout02),
                PROGRESS_STEP_DELAY);
        fadeOutAnimation.addFrame(resources.getDrawable(R.drawable.anim_fadeout03),
                PROGRESS_STEP_DELAY);
        fadeOutAnimation.addFrame(resources.getDrawable(R.drawable.anim_fadeout04),
                PROGRESS_STEP_DELAY);
        fadeOutAnimation.addFrame(resources.getDrawable(R.drawable.anim_fadeout05),
                PROGRESS_STEP_DELAY);
        fadeOutAnimation.addFrame(resources.getDrawable(R.drawable.anim_fadeout06),
                PROGRESS_STEP_DELAY);
        fadeOutAnimation.addFrame(resources.getDrawable(R.drawable.anim_fadeout07),
                PROGRESS_STEP_DELAY);
        fadeOutAnimation.addFrame(resources.getDrawable(R.drawable.ani_empty),
                PROGRESS_NEXT_LOOP_DELAY);
    }

    private void setCircleAnimation(Resources resources) {
        circleAnimation.addFrame(resources.getDrawable(R.drawable.ani_done0001),
                PROGRESS_STEP_DELAY);
        circleAnimation.addFrame(resources.getDrawable(R.drawable.ani_done0002),
                PROGRESS_STEP_DELAY);
        circleAnimation.addFrame(resources.getDrawable(R.drawable.ani_done0003),
                PROGRESS_STEP_DELAY);
        circleAnimation.addFrame(resources.getDrawable(R.drawable.ani_done0004),
                PROGRESS_STEP_DELAY);
        circleAnimation.addFrame(resources.getDrawable(R.drawable.ani_done0005),
                PROGRESS_STEP_DELAY);
        circleAnimation.addFrame(resources.getDrawable(R.drawable.ani_done0006),
                PROGRESS_STEP_DELAY);
        circleAnimation.addFrame(resources.getDrawable(R.drawable.ani_done0007),
                PROGRESS_STEP_DELAY);
        circleAnimation.addFrame(resources.getDrawable(R.drawable.ani_done0008),
                PROGRESS_STEP_DELAY);
        circleAnimation.addFrame(resources.getDrawable(R.drawable.ani_done0009),
                PROGRESS_STEP_DELAY);
        circleAnimation.addFrame(resources.getDrawable(R.drawable.ani_done0010),
                PROGRESS_STEP_DELAY);
        circleAnimation.addFrame(resources.getDrawable(R.drawable.ani_done0011),
                PROGRESS_STEP_DELAY);
        circleAnimation.addFrame(resources.getDrawable(R.drawable.ani_done0012),
                PROGRESS_STEP_DELAY);
        circleAnimation.addFrame(resources.getDrawable(R.drawable.ani_done0013),
                PROGRESS_STEP_DELAY);
        circleAnimation.addFrame(resources.getDrawable(R.drawable.ani_done0014),
                PROGRESS_STEP_DELAY);
    }

    /**
     * Starts the progress animation
     */
    /* package */void start() {
        reset();
        progressView.setVisibility(View.VISIBLE);
        startAnimation(AnimationType.FADE_IN);
    }

    /**
     * Stops the progress animation
     */
    /* package */void startDoneAnimation() {
        Log.d(LOG_TAG, "Stop animation request received");
        stopProgressAnimation = true;
        circularAnimationDone = false;
    }

    /**
     * Stops the progress animation
     */
    /* package */void reset() {
        Log.d(LOG_TAG, "Reset animation");
        animationHadler.removeCallbacksAndMessages(null);

        stopProgressAnimation = false;
        circularAnimationDone = false;

        fadeInAnimation.stop();
        fadeOutAnimation.stop();
        circleAnimation.stop();

        baseAnimationView.setImageDrawable(null);
        circleAnimationView.setImageDrawable(null);

        progressView.setVisibility(View.GONE);
    }

    private void animationCompleted(AnimationType animationType) {
        //Log.d(LOG_TAG, "Animation finished " + animationType);
        if (animationType == AnimationType.FADE_IN) {
            startAnimation(AnimationType.FADE_OUT);
            if (stopProgressAnimation) {
                circularAnimationDone = true;
                startAnimation(AnimationType.DOT_CIRCLE);
            }
        } else if (animationType == AnimationType.FADE_OUT) {
            if (!stopProgressAnimation || (stopProgressAnimation && !circularAnimationDone)) {
                startAnimation(AnimationType.FADE_IN);
            } else {
                baseAnimationView.setImageDrawable(null);
            }
        } else if (animationType == AnimationType.DOT_CIRCLE) {
            stopProgressAnimation = false;
            if (progressListener != null) {
                progressListener.onAnimationComplete();
            }
            circleAnimationView.setImageDrawable(null);
        }
    }

    private void startAnimation(AnimationType animationType) {
        //Log.d(LOG_TAG, "Start animation: " + animationType);
        if (animationType == AnimationType.FADE_IN) {
            baseAnimationView.setImageDrawable(fadeInAnimation);
            fadeInAnimation.reset();
            fadeInAnimation.start();

        } else if (animationType == AnimationType.FADE_OUT) {
            baseAnimationView.setImageDrawable(fadeOutAnimation);
            fadeOutAnimation.reset();
            fadeOutAnimation.start();

        } else if (animationType == AnimationType.DOT_CIRCLE) {
            circleAnimationView.setImageDrawable(circleAnimation);
            circleAnimation.reset();
            circleAnimation.start();
        }
    }
}
