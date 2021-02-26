/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.setupwizard.protect;

import android.app.Fragment;
import android.content.Context;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.qualcomm.qti.setupwizard.R;
import com.qualcomm.qti.setupwizard.SetupPageActivity;
import com.qualcomm.qti.setupwizard.ui.FragmentCallbacks;

public class ProtectFragment extends Fragment implements View.OnClickListener {

    private FragmentCallbacks mCallbacks;
    private View mView;
    private TextView mChoosePattern;
    private TextView mChoosePin;
    private TextView mChoosePassword;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        mView = inflater.inflate(R.layout.fragment_protect, container, false);
        initView(mView);
        onBackLabel(SetupPageActivity.PROTECT);
        return mView;
    }

    private void initView(View view) {
        mChoosePattern = (TextView) view.findViewById(R.id.choose_pattern);
        mChoosePin = (TextView) view.findViewById(R.id.choose_pin);
        mChoosePassword = (TextView) view.findViewById(R.id.choose_password);
        mChoosePattern.setOnClickListener(this);
        mChoosePin.setOnClickListener(this);
        mChoosePassword.setOnClickListener(this);
    }

    @Override
    public void onDetach() {
        super.onDetach();
        mCallbacks = null;
    }

    // TODO: Rename method, update argument and hook method into UI event
    public void onBackLabel(String currentPage) {
        if (mCallbacks != null) {
            mCallbacks.onFragmentInteraction(currentPage);
        }
    }

    @Override
    public void onAttach(Context context) {
        super.onAttach(context);
        if (context instanceof FragmentCallbacks) {
            mCallbacks = (FragmentCallbacks) context;
        } else {
            throw new RuntimeException(context.toString()
                    + " must implement OnFragmentInteractionListener");
        }
    }

    @Override
    public void onClick(View v) {
        if (v == mChoosePattern) {
            onBackLabel(SetupPageActivity.CHOOSE_PATTERN);
        } else if (v == mChoosePin) {
            onBackLabel(SetupPageActivity.CHOOSE_PIN);
        } else if (v == mChoosePassword) {
            onBackLabel(SetupPageActivity.CHOOSE_PASSWORD);
        }
    }
}
