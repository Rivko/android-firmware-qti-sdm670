/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.util;

import android.annotation.SuppressLint;
import android.view.GestureDetector;
import android.view.GestureDetector.OnGestureListener;
import android.view.GestureDetector.SimpleOnGestureListener;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;
import android.view.ScaleGestureDetector.OnScaleGestureListener;
import android.view.View;
import android.view.View.OnLayoutChangeListener;
import android.view.View.OnTouchListener;

/**
 * This class adds translation & scaling logic to any {@link View}. It attaches layout and touch
 * listeners, and allows smooth animations between translations and scale factors. To use it,
 * subclass to provide an implementation of {@link #onTranslateScaleChanged(float, float, float)},
 * create an instance for the desired {@link View}, and then inform the instance of the size of the
 * content via {@link #setContentSize(int, int)}.
 */
public abstract class TranslateScaleController {
    private final View view;
    private final float maxZoomFactor;
    private final GestureDetector translateGestureDetector;
    private final ScaleGestureDetector scaleGestureDetector;
    private int viewportWidth;
    private int viewportHeight;
    private int contentWidth;
    private int contentHeight;
    private float minScale;
    private float maxScale;
    private float centerX;
    private float centerY;
    private float scale;

    /**
     * Construct a {@link TranslateScaleController}
     * @param view {@link View} for which to provide translation & scaling
     * @param maxZoomFactor The maximum zoom factor to allow view to be zoomed to. Note that the
     *        actual maximum scale may be less than this, since this controller will not allow an
     *        overall scale of greater than 1.0.
     * @throws IllegalArgumentException if view is null, or if maxZoomFactor is < 1.0
     */
    public TranslateScaleController(View view, float maxZoomFactor) {
        if (view == null) {
            throw new IllegalArgumentException("view cannot be null");
        } else if (maxZoomFactor <= 1f) {
            throw new IllegalArgumentException("maxZoomFactor must be >= 1.0");
        }

        this.view = view;
        this.maxZoomFactor = maxZoomFactor;
        translateGestureDetector = new GestureDetector(view.getContext(), translateGestureListener);
        scaleGestureDetector = new ScaleGestureDetector(view.getContext(), scaleGestureListener);
        layoutChangeListener.onLayoutChange(view, view.getLeft(), view.getTop(), view.getRight(),
                view.getBottom(), 0, 0, 0, 0);
        setContentSize(0, 0);
        view.addOnLayoutChangeListener(layoutChangeListener);
        view.setOnTouchListener(touchListener);
    }

    /**
     * Sets the size of the content. This will set an initial scale & translation such that the
     * entire contents fit within the {@link View}.
     * @param width Width of the {@link View}s content, in px
     * @param height Height of the {@link View}s content, in px
     * @throws IllegalArgumentException if width or height are < 0
     */
    public void setContentSize(int width, int height) {
        if (width < 0 || height < 0) {
            throw new IllegalArgumentException("width and height must be non-negative");
        }

        contentWidth = width;
        contentHeight = height;
        resetTranslateScaleState();
    }

    /**
     * Get the content width
     * @return the content width
     */
    public int getContentWidth() {
        return contentWidth;
    }

    /**
     * Get the content height
     * @return the content height
     */
    public int getContentHeight() {
        return contentHeight;
    }

    /**
     * Get the current X translation of the content.
     * <p>
     * <i>NOTE: this translation value applies before scaling</i>
     * @return the current X translation
     */
    public float getTranslationX() {
        return centerX - viewportWidth / scale / 2f;
    }

    /**
     * Get the current Y translation of the content.
     * <p>
     * <i>NOTE: this translation value applies before scaling</i>
     * @return the current Y translation
     */
    public float getTranslationY() {
        return centerY - viewportHeight / scale / 2f;
    }

    /**
     * Get the current scale of the content
     * @return the current scale
     */
    public float getScale() {
        return scale;
    }

    /**
     * Get the minimum allowed scale of the content. This is calculated based on the size of the
     * {@link View} and the size of the content (set via {@link #setContentSize(int, int)}) such
     * that the content fills the entire {@link View}.
     * @return the minimum allowed scale
     */
    public float getMinScale() {
        return minScale;
    }

