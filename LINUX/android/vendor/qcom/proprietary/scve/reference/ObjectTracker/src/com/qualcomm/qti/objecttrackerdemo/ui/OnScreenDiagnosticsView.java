/*
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.objecttrackerdemo.ui;

import java.util.ArrayDeque;
import java.util.concurrent.atomic.AtomicReference;

import com.qualcomm.qti.objecttracker.aeafcontroller.AeAfController;
import com.qualcomm.qti.objecttracker.cameraengine.Frame;
import com.qualcomm.qti.objecttracker.cameraengine.FrameDispatcher;
import com.qualcomm.qti.objecttracker.cameraengine.FrameSink;
import com.qualcomm.qti.objecttracker.misc.CoordinateSpace;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Paint.FontMetrics;
import android.util.AttributeSet;
import android.util.DisplayMetrics;
import android.view.View;

/**
 * This class provides a basic set of on-screen diagnostics for profiling & debugging the object
 * tracker performance
 */
public class OnScreenDiagnosticsView extends View {
    private static final float PADDING_DP = 10f;
    private static final float TEXT_SIZE_SP = 6f;
    private static final float TEXT_SHADOW_LAYER_RADIUS_DP = 10f;

    private final AtomicReference<String> fps = new AtomicReference<String>();
    private final AtomicReference<String> aeAf = new AtomicReference<String>();
    private final Paint textPaint;
    private final FontMetrics textFontMetrics;
    private final float padding;

    /**
     * Construct an {@link OnScreenDiagnosticsView}
     * @param context Application {@link Context}
     */
    public OnScreenDiagnosticsView(Context context) {
        this(context, null, 0);
    }

    /**
     * Construct an {@link OnScreenDiagnosticsView}
     * @param context The Context the view is running in, through which it can access the current
     *      theme, resources, etc
     * @param attrs The attributes of the XML tag that is inflating the view
     */
    public OnScreenDiagnosticsView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    /**
     * Construct an {@link OnScreenDiagnosticsView}
     * @param context The Context the view is running in, through which it can access the current
     *      theme, resources, etc
     * @param attrs The attributes of the XML tag that is inflating the view
     * @param defStyleAttr An attribute in the current theme that contains a reference to a style
     *      resource to apply to this view. If 0, no default style will be applied
     */
    public OnScreenDiagnosticsView(Context context, AttributeSet attrs,
            int defStyleAttr) {
        super(context, attrs, defStyleAttr);

        float density = context.getResources().getDisplayMetrics().density;

        padding = PADDING_DP * density;

        textPaint = new Paint(Paint.HINTING_ON | Paint.SUBPIXEL_TEXT_FLAG);
        textPaint.setColor(Color.WHITE);
        textPaint.setTextSize(TEXT_SIZE_SP / 72f * DisplayMetrics.DENSITY_DEFAULT * density);
        textPaint.setTextAlign(Paint.Align.LEFT);
        // N.B. Max shadow layer radius is 25f, per ScriptIntrinsicBlur
        textPaint.setShadowLayer(Math.min(TEXT_SHADOW_LAYER_RADIUS_DP * density, 25f), 0, 0,
                Color.BLACK);
        textFontMetrics = textPaint.getFontMetrics();
    }

    /**
     * Attach this {@link OnScreenDiagnosticsView} as a frame sink to the given
     * {@link FrameDispatcher}
     * @param dispatcher {@link FrameDispatcher} on which to listen for frames
     */
    public void attachFrameDispatcher(FrameDispatcher dispatcher) {
        dispatcher.addFrameSink(sink);
    }

    /**
     * Attach this {@link OnScreenDiagnosticsView} as a diagnostic listener to the given
     * {@link AeAfController}
     * @param aeAfController an {@link AeAfController} from which to receive diagnostic data
     */
    public void attachAeAfController(AeAfController aeAfController) {
        aeAfController.setDiagnosticListener(aeAfDiagnosticListener);
    }

    /** {@inheritDoc} */
    @Override
    protected void onDraw(Canvas canvas) {
        float rowBaseline = padding - textFontMetrics.ascent;
        float rowHeight = textFontMetrics.descent - textFontMetrics.ascent + textFontMetrics.leading;

        String fpsLocal = (String)fps.get();
        if (fpsLocal != null) {
            canvas.drawText(fpsLocal, 0, rowBaseline, textPaint);
            rowBaseline += rowHeight;
        }

        String aeAfLocal = (String)aeAf.get();
        if (aeAfLocal != null) {
            for (String s : aeAfLocal.split("\n")) {
                canvas.drawText(s, 0, rowBaseline, textPaint);
                rowBaseline += rowHeight;
            }
        }
    }

    private final FrameSink sink = new FrameSink() {
        private static final int FRAME_HISTORY_SIZE = 5;

        private final ArrayDeque<Long> frameTimes = new ArrayDeque<Long>();

        @Override
        public void onInitialize(CoordinateSpace cameraSpace) {
            frameTimes.clear();
        }

        @Override
        public void onRelease() {
        }

        @Override
        public void onFrame(Frame frame) {
            long time = System.nanoTime();
            frameTimes.add(time);
            if (frameTimes.size() > FRAME_HISTORY_SIZE) {
                fps.set(String.format("FPS: %.0f", FRAME_HISTORY_SIZE * 1000000000f /
                        (float)(time - frameTimes.pop())));
            } else {
                fps.set("FPS: 0");
            }
            postInvalidate();

            frame.release();
        }
    };

    private final AeAfController.DiagnosticListener aeAfDiagnosticListener =
            new AeAfController.DiagnosticListener() {
        @Override
        public void onUpdate(String diagnostics) {
            aeAf.set(diagnostics);
            postInvalidate();
        }
    };
}
