/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.sta.mainui;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.PointF;
import android.graphics.RectF;
import android.util.DisplayMetrics;
import android.view.Display;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;
import android.widget.ImageView;

import com.qualcomm.qti.sta.R;
import com.qualcomm.qti.sta.data.ApplicationMessage;
import com.qualcomm.qti.sta.data.Utils;
import com.qualcomm.qti.sta.data.ViewSize;
import com.qualcomm.qti.sta.device.CoordinateSpace;
import com.qualcomm.qti.sta.mainui.CameraActivity.CaptureType;
import com.qualcomm.qti.sta.mainui.preview.AutoPreviewUI;
import com.qualcomm.qti.sta.mainui.preview.ButtonPreviewUI;
import com.qualcomm.qti.sta.mainui.snapshot.SnapshotUI;

/**
 * Class for handling capture ui
 */
public abstract class CaptureUI implements OnTouchListener {
    protected final CaptureUIListener uiListener;
    protected final ROIDrawable roiDrawable;
    protected final View roiView;
    // snapshot ui
    protected final View captureButton;
    protected final View snapshotView;

    /**
     * Enum to track the touch state
     */
    private enum TouchState {
        NONE, DRAG, ZOOM
    }

    private float minimumPinchZoom = 0;
    private TouchState touchState = TouchState.NONE;
    private PointF startLocation = new PointF();
    private PointF pinchMidPoint = new PointF();
    private float previousPinchDistance = 1.0f;
    private PointF moveFromLocation = new PointF();

    /**
     * interface for capture ui listener notificatons
     */
    public interface CaptureUIListener {
        /**
         * Notification on capture start
         */
        public void onCaptureStart();

        /**
         * Notification on ROI changed
         */
        public void onRoiChanged();

        /**
         * notification on application message
         *
         * @param message
         *            application message
         */
        public void onCaptureMessage(ApplicationMessage message);
    }

    /**
     * Constructor for the class
     *
     * @param context
     *            application context
     * @param parentView
     *            parent view
     * @param listener
     *            capture listener
     */
    protected CaptureUI(Context context, View parentView, CaptureUIListener listener) {
        uiListener = listener;

        minimumPinchZoom = context.getResources().getDimension(R.dimen.min_pinch_limit);
        roiView = parentView.findViewById(R.id.roi_view);
        roiDrawable = new ROIDrawable(context.getResources());
        ((ImageView) roiView).setImageDrawable(roiDrawable);

        snapshotView = parentView.findViewById(R.id.snapshot);
        captureButton = parentView.findViewById(R.id.btn_start_capturing);

        snapshotView.setVisibility(View.GONE);
        captureButton.setVisibility(View.GONE);
    }

    /**
     * Method to get the new instance of the Capture ui based on capture type
     *
     * @param captureType
     *            capture type
     * @param context
     *            application context
     * @param parentView
     *            parent view
     * @param listener
     *            capture listener
     * @return capture ui instance
     */
    public static CaptureUI getNewInstance(CaptureType captureType, Context context,
            View parentView, CaptureUIListener listener, Display display) {
        if (captureType == CaptureType.SNAPSHOT) {
            DisplayMetrics metrics = new DisplayMetrics();
            display.getMetrics(metrics);
            return new SnapshotUI(context, parentView, listener, metrics.widthPixels, metrics.heightPixels);

        } else if (captureType == CaptureType.AUTO_PREVIEW) {
            return new AutoPreviewUI(context, parentView, listener);

        } else if (captureType == CaptureType.BUTTON_PREVIEW) {
            return new ButtonPreviewUI(context, parentView, listener);
        }
        return null;
    }

    /**
     * enables roi visibility and touch-ability
     *
     * @param enableVisibility
     *            true if roi needs to be visible, false otherwise
     * @param enableTouch
     *            true if roi needs to accept touch events, false otherwise
     */
    public void enableRoi(boolean enableVisibility, boolean enableTouch) {
        int visibility = (enableVisibility) ? View.VISIBLE : View.GONE;
        OnTouchListener listener = (enableTouch) ? this : null;

        roiView.setVisibility(visibility);
        roiView.setOnTouchListener(listener);
    }

