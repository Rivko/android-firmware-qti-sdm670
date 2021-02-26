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

import android.graphics.Bitmap;
import android.graphics.Color;
import android.net.Uri;
import android.os.Bundle;

import com.qualcomm.qti.qces.imageedit.jnilib.SoftSegmentationJNILib;
import com.qualcomm.qti.qces.imageedit.segmentation.SegmentationBitmapManager.BitmapSet;

public abstract class AbstractSegmentationModel {

    private AbstractSegmentationActivity activity;
    private SoftSegmentationJNILib softSegmentationJNILib;
    protected BitmapSet bitmapSet;

    public AbstractSegmentationModel(AbstractSegmentationActivity activity) {
        this.activity = activity;
    }

    protected void initializeSoftSegJNILib() {

        if (softSegmentationJNILib == null && bitmapSet != null) {

            softSegmentationJNILib = new SoftSegmentationJNILib(bitmapSet.source,
                  SoftSegmentationJNILib.QUICK_SELECTION);

        }
    }

    SoftSegmentationJNILib getSoftSegmentationJNILib() {
        return softSegmentationJNILib;
    }

    void releaseImage() {
        if (softSegmentationJNILib != null) {
            softSegmentationJNILib = null;
        }

        if (bitmapSet != null) {
            bitmapSet.release();
            bitmapSet = null;
        }
    }

    Bitmap getImageBitmap() {
        //return (bitmapSet != null ? bitmapSet.source : null);
        return (bitmapSet != null ? bitmapSet.drawBitmap : null);
    }

    Bitmap getMaskAlphaBitmap() {
        return (bitmapSet != null ? bitmapSet.inputSegmentation : null);
    }

    Bitmap getOutputMaskBitmap() {
        return (bitmapSet != null ? bitmapSet.outputMask : null);
    }

    void clearMask() {
        bitmapSet.inputSegmentation.eraseColor(Color.TRANSPARENT);
    }

    protected BitmapSet initializeImageBitmap(Uri imgUri, Boolean isFullMP) {
        bitmapSet = SegmentationBitmapManager.getInstance(activity).acquire(imgUri, isFullMP);
        bitmapSet.inputSegmentation.eraseColor(Color.TRANSPARENT);
        bitmapSet.outputMask.eraseColor(Color.TRANSPARENT);
        bitmapSet.foreground.eraseColor(Color.TRANSPARENT);
        bitmapSet.background.eraseColor(Color.TRANSPARENT);
        return bitmapSet;
    }

    protected Uri saveBitmapWithName(Bitmap tempBitmap, String name) throws FileNotFoundException,
            IOException {
        File destination = new File(activity.getCacheDir(), name);

        FileOutputStream out = new FileOutputStream(destination);
        tempBitmap.compress(Bitmap.CompressFormat.PNG, 100, out);
        out.flush();
        out.close();
        return Uri.fromFile(destination);
    }

    void saveState(Bundle bundle) {
        // TODO Auto-generated method stub

    }

    void restoreState(Bundle bundle) {
        // TODO Auto-generated method stub

    }

}
