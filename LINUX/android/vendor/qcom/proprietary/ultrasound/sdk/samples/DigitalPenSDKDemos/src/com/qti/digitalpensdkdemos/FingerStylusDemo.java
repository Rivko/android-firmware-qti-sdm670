/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 * Not a Contribution.
 */
 /*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.qti.digitalpensdkdemos;

import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Path;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.LinearLayout;
import android.widget.TextView;
import java.text.DecimalFormat;

import com.qti.snapdragon.sdk.digitalpen.DigitalPenManager;
import com.qti.snapdragon.sdk.digitalpen.DigitalPenManager.OffScreenMode;

public class FingerStylusDemo extends Activity {

    /* The digital pen manager object */
    private DigitalPenManager mDigitalPenManager = null;

    private DrawingView mDrawingView = null;

    private TextView mPressureView = null;
    DecimalFormat form = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_finger_stylus_demo);
        mPressureView = (TextView)findViewById(R.id.pressure_view_content);

        // Initialize the drawing view and erase button
        LinearLayout drawingPane = (LinearLayout) findViewById(R.id.draw_area);
        mDrawingView = new DrawingView(this);
        drawingPane.addView(mDrawingView);

        // set erase button
        findViewById(R.id.buttonEraseCanvas).setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                mDrawingView.erase();
            }
        });

        mDigitalPenManager = new DigitalPenManager(getApplication());
        form = new DecimalFormat("0.000");

        // Enables the off-screen mode in duplicate mode. Events made by the pen
        // originating from the off-screen area will act as though they originated
        // from the on-screen area.
        mDigitalPenManager.getSettings()
                .setOffScreenMode(OffScreenMode.DUPLICATE)
                .apply();
    }

    private class DrawingView extends View {

        private static final int BACKGROUND_COLOR = Color.GRAY;

        private int mBackgroundPaintColor;

        private static final float BASE_STROKE_WIDTH = 10f;

        private static final float BACKGROUND_TEXT_SIZE = 300f;

        private float mPressure;

        private final Path mPath;

        private final Paint mPaint;

        private Bitmap mBitmap;

        private Canvas mCanvas;

        private final Paint mBitmapPaint;

        private final Paint mBackGroundPaint;

        public DrawingView(Context c) {
            super(c);

            mPath = new Path();

            mPaint = new Paint();
            mPaint.setAntiAlias(true);
            mPaint.setDither(true);
            mPaint.setColor(Color.RED);
            mPaint.setStyle(Paint.Style.STROKE);
            mPaint.setStrokeJoin(Paint.Join.ROUND);
            mPaint.setStrokeCap(Paint.Cap.ROUND);
            mPressure = 1f;
            mPaint.setStrokeWidth(BASE_STROKE_WIDTH);

            mBitmapPaint = new Paint(Paint.DITHER_FLAG);
            // change the gray color a bit
            mBackgroundPaintColor = Color.rgb( Color.red(BACKGROUND_COLOR) + 10,
                                               Color.green(BACKGROUND_COLOR)+ 10,
                                               Color.blue(BACKGROUND_COLOR) + 10);
            mBackGroundPaint = new Paint();
            mBackGroundPaint.setAntiAlias(true);
            mBackGroundPaint.setDither(true);
            mBackGroundPaint.setColor(mBackgroundPaintColor);
            mBackGroundPaint.setStyle(Paint.Style.FILL);
            mBackGroundPaint.setStrokeJoin(Paint.Join.ROUND);
            mBackGroundPaint.setStrokeCap(Paint.Cap.ROUND);
            mBackGroundPaint.setStrokeWidth(BASE_STROKE_WIDTH);
            mBackGroundPaint.setTextSize(BACKGROUND_TEXT_SIZE);
        }

        @Override
        protected void onSizeChanged(int w, int h, int oldw, int oldh) {
            super.onSizeChanged(w, h, oldw, oldh);
            mBitmap = Bitmap.createBitmap(w, h, Bitmap.Config.ARGB_8888);
            mCanvas = new Canvas(mBitmap);

            // Set background.
            float width = mCanvas.getWidth();
            float height = mCanvas.getHeight();
            mCanvas.drawText("| | |", width/16, height*(0.30f), mBackGroundPaint);
            mCanvas.drawText("| | |", width/16, height*(0.45f), mBackGroundPaint);
            mCanvas.drawText("| | |", width/16, height*(0.60f), mBackGroundPaint);
            mCanvas.drawText("| | |", width/16, height*(0.75f), mBackGroundPaint);

            mBackGroundPaint.setTextSize(BACKGROUND_TEXT_SIZE/2);
            mCanvas.drawText("|   |", width*0.4f, height*(0.15f), mBackGroundPaint);
            mCanvas.drawText("|   |", width*0.4f, height*(0.35f), mBackGroundPaint);
            mCanvas.drawText("|   |", width*0.4f, height*(0.55f), mBackGroundPaint);
            mCanvas.drawText("|   |", width*0.4f, height*(0.75f), mBackGroundPaint);
            mCanvas.drawText("|   |", width*0.4f, height*(0.95f), mBackGroundPaint);
            mBackGroundPaint.setTextSize(BACKGROUND_TEXT_SIZE);

            mCanvas.drawText("8 8", width*0.65f, height*(0.40f), mBackGroundPaint);
            mCanvas.drawText("8 8", width*0.65f, height*(0.80f), mBackGroundPaint);
        }

        @Override
        protected void onDraw(Canvas canvas) {
            canvas.drawColor(BACKGROUND_COLOR);

            canvas.drawBitmap(mBitmap, 0, 0, mBitmapPaint);

            canvas.drawPath(mPath, mPaint);
        }

        private void touch_start(float x, float y, int tooltype) {
            switch (tooltype) {
                // If the touch event was caused by a finger touching the screen,
                // the TOOL_TYPE_FINGER is sent in the MotionEvent. If the touch
                // event originated from the pen, a TOOL_TYPE_STYLUS is sent.
                // If eraser was on, a TOOL_TYPE_ERASER is sent, and we
                // consider it as a TOOL_TYPE_STYLUS for this demo.
                case MotionEvent.TOOL_TYPE_STYLUS:
                case MotionEvent.TOOL_TYPE_ERASER:
                    mPaint.setColor(Color.RED);
                    break;
                case MotionEvent.TOOL_TYPE_FINGER:
                    mPaint.setColor(Color.GREEN);
                    break;
            }
            mPath.reset();
            mPath.moveTo(x, y);
        }

        private void touch_move(float x, float y) {
            mPath.lineTo(x, y);
        }

        private void touch_up() {
            mCanvas.drawPath(mPath, mPaint);
        }

        @Override
        public boolean onTouchEvent(MotionEvent event) {
            float x = event.getX();
            float y = event.getY();
            float newPressure = event.getPressure();
            int action = event.getAction();

            // if pressure is 1% different than last event
            if ( newPressure > mPressure + 0.01f || newPressure < mPressure -0.01f ) {
                mPressure = newPressure;
                mPressureView.setText(form.format(mPressure));

                // Draw path, unless pen was up.
                if (action != MotionEvent.ACTION_DOWN) {
                    // This line to is necessary because it fills the empty gaps.
                    mPath.lineTo(x, y);
                    mCanvas.drawPath(mPath, mPaint);
                }
                // replace with event.getToolType(0)
                touch_start(x, y, event.getToolType(0));
                mPaint.setStrokeWidth(mPressure*BASE_STROKE_WIDTH);

            }
            else{
                switch (action) {
                case MotionEvent.ACTION_DOWN:
                    touch_start(x, y, event.getToolType(0));
                    break;
                case MotionEvent.ACTION_MOVE:
                    touch_move(x, y);
                    break;
                case MotionEvent.ACTION_UP:
                    touch_up();
                    mPressureView.setText("0.0");
                    break;
                default:
                    return true;
                }
            }
            invalidate();
            return true;
        }

        public void erase() {
            // erase last path
            mPath.reset();

            // erase whole canvas
            mCanvas.drawColor(BACKGROUND_COLOR);
            invalidate();
        }
    }

}

