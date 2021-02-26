/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.imageedit.segmentation;

import android.graphics.Bitmap;

public class Alpha8JNILib {
    static {
        try {
            System.loadLibrary("BitmapUtils");
        }
        catch (UnsatisfiedLinkError e) {
            System.err.println("Native code library failed to load.\n" + e);
        }
    }

    /**
     * Not constructable, static helper utils only
     */
    private Alpha8JNILib() {
    }

    public static native void FillAlphaBitmapWithByteArray(byte[] inputByteArray, Bitmap outputBitmap);

    public static native void CopyAlpha8Pixels(Bitmap inputBitmap, Bitmap outputBitmap);
}
