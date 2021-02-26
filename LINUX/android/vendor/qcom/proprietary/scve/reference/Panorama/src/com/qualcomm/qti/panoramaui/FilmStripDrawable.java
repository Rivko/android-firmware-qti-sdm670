/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.panoramaui;

import java.util.ArrayList;
import java.util.ListIterator;

import com.qualcomm.qti.panoramaui.data.CaptureDirection;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.ColorFilter;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.PixelFormat;
import android.graphics.Rect;
import android.graphics.Region.Op;
import android.graphics.drawable.Drawable;
import android.util.Log;

/**
 * This {@link Drawable} implements a filmstrip-like effect for combining multiple independent
 * thumbnails of a Panorama capture
 */
public class FilmStripDrawable extends Drawable {
    private final Paint paint = new Paint();
    private final ArrayList<Thumbnail> thumbnails = new ArrayList<Thumbnail>();
    private final Matrix matrix = new Matrix();
    private CaptureDirection captureDirection = CaptureDirection.NOT_ESTABLISHED;
    private int frameWidth;
    private int frameHeight;
    private float coordinateScale;

    /**
     * Construct a new {@link FilmStripDrawable}
     */
    public FilmStripDrawable() {
        paint.setFilterBitmap(true);
    }

    /**
     * Add a new thumbnail to the filmstrip
     * @param bitmap {@link Bitmap} containing the thumbnail
     * @param x X coordinate, relative to an arbitrary origin. This coordinate uses the units
     *      defined by {@link #setFrameSize(int, int)}, not the units of the thumbnail bitmap.
     * @param y Y coordinate, relative to an arbitrary origin. This coordinate uses the units
     *      defined by {@link #setFrameSize(int, int)}, not the units of the thumbnail bitmap.
     * @throws NullPointerException if bitmap is null
     */
    public void addThumbnail(Bitmap bitmap, int x, int y) {
        if (bitmap == null) {
            throw new NullPointerException("bitmap may not be null");
        }

        if (bitmap.isMutable()) {
            bitmap = Bitmap.createBitmap(bitmap);
        }

        thumbnails.add(new Thumbnail(bitmap, x, y));
        if (thumbnails.size() == 1) {
            updateMatrix();
        } else {
            invalidateSelf();
        }
    }

    /**
     * Clear all filmstrip thumbnails
     */
    public void clearThumbnails() {
        thumbnails.clear();
        updateMatrix();
    }

    public void copy1stThumbnail(FilmStripDrawable fromDrawable, CaptureDirection direction) {
        if (fromDrawable.thumbnails != null && fromDrawable.thumbnails.size() == 1) {
            captureDirection = direction;
            thumbnails.add(fromDrawable.thumbnails.get(0));
            if (thumbnails.size() == 1) {
                updateMatrix();
            } else {
                invalidateSelf();
            }
        }
    }
    /**
     * Set the size of a frame (not the size of the thumbnails comprising the filmstrip). This is
     * used to translate the position provided to {@link #addThumbnail(Bitmap, int, int)} into
     * thumbnail size units.
     * @param width Width, in px, of the image frames
     * @param height Height, in px, of the image frames
     * @throws IllegalArgumentException if width or height are < 0
     */
    public void setFrameSize(int width, int height) {
        if (width < 0 || height < 0) {
            throw new IllegalArgumentException("width and height must be non-negative");
        }

        frameWidth = width;
        frameHeight = height;
        updateMatrix();
    }

    /**
     * Set the direction of capture. This controls how the filmstrip is rendered.
     * @param captureDirection the direction of capture
     * @throws NullPointerException if captureDirection is null
     */
    public void setCaptureDirection(CaptureDirection captureDirection) {
        if (captureDirection == null) {
            throw new NullPointerException("captureDirection may not be null");
        }

        this.captureDirection = captureDirection;
        updateMatrix();
    }

    @Override
    public void draw(Canvas canvas) {
        if (thumbnails.isEmpty()) {
            return;
        }

        // Clip to Drawable bounds, and transform to match thumbnail coordinate space
        int saveCount = canvas.save();
        canvas.clipRect(getBounds());
        canvas.concat(matrix);

        ListIterator<Thumbnail> it = thumbnails.listIterator(thumbnails.size());
        while (it.hasPrevious()) {
            Thumbnail t = it.previous();
            final float scaledX = t.x * coordinateScale;
            final float scaledY = t.y * coordinateScale;
            canvas.drawBitmap(t.bitmap, scaledX, scaledY, paint);
            canvas.clipRect(scaledX, scaledY, scaledX + t.bitmap.getWidth(),
                    scaledY + t.bitmap.getHeight(), Op.DIFFERENCE);
        }

        canvas.restoreToCount(saveCount);
    }

    @Override
    public void setAlpha(int alpha) {
        paint.setAlpha(alpha);
    }

    @Override
    public int getAlpha() {
        return paint.getAlpha();
    }

    @Override
    public void setColorFilter(ColorFilter cf) {
        paint.setColorFilter(cf);
    }

    @Override
    public ColorFilter getColorFilter() {
        return paint.getColorFilter();
    }

    @Override
    public int getOpacity() {
        return PixelFormat.TRANSLUCENT;
    }

    @Override
    public void setDither(boolean dither) {
        paint.setDither(dither);
    }

    @Override
    public void setFilterBitmap(boolean filter) {
        paint.setFilterBitmap(filter);
    }

    @Override
    public Drawable mutate() {
        throw new UnsupportedOperationException("This is an unshared drawable, so mutate is not " +
                "supported");
    }

    @Override
    protected void onBoundsChange(Rect bounds) {
        super.onBoundsChange(bounds);
        updateMatrix();
    }

    private void updateMatrix() {
        if (thumbnails.isEmpty() || frameWidth == 0 || frameHeight == 0) {
            matrix.reset();
            invalidateSelf();
            return;
        }

        Thumbnail reference = thumbnails.get(0);
        final Rect bounds = getBounds();

        coordinateScale = (float)reference.bitmap.getWidth() / frameWidth;
        matrix.setTranslate(-reference.x * coordinateScale, -reference.y * coordinateScale);

        final float scale;
        switch (captureDirection) {
        case LEFT:
            scale = (float)bounds.height() / reference.bitmap.getHeight();
            matrix.postTranslate(bounds.width() / scale - reference.bitmap.getWidth(), 0);
            matrix.postScale(scale, scale);
            break;

        case RIGHT:
            scale = (float)bounds.height() / reference.bitmap.getHeight();
            matrix.postScale(scale, scale);
            break;

        case UP:
            scale = (float)bounds.width() / reference.bitmap.getWidth();
            matrix.postTranslate(0, bounds.height() / scale - reference.bitmap.getHeight());
            matrix.postScale(scale, scale);
            break;

        case DOWN:
            scale = (float)bounds.width() / reference.bitmap.getWidth();
            matrix.postScale(scale, scale);
            break;

        case NOT_ESTABLISHED:
        default:
            scale = Math.min((float)bounds.width() / reference.bitmap.getWidth(),
                    (float)bounds.height() / reference.bitmap.getHeight());
            matrix.postTranslate((bounds.width() / scale - reference.bitmap.getWidth()) / 2f,
                    (bounds.height() / scale - reference.bitmap.getHeight()) / 2f);
            matrix.postScale(scale, scale);
            break;
        }

        invalidateSelf();
    }

    private static class Thumbnail {
        private final Bitmap bitmap;
        private final int x;
        private final int y;

        private Thumbnail(Bitmap bitmap, int x, int y) {
            this.bitmap = bitmap;
            this.x = x;
            this.y = y;
        }
    }
}
