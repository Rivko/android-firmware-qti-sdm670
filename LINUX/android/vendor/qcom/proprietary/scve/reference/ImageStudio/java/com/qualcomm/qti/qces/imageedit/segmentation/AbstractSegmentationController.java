/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.imageedit.segmentation;

import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;

import com.qualcomm.qti.qces.imageedit.effects.EffectsActivity;
import com.qualcomm.qti.qces.imageedit.objectclone.CloneActivity;
import com.qualcomm.qti.qces.util.ImageSharingIntentExtras;

public abstract class AbstractSegmentationController {

    @SuppressWarnings("unused")
    private static final String TAG = AbstractSegmentationController.class.getSimpleName();

    private AbstractSegmentationActivity activity;
    private AbstractSegmentationModel model;

    public AbstractSegmentationController(AbstractSegmentationActivity activity,
            AbstractSegmentationModel model) {
        this.activity = activity;
        this.model = model;
    }

    void resetViewBitmaps() {
        activity.setImageBitmap(model.getImageBitmap());
        activity.setMaskAlphaBitmap(model.getMaskAlphaBitmap());
        activity.setOutputMaskBitmap(model.getOutputMaskBitmap());
        activity.refreshOutputMaskBitmap();
    }

    void doneDrawingStroke(float x, float y) {
        activity.clearUndoStrokes();
        activity.enableUndoBtn();
        activity.disableRedoBtn();
    }

    void onSaveInstanceState(Bundle bundle) {
        model.saveState(bundle);
    }

    void onRestoreInstanceState(Bundle bundle) {
        model.restoreState(bundle);
    }

    void onStart() {
        activity.showSpinner();
    }

    void onStop() {
        activity.hideSpinner();
        model.releaseImage();
    }

    void showSpinner() {
        activity.showSpinner();
    }

    void hideSpinner() {
        activity.hideSpinner();
    }

    void processSuccess() {
        hideSpinner();
    }

    void processFailed(int segmentationResult) {
        hideSpinner();
    }

    void refreshUndoRedoEnables() {

        if (activity.getUndoStrokeCount() == 0)
            activity.disableRedoBtn();
        else
            activity.enableRedoBtn();

        if (activity.getStrokeCount() == 0)
            activity.disableUndoBtn();
        else
            activity.enableUndoBtn();
    }

    void redoClicked() {
        if (activity.getUndoStrokeCount() > 0) {
            activity.putBackLastUndoStroke();
        }
        refreshUndoRedoEnables();
    }

    void undoClicked() {
        if (activity.getStrokeCount() > 0) {
            activity.removeLastStroke();
        }
        refreshUndoRedoEnables();
    }

    void helpClicked() {

    }

    void cancelClicked() {

    }

    void doneClicked() {
        showSpinner();
    }

    void foregroundBackgroundSeparationCompleted(Uri foregroundUri, Uri backgroundUri, Uri imageUri) {
        if (activity.nextActivity != null &&
                activity.nextActivity.equalsIgnoreCase(CloneActivity.class.getSimpleName())) {
            launchCloneActivity(foregroundUri, backgroundUri, imageUri);
        } else {
            launchEffectsActivity(foregroundUri, backgroundUri);
        }
    }

    void launchEffectsActivity(Uri foregroundUri, Uri backgroundUri) {
        Intent effectsIntent = new Intent(activity.getApplicationContext(), EffectsActivity.class);
        effectsIntent.putExtra(ImageSharingIntentExtras.FOREGROUND_IMAGE_URI,
                foregroundUri.toString());
        effectsIntent.putExtra(ImageSharingIntentExtras.BACKGROUND_IMAGE_URI,
                backgroundUri.toString());
        effectsIntent.putExtra(ImageSharingIntentExtras.FULL_MP_MODE, activity.isFullMP);
        activity.startActivity(effectsIntent);
    }

    void launchCloneActivity(Uri foregroundUri, Uri backgroundUri, Uri imageUri) {
        Intent cloneIntent = new Intent(activity.getApplicationContext(), CloneActivity.class);
        cloneIntent.putExtra(ImageSharingIntentExtras.EXTRA_OBJECT_IMAGE_URI, imageUri.toString());
        cloneIntent.putExtra(ImageSharingIntentExtras.EXTRA_MASK_IMAGE_URI,
                foregroundUri.toString());
        cloneIntent.putExtra(ImageSharingIntentExtras.EXTRA_TARGET_IMAGE_URI, imageUri.toString());
        cloneIntent.putExtra(ImageSharingIntentExtras.FULL_MP_MODE, activity.isFullMP);
        activity.startActivity(cloneIntent);
        hideSpinner();
    }


}
