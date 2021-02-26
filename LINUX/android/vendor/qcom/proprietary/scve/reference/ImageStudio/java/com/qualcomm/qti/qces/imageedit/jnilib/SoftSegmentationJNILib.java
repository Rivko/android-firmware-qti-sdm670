/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.imageedit.jnilib;

import android.graphics.Bitmap;

public class SoftSegmentationJNILib {

    private static final String TAG = SoftSegmentationJNILib.class.getSimpleName();
    // These constants come from SCVE library
    public static final int MASK_UNKNOWN = 0;
    public static final int MASK_BACKGROUND = 1;
    public static final int MASK_FOREGROUND = 2;
    public static final int MASK_MATTE = 3;

    public static final int SCRIBBLE_SELECTION = 0;
    public static final int QUICK_SELECTION = 1;

    static {
        try {
            System.loadLibrary("SoftSegmentation");
            nClassInit();
        } catch (UnsatisfiedLinkError e) {
            System.err.println("Native code library failed to load.\n" + e);
        }
    }

    private Bitmap source;
    private boolean released;

    public SoftSegmentationJNILib(Bitmap source, int segmentationMethod) {
        if (source != null && source.getConfig() == Bitmap.Config.ARGB_8888) {
            this.source = source;
            nInit(source, segmentationMethod);
        } else {
            throw new IllegalArgumentException("The image selected was invalid");
        }
    }

    public void release() {
        if (!this.released) {
            this.released = true;
            nDestroy();
        }
    }

    @Override
    protected void finalize() throws Throwable {
        super.finalize();
        release();
    }

    public int processSegmentationImage(final Bitmap inputMask, final Bitmap outputMask,
                                        final Bitmap previousMask) {
        if (released) {
            throw new RuntimeException("Object already released!");
        }

        return nProcessSegmentation(inputMask, outputMask, previousMask);
    }

    public int processMattingImage(final Bitmap trimap, final Bitmap outputMask,
            final Bitmap outputForeground, final Bitmap outputBackground) {
        if (released) {
            throw new RuntimeException("Object already released!");
        } else if (!inputsValid(trimap, outputMask, outputForeground, outputBackground)) {
            throw new IllegalArgumentException("Invalid input arguments");
        }

        return nProcessMatting(trimap, outputMask, outputForeground, outputBackground);
    }

    private boolean inputsValid(Bitmap trimap, Bitmap outputMask, Bitmap outputForeground,
            Bitmap outputBackground) {
        if (trimap != null && outputMask != null && outputForeground != null &&
                outputBackground != null &&
                imagesHaveSameDimensions(trimap, outputMask, outputForeground, outputBackground) &&
                trimap.getConfig() == Bitmap.Config.ALPHA_8 &&
                outputMask.getConfig() == Bitmap.Config.ALPHA_8 &&
                outputForeground.getConfig() == Bitmap.Config.ARGB_8888 &&
                outputBackground.getConfig() == Bitmap.Config.ARGB_8888 && outputMask.isMutable() &&
                outputForeground.isMutable() && outputBackground.isMutable()) {
            return true;
        }
        return false;
    }

    private boolean imagesHaveSameDimensions(Bitmap trimap, Bitmap outputMask,
            Bitmap outputForeground, Bitmap outputBackground) {
        if (trimap.getWidth() == outputMask.getWidth() &&
                trimap.getWidth() == outputForeground.getWidth() &&
                trimap.getWidth() == outputBackground.getWidth() &&
                trimap.getHeight() == outputMask.getHeight() &&
                trimap.getHeight() == outputForeground.getHeight() &&
                trimap.getHeight() == outputBackground.getHeight()) {
            return true;
        }
        return false;
    }

    /* ---------------- JNI INTERFACE BELOW HERE ---------------- */

    private long nativeContext;

    private static native void nClassInit();

    private native void nInit(Bitmap source, int segmentationMethod);

    private native int nProcessSegmentation(Bitmap inputMask, Bitmap outputMask, Bitmap previousMask);

    private native int nProcessMatting(Bitmap trimap, Bitmap outputMask, Bitmap outputForeground,
            Bitmap outputBackground);

    private native void nDestroy();
}
