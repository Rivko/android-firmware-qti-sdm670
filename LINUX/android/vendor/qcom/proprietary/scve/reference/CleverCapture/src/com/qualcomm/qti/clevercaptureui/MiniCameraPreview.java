/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.clevercaptureui;

import java.text.AttributedCharacterIterator.Attribute;

import android.content.Context;
import android.os.Handler;
import android.os.Looper;
import android.util.AttributeSet;
import android.widget.ImageView;

import com.qualcomm.qti.clevercaptureui.CameraPreviewView.OutputFrame;

/**
 * A custom {@link ImageView} which will display a thumbnail size preview on each
 * {@link CameraPreviewView.OnFrameAvailableListener} callback
 *
 */
public class MiniCameraPreview extends ImageView implements CameraPreviewView.OnFrameAvailableListener {

    private Handler handler;

    /**
     * Constructor
     * @param context {@link Context
     * }
     */
    public MiniCameraPreview(Context context) {
        this(context, null, 0);
    }

    /**
     * Constructor
     * @param context {@link Context}
     * @param attrs {@link AttributeSet}
     */
    public MiniCameraPreview(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    /**
     * Constructor
     * @param context {@link Context}
     * @param attrs {@link Attribute}
     * @param defStyle int
     */
    public MiniCameraPreview(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);

        handler = new Handler(Looper.getMainLooper());
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean onFrameAvailable(final OutputFrame frame) {

        handler.post(new Runnable() {
            @Override
            public void run() {
                setImageBitmap(frame.thumb);
                frame.sem.release();
            }
        });

        return true;
    }
}
