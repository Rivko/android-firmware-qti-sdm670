/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.imageedit.objectclone;

import android.graphics.Bitmap;

final class CL2JNILib {

    static {
        try {
            System.loadLibrary("ImageCloningSample");
        } catch (UnsatisfiedLinkError e) {
            System.err.println("Native code library failed to load.\n" + e);
        }
    }

    private CL2JNILib() {
    }

    public static native void cloningInit(boolean gpuEnabled);

    public static native void cloningDeinit();

    public static native void cloningInitImage(Bitmap object, Bitmap mask, boolean isFilled);

    public static native void cloningOperation(Bitmap target);
}
