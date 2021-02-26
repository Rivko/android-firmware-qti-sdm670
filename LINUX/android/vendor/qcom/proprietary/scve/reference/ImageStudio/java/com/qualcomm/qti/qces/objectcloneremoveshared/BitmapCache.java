/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.objectcloneremoveshared;

import java.io.FileNotFoundException;
import java.lang.ref.WeakReference;
import java.util.HashMap;

import android.content.Context;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.Matrix;
import android.graphics.Matrix.ScaleToFit;
import android.graphics.RectF;
import android.net.Uri;

import com.qualcomm.qti.qces.util.ImageUtils;

/**
 * This class holds weak references on bitmaps it loads. If a request for a bitmap is made while it
 * is still active (ie someone else had a reference to it), it will be reused, rather than reloaded.
 * This both speeds up bitmap loading and reduces memory requirements.
 */
public class BitmapCache {
    private static HashMap<Uri, WeakReference<Bitmap>> mOrigCache = new HashMap<Uri, WeakReference<Bitmap>>();
    private static HashMap<Uri, WeakReference<Bitmap>> mScaledCache = new HashMap<Uri, WeakReference<Bitmap>>();

    /**
     * Loads a bitmap from the specified path. If the bitmap has previously been loaded during this
     * app's lifetime, and at least one instance of it still exists somewhere, it will be loaded
     * from a runtime cache instead.
     * @param res Application resources, used to retrieve screen dimensions
     * @param path Path of the image to load
     * @param scaleToScreenDimensions If true, the bitmap will be scaled to be no larger than the
     *        device screen
     * @return The specified bitmap, scaled down to screen dimensions if requested
     */
    public static Bitmap loadBitmapThroughCache(Context context, Uri uri,
            boolean scaleToScreenDimensions, boolean isFullMP) {

        Bitmap b = null;
        WeakReference<Bitmap> weak = null;

        if (scaleToScreenDimensions) {
            weak = mScaledCache.get(uri);
        }
        if (weak == null) {
            weak = mOrigCache.get(uri);
        }

        if (weak != null) {
            b = weak.get();
        }

        if (b == null) {
            try {
                b = ImageUtils.loadImage(context, uri, isFullMP);
                mOrigCache.put(uri, new WeakReference<Bitmap>(b));
            } catch (FileNotFoundException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }

            if (b.getWidth() > resizeToHdImgWidth || b.getHeight() > resizeToHdImgHeight) {

                Matrix m = new Matrix();
                //TODO: can this rescale be combined with possible below rescale into single scale?
                m.setRectToRect(new RectF(0, 0, b.getWidth(), b.getHeight()), new RectF(0, 0,
                        resizeToHdImgWidth, resizeToHdImgHeight), ScaleToFit.CENTER);
                Bitmap bScaled = Bitmap.createBitmap(b, 0, 0, b.getWidth(), b.getHeight(), m, true);
                mScaledCache.put(uri, new WeakReference<Bitmap>(bScaled));

                b = bScaled;
            }


            if (b.getWidth() % 8 != 0) {
                //scale (possibly again) in case the bitmap width isn't multiple of 8 (necessary for FastCV dilate on the mask)
                int newWidth = (b.getWidth() / 8) * 8;

                Matrix m = new Matrix();
                m.setRectToRect(new RectF(0, 0, b.getWidth(), b.getHeight()), new RectF(0, 0,
                        newWidth, resizeToHdImgHeight), ScaleToFit.CENTER);

                Bitmap bScaled = Bitmap.createBitmap(b, 0, 0, b.getWidth(), b.getHeight(), m, true);
                mScaledCache.put(uri, new WeakReference<Bitmap>(bScaled));

                b = bScaled;
            }

        }

        return b;
    }

    /**
     * @see BitmapCache#loadBitmapThroughCache(Resources, String, boolean)
     */
    public static Bitmap loadBitmapThroughCache(Context context, Uri uri, boolean isFullMP) {
        return BitmapCache.loadBitmapThroughCache(context, uri, true, isFullMP);
    }

    public static final int resizeToHdImgWidth = 1920;
    public static final int resizeToHdImgHeight = 1080;
}
