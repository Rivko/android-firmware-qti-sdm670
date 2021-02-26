/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies, Inc. Confidential and Proprietary.
 */
package com.qualcomm.qti.sta.resultui;

import android.content.Context;
import android.util.AttributeSet;
import android.view.KeyEvent;
import android.view.View;
import android.widget.EditText;

/**
 * Class to handle action edit text
 */
public class ActionEditText extends EditText {

    private ActionEditTextListener editTextListener = null;
    private View parentTextView = null;

    /**
     * Interface for edit text notifications
     */
    public interface ActionEditTextListener {
        /**
         * Notification on dismissing the soft keybord
         *
         * @param editText
         *            associated edit text
         * @param textView
         *            associated text view
         */
        public void onKeyboardRemoved(ActionEditText editText, View textView);
    }

    /**
     * Constructor for the class
     *
     * @param context
     *            application context
     */
    public ActionEditText(Context context) {
        super(context);
    }

    /**
     * Constructor for the class
     *
     * @param context
     *            application context
     * @param attrs
     *            edit text attributes
     */
    public ActionEditText(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    /**
     * Constructor for the class
     *
     * @param context
     *            application context
     * @param attrs
     *            edit text attributes
     * @param defStyle
     *            edit text def style
     */
    public ActionEditText(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
    }

    /**
     * Sets the information or the edit text
     *
     * @param textView
     *            text view
     * @param listener
     *            edit text listener
     */
    public void setInformation(View textView, ActionEditTextListener listener) {
        parentTextView = textView;
        editTextListener = listener;
    }
    /**
     * {@inheritDoc}
     */
    @Override
    public boolean onKeyPreIme(int keyCode, KeyEvent event) {
        if (event.getKeyCode() == KeyEvent.KEYCODE_BACK) {
            if (editTextListener != null && parentTextView != null) {
                editTextListener.onKeyboardRemoved(this, parentTextView);
            }
        }
        return super.dispatchKeyEvent(event);
    }
}
