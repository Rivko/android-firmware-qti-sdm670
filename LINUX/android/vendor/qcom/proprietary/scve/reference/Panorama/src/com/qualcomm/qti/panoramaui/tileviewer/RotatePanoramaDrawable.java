/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.panoramaui.tileviewer;

import android.graphics.Canvas;
import android.graphics.ColorFilter;
import android.graphics.Rect;
import android.graphics.drawable.Drawable;

/**
 * This class rotates another {@link Drawable} by a fixed rotation.
 */
public class RotatePanoramaDrawable extends Drawable {
    private final Drawable localDrawable;
    private RotatePanoramaState state;
    private boolean mutated;

    /**
     * Construct a {@link RotatePanoramaDrawable}
     * @param drawable {@link Drawable} to rotate
     * @param rotation Degrees clockwise that drawable should be rotated
     * @throws IllegalArgumentException if drawable is null, or if rotation is not 0/90/180/270
     */
    public RotatePanoramaDrawable(Drawable drawable, int rotation) {
        if (drawable == null) {
            throw new IllegalArgumentException("drawable cannot be null");
        } else if (rotation != 0 && rotation != 90 && rotation != 180 && rotation != 270) {
            throw new IllegalArgumentException("rotation must be 0, 90, 180, or 270");
        }

        state = new RotatePanoramaState(drawable, rotation);
        localDrawable = drawable;
        localDrawable.setCallback(callback);
    }

    private RotatePanoramaDrawable(RotatePanoramaState state) {
        this.state = state;
        localDrawable = state.sourceDrawable.getConstantState().newDrawable();
        localDrawable.setCallback(callback);
    }

    /** {@inheritDoc} */
    @Override
    public void draw(Canvas canvas) {
        final int restoreToCount = canvas.save();
        final Rect bounds = getBounds();

        canvas.clipRect(bounds);
        canvas.translate(bounds.left + bounds.width() / 2f, bounds.top + bounds.height() / 2f);
        canvas.rotate(state.rotation);
        if (state.rotation == 0 || state.rotation == 180) {
            canvas.translate(-bounds.width() / 2f, -bounds.height() / 2f);
        } else {
            canvas.translate(-bounds.height() / 2f, -bounds.width() / 2f);
        }

        localDrawable.draw(canvas);

        canvas.restoreToCount(restoreToCount);
    }

    /** {@inheritDoc} */
    @Override
    public Drawable getCurrent() {
        return localDrawable;
    }

    /** {@inheritDoc} */
    @Override
    public void setAlpha(int alpha) {
        localDrawable.setAlpha(alpha);
    }

    /** {@inheritDoc} */
    @Override
    public int getAlpha() {
        return localDrawable.getAlpha();
    }

    /** {@inheritDoc} */
    @Override
    public void setColorFilter(ColorFilter cf) {
        state.sourceDrawable.setColorFilter(cf);
    }

    /** {@inheritDoc} */
    @Override
    public int getOpacity() {
        return state.sourceDrawable.getOpacity();
    }

    /** {@inheritDoc} */
    @Override
    public int getIntrinsicWidth() {
        return (state.rotation == 0 || state.rotation == 180 ?
                state.sourceDrawable.getIntrinsicWidth() :
                state.sourceDrawable.getIntrinsicHeight());
    }

    /** {@inheritDoc} */
    @Override
    public int getIntrinsicHeight() {
        return (state.rotation == 0 || state.rotation == 180 ?
                state.sourceDrawable.getIntrinsicHeight() :
                state.sourceDrawable.getIntrinsicWidth());
    }

    /** {@inheritDoc} */
    @Override
    public int getMinimumWidth() {
        return (state.rotation == 0 || state.rotation == 180 ?
                state.sourceDrawable.getMinimumWidth() :
                state.sourceDrawable.getMinimumHeight());
    }

    /** {@inheritDoc} */
    @Override
    public int getMinimumHeight() {
        return (state.rotation == 0 || state.rotation == 180 ?
                state.sourceDrawable.getMinimumHeight() :
                state.sourceDrawable.getMinimumWidth());
    }

