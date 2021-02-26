/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.ipass;

import android.content.Context;
import android.util.Log;
import android.view.ViewGroup;
import android.view.TextureView;
import android.util.AttributeSet;
import android.view.Surface;
import android.graphics.SurfaceTexture;

public class IrisView extends ViewGroup implements TextureView.SurfaceTextureListener {
    private static final String TAG = "IrisView";
    private TextureView mTextureView;
    private int mCroppedX, mCroppedY;
    private int mCroppedWidth, mCroppedHeight;
    private int mPreviewWidth, mPreviewHeight;
    private double mScaleRation;

    private IrisViewListner mCallback;


    public static interface IrisViewListner {
        public void onSurfaceAvailable();
        public void onSurfaceDestroyed();
    };

    @Override
    public void onSurfaceTextureAvailable(SurfaceTexture surface,
        int width, int height) {
        Log.v(TAG, "onSurfaceTextureAvailable, width="+width+",height="+height);
        surface.setDefaultBufferSize(mPreviewWidth, mPreviewHeight);
        if (mCallback != null)
            mCallback.onSurfaceAvailable();
    }

    @Override
    public void onSurfaceTextureSizeChanged(SurfaceTexture surface,
            int width, int height) {
        Log.v(TAG, "onSurfaceTextureSizeChanged w="+width+" h="+height);
    }

    @Override
    public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
        Log.v(TAG, "onSurfaceTextureDestroyed");
        if (mCallback != null)
            mCallback.onSurfaceDestroyed();

        return true;
    }

    @Override
    public void onSurfaceTextureUpdated(SurfaceTexture surface) {
    }

    public IrisView( Context context ) {
        super( context );
        addTextureView(context);
    }

    public IrisView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public IrisView(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
        addTextureView(context);
    }

    private void addTextureView(Context context) {
        mTextureView = new TextureView(context);
        mTextureView.setSurfaceTextureListener(this);
        addView(mTextureView);
    }

    public Surface getSurface() {
        return new Surface(mTextureView.getSurfaceTexture());
    }

    public void setCalback(IrisViewListner callback) {
         mCallback = callback;
    }

    public void setCropWindow(int x, int y, int width, int height) {
        mCroppedWidth = width;
        mCroppedHeight = height;
        mCroppedX = x;
        mCroppedY = y;
    }

    public void setPreviewSize(int width, int height) {
        mPreviewWidth = width;
        mPreviewHeight = height;
    }

    @Override
    protected void onMeasure( int widthMeasureSpec, int heightMeasureSpec ) {
        int parentWidth = MeasureSpec.getSize(widthMeasureSpec);
        int parentHeight = MeasureSpec.getSize(heightMeasureSpec);
        mScaleRation = (double)parentWidth / mCroppedWidth;

        setMeasuredDimension( (int)(mCroppedWidth * mScaleRation), (int)(mCroppedHeight * mScaleRation));
        Log.v(TAG, "onMeasure parent width=" + parentWidth + "scale=" + mScaleRation);
    }


    @Override
    protected void onLayout( boolean changed, int l, int t, int r, int b ) {
        Log.v(TAG, "onLayout l="+l+" t="+t+" r="+r+" b="+b);
        int croppedX = (int)(mCroppedX * mScaleRation);
        int croppedY = (int)(mCroppedY * mScaleRation);
        int previewWidth = (int)(mPreviewWidth * mScaleRation);
        int previewHeight = (int)(mPreviewHeight * mScaleRation);

        if ( mTextureView != null ) {
            mTextureView.layout(0-croppedX-l, 0-croppedY-t, previewHeight - croppedX-l, previewWidth - croppedY-t);
        }
    }
}


