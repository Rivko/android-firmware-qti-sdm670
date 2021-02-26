/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */
package com.qti.csm.encryption.system;

import android.content.Context;
import android.preference.Preference;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.qti.csm.R;

public class CustomInfo extends Preference {

    private Context mContext;

    public CustomInfo(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
        mContext = context;
    }

    public CustomInfo(Context context) {
        super(context, null);
    }

    public CustomInfo(Context context, AttributeSet attrs) {
        this(context, attrs, 0);

    }

    @Override
    protected void onBindView(View view) {
        super.onBindView(view);
        ProcessInfo.logd("");
    }

    @Override
    protected View onCreateView(ViewGroup parent) {
        ProcessInfo.logd("");

        LayoutInflater mLayoutInflater = LayoutInflater.from(mContext);
        View view = mLayoutInflater.inflate(R.layout.file_operation, null);
        return view;

    }
}