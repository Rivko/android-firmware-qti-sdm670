/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.panoramaui.utils;

import android.graphics.drawable.AnimationDrawable;
import android.graphics.drawable.Drawable;
import android.graphics.drawable.LayerDrawable;

/**
 * Methods to help manage animations
 */
public class AnimationUtils {
    // Not constructable
    private AnimationUtils() {}

    /**
     * Recursively walk a {@link Drawable} (which may have children), and start any
     * {@link AnimationDrawable}s found
     * @param d the root of the {@link Drawable} tree
     * @throws NullPointerException if d is null
     */
    public static void startAnimationsRecursively(Drawable d) {
        if (d == null) {
            throw new NullPointerException("d may not be null");
        }

        // N.B. AnimationDrawable is also a DrawableContainer, hence don't combine this with the
        // below if statement
        if (d instanceof AnimationDrawable) {
            ((AnimationDrawable)d).start();
        }

        if (d instanceof LayerDrawable) {
            LayerDrawable layerDrawable = (LayerDrawable)d;
            for (int i = 0; i < layerDrawable.getNumberOfLayers(); i++) {
                startAnimationsRecursively(layerDrawable.getDrawable(i));
            }
        } else {
            Drawable d2 = d.getCurrent();
            if (d2 != d) {
                startAnimationsRecursively(d2);
            }
        }
    }

    /**
     * Get the total duration of an {@link AnimationDrawable}
     * @param animation the {@link AnimationDrawable} for which to get the total duration
     * @return the total duration, in ms
     * @throws NullPointerException if animation is null
     */
    public static int getTotalDuration(AnimationDrawable animation) {
        if (animation == null) {
            throw new NullPointerException("animation may not be null");
        }

        int totalDurationMs = 0;
        for (int i = 0; i < animation.getNumberOfFrames(); i++) {
            totalDurationMs += animation.getDuration(i);
        }

        return totalDurationMs;
    }
}
