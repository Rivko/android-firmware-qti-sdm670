/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.imageedit.segmentation;

import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.BitmapShader;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.ColorMatrixColorFilter;
import android.graphics.Paint;
import android.graphics.PorterDuff;
import android.graphics.PorterDuffXfermode;
import android.graphics.Shader;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;

import com.qualcomm.qti.qces.imageedit.jnilib.SegmentationStatusCode;
import com.qualcomm.qti.qces.imageedit.jnilib.SoftSegmentationJNILib;
import com.qualcomm.qti.qces.imageedit.segmentation.HardCutController.HintType;
import com.qualcomm.qti.qces.imageedit.segmentation.SegmentationBitmapManager.BitmapSet;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;

public class HardCutModel extends AbstractSegmentationModel {

    private static final String TAG = HardCutModel.class.getSimpleName();

    private HardCutActivity activity;
    private boolean isImageLoaded = false;

    public HashMap<HintType, Boolean> hintShown;
    public boolean hintActive;
    public boolean helpActive;
    public boolean previewEnabled = false;

    public Bundle softCutStateBundle;

    public boolean undoClicked = false;
    public boolean redoClicked = false;

    private ArrayList<String> storedMaskPaths;
    private int scaledWidth;
    private int scaledHeight;

    public HardCutModel(AbstractSegmentationActivity activity) {
        super(activity);
        this.activity = (HardCutActivity) activity;
        hintShown = new HashMap<HintType, Boolean>();
        hintShown.put(HintType.FOREGROUND, false);
        hintShown.put(HintType.BACKGROUND_AWARE, false);
        hintShown.put(HintType.BACKGROUND, false);
        hintShown.put(HintType.ENHANCE_EDGES, false);
        hintActive = false;
        helpActive = false;

        storedMaskPaths = new ArrayList<String>();
    }

    @Override
    void saveState(Bundle state) {

        state.putStringArrayList("maskPaths", storedMaskPaths);
        state.putInt("scaledWidth", scaledWidth);
        state.putInt("scaledHeight", scaledHeight);
        state.putBoolean("previewEnabled", previewEnabled);
        state.putBoolean("hintActive", hintActive);
        state.putSerializable("hintMap", hintShown);
        state.putBoolean("helpActive", helpActive);
    }

    @SuppressWarnings("unchecked")
    @Override
    void restoreState(Bundle state) {

        storedMaskPaths = state.getStringArrayList("maskPaths");
        scaledWidth = state.getInt("scaledWidth");
        scaledHeight = state.getInt("scaledHeight");
        previewEnabled = state.getBoolean("previewEnabled");
        hintActive = state.getBoolean("hintActive");
        hintShown = (HashMap<HintType, Boolean>) state.getSerializable("hintMap");
        helpActive = state.getBoolean("helpActive");
    }

    void loadImage(Uri imageUri) {
        if (imageUri != null) {
            initializeImageBitmap(imageUri, activity.isFullMP);
            initializeSoftSegJNILib();
        }

        isImageLoaded = true;
    }

    void generateForegroundAndBackgroundImage() {
        if (bitmapSet != null) {
            new SeparateForegroundBackgroundTask().execute("forSave");
        } else {
            Log.d(TAG,
                    "bitmapSet is already released... don't start SeparateForegroundBackgroundTask");
        }

    }

    void processImageSegmentation() {
        if (getSoftSegmentationJNILib() != null && bitmapSet != null) {
            new SegmentationBackgroundTask().execute();
        } else {
            Log.d(TAG,
                    "bitmapSet or softSegmentationJNILib is already released... don't start SegmentationBackgroundTask");
        }
    }

    Uri saveHardCutOutput() throws IOException {
        if (bitmapSet == null) {
            Log.e(TAG, "No hardcut output bitmap to save");
            return null;
        }

        return saveBitmapWithName(activity.getOutputMaskBitmap().copy(Config.ARGB_8888, true),
                "hardcut_mask.png");
    }

    Uri saveBackgroundBitmap(BitmapSet pBitmapSet) throws IOException {
        if (pBitmapSet == null) {
            Log.e(TAG, "No background bitmap to save");
            return null;
        }

        //use full image pre-seg as the saved background, because bitmapset.background
        //has the foreground object extruded from the background
        return saveBitmapWithName(pBitmapSet.source, "background.png");
    }

