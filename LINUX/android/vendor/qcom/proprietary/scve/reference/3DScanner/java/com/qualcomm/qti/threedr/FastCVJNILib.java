/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.threedr;

import android.graphics.Bitmap;

import java.nio.ByteBuffer;
import java.nio.ShortBuffer;

public class FastCVJNILib {
    static {
        try {
            System.loadLibrary("fastcvopt");
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

    public static native boolean RGB888ByteBufferFlip(ByteBuffer src, int imgWidth, int imgHeight, byte[] dst);

    public static native boolean FlipU16(byte[] src, int imgWidth, int imgHeight, short[] dst);

    public static native boolean RGB888ByteBufferToYCbCr420PseudoPlanar(ByteBuffer src, int imgWidth, int imgHeight, byte[] outYUV);

    public static native boolean RGB565ByteBufferToRGB888ByteArray(byte[] src, int imgWidth, int imgHeight, byte[] dst);

}


