/*************************************************************************
 Copyright (c)2012-2013,2017 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
*************************************************************************/

package com.qualcomm.qti.presenceappSub2;

import android.content.Context;
import android.text.Editable;
import android.text.method.MovementMethod;
import android.text.method.ScrollingMovementMethod;
import android.util.AttributeSet;
import android.util.Log;
import android.widget.TextView;

public class LogTextBox extends TextView {

    private static LogTextBox logTextBox = null;
    private int index = 1;
	private static String TAG = "Presence_UI2: LogTextBox";

    static LogTextBox getInstance() {
        return logTextBox;
    }

    public static void setInstance(LogTextBox textBox) {
        logTextBox = textBox;
    }


    public LogTextBox(Context context, AttributeSet attrs) {
        this(context, attrs, android.R.attr.textViewStyle);
    }

    public LogTextBox(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
    }

    @Override
        protected MovementMethod getDefaultMovementMethod() {
            return ScrollingMovementMethod.getInstance();
        }

    @Override
        public Editable getText() {
            return (Editable) super.getText();
        }

    @Override
        public void setText(CharSequence text, BufferType type) {
            super.setText(text, BufferType.EDITABLE);
        }

    @Override
        public void append(CharSequence text, int start, int end) {
            setTextSize(10);
            String temp = text.toString();

            temp = "["+index+"]:"+temp+"\n";

            Log.d(TAG, "temp:" + temp);
            index++;

            CharSequence out = temp.subSequence(0, temp.length());
            super.append(out, start, temp.length());
        }
}
