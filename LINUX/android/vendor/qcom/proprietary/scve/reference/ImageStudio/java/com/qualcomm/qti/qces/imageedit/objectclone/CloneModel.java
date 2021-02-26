/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.imageedit.objectclone;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.HashMap;

import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Matrix;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Bundle;
import android.renderscript.Allocation;
import android.renderscript.RenderScript;
import android.util.Log;

import com.qualcomm.qti.qces.imageedit.objectclone.CloneController.HintType;
import com.qualcomm.qti.qces.stage.StageActivity;
import com.qualcomm.qti.qces.util.ImageSharingIntentExtras;

public class CloneModel {
    private CloneActivity activity;
    private static final String TAG = CloneModel.class.getSimpleName();

    public HashMap<HintType, Boolean> hintShown;
    public boolean hintActive;

    public ObjectCloner mObjectCloner;

    private RenderScript rs;
    private ScriptC_Blend scriptCBlend;
    private Allocation a_bitmap1;
    private Allocation a_blend_out;

    public Bitmap mBackground;
    public float blend_factor = 1;
    public Bitmap blend_output;
    public Bitmap finalMergeFull;
    public Matrix imageMatrix;
    protected Bitmap mBlend;

    private Canvas mergeCanvas;

    public CloneModel(CloneActivity activity) {
        this.activity = activity;
        hintShown = new HashMap<HintType, Boolean>();
        hintShown.put(HintType.FOREGROUND, false);
    }

    void saveState(Bundle state) {
        state.putBoolean("hintActive", hintActive);
        state.putFloat("blend_factor",blend_factor);
        state.putSerializable("hintMap", hintShown);
        state.putSerializable(ImageSharingIntentExtras.EXTRA_TARGET_IMAGE_URI, activity.getIntent()
                .getStringExtra(ImageSharingIntentExtras.EXTRA_TARGET_IMAGE_URI));

    }

    @SuppressWarnings("unchecked")
    void restoreState(Bundle state) {
        hintActive = state.getBoolean("hintActive");
        hintShown = (HashMap<HintType, Boolean>)state.getSerializable("hintMap");
        blend_factor = state.getFloat("blend_factor");
        Intent i = activity.getIntent();
        i.putExtra(ImageSharingIntentExtras.EXTRA_TARGET_IMAGE_URI,
                state.getSerializable(ImageSharingIntentExtras.EXTRA_TARGET_IMAGE_URI));
        activity.setIntent(i);
    }

    //for saving the foreground layer when we pass 2 layer information back to Stage
    public Bitmap getFGImage()
    {
        Bitmap finalFG = Bitmap.createBitmap(mBackground.getWidth(),mBackground.getHeight(), Bitmap.Config.ARGB_8888);
        Canvas testCanvas = new Canvas(finalFG);
        testCanvas.drawBitmap(blend_output, imageMatrix, null);
        return finalFG;
    }

    /**
     * Helper function to perform the actual blend operation
     */
    public void blendImages() {

        imageMatrix = new Matrix();
        activity.mBackgroundImageView.getImageMatrix().invert(imageMatrix);
        imageMatrix.preConcat(activity.mClippedImageView.getImageMatrix());
        imageMatrix.preTranslate(-activity.mClippedGlowingInflation[0], -activity.mClippedGlowingInflation[1]);

        //do SCVE clone/blend
        mBlend = mObjectCloner.clone(imageMatrix);

        //do a Renderscript user-controlled blend between pure segmentation overlay, SCVE blend, and background
        doRSKernelBlend(mBlend);

        finalMergeFull = mBackground.copy(Bitmap.Config.ARGB_8888, true);
        mergeCanvas = new Canvas(finalMergeFull);
        mergeCanvas.drawBitmap(blend_output, imageMatrix, null);

    }

    public void initRSKernelBlend(Bitmap input1)
    {
        rs = RenderScript.create(this.activity);
        scriptCBlend = new ScriptC_Blend(rs);

        a_bitmap1 = Allocation.createFromBitmap(rs, input1);
        blend_output = Bitmap.createBitmap(input1.getWidth(),
                input1.getHeight(), Bitmap.Config.ARGB_8888);
        a_blend_out = Allocation.createFromBitmap(rs, blend_output);

    }

