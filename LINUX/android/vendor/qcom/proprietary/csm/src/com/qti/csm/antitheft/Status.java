/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qti.csm.antitheft;

import android.Manifest;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.telephony.ServiceState;
import android.telephony.SmsMessage;
import android.util.Log;
import com.android.internal.telephony.TelephonyIntents;

public class Status extends BroadcastReceiver {

    private static final String[] RUNTIME_PERMISSIONS = {Manifest.permission.RECEIVE_SMS, Manifest.permission.SEND_SMS,
            Manifest.permission.READ_PHONE_STATE};

    @Override
    public void onReceive(Context context, Intent intent) {
    }
}

