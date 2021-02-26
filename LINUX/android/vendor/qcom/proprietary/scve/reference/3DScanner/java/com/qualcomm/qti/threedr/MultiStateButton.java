/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.threedr;

import android.annotation.TargetApi;
import android.content.Context;
import android.content.res.TypedArray;
import android.os.Build;
import android.util.AttributeSet;
import android.util.Log;
import android.widget.ImageButton;

public class MultiStateButton extends ImageButton {

    int state = 0;
    int maxStates = 1;
    int[] drawables;

    public MultiStateButton(Context context) {
        super(context);
    }

    public MultiStateButton(Context context, AttributeSet attrs) {
        super(context, attrs);
        if (!isInEditMode()) {
            init(context, attrs);
        }
    }

    public MultiStateButton(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        if (!isInEditMode()) {
            init(context, attrs);
        }
    }

    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    public MultiStateButton(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
        if (!isInEditMode()) {
            init(context, attrs);
        }
    }

    private void init(Context context, AttributeSet attrs) {
        int id = attrs.getAttributeResourceValue("http://schemas.android.com/apk/res-auto", "resources", -1);
        Log.e("X3", "id = " + id);
        if (id > 0) {
            TypedArray ar = context.getResources().obtainTypedArray(id);
            maxStates = ar.length();
            drawables = new int[maxStates];
            for (int i = 0; i < maxStates; i++) {
                drawables[i] = ar.getResourceId(i, -1);
            }
            ar.recycle();
        }

        state = attrs.getAttributeIntValue("http://schemas.android.com/apk/res-auto", "state", 0) % maxStates;

        updateResource();
    }

    @Override
    public boolean performClick() {
        boolean returnValue = super.performClick();
        nextState();
        return returnValue;
    }

    private void nextState() {
        state = (state + 1) % maxStates;
        updateResource();
    }

    private void updateResource() {
        this.setImageResource(drawables[state]);
        requestLayout();
    }

    public int getState() {
        return state;
    }

    public int getMaxStates() {
        return maxStates;
    }
}