    /**
     * Get the maximum allowed scale of the content. This is calculated based on the size of the
     * content (set via {@link #setContentSize(int, int)}) and the maximum scale factor provided to
     * {@link #TranslateScaleController(View, float)}.
     * @return the maximum allowed scale
     */
    public float getMaxScale() {
        return maxScale;
    }

    private void resetTranslateScaleState() {

        if (contentWidth != 0 && contentHeight != 0 && viewportWidth != 0 && viewportHeight != 0) {
            minScale = Math.min((float)viewportWidth / (float)contentWidth, (float)viewportHeight /
                    (float)contentHeight);
        } else {
            minScale = 1f;
        }

        maxScale = minScale * maxZoomFactor;
        if (maxScale > 1f) {
            // maxScale = 1f;
        }

        applyTranslateScale(contentWidth / 2f, contentHeight / 2f, minScale);
    }

    private void applyTranslateScale(float x, float y, float s) {
        if (s < minScale) {
            s = minScale;
        } else if (s > maxScale) {
            s = maxScale;
        }
        scale = s;

        // N.B. Don't clamp the intermediate x/y values while animating - the final values will be
        // valid, and clamping in-between creates annoying discontinuities in the animation flow
        // (due to the interaction between scale and translation)

        x = clampOffsetToValidRange(x, viewportWidth, contentWidth, scale);
        y = clampOffsetToValidRange(y, viewportHeight, contentHeight, scale);

        centerX = x;
        centerY = y;

        onTranslateScaleChanged(getTranslationX(), getTranslationY(), scale);
    }

    private static float clampOffsetToValidRange(float offset, int viewportSize, int contentSize,
            float scale) {
        float minOffset = Math.min(contentSize, viewportSize / scale) / 2f;
        if (offset < minOffset) {
            return minOffset;
        } else if (offset > contentSize - minOffset) {
            return contentSize - minOffset;
        } else {
            return offset;
        }
    }

    protected abstract void onTranslateScaleChanged(float x, float y, float scale);

    private final OnLayoutChangeListener layoutChangeListener = new OnLayoutChangeListener() {
        @Override
        public void onLayoutChange(View v, int left, int top, int right, int bottom, int oldLeft,
                int oldTop, int oldRight, int oldBottom) {
            viewportWidth = right - left;
            viewportHeight = bottom - top;
            resetTranslateScaleState();
        }
    };

    private final OnTouchListener touchListener = new OnTouchListener() {
        @SuppressLint("ClickableViewAccessibility")
        @Override
        public boolean onTouch(View v, MotionEvent event) {
            translateGestureDetector.onTouchEvent(event);
            scaleGestureDetector.onTouchEvent(event);
            return true;
        }
    };

    private final OnGestureListener translateGestureListener = new SimpleOnGestureListener() {
        @Override
        public boolean onDown(MotionEvent e) {
            view.setPressed(true);
            return true;
        }

        @Override
        public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY) {
            final float scale = getScale();
            applyTranslateScale(centerX + distanceX / scale, centerY + distanceY / scale, scale);
            return true;
        }

        @Override
        public boolean onSingleTapUp(MotionEvent e) {
            view.setPressed(false);
            view.performClick();
            return true;
        }
    };

    private final OnScaleGestureListener scaleGestureListener = new OnScaleGestureListener() {
        @Override
        public boolean onScaleBegin(ScaleGestureDetector detector) {
            return true;
        }

        @Override
        public void onScaleEnd(ScaleGestureDetector detector) {
        }

        @Override
        public boolean onScale(ScaleGestureDetector detector) {
            float sf = detector.getScaleFactor();
            float focusX = detector.getFocusX() - viewportWidth / 2f;
            float focusY = detector.getFocusY() - viewportHeight / 2f;
            applyTranslateScale(centerX + (sf - 1f) * focusX / scale, centerY + (sf - 1f) * focusY /
                    scale, scale * sf);
            return true;
        }
    };
}
