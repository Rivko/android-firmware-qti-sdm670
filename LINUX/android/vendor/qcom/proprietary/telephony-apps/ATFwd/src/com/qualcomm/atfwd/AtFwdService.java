/*******************************************************************************
    Copyright (c) 2013, 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
    Qualcomm Technologies Proprietary and Confidential.
*******************************************************************************/

package com.qualcomm.atfwd;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.os.IHwBinder;
import android.util.Log;

public class AtFwdService extends Service {

    private static final String TAG = "AtFwdService";
    private static AtCmdFwd mAtCmdFwd;

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public void onCreate() {
        Log.d(TAG, "onCreate method");
        try {
            mAtCmdFwd = new AtCmdFwd(this);
            mAtCmdFwd.registerAsService("AtCmdFwdService");
        } catch (Throwable e) {
            Log.e(TAG, "Starting AtCmdFwd Service", e);
        }
    }

    @Override
    public void onDestroy() {
        Log.d(TAG, "AtFwdService Destroyed Successfully...");
        super.onDestroy();
    }
}
