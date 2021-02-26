/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.sta.device;

import android.content.Context;
import android.util.AttributeSet;
import android.view.TextureView;

/**
 * This class handles the show and hide of the camera preview
 */
public class Camera2Preview extends TextureView {
    // private static final String LOG_TAG = "Camera2Preview";
    private int width = 0;
    private int height = 0;

    /**
     * Constructor for the camera Preview
     *
     * @param context
     *            application context
     */
    public Camera2Preview(Context context) {
        this(context, null);
    }

    /**
     * Constructor for the camera Preview
     *
     * @param context
     *            application context
     * @param attrs
     *            attributes
     */
    public Camera2Preview(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    /**
     * Constructor for the camera Preview
     *
     * @param context
     *            application context
     * @param attrs
     *            attributes
     * @param defStyle
     *            style
     */
    public Camera2Preview(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
    }

    /**
     * set the aspect ratio for the texture
     *
     * @param width
     *            width of the texture
     * @param height
     *            height of the texture
     */
    public void setAspectRatio(int width, int height) {
        if (width < 0 || height < 0) {
            throw new IllegalArgumentException("width ot height less than 0");
        }
        this.width = width;
        this.height = height;
        requestLayout();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);
        int specWidth = MeasureSpec.getSize(widthMeasureSpec);
        int specHeight = MeasureSpec.getSize(heightMeasureSpec);
        if (0 == width || 0 == height) {
            setMeasuredDimension(specWidth, specHeight);
        } else {
            if (specWidth < specHeight * width / height) {
                setMeasuredDimension(specWidth, specWidth * height / width);
            } else {
                setMeasuredDimension(specHeight * width / height, specHeight);
            }
        }
    }
}
