/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.panoramaui.tileviewer;

import java.nio.ByteBuffer;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.ColorFilter;
import android.graphics.Paint;
import android.graphics.PixelFormat;
import android.graphics.PointF;
import android.graphics.Rect;
import android.graphics.Region;
import android.graphics.drawable.Drawable;

/**
 * The {@link TiledImageDrawable} class is a {@link Drawable} which can be used to draw very large
 * images. The image contents will be downsampled accordingly to optimize the memory consumption
 * for the current display size, as well as providing efficient translate & scale operations.
 */
public class TiledImageDrawable extends Drawable {
    private final TileCache tileCache;
    private final Rect drawWorkingRect = new Rect();
    private final Region drawWorkingRegion = new Region();
    private TiledImageState state;
    private boolean mutated;
    private final PointF origin = new PointF();
    private float currentScale;
    private float minScale;

    // These fields are synthesized from those above
    private int downsample;
    private int downsampleLog2;
    private float downsampledScale;
    private int downsampledImageWidth;
    private int downsampledImageHeight;

    /**
     * Construct a new {@link TiledImageDrawable}. The position and scale will be initialized such
     * that the entire image fits within the Drawable bounds.
     * @param imageData a {@link ByteBuffer} containing the image data to display
     * @param maxScale the maximum scale to which the image may be zoomed. Must be >= 1.0.
     * @throws IllegalArgumentException if imageData is null, or if maxScale < 1.0
     */
    public TiledImageDrawable(ByteBuffer imageData, float maxScale) {
        this(new TiledImageState(imageData, maxScale));
    }

    private TiledImageDrawable(TiledImageState state) {
        tileCache = new TileCache(state.decoder, tileCacheListener);
        this.state = state;
        setOrigin(getIntrinsicWidth() / 2f, getIntrinsicHeight() / 2f);
        minScale = Float.MIN_NORMAL;
        setScale(Float.MIN_NORMAL);
    }

    /**
     * Set the position within the image which will be centered within the Drawable bounds
     * @param x X coordinate to be centered within the Drawable bounds
     * @param y Y coordinate to be centered within the Drawable bounds
     */
    public void setOrigin(float x, float y) {
        if (x < 0) {
            x = 0;
        } else if (x > getIntrinsicWidth()) {
            x = getIntrinsicWidth();
        }

        if (y < 0) {
            y = 0;
        } else if (y > getIntrinsicHeight()) {
            y = getIntrinsicHeight();
        }

        if (!origin.equals(x, y)) {
            origin.set(x, y);
            invalidateSelf();
        }
    }

    /**
     * Get the current position within the image which is centered within the Drawable bounds
     * @return a {@link PointF} containing the current position within the image which is centered
     *      within the Drawable bounds. Do not modify the returned object.
     */
    public PointF getOrigin() {
        return origin;
    }

    /**
     * Set the current scale to apply to the image (around the current origin)
     * @param scale the scale to apply, between {@link #getMinimumScale()} and 1.0)
     */
    public void setScale(float scale) {
        if (scale > state.maxScale) {
            scale = state.maxScale;
        } else if (scale < minScale) {
            scale = minScale;
        }

        if (currentScale != scale) {
            currentScale = scale;

            // N.B. This is an optimization for calculating (int)log2(1f / scale)
            if (scale <= 0.5f) {
                downsampleLog2 = 31 - Integer.numberOfLeadingZeros(((int)(1f / scale)));
            } else {
                downsampleLog2 = 0;
            }
            downsample = (1 << downsampleLog2);
            downsampledScale = currentScale * downsample;
            downsampledImageWidth = state.decoder.getWidth(downsampleLog2);
            downsampledImageHeight = state.decoder.getHeight(downsampleLog2);

            invalidateSelf();
        }
    }

    /**
     * Get the current scale applied to the image (around the current origin)
     * @return the current scale, between {@link #getMinimumScale()} and 1.0)
     */
    public float getScale() {
        return currentScale;
    }

    /**
     * Get the minimum scale which can be applied to this {@link TiledImageDrawable} (ie the scale
     * value which will cause the image to fill the Drawable bounds)
     */
    public float getMinimumScale() {
        return minScale;
    }

    /** {@inheritDoc} */
    @Override
    public int getIntrinsicWidth() {
        return state.decoder.getWidth(0);
    }

    /** {@inheritDoc} */
    @Override
    public int getIntrinsicHeight() {
        return state.decoder.getHeight(0);
    }

