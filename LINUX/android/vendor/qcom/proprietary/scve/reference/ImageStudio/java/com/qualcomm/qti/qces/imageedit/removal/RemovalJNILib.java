/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.imageedit.removal;

import android.graphics.Bitmap;

final class RemovalJNILib {
    static
    {
        try
        {
            System.loadLibrary("ImageRemoval");
        }
        catch( UnsatisfiedLinkError e )
        {
            System.err.println("Native code library failed to load.\n" + e);
        }
    }

    private RemovalJNILib() { }

    public static native void removalInit(ObjectRemover obj, boolean gpuEnabled);
    public static native void removalDeInit();
    public static native void removalOperation(Bitmap image, Bitmap mask, int patchSize);
    public static native void removalStop();
}
