/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies, Inc. Confidential and Proprietary.
 */
package com.qualcomm.qti.sta.resultui;

import java.io.File;
import java.util.ArrayList;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.PointF;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;
import android.widget.ImageView;

import com.qualcomm.qti.sta.data.CanvasView;
import com.qualcomm.qti.sta.data.CanvasView.CanvasEventListener;
import com.qualcomm.qti.sta.data.CanvasView.ViewType;
import com.qualcomm.qti.sta.data.RecognizedData;
import com.qualcomm.qti.sta.data.Utils;

/**
 * class to handle result image interactions and display
 */
public class ResultImageHandler implements OnTouchListener, CanvasEventListener {
    private static final String LOG_TAG = "ResultImageHandler";

    private static final int MAX_SCALE = 2;
    private ResultImageDrawable imageDrawable = null;
    private CanvasView imageCanvasView = null;
    private final ImageView imageView;

    /**
     * Enum to track the touch state
     */
    protected enum TouchState {
        NONE, DRAG, ZOOM
    }

    protected float minimumPinchZoom = 0;
    protected TouchState touchState = TouchState.NONE;
    protected PointF startLocation = new PointF();
    protected PointF pinchMidPoint = new PointF();
    protected float previousPinchDistance = 1.0f;

    /**
     * Constructor for the class
     *
     * @param context
     *            caller activity context
     * @param view
     *            image view
     * @param bitmap
     *            image for image view
     * @param dataList
     *            parsed out data list
     * @param dataId
     *            current data id
     * @param displayWidth
     *            display width
     * @param displayHeight
     *            display height
     */
    public ResultImageHandler(Context context, ImageView view, ArrayList<RecognizedData> dataList,
            int dataId, int displayWidth, int displayHeight) {
        String finalImage = Utils.getLocalImageFile(context);
        BitmapFactory.Options mutableOptions = new BitmapFactory.Options();
        mutableOptions.inMutable = true;

        Bitmap roiBitmap = BitmapFactory.decodeFile(finalImage, mutableOptions);
        int maxWidth = displayWidth * MAX_SCALE;
        int maxHeight = displayHeight * MAX_SCALE;
        float imageScale = 1.0f;
        if (roiBitmap.getWidth() > maxWidth || roiBitmap.getHeight() > maxHeight) {
            float horizontalScale = (float) maxWidth / roiBitmap.getWidth();
            float verticalScale = (float) maxHeight / roiBitmap.getHeight();
            imageScale = Math.min(horizontalScale, verticalScale);
            int newWidth = (int) (roiBitmap.getWidth() * imageScale);
            int newHeight = (int) (roiBitmap.getHeight() * imageScale);
            Log.d(LOG_TAG,
                    "image scaled down: " + roiBitmap.getWidth() + "X" + roiBitmap.getHeight()
                            + " => " + newWidth + "X" + newHeight);
            Bitmap image = Bitmap.createScaledBitmap(roiBitmap, newWidth, newHeight, false);
            roiBitmap.recycle();
            roiBitmap = image;
        }

        imageCanvasView = new CanvasView((float) roiBitmap.getWidth(),
                (float) roiBitmap.getHeight(), this, ViewType.TOP);
        imageDrawable = new ResultImageDrawable(context, roiBitmap, dataList, dataId,
                imageCanvasView, imageScale);
        imageView = view;

        view.setImageDrawable(imageDrawable);
    }

    private void getMidPoint(PointF midPoint, MotionEvent event) {
        float xMid = (event.getX(0) + event.getX(1)) / 2;
        float yMid = (event.getY(0) + event.getY(1)) / 2;
        midPoint.set(xMid, yMid);
    }

    private float getSpacing(MotionEvent event) {
        float xValue = event.getX(0) - event.getX(1);
        float yValue = event.getY(0) - event.getY(1);
        return (float) Math.hypot(xValue, yValue);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean onTouch(View view, MotionEvent event) {
        ImageView imageView = (ImageView) view;

        switch (event.getAction() & MotionEvent.ACTION_MASK) {
        case MotionEvent.ACTION_DOWN: {
            startLocation.set(event.getX(), event.getY());
            touchState = TouchState.DRAG;
            break;
        }

        case MotionEvent.ACTION_POINTER_DOWN:
            previousPinchDistance = getSpacing(event);
            if (previousPinchDistance > minimumPinchZoom) {
                getMidPoint(pinchMidPoint, event);
                imageCanvasView.lockView();
                touchState = TouchState.ZOOM;
            }
            break;

        case MotionEvent.ACTION_UP:
        case MotionEvent.ACTION_POINTER_UP:
            if (touchState == TouchState.ZOOM) {
                imageCanvasView.unlockView();
            }
            touchState = TouchState.NONE;
            break;

        case MotionEvent.ACTION_MOVE:
            boolean isMove = false;
            if (touchState == TouchState.DRAG) {
                isMove = imageCanvasView.translateBy(event.getX() - startLocation.x, event.getY()
                        - startLocation.y);
                startLocation.set(event.getX(), event.getY());

            } else if (touchState == TouchState.ZOOM) {
                float newDistance = getSpacing(event);
                if (newDistance > minimumPinchZoom) {
                    float scale = newDistance / previousPinchDistance;
                    isMove = imageCanvasView.scaleBy(scale, pinchMidPoint.x, pinchMidPoint.y);
                }
            }
            if (isMove) {
                imageView.invalidate();
            }
            break;
        }
        return true;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onScaleDone() {
        imageDrawable.invalidateSelf();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onScaleAnimationUpdate() {
        imageDrawable.invalidateSelf();
    }

    /**
     * gets the image current y position
     *
     * @return current y position
     */
    public float getImageStartY() {
        return imageDrawable.getImageStartY();
    }

    /**
     * sets the new image start Y position
     *
     * @param y
     *            image y position
     */
    public void setImageY(float y) {
        imageDrawable.setImageY(y);
    }

    /**
     * Enables the touch event on image
     */
    public void enableTouchEvents() {
        imageView.setOnTouchListener(this);
        imageDrawable.enableTouchEvents();
    }
}
