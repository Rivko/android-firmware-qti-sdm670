/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.gsma.services.nfc;

import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.content.ComponentName;

public class ServiceUtils{
    static public boolean bindService(Context context, ServiceConnection connection) {
        Intent intent = new Intent(IGsmaService.class.getName());
        intent.setClassName("com.qualcomm.qti.gsma.services.nfc",
                            "com.qualcomm.qti.gsma.services.nfc.GsmaService");
        return context.bindService(intent, connection, Context.BIND_AUTO_CREATE);
    }
}
