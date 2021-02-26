/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.sta.mainui.snapshot;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.ColorFilter;
import android.graphics.Paint;
import android.graphics.Rect;
import android.graphics.drawable.Drawable;

/**
 * Drawable to show the background with OCR bitmap
 */
public class SnapshotDrawable extends Drawable {
    //private static final String LOG_TAG = "SnapshotDrawable";

    private float atX = 0;
    private float atY = 0;
    private float scalefactor;

    private final Bitmap snapshot;
    private final Paint imagePaint;

    private boolean haveOpacity = false;
    private int opacity = 0;

    /**
     * Constructor for the class
     *
     * @param snapshot
     *            snapshot from ROI which needs to be shown as background image
     */
    public SnapshotDrawable(Bitmap snapshot) {
        this.snapshot = snapshot;

        imagePaint = new Paint(Paint.ANTI_ALIAS_FLAG);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onBoundsChange(Rect bounds) {
        float horizontalScale = (float) bounds.width() / snapshot.getWidth();
        float verticalScale = (float) bounds.height() / snapshot.getHeight();
        scalefactor = Math.min(horizontalScale, verticalScale);

        atX = (bounds.width() - snapshot.getWidth() * scalefactor) / 2;
        atY = (bounds.height() - snapshot.getHeight() * scalefactor) / 2;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void draw(Canvas canvas) {
        int saveCount = canvas.save();
        canvas.translate(atX, atY);
        canvas.scale(scalefactor, scalefactor);
        canvas.drawBitmap(snapshot, 0, 0, imagePaint);
        canvas.restoreToCount(saveCount);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void setAlpha(int alpha) {
        imagePaint.setAlpha(alpha);

    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void setColorFilter(ColorFilter filter) {
        imagePaint.setColorFilter(filter);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public int getOpacity() {
        if (haveOpacity) {
            return opacity;
        }
        opacity = Drawable.resolveOpacity(opacity, imagePaint.getAlpha());

        haveOpacity = true;
        return opacity;
    }
}
