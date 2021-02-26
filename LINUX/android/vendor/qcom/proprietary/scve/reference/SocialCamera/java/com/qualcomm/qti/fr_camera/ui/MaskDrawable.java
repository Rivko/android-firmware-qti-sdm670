/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.fr_camera.ui;

import android.content.res.ColorStateList;
import android.content.res.Resources;
import android.graphics.Canvas;
import android.graphics.ColorFilter;
import android.graphics.Outline;
import android.graphics.Paint;
import android.graphics.PorterDuff;
import android.graphics.PorterDuffXfermode;
import android.graphics.Rect;
import android.graphics.Region;
import android.graphics.drawable.Drawable;

import java.util.Objects;

/**
 * This {@link Drawable} will make one Drawable with the alpha channel of another
 */
public class MaskDrawable extends Drawable {
    private final Drawable mask;
    private final Drawable image;
    private final Paint imagePaint;
    private final Paint maskPaint;

    private boolean isAutoMirrored;

    /**
     * Construct a new {@link MaskDrawable}
     * @param mask the {@link Drawable} whose alpha channel to use as a mask for image
     * @param image the {@link Drawable} to be masked
     * @throws NullPointerException if mask or image are null
     */
    public MaskDrawable(Drawable mask, Drawable image) {
        Objects.requireNonNull(mask, "mask may not be null");
        Objects.requireNonNull(image, "image may not be null");

        this.mask = mask;
        this.image = image;

        mask.setCallback(childCallback);
        image.setCallback(childCallback);

        maskPaint = new Paint();
        maskPaint.setXfermode(new PorterDuffXfermode(PorterDuff.Mode.DST_IN));

        imagePaint = new Paint();
    }

    @Override
    public void draw(Canvas canvas) {
        Rect bounds = getBounds();
        int saveCount = canvas.saveLayer(bounds.left, bounds.top, bounds.right, bounds.bottom,
                imagePaint);
        image.draw(canvas);

        canvas.saveLayer(bounds.left, bounds.top, bounds.right, bounds.bottom, maskPaint);
        mask.draw(canvas);

        canvas.restoreToCount(saveCount);
    }

    @Override
    public void setAlpha(int alpha) {
        imagePaint.setAlpha(alpha);
    }

    @Override
    public int getAlpha() {
        return imagePaint.getAlpha();
    }

    @Override
    public void setAutoMirrored(boolean mirrored) {
        isAutoMirrored = mirrored;
        mask.setAutoMirrored(mirrored);
        image.setAutoMirrored(mirrored);
    }

    @Override
    public boolean isAutoMirrored() {
        return isAutoMirrored;
    }

    @Override
    public void setColorFilter(ColorFilter cf) {
        imagePaint.setColorFilter(cf);
    }

    @Override
    public ColorFilter getColorFilter() {
        return imagePaint.getColorFilter();
    }

    @Override
    @SuppressWarnings("deprecation")
    public void setDither(boolean dither) {
        // N.B. mask and image are painted to a full-color layer, so no effect of setting dither on
        // these drawables
        imagePaint.setDither(dither);
    }

    @Override
    public void setFilterBitmap(boolean filter) {
        mask.setFilterBitmap(filter);
        image.setFilterBitmap(filter);
    }

    @Override
    public void setHotspot(float x, float y) {
        // N.B. don't apply hotspot to mask; only the image Drawable may be modified by the hotspot
        image.setHotspot(x, y);
    }

    @Override
    public void setHotspotBounds(int left, int top, int right, int bottom) {
        // N.B. don't apply hotspot to mask; only the image Drawable may be modified by the hotspot
        image.setHotspotBounds(left, top, right, bottom);
    }

    @Override
    public int getIntrinsicWidth() {
        return mask.getIntrinsicWidth();
    }

    @Override
    public int getIntrinsicHeight() {
        return mask.getIntrinsicHeight();
    }

    @Override
    public int getMinimumWidth() {
        return mask.getMinimumWidth();
    }

    @Override
    public int getMinimumHeight() {
        return mask.getMinimumHeight();
    }

    @Override
    public int getOpacity() {
        return resolveOpacity(mask.getOpacity(), image.getOpacity());
    }

