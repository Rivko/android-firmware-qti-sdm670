/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.service.diag;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.PowerManager;

import com.qualcomm.qti.qmmi.model.CaseManager;
import com.qualcomm.qti.qmmi.utils.LogUtils;

public class MmiDiagService extends Service {

    private MmiDiagJNIInterface diagInterface;
    private CaseManager mCaseManager = null;
    Handler callbackHandler;

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    public int onStartCommand(Intent intent, int flags, int startId) {

        if (intent != null) {
            String fileBaseDir = intent.getStringExtra("fileBaseDir");
            LogUtils.logi("Diag service onStartCommand with file base is:" + fileBaseDir);

            /**set file base dir*/
            if (MmiDiagJNIInterface.isMmiJniLoaded) {
                LogUtils.logi("libmmi_jni lib ready and start to set base dir");
                native_set_file_base_dir(fileBaseDir);
            } else {
                LogUtils.loge("libmmi_jni lib not ready");
            }
        }

        return super.onStartCommand(intent, flags, startId);
    }

    @Override
    public void onCreate() {
        callbackHandler = new Handler() {
            public void handleMessage(Message msg) {
                String s;
                Bundle b = msg.getData();
                s = b.getString("diag_command");
                if (s.equals("reboot")) {
                    PowerManager pm = (PowerManager) getSystemService(Context.POWER_SERVICE);
                    pm.reboot(null);
                } else {
                    LogUtils.loge("Cannot handle command");
                }
            }
        };


        LogUtils.logi("diag service created");
        diagInterface = new MmiDiagJNIInterface();
        diagInterface.setHandler(callbackHandler);

        mCaseManager = CaseManager.getInstance(this.getApplicationContext());
        diagInterface.setCaseManager(mCaseManager);
    }

    @Override
    public void onDestroy() {
        LogUtils.logi( "diag service destroy");
    }

    private native void native_set_file_base_dir(String fileBaseDir);
}
