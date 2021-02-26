/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.imageedit.removal;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.ImageButton;
import android.widget.ImageView;

import com.qualcomm.qti.qces.customui.AnimatedImageView;
import com.qualcomm.qti.qces.imageedit.softcut.R;
import com.qualcomm.qti.qces.util.ImageSharingIntentExtras;

public class RemovalActivity extends Activity {

    private RemovalModel model;
    private RemovalController controller;
    public Uri removalUri;
    public boolean isFullMP;
    private AnimatedImageView spinner;
    private View spinnerBackground;

    public Bitmap displayBitmap;
    public ImageView displayImageView;

    private RemovalMaskView removalMaskAlphaView;

    private ImageButton undoButton;
    private ImageButton redoButton;
    private ImageButton doneButton;
    public ImageButton helpButton;
    private ImageButton cancelButton;

    private ImageButton strokeWidthSmall;
    private ImageButton strokeWidthMedium;
    private ImageButton strokeWidthLarge;

    private static final String TAG = "ImgStudio";

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        Log.d(TAG, "RemovalActivity onCreate");

        model = new RemovalModel(this);
        controller = new RemovalController(this, model);


        if (savedInstanceState != null) {
            controller.onRestoreInstanceState(savedInstanceState);
        }

        setContentView(R.layout.removal_layout);
        initializeViews();
        initializeActivity();
        initializeMaskAlphaView();


    }

    @Override
    public void onResume() {
       super.onResume();
       model.onResume();
    }

    @Override
    protected void onSaveInstanceState(Bundle savedInstanceState) {
        super.onSaveInstanceState(savedInstanceState);
        controller.onSaveInstanceState(savedInstanceState);
    }

    @Override
    protected void onStop() {
        super.onStop();

        hideSpinner();
        model.releaseImage();

        model.mObjectRemover.close();
        model.mObjectRemover = null;
    }

    void initializeViews() {

        initializeSpinner(R.id.matting_spinner, R.id.matting_spinner_background);
        displayImageView = (ImageView) findViewById(R.id.removal_image);
        removalMaskAlphaView = (RemovalMaskView) findViewById(R.id.removal_mask_alpha_view);

        strokeWidthSmall = (ImageButton) findViewById(R.id.removal_small_stroke);
        strokeWidthMedium = (ImageButton) findViewById(R.id.removal_medium_stroke);
        strokeWidthLarge = (ImageButton) findViewById(R.id.removal_large_stroke);

        initializeHelpButton(R.id.removal_help_button);
        initializeCancelButton(R.id.removal_cancel_button);
        initializeUndoButton(R.id.removal_undo_button);
        initializeRedoButton(R.id.removal_redo_button);
        initializeDoneButton(R.id.removal_done_button);

    }

    void initializeMaskAlphaView() {
        removalMaskAlphaView.attachCompanionViews(displayImageView);
        removalMaskAlphaView.setOnMaskUpdatedListener(new RemovalMaskView.OnMaskUpdatedListener() {

            @Override
            public void onMaskUpdated(RemovalStroke stroke) {

                controller.doneDrawingStroke(stroke);

            }
        });
    }

    void initializeActivity() {

        Intent intent = getIntent();

        String removalUriString = intent
                .getStringExtra(ImageSharingIntentExtras.EXTRA_TARGET_IMAGE_URI);

        // if restore state didn't init the removalUri image, then use the one from intent
        if (removalUri == null && removalUriString != null) {
            removalUri = Uri.parse(removalUriString);
        }

        isFullMP = intent.getBooleanExtra(ImageSharingIntentExtras.FULL_MP_MODE, true);

        if (removalUri != null)
            controller.initializeByImageUri(removalUri);

    }

    void setBitmapToImageView(Bitmap removalBitmap) {
        // this.displayBitmap = removalBitmap;
        // displayImageView.setImageBitmap(removalBitmap);

        // this.displayBitmap = removalBitmap.copy(Bitmap.Config.ARGB_8888, true);

        if (displayBitmap == null) {
            // make a new mutable bitmap
            displayBitmap = Bitmap.createBitmap(removalBitmap.getWidth(),
                    removalBitmap.getHeight(), removalBitmap.getConfig());
            Canvas c = new Canvas(displayBitmap);
            c.drawBitmap(removalBitmap, 0, 0, null);
            displayImageView.setImageBitmap(displayBitmap);

        } else {
            displayBitmap = removalBitmap;
            displayImageView.setImageBitmap(displayBitmap);
        }
    }

    public Bitmap getRemovalBitmap() {
        return displayBitmap;
    }

    public ImageView getImageView() {
        return displayImageView;
    }

    void initializeSpinner(int idSpinner, int idSpinnerBackground) {
        spinner = (AnimatedImageView) findViewById(idSpinner);
        spinnerBackground = findViewById(idSpinnerBackground);
    }

    void showSpinner() {
        spinner.setVisibility(View.VISIBLE);
        spinnerBackground.setVisibility(View.VISIBLE);
    }

    void hideSpinner() {
        spinner.setVisibility(View.GONE);
        spinnerBackground.setVisibility(View.GONE);
    }

    @Override
    public boolean dispatchTouchEvent(MotionEvent event) {
        super.dispatchTouchEvent(event);

        /* Do not dismiss any hints for MotionEvent.ACTION_DOWN when helpButton is pressed */
        if (helpButton.isPressed()) {
            return false;
        }
        if (event.getActionMasked() == MotionEvent.ACTION_DOWN) {
            controller.dismissHint();
            model.hintActive = false;
        }
        return false;
    }

    void initializeHelpButton(int id) {
        helpButton = (ImageButton) findViewById(id);
        helpButton.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                // controller.helpClicked();
            }
        });

        helpButton.setVisibility(View.GONE); //temporary until we have content from UX team
    }

    void initializeUndoButton(int id) {
        undoButton = (ImageButton) findViewById(id);
        undoButton.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                controller.undoClicked();
            }
        });

        undoButton.setEnabled(false);
    }

    void initializeRedoButton(int id) {
        redoButton = (ImageButton) findViewById(id);
        redoButton.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                controller.redoClicked();
            }
        });

        redoButton.setEnabled(false);
    }

    int getStrokeCount() {
        return removalMaskAlphaView.getStrokeCount();
    }

    void redrawStrokes() {
        removalMaskAlphaView.redrawStrokes();
    }

    void enableUndoBtn() {
        undoButton.setEnabled(true);
    }

    void disableUndoBtn() {
        undoButton.setEnabled(false);
    }

    void enableRedoBtn() {
        redoButton.setEnabled(true);
    }

    void disableRedoBtn() {
        redoButton.setEnabled(false);
    }

    void initializeDoneButton(int id) {
        doneButton = (ImageButton) findViewById(id);
        doneButton.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                // maskAlphaView.doneClicked();
                controller.doneClicked();
            }
        });
    }

    void initializeCancelButton(int id) {
        cancelButton = (ImageButton) findViewById(id);
        cancelButton.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                controller.cancelClicked();
            }
        });
    }

    public void enableTouchMaskImageView(boolean enable) {
        removalMaskAlphaView.setEnabled(enable);
    }

    void refreshStrokeButtons() {
        switch (model.strokeSizeSelected) {
            case SMALL:
                removalMaskAlphaView.setMattingStrokeSize(getResources().getDimension(
                        R.dimen.stroke_width_small));
                strokeWidthSmall.setSelected(true);
                strokeWidthMedium.setSelected(false);
                strokeWidthLarge.setSelected(false);
                break;
            case MEDIUM:
                removalMaskAlphaView.setMattingStrokeSize(getResources().getDimension(
                        R.dimen.stroke_width_medium));
                strokeWidthSmall.setSelected(false);
                strokeWidthMedium.setSelected(true);
                strokeWidthLarge.setSelected(false);
                break;
            case LARGE:
                removalMaskAlphaView.setMattingStrokeSize(getResources().getDimension(
                        R.dimen.stroke_width_large));
                strokeWidthSmall.setSelected(false);
                strokeWidthMedium.setSelected(false);
                strokeWidthLarge.setSelected(true);
                break;
        }
    }

    public void smallStrokeClicked(View view) {
        controller.smallStrokeClicked();
    }

    public void mediumStrokeClicked(View view) {
        controller.mediumStrokeClicked();
    }

    public void largeStrokeClicked(View view) {
        controller.largeStrokeClicked();
    }

    public void setMaskAlphaBitmap(Bitmap maskAlphaBitmap) {

        removalMaskAlphaView.setMaskBitmap(maskAlphaBitmap);
        removalMaskAlphaView.invalidate();
    }

    // clear the visibility of the stroke, but don't delete it from the stroke list
    public void clearStroke() {
        removalMaskAlphaView.clearStroke();
    }

    // delete an invalid last stroke from the stroke list
    public void deleteLastStroke() {
        removalMaskAlphaView.deleteLastStroke();
    }

    public void drawBitmapOntoImageView(int startX, int startY, Bitmap bitmapOverlay) {
        Canvas c = new Canvas(displayBitmap);
        c.drawBitmap(bitmapOverlay, startX, startY, null);
        displayImageView.invalidate();
    }

}
