/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.callfeaturessetting.cdma.CdmaCallForwardWaiting;

import android.app.Activity;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.res.TypedArray;
import android.net.Uri;
import android.telecom.PhoneAccountHandle;
import android.telecom.TelecomManager;
import android.util.AttributeSet;
import android.util.Log;
import android.widget.Toast;

import com.android.internal.telephony.CommandsInterface;
import com.qualcomm.qti.callfeaturessetting.R;

public class CdmaCallForwardEditPreference extends EditPhoneNumberPreference {
    private static final String LOG_TAG = "CdmaCallForwardEditPreference";

    private int mButtonClicked;
    private Context mContext;

    private int mPhoneId;
    private String mPrefixNumber;

    private Activity mForeground;

    int reason;

    public CdmaCallForwardEditPreference(Context context, AttributeSet attrs) {
        super(context, attrs);

        mContext = context;
        TypedArray a = context.obtainStyledAttributes(attrs,
                R.styleable.CallForwardEditPreference, 0, R.style.EditPhoneNumberPreference);
        reason = a.getInt(R.styleable.CallForwardEditPreference_reason,
                CommandsInterface.CF_REASON_UNCONDITIONAL);
        a.recycle();
    }

    public CdmaCallForwardEditPreference(Context context) {
        this(context, null);
    }

    public int getCallOptionType() {
        return reason;
    }

    public void init(Activity foreground, int phoneId, String prefixNum) {
        mForeground = foreground;
        mPhoneId = phoneId;
        mPrefixNumber = prefixNum;
        setSummary(prefixNum);
    }

    @Override
    public void onClick(DialogInterface dialog, int which) {
        mButtonClicked = which;
        Log.d(LOG_TAG, "mButtonClicked= " + mButtonClicked);
    }

    @Override
    protected void onDialogClosed(boolean positiveResult) {
        Log.d(LOG_TAG, "mButtonClicked: " + mButtonClicked
                + ", positiveResult: " + positiveResult + " mPrefixNumber: " + mPrefixNumber);
        // A positive result is technically either button1 or button3.
        if (mButtonClicked == DialogInterface.BUTTON_NEUTRAL) {
            final String forwardingNumber = getEditText().getText().toString();
            if (forwardingNumber.trim().length() == 0) {
                Toast.makeText(mContext, R.string.null_phone_number, Toast.LENGTH_LONG).show();
                return;
            }
            Intent intent = new Intent(Intent.ACTION_CALL_PRIVILEGED);
            intent.setData(Uri.fromParts("tel", mPrefixNumber + forwardingNumber, null));

            PhoneAccountHandle account = GlobalUtils.getPhoneAccountHandle(mForeground, mPhoneId);
            if (account != null) {
                intent.putExtra(
                        TelecomManager.EXTRA_PHONE_ACCOUNT_HANDLE, account);
            }
            mForeground.startActivity(intent);
        } else {
            super.onDialogClosed(positiveResult);
        }
    }
}