    @Override
    @SuppressWarnings("NullableProblems")
    public void getOutline(Outline outline) {
        mask.getOutline(outline);
    }

    @Override
    public void setTintList(ColorStateList tint) {
        // N.B. Only tint image; mask should not be tinted, it is only a source of alpha
        image.setTintList(tint);
    }

    @Override
    @SuppressWarnings("NullableProblems")
    public void setTintMode(PorterDuff.Mode tintMode) {
        // N.B. Only tint image; mask should not be tinted, it is only a source of alpha
        image.setTintMode(tintMode);
    }

    @Override
    public boolean isStateful() {
        return (mask.isStateful() || image.isStateful());
    }

    @Override
    public void jumpToCurrentState() {
        if (mask.isStateful()) {
            mask.jumpToCurrentState();
        }

        if (image.isStateful()) {
            image.jumpToCurrentState();
        }
    }

    @Override
    public void applyTheme(Resources.Theme t) {
        if (mask.canApplyTheme()) {
            mask.applyTheme(t);
        }

        if (image.canApplyTheme()) {
            image.applyTheme(t);
        }
    }

    @Override
    public boolean canApplyTheme() {
        return mask.canApplyTheme() || image.canApplyTheme();
    }

    @Override
    public Region getTransparentRegion() {
        // N.B. This is a safe subset of the total transparent region. It's also possible that
        // image has a transparent region in the mask's non-transparent region, but calculating
        // it requires intersecting regions, which is probably more expensive to calculate than is
        // saved by obtaining it.
        return mask.getTransparentRegion();
    }

    /**
     * {@link MaskDrawable} does not support {@link Drawable.ConstantState}. As such, it is not
     * cloneable, and does not share state with any other {@link MaskDrawable}.
     * @return null
     */
    @Override
    public ConstantState getConstantState() {
        return null;
    }

    @Override
    public Drawable mutate() {
        // N.B. Since MaskDrawable does not support ConstantState, all MaskDrawables are
        // automatically mutable
        return super.mutate();
    }

    @Override
    public int getChangingConfigurations() {
        return super.getChangingConfigurations() | mask.getChangingConfigurations() |
                image.getChangingConfigurations();
    }

    @Override
    protected boolean onStateChange(int[] state) {
        // N.B. Note use of non-conditional-or operator
        return ((mask.isStateful() && mask.setState(state)) |
                (image.isStateful() && image.setState(state)));
    }

    @Override
    protected boolean onLevelChange(int level) {
        // N.B. Note use of non-conditional-or operator
        return (mask.setLevel(level) | image.setLevel(level));
    }

    @Override
    protected void onBoundsChange(Rect bounds) {
        mask.setBounds(bounds);

        // Adjust the bounds of image to maintain aspect ratio and completely fill our bounds. Note
        // that draw() will take care of clipping to bounds.
        final int width = bounds.width();
        final int height = bounds.height();
        int imageWidth = image.getIntrinsicWidth();
        int imageHeight = image.getIntrinsicHeight();
        if (imageWidth <= 0 || imageHeight <= 0) {
            imageWidth = width;
            imageHeight = height;
        } else if (imageWidth * height >= imageHeight * width) {
            // Image is wider than mask
            imageWidth = (int)(((float)imageWidth / imageHeight) * height);
            imageHeight = height;
        } else {
            // Image is taller than mask
            imageHeight = (int)(((float)imageHeight / imageWidth) * width);
            imageWidth = width;
        }

        final int imageLeft = (width - imageWidth) / 2 + bounds.left;
        final int imageTop = (height - imageHeight) / 2 + bounds.top;
        image.setBounds(new Rect(imageLeft, imageTop, imageLeft + imageWidth,
                imageTop + imageHeight));
    }

    @SuppressWarnings("FieldCanBeLocal")
    private final Drawable.Callback childCallback = new Drawable.Callback() {
        @Override
        public void invalidateDrawable(Drawable who) {
            invalidateSelf();
        }

        @Override
        public void scheduleDrawable(Drawable who, Runnable what, long when) {
            scheduleSelf(what, when);
        }

        @Override
        public void unscheduleDrawable(Drawable who, Runnable what) {
            unscheduleSelf(what);
        }
    };
}
