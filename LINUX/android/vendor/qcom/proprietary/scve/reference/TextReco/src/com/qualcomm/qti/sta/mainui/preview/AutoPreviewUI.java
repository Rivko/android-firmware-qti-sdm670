/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.sta.mainui.preview;

import android.content.Context;
import android.util.Log;
import android.view.View;

import com.qualcomm.qti.sta.data.ApplicationMessage;
import com.qualcomm.qti.sta.mainui.CaptureUI;

/**
 * class for handling auto-preview ui
 */
public class AutoPreviewUI extends CaptureUI {
    private static final String LOG_TAG = "AutoPreviewUI";

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
    public AutoPreviewUI(Context context, View parentView, CaptureUIListener listener) {
        super(context, parentView, listener);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void parserReady() {
        Log.d(LOG_TAG, "Parser Ready");
        scheduleCaptureStart();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void pause(int reason) {
        Log.d(LOG_TAG, "pause");
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void hide() {
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void resume() {
        scheduleCaptureStart();
    }

    private void scheduleCaptureStart() {
        Log.d(LOG_TAG, "Schedule capture callback");
        ApplicationMessage message = ApplicationMessage.HOLD_STEADY;
        message.setFinishCallback(new Runnable() {
            /**
             * {@inheritDoc}
             */
            @Override
            public void run() {
                uiListener.onCaptureStart();
            }
        });
        uiListener.onCaptureMessage(message);
    }
}
