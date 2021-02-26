/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.panoramaui.tileviewer;

import android.graphics.drawable.Drawable;
import android.graphics.drawable.LayerDrawable;

/**
 * A standard {@link LayerDrawable} can change layer insets, but they only take effect after the
 * Drawable's bounds are changed. This subclass forces an
 * {@link #onBoundsChange(android.graphics.Rect)} after each
 * {@link #setLayerInset(int, int, int, int, int)}
 */
public class DynamicInsetLayerDrawable extends LayerDrawable {
    /**
     * Construct a new {@link DynamicInsetLayerDrawable}
     * @param layers see {@link LayerDrawable#LayerDrawable(Drawable[])}
     */
    public DynamicInsetLayerDrawable(Drawable[] layers) {
        super(layers);
    }

    /** {@inheritDoc} */
    @Override
    public void setLayerInset(int index, int l, int t, int r, int b) {
        super.setLayerInset(index, l, t, r, b);
        onBoundsChange(getBounds());
    }
}
