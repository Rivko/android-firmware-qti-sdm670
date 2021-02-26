/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.imageedit.segmentation;

import android.app.DialogFragment;
import android.content.Intent;
import android.content.SharedPreferences;
import android.graphics.Bitmap;
import android.net.Uri;
import android.os.AsyncTask;

import com.qualcomm.qti.qces.customui.HardCutCancelDialogFragment;
import com.qualcomm.qti.qces.customui.HintDialogFragment;
import com.qualcomm.qti.qces.customui.HintDialogFragment.HintPosition;
import com.qualcomm.qti.qces.imageedit.softcut.R;
import com.qualcomm.qti.qces.util.ImageSharingIntentExtras;
import com.qualcomm.qti.qces.walkthrough.SlideshowFragmentActivity;

import java.io.IOException;

public class HardCutController extends AbstractSegmentationController {

    @SuppressWarnings("unused")
    private static final String TAG = HardCutController.class.getSimpleName();

    private static final String dontShowAutomatically = "DontShowHelpAutoSegmentation";

    private HardCutActivity activity;
    private HardCutModel model;
    private Uri imageUri;

    public enum HintType {
        FOREGROUND, BACKGROUND_AWARE, BACKGROUND, ENHANCE_EDGES
    };

    public HardCutController(AbstractSegmentationActivity activity, AbstractSegmentationModel model) {
        super(activity, model);

        this.activity = (HardCutActivity)activity;
        this.model = (HardCutModel)model;

    }

    @Override
    void onStart() {
        /*
         * foregroundClicked() or backgroundClicked() in onResume will call showHint(FOREGROUND) any
         * ways, so commenting for now, to be removed later
         */
        // showHint(HintType.FOREGROUND);
    }

    void onResume() {
        /*
         * To restore the Segmentation state in case of Orientation changes
         */
        switch (activity.getSegmentationState()) {
        case FOREGROUND:
            foregroundClicked();
            break;
        case BACKGROUND:
            backgroundClicked();
            break;
        default:
            break;
        }
    }

    void setupMaskView() {
        activity.showMaskView();
        activity.enableTouchMaskImageView();
    }

    @Override
    void cancelClicked() {
        if (activity.getStrokeCount() > 0) {
            HardCutCancelDialogFragment newCancelFragment = new HardCutCancelDialogFragment();
            newCancelFragment.show(activity.getFragmentManager(), "cancel");
        } else {
            activity.finish();
        }
    }

    @Override
    void redoClicked() {
        if (activity.getUndoStrokeCount() > 0) {
            model.clearMask();
            super.redoClicked();
            model.redoClicked = true;
            processImageIfNecessary();
            restoreFgndBkgndStates(activity.peekStrokesSegmentationState());
        }

    }

    @Override
    void undoClicked() {
        if (activity.getStrokeCount() > 0) {
            model.clearMask();
            super.undoClicked();

            if ( activity.getStrokeCount() == 0)
            {
            // hide the red BG mask visible to the user if there aren't enough strokes
                activity.hideOutputMaskView();
                activity.disableDoneButton();
                activity.disableEnhanceEdgesBtn();
                activity.setPreviewButtonEnable(false);
            } else {
                model.undoClicked = true;

                processImageIfNecessary();

            }

            restoreFgndBkgndStates(activity.peekUndoStrokesSegmentationState());
        }
    }


    /*
     * This function will be called at the end of an undo and/or redo, and hence will update the
     * foreground/background buttons and segmentation state to the correct state
     */
    void restoreFgndBkgndStates(int segmentationState) {
        switch (segmentationState) {
        case Stroke.FOREGROUND:
            foregroundClicked();
            break;
        case Stroke.BACKGROUND:
            backgroundClicked();
            break;
        default:
            break;
        }
    }

    void imageSelected(Uri imageUri) {
        this.imageUri = imageUri;
        showSpinner();
        new LoadBitmapBackgroundTask().execute();
    }

    void enhanceEdgesClicked() {
        showSpinner();
        new LaunchMattingBackgroundTask().execute();
    }

    void backgroundClicked() {
        activity.setBackgroundButtonActive();
        activity.setSegmentationState(SegmentationState.BACKGROUND);
        showHint(HintType.BACKGROUND);
    }

    void foregroundClicked() {
        activity.setForegroundButtonActive();
        activity.setSegmentationState(SegmentationState.FOREGROUND);
        showHint(HintType.FOREGROUND);
    }

    public void previewClicked() {
        if (model.bitmapSet != null) {
            activity.setPreviewButtonEnable(false);
            model.separateForegroundForPreview();
        }
    }