    /** {@inheritDoc} */
    @Override
    public boolean getPadding(Rect padding) {
        boolean hasPadding = super.getPadding(padding);

        int temp;
        switch (state.rotation) {
        case 90:
            temp = padding.left;
            padding.left = padding.bottom;
            padding.bottom = padding.right;
            padding.right = padding.top;
            padding.top = temp;
            break;

        case 180:
            temp = padding.left;
            padding.left = padding.right;
            padding.right = temp;
            temp = padding.top;
            padding.top = padding.bottom;
            padding.bottom = temp;
            break;

        case 270:
            temp = padding.left;
            padding.left = padding.top;
            padding.top = padding.right;
            padding.right = padding.bottom;
            padding.bottom = temp;
            break;
        }

        return hasPadding;
    }

    /** {@inheritDoc} */
    @Override
    public boolean isStateful() {
        return state.sourceDrawable.isStateful();
    }

    /** {@inheritDoc} */
    @Override
    protected void onBoundsChange(Rect bounds) {
        if (state.rotation == 0 || state.rotation == 180) {
            localDrawable.setBounds(0, 0, bounds.width(), bounds.height());
        } else {
            localDrawable.setBounds(0, 0, bounds.height(), bounds.width());
        }
    }

    /** {@inheritDoc} */
    @Override
    protected boolean onLevelChange(int level) {
        return state.sourceDrawable.setLevel(level);
    }

    /** {@inheritDoc} */
    @Override
    protected boolean onStateChange(int[] state) {
        return this.state.sourceDrawable.setState(state);
    }

    /** {@inheritDoc} */
    @Override
    public boolean setVisible(boolean visible, boolean restart) {
        return state.sourceDrawable.setVisible(visible, restart);
    }

    /** {@inheritDoc} */
    @Override
    public void setDither(boolean dither) {
        state.sourceDrawable.setDither(dither);
    }

    /** {@inheritDoc} */
    @Override
    public void setFilterBitmap(boolean filter) {
        state.sourceDrawable.setFilterBitmap(filter);
    }

    /** {@inheritDoc} */
    @Override
    public int getChangingConfigurations() {
        return super.getChangingConfigurations() |
                state.changingConfigurations |
                state.sourceDrawable.getChangingConfigurations();
    }

    /** {@inheritDoc} */
    @Override
    public ConstantState getConstantState() {
        state.changingConfigurations = getChangingConfigurations();
        return state;
    }

    /** {@inheritDoc} */
    @Override
    public Drawable mutate() {
        if (!mutated && super.mutate() == this) {
            localDrawable.mutate();
            state = new RotatePanoramaState(state, localDrawable);
            mutated = true;
        }
        return this;
    }

    private final Callback callback = new Callback() {
        @Override
        public void scheduleDrawable(Drawable who, Runnable what, long when) {
            final Callback rotateCallback = getCallback();
            if (rotateCallback != null && who == localDrawable) {
                rotateCallback.scheduleDrawable(RotatePanoramaDrawable.this, what, when);
            }
        }

        @Override
        public void unscheduleDrawable(Drawable who, Runnable what) {
            final Callback rotateCallback = getCallback();
            if (rotateCallback != null && who == localDrawable) {
                rotateCallback.unscheduleDrawable(RotatePanoramaDrawable.this, what);
            }
        }

        @Override
        public void invalidateDrawable(Drawable who) {
            final Callback rotateCallback = getCallback();
            if (rotateCallback != null && who == localDrawable) {
                rotateCallback.invalidateDrawable(RotatePanoramaDrawable.this);
            }
        }
    };

    protected static final class RotatePanoramaState extends ConstantState {
        final Drawable sourceDrawable;
        final int rotation; // restricted to 0, 90, 180, or 270
        int changingConfigurations;

        public RotatePanoramaState(Drawable drawable, int rotation) {
            this.sourceDrawable = drawable;
            this.rotation = rotation;
        }

        public RotatePanoramaState(RotatePanoramaState state, Drawable drawable) {
            this.sourceDrawable = drawable;
            this.rotation = state.rotation;
        }

        @Override
        public Drawable newDrawable() {
            return new RotatePanoramaDrawable(this);
        }

        @Override
        public int getChangingConfigurations() {
            return changingConfigurations;
        }
    }
}
