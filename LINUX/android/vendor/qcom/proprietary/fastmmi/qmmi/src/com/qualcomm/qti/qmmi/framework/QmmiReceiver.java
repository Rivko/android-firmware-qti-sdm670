/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.framework;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;

import com.qualcomm.qti.qmmi.utils.LogUtils;
import com.qualcomm.qti.qmmi.utils.Utils;

public class QmmiReceiver extends BroadcastReceiver {

    @Override
    public void onReceive(Context context, Intent intent) {
        String bootmode = Utils.getSystemProperties("ro.bootmode", "00");
        LogUtils.logi("bootmode:" + bootmode);
        if (intent.getAction().equals(Intent.ACTION_BOOT_COMPLETED) && bootmode.equals("ffbm-02")) {
            LogUtils.logi("receive boot complete");
            startMainActivity(context);
        } else if (intent.getAction().equals("android.provider.Telephony.SECRET_CODE")) {
            LogUtils.logi("receive SECRET_CODE ");
            startMainActivity(context);
        }
    }

    private void startMainActivity(Context context){
        Intent i = new Intent(Intent.ACTION_MAIN);
        i.setClass(context, MainActivity.class);
        i.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        context.startActivity(i);
    }
}
