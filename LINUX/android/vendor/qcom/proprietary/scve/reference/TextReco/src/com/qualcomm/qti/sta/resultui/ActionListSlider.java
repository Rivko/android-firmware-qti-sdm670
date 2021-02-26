/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.

 * All Rights Reserved.
 * Qualcomm Technologies, Inc. Confidential and Proprietary.
 */
package com.qualcomm.qti.sta.resultui;

import android.animation.Animator;
import android.animation.Animator.AnimatorListener;
import android.animation.ValueAnimator;
import android.animation.ValueAnimator.AnimatorUpdateListener;
import android.content.Context;
import android.os.Handler;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;
import android.view.animation.DecelerateInterpolator;
import android.widget.FrameLayout;

import com.qualcomm.qti.sta.R;

/**
 * Class for handling action list sliding
 */
class ActionListSlider implements OnTouchListener {
    private static final float DURATION_STEP = 0.3f;

    private float moveStart = 0;
    private float startTopMargin = 0;
    private boolean dragging = false;

    private final View view;
    private final View header;

    private int maxY;
    private final float midY;

    /**
     * Constructor for the class
     *
     * @param context
     *            activity context
     * @param view
     *            list view
     * @param midY
     *            list mid y value
     */
    /* package */ActionListSlider(Context context, View view, float midY) {
        this.view = view;
        this.header = view.findViewById(R.id.list_header);
        this.midY = midY;
    }

    /**
     * updates the list view maximum Y position
     *
     * @param slidingHeight
     *            sliding area height
     */
    /* package */void updateMaxY(int slidingHeight) {
        this.maxY = slidingHeight - header.getHeight();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean onTouch(View v, MotionEvent event) {
        FrameLayout.LayoutParams params = (FrameLayout.LayoutParams) view.getLayoutParams();
        switch (event.getAction() & MotionEvent.ACTION_MASK) {
        case MotionEvent.ACTION_DOWN: {

            moveStart = event.getRawY() - params.topMargin;
            startTopMargin = params.topMargin;
            dragging = true;
            break;
        }
        case MotionEvent.ACTION_UP:
        case MotionEvent.ACTION_POINTER_UP:
            if (dragging) {
                dragging = false;
                int till = -1;
                if (params.topMargin < startTopMargin) {// moved up
                    till = (params.topMargin > midY) ? (int) midY : 0;

                } else if (params.topMargin > startTopMargin) {
                    till = (params.topMargin < midY) ? (int) midY : (int) maxY;
                }
                if (till != -1) {
                    header.setOnTouchListener(null);
                    animateListView(params.topMargin, till, new Runnable() {
                        /**
                         * {@inheritDoc}
                         */
                        @Override
                        public void run() {
                            header.setOnTouchListener(ActionListSlider.this);
                        }
                    });
                }
            }
            break;

        case MotionEvent.ACTION_MOVE:
            if (dragging) {
                params.topMargin = (int) (event.getRawY() - moveStart);
                if (params.topMargin < 0) {
                    params.topMargin = 0;
                } else if (params.topMargin > (int) maxY) {
                    params.topMargin = (int) maxY;
                }
                view.setLayoutParams(params);
            }
            break;
        }
        return true;
    }

    private void animateListView(int from, final int to, final Runnable onAnimationEnd) {
        // calculate duration
        int duration = (int) (Math.abs(from - to) * DURATION_STEP);
        //Log.d(LOG_TAG, "List animation duration: " + duration);
        ValueAnimator insertActionList = ValueAnimator.ofInt(from, to);
        insertActionList.setInterpolator(new DecelerateInterpolator());
        insertActionList.setDuration(duration);
        insertActionList.addUpdateListener(new AnimatorUpdateListener() {
            /**
             * {@inheritDoc}
             */
            @Override
            public void onAnimationUpdate(ValueAnimator animation) {
                int value = (Integer) animation.getAnimatedValue();
                FrameLayout.LayoutParams params = (FrameLayout.LayoutParams) view.getLayoutParams();
                params.topMargin = value;
                view.setLayoutParams(params);
            }
        });
        insertActionList.addListener(new AnimatorListener() {
            /**
             * {@inheritDoc}
             */
            @Override
            public void onAnimationStart(Animator animation) {
            }

            /**
             * {@inheritDoc}
             */
            @Override
            public void onAnimationRepeat(Animator animation) {
            }

            /**
             * {@inheritDoc}
             */
            @Override
            public void onAnimationEnd(Animator animation) {
                new Handler().post(onAnimationEnd);
            }

            /**
             * {@inheritDoc}
             */
            @Override
            public void onAnimationCancel(Animator animation) {
            }
        });
        insertActionList.start();
    }

    /**
     * enable/disable the visibility of the list view
     *
     * @param enable
     *            true if the list view needs to be visisble, flase otherwise
     */
    /* package */void enableVisibility(boolean enable) {
        view.setVisibility((enable) ? View.VISIBLE : View.GONE);
    }

    /**
     * enables/disables the touch events for the list view
     *
     * @param enable
     *            true if needs to be enable, false otherwise
     *
     */
    /* package */void enableTouchEvents(boolean enable) {
        header.setOnTouchListener((enable) ? this : null);
    }

    /**
     * Slides the list view to top
     *
     * @param runnable
     *            runnable which needs to be run after slide top
     */
    /* package */void slideTop(Runnable runnable) {
        header.setOnTouchListener(null);
        FrameLayout.LayoutParams params = (FrameLayout.LayoutParams) view.getLayoutParams();
        animateListView(params.topMargin, 0, runnable);
    }

    /**
     * Sets the list Y position
     *
     * @param y
     *            list y position
     */
    /* package */void setListY(int y) {
        FrameLayout.LayoutParams params = (FrameLayout.LayoutParams) view.getLayoutParams();
        params.topMargin = y;
        view.setLayoutParams(params);
    }

    /**
     * gets the mid Y position of the list view
     *
     * @return mid Y position
     */
    /* package */float getMidY() {
        return midY;
    }
}
