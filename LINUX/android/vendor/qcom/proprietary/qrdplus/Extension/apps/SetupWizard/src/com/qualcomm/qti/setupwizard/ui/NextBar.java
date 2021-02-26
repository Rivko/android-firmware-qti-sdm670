/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.setupwizard.ui;

import android.content.Context;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.qualcomm.qti.setupwizard.R;

public class NextBar extends RelativeLayout {
    private TextView mSkipText;

    public NextBar(Context context, AttributeSet attr) {
        super(context, attr);

        LayoutInflater.from(context).inflate(R.layout.next_bar, this, true);

        mSkipText = (TextView) findViewById(R.id.next_text);
    }

    /**
     * set textview text
     */
    public void setTextViewText(String str) {
        mSkipText.setText(str);
    }

    /**
     * set textview text
     */
    public void setTextViewText(int resId) {
        mSkipText.setText(resId);
    }
}
