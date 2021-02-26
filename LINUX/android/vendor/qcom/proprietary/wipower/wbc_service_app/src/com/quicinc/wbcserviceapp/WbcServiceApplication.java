/*=========================================================================
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  =========================================================================*/

package com.quicinc.wbcserviceapp;

import android.app.Application;
import android.os.IBinder;
import android.os.SystemProperties;
import android.os.ServiceManager;
import android.util.Log;

import com.quicinc.wbcservice.WbcService;

public class WbcServiceApplication extends Application {
    private static final String TAG = "WbcSvcApp";
    private static final String WBC_SERVICE_NAME = "wbc_service";
    private static final boolean DBG = true;

    private WbcService m_wbcService;

    @Override
    public void onCreate() {
        super.onCreate();

        // check if platform supports wipower
        if (SystemProperties.getBoolean("ro.bluetooth.wipower", false) == false) {
            if (DBG) Log.d(TAG, "Wipower not supported");
            return;
        }

        // check if wbc_service is already present in ServiceManager
        if (ServiceManager.getService(WBC_SERVICE_NAME) != null) {
            Log.i(TAG, "wbc_service already present in ServiceManager");
            return;
        }

        // instantiate and add WbcService to ServiceManager
        boolean isWbcSvcAdded = false;
        try {
            m_wbcService = new WbcService(this);
            if (m_wbcService != null) {
                ServiceManager.addService(WBC_SERVICE_NAME, (IBinder) m_wbcService);
                isWbcSvcAdded = true;
            }
        } catch (Exception e) {
            Log.e(TAG, "Error adding wbc_service in ServiceManager");
        }

        if (isWbcSvcAdded) {
            Log.i(TAG, "Started wbc_service successfully");
        } else {
            Log.e(TAG, "Could not start wbc_service!");
        }
    }
}
