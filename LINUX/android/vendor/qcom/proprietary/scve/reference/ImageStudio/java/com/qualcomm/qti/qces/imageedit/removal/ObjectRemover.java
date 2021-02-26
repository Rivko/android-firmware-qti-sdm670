/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.imageedit.removal;

import com.qualcomm.qti.qces.util.ImageUtils;

import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.os.Handler;
import android.util.Log;

/**
 * This class encapsulates the native Object Remove functionality in an object-oriented fashion. The
 * Object Remove algorithm operates on two pieces of data, a source image and a mask. The mask is
 * used to select the portion of the source image to be removed, and it is progressively eliminated
 * as the remove algorithm executes
 * @author slaver
 */
public class ObjectRemover {
    private Bitmap mProgressBitmap;
    private OnProgressListener mProgressListener;
    private Handler mUIThreadHandler = null;
    private boolean isFinished = false;
    private int nPatchSize = 14;
    long startTime;
    long stopTime;

    /**
     * This interface is used by the caller of
     * {@link ObjectRemover#asyncRemove(Handler, Bitmap, Bitmap, OnProgressListener)} to get
     * progress and completion information about the asynchronous remove operation
     */
    public interface OnProgressListener {
        /**
         * Called periodically whenever there is progress to report Note: this is always invoked on
         * the UI thread of the application
         * @param progress A Bitmap containing the latest image state, with the unprocessed portion
         *        of the image masked in green. This Bitmap is shared, and so should not be
         *        {@link Bitmap#recycle()}'d by the recipient.
         */
        public void onProgress(Bitmap progress);

        /**
         * Called when the object remove operation is complete. Note: this is always invoked on the
         * UI thread of the application
         * @param finished A bitmap containing the results of the object remove processing. This
         *        Bitmap is considered to be owned by the recipient, so it may be recycled when the
         *        recipient is finished with it.
         */
        public void onFinished(Bitmap finished);
    }

    /**
     * Creates a new instance of the ObjectRemover.
     * @param res App resources, from which the compiled OpenCL kernel for the remove operation will
     *        be extracted
     */
    public ObjectRemover(boolean useGpu, int patch_size) {
        nPatchSize = patch_size;
        RemovalJNILib.removalInit(ObjectRemover.this, useGpu);
    }

    public void removalCallBack(boolean finished, int left, int top, int right, int bottom) {

        isFinished = finished;

        runOnActivtyUiThreadSync(new Runnable() {
            @Override
            public void run() {
                if (false == ObjectRemover.this.isFinished) {
                    mProgressListener.onProgress(mProgressBitmap);
                } else {
                    stopTime = System.nanoTime();
                    System.out.println("exec time:");
                    System.out.println(stopTime - startTime);
                    mProgressListener.onFinished(mProgressBitmap);

                }
            }
        });

    }

    private void runOnActivtyUiThreadSync(final Runnable r) {
        synchronized (r) {
            mUIThreadHandler.post(new Runnable() {
                @Override
                public void run() {
                    synchronized (r) {
                        r.run();
                        r.notify();
                    }
                }
            });
        }
    }

    /**
     * Executes the async remove operation asynchronously. The caller will receive progress and
     * finished notifications via the progressListener (on the application UI thread). Only a single
     * async remove operation is allowed to run at a time; attempts to call again will throw a
     * {@link RuntimeException}
     * @param uiThreadHandler Handler on which to invoke the progressListener callbacks
     * @param src Source image on which to run the remove operation
     * @param removalStroke Mask image to select which part of source image is to be removed. The
     *        alpha channel in the mask image will select which parts of the source image are to be
     *        removed (alpha 0xFF indicates a portion to be removed, alpha < 0xFF (nominally 0) will
     *        be kept)
     * @param progressListener Callbacks to invoke on progress or completion.
     * @throws IllegalArgumentException if src, mask, or progressListener are null
     * @throws IllegalArgumentException if src and mask do not have the same dimensions
     * @throws RuntimeException if an instance of remove is already running
     */
    public void asyncRemove(final Handler uiThreadHandler, Bitmap src, Bitmap mask,
            final OnProgressListener progressListener) {

        if (src == null) {
            throw new IllegalArgumentException("src bitmap cannot be null");
        } else if (mask == null) {
            throw new IllegalArgumentException("mask bitmap cannot be null");
        } else if (progressListener == null) {
            throw new IllegalArgumentException("progressListener cannot be null");
        } else if (src.getWidth() != mask.getWidth() || src.getHeight() != mask.getHeight()) {
            throw new IllegalArgumentException(
                    "src and mask bitmaps must have identical dimensions");
        }

        mProgressListener = progressListener;
        mUIThreadHandler = uiThreadHandler;

        // here we basically create a new flattened bitmap to pass to object removal JNI call
        // first copy the background source
        mProgressBitmap = src.copy(Config.ARGB_8888, true);
        // create a working new canvas with this source
        Canvas c = new Canvas(mProgressBitmap);

        Paint paint = new Paint();
        paint.setColor(Color.GREEN);
        c.drawBitmap(mask.extractAlpha(), 0, 0, paint);

        ImageUtils.writeBitmapToPng(mask, "mask.png");

        // for debug
        // c.drawBitmap(BitmapFactory.decodeFile("/sdcard/testmask.png").extractAlpha(),0, 0,
        // paint);
        startTime = System.nanoTime();

        Log.v("ObjectRemover", "patch size = " + nPatchSize);

        RemovalJNILib.removalOperation(mProgressBitmap, mProgressBitmap, nPatchSize);
    }

    /**
     * Close the ObjectRemover instance and free all native resources associated.
     */
    public void close() {
        RemovalJNILib.removalStop();
        RemovalJNILib.removalDeInit();

        mProgressBitmap = null;
    }

}
