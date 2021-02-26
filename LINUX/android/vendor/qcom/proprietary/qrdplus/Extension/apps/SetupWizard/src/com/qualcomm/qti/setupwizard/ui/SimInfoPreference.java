/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.setupwizard.ui;

import android.content.Context;
import android.preference.Preference;
import android.util.AttributeSet;
import android.view.View;
import android.widget.TextView;

import com.qualcomm.qti.setupwizard.R;

public class SimInfoPreference extends Preference {
    private Context mContext;
    private TextView mSim1Name;
    private TextView mSim2Name;
    private CharSequence mSim1Text;
    private CharSequence mSim2Text;

    public SimInfoPreference(Context context) {
        this(context, null);
    }

    public SimInfoPreference(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public SimInfoPreference(Context context, AttributeSet attrs, int defStyleAttr) {
        this(context, attrs, defStyleAttr, 0);
    }

    public SimInfoPreference(Context context, AttributeSet attrs, int defStyleAttr,
             int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
        mContext = context;
        init();
    }

    /**
     * Method that initializes the preference
     */
    private void init() {
        setLayoutResource(R.layout.sim_cards_info);
    }

    @Override
    protected void onBindView(View view) {
        super.onBindView(view);
        mSim1Name = (TextView) view.findViewById(R.id.sim1_card_name);
        mSim2Name = (TextView) view.findViewById(R.id.sim2_card_name);
        update();
    }

    private void update() {
        if (mSim1Name != null) {
            mSim1Name.setText(mSim1Text);
        }

        if (mSim2Name != null) {
            mSim2Name.setText(mSim2Text);
        }
    }

    public void setSim1NameText(CharSequence str) {
        this.mSim1Text = str;
        update();
        notifyChanged();
    }

    public void setSim2NameText(CharSequence str) {
        this.mSim2Text = str;
        update();
        notifyChanged();
    }
}
