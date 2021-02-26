/* Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package org.codeaurora.ims;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.preference.PreferenceManager;
import android.telephony.PhoneStateListener;
import android.telephony.PreciseCallState;
import android.telephony.SubscriptionInfo;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;

import com.android.internal.telephony.CommandsInterface;
import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneFactory;
import com.android.internal.telephony.TelephonyIntents;
import com.qualcomm.ims.utils.Log;

import java.util.List;

public class SmartCallForward {
    private final BroadcastReceiver mReceiver = new SubInfoBroadcastReceiver();
    private int mPhoneCount = 0;
    private PhoneStateListener[] mPhoneStateListener;
    private Context mContext;
    private Phone[] mPhoneObj;
    private boolean mCallForwardEnabled = false;
    private boolean mListenForCallStateEnabled = false;
    private ImsServiceSub mServiceSub;

    private static final String SIM_NUMBER = "sim_number_key";
    private static final String XDIVERT_STATUS = "xdivert_status_key";

    public SmartCallForward (Context context, ImsServiceSub serviceSub) {
        if (ImsCallUtils.isCarrierOneSupported() && (TelephonyManager.getDefault().
                getMultiSimConfiguration() == TelephonyManager.MultiSimVariants.DSDS)) {
            mContext = context;
            mServiceSub = serviceSub;
            mPhoneCount = TelephonyManager.getDefault().getPhoneCount();
            mPhoneStateListener = new PhoneStateListener[mPhoneCount];

            mPhoneObj = new Phone[mPhoneCount];
            for (int i = 0; i < mPhoneCount; i++) {
                mPhoneObj[i] = PhoneFactory.getPhone(i);
                mPhoneStateListener[i] = null;
            }

            IntentFilter intentFilter = new IntentFilter(
                    TelephonyIntents.ACTION_SUBINFO_RECORD_UPDATED);
            mContext.registerReceiver(mReceiver, intentFilter);
        }
    }

    private class SubInfoBroadcastReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            if (action.equals(TelephonyIntents.ACTION_SUBINFO_RECORD_UPDATED)) {
                if (isAllSubsActive()) {
                    if (!mListenForCallStateEnabled) {
                        listen();
                    }
                } else if (mListenForCallStateEnabled) {
                    unRegisterPhoneStateListener();
                }
            }
        }
    }

    private void listen() {
        TelephonyManager tm =
                (TelephonyManager) mContext.getSystemService(Context.TELEPHONY_SERVICE);
        SubscriptionManager subMgr = SubscriptionManager.from(mContext);
        Log.i(this, "Register for call state change");
        for (int i = 0; i < mPhoneCount; i++) {
            final SubscriptionInfo subInfo = subMgr.getActiveSubscriptionInfoForSimSlotIndex(i);

            if (subInfo == null) {
                Log.e(this, "register for call state change subInfo : " + subInfo);
                return;
            }

            int subId = subInfo.getSubscriptionId();
            Log.i(this, "Register for call state change subId : " + subId + " for phone Id: " + i);
            tm.listen(getPhoneStateListener(i, subId),
                    PhoneStateListener.LISTEN_PRECISE_CALL_STATE);
        }
        mListenForCallStateEnabled = true;
    }

    private void unRegisterPhoneStateListener() {
        Log.i(this, "unRegister for call state change");
        mListenForCallStateEnabled = false;
        TelephonyManager tm =
               (TelephonyManager) mContext.getSystemService(Context.TELEPHONY_SERVICE);
        for (int i = 0; i < mPhoneCount; i++) {
            if (mPhoneStateListener[i] != null) {
                tm.listen(mPhoneStateListener[i], PhoneStateListener.LISTEN_NONE);
                mPhoneStateListener[i] = null;
            }
        }
    }

    private PhoneStateListener getPhoneStateListener(int phoneId, int subId) {
        mPhoneStateListener[phoneId]  = new PhoneStateListener(subId) {
            @Override
            public void onPreciseCallStateChanged(PreciseCallState callState) {
                Log.i(this, "onPreciseCallStateChanged : mSubId " + mSubId);
                processSmartCallForward(mSubId, callState);
            }
        };
        return mPhoneStateListener[phoneId];
    }

    private boolean isActiveOrHoldingCallPresent(PreciseCallState callState) {
        return ((callState.getForegroundCallState()) ==
                PreciseCallState.PRECISE_CALL_STATE_ACTIVE) ||
                ((callState.getBackgroundCallState()) ==
                PreciseCallState.PRECISE_CALL_STATE_HOLDING);
    }

    private void processSmartCallForward(int subId, PreciseCallState callState) {
        int phoneId = SubscriptionManager.getPhoneId(subId);
        if (phoneId == SubscriptionManager.INVALID_PHONE_INDEX) {
            Log.e(this, "processSmartCallForward invalid phone ID");
            return;
        }

        if (!getXDivertStatus()) {
            Log.e(this, "processSmartCallForward xdivert is not enabled");
            return;
        }

        String lineNumber = getLineNumber(phoneId);
        if (lineNumber == null) {
            Log.e(this, "processSmartCallForward invalid phone number");
            return;
        }

        enableSmartCallForward((mServiceSub.isVoiceSupportedOverWifi() ||
                mServiceSub.isVideoSupportedOverWifi()) && isActiveOrHoldingCallPresent(callState),
                getNextPhoneId(phoneId), lineNumber);
    }

    private void enableSmartCallForward(boolean enable, int phoneId, String lineNumber) {
        Log.i(this, "enableSmartCallForward enable: " + enable + " phoneId: " + phoneId +
                " current callforward status: " + mCallForwardEnabled);
        if (enable) {
            if (!mCallForwardEnabled) {
                mCallForwardEnabled = true;
                mPhoneObj[phoneId].setCallForwardingOption(CommandsInterface.CF_ACTION_ENABLE,
                        CommandsInterface.CF_REASON_BUSY, lineNumber, 0, null);
            }
        } else if (mCallForwardEnabled) {
            mCallForwardEnabled = false;
            mPhoneObj[phoneId].setCallForwardingOption(CommandsInterface.CF_ACTION_ERASURE,
                    CommandsInterface.CF_REASON_BUSY, lineNumber, 0, null);
        }
    }

    private int getNextPhoneId(int phoneId) {
        int nextPhoneId = ++phoneId;
        if (nextPhoneId >= mPhoneCount) {
            nextPhoneId = 0;
        }
        return nextPhoneId;
    }

    private boolean isSlotActive(int slotId) {
        boolean slotActive = false;
        SubscriptionManager mgr = SubscriptionManager.from(mContext);
        List<SubscriptionInfo> activeSubList = mgr.getActiveSubscriptionInfoList();
        if (activeSubList != null) {
            for (SubscriptionInfo subInfoRecord : activeSubList) {
                if (subInfoRecord.getSimSlotIndex() == slotId) {
                    slotActive = true;
                    break;
                }
            }
        }
        return slotActive;
    }

    private boolean isAllSubsActive() {
        for (int i = 0; i < mPhoneCount; i++) {
            if (!isSlotActive(i)) return false;
        }
        return true;
    }

    private String getLineNumber(int phoneId) {
        Context context = PhoneFactory.getDefaultPhone().getContext();
        SharedPreferences sp = PreferenceManager.getDefaultSharedPreferences(context);
        String lineNumber = sp.getString(SIM_NUMBER + phoneId, null);
        Log.i(this, "getLineNumber phoneId: " + phoneId + " number: " + lineNumber);
        return lineNumber;
    }

    private boolean getXDivertStatus() {
        Context context = PhoneFactory.getDefaultPhone().getContext();
        SharedPreferences sp = PreferenceManager.getDefaultSharedPreferences(context);
        boolean status = sp.getBoolean(XDIVERT_STATUS, false);
        return status;
    }
}
