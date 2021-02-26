/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.internal.telephony;

import android.content.ActivityNotFoundException;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Intent;
import android.content.IntentFilter;
import android.hardware.radio.V1_0.RegState;
import android.hardware.radio.V1_0.VoiceRegStateResult;
import android.os.AsyncResult;
import android.os.Bundle;
import android.os.Message;
import android.content.Context;
import android.telephony.ServiceState;

import android.util.Log;
import com.android.internal.annotations.VisibleForTesting;
import com.android.internal.telephony.CommandsInterface;
import com.android.internal.telephony.GsmCdmaPhone;
import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.ServiceStateTracker;


public class QtiServiceStateTracker extends ServiceStateTracker {
    private static final String LOG_TAG = "QtiServiceStateTracker";
    private static final boolean DBG = true;
    private static final boolean VDBG = false;  // STOPSHIP if true
    private static final String ACTION_MANAGED_ROAMING_IND =
            "codeaurora.intent.action.ACTION_MANAGED_ROAMING_IND";
    private final String ACTION_RAC_CHANGED = "qualcomm.intent.action.ACTION_RAC_CHANGED";
    private final String mRatInfo = "rat";
    private final String mRacChange = "rac";
    private int mRac;
    private int mRat;
    private int mTac = -1;

    private BroadcastReceiver mIntentReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            if (intent.getAction().equals(ACTION_RAC_CHANGED)) {
                Bundle bundle = intent.getExtras();
                if (bundle != null) {
                    mRac = bundle.getInt(mRacChange);
                    mRat = bundle.getInt(mRatInfo);
                }
            }
        }
    };

    public QtiServiceStateTracker(GsmCdmaPhone phone, CommandsInterface ci) {
        super(phone,ci);
        IntentFilter filter = new IntentFilter();
        filter.addAction(ACTION_RAC_CHANGED);
        phone.getContext().registerReceiver(mIntentReceiver, filter);
    }

    @Override
    protected void handlePollStateResultMessage(int what, AsyncResult ar) {
        switch (what) {
            case EVENT_POLL_STATE_REGISTRATION: {
                super.handlePollStateResultMessage(what, ar);
                if (mPhone.isPhoneTypeGsm()) {
                    VoiceRegStateResult voiceRegStateResult = (VoiceRegStateResult) ar.result;
                    int regState =  voiceRegStateResult.regState;

                    if (regState == RegState.REG_DENIED || regState == RegState.REG_DENIED_EM) {
                        int rejCode = voiceRegStateResult.reasonForDenial;
                        // Check if rejCode is "Persistent location update reject",
                        if (rejCode == 10) {
                            log(" Posting Managed roaming intent sub = "
                                    + mPhone.getSubId());
                            try {
                                Intent intent =
                                        new Intent(ACTION_MANAGED_ROAMING_IND);
                                intent.setComponent(new ComponentName(
                                        "com.qualcomm.qti.networksetting",
                                        "com.qualcomm.qti.networksetting.ManagedRoaming"));
                                intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                                intent.putExtra(PhoneConstants.SUBSCRIPTION_KEY,
                                        mPhone.getSubId());
                                mPhone.getContext().startActivity(intent);
                            } catch (ActivityNotFoundException e) {
                                loge("unable to start activity: " + e);
                            }
                        }
                    }
                }
                break;
            }

            default:
                super.handlePollStateResultMessage(what, ar);
        }
    }

    @Override
    public void pollState(boolean modemTriggered) {
        if (mPhone.mCi.getRadioState() == CommandsInterface.RadioState.RADIO_OFF
                && (!isDeviceShuttingDown() || modemTriggered)
                && ServiceState.RIL_RADIO_TECHNOLOGY_IWLAN != mSS.getRilDataRadioTechnology()) {
            mSS.setStateOutOfService();
        }
        super.pollState(modemTriggered);
    }

    @VisibleForTesting
    public int[] getPollingContext() {
        return mPollingContext;
    }
}
