/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc. 
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package org.simalliance.openmobileapi.service;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.nfc.NfcAdapter;
import android.os.UserHandle;
import android.util.Log;
import org.simalliance.openmobileapi.service.SmartcardService;

public class StartNfcIntentsService extends BroadcastReceiver {
    public final static String _TAG = "SmartcardService - NfcIntents";
    private final String NfcIntentClass = "org.simalliance.openmobileapi.service.NfcIntentsService";
    Intent serviceIntent;

    @Override
    public void onReceive(Context context, Intent intent) {
        final boolean bootCompleted = intent.getAction().equals("android.intent.action.BOOT_COMPLETED");
        final boolean nfcAdapterAction = intent.getAction().equals(NfcAdapter.ACTION_ADAPTER_STATE_CHANGED);
        final boolean nfcAdapterOn = nfcAdapterAction && (intent.getIntExtra(NfcAdapter.EXTRA_ADAPTER_STATE, NfcAdapter.STATE_OFF) == NfcAdapter.STATE_ON); // is NFC Adapter turned on ?
        NfcAdapter mnfcAdapter = NfcAdapter.getDefaultAdapter(context);

        if (bootCompleted || nfcAdapterOn) {

            if (nfcAdapterOn)
                Log.v(_TAG, "Received broadcast NFC is ON - Starting NfcIntentsService");
            else if (bootCompleted)
                Log.v(_TAG, "Received broadcast Boot completed - Starting NfcIntentsService");

            if (mnfcAdapter != null) {
                try {
                    Class NfcISclass = Class.forName(NfcIntentClass);
                    serviceIntent = new Intent();
                    serviceIntent.setClassName("org.simalliance.openmobileapi.service" ,NfcIntentClass);
                    context.startServiceAsUser(serviceIntent, UserHandle.CURRENT);
                } catch (ClassNotFoundException e) {
                    Log.e(_TAG, "NfcIntentsService not found - service not started");
                }
            } else {
                Log.e(_TAG, "No Nfc Adapter found");
            }

            if (!SmartcardService.hasStarted) {
                Log.v(_TAG, "Starting SmartCardService");
                serviceIntent = new Intent(context, org.simalliance.openmobileapi.service.SmartcardService.class);
                context.startServiceAsUser(serviceIntent, UserHandle.CURRENT);
            }
        }
    }
};
