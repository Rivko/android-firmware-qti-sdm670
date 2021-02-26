/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.imageedit.segmentation;

import android.app.DialogFragment;
import android.content.Intent;
import android.content.SharedPreferences;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Bundle;

import com.qualcomm.qti.qces.customui.HintDialogFragment;
import com.qualcomm.qti.qces.customui.HintDialogFragment.HintPosition;
import com.qualcomm.qti.qces.customui.MattingCancelDialogFragment;
import com.qualcomm.qti.qces.imageedit.segmentation.MattingModel.StrokeSize;
import com.qualcomm.qti.qces.imageedit.softcut.R;
import com.qualcomm.qti.qces.walkthrough.SlideshowFragmentActivity;

public class MattingController extends AbstractSegmentationController {

    private static final String dontShowAutomatically = "DontShowHelpAutoSegmentation";

    private MattingActivity activity;
    private MattingModel model;

    public MattingController(AbstractSegmentationActivity activity, AbstractSegmentationModel model) {
        super(activity, model);

        this.activity = (MattingActivity)activity;
        this.model = (MattingModel)model;

    }

    void initialize(Uri imageUri, Uri maskUri) {
        activity.showSpinner();
        activity.refreshStrokeButtons();
        new LoadBitmapBackgroundTask().execute(imageUri, maskUri);
    }

    void cancelClicked() {
        Intent returnIntent = new Intent();
        Bundle state = new Bundle();
        model.saveState(state);
        returnIntent.putExtra("softCutStateBundle", state);

        activity.setResult(0, returnIntent);

        // MattingCancelDialogFragment will call finish on this activity if user confirms exit
        if (activity.getStrokeCount() > 0) {
            MattingCancelDialogFragment newCancelFragment = new MattingCancelDialogFragment();
            newCancelFragment.show(activity.getFragmentManager(), "cancel");
        } else {
            activity.finish();
        }
    }

    @Override
    void doneClicked() {
        super.doneClicked();
        model.generateForegroundAndBackgroundImage();
    }

    @Override
    void doneDrawingStroke(float x, float y) {
        super.doneDrawingStroke(x, y);
        activity.showSpinner();
        model.processImageMatting();
    }

    void hideSpinner() {
        activity.hideSpinner();
    }

    @Override
    void processSuccess() {
        super.processSuccess();
        activity.setMaskAlphaBitmap(model.getMaskAlphaBitmap());
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

    @Override
    void redoClicked() {
        if (activity.getUndoStrokeCount() > 0) {
            model.resetMask();
            super.redoClicked();
            model.processImageMatting();
        }
    }

    @Override
    void undoClicked() {
        if (activity.getStrokeCount() > 0) {
            if (activity.getStrokeCount() == 0) // we've gone from 1 to zero strokes
            {
                super.undoClicked();
                activity.showSpinner();
                model.resetMatting();
                resetViewBitmaps();
                activity.hideSpinner();
            } else {
                model.resetMask();
                super.undoClicked();
                model.processImageMatting();
            }
        }
    }

    @Override
    void helpClicked() {
        Intent intent = new Intent(activity, SlideshowFragmentActivity.class);
        intent.putExtra("first_slide", 3);
        activity.startActivity(intent);
        model.helpActive = true;
    }

    void showHint() {
        if (model.hintShown == false || model.hintActive == true) {
            dismissHint(); // if already there, remove it
            DialogFragment newHintFragment = HintDialogFragment.newInstance(HintPosition.TOP_TOAST,
                    activity.getResources().getString(R.string.hint_enhance_screen));

            newHintFragment.show(activity.getFragmentManager(), "hint");
            model.hintActive = true;
            model.hintShown = true;
        }
    }

    void dismissHint() {

        if (model.hintActive) {

            DialogFragment df = (DialogFragment)activity.getFragmentManager().findFragmentByTag(
                    "hint");
            if (df != null) {
                df.getDialog().dismiss();
                model.hintActive = false;
            }
        }

    }

    private class LoadBitmapBackgroundTask extends AsyncTask<Uri, Void, String> {

        @Override
        protected String doInBackground(Uri... params) {
            model.loadImageAndMask(params[0], params[1]);
            return "Executed";
        }

        @Override
        protected void onPostExecute(String result) {

            resetViewBitmaps();
            activity.setSegmentationState(SegmentationState.SOFT_EDGES);
            activity.enableTouchMaskImageView();
            activity.hideSpinner();
            activity.showHint();
            // 3 below for re-creation of stroke state
            refreshUndoRedoEnables();
            activity.redrawStrokes();
            model.processImageMatting();
        }
    }

    void setStateFromBundle(Bundle bundleExtra) {
        if (bundleExtra != null)
            model.restoreState(bundleExtra);
    }

    public void checkShowHelp(SharedPreferences prefs) {
        if (prefs.getBoolean(dontShowAutomatically, true) == false && model.helpActive == false) {
            helpClicked();
        }
    }

}