    /**
     * Gets the visible rect from preview
     *
     * @param surfaceSize
     *            surface size
     * @param surfaceViewRatio
     *            Surface view ratio
     * @return visible rect from preview
     */
    public RectF getPreviewRect(ViewSize surfaceSize, float surfaceViewRatio) {
        int width = roiDrawable.getBounds().width();
        int height = roiDrawable.getBounds().height();
        float offsetX = (width - surfaceSize.width) / 2;
        float offsetY = (height - surfaceSize.height) / 2;

        RectF roiRect = roiDrawable.getEnclosedArea();
        float previewRectLeft = roiRect.left - offsetX;
        if (previewRectLeft < 0) {
            previewRectLeft = 0;
        }
        float previewRectTop = roiRect.top - offsetY;
        if (previewRectTop < 0) {
            previewRectTop = 0;
        }
        float previewRectRight = roiRect.right - offsetX;
        if (previewRectRight > surfaceSize.width) {
            previewRectRight = surfaceSize.width;
        }
        float previewRectBottom = roiRect.bottom - offsetY;
        if (previewRectBottom > surfaceSize.height) {
            previewRectBottom = surfaceSize.height;
        }

        return new RectF(previewRectLeft * surfaceViewRatio, previewRectTop * surfaceViewRatio,
                previewRectRight * surfaceViewRatio, previewRectBottom * surfaceViewRatio);
    }

    /**
     * Gets the cropped ROI image from snapshot
     *
     * @param snapshot
     *            snapshot bitmap
     * @param surfaceSize
     *            surface size
     * @param surfaceViewRatio
     *            surface view ratio
     * @param previewSpace
     *            preview space
     * @param pictureSpace
     *            picture space
     * @return ROI bitmap
     */
    public Bitmap getRoiImage(Bitmap snapshot, ViewSize surfaceSize, float surfaceViewRatio,
            CoordinateSpace previewSpace, CoordinateSpace pictureSpace) {
        RectF previewRect = getPreviewRect(surfaceSize, surfaceViewRatio);
        return roiDrawable.getCroppedImage(snapshot, previewRect, previewSpace, pictureSpace);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean onTouch(View view, MotionEvent event) {
        switch (event.getAction() & MotionEvent.ACTION_MASK) {
        case MotionEvent.ACTION_DOWN: {
            startLocation.set(event.getX(), event.getY());
            moveFromLocation.set(event.getX(), event.getY());
            touchState = TouchState.DRAG;
            break;
        }

        case MotionEvent.ACTION_POINTER_DOWN:
            previousPinchDistance = Utils.getSpacing(event);
            if (previousPinchDistance > minimumPinchZoom) {
                Utils.getMidPoint(pinchMidPoint, event);
                touchState = TouchState.ZOOM;
                roiDrawable.lockView();
            }
            break;

        case MotionEvent.ACTION_UP:
        case MotionEvent.ACTION_POINTER_UP:
            if (touchState == TouchState.ZOOM || touchState == TouchState.DRAG) {
                roiDrawable.unlockView();
            }
            touchState = TouchState.NONE;
            break;

        case MotionEvent.ACTION_MOVE:
            if (touchState == TouchState.DRAG) {
                // move the roi
                roiDrawable.drag(startLocation, moveFromLocation, event.getX(), event.getY());
                moveFromLocation.set(event.getX(), event.getY());
                uiListener.onRoiChanged();

            } else if (touchState == TouchState.ZOOM) {
                float newDistance = Utils.getSpacing(event);
                if (newDistance > minimumPinchZoom) {
                    float scale = newDistance / previousPinchDistance;
                    roiDrawable.zoomBy(scale);
                    uiListener.onRoiChanged();
                }
            }

            break;
        }
        return true;
    }

    /**
     * notification on parser ready
     */
    protected abstract void parserReady();

    /**
     * notification on capture resume
     */
    protected abstract void resume();

    /**
     * notification on capture pause
     *
     * @param reason
     *            pause reason
     */
    protected abstract void pause(int reason);

    /**
     * notification to hide the capture ui
     */
    protected abstract void hide();
}