    void showHint(HintType hintType) {
        DialogFragment newHintFragment = null;

        dismissHint(); // dismiss any previous hints still on the screen

        switch (hintType) {
        case FOREGROUND:
            if (activity.getForegroundStrokesCount() == 0) {
                newHintFragment = HintDialogFragment.newInstance(HintPosition.TOP_TOAST, activity
                        .getResources().getString(R.string.hint_foreground));
            }
            break;
        case BACKGROUND_AWARE:
            if (model.hintShown.get(hintType) == false) {
                newHintFragment = HintDialogFragment.newInstance(HintPosition.TOP_BUBBLE, activity
                        .getResources().getString(R.string.hint_aware_background));
            }
            break;
        case BACKGROUND:
            if (activity.getBackgroundStrokesCount() == 0) {
                newHintFragment = HintDialogFragment.newInstance(HintPosition.TOP_TOAST, activity
                        .getResources().getString(R.string.hint_background));
            }
            break;
        case ENHANCE_EDGES:
            if (model.hintShown.get(hintType) == false) {
                newHintFragment = HintDialogFragment.newInstance(HintPosition.BOTTOM_BUBBLE,
                        activity.getResources().getString(R.string.hint_enhance_edges));
            }
            break;
        }

        if (newHintFragment != null) {
            newHintFragment.show(activity.getFragmentManager(), "hint");
            model.hintActive = true;
            model.hintShown.put(hintType, true);
        }

    }

    void dismissHint() {
        DialogFragment df = (DialogFragment)activity.getFragmentManager().findFragmentByTag("hint");
        if (df != null && df.getDialog() != null) {
            df.getDialog().dismiss();
            model.hintActive = false;
        }
    }

    @Override
    void doneDrawingStroke(float x, float y) {
        super.doneDrawingStroke(x, y);

        processImageIfNecessary();

        if (activity.getForegroundStrokesCount() == 1 && activity.getBackgroundStrokesCount() == 0)
            showHint(HintType.BACKGROUND_AWARE);

    }

    void processImageIfNecessary() {
        int foregroundStrokesCount = activity.getForegroundStrokesCount();

        if ( foregroundStrokesCount >= 1) {
            activity.enableEnhanceEdgesBtn();
            activity.enableDoneButton();
            showSpinner();
            model.processImageSegmentation();
        }
    }

    void refreshHardCutSpecificUIButtons(){
        int foregroundStrokesCount = activity.getForegroundStrokesCount();

        if ( foregroundStrokesCount >= 1) {
            activity.enableEnhanceEdgesBtn();
            activity.enableDoneButton();
        }
    }

    private class LoadBitmapBackgroundTask extends AsyncTask<String, Void, String> {

        @Override
        protected void onPreExecute() {
            activity.setImageBitmap(null);
            activity.setMaskAlphaBitmap(null);
            activity.setOutputMaskBitmap(null);
            activity.hideOutputMaskView();
            activity.disableEnhanceEdgesBtn();
            activity.disableDoneButton();
        }

        @Override
        protected String doInBackground(String... params) {
            model.loadImage(imageUri);
            return "Executed";
        }

        @Override
        protected void onPostExecute(String result) {
            resetViewBitmaps();
            hideSpinner();
            refreshUndoRedoEnables();
            // below was added to take care of mask recreation on activity recreation for UI
            int strokeCnt = activity.getStrokeCount();

            if (strokeCnt > 0) {
                Bitmap currentMask = model.recreatePreviousMaskFromStorage(strokeCnt - 1);
                activity.setOutputMaskBitmap(currentMask);
                activity.refreshOutputMaskBitmap();
                refreshHardCutSpecificUIButtons();
                activity.updateOutputMask();
                activity.redrawStrokes();
            }

        }
    }

    @Override
    void helpClicked() {

        Intent intent = new Intent(activity, SlideshowFragmentActivity.class);
        activity.startActivity(intent);
        model.helpActive = true;

    }

    @Override
    void doneClicked() {
        super.doneClicked();
        model.generateForegroundAndBackgroundImage();
    }

    @Override
    void processSuccess() {
        super.processSuccess();
        activity.updateOutputMask();
        showHint(HintType.ENHANCE_EDGES);
    }

    private class LaunchMattingBackgroundTask extends AsyncTask<String, Void, String> {

        private Intent enhanceEdgesIntent = new Intent(activity.getApplicationContext(),
                MattingActivity.class);

        @Override
        protected String doInBackground(String... params) {
            try {
                showSpinner();
                Uri outputMaskFileUri = model.saveHardCutOutput();
                enhanceEdgesIntent.putExtra(ImageSharingIntentExtras.EXTRA_TARGET_IMAGE_URI,
                        HardCutController.this.imageUri.toString());
                enhanceEdgesIntent.putExtra(ImageSharingIntentExtras.EXTRA_MASK_IMAGE_URI,
                        outputMaskFileUri.toString());
                enhanceEdgesIntent.putExtra("softCutStateBundle", model.softCutStateBundle);
                enhanceEdgesIntent.putExtra(ImageSharingIntentExtras.NEXT_ACTIVITY,
                        activity.nextActivity);
                enhanceEdgesIntent.putExtra(ImageSharingIntentExtras.FULL_MP_MODE,
                        activity.isFullMP);
            } catch (IOException e) {
                e.printStackTrace();
                // TODO: create a a common error reporting.
            }
            return "Executed";
        }

        @Override
        protected void onPostExecute(String result) {
            activity.startActivityForResult(enhanceEdgesIntent, 1);
        }
    }

    public void checkShowHelp(SharedPreferences prefs) {
        if (prefs.getBoolean(dontShowAutomatically, true) == false && model.helpActive == false) {
            helpClicked();
        }
    }

}
