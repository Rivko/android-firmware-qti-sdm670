/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 *
 * Copyright (C) 2012 The Android Open Source Project
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
 * limitations under the License.
 */

package com.qualcomm.qti.server.wigigapp;

import android.app.AlertDialog;
import android.content.Context;
import android.net.wifi.WifiInfo;
import android.net.wifi.WpsInfo;
import android.os.Bundle;
import android.os.Handler;
import android.text.Editable;
import android.text.TextUtils;
import android.text.TextWatcher;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ProgressBar;
import android.widget.TextView;

import java.util.Timer;
import java.util.TimerTask;

import com.qualcomm.qti.wigig.WigigManager;

public class WpsDialog extends AlertDialog {

    private final static String TAG = "WpsDialog";
    private static final String DIALOG_STATE = "android:dialogState";
    private static final String DIALOG_MSG_STRING = "android:dialogMsg";
    private static final int PIN_CODE_4_LEN = 4;
    private static final int PIN_CODE_8_LEN = 8;

    private TextView mTextView;
    private ProgressBar mTimeoutBar;
    private ProgressBar mProgressBar;
    private Button mButtonCancel;
    private Button mButtonConnect;
    private Timer mTimer;
    private EditText mPinCodeInput;

    private static final int WPS_TIMEOUT_S = 120;
    private int labelTimeout = 0;

    private final WigigManager mWigigManager;
    private final WigigManager.WpsCallback mWpsListener;
    private final int mWpsSetup;

    private final Context mContext;
    private final Handler mHandler = new Handler();
    private String mMsgString = "";

    private enum DialogState {
        WPS_INIT,
        WPS_START,
        WPS_COMPLETE,
        CONNECTED, //WPS + IP config is done
        WPS_FAILED
    }
    private DialogState mDialogState = DialogState.WPS_INIT;

    public WpsDialog(Context context, int wpsSetup, WigigManager wigigManager) {
        super(context);

        mContext = context;
        mWpsSetup = wpsSetup;
        mWigigManager = wigigManager;

        class WpsListener extends WigigManager.WpsCallback {
            public void onStarted(String pin) {
                if (!TextUtils.isEmpty(pin)) {
                    updateDialog(DialogState.WPS_START, String.format(
                            mContext.getString(R.string.wifi_wps_onstart_pin), pin));
                } else {
                    updateDialog(DialogState.WPS_START, mContext.getString(
                            R.string.wifi_wps_onstart_pbc));
                }
            }

            public void onSucceeded() {
                updateDialog(DialogState.WPS_COMPLETE,
                        mContext.getString(R.string.wifi_wps_complete));
            }

            public void onFailed(int reason) {
                Log.e(TAG, "Failed to start WPS session (" + reason + ")");

                String msg;
                switch (reason) {
                    case WigigManager.WPS_OVERLAP_ERROR:
                        msg = mContext.getString(R.string.wifi_wps_failed_overlap);
                        break;
                    case WigigManager.WPS_WEP_PROHIBITED:
                        msg = mContext.getString(R.string.wifi_wps_failed_wep);
                        break;
                    case WigigManager.WPS_TKIP_ONLY_PROHIBITED:
                        msg = mContext.getString(R.string.wifi_wps_failed_tkip);
                        break;
                    case WigigManager.IN_PROGRESS:
                        msg = mContext.getString(R.string.wifi_wps_in_progress);
                        break;
                    case WigigManager.WPS_TIMED_OUT:
                        msg = mContext.getString(R.string.wifi_wps_failed_timeout);
                        break;
                    default:
                        msg = mContext.getString(R.string.wifi_wps_failed_generic);
                        break;
                }
                updateDialog(DialogState.WPS_FAILED, msg);
            }
        }

        mWpsListener = new WpsListener();

        setCanceledOnTouchOutside(false);
    }

    @Override
    public Bundle onSaveInstanceState () {
        Bundle bundle  = super.onSaveInstanceState();
        bundle.putString(DIALOG_STATE, mDialogState.toString());
        bundle.putString(DIALOG_MSG_STRING, mMsgString.toString());
        return bundle;
    }

