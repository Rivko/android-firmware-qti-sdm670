/*=========================================================================
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  =========================================================================*/

package com.quicinc.wipoweragent;

import android.app.Application;
import android.os.IBinder;
import android.os.SystemProperties;
import android.os.ServiceManager;
import android.content.Intent;
import android.content.IntentFilter;
import android.util.Log;

public class WipowerAgentApplication extends Application {
    private static final boolean DBG = true;
    private static final String TAG = "WiPwrAg-App";
    private boolean mRegisteredReceiver = false;
    private WipowerAgentReceiver mReceiver = new WipowerAgentReceiver();

    @Override
    public void onCreate() {
        super.onCreate();

        // check if platform supports wipower
        if (SystemProperties.getBoolean("ro.bluetooth.wipower", false) == false) {
            if (DBG) Log.d(TAG, "Wipower not supported");
            return;
        }

        IntentFilter filter = new IntentFilter();
        filter.addAction(Intent.ACTION_BOOT_COMPLETED);
        filter.addAction("com.quicinc.wbc.action.SHOW_BLUETOOTH_NEEDED_UI_DIALOG");
        filter.addAction("com.quicinc.wbc.action.WIPOWER_ICON_ENABLE");
        filter.addAction("com.quicinc.wbc.action.WIPOWER_ICON_DISABLE");
        if (!mRegisteredReceiver) {
            try {
                registerReceiver(mReceiver, filter);
                mRegisteredReceiver = true;
            } catch (Exception e) {
                Log.e(TAG, "Unable to register WipowerAgentReceiver receiver", e);
            }
        }
    }
}
