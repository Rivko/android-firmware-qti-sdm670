/*
 * Copyright (c) 2015,2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.ui;

import android.app.ActionBar;
import android.app.admin.DevicePolicyManager;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.text.InputFilter;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.os.ServiceManager;
import android.util.Log;
import com.android.internal.widget.ILockSettings;
import com.android.internal.widget.LockPatternUtils;
import android.os.RemoteException;
import android.app.ActivityManagerNative;
import android.content.DialogInterface;
import android.annotation.TargetApi;
import android.os.Build;
import com.android.internal.widget.VerifyCredentialResponse;

import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.Consts;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.R;

public class PinActivity extends VoicePrintActivity {

    private ILockSettings mLockSettingsService;
    private DevicePolicyManager mDevicePolicyManager;
    private EditText mPasswordBox;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.pin_activity);

        try {
            // TODO! Need to handle lock pattern case or no pin/password has been setup.
            if ( getLockSettings().havePassword(ActivityManagerNative.getDefault()
                                                .getCurrentUser().id) == false ) {
                Log.d(TAG, "User does not have a password/pin setup.");
                processPinResult(true);
                return;
            }
        } catch (RemoteException e) {
            e.printStackTrace();
            Log.e(TAG, "Error checking user's password status.");
        }


        // Navigation back
        ActionBar actionBar = this.getActionBar();
        actionBar.setDisplayHomeAsUpEnabled(true);

        Button accept = (Button) findViewById(R.id.accept);
        accept.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                validatePin();
            }
        });

        Button cancel = (Button) findViewById(R.id.cancel);
        cancel.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                onCancel();
            }
        });

        // Set edit text bottom color
        mPasswordBox = (EditText) findViewById(R.id.entry);
        applyEditTextStyle(mPasswordBox, new KeyboardDoneListener() {
            @Override
            public void onDone() {
                validatePin();
            }
        });

        // Device policy
        if ( getDevicePolicyManager() == null ) {
            processPinResult(false);
        } else {
            int maxLength = getDevicePolicyManager().
                            getPasswordMaximumLength(DevicePolicyManager.PASSWORD_QUALITY_UNSPECIFIED);
            mPasswordBox.setFilters(new InputFilter[] {new InputFilter.LengthFilter(maxLength)});
        }
    }

    private void validatePin() {
        try {
            VerifyCredentialResponse r = getLockSettings().checkCredential(mPasswordBox.getText().toString(),
                LockPatternUtils.CREDENTIAL_TYPE_PASSWORD,
                ActivityManagerNative.getDefault().getCurrentUser().id, null);

            processPinResult(r.getResponseCode() == VerifyCredentialResponse.RESPONSE_OK);
        } catch (RemoteException e) {
            e.printStackTrace();
            Log.e(TAG, "RemoteException validating pin.");
        }
    }

    private ILockSettings getLockSettings() {
        if (mLockSettingsService == null) {
            ILockSettings service = ILockSettings.Stub.asInterface(
                                        ServiceManager.getService("lock_settings"));
            mLockSettingsService = service;

        }
        return mLockSettingsService;

    }

    public DevicePolicyManager getDevicePolicyManager() {
        if (mDevicePolicyManager == null) {
            mDevicePolicyManager =
                (DevicePolicyManager)getSystemService(Context.DEVICE_POLICY_SERVICE);
            if (mDevicePolicyManager == null) {
                Log.e(TAG, "Cannot retrieve DevicePolicyManager!");
            }
        }
        return mDevicePolicyManager;
    }

    @Override
    public void onResume() {
        super.onResume();

        final EditText edit = (EditText) findViewById(R.id.entry);
        // Bring up keyboard
        edit.requestFocus();
        forceShowKeyboard();
    }

    @Override
    public void onPause() {
        hideKeyboard();
        super.onPause();
    }


    private void processPinResult(boolean success) {
        Log.d(TAG, "Pin valid=" + success);

        if ( !success ) {
            showDialog(this, R.string.pin_validation, R.string.pin_failed, R.string.cap_ok, new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    Intent intent = getIntent();
                    setResult(Consts.RESULT_FAILED, intent);
                    finish();
                }
            });
        } else {
            Intent intent = this.getIntent();
            setResult(Consts.RESULT_SUCCESS, intent);
            finish();
        }

    }

    private void onCancel() {
        Intent intent = this.getIntent();
        setResult(Consts.RESULT_CANCELLED, intent);
        finish();
    }
}