    Uri saveForegroundBitmap(BitmapSet pBitmapSet) throws IOException {
        if (pBitmapSet == null) {
            Log.e(TAG, "No foreground bitmap to save");
            return null;
        }

        return saveBitmapWithName(pBitmapSet.foreground, "foreground.png");
    }

    @Override
    void releaseImage() {
        super.releaseImage();
        isImageLoaded = false;
    }

    private void downscaleAndPersistMaskToStorage() {
        Bitmap pMask = copyMaskToDownScaled();
        String fName = "pMask_" + "stroke_" + activity.getStrokeCount() + ".bin";
        String fullPath = Util.saveBitmapBytesToFile(activity, fName, pMask);
        storedMaskPaths.add(fullPath);
    }

    public Bitmap recreatePreviousMaskFromStorage(int index) {

        String path = storedMaskPaths.get(index);
        File file = new File(path);
        byte[] bitmapData = new byte[(int) file.length()];
        try {
            BufferedInputStream buf = new BufferedInputStream(new FileInputStream(file));
            buf.read(bitmapData, 0, bitmapData.length);
            buf.close();
        } catch (FileNotFoundException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }

        //copy saved scaled down byte array to a scaled down Alpha8 bitmap
        Bitmap mask = Bitmap.createBitmap(scaledWidth, scaledHeight, Config.ALPHA_8);
        Alpha8JNILib.FillAlphaBitmapWithByteArray(bitmapData, mask);

        //upscale if necessary
        if (getScaleRate() != 1) {
            mask = Bitmap.createScaledBitmap(mask,
                    bitmapSet.outputMask.getWidth(),
                    bitmapSet.outputMask.getHeight(), false);
        }

        return mask;
    }

    private class SegmentationBackgroundTask extends AsyncTask<Void, Void, Void> {

        private int segmentationResult;
        private SoftSegmentationJNILib softSegmentationJNILib;
        private BitmapSet localBitmapSet;

        public SegmentationBackgroundTask() {
            // we assume that softSegmentationJNILib will always be valid
            softSegmentationJNILib = getSoftSegmentationJNILib();
            localBitmapSet = bitmapSet;

        }

        @Override
        protected Void doInBackground(Void... params) {

            //Util.printBitmap("inputSegmentation before softcut", localBitmapSet.inputSegmentation);

            int strokeCnt = activity.getStrokeCount();

            Bitmap resultBitmap;

            if (undoClicked || redoClicked) {

                resultBitmap = recreatePreviousMaskFromStorage(strokeCnt-1);
                activity.setOutputMaskBitmap(resultBitmap);
            }
            else {

                //if here then we're processing a normal stroke, not an undo or redo
                //creating a new node

                if (strokeCnt == 1)
                    activity.getOutputMaskBitmap().eraseColor(0x01010101); //set to all BG at first

                if (strokeCnt > storedMaskPaths.size())
                { //for new strokes at the top of undo redo stack, prevMask is internal to algo so pass in null
                    segmentationResult = softSegmentationJNILib.processSegmentationImage(
                            localBitmapSet.inputSegmentation, localBitmapSet.outputMask, null);
                } //for new strokes within the undo redo stack, prevMask is unknown to algo so we pass it in
                else {
                    segmentationResult = softSegmentationJNILib.processSegmentationImage(
                            localBitmapSet.inputSegmentation, localBitmapSet.outputMask, activity.getOutputMaskBitmap());
                }

                //if there are more masks than the current stroke index, remove them
                while (storedMaskPaths.size() > strokeCnt - 1)
                    storedMaskPaths.remove(storedMaskPaths.size() - 1);

                //save the processed output mask from the algo
                downscaleAndPersistMaskToStorage();

                activity.setOutputMaskBitmap(localBitmapSet.outputMask);

            }
            return null;
        }

        @Override
        protected void onPostExecute(Void params) {

            undoClicked = false;
            redoClicked = false;

            activity.refreshOutputMaskBitmap();
            activity.setPreviewButtonEnable(true);

            if (!isImageLoaded) {
                Log.i(TAG, "Activity is null or destroyed after segmentation complete");
                return;
            }
            if (segmentationResult == SegmentationStatusCode.SUCCESS) {
                //Util.printBitmap("outputMask after hard cut", localBitmapSet.outputMask);
                activity.processSuccess();
            } else {
                activity.processFailed(segmentationResult);
            }
        }
    }

