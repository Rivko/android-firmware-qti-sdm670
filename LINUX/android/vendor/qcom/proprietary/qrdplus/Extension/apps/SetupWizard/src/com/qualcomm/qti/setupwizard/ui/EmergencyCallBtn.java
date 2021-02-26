/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.setupwizard.ui;

import android.content.Context;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.qualcomm.qti.setupwizard.R;

public class EmergencyCallBtn extends LinearLayout {
    private ImageView mEmergencyImage;
    private TextView mEmergencyText;

    public EmergencyCallBtn(Context context) {
        this(context, null);
    }

    public EmergencyCallBtn(Context context, AttributeSet attrs) {
        super(context, attrs);

        LayoutInflater.from(context).inflate(R.layout.emergency_call_btn, this, true);

        mEmergencyImage = (ImageView) findViewById(R.id.image_view);
        mEmergencyText = (TextView) findViewById(R.id.text_view);
    }

    /**
     * set imageview resource
     */
    public void setImageResource(int resId) {
        mEmergencyImage.setImageResource(resId);
    }

    /**
     * set textview text
     */
    public void setTextViewText(String str) {
        mEmergencyText.setText(str);
    }

    /**
     * set textview text
     */
    public void setTextViewText(int resId) {
        mEmergencyText.setText(resId);
    }
}
