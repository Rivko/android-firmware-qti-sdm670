/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
package com.qualcomm.qti.smartassistant.widget;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.PixelFormat;
import android.graphics.PorterDuff;
import android.util.AttributeSet;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import com.qualcomm.qti.smartassistant.R;

public class WaveSurfaceView extends SurfaceView implements SurfaceHolder.Callback, Runnable {
    private final Object mSurfaceLock = new Object();
    private SurfaceHolder mHolder;
    private Canvas mCanvas;
    private boolean mIsDrawing;

    private final int SLEEP_TIME = 20;

    private int mWidth, mHeight, mHalfHeight;

    private Paint mPaint1;
    private Path mPath1;

    private Paint mPaint2;
    private Path mPath2;

    private float mFactorW = 0.0f;
    private float[] decayRatios;
    private float[] mWaveY1;
    private float[] mWaveY2;
    private float mWaveHeight = 100.0f;

    private float mPhrase = 0.0f;
    private float mSpeed = 0.06f;
    private int mSampleRatio = 6;

    public WaveSurfaceView(Context context) {
        super(context);
        initView();
    }

    public WaveSurfaceView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public WaveSurfaceView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        initView();
    }

    public WaveSurfaceView(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        this(context, attrs, defStyleAttr);
    }

    private void initView() {
        mHolder = getHolder();
        mHolder.addCallback(this);
        this.setZOrderOnTop(true);
        mHolder.setFormat(PixelFormat.TRANSLUCENT);
        setFocusable(true);
        setFocusableInTouchMode(true);
        this.setKeepScreenOn(true);
        initPaint();
    }


    @Override
    public void surfaceCreated(SurfaceHolder surfaceHolder) {
        mIsDrawing = true;
        new Thread(this).start();
    }

    @Override
    public void surfaceChanged(SurfaceHolder surfaceHolder, int format, int width, int height) {
        mWidth = width;
        mHeight = height;
        mHalfHeight = mHeight / 2;
        initFactors();
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder surfaceHolder) {
        synchronized (mSurfaceLock) {
            mIsDrawing = false;
        }
    }

    private void initPaint() {
        mPaint1 = new Paint();
        mPaint1.setColor(getResources().getColor(R.color.color_wave_curve));
        mPaint1.setAntiAlias(true);
        mPaint1.setStyle(Paint.Style.STROKE);
        mPaint1.setStrokeWidth(3.0f);
        mPath1 = new Path();

        mPaint2 = new Paint();
        mPaint2.setColor(getResources().getColor(R.color.color_wave_curve));
        mPaint2.setAntiAlias(true);
        mPaint2.setStyle(Paint.Style.STROKE);
        mPath2 = new Path();
    }

    private void initFactors() {
        mFactorW = (float) (4 * Math.PI / mWidth);
        mWaveY1 = new float[mWidth / mSampleRatio];
        mWaveY2 = new float[mWidth / mSampleRatio];
        initRatio();
        updateWaveY();
    }

    private void initRatio() {
        decayRatios = new float[mWidth /mSampleRatio];
        for (int i = 0; i < mWidth / mSampleRatio; i++) {
            float ratio = 4.0f * i * mSampleRatio / mWidth - 2.0f;
            float tmp = (float)Math.pow(ratio, 4.0f);
            float decayRatio = (float)(Math.pow( 4 / (4 + tmp), 2.0f));
            decayRatios[i] = decayRatio;
        }
    }

    private void updateWaveY() {
        //TODO: update y position
        for (int i = 0; i < mWidth / mSampleRatio; i++) {
            mWaveY1[i] = mHalfHeight + (float)(decayRatios[i] * Math.sin(mFactorW * i * mSampleRatio
                    - mPhrase)) * mWaveHeight;
            mWaveY2[i] = mHalfHeight + (float)(decayRatios[i] * Math.sin(mFactorW * i * mSampleRatio
                    - mPhrase + Math.PI)) * mWaveHeight;
        }
    }

    private void doDraw(Canvas canvas) {
        updateWavePath();
        canvas.drawColor(Color.TRANSPARENT, PorterDuff.Mode.CLEAR);
        canvas.drawPath(mPath1, mPaint1);
        canvas.drawPath(mPath2, mPaint2);
    }

    private void updateWavePath() {
        updateWaveY();
        mPath1.rewind();
        mPath2.rewind();
        mPath1.moveTo(0, mHeight / 2);
        mPath2.moveTo(0, mHeight / 2);
        for (int i = 0; i < mWidth / mSampleRatio; i++) {
            mPath1.lineTo(i * mSampleRatio, mWaveY1[i]);
            mPath2.lineTo(i * mSampleRatio, mWaveY2[i]);
        }
        mPhrase = (float)((mPhrase + Math.PI * mSpeed) % (2 * Math.PI));
    }

    @Override
    public void run() {
        while (mIsDrawing) {
            draw();
            try {
                Thread.sleep(SLEEP_TIME);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }

    private void draw() {
        try {
            mCanvas = mHolder.lockCanvas();
            doDraw(mCanvas);
        } catch (Exception e) {

        } finally {
            if (mCanvas != null) {
                mHolder.unlockCanvasAndPost(mCanvas);
            }
        }
    }

}
