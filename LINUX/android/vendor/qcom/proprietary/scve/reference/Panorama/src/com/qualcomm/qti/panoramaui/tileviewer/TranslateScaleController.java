/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.panoramaui.tileviewer;
import android.annotation.SuppressLint;
import android.graphics.Rect;
import android.graphics.RectF;
import android.graphics.drawable.Drawable;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;
import android.view.View;
import android.widget.ImageView;

import com.qualcomm.qti.panoramaui.tileviewer.TiledImageDrawable;

/**
 * This class handles translation & scaling of a {@link TiledImageDrawable} installed within an
 * {@link ImageView}.
 */
public class TranslateScaleController {
    /**
     * Classes that are interested in receiving translate/scale updates may implement {@link
     * OnTranslateScaleListener} to be notified when the translate/scale values are updated.
     */
    public static interface OnTranslateScaleListener {
        /**
         * This method will be invoked whenever the current displayed area changes
         * @param region the region of the image that is currently displayed to the user
         */
        void onTranslateScale(RectF region);
    }

    private final int rotation;
    private final TiledImageDrawable tiledDrawable;
    private final GestureDetector translateGestureDetector;
    private final ScaleGestureDetector scaleGestureDetector;
    private final int contentWidth;
    private final int contentHeight;
    private int viewportWidth;
    private int viewportHeight;
    private float minScale;
    private float centerX;
    private float centerY;
    private float scale;
    private OnTranslateScaleListener listener;
    private final RectF listenerUpdateRect = new RectF();

    /**
     * Construct a {@link TranslateScaleController}
     * @param view {@link ImageView} which contains the {@link TiledImageDrawable}
     * @param rotation Degrees clockwise that the {@link TiledImageDrawable} will be rotated when
     *      it is rendered. Must be one of 0/90/180/270.
     * @throws IllegalArgumentException if view is null, or if view's {@link Drawable} is not an
     *      instance of {@link TiledImageDrawable}
     */
    public TranslateScaleController(ImageView view, int rotation) {
        if (view == null) {
            throw new IllegalArgumentException("view cannot be null");
        } else if (!(view.getDrawable().getCurrent() instanceof TiledImageDrawable)) {
            throw new IllegalArgumentException("view's Drawable must be a TiledImageDrawable");
        } else if (rotation != 0 && rotation != 90 && rotation != 180 && rotation != 270) {
            throw new IllegalArgumentException("rotation must be 0/90/180/270");
        }

        this.rotation = rotation;
        tiledDrawable = (TiledImageDrawable)view.getDrawable().getCurrent();
        translateGestureDetector = new GestureDetector(view.getContext(), translateGestureListener);
        scaleGestureDetector = new ScaleGestureDetector(view.getContext(), scaleGestureListener);
        contentWidth = tiledDrawable.getIntrinsicWidth();
        contentHeight = tiledDrawable.getIntrinsicHeight();

        layoutChangeListener.onLayoutChange(view, 0, 0, 0, 0, 0, 0, 0, 0);
        view.addOnLayoutChangeListener(layoutChangeListener);
        view.setOnTouchListener(touchListener);
    }

    /**
     * Set a {@link OnTranslateScaleListener} to be notified on translate/scale changes
     * @param listener a {@link OnTranslateScaleListener} to notify when the translate/scale values
     *      change. If null, any previously set {@link OnTranslateScaleListener} will be cleared
     */
    public void setOnTranslateScaleListener(OnTranslateScaleListener listener) {
        this.listener = listener;
    }

    private void applyTranslateScale(float x, float y, float s) {
        if (s < minScale) {
            s = minScale;
        } else if (s > 1f) {
            s = 1f;
        }
        scale = s;

        centerX = clampOffsetToValidRange(x, viewportWidth, contentWidth, scale);
        centerY = clampOffsetToValidRange(y, viewportHeight, contentHeight, scale);

        tiledDrawable.setOrigin(centerX, centerY);
        tiledDrawable.setScale(scale);

        if (listener != null) {
            listenerUpdateRect.set(Math.max(centerX - viewportWidth / 2f / scale, 0f),
                    Math.max(centerY - viewportHeight / 2f / scale, 0f),
                    Math.min(centerX + viewportWidth / 2f / scale, contentWidth),
                    Math.min(centerY + viewportHeight / 2f / scale, contentHeight));
            listener.onTranslateScale(listenerUpdateRect);
        }
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

    private final View.OnLayoutChangeListener layoutChangeListener =
            new View.OnLayoutChangeListener() {
        @Override
        public void onLayoutChange(View v, int left, int top, int right, int bottom,
                int oldLeft, int oldTop, int oldRight, int oldBottom) {
            final Rect bounds = tiledDrawable.getBounds();
            viewportWidth = bounds.width();
            viewportHeight = bounds.height();
            minScale = tiledDrawable.getMinimumScale();
            applyTranslateScale(contentWidth / 2f, contentHeight / 2f, minScale);
        }
    };

    private final View.OnTouchListener touchListener =
            new View.OnTouchListener() {
        @SuppressLint("ClickableViewAccessibility")
        @Override
        public boolean onTouch(View v, MotionEvent event) {
            translateGestureDetector.onTouchEvent(event);
            scaleGestureDetector.onTouchEvent(event);
            return true;
        }
    };

    private final GestureDetector.OnGestureListener translateGestureListener =
            new GestureDetector.SimpleOnGestureListener() {
        @Override
        public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY) {
            // Rotate the scroll distance vector to match the Drawable rotation
            float temp;
            switch (rotation) {
            case 90:
                temp = distanceX;
                distanceX = distanceY;
                distanceY = -temp;
                break;

            case 180:
                distanceX = -distanceX;
                distanceY = -distanceY;
                break;

            case 270:
                temp = distanceX;
                distanceX = -distanceY;
                distanceY = temp;
                break;
            }

            applyTranslateScale(centerX + distanceX / scale, centerY + distanceY / scale, scale);
            return true;
        }
    };

    private final ScaleGestureDetector.OnScaleGestureListener scaleGestureListener =
            new ScaleGestureDetector.OnScaleGestureListener() {
        @Override
        public boolean onScaleBegin(ScaleGestureDetector detector) {
            return true;
        }

        @Override
        public void onScaleEnd(ScaleGestureDetector detector) {
        }

        @Override
        public boolean onScale(ScaleGestureDetector detector) {
            // Rotate the touch focus point to match the Drawable rotation
            float focusX = 0f;
            float focusY = 0f;
            switch (rotation) {
            case 0:
                focusX = detector.getFocusX();
                focusY = detector.getFocusY();
                break;

            case 90:
                focusX = detector.getFocusY();
                focusY = viewportHeight - detector.getFocusX();
                break;

            case 180:
                focusX = viewportWidth - detector.getFocusX();
                focusY = viewportHeight - detector.getFocusY();
                break;

            case 270:
                focusX = viewportWidth - detector.getFocusY();
                focusY = detector.getFocusX();
                break;
            }

            float sf = detector.getScaleFactor();
            focusX -= viewportWidth / 2f;
            focusY -= viewportHeight / 2f;
            applyTranslateScale(centerX + (sf - 1f) * focusX / scale,
                    centerY + (sf - 1f) * focusY / scale, scale * sf);
            return true;
        }
    };
}