    /** {@inheritDoc} */
    @Override
    protected void onBoundsChange(Rect bounds) {
        if (getIntrinsicWidth() * bounds.height() >= bounds.width() * getIntrinsicHeight()) {
            // Image has wider aspect than bounds
            minScale = (float)bounds.width() / getIntrinsicWidth();
        } else {
            // Image has narrower aspect than bounds
            minScale = (float)bounds.height() / getIntrinsicHeight();
        }

        if (minScale > 1f) {
            minScale = 1f;
        }

        if (currentScale < minScale) {
            setScale(minScale);
        }

        tileCache.setTiledAreaDimensions(bounds.width(), bounds.height());
    }

    /** {@inheritDoc} */
    @Override
    public void draw(Canvas canvas) {
        final int restoreTo = canvas.save();

        final Rect bounds = getBounds();
        canvas.clipRect(bounds);
        canvas.translate(bounds.left + bounds.width() / 2f, bounds.top + bounds.height() / 2f);
        canvas.scale(downsampledScale, downsampledScale);
        canvas.translate(-origin.x / downsample, -origin.y / downsample);
        canvas.clipRect(0, 0, downsampledImageWidth, downsampledImageHeight);

        // Request tiles up to 2 tiles beyond the edge of the displayed image, which helps hide the
        // tile load delay when scrolling
        canvas.getClipBounds(drawWorkingRect);
        drawWorkingRect.inset(-TileCache.TILE_SIZE_PX * 2, -TileCache.TILE_SIZE_PX * 2);
        drawWorkingRegion.set(drawWorkingRect);

        tileCache.mark();

        for (int y = 0; y < downsampledImageHeight; y += TileCache.TILE_SIZE_PX) {
            for (int x = 0; x < downsampledImageWidth; x += TileCache.TILE_SIZE_PX) {
                if (drawWorkingRegion.quickReject(x, y, x + TileCache.TILE_SIZE_PX,
                        y + TileCache.TILE_SIZE_PX)) {
                    continue;
                }

                Bitmap tile = tileCache.getTile(x, y, downsampleLog2);
                canvas.drawBitmap(tile, x, y, state.paint);
            }
        }

        tileCache.sweep();

        canvas.restoreToCount(restoreTo);
    }

    /** {@inheritDoc} */
    @Override
    public void setAlpha(int alpha) {
        state.paint.setAlpha(alpha);
    }

    /** {@inheritDoc} */
    @Override
    public int getAlpha() {
        return state.paint.getAlpha();
    }

    /** {@inheritDoc} */
    @Override
    public void setColorFilter(ColorFilter cf) {
        state.paint.setColorFilter(cf);
    }

    /** {@inheritDoc} */
    @Override
    public int getOpacity() {
        return PixelFormat.TRANSPARENT; // since we may not render our entire bounds
    }

    /** {@inheritDoc} */
    @Override
    public int getChangingConfigurations() {
        return super.getChangingConfigurations() | state.changingConfigurations;
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
        if (!mutated) {
            super.mutate();
            state = new TiledImageState(state);
            mutated = true;
        }

        return this;
    }

    private final TileCache.Listener tileCacheListener = new TileCache.Listener() {
        @Override
        public void onTileCacheUpdated() {
                invalidateSelf();
            }
    };

    protected final static class TiledImageState extends ConstantState {
        final JpegTileDecoder decoder;
        final Paint paint;
        final float maxScale;

        int changingConfigurations;

        TiledImageState(ByteBuffer imageData, float maxScale) {
            if (imageData == null) {
                throw new IllegalArgumentException("imageData cannot be null");
            } else if (maxScale < 1f) {
                throw new IllegalArgumentException("maxScale must be >= 1.0");
            }

            this.maxScale = maxScale;
            decoder = new JpegTileDecoder(imageData);
            paint = new Paint(Paint.ANTI_ALIAS_FLAG | Paint.FILTER_BITMAP_FLAG);
        }

        TiledImageState(TiledImageState state) {
            maxScale = state.maxScale;
            decoder = state.decoder;
            paint = new Paint(state.paint);
            changingConfigurations = state.changingConfigurations;
        }

        @Override
        public Drawable newDrawable() {
            return new TiledImageDrawable(this);
        }

        @Override
        public int getChangingConfigurations() {
            return changingConfigurations;
        }
    }
}
