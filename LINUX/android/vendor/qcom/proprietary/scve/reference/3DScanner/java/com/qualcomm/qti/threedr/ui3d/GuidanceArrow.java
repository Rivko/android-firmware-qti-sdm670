/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.threedr.ui3d;

import android.animation.Animator;
import android.animation.AnimatorSet;
import android.animation.ObjectAnimator;
import android.app.Activity;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;

import com.qualcomm.qti.threedr.R;

/**
 * Created by mithun on 4/11/2016.
 */
public class GuidanceArrow {

    private static final String TAG = "Scan3D";

    public enum Direction {LEFT, UP, RIGHT, DOWN, UNKNOWN}

    int NUM_DIRN = 4;
    int REPEAT_COUNT = 2;

    int numArrowsPerDirn = 0;
    ImageView arrowImageView[];
    AnimatorSet mainAnimatorSet;
    Direction currentDirn;
    LinearLayout arrowLayout[];
    RelativeLayout guidanceMainLayout;
    ImageView donutImage;
    boolean isDisableGuidance = false;//False is default value, if set to True will disable the guidance arrows

    public GuidanceArrow(Activity activity) {
        init(activity);
    }

    private void init(Activity activity) {
        initViews(activity);
        if (arrowImageView != null) {
            numArrowsPerDirn = arrowImageView.length / NUM_DIRN;
        }
    }

    private void initViews(Activity activity) {
        guidanceMainLayout = (RelativeLayout) activity.findViewById(R.id.guidance_main_layout);

        if (!isDisableGuidance) {
            donutImage = (ImageView) activity.findViewById(R.id.donut);
            donutImage.setVisibility(View.VISIBLE);

            arrowLayout = new LinearLayout[4];
            arrowLayout[0] = (LinearLayout) activity.findViewById(R.id.left_arrow_layout);
            arrowLayout[1] = (LinearLayout) activity.findViewById(R.id.up_arrow_layout);
            arrowLayout[2] = (LinearLayout) activity.findViewById(R.id.right_arrow_layout);
            arrowLayout[3] = (LinearLayout) activity.findViewById(R.id.down_arrow_layout);

            arrowImageView = new ImageView[16];
            //left , assigning in reverse
            arrowImageView[3] = (ImageView) activity.findViewById(R.id.image_view0);
            arrowImageView[2] = (ImageView) activity.findViewById(R.id.image_view1);
            arrowImageView[1] = (ImageView) activity.findViewById(R.id.image_view2);
            arrowImageView[0] = (ImageView) activity.findViewById(R.id.image_view3);

            //up, assigning in reverse
            arrowImageView[7] = (ImageView) activity.findViewById(R.id.image_view4);
            arrowImageView[6] = (ImageView) activity.findViewById(R.id.image_view5);
            arrowImageView[5] = (ImageView) activity.findViewById(R.id.image_view6);
            arrowImageView[4] = (ImageView) activity.findViewById(R.id.image_view7);

            //right
            arrowImageView[8] = (ImageView) activity.findViewById(R.id.image_view8);
            arrowImageView[9] = (ImageView) activity.findViewById(R.id.image_view9);
            arrowImageView[10] = (ImageView) activity.findViewById(R.id.image_view10);
            arrowImageView[11] = (ImageView) activity.findViewById(R.id.image_view11);

            //down
            arrowImageView[12] = (ImageView) activity.findViewById(R.id.image_view12);
            arrowImageView[13] = (ImageView) activity.findViewById(R.id.image_view13);
            arrowImageView[14] = (ImageView) activity.findViewById(R.id.image_view14);
            arrowImageView[15] = (ImageView) activity.findViewById(R.id.image_view15);

            //hide all the arrow layouts initially
            for (int i = 0; i < NUM_DIRN; i++) {
                arrowLayout[i].setVisibility(View.GONE);
            }
        }
    }

