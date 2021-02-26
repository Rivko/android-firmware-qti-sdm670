/* ====================================================================
 * Copyright (c) 2014 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * =====================================================================
 * @file ColorServiceApp.java
 *
 */

package com.qti.service.colorservice;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.os.ServiceManager;
import android.util.Log;
import com.qti.snapdragon.sdk.display.IColorService;

public class ColorServiceApp extends Service {

    private static final String TAG = "ColorServiceApp";
    private static final String REMOTE_SERVICE_NAME = IColorService.class.getName();
    private IColorServiceImpl serviceImpl;


    @Override
    public IBinder onBind(Intent intent) {
        Log.d(TAG, "onBind Called");
        try {
            this.serviceImpl = new IColorServiceImpl(this);
            Log.d(TAG, "Created the service");
        } catch (InstantiationException e) {
            Log.d(TAG, "Couldnt create service instance " + e.getMessage());
            return null;
        }

        ServiceManager.addService(REMOTE_SERVICE_NAME, this.serviceImpl);
        return serviceImpl;
    }

}