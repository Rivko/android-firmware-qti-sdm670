/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.sta.mainui.preview;

import android.content.Context;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;

import com.qualcomm.qti.sta.mainui.CaptureUI;

/**
 * class for handling button-preview ui
 */
public class ButtonPreviewUI extends CaptureUI {
    private static final String LOG_TAG = "ButtonPreviewUI";

    /**
     * Constructor for the class
     *
     * @param context
     *            application context
     * @param parentView
     *            parent view
     * @param listener
     *            ui listener
     */
    public ButtonPreviewUI(Context context, View parentView, CaptureUIListener listener) {
        super(context, parentView, listener);

        captureButton.setVisibility(View.VISIBLE);
        captureButton.setOnClickListener(new OnClickListener() {
            /**
             * {@inheritDoc}
             */
            @Override
            public void onClick(View v) {
                Log.d(LOG_TAG, "Start Button clicked");
                captureButton.setVisibility(View.GONE);
                roiView.setOnTouchListener(null);
                uiListener.onCaptureStart();
            }
        });
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void parserReady() {
        Log.d(LOG_TAG, "Parser Ready");
        captureButton.setVisibility(View.VISIBLE);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void pause(int reason) {
        Log.d(LOG_TAG, "pause");
        hide();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void hide() {
        Log.d(LOG_TAG, "Hide ui");
        captureButton.setVisibility(View.GONE);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void resume() {
        Log.d(LOG_TAG, "Resume");
        captureButton.setVisibility(View.VISIBLE);
        roiView.setOnTouchListener(this);
    }
}
