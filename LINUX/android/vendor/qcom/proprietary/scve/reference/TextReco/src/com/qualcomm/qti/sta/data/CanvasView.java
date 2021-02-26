/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies, Inc. Confidential and Proprietary.
 */
package com.qualcomm.qti.sta.data;

import android.animation.Animator;
import android.animation.Animator.AnimatorListener;
import android.animation.PropertyValuesHolder;
import android.animation.ValueAnimator;
import android.animation.ValueAnimator.AnimatorUpdateListener;
import android.graphics.PointF;
import android.util.Log;
import android.view.animation.DecelerateInterpolator;

/**
 * class for handling canvas
 */
public class CanvasView {
    private static final String LOG_TAG = "CanvasView";

    /**
     * canvas view type for showing the image at center ro at top
     */
    public enum ViewType {
        TOP, CENTER;
    }

    /**
     * Interface for passing notification to the listeners
     */
    public interface CanvasEventListener {

        /**
         * Notification on scaling done
         */
        public void onScaleDone();

        /**
         * Notification on scaling/repositioning animation up-date
         */
        public void onScaleAnimationUpdate();
    }

    private final CanvasEventListener eventListener;

    private static final long MIN_DELAY = 60;
    private static final long MAX_DELAY = 1000;
    private static final float TRANSLATE_MOVE_DELAY = 0.2f;

    private static final float MAX_SCALE = 3.0f;

    private PointF validTranslation = new PointF();
    private float displayWidth;
    private float displayHeight;
    private final float imageWidth;
    private final float imageHeight;

    private float scaleLocked = 1.0f;

    private float minScale = 1.0f;
    private float maxScale = 2.0f;
    private float scaleFactor = 1.0f;
    private float translateX = 0;
    private float translateY = 0;

    private final ViewType viewType;

    /**
     * Constructor for the canvas class
     *
     * @param imageWidth
     *            image current width
     * @param imageHeight
     *            image current height
     * @param listener
     *            scale animation listener
     * @param viewType
     *            image view type (center or top)
     */
    public CanvasView(float imageWidth, float imageHeight, CanvasEventListener listener,
            ViewType viewType) {
        this.imageWidth = imageWidth;
        this.imageHeight = imageHeight;

        eventListener = listener;
        this.viewType = viewType;
    }

    /**
     * sets the CanvasView display parameters
     *
     * @param displayWidth
     *            Display-able width for the image
     * @param displayHeight
     *            Display-able height for the image
     */
    public void setDisplaySize(int displayWidth, int displayHeight) {
        this.displayWidth = displayWidth;
        this.displayHeight = displayHeight;
        float horizontalScale = (displayWidth / imageWidth);
        float verticalScale = (displayHeight / imageHeight);

        scaleFactor = Math.min(horizontalScale, verticalScale);
        scaleLocked = scaleFactor;
        minScale = scaleFactor;
        maxScale = MAX_SCALE;

        float currentImageWidth = toScreenUnits(imageWidth);
        float diffX = toImageUnits(displayWidth - currentImageWidth);
        if (displayWidth >= currentImageWidth) {
            translateX = diffX / 2;
        }

        if (viewType == ViewType.CENTER) {
            float currentImageHeight = toScreenUnits(imageHeight);
            float diffY = toImageUnits(displayHeight - currentImageHeight);
            if (displayHeight >= currentImageHeight) {
                translateY = diffY / 2;
            }
        }
    }

    /**
     * returns the current scale value for the canvas
     *
     * @return scale value
     */
    public float getScale() {
        return scaleFactor;
    }

    /**
     * Locks the canvas
     */
    public void lockView() {

    }

    /**
     * Unlock the locked canvasView
     */
    public boolean unlockView() {
        float prevScale = scaleLocked;
        scaleLocked = scaleFactor;

        PointF validTranslations = (viewType == ViewType.CENTER) ? getValidXYForCenterType()
                : getValidXYOnZoomOut(prevScale, scaleLocked);
        boolean isAnimating = animateToReposition(validTranslations.x, validTranslations.y);
        if (!isAnimating && eventListener != null) {
            eventListener.onScaleDone();
        }
        return true;
    }

