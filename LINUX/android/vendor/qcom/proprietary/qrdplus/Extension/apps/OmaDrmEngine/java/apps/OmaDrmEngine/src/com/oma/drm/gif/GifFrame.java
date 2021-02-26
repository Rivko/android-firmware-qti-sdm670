/*
 * This file is originated from Android Open Source Project,
 * platform/packages/apps/Gallery2.git /src/com/android/gallery3d/util/GifFrame.java
 */

package com.oma.drm.gif;

import android.graphics.Bitmap;

public class GifFrame {

    public Bitmap mImage;
    public int mDelayInMs; // in milliseconds
    public int mDispose;
    public GifFrame mNextFrame = null;

    public GifFrame(Bitmap bitmap, int delay, int dispose) {
        mImage = bitmap;
        mDelayInMs = delay;
        mDispose = dispose;
    }
}
