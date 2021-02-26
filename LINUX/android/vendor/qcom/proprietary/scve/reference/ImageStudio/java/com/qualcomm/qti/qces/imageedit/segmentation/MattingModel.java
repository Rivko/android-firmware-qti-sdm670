/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.imageedit.segmentation;

import java.io.FileNotFoundException;
import java.io.IOException;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.ColorMatrixColorFilter;
import android.graphics.Paint;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Bundle;
import android.renderscript.Allocation;
import android.renderscript.RenderScript;
import android.util.Log;

import com.qualcomm.qti.qces.imageedit.jnilib.SegmentationStatusCode;
import com.qualcomm.qti.qces.imageedit.jnilib.SoftSegmentationJNILib;
import com.qualcomm.qti.qces.imageedit.segmentation.SegmentationBitmapManager.BitmapSet;
import com.qualcomm.qti.qces.util.ImageUtils;

public class MattingModel extends AbstractSegmentationModel {

    public static final String TAG = MattingModel.class.getSimpleName();

    private final MattingActivity activity;
    private RenderScript renderscript;
    private BitmapSet bitmapSet;
    private Uri maskUri;
    private Uri imgUri;

    public boolean hintShown;
    public boolean hintActive;
    public boolean helpActive;
    public StrokeSize strokeSizeSelected;

    private boolean isImageLoaded = false;

    public enum StrokeSize {
        SMALL, MEDIUM, LARGE
    };

    public MattingModel(AbstractSegmentationActivity activity) {
        super(activity);
        this.activity = (MattingActivity)activity;

        hintShown = false; // in matting we only have a single hint, so no hashmap used here
        hintActive = false;
        helpActive = false;
        strokeSizeSelected = StrokeSize.MEDIUM; // default size at first load
    }

    @Override
    void saveState(Bundle state) {
        state.putBoolean("hintActive", hintActive);
        state.putBoolean("hintShown", hintShown);
        state.putSerializable("strokeSize", strokeSizeSelected);
        state.putBoolean("helpActive", helpActive);
        activity.dismissHint();
    }

    @Override
    void restoreState(Bundle state) {
        hintActive = state.getBoolean("hintActive");
        hintShown = state.getBoolean("hintShown");
        strokeSizeSelected = (StrokeSize)state.getSerializable("strokeSize");
        helpActive = state.getBoolean("helpActive");
    }

    public void loadImageAndMask(Uri imageUri, Uri maskUri) {
        this.maskUri = maskUri;
        this.imgUri = imageUri;
        if (imageUri != null && maskUri != null) {
            bitmapSet = initializeImageBitmap(imageUri, activity.isFullMP);
            initializeSoftSegJNILib();
            initializeMaskBitmaps();
            renderscript = RenderScript.create(activity);
        }

        isImageLoaded = true;
    }

    public void resetMatting() {
        bitmapSet = initializeImageBitmap(imgUri, activity.isFullMP);
        initializeMaskBitmaps();
    }

