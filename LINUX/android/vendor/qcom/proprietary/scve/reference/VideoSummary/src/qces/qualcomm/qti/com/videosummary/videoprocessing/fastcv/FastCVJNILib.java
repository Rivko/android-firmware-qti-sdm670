/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.videoprocessing.fastcv;

import android.graphics.Bitmap;

import java.nio.ByteBuffer;

public class FastCVJNILib {
    static {
        try {
            System.loadLibrary("UtilsColorConv");
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

    public static native boolean YUV420toRGB8888down(byte[] imgdata, int srcWidth, int srcHeight, int extraRows, Bitmap outputBitmap);

    public static native boolean YUV420toRGB8888downscaleByteArrays(byte[] imgdata, int[] rgbdata, int srcWidth, int srcHeight, int srcYStride, int srcCStride, int outWidth, int outHeight);

    public static native boolean YUV420SPscaleDownLetterbox(ByteBuffer src, int srcWidth, int srcHeight, ByteBuffer dst, int dstWidth, int dstHeight, boolean outputForBitmap);

    public static native boolean YUV420ByteBufferToRGB8888Bitmap(ByteBuffer src, int srcWidth, int srcHeight, int srcYStride, int srcCStride, Bitmap outputBitmap);

    public static native boolean RGB565ByteBufferToYUV420ByteBuffer(ByteBuffer src, int imgWidth, int imgHeight, int imgStride, ByteBuffer dstYUV);

}


