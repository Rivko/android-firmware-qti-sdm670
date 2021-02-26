/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qtisystemservice;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.os.RemoteException;


public class SystemService extends Service {

    @Override
    public void onCreate() {
        QLog.d(this, "onCreate");
    }

    @Override
    public IBinder onBind(Intent intent) {
        return mSysBinder;
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        return START_STICKY;
    }

    IBinder mSysBinder  = new ISystemService.Stub() {

        @Override
        public String getMetaInfo() throws RemoteException {
            return MetaInfoParser.getInstance().getMetaInfo();
        }
    };
}
