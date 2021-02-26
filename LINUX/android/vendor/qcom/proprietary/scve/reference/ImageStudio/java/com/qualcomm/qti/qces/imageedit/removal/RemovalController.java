/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.imageedit.removal;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

import android.app.DialogFragment;
import android.content.Intent;
import android.graphics.Bitmap;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;

import com.qualcomm.qti.qces.customui.HintDialogFragment;
import com.qualcomm.qti.qces.customui.HintDialogFragment.HintPosition;
import com.qualcomm.qti.qces.imageedit.removal.RemovalModel.StrokeSize;
import com.qualcomm.qti.qces.imageedit.softcut.R;
import com.qualcomm.qti.qces.stage.StageActivity;
import com.qualcomm.qti.qces.util.ImageSharingIntentExtras;

public class RemovalController {

    private static final String TAG = "ImgStudio";
    private static final String REMOVAL_FILE_NAME = "remove_flat.png";

    private RemovalActivity activity;
    private RemovalModel model;

    public RemovalController(RemovalActivity activity, RemovalModel model) {

        this.activity = (RemovalActivity)activity;
        this.model = (RemovalModel)model;

    }

    void onSaveInstanceState(Bundle bundle) {
        model.saveState(bundle);
    }

    void onRestoreInstanceState(Bundle bundle) {
        model.restoreState(bundle);
    }

    void initializeByImageUri(Uri removalImageUri) {
        activity.showSpinner();
        activity.refreshStrokeButtons();
        new LoadInputBitmapTask().execute(removalImageUri);
    }

    private class LoadInputBitmapTask extends AsyncTask<Uri, Void, Void> {

        @Override
        protected Void doInBackground(Uri... params) {
            model.loadImage(params[0]);
            return null;
        }

        @Override
        protected void onPostExecute(Void result) {
            if (model.getImageBitmap() != null) {

                activity.setBitmapToImageView(model.getImageBitmap());
                activity.setMaskAlphaBitmap(model.getMaskAlphaBitmap());

                activity.hideSpinner();
                activity.enableTouchMaskImageView(true);
                model.refreshUndoRedoBtns();

                dismissHint();

                if (model.hintShown == false || model.hintActive == true) {
                    HintDialogFragment objRemoveHintFragment = HintDialogFragment.newInstance(
                            HintPosition.TOP_TOAST,
                            activity.getResources().getString(R.string.hint_object_remove));
                    objRemoveHintFragment.show(activity.getFragmentManager(), "hint");
                    model.hintActive = true;
                    model.hintShown = true;
                }
            } else {
                Log.e(TAG, "Failed to load bitmap for obj removal");

            }
        }
    }

    public void dismissHint() {
        DialogFragment df = (DialogFragment)activity.getFragmentManager().findFragmentByTag("hint");
        if (df != null && df.getDialog() != null) {
            df.getDialog().dismiss();
        }
    }

    void cancelClicked() {

        activity.finish();

    }

    public void doneDrawingStroke(RemovalStroke stroke) {
        activity.showSpinner();
        model.processImageRemoval(stroke);
    }

    void smallStrokeClicked() {
        model.strokeSizeSelected = StrokeSize.SMALL;
        activity.refreshStrokeButtons();
    }

    void mediumStrokeClicked() {
        model.strokeSizeSelected = StrokeSize.MEDIUM;
        activity.refreshStrokeButtons();
    }

    void largeStrokeClicked() {
        model.strokeSizeSelected = StrokeSize.LARGE;
        activity.refreshStrokeButtons();
    }

    public void doneClicked() {
        new SaveTempImageTask().execute(activity.getRemovalBitmap());
    }

    private class SaveTempImageTask extends AsyncTask<Bitmap, Void, Uri> {
        @Override
        protected void onPreExecute() {
            activity.showSpinner();
        }

        @Override
        protected Uri doInBackground(Bitmap... params) {

            File destination = new File(activity.getCacheDir(), REMOVAL_FILE_NAME);

            try {
                FileOutputStream out = new FileOutputStream(destination);
                params[0].compress(Bitmap.CompressFormat.PNG, 100, out);
                out.flush();
                out.close();
            } catch (IOException e) {
                Log.e(TAG, "Failed writing flat image to temporary storage", e);
                return null;
            }

            params[0].recycle();
            params[0] = null;
            return Uri.fromFile(new File(destination.getAbsolutePath()));

        }

        @Override
        protected void onPostExecute(Uri uri_result) {
            if (uri_result == null) {
                Log.e(TAG, "Failed to save temp flattened effects image");
                // TODO: handle failure gracefully
                return;
            }

            Intent nextActivity = new Intent(activity, StageActivity.class);
            nextActivity.putExtra(ImageSharingIntentExtras.EXTRA_TARGET_IMAGE_URI,
                    uri_result.toString());
            nextActivity.putExtra(ImageSharingIntentExtras.FULL_MP_MODE, activity.isFullMP);
            activity.startActivity(nextActivity);
            activity.finish();

            activity.hideSpinner();
        }
    }

    public void undoClicked() {
        model.getHistoryImageForUndo();
    }

    void redoClicked() {
        model.getHistoryImageForRedo();
    }

}
