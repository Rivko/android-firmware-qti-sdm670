/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.panoramaui;

import android.graphics.Canvas;
import android.graphics.ColorFilter;
import android.graphics.Paint;
import android.graphics.RectF;
import android.graphics.drawable.Drawable;

/**
 * Class to show the AF indicator
 */
public class AfIndicator extends Drawable {
    // private static final String LOG_TAG = "AfIndicator";

    private static final float ROTATE_MAX = 40.0f;

    /**
     * AF Indicator states
     */
    public enum IndicatorState {
        START, PROGRESS, DONE,
    }

    private IndicatorState state = IndicatorState.START;
    private float rotateOffset = 0;

    private boolean haveOpacity = false;
    private int opacity = 0;

    private float atX = 0;
    private float atY = 0;
    private final float outerRadius;
    private final float innerRadius;
    private final RectF arcRect = new RectF();

    private final Paint circlePaint = new Paint(Paint.ANTI_ALIAS_FLAG);
    private final Paint progressArcsPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
    private final Paint doneArcsPaint = new Paint(Paint.ANTI_ALIAS_FLAG);

    /**
     * Constructor for the AF indicator
     *
     * @param progressColor
     *            color to be used while AF progress
     * @param doneColor
     *            color to be used when AF completed successfully
     * @param outerCircleThick
     *            Indicator outer circle thickness
     * @param arcThick
     *            Indicator inner arcs thickness
     * @param outerRadius
     *            Indicator outer arc radius
     * @param innerRadius
     *            Indicator inner arc radius
     */
    /* package */AfIndicator(int progressColor, int doneColor, float outerCircleThick,
            float arcThick, float outerRadius, float innerRadius) {
        this.outerRadius = outerRadius;
        this.innerRadius = innerRadius;

        circlePaint.setStyle(Paint.Style.STROKE);
        circlePaint.setColor(progressColor);
        circlePaint.setStrokeWidth(outerCircleThick);

        progressArcsPaint.setStyle(Paint.Style.STROKE);
        progressArcsPaint.setColor(progressColor);
        progressArcsPaint.setStrokeWidth(arcThick);

        doneArcsPaint.setStyle(Paint.Style.STROKE);
        doneArcsPaint.setColor(doneColor);
        doneArcsPaint.setStrokeWidth(arcThick);
    }

    /**
     * Sets the location of the indicator
     *
     * @param x
     *            x position for the indicator
     * @param y
     *            y position for the indicator
     */
    public void setLocation(float x, float y) {
        atX = x;
        atY = y;

        arcRect.set(x - innerRadius, y - innerRadius, x + innerRadius, y + innerRadius);
    }

    /**
     * method to set the indicator state
     *
     * @param state
     *            insicator state
     */
    public void setIndicatorState(IndicatorState state) {
        if (state == IndicatorState.START) {
            rotateOffset = 0;
        }
        if (this.state == state) {
            return;
        }
        this.state = state;
        invalidateSelf();
    }

    /**
     * Sets the progress of the AF
     *
     * @param fraction
     *            fraction between 0 to 1
     */
    public void setProgress(float fraction) {
        rotateOffset = ROTATE_MAX * fraction;
        invalidateSelf();
    }

    /** {@inheritDoc} */
    @Override
    public void draw(Canvas canvas) {
        canvas.drawCircle(atX, atY, outerRadius, circlePaint);
        if (state == IndicatorState.DONE) {
            canvas.drawArc(arcRect, 225, 90, false, doneArcsPaint);
            canvas.drawArc(arcRect, 45, 90, false, doneArcsPaint);

        } else {
            canvas.drawArc(arcRect, 225, 90, false, progressArcsPaint);
            canvas.drawArc(arcRect, 45, 90, false, progressArcsPaint);

            canvas.drawArc(arcRect, 185 + rotateOffset, 90, false, progressArcsPaint);
            canvas.drawArc(arcRect, 5 + rotateOffset, 90, false, progressArcsPaint);
        }

    }

    /** {@inheritDoc} */
    @Override
    public void setAlpha(int alpha) {
        circlePaint.setAlpha(alpha);
        progressArcsPaint.setAlpha(alpha);
        doneArcsPaint.setAlpha(alpha);
    }

    /** {@inheritDoc} */
    @Override
    public void setColorFilter(ColorFilter filter) {
        circlePaint.setColorFilter(filter);
        progressArcsPaint.setColorFilter(filter);
        doneArcsPaint.setColorFilter(filter);
    }

    /** {@inheritDoc} */
    @Override
    public int getOpacity() {
        if (haveOpacity) {
            return opacity;
        }
        opacity = Drawable.resolveOpacity(opacity, circlePaint.getAlpha());
        opacity = Drawable.resolveOpacity(opacity, progressArcsPaint.getAlpha());
        opacity = Drawable.resolveOpacity(opacity, doneArcsPaint.getAlpha());

        haveOpacity = true;
        return opacity;
    }

}