    /**
     * Translates the image by x, y value
     *
     * @param x
     *            value by which x needs to be translated
     * @param y
     *            value by which y needs to be translated
     */
    public boolean translateBy(float x, float y) {
        float prevX = translateX;
        float prevY = translateY;
        translateX += (x / scaleFactor);
        translateY += (y / scaleFactor);

        PointF validTranslations = (viewType == ViewType.CENTER) ? getValidXYForCenterType()
                : getValidXYOnPan(prevX, prevY, translateX, translateY);
        translateX = validTranslations.x;
        translateY = validTranslations.y;

        if (prevX == translateX && prevY == translateY) {
            return false;
        }
        return true;
    }

    /**
     * Scales a view with the scale value provided as argument
     *
     * @param scale
     *            scale value
     */
    private void scaleBy(float scale) {
        scaleFactor = scaleLocked * scale;
        if (scaleFactor < minScale) {
            scaleFactor = minScale;
        }
        if (scaleFactor > maxScale) {
            scaleFactor = maxScale;
        }
    }

    /**
     * Scales a view with the scale value provided as argument
     *
     * @param scale
     *            scale value
     * @param anchorX
     *            anchor point x from where image needs to be scaled
     * @param anchorY
     *            anchor point y from where image needs to be scaled
     */
    public boolean scaleBy(float scale, float anchorX, float anchorY) {
        float prevScale = scaleFactor;
        scaleBy(scale);
        if (scaleFactor == prevScale) {
            return false;
        }

        float pinchZoom = scaleFactor / prevScale;
        if (prevScale != scaleFactor) {
            float tx = (1 - pinchZoom) * anchorX / scaleFactor;
            float ty = (1 - pinchZoom) * anchorY / scaleFactor;
            translateX += tx;
            translateY += ty;
        }
        return true;
    }

    private float toScreenUnits(float storeDistance) {
        return storeDistance * scaleFactor;
    }

    private float toImageUnits(float screenDistance) {
        return screenDistance / scaleFactor;
    }

    /**
     * gets the display-able width in term of image units
     *
     * @return display-able width in term of image units
     */
    public float getImageDisplayableWidth() {
        return toImageUnits(displayWidth);
    }

    /**
     * gets the display-able height in term of image units
     *
     * @return display-able height in term of image units
     */
    public float getImageDisplayableHeight() {
        return toImageUnits(displayHeight);
    }

    /**
     * Converts the screen X coordinate to image x coordinate
     *
     * @param screenX
     *            screen x coordinate
     * @return image x coordinate
     */
    public float toImageX(float screenX) {
        return screenX / scaleFactor - translateX;
    }

    /**
     * Converts the screen y coordinate to image x coordinate
     *
     * @param screenY
     *            screen y coordinate
     * @return image y coordinate
     */
    public float toImageY(float screenY) {
        return screenY / scaleFactor - translateY;
    }

    /**
     * returns the current translate x value for the canvas
     *
     * @return current translate x value
     */
    public float getTranslateX() {
        return translateX;
    }

    /**
     * returns the current translate y value for the canvas
     *
     * @return current translate y value
     */
    public float getTranslateY() {
        return translateY;
    }

    /**
     * Validate translation values after applying translation. this method
     * checks for the boundary condition on movement.
     */
    private PointF getValidXYOnZoomOut(float fromScale, float toScale) {
        if (toScale > fromScale) {
            validTranslation.set(translateX, translateY);

        } else {
            float toX = translateX;
            float toY = translateY;

            float currentImageWidth = toScreenUnits(imageWidth);
            // validate x value
            if (currentImageWidth <= displayWidth) {
                toX = toImageUnits(displayWidth - currentImageWidth) / 2;
            }
            validTranslation.set(toX, toY);
        }
        return validTranslation;
    }

