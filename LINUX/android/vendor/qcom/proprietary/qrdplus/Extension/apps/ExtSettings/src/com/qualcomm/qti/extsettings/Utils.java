/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.extsettings;

import java.util.List;

import android.content.Context;
import android.telephony.SubscriptionInfo;
import android.telephony.SubscriptionManager;

public class Utils {

    public static String getSimNameFromSlotIndex(Context context, int slotId) {
        final SubscriptionInfo sir = findRecordBySlotId(context, slotId);
        if (sir != null) {
            return sir.getDisplayName().toString();
        } else {
            return context.getResources().getString(R.string.sim_card_number_title, slotId + 1);
        }
    }

    /**
     * finds a record with slotId.
     * Since the number of SIMs are few, an array is fine.
     */
    public static SubscriptionInfo findRecordBySlotId(Context context, final int slotId) {
        List<SubscriptionInfo> subInfoList =
                SubscriptionManager.from(context).getActiveSubscriptionInfoList();

        if (subInfoList != null){
            final int availableSubInfoLength = subInfoList.size();

            for (int i = 0; i < availableSubInfoLength; ++i) {
                final SubscriptionInfo sir = subInfoList.get(i);
                if (sir.getSimSlotIndex() == slotId) {
                    //Right now we take the first subscription on a SIM.
                    return sir;
                }
            }
        }
        return null;
    }
}
