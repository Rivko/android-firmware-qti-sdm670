/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.imageedit.objectclone;

import java.io.File;

import android.app.Activity;
import android.app.DialogFragment;
import android.app.Fragment;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.provider.MediaStore;

import com.qualcomm.qti.qces.customui.HintDialogFragment;
import com.qualcomm.qti.qces.customui.HintDialogFragment.HintPosition;
import com.qualcomm.qti.qces.imageedit.softcut.R;
import com.qualcomm.qti.qces.util.ImageSharingIntentExtras;

public class CloneController {
    private static final int SELECT_PICTURE = 10;
    private static final int REQUEST_CAMERA = 20;

    public enum HintType {
        FOREGROUND
    };

    private static final String TAG = CloneController.class.getSimpleName();

    private CloneActivity activity;
    private CloneModel model;
    private Uri fileUri;

    public CloneController(CloneActivity activity, CloneModel model) {
        this.activity = activity;
        this.model = model;
    }

    void onSaveInstanceState(Bundle bundle) {
        model.saveState(bundle);
        if (fileUri != null) {
            bundle.putString("fileUri", fileUri.toString());
        }
    }

    void onRestoreInstanceState(Bundle bundle) {
        model.restoreState(bundle);
        String fileUriString = bundle.getString("fileUri");
        if (fileUriString != null) {
            fileUri = Uri.parse(fileUriString);
        }
    }

    void onResume() {
        if (model.hintActive == true) {
            model.hintShown.put(HintType.FOREGROUND, false);
        }
        showHint(HintType.FOREGROUND);
        refreshUndoRedoEnables();
    }

    void helpClicked() {
        showHelp();
    }

    void cancelClicked() {
        activity.onBackPressed();
    }

    void doneClicked() {
        model.generateFlattenedImage();
    }

    void replaceBgClicked() {
        addNewBackground();
    }

    void showSpinner() {
        activity.showSpinner();
    }

    void undoClicked() {
        activity.undoTransScale();
        refreshUndoRedoEnables();
    }

    void redoClicked() {
        activity.redoTransScale();
        refreshUndoRedoEnables();
    }

    void refreshUndoRedoEnables() {

        if (activity.getRedoTransScaleCount() == 0)
            activity.disableRedoBtn();
        else
            activity.enableRedoBtn();

        if (activity.getUndoTransScaleCount() == 0)
            activity.disableUndoBtn();
        else
            activity.enableUndoBtn();
    }

    void addNewBackground() {
        AddNewBackgroundDialogFragment frag = new AddNewBackgroundDialogFragment();
        frag.show(activity.getFragmentManager(), "bg_dialog");
    }

    void getBackgroundFromGalleryClicked() {
        Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT);
        intent.addCategory(Intent.CATEGORY_OPENABLE);
        intent.setType("image/*");
        activity.startActivityForResult(Intent.createChooser(intent, "Select Picture"),
                SELECT_PICTURE);
    }

    void getBackgroundFromCameraClicked() {
        Intent intent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);
        fileUri = Uri.fromFile(new File(activity.getExternalCacheDir(), "tempimg_bg.jpg"));
        intent.putExtra(MediaStore.EXTRA_OUTPUT, fileUri);
        activity.startActivityForResult(intent, REQUEST_CAMERA);
    }

    void dismissFragment(String fragName) {
        Fragment prev = activity.getFragmentManager().findFragmentByTag(fragName);
        if (prev != null) {
            DialogFragment df = (DialogFragment)prev;
            df.dismiss();
        }
    }

    void handleOnActivityResult(int requestCode, int resultCode, Intent data) {
        dismissFragment("bg_dialog");
        activity.showSpinner();
        if (resultCode == Activity.RESULT_OK) {
            Uri sceneImageUri = null;
            switch (requestCode) {
            case SELECT_PICTURE:
                if (data == null) {
                    return;
                }
                sceneImageUri = data.getData();
                break;

            case REQUEST_CAMERA:
                sceneImageUri = fileUri;
                break;
            }
            activity.onStop();

            startCloneActivityWithNewBgImage(sceneImageUri);

        }
        activity.hideSpinner();
    }

    void startCloneActivityWithNewBgImage(Uri sceneImageUri) {

        Intent cloneIntent = activity.getIntent();
        cloneIntent.putExtra(ImageSharingIntentExtras.EXTRA_TARGET_IMAGE_URI,
                sceneImageUri.toString());

        activity.clearUndoRedoStack();
        activity.setIntent(null);
        activity.setIntent(cloneIntent);
        activity.onStart();
    }

    void showHelp() {
        Intent intent = new Intent(activity, CloneIntroFragmentActivity.class);
        activity.startActivityForResult(intent, R.integer.clone_intro_code);
    }

    void showHint(HintType hintType) {
        DialogFragment newHintFragment = null;

        dismissHint(); // dismiss any previous hints still on the screen

        switch (hintType) {
        case FOREGROUND:
            if (model.hintShown.get(hintType) == false || model.hintActive) {
                newHintFragment = HintDialogFragment.newInstance(HintPosition.TOP_TOAST, activity
                        .getResources().getString(R.string.hint_object_clone));
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

}
