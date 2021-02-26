/*
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.threedr.utils;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;

import java.lang.ref.WeakReference;
import java.util.HashMap;

/**
 * This class holds weak references on bitmaps it loads. If a request for a bitmap is made while it
 * is still active (ie someone else had a reference to it), it will be reused, rather than reloaded.
 * This both speeds up bitmap loading and reduces memory requirements.
 */
public class BitmapCache {
    private static HashMap<String, WeakReference<Bitmap>> mOrigCache = new HashMap<String, WeakReference<Bitmap>>();

    /**
     * Loads a bitmap from the specified path. If the bitmap has previously been loaded during this
     * app's lifetime, and at least one instance of it still exists somewhere, it will be loaded
     * from a runtime cache instead.
     *
     * @param path Path of the image to load
     * @return The specified bitmap, scaled down to screen dimensions if requested
     */
    public static Bitmap loadBitmapThroughCache(String path) {

        Bitmap b = null;
        WeakReference<Bitmap> weak = null;

        if (weak == null) {
            weak = mOrigCache.get(path);
        }

        if (weak != null) {
            b = weak.get();
        }

        if (b == null) {
            b = BitmapFactory.decodeFile(path);
            mOrigCache.put(path, new WeakReference<Bitmap>(b));
        }

        return b;
    }
}
