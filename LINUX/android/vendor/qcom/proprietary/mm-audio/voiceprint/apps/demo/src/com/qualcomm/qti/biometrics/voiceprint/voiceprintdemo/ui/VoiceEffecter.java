/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.ui;

import android.util.Log;
import android.view.View;


public class VoiceEffecter {
    private final static float MIN_THRESHOLD = 0.2f;
    private static final String TAG = "VoicePrintDemo";

    private final View effectView;
    private final float min;
    private final float max;
    private final float scaleMax;
    private boolean updatable;

    public VoiceEffecter(float min, float max, View view, float scaleMax) {
        this.effectView = view;
        this.min = min;
        this.max = max;
        this.scaleMax = scaleMax;
        this.updatable = false;

        view.setVisibility(View.INVISIBLE);
    }

    public void update(float value) {
        if (updatable) {
            float percentageRate = getPercentageRate(value);
            float scaleSize;

           // Log.d(TAG, "Energy " + value + ", %=" + percentageRate);
            if (MIN_THRESHOLD < percentageRate) {
                scaleSize = percentageRate * scaleMax;
            } else {
                scaleSize = MIN_THRESHOLD * scaleMax;
            }

            setScaleXY(scaleSize);

            //            Log.i(TAG, "updated");
        } else {
            //            Log.i(TAG, "not updated");
        }
    }

    private float getPercentageRate(float value) {
        if (min >= value) {
            return 0.0f;
        } else if (max <= value) {
            return 1.0f;
        } else {
            value -= min;
            return value / (max - min);
        }
    }

    public void stop() {
        updatable = false;
        setScaleXY(0);

        effectView.setVisibility(View.INVISIBLE);
    }

    public void start() {
        updatable = true;
        setScaleXY(0);

        effectView.setVisibility(View.VISIBLE);
    }

    private void setScaleXY(float scaleSize) {
        effectView.setScaleX(scaleSize);
        effectView.setScaleY(scaleSize);
    }

}
