/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 */
/*
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License
 */

package com.qualcomm.qti.setupwizard.protect;

import android.app.admin.DevicePolicyManager;
import android.content.Context;
import android.os.Bundle;
import android.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.RadioButton;
import android.widget.TextView;

import com.qualcomm.qti.setupwizard.R;
import com.qualcomm.qti.setupwizard.ui.FragmentCallbacks;

public class SecureStartUpFragment extends Fragment implements View.OnClickListener {

    private FragmentCallbacks mCallbacks;
    private View mView;
    private TextView mMsgTextView;
    private RadioButton mRequirePasswordToDecryptButton;
    private RadioButton mDontRequirePasswordToDecryptButton;
    private int mQuality;

    private void initViews(View view) {
        mMsgTextView = (TextView) view.findViewById(R.id.secure_start_up_msg);
        mRequirePasswordToDecryptButton =
                (RadioButton) view.findViewById(R.id.encrypt_require_password);
        mDontRequirePasswordToDecryptButton =
                (RadioButton) view.findViewById(R.id.encrypt_no_require_password);
    }

    public void initData(int args) {
        mQuality = args;
        final int msgId;
        final int enableId;
        switch (mQuality) {
            case DevicePolicyManager.PASSWORD_QUALITY_SOMETHING:
                msgId = R.string.secure_start_up_msg_pattern;
                enableId = R.string.encrypt_require_pattern;
                break;
            case DevicePolicyManager.PASSWORD_QUALITY_NUMERIC:
                msgId = R.string.secure_start_up_msg_pin;
                enableId = R.string.encrypt_require_pin;
                break;
            case DevicePolicyManager.PASSWORD_QUALITY_ALPHANUMERIC:
                msgId = R.string.secure_start_up_msg_password;
                enableId = R.string.encrypt_require_password;
                break;
            default:
                msgId = R.string.secure_start_up_msg_password;
                enableId = R.string.encrypt_require_password;
                break;
        }
        mMsgTextView.setText(msgId);
        mRequirePasswordToDecryptButton.setOnClickListener(this);
        mRequirePasswordToDecryptButton.setText(enableId);
        mDontRequirePasswordToDecryptButton.setOnClickListener(this);
        setRequirePasswordState(false);
    }

    private void setRequirePasswordState(boolean required) {
        mRequirePasswordToDecryptButton.setChecked(required);
        mDontRequirePasswordToDecryptButton.setChecked(!required);
        onBackLabel(required);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        mView = inflater.inflate(R.layout.fragment_secure_start_up, container, false);
        initViews(mView);
        if (getArguments() != null) {
            initData(getArguments().getInt("quality"));
        }
        return mView;
    }

    // TODO: Rename method, update argument and hook method into UI event
    public void onBackLabel(Boolean isRequirePassword) {
        if (mCallbacks != null) {
            mCallbacks.onFragmentInteraction(isRequirePassword);
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
    public void onDetach() {
        super.onDetach();
        mCallbacks = null;
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.encrypt_require_password:
                setRequirePasswordState(true);
                break;
            case R.id.encrypt_no_require_password:
                setRequirePasswordState(false);
                break;
            default:
                break;
        }
    }
}