    public void show(Direction dirn) {
        if (isDisableGuidance || numArrowsPerDirn == 0) {
            Log.i(TAG, "GuidanceArrow class not inited or Guidance is disabled");
            return;
        }

        //Stop any previously running animation and disable its layout
        stop();

        if (dirn == Direction.UNKNOWN) {
            return;//no direction to be shown
        }


        currentDirn = dirn;

        arrowLayout[dirn.ordinal()].setVisibility(View.VISIBLE);
        long duration = 200;
        int ivOffset = dirn.ordinal() * numArrowsPerDirn;

        ObjectAnimator animatorInit[] = new ObjectAnimator[numArrowsPerDirn];
        for (int idx = 0; idx < numArrowsPerDirn; idx++) {
            animatorInit[idx] = ObjectAnimator.ofFloat(arrowImageView[ivOffset + idx], "alpha", 1.0f, 0.0f);
            animatorInit[idx].setDuration(0);
        }

        ObjectAnimator animatorOn[] = new ObjectAnimator[numArrowsPerDirn];
        for (int idx = 0; idx < numArrowsPerDirn; idx++) {
            animatorOn[idx] = ObjectAnimator.ofFloat(arrowImageView[ivOffset + idx], "alpha", 0.0f, 1.0f);
            animatorOn[idx].setDuration(duration);
        }

        ObjectAnimator animatorHold[] = new ObjectAnimator[numArrowsPerDirn];
        for (int idx = 0; idx < numArrowsPerDirn; idx++) {
            animatorHold[idx] = ObjectAnimator.ofFloat(arrowImageView[ivOffset + idx], "alpha", 1.0f, 1.0f);
            animatorHold[idx].setDuration(duration);
        }

        ObjectAnimator animatorOff[] = new ObjectAnimator[numArrowsPerDirn];
        for (int idx = 0; idx < numArrowsPerDirn; idx++) {
            animatorOff[idx] = ObjectAnimator.ofFloat(arrowImageView[ivOffset + idx], "alpha", 1.0f, 0.0f);
            animatorOff[idx].setDuration(duration);
        }

        //To set the arrows to transparent i.e alpha 0 initially before starting the blink animation
        AnimatorSet initSet = new AnimatorSet();
        initSet.playTogether(animatorInit[0], animatorInit[1], animatorInit[2], animatorInit[3]);
        initSet.start();

        AnimatorSet set[] = new AnimatorSet[4];
        for (int i = 0; i < numArrowsPerDirn; i++) {
            set[i] = new AnimatorSet();
            set[i].setStartDelay(i * duration * 1 / 2);
            set[i].playSequentially(animatorOn[i], animatorHold[i], animatorOff[i]);
        }

        mainAnimatorSet = new AnimatorSet();
        mainAnimatorSet.playTogether(set[0], set[1], set[2], set[3]);

        mainAnimatorSet.addListener(new Animator.AnimatorListener() {
            private boolean mCanceled;
            private int count = REPEAT_COUNT;

            @Override
            public void onAnimationStart(Animator animation) {
                // Log.i("TEST", "onAnimationStart");
                mCanceled = false;
                count--;
            }

            @Override
            public void onAnimationEnd(Animator animation) {
                // Log.i("TEST", "onAnimationEnd");
                if (!mCanceled && count > 0) {
                    mainAnimatorSet.start();
                } else {
                    stop();
                }
            }

            @Override
            public void onAnimationCancel(Animator animation) {
                // Log.i("TEST", "onAnimationCancel");
                mCanceled = true;
            }

            @Override
            public void onAnimationRepeat(Animator animation) {
                // Log.i("TEST", "onAnimationRepeat");
            }
        });

        mainAnimatorSet.start();
    }

    public void stop() {
        if (mainAnimatorSet == null) {
            return;//stop called even before starting for 1st time, nothing is started to stop.
        }
        if (mainAnimatorSet.isRunning()) {
            mainAnimatorSet.cancel();
        }
        if (arrowLayout != null) {
            arrowLayout[currentDirn.ordinal()].setVisibility(View.GONE);
        }
    }

    public void destroy() {
        guidanceMainLayout.setVisibility(View.GONE);
    }
}
