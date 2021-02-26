/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.imageedit.segmentation;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.ColorMatrixColorFilter;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.ImageButton;
import android.widget.ImageView;

import com.qualcomm.qti.qces.customui.AnimatedImageView;

public abstract class AbstractSegmentationActivity extends Activity {

    @SuppressWarnings("unused")
    private AbstractSegmentationModel model;
    private AbstractSegmentationController controller;

    private AnimatedImageView spinner;
    private View spinnerBackground;

    private ImageView imageView;
    private MaskView maskAlphaView;
    private ImageView outputMaskView;
    private Bitmap outputMaskBitmap;

    public Bitmap getImageViewBitmap() {
        return imageViewBitmap;
    }

    public void setImageViewBitmap(Bitmap imageViewBitmap) {
        this.imageViewBitmap = imageViewBitmap;
    }

    private Bitmap imageViewBitmap;

    private ImageButton undoButton;
    private ImageButton redoButton;
    private ImageButton doneButton;
    public ImageButton helpButton;
    private ImageButton cancelButton;

    public Boolean isFullMP;
    protected String nextActivity;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if (savedInstanceState != null) {
            controller.onRestoreInstanceState(savedInstanceState);
        }
    }

    @Override
    protected void onSaveInstanceState(Bundle savedInstanceState) {
        super.onSaveInstanceState(savedInstanceState);
        controller.onSaveInstanceState(savedInstanceState);
    }

    public ImageView getImageView() {
        return imageView;
    }

    public void setImageView(ImageView imageView) {
        this.imageView = imageView;
    }

    @Override
    protected void onStop() {
        super.onStop();

        // set all View objects bitmaps to null to ensure a recycled Bitmap object
        // isn't attempted to get redrawn if the Activity comes back in view

        imageView.setImageBitmap(null);
        maskAlphaView.setMaskBitmap(null);
        outputMaskView.setImageBitmap(null);
    }

    ImageView getOutputMaskImageView() {
        return outputMaskView;
    }

    protected boolean isActivityInActiveState() {
        return (!isFinishing() && !isDestroyed());
    }

    void setModel(AbstractSegmentationModel model) {
        this.model = model;
    }

    void setController(AbstractSegmentationController controller) {
        this.controller = controller;
    }

    void initializeSpinner(int idSpinner, int idSpinnerBackground) {
        spinner = (AnimatedImageView)findViewById(idSpinner);
        spinnerBackground = findViewById(idSpinnerBackground);
    }

    void initializeImageView(int id) {
        imageView = (ImageView)findViewById(id);
    }

    void initializeMaskAlphaView(int id) {
        maskAlphaView = (MaskView)findViewById(id);
    }

    MaskView getMaskAlphaView() {
        return maskAlphaView;
    }

    void initializeOutputMaskView(int id) {
        outputMaskView = (ImageView)findViewById(id);
    }

    void initializeCancelButton(int id) {
        cancelButton = (ImageButton)findViewById(id);
        cancelButton.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                controller.cancelClicked();
            }
        });
    }

    void initializeHelpButton(int id) {
        helpButton = (ImageButton)findViewById(id);
        helpButton.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                controller.helpClicked();
            }
        });
    }

    void initializeUndoButton(int id) {
        undoButton = (ImageButton)findViewById(id);
        undoButton.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                controller.undoClicked();
            }
        });
    }

    void initializeRedoButton(int id) {
        redoButton = (ImageButton)findViewById(id);
        redoButton.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                controller.redoClicked();
            }
        });
    }

    void initializeDoneButton(int id) {
        doneButton = (ImageButton)findViewById(id);
        doneButton.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                maskAlphaView.doneClicked();
                controller.doneClicked();
            }
        });
    }

    void showSpinner() {
        spinner.setVisibility(View.VISIBLE);
        spinnerBackground.setVisibility(View.VISIBLE);
    }

    void hideSpinner() {
        spinner.setVisibility(View.GONE);
        spinnerBackground.setVisibility(View.GONE);
    }

    void setSegmentationState(SegmentationState segmentationState) {
        maskAlphaView.setState(segmentationState);
    }

    SegmentationState getSegmentationState() {
        return maskAlphaView.getSegmentationState();
    }

    void initializeOutputMaskViewColors(float[] color) {
        outputMaskView.setColorFilter(new ColorMatrixColorFilter(color));
    }

    void initializeMaskAlphaView() {
        maskAlphaView.attachCompanionViews(imageView, outputMaskView);

        maskAlphaView.setStrokeListener(new MaskView.OnStrokeListener() {
            @Override
            public void onStrokeStartPoint(boolean startPoint, float x, float y) {
                if (startPoint == false)
                    controller.doneDrawingStroke(x, y);
            }
        });

    }

    void setImageBitmap(Bitmap imageBitmap) {
        imageViewBitmap = imageBitmap;
        imageView.setImageBitmap(imageBitmap);
    }

    void setOutputMaskBitmap(Bitmap outputMaskBitmap) {
        this.outputMaskBitmap = outputMaskBitmap;
    }

    void refreshOutputMaskBitmap() {
        outputMaskView.setImageBitmap(outputMaskBitmap);
        outputMaskView.invalidate();
    }

    public Bitmap getOutputMaskBitmap() {
        return outputMaskBitmap;
    }

    void setMaskAlphaBitmap(Bitmap maskAlphaBitmap) {
        maskAlphaView.setMaskBitmap(maskAlphaBitmap);
        maskAlphaView.invalidate();
    }

    void invalidateMaskAlphaView() {
        maskAlphaView.invalidate();
    }

    void enableTouchMaskImageView() {
        maskAlphaView.setEnabled(true);
    }

    void disableTouchMaskImageView() {
        maskAlphaView.setEnabled(false);
    }

    void updateOutputMask() {
        outputMaskView.invalidate();
        showOutputMaskView();
    }

    void showOutputMaskView() {
        outputMaskView.setVisibility(View.VISIBLE);
    }

    void hideOutputMaskView() {
        outputMaskView.setVisibility(View.INVISIBLE);
    }

    public ImageView getOutputMaskView() {
        return outputMaskView;
    }

    void setOutputMaskViewOpacity(float alpha) {
        outputMaskView.setAlpha(alpha);
    }

    void removeLastStroke() {
        maskAlphaView.removeLastStroke();
    }

    void redrawStrokes() {
        maskAlphaView.redrawStrokes();
    }

    void putBackLastUndoStroke() {
        maskAlphaView.putBackLastUndoStroke();
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

    int getStrokeCount() {
        return maskAlphaView.getStrokeCount();
    }

    int peekStrokesSegmentationState() {
        return maskAlphaView.peekStrokesSegmentationState();
    }

    int getUndoStrokeCount() {
        return maskAlphaView.getUndoStrokeCount();
    }

    void clearUndoStrokes() {
        maskAlphaView.clearUndoStrokes();
    }

    int peekUndoStrokesSegmentationState() {
        return maskAlphaView.peekUndoStrokesSegmentationState();
    }

    void enableDoneButton() {
        doneButton.setEnabled(true);
    }

    void disableDoneButton() {
        doneButton.setEnabled(false);
    }

    int getBackgroundStrokesCount() {
        return maskAlphaView.getBackgroundStrokesCount();
    }

    int getForegroundStrokesCount() {
        return maskAlphaView.getForegroundStrokesCount();
    }

    int getImageViewHeight() {
        return imageView.getHeight();
    }

    int getImageViewWidth() {
        return imageView.getWidth();
    }

    void showMaskView() {
        maskAlphaView.setVisibility(View.VISIBLE);
    }

    void hideMaskView() {
        maskAlphaView.setVisibility(View.INVISIBLE);
    }

    void clearSegmentationStrokes() {
        maskAlphaView.clearSegmentationStrokes();
    }

    void processSuccess() {
        if (!isActivityInActiveState()) {
            return;
        }
        controller.processSuccess();
    }

    void processFailed(int segmentationResult) {
        controller.processFailed(segmentationResult);
    }

}
