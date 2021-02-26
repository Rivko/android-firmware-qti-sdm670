/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.imageedit.fastcv;

import android.graphics.Bitmap;

public class FastCVJNILib {
    static {
        try {
            System.loadLibrary("FastCV");
        }
        catch (UnsatisfiedLinkError e) {
            System.err.println("Native code library failed to load.\n" + e);
        }
    }

    /**
     * Not constructable, static helper utils only
     */
    private FastCVJNILib() {
    }

    public static native String getFastCVVersion();

    public static native boolean dilateNxN(Bitmap intputMask, int N, Bitmap outputMask);

}
