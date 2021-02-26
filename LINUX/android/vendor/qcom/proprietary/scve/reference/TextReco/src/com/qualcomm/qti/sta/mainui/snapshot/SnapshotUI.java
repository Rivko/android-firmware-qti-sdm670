/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.sta.mainui.snapshot;

import android.content.Context;
import android.graphics.Bitmap;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.ImageView;

import com.qualcomm.qti.sta.mainui.CaptureUI;

/**
 * class for handling snapshot ui
 */
public class SnapshotUI extends CaptureUI {
    private static final String LOG_TAG = "SnapshotUI";
    private SnapshotDrawable imageDrawable = null;
    private final int displayWidth;
    private final int displayHeight;

    /**
     * Constructor for the class
     *
     * @param context
     *            application context
     * @param parentView
     *            parent view
     * @param listener
     *            ui listener
     * @param displayWidth
     *            display width
     * @param displayHeight
     *            display height
     */
    public SnapshotUI(Context context, View parentView, CaptureUIListener listener,
            int displayWidth, int displayHeight) {
        super(context, parentView, listener);

        this.displayWidth = displayWidth;
        this.displayHeight = displayHeight;

        snapshotView.setVisibility(View.VISIBLE);
        captureButton.setVisibility(View.VISIBLE);
        captureButton.setOnClickListener(new OnClickListener() {
            /**
             * {@inheritDoc}
             */
            @Override
            public void onClick(View v) {
                Log.d(LOG_TAG, "Start Button clicked");
                captureButton.setVisibility(View.GONE);
                roiDrawable.enableSnapshotUI(true);
                roiView.setOnTouchListener(null);
                uiListener.onCaptureStart();
            }
        });
    }

    /**
     * shows the captured image
     *
     * @param roiBitmap
     *            captured bitmap image
     */
    public void showCapturedImage(Bitmap roiBitmap) {
        Log.d(LOG_TAG, "Show captured image");
        snapshotView.setVisibility(View.VISIBLE);

        Bitmap image = roiBitmap;
        if (roiBitmap.getWidth() > displayWidth || roiBitmap.getHeight() > displayHeight) {
            float horizontalScale = (float) displayWidth / roiBitmap.getWidth();
            float verticalScale = (float) displayHeight / roiBitmap.getHeight();
            float scale = Math.min(horizontalScale, verticalScale);
            int newWidth = (int) (roiBitmap.getWidth() * scale);
            int newHeight = (int) (roiBitmap.getHeight() * scale);
            Log.d(LOG_TAG, "image scaled down: " + roiBitmap.getWidth() + "X" + roiBitmap.getHeight() + " => "
                    + newWidth + "X" + newHeight);
            image = Bitmap.createScaledBitmap(roiBitmap, newWidth, newHeight, false);
        }
        imageDrawable = new SnapshotDrawable(image);
        ((ImageView) snapshotView).setImageDrawable(imageDrawable);
        imageDrawable.invalidateSelf();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void parserReady() {
        Log.d(LOG_TAG, "Parser Ready");
        roiDrawable.enableSnapshotUI(false);
        captureButton.setVisibility(View.VISIBLE);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void pause(int reason) {
        Log.d(LOG_TAG, "pause");
        roiDrawable.enableSnapshotUI(false);
        hide();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void hide() {
        Log.d(LOG_TAG, "Hide ui");
        ((ImageView) snapshotView).setImageDrawable(null);
        snapshotView.setVisibility(View.GONE);
        captureButton.setVisibility(View.GONE);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void resume() {
        Log.d(LOG_TAG, "Resume");
        captureButton.setVisibility(View.VISIBLE);
    }
}
