/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.sta.mainui.gallery;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.ColorFilter;
import android.graphics.Paint;
import android.graphics.Rect;
import android.graphics.drawable.Drawable;

import com.qualcomm.qti.sta.data.CanvasView;

public class GalleryImageDrawable extends Drawable {
    //private static final String LOG_TAG = "GalleryImageDrawable";

    private final Paint paint;
    private final Bitmap image;
    private final CanvasView canvasView;
    private final int intrinsicWidth;
    private final int intrinsicHeight;

    /**
     * Constructor for the class
     *
     * @param image
     *            image to be shown
     * @param canvasView
     *            canvas view which takes care of translations and zoom
     */
    public GalleryImageDrawable(Bitmap image, CanvasView canvasView) {
        this.image = image;
        this.canvasView = canvasView;
        intrinsicWidth = image.getWidth();
        intrinsicHeight = image.getHeight();

        paint = new Paint(Paint.ANTI_ALIAS_FLAG);
    }
    /**
     * {@inheritDoc}
     */
    @Override
    public void onBoundsChange(Rect bounds) {
        canvasView.setDisplaySize(bounds.width(), bounds.height());
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void draw(Canvas canvas) {
        int saveLevel = canvas.save();
        canvas.scale(canvasView.getScale(), canvasView.getScale());
        canvas.drawBitmap(image, canvasView.getTranslateX(), canvasView.getTranslateY(), paint);
        canvas.restoreToCount(saveLevel);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public int getOpacity() {
        return paint.getAlpha();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public int getIntrinsicWidth() {
        return intrinsicWidth;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public int getIntrinsicHeight() {
        return intrinsicHeight;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void setAlpha(int alpha) {
        paint.setAlpha(alpha);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void setColorFilter(ColorFilter filter) {
        paint.setColorFilter(filter);
    }
}
