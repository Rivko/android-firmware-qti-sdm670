/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.callfeaturessetting.cdma.CdmaCallForwardWaiting;

import android.content.ComponentName;
import android.content.Context;
import android.content.res.Resources;
import android.telecom.PhoneAccountHandle;
import android.telecom.TelecomManager;
import android.telephony.SubscriptionManager;
import android.text.TextUtils;

import com.android.internal.telephony.CommandsInterface;
import com.android.internal.telephony.PhoneFactory;

import java.util.List;
import com.qualcomm.qti.callfeaturessetting.R;

public class GlobalUtils {
    public static final String LOG_TAG = "GlobalUtils";
    public static final int CALL_WAITING = 7;
    private static final int ACTIVATE = 0;
    private static final int DEACTIVATE = 1;

    public static PhoneAccountHandle getPhoneAccountHandle(Context context, int phoneId) {
        if (!SubscriptionManager.isValidPhoneId(phoneId)) {
            return null;
        }
        String subId = String.valueOf(PhoneFactory.getPhone(phoneId).getSubId());
        TelecomManager telecomManager = TelecomManager.from(context);
        List<PhoneAccountHandle> accounts = telecomManager.getCallCapablePhoneAccounts();
        for (PhoneAccountHandle account : accounts) {
            if (subId.equals(account.getId())) {
                return account;
            }
        }
        return null;
    }

    private static int getResId(int type, int state) {
        if (state == ACTIVATE) {
            switch (type) {
                case CommandsInterface.CF_REASON_UNCONDITIONAL:
                    return R.string.CFU_ACT;
                case CommandsInterface.CF_REASON_BUSY:
                    return R.string.CFB_ACT;
                case CommandsInterface.CF_REASON_NO_REPLY:
                    return R.string.CFNRY_ACT;
                case CommandsInterface.CF_REASON_NOT_REACHABLE:
                    return R.string.CFNRC_ACT;
                case CALL_WAITING:
                    return R.string.CW_ACT;
                default:
                    return R.string.CFU_ACT;
            }
        } else {
            switch (type) {
                case CommandsInterface.CF_REASON_UNCONDITIONAL:
                    return R.string.CFU_DEACT;
                case CommandsInterface.CF_REASON_BUSY:
                    return R.string.CFB_DEACT;
                case CommandsInterface.CF_REASON_NO_REPLY:
                    return R.string.CFNRY_DEACT;
                case CommandsInterface.CF_REASON_NOT_REACHABLE:
                    return R.string.CFNRC_DEACT;
                case CommandsInterface.CF_REASON_ALL:
                    return R.string.CFA_DEACT;
                case CALL_WAITING:
                    return R.string.CW_DEACT;
                default:
                    return R.string.CFU_DEACT;
            }
        }
    }

    public static String getActivateNumber(Context context, int subId, int type) {
        Resources subRes = SubscriptionManager.getResourcesForSubId(
                context, subId);
        if (subRes != null)
            return subRes.getString(getResId(type, ACTIVATE));
        else
            return null;
    }

    public static String getDeactivateNumber(Context context, int subId, int type) {
        Resources subRes = SubscriptionManager.getResourcesForSubId(
                context, subId);
        if (subRes != null)
            return subRes.getString(getResId(type, DEACTIVATE));
        else
            return null;
    }
}

