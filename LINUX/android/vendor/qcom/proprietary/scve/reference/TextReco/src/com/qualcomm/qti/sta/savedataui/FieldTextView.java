/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies, Inc. Confidential and Proprietary.
 */
package com.qualcomm.qti.sta.savedataui;

import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.Rect;
import android.util.AttributeSet;
import android.view.View;
import android.widget.AutoCompleteTextView;

/**
 * Class for custom AutoCompleteTextView
 */
public class FieldTextView extends AutoCompleteTextView {
    //private static final String LOG_TAG = "FieldTextView";
    /**
     * Constructor for the class
     *
     * @param context
     *            Application context
     */
    public FieldTextView(Context context) {
        super(context);

        initialize();
    }

    /**
     * Constructor for the class
     *
     * @param context
     *            Application context
     * @param attrs
     *            view attributes
     */
    public FieldTextView(Context context, AttributeSet attrs) {
        super(context, attrs);
        initialize();
    }

    /**
     * Constructor for the class
     *
     * @param context
     *            Application context
     * @param attrs
     *            view attributes
     * @param defStyle
     *            style
     */
    public FieldTextView(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
        initialize();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void performFiltering(final CharSequence text, final int keyCode) {
        String filterText = "";
        super.performFiltering(filterText, keyCode);
    }
    /**
     * {@inheritDoc}
     */
    @Override
    public boolean enoughToFilter() {
        return true;
    }
    /**
     * {@inheritDoc}
     */
    @Override
    protected void onFocusChanged(boolean focused, int direction, Rect previouslyFocusedRect) {
        super.onFocusChanged(focused, direction, previouslyFocusedRect);
        if (focused) {
            performFiltering(getText(), 0);
        }
        //Log.d(LOG_TAG, "Focus: " + focused);
    }
    /**
     * {@inheritDoc}
     */
    @SuppressLint("ClickableViewAccessibility")
    private void initialize() {
        setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View view) {
                showDropDown();
                requestFocus();
                //Log.d(LOG_TAG, "OnClick");
            }
        });
    }
}
