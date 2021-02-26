/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc. 
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 */

package org.simalliance.openmobileapi.service;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.UserHandle;
import android.telephony.SubscriptionManager;
import android.util.Log;
import com.android.internal.telephony.IccCardConstants;
import com.android.internal.telephony.PhoneConstants;
import org.simalliance.openmobileapi.service.SmartcardService;

public class SmartcardServiceBootCompletedBroadcastReceiver extends BroadcastReceiver {
    public final static String _TAG = "SmartcardService";
    private final static String simStateChangedPerm = "org.simalliance.openmobileapi.permission.SIM_STATE_CHANGED";
    @Override
    public void onReceive(Context context, Intent intent) {
        final boolean bootCompleted = intent.getAction().equals("android.intent.action.BOOT_COMPLETED");
        final boolean simStateChanged = intent.getAction().equals("android.intent.action.SIM_STATE_CHANGED");

        if (bootCompleted && !SmartcardService.hasStarted) {
            Log.i(_TAG, " Received broadcast Boot completed - Starting smartcard service");
            Intent serviceIntent = new Intent(context, org.simalliance.openmobileapi.service.SmartcardService.class);
            context.startServiceAsUser(serviceIntent, UserHandle.CURRENT);
        }
        if (simStateChanged) {
            final String stateExtra = intent.getStringExtra(IccCardConstants.INTENT_KEY_ICC_STATE);
            final int slotId = intent.getIntExtra(PhoneConstants.SLOT_KEY,
                    SubscriptionManager.getPhoneId(SubscriptionManager.getDefaultSubscriptionId()));
            Intent simIntent = new Intent();
            simIntent.setAction(SmartcardService.simStateChangedAction);
            if (stateExtra != null)
                simIntent.putExtra("SimState", stateExtra);
            simIntent.putExtra("SimSlot", slotId);
            context.sendBroadcastAsUser(simIntent, UserHandle.CURRENT, simStateChangedPerm);
        }
    }
};
