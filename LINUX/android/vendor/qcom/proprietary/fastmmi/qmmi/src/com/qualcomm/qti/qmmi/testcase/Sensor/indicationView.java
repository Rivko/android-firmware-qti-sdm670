/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.qmmi.testcase.Sensor;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.RectF;
import android.graphics.Typeface;
import android.util.AttributeSet;
import android.view.View;
import android.hardware.Sensor;

public class indicationView extends View {
    private final int mCircleLineStrokeWidth = 8;
    private final double M_PI = 3.14159265358979323846;
    private Paint mIndicationPaint;
    private int mSensorType = 0;
    private int mAngle = 0;

    public indicationView(Context context) {
        this(context, null);
    }

    public indicationView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public indicationView(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);

        mIndicationPaint = new Paint();
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);

        if(mSensorType == Sensor.TYPE_MAGNETIC_FIELD) {
            int centre = getWidth() / 2;
            int radius = (int) (centre - getWidth() / 4);
            mIndicationPaint.setColor(Color.RED);
            mIndicationPaint.setStyle(Paint.Style.STROKE);
            mIndicationPaint.setStrokeWidth(mCircleLineStrokeWidth);
            mIndicationPaint.setAntiAlias(true);
            canvas.drawCircle(centre, centre, radius, mIndicationPaint);

            mIndicationPaint.setStrokeWidth(0);
            mIndicationPaint.setTypeface(Typeface.DEFAULT_BOLD);
            mIndicationPaint.setStrokeWidth(mCircleLineStrokeWidth);
            mIndicationPaint.setColor(Color.GREEN);
            RectF oval = new RectF(centre - radius, centre - radius, centre
                         + radius, centre + radius);
            mIndicationPaint.setStyle(Paint.Style.STROKE);
            canvas.drawArc(oval, -90, mAngle, false, mIndicationPaint);
        }
    }

    public synchronized void setValue(String typeStr, String xStr, String yStr, String zStr) {
        double x = 0.0;
        double y = 0.0;
        double z = 0.0;
        double azimuth = 0;
        double rad2deg = 180 / M_PI;

        mSensorType = Integer.parseInt(typeStr);
        if(mSensorType == Sensor.TYPE_MAGNETIC_FIELD) {
            x = Double.parseDouble(xStr);
            y = Double.parseDouble(yStr);
            z = Double.parseDouble(zStr);

            azimuth = Math.atan2(y,-x);
            mAngle = (int) (azimuth * rad2deg) + 90;
            if(mAngle < 0)
            mAngle = 360 + mAngle;
        }

        postInvalidate();
    }

}