/*
 *Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 *All Rights Reserved.
 *Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 *Not a Contribution.
 *Apache license notifications and license are retained
 *for attribution purposes only.
 *
 *  Copyright (C) 2008 The Android Open Source Project

 *  Licensed under the Apache License, Version 2.0 (the "License");
 *you may not use this file except in compliance with the License.
 *You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *Unless required by applicable law or agreed to in writing, software
 *distributed under the License is distributed on an "AS IS" BASIS,
 *WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *See the License for the specific language governing permissions and
 *limitations under the License.
 */

package com.qualcomm.qti.callfeaturessetting.gsm.callbarring;

import android.app.AlertDialog;
import android.content.Context;
import android.preference.EditTextPreference;
import android.text.InputType;
import android.text.method.DigitsKeyListener;
import android.text.method.PasswordTransformationMethod;
import android.util.AttributeSet;
import android.view.View;
import android.widget.EditText;

import com.qualcomm.qti.callfeaturessetting.R;

import java.util.Map;

public class EditPinPreference extends EditTextPreference {

    private boolean shouldHideButtons;

    public interface OnPinEnteredListener {
        void onPinEntered(EditPinPreference preference, boolean positiveResult);
    }

    private OnPinEnteredListener mPinListener;

    public void setOnPinEnteredListener(OnPinEnteredListener listener) {
        mPinListener = listener;
    }

    public EditPinPreference(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    public EditPinPreference(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
    }

    @Override
    protected View onCreateDialogView() {
        setDialogLayoutResource(R.layout.pref_dialog_edit_pin);

        View dialog = super.onCreateDialogView();

        getEditText().setInputType(InputType.TYPE_CLASS_NUMBER |
            InputType.TYPE_NUMBER_VARIATION_PASSWORD);

        return dialog;
    }

    @Override
    protected void onBindDialogView(View view) {
        super.onBindDialogView(view);

        // If the layout does not contain an edittext, hide the buttons.
        shouldHideButtons = (view.findViewById(android.R.id.edit) == null);
    }

    @Override
    protected void onPrepareDialogBuilder(AlertDialog.Builder builder) {
        super.onPrepareDialogBuilder(builder);

        // hide the buttons if we need to.
        if (shouldHideButtons) {
            builder.setPositiveButton(null, this);
            builder.setNegativeButton(null, this);
        }
    }

    @Override
    protected void onDialogClosed(boolean positiveResult) {
        super.onDialogClosed(positiveResult);
        if (mPinListener != null) {
            mPinListener.onPinEntered(this, positiveResult);
        }
    }

    public void showPinDialog() {
        showDialog(null);
    }
}
