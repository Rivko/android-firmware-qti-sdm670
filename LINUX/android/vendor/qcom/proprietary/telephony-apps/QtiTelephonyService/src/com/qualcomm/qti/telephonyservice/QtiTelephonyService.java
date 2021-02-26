/*
 * Copyright (c) 2014, 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.telephonyservice;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.IBinder;
import android.telephony.TelephonyManager;
import android.util.Log;

import com.qualcomm.qcrilhook.QcRilHook;
import com.qualcomm.qcrilhook.IQcRilHook;

public class QtiTelephonyService extends Service {

    private static final String TAG = "QtiTelephonyService";

    private IQcRilHook mQcRilHook;
    private QtiTelephonyServiceImpl mService;
    private List<QcRilAudio> mQcRilAudioHals = new ArrayList<>();

    @Override
    public void onCreate() {
        Log.i(TAG, "onCreate:");
        mQcRilHook = new QcRilHook(getApplicationContext());
        mService = new QtiTelephonyServiceImpl(super.getApplicationContext(), mQcRilHook);

        // RIL Audio HAL initialization.
        TelephonyManager tm  = TelephonyManager.from(this);
        final int slotCount = tm.getSimCount() + 1;
        for (int i = 1; i < slotCount; ++i ) {
            mQcRilAudioHals.add(new QcRilAudio(i, getApplicationContext()));
        }
    }

    // setQcRilHook allows instrumentation to set a test double for IQcRilHook.
    // The method is intentionally private - instrumentation uses reflection to access this method.
    private void setQcRilHook(IQcRilHook qcRilHook) {
        mQcRilHook = qcRilHook;
        mService.setQcRilHook(qcRilHook);
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.d(TAG, "Service started");
        return super.onStartCommand(intent, flags, startId);
    }

    @Override
    public IBinder onBind(Intent intent) {
        Log.d(TAG, "onBind");
        return mService;
    }

    @Override
    public boolean onUnbind(Intent intent) {
        Log.d(TAG, "onUnbind");
        return super.onUnbind(intent);
    }

    @Override
    public void onDestroy() {
        Log.d(TAG, "onDestroy");
        mService = null;

        // RIL Audio HAL de-initialization
        for (QcRilAudio h : mQcRilAudioHals) {
            h.dispose();
        }
        super.onDestroy();
    }
}