    private void initializeMaskBitmaps() {
        Bitmap decodedInput;
        try {
            decodedInput = ImageUtils.loadImage(activity, maskUri, activity.isFullMP);

            bitmapSet.inputSegmentation.eraseColor(Color.TRANSPARENT);
            Canvas c = new Canvas(bitmapSet.inputSegmentation);
            c.drawBitmap(decodedInput, 0f, 0f, null);

            // Convert from the input mask (special values for FG, BG) to a real alpha mask. Note
            // that
            // this step cannot use ALPHA_8 bitmaps as the render target, as the
            // ColorMatrixColorFilter
            // does not work with ALPHA_8 targets.
            // N.B. foreground is not used at this point, use it as a temporary working buffer
            Bitmap intermediate = bitmapSet.foreground;
            intermediate.eraseColor(Color.TRANSPARENT);
            c.setBitmap(intermediate);
            Paint p = new Paint();
            // N.B. This color filter will map alpha values of 1 (ie MASK_BACKGROUND) to 0, and
            // alpha values of 2 (ie MASK_FOREGROUND) to 255. RGB values will be fixed to WHITE.
            p.setColorFilter(new ColorMatrixColorFilter(new float[] { 0f, 0f, 0f, 0f, 255f, 0f, 0f,
                    0f, 0f, 255f, 0f, 0f, 0f, 0f, 255f, 0f, 0f, 0f, 255f, -255f }));
            c.drawBitmap(decodedInput, 0f, 0f, p);

            bitmapSet.outputMask.eraseColor(Color.TRANSPARENT);
            c.setBitmap(bitmapSet.outputMask);
            p.setColorFilter(null);
            p.setColor(Color.WHITE);
            c.drawBitmap(intermediate, 0f, 0f, p);

            decodedInput.recycle();
        } catch (FileNotFoundException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }

    @Override
    void releaseImage() {
        super.releaseImage();

        if (renderscript != null) {
            renderscript.destroy();
            renderscript = null;
        }

        isImageLoaded = false;
    }

    void generateForegroundAndBackgroundImage() {
        new SaveForegroundBackgroundTask().execute();
    }

    Uri saveBackgroundBitmap(BitmapSet pBitmapSet) throws IOException {
        if (bitmapSet == null) {
            Log.e(TAG, "background bitmap not available");
            return null;
        }

        //use full image pre-seg as the saved background, because bitmapset.background
        //has the foreground object extruded from the background
        return saveBitmapWithName(pBitmapSet.source, "background.png");
    }

    Uri saveForegroundBitmap(BitmapSet pBitmapSet) throws IOException {
        if (bitmapSet == null) {
            Log.e(TAG, "foreground bitmap not available");
            return null;
        }

        return saveBitmapWithName(pBitmapSet.foreground, "foreground.png");
    }

    void processImageMatting() {
        if (getSoftSegmentationJNILib() != null && bitmapSet != null && renderscript != null) {
            new MattingBackgroundTask().execute();
        } else {
            Log.i(TAG,
                    "SoftSegmentationLib or bitmapSet is already released... do not start MattingBackgroundTask");
        }
    }

    void resetMask() {
        initializeMaskBitmaps();
    }

    private class MattingBackgroundTask extends AsyncTask<String, Void, Integer> {

        private SoftSegmentationJNILib softSegmentationJNILib;
        private BitmapSet localBitmapSet;
        private RenderScript localRenderScript;

        @Override
        protected void onPreExecute() {
            activity.showSpinner();
        }

        public MattingBackgroundTask() {
            // we assume that softSegmentationJNILib will always be valid
            softSegmentationJNILib = getSoftSegmentationJNILib();
            localBitmapSet = bitmapSet;
            localRenderScript = renderscript;
        }

        @Override
        protected Integer doInBackground(String... params) {
            int result = softSegmentationJNILib.processMattingImage(
                    localBitmapSet.inputSegmentation, localBitmapSet.outputMask,
                    localBitmapSet.foreground, localBitmapSet.background);

            if (result == SegmentationStatusCode.SUCCESS) {

            }

            return result;
        }

        @Override
        protected void onPostExecute(Integer result) {

            if (!isImageLoaded) {
                Log.i(TAG, "Image is not loaded or activity is destroyed. Do nothing and return");
                return;
            }
            if (result == SegmentationStatusCode.SUCCESS) {
                // Remove any transparency on the background, and replace it with alpha=255. This
                // will allow us to use PorterDuff.Mode.SRC_OVER to blend FG and BG layers.
                ScriptC_RemoveTransparency script_RemoveTransparency = new ScriptC_RemoveTransparency(
                        localRenderScript);
                Allocation a = Allocation.createFromBitmap(localRenderScript,
                        localBitmapSet.background);
                a.syncAll(Allocation.USAGE_SCRIPT);
                script_RemoveTransparency.forEach_root(a);
                a.syncAll(Allocation.USAGE_SHARED);
                a.destroy();

                activity.processSuccess();
                activity.invalidateMaskAlphaView();
            } else {
                Log.e(TAG, "Matting failed, err=" + result);
                activity.processFailed(result);
            }
        }
    }

    private class SaveForegroundBackgroundTask extends AsyncTask<String, Void, String> {
        private Uri foregroundUri;
        private Uri backgroundUri;
        private BitmapSet localBitmapSet;

        public SaveForegroundBackgroundTask() {
            localBitmapSet = bitmapSet;
        }

        @Override
        protected String doInBackground(String... params) {
            try {
                foregroundUri = saveForegroundBitmap(localBitmapSet);
                backgroundUri = saveBackgroundBitmap(localBitmapSet);
            } catch (IOException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
            return "Executed";
        }

        @Override
        protected void onPostExecute(String result) {
            if (!isImageLoaded) {
                Log.i(TAG, "Image is not loaded or activity is destroyed. Do nothing and return");
                return;
            }
            activity.foregroundBackgroundSeparationCompleted(foregroundUri, backgroundUri);
        }
    }

}
