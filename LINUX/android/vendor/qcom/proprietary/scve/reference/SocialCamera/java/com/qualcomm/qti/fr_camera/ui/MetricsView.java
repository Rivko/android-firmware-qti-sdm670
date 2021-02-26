/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.fr_camera.ui;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.os.Handler;
import android.text.Layout;
import android.text.StaticLayout;
import android.text.TextPaint;
import android.util.AttributeSet;
import android.util.DisplayMetrics;
import android.view.View;

import com.qualcomm.qti.fr_camera.metrics.Metric;
import com.qualcomm.qti.fr_camera.metrics.Metrics;
import com.qualcomm.qti.fr_camera.metrics.MetricsPreferenceManager;

/**
 * This View displays the value of all registered {@link Metric}s, as reported by the singleton
 * {@link Metrics} instance. It is only minimally configurable/styleable, since it is only intended
 * for debugging purposes.
 */
public class MetricsView extends View {
    private static final float SHADOW_RADIUS = 15f; // N.B. Must be <= 25, see ScriptIntrinsicBlur
    private static final float TEXT_SIZE_SP = 6f;

    private final TextPaint paint = new TextPaint(Paint.ANTI_ALIAS_FLAG | Paint.HINTING_ON);
    private final Metrics metrics = Metrics.getInstance();
    private final MetricsPreferenceManager metricsPreferenceManager;
    private boolean isActive;
    private Handler handler;
    private Layout textLayout;

    /** @see android.view.View#View(android.content.Context) */
    public MetricsView(Context context) {
        this(context, null, 0, 0);
    }

    /** @see android.view.View#View(Context, AttributeSet) */
    public MetricsView(Context context, AttributeSet attrs) {
        this(context, attrs, 0, 0);
    }

    /** @see android.view.View#View(Context, AttributeSet, int) */
    public MetricsView(Context context, AttributeSet attrs, int defStyleAttr) {
        this(context, attrs, defStyleAttr, 0);
    }

    /** @see android.view.View#View(Context, AttributeSet, int, int) */
    public MetricsView(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);

        metricsPreferenceManager = new MetricsPreferenceManager(context);

        paint.setTextSize(TEXT_SIZE_SP * DisplayMetrics.DENSITY_DEFAULT / 72f *
                getContext().getResources().getDisplayMetrics().scaledDensity);
        paint.setColor(Color.WHITE);
        paint.setShadowLayer(SHADOW_RADIUS, 0f, 0f, Color.BLACK);

        textLayout = new StaticLayout("", paint, 0, Layout.Alignment.ALIGN_NORMAL, 1f, 0f, false);
    }

    @Override
    protected void onAttachedToWindow() {
        super.onAttachedToWindow();
        handler = getHandler();
    }

    @Override
    protected void onWindowVisibilityChanged(int visibility) {
        super.onWindowVisibilityChanged(visibility);

        if (visibility == View.VISIBLE) {
            isActive = metricsPreferenceManager.getDisplayMetrics();
            if (isActive) {
                setVisibility(View.VISIBLE);
                metrics.setMetricsUpdatedListener(listener, null);
            } else {
                setVisibility(View.GONE);
            }
        } else if (isActive) {
            isActive = false;
            metrics.setMetricsUpdatedListener(null, null);
        }
    }

    @Override
    protected void onDraw(Canvas canvas) {
        int saveCount = canvas.save();
        canvas.clipRect(getPaddingLeft(), getPaddingTop(),
                getWidth() - getPaddingRight(), getHeight() - getPaddingBottom());
        canvas.translate(getPaddingLeft(), getPaddingTop() - paint.ascent());
        textLayout.draw(canvas);
        canvas.restoreToCount(saveCount);
    }

    private final Metrics.MetricsUpdatedListener listener = new Metrics.MetricsUpdatedListener() {
        @Override
        public void onUpdateMetrics() {
            handler.removeCallbacks(updateMetricsRunnable);
            handler.post(updateMetricsRunnable);
        }
    };

    private final Runnable updateMetricsRunnable = new Runnable() {
        @Override
        public void run() {
            textLayout = new StaticLayout(metrics.getAggregateValue(), paint, Integer.MAX_VALUE,
                    Layout.Alignment.ALIGN_NORMAL, 1f, 0f, false);
            invalidate();
        }
    };
}
