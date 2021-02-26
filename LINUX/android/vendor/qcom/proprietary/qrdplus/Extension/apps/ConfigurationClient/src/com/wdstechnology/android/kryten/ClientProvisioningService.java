/**
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc
 */


package com.wdstechnology.android.kryten;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;

/**
 * No functionality is added, it is dummy service class which can handle
 * marker intent and make this default application to handle SMS provisioning.
 * Please check manifest file to know the intent it can handle.
 */

public class ClientProvisioningService extends Service {
    public ClientProvisioningService() {
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

}
