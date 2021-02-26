/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies, Inc. Confidential and Proprietary.
 */
package com.qualcomm.qti.sta.resultui;

import android.content.Context;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.widget.ScrollView;

/**
 * Custom scroll view to add the ability of canceling the scrolling
 */
public class ActionListScrollView extends ScrollView {
    private boolean scrollable = true;

    /**
     * Constructor for the class
     *
     * @param context
     *            application Context
     */
    public ActionListScrollView(Context context) {
        super(context);
    }

    /**
     * Constructor for the class
     *
     * @param context
     *            application Context
     * @param attrs
     *            attributes
     */
    public ActionListScrollView(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    /**
     * Constructor for the class
     *
     * @param context
     *            application Context
     * @param attrs
     *            attributes
     * @param defStyle
     *            style
     */
    public ActionListScrollView(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
    }

    /**
     * enable/disable the scrolling
     *
     * @param enabled
     *            true if scrolling needs to be enabled, false otherwise
     */
    public void enableScroll(boolean enabled) {
        scrollable = enabled;
    }
    /**
     * {@inheritDoc}
     */
    @Override
    public boolean onTouchEvent(MotionEvent motionEvent) {
        switch (motionEvent.getAction()) {
        case MotionEvent.ACTION_DOWN:
            return (scrollable) ? super.onTouchEvent(motionEvent) : scrollable;

        default:
            return super.onTouchEvent(motionEvent);
        }
    }
    /**
     * {@inheritDoc}
     */
    @Override
    public boolean onInterceptTouchEvent(MotionEvent motionEvent) {
        return (scrollable) ? super.onInterceptTouchEvent(motionEvent) : false;
    }
}
