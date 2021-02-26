/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.imageedit.objectclone;

import android.graphics.Bitmap;

public class BorderReplicateJNILib {
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
    private BorderReplicateJNILib() {
    }

    public static native void BorderReplicateRGBA32(Bitmap intputBitmap, Bitmap outputBitmap);

}
