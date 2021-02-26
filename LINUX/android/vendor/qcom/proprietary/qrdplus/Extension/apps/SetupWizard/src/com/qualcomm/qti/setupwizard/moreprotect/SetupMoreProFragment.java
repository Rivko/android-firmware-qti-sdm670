/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.setupwizard.moreprotect;

import android.app.Fragment;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.Button;

import com.qualcomm.qti.setupwizard.R;

public class SetupMoreProFragment extends Fragment {

    private static final String MORE_PROTECTION = "more_protection";
    private Button mSetupNow;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.setup_more_protection, container, false);
        mSetupNow = (Button) view.findViewById(R.id.setup_now);
        mSetupNow.setOnClickListener(setupNowListener);
        return view;
    }

    public OnClickListener setupNowListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {

        }
    };
}
