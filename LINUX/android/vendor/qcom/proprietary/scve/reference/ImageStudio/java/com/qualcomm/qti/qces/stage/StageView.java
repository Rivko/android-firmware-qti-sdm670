/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.stage;

import android.graphics.Bitmap;
import android.graphics.Matrix;
import android.view.View;
import android.widget.ImageView;

import com.qualcomm.qti.qces.customui.AnimatedImageView;
import com.qualcomm.qti.qces.imageedit.softcut.R;
import com.qualcomm.qti.qces.util.TranslateScaleController;

public class StageView {
    private StageActivity activity;
    private AnimatedImageView spinner;
    private ImageView stageImageView;
    private Bitmap stageBitmap;

    private static final float MAX_SCALE_FACTOR = 10f;
    private TranslateScaleController translateScaleController;

    public StageView(StageActivity stageActivity) {
        this.activity = stageActivity;
        initializeViews();
        initializeTranslationScaling();
    }

    void initializeViews() {
        stageImageView = (ImageView)getViewForId(R.id.stage_image);
        spinner = (AnimatedImageView)getViewForId(R.id.stage_spinner);
    }

    void initializeTranslationScaling() {
        translateScaleController = new TranslateScaleController(stageImageView, MAX_SCALE_FACTOR) {
            @Override
            public void onTranslateScaleChanged(float x, float y, float scale) {
                Matrix m = new Matrix();
                m.postTranslate(-x, -y);
                m.postScale(scale, scale);
                stageImageView.setImageMatrix(m);
            }
        };
    }

    void updateContentSize() {
        int contentWidth = stageImageView.getDrawable().getIntrinsicWidth();
        int contentHeight = stageImageView.getDrawable().getIntrinsicHeight();
        translateScaleController.setContentSize(contentWidth, contentHeight);
    }

    private View getViewForId(int viewId) {
        return activity.findViewById(viewId);
    }

    void showSpinner() {
        spinner.setVisibility(View.VISIBLE);
    }

    void hideSpinner() {
        spinner.setVisibility(View.INVISIBLE);
    }

    void setStageBitmap(Bitmap stageBitmap) {
        this.stageBitmap = stageBitmap;
        stageImageView.setImageBitmap(stageBitmap);
    }

    public Bitmap getStageBitmap() {
        return stageBitmap;
    }
}
