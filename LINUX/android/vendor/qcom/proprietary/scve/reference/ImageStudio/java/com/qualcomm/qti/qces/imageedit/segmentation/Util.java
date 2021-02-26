/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.imageedit.segmentation;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;

import com.qualcomm.qti.qces.imageedit.jnilib.SoftSegmentationJNILib;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.drawable.Drawable;
import android.util.Log;

public class Util {

    private static final String TAG = "Segmentation";

    public static void printBitmap(String title, Bitmap bitmap) {
        int uknownPixelCount = 0;
        int backgroundPixelCount = 0;
        int foregroundPixelCount = 0;
        int mattePixelCount = 0;
        int otherPixelCount = 0;
        byte[] pixel = getPixelsAsByte(bitmap);
        for (int i = 0; i < pixel.length; i++) {
            if (pixel[i] == SoftSegmentationJNILib.MASK_UNKNOWN)
                uknownPixelCount++;
            else if (pixel[i] == SoftSegmentationJNILib.MASK_BACKGROUND)
                backgroundPixelCount++;
            else if (pixel[i] == SoftSegmentationJNILib.MASK_FOREGROUND)
                foregroundPixelCount++;
            else if (pixel[i] == SoftSegmentationJNILib.MASK_MATTE)
                mattePixelCount++;
            else {
                otherPixelCount++;
            }
        }
        Log.d(TAG, title + " total pixel count = " + pixel.length + ", backgroundPixelCount = " +
                backgroundPixelCount + ", foregroundPixelCount = " + foregroundPixelCount +
                ", uknownPixelCount = " + uknownPixelCount + ", mattePixelCount = " +
                mattePixelCount + ", otherPixelCount = " + otherPixelCount);
    }

    public static int[] getPixelsAsInt(Bitmap bitmap) {
        if (bitmap != null) {
            int[] intArray = new int[bitmap.getWidth() * bitmap.getHeight()];
            bitmap.getPixels(intArray, 0, bitmap.getWidth(), 0, 0, bitmap.getWidth(),
                    bitmap.getHeight());
            return intArray;
        }
        return new int[0];
    }

    public static byte[] getPixelsAsByte(Bitmap bitmap) {
        if (bitmap != null) {
            ByteBuffer buffer = ByteBuffer.allocate(bitmap.getAllocationByteCount());
            bitmap.copyPixelsToBuffer(buffer);
            return buffer.array();
        }
        return new byte[0];
    }

    public static String saveBitmapBytesToFile(Activity activity, String filename, Bitmap bmap) {

        byte[] bytes = getPixelsAsByte(bmap);
        String filePath = null;

        File destination = new File(activity.getCacheDir(), filename);
        try {
            FileOutputStream out = new FileOutputStream(destination);
            out.write(bytes);
            filePath = destination.getAbsolutePath();
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }

        return filePath;
    }

    public static float getHeightToWidthRatioForDrawable(Drawable asset) {

        // Find the images Height to Width ratio
        int imgWidth = asset.getIntrinsicWidth();
        int imgHeight = asset.getIntrinsicHeight();
        float heighToWidthRatio = (float)imgHeight / (float)imgWidth;

        return heighToWidthRatio;

    }
}