    public Bitmap doRSKernelBlend(Bitmap bitmap2)
    {

        Allocation a_bitmap2 = Allocation.createFromBitmap(rs, bitmap2);

        scriptCBlend.set_blend_factor(blend_factor);

        a_bitmap1.syncAll(Allocation.USAGE_SCRIPT);
        a_bitmap2.syncAll(Allocation.USAGE_SCRIPT);
        a_blend_out.syncAll(Allocation.USAGE_SCRIPT);

        scriptCBlend.set_a_in2(a_bitmap2);
        scriptCBlend.forEach_root(a_bitmap1, a_blend_out);

        a_bitmap1.syncAll(Allocation.USAGE_SHARED);
        a_bitmap2.syncAll(Allocation.USAGE_SHARED);
        a_blend_out.syncAll(Allocation.USAGE_SHARED);

        return blend_output;
    }

    void generateFlattenedImage() {
        new SaveFlattenedImageTask().execute();
    }

    public void blendAndDraw() {
        doRSKernelBlend(mBlend);
        mergeCanvas.drawBitmap(blend_output, imageMatrix, null);
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

    private float[] storeCurrentImageMatrix() {

        float[] floatArray = new float[9+2];
        ObjectTransformation objTrans = activity.mClippedImageView.getObjTransformation();
        objTrans = activity.mClippedImageView.getCurrentOrientationToBitmapTransformedMatrix(objTrans);
        objTrans.getValues(floatArray);
        floatArray[9] = objTrans.getHorizontalFlipState()?1f:0f;
        floatArray[10] = objTrans.getVerticalFlipState()?1f:0f;
        return floatArray;
    }


    private class SaveFlattenedImageTask extends AsyncTask<Void, Void, Boolean> {

        private Intent stageActivityIntent = new Intent(activity, StageActivity.class);

        @Override
        protected void onPreExecute() {
            activity.showSpinner();
        }

        @Override
        protected Boolean doInBackground(Void... params) {

            try {

                Uri cloneResult = saveBitmapWithName(finalMergeFull,"clone.png");
                Uri cloneFG = saveBitmapWithName(getFGImage(),"cloneFG.png");

                //new target img is for stage's view
                stageActivityIntent.putExtra(ImageSharingIntentExtras.EXTRA_TARGET_IMAGE_URI,
                        cloneResult.toString());
                //new foreground img is for effects if we go from stage
                stageActivityIntent.putExtra(ImageSharingIntentExtras.FOREGROUND_IMAGE_URI,
                        cloneFG.toString());
                //background img is for effects and is the original full target
                stageActivityIntent.putExtra(ImageSharingIntentExtras.BACKGROUND_IMAGE_URI,
                        activity.getIntent().getStringExtra(ImageSharingIntentExtras.EXTRA_TARGET_IMAGE_URI));

                //if we go to stage then back to clone, we need to recreate object's position
                stageActivityIntent.putExtra(ImageSharingIntentExtras.CLONE_MATRIX, storeCurrentImageMatrix());
                //if we go to stage then back to clone, we'll need the precise mask again
                stageActivityIntent.putExtra(ImageSharingIntentExtras.EXTRA_MASK_IMAGE_URI, activity.getIntent().getStringExtra(ImageSharingIntentExtras.EXTRA_MASK_IMAGE_URI));
                //if we go to stage then back to clone, we'll need the object again
                stageActivityIntent.putExtra(ImageSharingIntentExtras.EXTRA_OBJECT_IMAGE_URI, activity.getIntent().getStringExtra(ImageSharingIntentExtras.EXTRA_OBJECT_IMAGE_URI));

                stageActivityIntent.putExtra(ImageSharingIntentExtras.FULL_MP_MODE, activity.isFullMP);

                return true;
            } catch (IOException e) {
                return false;
            }

        }

        @Override
        protected void onPostExecute(Boolean success) {
            if (success == false) {
                Log.e(TAG, "Failed to save images");
                // TODO: handle failure gracefully
                return;
            }

            // clear_top here will force segment and effects activities to close
            stageActivityIntent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);

            activity.startActivity(stageActivityIntent);
            activity.finish();

            activity.hideSpinner();
        }
    }

}