    /**
     * Validate translation values after applying translation. this method
     * checks for the boundary condition on movement.
     */
    private PointF getValidXYOnPan(float fromX, float fromY, float toX, float toY) {
        float validX = toX;
        float validY = toY;

        float currentImageWidth = toScreenUnits(imageWidth);
        float currentImageHeight = toScreenUnits(imageHeight);
        if (currentImageWidth >= displayWidth) {
            if (fromX < toX) {// move down
                if (fromX > 0f && toX > 0f) {
                    validX = fromX;

                } else if (fromX <= 0f && toX > 0f) {
                    validX = 0f;
                }
            } else { // move up
                float diffX = toImageUnits(displayWidth - currentImageWidth);
                if (fromX < diffX && toX < diffX) {
                    validX = fromX;

                } else if (fromX >= diffX && toX < diffX) {
                    validX = diffX;
                }
            }
        } else {
            if (fromX >= 0f && toX < 0f) {
                validX = 0f;
            } else {
                float diffX = toImageUnits(displayWidth - currentImageWidth);
                if (toX > diffX) {
                    validX = diffX;
                }
            }
        }

        if (currentImageHeight >= displayHeight) {
            if (fromY < toY) {// move right
                if (fromY > 0f && toY > 0f) {
                    validY = fromY;
                } else if (fromY <= 0f && toY > 0f) {
                    validY = 0f;
                }
            } else {// move left
                float diffY = toImageUnits(displayHeight - currentImageHeight);
                if (fromY < diffY && toY < diffY) {
                    validY = fromY;

                } else if (fromY >= diffY && toY < diffY) {
                    validY = diffY;
                }
            }
        } else {
            if (fromY >= 0f && toY < 0f) {
                validY = 0f;
            } else {
                float diffY = toImageUnits(displayHeight - currentImageHeight);
                if (toY > diffY) {
                    validY = diffY;
                }
            }
        }

        validTranslation.set(validX, validY);
        return validTranslation;
    }

    /**
     * Validate translation values after applying translation. this method
     * checks for the boundary condition on movement.
     */
    private PointF getValidXYForCenterType() {
        float toX = translateX;
        float toY = translateY;

        float currentImageWidth = toScreenUnits(imageWidth);
        float currentImageHeight = toScreenUnits(imageHeight);

        // validate x value
        if (currentImageWidth >= displayWidth) {
            if (toX > 0) {
                toX = 0;
            }
            float diffX = toImageUnits(displayWidth - currentImageWidth);
            if (toX < diffX) {
                toX = diffX;
            }
        } else {// image smaller in height than canvas
            toX = toImageUnits(displayWidth - currentImageWidth) / 2;
        }

        // validate x value
        if (imageHeight * scaleFactor >= displayHeight) {
            if (toY > 0) {
                toY = 0;
            }
            float diffY = toImageUnits(displayHeight - currentImageHeight);
            if (toY < diffY) {
                toY = diffY;
            }
        } else {// image smaller in height than canvas
            toY = toImageUnits(displayHeight - currentImageHeight) / 2;
        }
        validTranslation.set(toX, toY);
        return validTranslation;
    }

    private boolean animateToReposition(float toX, float toY) {
        if (translateX == toX && translateY == toY) {
            return false;
        }
        float byX = toScreenUnits(translateX - toX);
        float byY = toScreenUnits(translateY - toY);

        long duration = (long) (Math.max(Math.abs(byX), Math.abs(byY)) * TRANSLATE_MOVE_DELAY);
        if (duration < MIN_DELAY) {
            duration = MIN_DELAY;
        } else if (duration > MAX_DELAY) {
            Log.d(LOG_TAG, "Max delay crossed: x=" + byX + ",y=" + byX);
            duration = MAX_DELAY;
        }
        Log.d(LOG_TAG, "panIn Manual duration: " + duration);

        PropertyValuesHolder translateXProperty = PropertyValuesHolder
                .ofFloat("x", translateX, toX);
        PropertyValuesHolder translateYProperty = PropertyValuesHolder
                .ofFloat("y", translateY, toY);
        ValueAnimator animation = ValueAnimator.ofPropertyValuesHolder(translateXProperty,
                translateYProperty);
        animation.setDuration(duration);
        animation.setInterpolator(new DecelerateInterpolator());
        animation.addUpdateListener(new AnimatorUpdateListener() {
            /**
             * {@inheritDoc}
             */
            @Override
            public void onAnimationUpdate(ValueAnimator animation) {
                translateX = (Float) animation.getAnimatedValue("x");
                translateY = (Float) animation.getAnimatedValue("y");

                if (eventListener != null) {
                    eventListener.onScaleAnimationUpdate();
                }
            }
        });
        animation.start();
        animation.addListener(new AnimatorListener() {
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
                if (eventListener != null) {
                    eventListener.onScaleDone();
                }
            }

            /**
             * {@inheritDoc}
             */
            @Override
            public void onAnimationCancel(Animator animation) {
            }
        });
        return true;
    }
}
