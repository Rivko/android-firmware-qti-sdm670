/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.clevercaptureui;

import android.content.res.Resources;
import android.graphics.Canvas;
import android.graphics.ColorFilter;
import android.graphics.Paint;
import android.graphics.PointF;
import android.graphics.Rect;
import android.graphics.RectF;
import android.graphics.drawable.Drawable;
import android.util.Log;

/**
 * Class for displaying ROI
 */
public class ROIDrawable extends Drawable {
    private static final String LOG_TAG = "ROIDrawable";

    public static final int TOPLEFT = 0;
    public static final int TOP = 1;
    public static final int TOPRIGHT = 2;
    public static final int RIGHT = 3;
    public static final int BOTTOMRIGHT = 4;
    public static final int BOTTOM = 5;
    public static final int BOTTOMLEFT = 6;
    public static final int LEFT = 7;
    public static final int INNER = 8;
    public static final int MAXINDEX = 9;
    private int mStartIndex = -1; // 0 topleft; 1 top; 2 topright; 3 right; 4 bottomright;
                                  // 5 bottom; 6 bottomleft; 7 left; 8 inner; -1 invalid;

    // grid
    private final float gridCellSize;
    private float[] linesAtX = null;
    private float[] linesAtY = null;
    private final Paint gridPaint = new Paint(Paint.ANTI_ALIAS_FLAG);

    // background
    private final RectF leftRect = new RectF();
    private final RectF topRect = new RectF();
    private final RectF rightRect = new RectF();
    private final RectF bottomRect = new RectF();
    private final Paint backgroundPaint = new Paint(Paint.ANTI_ALIAS_FLAG);

    // frame
    private final RectF frame = new RectF();
    private final RectF []frameLines = new RectF[8];
    private final Paint framePaint = new Paint(Paint.ANTI_ALIAS_FLAG);

    // frame handlers
    private final Paint handlerPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
    private final float handlerSize;
    private final RectF[] handlers = new RectF[MAXINDEX];

    private final float minFrameWidth;
    private final float minFrameHeight;

    private final float frameStroke;
    private final float frameThickStroke;

    private boolean haveOpacity = false;
    private int opacity = 0;

    private float frameWidthLocked = 0;
    private float frameHeightLocked = 0;

    private boolean mDragEdgeNeedUpdate = false;
    private boolean mLocked = false;
    private static final int mRange = 50;