    private Bitmap copyMaskToDownScaled() {

        int downScale = getScaleRate();

        scaledWidth = bitmapSet.outputMask.getWidth() / downScale;
        scaledHeight = bitmapSet.outputMask.getHeight() / downScale;

        //always save the output of the algo
        return Bitmap.createScaledBitmap(bitmapSet.outputMask,
                scaledWidth, scaledHeight, false);

    }

    private int getScaleRate() {

        int numPixels = bitmapSet.outputMask.getWidth() *
                bitmapSet.outputMask.getHeight();
        int scaleRate = 1;

        if (numPixels <= 1500000)
            scaleRate = 1;
        else if (numPixels <= 6500000)
            scaleRate = 2;
        else if (numPixels <= 10500000)
            scaleRate = 3;
        else if (numPixels <= 16500000)
            scaleRate = 4;
        else if (numPixels <= 20500000)
            scaleRate = 5;

        return scaleRate;
    }

    public void separateForegroundForPreview() {

        new SeparateForegroundBackgroundTask().execute("forPreview");

    }

    private class SeparateForegroundBackgroundTask extends AsyncTask<String, Void, String> {

        private Uri foregroundUri;
        private Uri backgroundUri;
        private BitmapSet localBitmapSet;

        public SeparateForegroundBackgroundTask() {
            localBitmapSet = bitmapSet;
        }

        @Override
        protected String doInBackground(String... params) {

            Paint outputMaskPaint = new Paint();

            Bitmap activeBitmap = activity.getOutputMaskBitmap();

            outputMaskPaint.setShader(new BitmapShader(activeBitmap,
                    Shader.TileMode.REPEAT, Shader.TileMode.REPEAT));

            // N.B. This color filter will map alpha values of 1 (ie MASK_BACKGROUND) to 0, and
            // alpha values of 2 (ie MASK_FOREGROUND) to 255. RGB values will be fixed to WHITE.
            outputMaskPaint.setColorFilter(new ColorMatrixColorFilter(new float[]{0f, 0f, 0f, 0f,
                    255f, 0f, 0f, 0f, 0f, 255f, 0f, 0f, 0f, 0f, 255f, 0f, 0f, 0f, 255f, -255f}));

            Paint xferPaint = new Paint();

            localBitmapSet.foreground.eraseColor(Color.TRANSPARENT);

            Canvas c = new Canvas(localBitmapSet.foreground);
            c.drawPaint(outputMaskPaint);
            xferPaint.setXfermode(new PorterDuffXfermode(PorterDuff.Mode.SRC_IN));
            c.drawBitmap(localBitmapSet.source, 0f, 0f, xferPaint);

            //added this eraseColor to fix problem with subsequent calls drawBitmap
            localBitmapSet.background.eraseColor(Color.TRANSPARENT);

            c.setBitmap(localBitmapSet.background);
            c.drawPaint(outputMaskPaint);
            xferPaint.setXfermode(new PorterDuffXfermode(PorterDuff.Mode.SRC_OUT));
            c.drawBitmap(localBitmapSet.source, 0f, 0f, xferPaint);

            //if boolean set to true for this task then save the Bitmaps
            if (params[0].equals("forSave")) {
                try {
                    foregroundUri = saveForegroundBitmap(localBitmapSet);
                    backgroundUri = saveBackgroundBitmap(localBitmapSet);
                } catch (IOException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                }

            }
            return params[0];
        }

        @Override
        protected void onPostExecute(String result) {
            if (!isImageLoaded) {
                Log.i(TAG,
                        "SeparateForegroundBackgroundTask Activity is null or destroyed after separate foreground from background complete");
                return;
            }

            if (result.equals("forSave"))
                activity.foregroundBackgroundSeparationCompleted(foregroundUri, backgroundUri);
            else if (result.equals("forPreview"))
                activity.doSegmentationPreview(localBitmapSet.foreground, localBitmapSet.background);

        }
    }
}