    @Override
    public void onRestoreInstanceState(Bundle savedInstanceState) {
        if (savedInstanceState != null) {
            super.onRestoreInstanceState(savedInstanceState);
            DialogState dialogState = DialogState.valueOf(savedInstanceState.getString(DIALOG_STATE));
            String msg = savedInstanceState.getString(DIALOG_MSG_STRING);
            updateDialog(dialogState, msg);
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        View mView = getLayoutInflater().inflate(R.layout.wifi_wps_dialog, null);

        mTextView = (TextView) mView.findViewById(R.id.wps_dialog_txt);
        mTextView.setText(R.string.wifi_wps_setup_msg);

        mTimeoutBar = ((ProgressBar) mView.findViewById(R.id.wps_timeout_bar));
        mTimeoutBar.setMax(WPS_TIMEOUT_S);
        mTimeoutBar.setProgress(0);

        mProgressBar = ((ProgressBar) mView.findViewById(R.id.wps_progress_bar));
        mProgressBar.setVisibility(View.GONE);

        mButtonCancel = ((Button) mView.findViewById(R.id.wps_dialog_btn_cancel));
        mButtonCancel.setText(R.string.wifi_cancel);
        mButtonCancel.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                dismiss();
                if (mDialogState == DialogState.CONNECTED)
                    ((AdvancedWifiSettings) mContext).onBackPressed();
            }
        });

        if (mWpsSetup == WpsInfo.LABEL) {
            updateDialog(DialogState.WPS_START, mContext.getString(R.string.wifi_wps_onstart_label));
            mButtonConnect = ((Button) mView.findViewById(R.id.wps_dialog_btn_connect));
            mButtonConnect.setVisibility(View.VISIBLE);
            mButtonConnect.setEnabled(false);
            mButtonConnect.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    mButtonConnect.setEnabled(false);
                    mPinCodeInput.setEnabled(false);
                    startWps(mPinCodeInput.getText().toString());
                }
            });

            mPinCodeInput = (EditText) mView.findViewById(R.id.wps_pin_code_input);
            mPinCodeInput.setVisibility(View.VISIBLE);
            mPinCodeInput.addTextChangedListener(new TextWatcher() {
                @Override
                public void beforeTextChanged(CharSequence s, int start, int count, int after) {
                }

                @Override
                public void onTextChanged(CharSequence s, int start, int before, int count) {
                }

                @Override
                public void afterTextChanged(Editable s) {
                    mButtonConnect.setEnabled(mPinCodeInput.length() == PIN_CODE_4_LEN ||
                            mPinCodeInput.length() == PIN_CODE_8_LEN);
                }
            });
        }

        setView(mView);
        super.onCreate(savedInstanceState);
    }

    void startWpsTimer() {
        /*
         * increment timeout bar per second.
         */
        mTimer = new Timer(false);
        mTimer.schedule(new TimerTask() {
            @Override
            public void run() {
                mHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        mTimeoutBar.incrementProgressBy(1);
                        if (mWpsSetup == WpsInfo.LABEL) {
                            labelTimeout++;
                            if (labelTimeout > WPS_TIMEOUT_S) {
                                mButtonConnect.setEnabled(false);
                                mPinCodeInput.setVisibility(View.GONE);
                                updateDialog(DialogState.WPS_FAILED, mContext.getString(R.string.wifi_wps_failed_timeout));
                                labelTimeout = 0;
                            }
                        }
                    }
                });
            }
        }, 1000, 1000);
    }

    @Override
    protected void onStart() {
        if (mWpsSetup != WpsInfo.LABEL) {
            startWps(null);
        }
    }

    private void startWps(String pinCodeInput) {
        startWpsTimer();

        WpsInfo wpsConfig = new WpsInfo();
        wpsConfig.setup = mWpsSetup;
        if (pinCodeInput != null)
            wpsConfig.pin = pinCodeInput;
        mWigigManager.startWps(wpsConfig, mWpsListener);
    }

    @Override
    protected void onStop() {
        if (mDialogState != DialogState.WPS_COMPLETE) {
            mWigigManager.cancelWps(null);
        }
        if (mTimer != null) {
            mTimer.cancel();
        }
    }

    public void updateDialogOnComplete(){
        if (mDialogState == DialogState.WPS_COMPLETE) {
            WifiInfo wifiInfo = mWigigManager.getConnectionInfo();
            if (wifiInfo != null) {
                String msg = String.format(mContext.getString(
                        R.string.wifi_wps_connected), wifiInfo.getSSID());
                updateDialog(DialogState.CONNECTED, msg);
            }
        }
    }

    private void updateDialog(final DialogState state, final String msg) {
        if (mDialogState.ordinal() >= state.ordinal()) {
             //ignore.
            return;
        }
        mDialogState = state;
        mMsgString = msg;

        mHandler.post(new Runnable() {
            @Override
            public void run() {
                switch(state) {
                    case WPS_COMPLETE:
                        mTimeoutBar.setVisibility(View.GONE);
                        mProgressBar.setVisibility(View.VISIBLE);
                        break;
                    case CONNECTED:
                    case WPS_FAILED:
                        mButtonCancel.setText(mContext.getString(R.string.dlg_ok));
                        mTimeoutBar.setVisibility(View.GONE);
                        mProgressBar.setVisibility(View.GONE);
                     break;
                }
                mTextView.setText(msg);
            }
        });
    }
}