    /**
     * Constructor for the ROI Drawable
     *
     * @param resources
     *            application resources
     */
    /* package */ROIDrawable(Resources resources) {
        minFrameWidth = resources.getDimension(R.dimen.min_frame_width);
        minFrameHeight = resources.getDimension(R.dimen.min_frame_height);
        frameStroke = resources.getDimension(R.dimen.roi_frame_stroke);
        frameThickStroke = resources.getDimension(R.dimen.roi_frame_thick_stroke);
        handlerSize = resources.getDimension(R.dimen.handler_size);
        gridCellSize = resources.getDimension(R.dimen.roi_grid_size);

        int backgroundColor = resources.getColor(R.color.faded_background);
        int frameColor = resources.getColor(R.color.roi_frame);
        int gridColor = resources.getColor(R.color.grid);

        backgroundPaint.setColor(backgroundColor);
        handlerPaint.setStyle(Paint.Style.STROKE);
        handlerPaint.setColor(frameColor);
        handlerPaint.setStrokeWidth(frameStroke);

        framePaint.setStyle(Paint.Style.STROKE);
        framePaint.setColor(frameColor);
        framePaint.setStrokeWidth(frameStroke);

        gridPaint.setStyle(Paint.Style.STROKE);
        gridPaint.setColor(gridColor);
        gridPaint.setStrokeWidth(0);

        for(int i = 0; i< frameLines.length; i++) {
            frameLines[i] = new RectF();
        }

        for(int i = 0; i < MAXINDEX; i++) {
            handlers[i] = new RectF();
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onBoundsChange(Rect bounds) {
        calculateGridParameters(bounds);

        updatePositions(bounds, bounds.width() * 0.5f, bounds.height() * 0.5f);
    }

    private void calculateGridParameters(Rect viewArea) {
        float xParts = viewArea.width() / gridCellSize;
        int xLines = (int) xParts;
        if (xParts > (float) xLines) {
            xLines++;
        }

        float yParts = viewArea.height() / gridCellSize;
        int yLines = (int) yParts;
        if (yParts > (float) yLines) {
            yLines++;
        }

        linesAtX = new float[xLines];
        linesAtY = new float[yLines];

        for (int i = 0; i < xLines; i++) {
            linesAtX[i] = (i + 1) * gridCellSize;
        }

        for (int i = 0; i < yLines; i++) {
            linesAtY[i] = (i + 1) * gridCellSize;
        }
    }

    private void updatePositions(Rect viewArea, float width, float height) {
        if (width == frame.width() && height == frame.height()) {
            //Log.d(LOG_TAG, "Already at correct size");
            return;
        }
        float left = viewArea.centerX() - width / 2;
        float top = viewArea.centerY() - height / 2;
        float right = viewArea.centerX() + width / 2;
        float bottom = viewArea.centerY() + height / 2;

        frame.set(left, top, right, bottom);

        // update background
        topRect.set(0, 0, viewArea.width(), frame.top);
        leftRect.set(0, frame.top, frame.left, frame.bottom);
        rightRect.set(frame.right, frame.top, viewArea.width(), frame.bottom);
        bottomRect.set(0, frame.bottom, viewArea.width(), viewArea.height());

        // update handlers
        float handlerCenterX = handlerSize / 2;
        float handlerCenterY = handlerSize / 2;
        RectF topHandler = handlers[TOP];
        RectF leftHandler = handlers[LEFT];
        RectF rightHandler = handlers[RIGHT];
        RectF bottomHandler = handlers[BOTTOM];
        RectF topleftHandler = handlers[TOPLEFT];
        RectF toprightHandler = handlers[TOPRIGHT];
        RectF bottomleftHandler = handlers[BOTTOMLEFT];
        RectF bottomrightHandler = handlers[BOTTOMRIGHT];

        setHandler(topHandler, frame.centerX() - handlerCenterX, frame.top - handlerCenterY);
        setHandler(leftHandler, frame.left - handlerCenterX, frame.centerY() - handlerCenterY);
        setHandler(rightHandler, frame.right - handlerCenterX, frame.centerY() - handlerCenterY);
        setHandler(bottomHandler, frame.centerX() - handlerCenterX, frame.bottom - handlerCenterY);
        setHandler(topleftHandler, frame.left - handlerCenterX, frame.top - handlerCenterY);
        setHandler(toprightHandler, frame.right - handlerCenterX, frame.top - handlerCenterY);
        setHandler(bottomleftHandler, frame.left - handlerCenterX, frame.bottom - handlerCenterY);
        setHandler(bottomrightHandler, frame.right - handlerCenterX, frame.bottom - handlerCenterY);

        // update frame lines
        frameLines[0].set(topleftHandler.right, frame.top, topHandler.left, frame.top);
        frameLines[1].set(topHandler.right, frame.top, toprightHandler.left, frame.top);
        frameLines[2].set(frame.left, topleftHandler.bottom, frame.left, leftHandler.top);
        frameLines[3].set(frame.left, leftHandler.bottom, frame.left, bottomleftHandler.top);
        frameLines[4].set(frame.right, toprightHandler.bottom, frame.right, rightHandler.top);
        frameLines[5].set(frame.right, rightHandler.bottom, frame.right, bottomrightHandler.top);
        frameLines[6].set(bottomleftHandler.right, frame.bottom, bottomHandler.left, frame.bottom);
        frameLines[7].set(bottomHandler.right, frame.bottom, bottomrightHandler.left, frame.bottom);

        invalidateSelf();
    }

    private void updateFrame(Rect viewArea, RectF frameArea) {
        frame.set(frameArea);

        // update background
        topRect.set(0, 0, viewArea.width(), frame.top);
        leftRect.set(0, frame.top, frame.left, frame.bottom);
        rightRect.set(frame.right, frame.top, viewArea.width(), frame.bottom);
        bottomRect.set(0, frame.bottom, viewArea.width(), viewArea.height());

        // update handlers
        float handlerCenterX = handlerSize / 2;
        float handlerCenterY = handlerSize / 2;
        RectF topHandler = handlers[TOP];
        RectF leftHandler = handlers[LEFT];
        RectF rightHandler = handlers[RIGHT];
        RectF bottomHandler = handlers[BOTTOM];
        RectF topleftHandler = handlers[TOPLEFT];
        RectF toprightHandler = handlers[TOPRIGHT];
        RectF bottomleftHandler = handlers[BOTTOMLEFT];
        RectF bottomrightHandler = handlers[BOTTOMRIGHT];

        setHandler(topHandler, frame.centerX() - handlerCenterX, frame.top - handlerCenterY);
        setHandler(leftHandler, frame.left - handlerCenterX, frame.centerY() - handlerCenterY);
        setHandler(rightHandler, frame.right - handlerCenterX, frame.centerY() - handlerCenterY);
        setHandler(bottomHandler, frame.centerX() - handlerCenterX, frame.bottom - handlerCenterY);
        setHandler(topleftHandler, frame.left - handlerCenterX, frame.top - handlerCenterY);
        setHandler(toprightHandler, frame.right - handlerCenterX, frame.top - handlerCenterY);
        setHandler(bottomleftHandler, frame.left - handlerCenterX, frame.bottom - handlerCenterY);
        setHandler(bottomrightHandler, frame.right - handlerCenterX, frame.bottom - handlerCenterY);

        // update frame lines
        frameLines[0].set(topleftHandler.right, frame.top, topHandler.left, frame.top);
        frameLines[1].set(topHandler.right, frame.top, toprightHandler.left, frame.top);
        frameLines[2].set(frame.left, topleftHandler.bottom, frame.left, leftHandler.top);
        frameLines[3].set(frame.left, leftHandler.bottom, frame.left, bottomleftHandler.top);
        frameLines[4].set(frame.right, toprightHandler.bottom, frame.right, rightHandler.top);
        frameLines[5].set(frame.right, rightHandler.bottom, frame.right, bottomrightHandler.top);
        frameLines[6].set(bottomleftHandler.right, frame.bottom, bottomHandler.left, frame.bottom);
        frameLines[7].set(bottomHandler.right, frame.bottom, bottomrightHandler.left, frame.bottom);

        invalidateSelf();
    }

    private void setHandler(RectF handlerRect, float left, float top) {
        handlerRect.set(left, top, left + handlerSize, top + handlerSize);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void draw(Canvas canvas) {
        Rect viewArea = getBounds();
        // draw grid
        for (int i = 0; i < linesAtX.length; i++) {
            canvas.drawLine(linesAtX[i], 0, linesAtX[i], viewArea.height(), gridPaint);
        }
        for (int i = 0; i < linesAtY.length; i++) {
            canvas.drawLine(0, linesAtY[i], viewArea.width(), linesAtY[i], gridPaint);
        }

        // draw background
        canvas.drawRect(topRect, backgroundPaint);
        canvas.drawRect(leftRect, backgroundPaint);
        canvas.drawRect(rightRect, backgroundPaint);
        canvas.drawRect(bottomRect, backgroundPaint);

        // draw frame lines and handlers
        for(RectF line: frameLines) {
            canvas.drawLine(line.left, line.top, line.right, line.bottom, framePaint);
        }

        canvas.drawRect(handlers[TOP], handlerPaint);
        canvas.drawRect(handlers[LEFT], handlerPaint);
        canvas.drawRect(handlers[RIGHT], handlerPaint);
        canvas.drawRect(handlers[BOTTOM], handlerPaint);

        canvas.drawRect(handlers[TOPLEFT], handlerPaint);
        canvas.drawRect(handlers[TOPRIGHT], handlerPaint);
        canvas.drawRect(handlers[BOTTOMLEFT], handlerPaint);
        canvas.drawRect(handlers[BOTTOMRIGHT], handlerPaint);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void setAlpha(int alpha) {
        backgroundPaint.setAlpha(alpha);
        framePaint.setAlpha(alpha);
        handlerPaint.setAlpha(alpha);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void setColorFilter(ColorFilter filter) {
        backgroundPaint.setColorFilter(filter);
        framePaint.setColorFilter(filter);
        handlerPaint.setColorFilter(filter);
    }

    /**
     * {@inheritDoc}
     */
    public int getOpacity() {
        if (haveOpacity) {
            return opacity;
        }
        opacity = Drawable.resolveOpacity(opacity, backgroundPaint.getAlpha());
        opacity = Drawable.resolveOpacity(opacity, framePaint.getAlpha());
        opacity = Drawable.resolveOpacity(opacity, handlerPaint.getAlpha());

        haveOpacity = true;
        return opacity;
    }

    /**
     * Unlock the view
     */
    /* package */void unlockView() {
        mLocked = false;
    }

    /**
     * lock the view
     */
    /* package */void lockView() {

        mLocked = true;
    }

    private float validateWidth(float rawWidth) {
        float validWidth = rawWidth;
        if (validWidth < minFrameWidth) {
            validWidth = minFrameWidth;
        } else if (validWidth > getBounds().width()) {
            validWidth = getBounds().width();
        }
        return validWidth;
    }

    private float validateHeight(float rawHeight) {
        float validHeight = rawHeight;
        if (validHeight < minFrameHeight) {
            validHeight = minFrameHeight;
        } else if (validHeight > getBounds().height()) {
            validHeight = getBounds().height();
        }
        return validHeight;
    }

    /**
     * Zooms the ROI with scale factor
     *
     * @param scale
     *            scale factor
     */
    /* package */void zoomBy(float scale) {
        float newWidth = validateWidth(frameWidthLocked * scale);
        float newHeight = validateHeight(frameHeightLocked * scale);
        updatePositions(getBounds(), newWidth, newHeight);
    }

    // validate frame rect, if no modification, return 0, otherwise return -1
    public boolean validateFrameRect(RectF newframe) {
        boolean res = false;

        if(newframe.left < 0) {
            newframe.left = 0;
            res = true;
        }

        if(newframe.right > getBounds().right) {
            newframe.right = getBounds().right;
            res = true;
        }

        if(newframe.top < 0) {
            newframe.top = 0;
            res = true;
        }

        if(newframe.bottom > getBounds().bottom) {
            newframe.bottom = getBounds().bottom;
            res = true;
        }

        return res;
    }

    public void zoom(PointF start, PointF from, float toX, float toY) {
        RectF newframe = new RectF(frame);
        float yDistance = toY - from.y;
        float xDistance = toX - from.x;
        float distance = 0;

      //Log.d(LOG_TAG, "move from " + newframe.toString());
        if(mStartIndex == TOPLEFT) {
            if(yDistance * xDistance < 0) {
                return;
            }

            if(yDistance < 0) {
                distance = -xDistance < -yDistance ? -xDistance : -yDistance;

                newframe.top -= distance;
                newframe.bottom += distance;
                newframe.left -= distance;
                newframe.right += distance;
            } else {
                distance = xDistance < yDistance ? xDistance : yDistance;

                newframe.top += distance;
                newframe.bottom -= distance;
                newframe.left += distance;
                newframe.right -= distance;
            }
        }

        if(mStartIndex == TOPRIGHT) {
            if(yDistance * xDistance > 0) {
                return;
            }

            if(yDistance < 0) {
                distance = xDistance < -yDistance ? xDistance : -yDistance;

                newframe.top -= distance;
                newframe.bottom += distance;
                newframe.left -= distance;
                newframe.right += distance;
            } else {
                distance = -xDistance < yDistance ? -xDistance : yDistance;

                newframe.top += distance;
                newframe.bottom -= distance;
                newframe.left += distance;
                newframe.right -= distance;
            }
        }

        if(mStartIndex == BOTTOMLEFT) {
            if(yDistance * xDistance > 0) {
                return;
            }

            if(yDistance < 0) {
                distance = xDistance < -yDistance ? xDistance : -yDistance;

                newframe.top += distance;
                newframe.bottom -= distance;
                newframe.left += distance;
                newframe.right -= distance;
            } else {
                distance = -xDistance < yDistance ? -xDistance : yDistance;

                newframe.top -= distance;
                newframe.bottom += distance;
                newframe.left -= distance;
                newframe.right += distance;
            }
        }

        if(mStartIndex == BOTTOMRIGHT)
        {
            if(yDistance * xDistance < 0) {
                return;
            }

            if(yDistance < 0) {
                distance = -xDistance < -yDistance ? -xDistance : -yDistance;

                newframe.top += distance;
                newframe.bottom -= distance;
                newframe.left += distance;
                newframe.right -= distance;
            } else {
                distance = xDistance < yDistance ? xDistance : yDistance;

                newframe.top -= distance;
                newframe.bottom += distance;
                newframe.left -= distance;
                newframe.right += distance;
            }
        }
        validateFrameRect(newframe);
        //Log.d(LOG_TAG, "move to " + newframe.toString());

        //updatePositions(bound, bound.width(), bound.height());
        if(newframe.width() >= minFrameWidth && newframe.height() >= minFrameHeight)
            updateFrame(getBounds(), newframe);
    }

    public void drag(PointF start, PointF from, float toX, float toY) {
        RectF newframe = new RectF(frame);
        float yDistance = toY - from.y;
        float xDistance = toX - from.x;

      //Log.d(LOG_TAG, "move from " + newframe.toString());
        if(mStartIndex == TOP) {
            if(frame.top == from.y) {
                newframe.top += yDistance;
                newframe.bottom -= yDistance;
            } else if(Math.abs(toY - newframe.top) < mRange) {
                yDistance = toY - newframe.top;

                newframe.top += yDistance;
                newframe.bottom -= yDistance;
            }
        }

        if(mStartIndex == BOTTOM) {
            if(frame.top == from.y) {
                newframe.bottom += yDistance;
                newframe.top -= yDistance;

            } else if(Math.abs(toY - newframe.bottom) < mRange) {
                yDistance = toY - newframe.bottom;

                newframe.bottom += yDistance;
                newframe.top -= yDistance;
            }
        }

        if(mStartIndex == RIGHT) {
            if(frame.right == from.x) {
                newframe.right += xDistance;
                newframe.left -= xDistance;
            } else if(Math.abs(toX - newframe.right) < mRange) {
                xDistance = toX - newframe.right;

                newframe.right += xDistance;
                newframe.left -= xDistance;
            }
        }

        if(mStartIndex == LEFT)
        {
            if(frame.left == from.x) {
                newframe.left += xDistance;
                newframe.right -= xDistance;
            } else if(Math.abs(toX - newframe.left) < mRange) {
                xDistance = toX - newframe.left;

                newframe.right += xDistance;
                newframe.left -= xDistance;
            }
        }
        validateFrameRect(newframe);
        //Log.d(LOG_TAG, "move to " + newframe.toString());

        //updatePositions(bound, bound.width(), bound.height());
        if(newframe.width() >= minFrameWidth && newframe.height() >= minFrameHeight)
            updateFrame(getBounds(), newframe);
    }

    public RectF copyFrameRectF() {
        return new RectF(frame);
    }

    public void move(PointF start, PointF from, float toX, float toY) {
        RectF newframe = new RectF(frame);
        float yDistance = toY - from.y;
        float xDistance = toX - from.x;

        Log.d(LOG_TAG, "move from " + newframe.toString());
        newframe.left += xDistance;
        newframe.right += xDistance;
        newframe.top += yDistance;
        newframe.bottom += yDistance;
        validateFrameRect(newframe);
        Log.d(LOG_TAG, "move to " + newframe.toString());

        //updatePositions(bound, bound.width(), bound.height());
        if(newframe.width() >= minFrameWidth && newframe.height() >= minFrameHeight)
            updateFrame(getBounds(), newframe);
    }

    /**
     * show/hides the UI related to taking snapshot
     *
     * @param enable
     *            true if ui needs to be shown, false otherwise
     */
    /* package */void enableSnapshotUI(boolean enable) {
        framePaint.setStrokeWidth(enable ? frameThickStroke : frameStroke);
        invalidateSelf();
    }

    public int updateStartPoint(float x, float y) {
        RectF tmp = new RectF();

        for(int i = 0; i < INNER; i++) {
            tmp.set(handlers[i].left - mRange, handlers[i].top - mRange, handlers[i].right + mRange, handlers[i].bottom + mRange);
            if(tmp.contains(x, y)) {
                mStartIndex = i;
                return mStartIndex;
            }
        }

        if(frame.contains(x, y)) {
            mStartIndex = INNER;
            return mStartIndex;
        }

        mStartIndex = -1;
        return mStartIndex;
    }
}
