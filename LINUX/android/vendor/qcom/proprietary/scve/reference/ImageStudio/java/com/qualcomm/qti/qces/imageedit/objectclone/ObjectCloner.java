/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.imageedit.objectclone;

import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Matrix;

/**
 * This class encapsulates the native Object Clone functionality in an object-oriented fashion. In
 * principle, there are 2 pieces of data needed for the clone, the unchanging data and the changing
 * data. The unchanging data is the object that is being cloned, and the changing data is the target
 * image that the object is being added into.
 * @author slaver
 */
public final class ObjectCloner {

    private final Bitmap mTarget;
    private final Bitmap mObject;
    private final int nBorderPixels;

    private Bitmap mOutputTarget; // chunk of target that is the size of the object
    private boolean mClosed = false;

    /**
     * Create an ObjectCloner for the given set of images. To change the images being operated on, a
     * new ObjectCloner instance needs to be created.
     * @param res App resources, from which the compiled OpenCL kernel for the clone operation will
     *        be extracted
     * @param target Destination image into which the object image will be cloned
     * @param object The object image to be cloned
     * @param mask A mask image, with the same dimensions as the object image, to select which parts
     *        of the object image should be cloned. The alpha channel controls the masking; any part
     *        of the image with alpha 0xFF will be cloned, and any part with alpha < 0xFF (nominally
     *        0) will not be cloned
     */
    public ObjectCloner(Resources res, Bitmap target, Bitmap object, Bitmap mask, int borderPixels, boolean useGpu) {
        if (target == null || object == null || mask == null) {
            throw new IllegalArgumentException("All Bitmaps must be non-null");
        } else if (object.getWidth() != mask.getWidth() || object.getHeight() != mask.getHeight()) {
            throw new IllegalArgumentException("Object and Mask bitmaps must be the same size");
        }

        mTarget = target;
        mObject = object;

        nBorderPixels = borderPixels;

        // Just in case, release any outstanding instance before initializing
        CL2JNILib.cloningDeinit();
        CL2JNILib.cloningInit(useGpu);
        CL2JNILib.cloningInitImage(mObject, mask, false);

        // Create a bitmap for the piece of the target image that matches the output object image
        // in size. It will be used as the working memory for the algorithm
        mOutputTarget = Bitmap.createBitmap(mObject.getWidth(), mObject.getHeight(),
                Config.ARGB_8888);
    }

    /**
     * Apply the clone operation to blend the object image into the background, with the object
     * position and scale transformed by the objectTranslateScale matrix
     * @param objectTranslateScale A matrix which will be applied (nominally) to the object to
     *        position and scale it over the target before cloning. In fact, the inverse of the
     *        matrix is applied to the background image, to keep the roles of unchanging and
     *        changing data for the OpenCL algorithm.
     * @return A new Bitmap, of the same dimensions as the target image, which is the result of the
     *         clone operation
     */
    public Bitmap clone(Matrix objectTranslateScale) {
        if (mClosed) {
            throw new RuntimeException("ObjectCloner is closed, not valid for use");
        }

        Matrix backgroundTranslateScale = new Matrix();
        objectTranslateScale.invert(backgroundTranslateScale);

        backgroundTranslateScale.preTranslate((float)-nBorderPixels, (float)-nBorderPixels); //translate according to the border replication offset

        // Select the target region (the "changing" region) that is overlapped by the object
        mOutputTarget.eraseColor(Color.TRANSPARENT);

        Canvas c = new Canvas(mOutputTarget);
        c.drawBitmap(mTarget, backgroundTranslateScale, null);

        CL2JNILib.cloningOperation(mOutputTarget);

        return mOutputTarget;

    }

    /**
     * Close the ObjectCloner instance and free all native resources associated. This will
     * eventually be invoked by {@link #finalize()}, however if you know you are done with cloning
     * you can manually invoke this to clean up sooner.
     */
    public void close() {
        if (!mClosed) {
            mClosed = true;

            if (mOutputTarget != null) {
                mOutputTarget.recycle();
                mOutputTarget = null;
            }

            //CL2JNILib.cloningDeinit();
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void finalize() throws Throwable {
        this.close();
        super.finalize();
    }
}
