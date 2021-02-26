/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.callenhancement;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.IBinder;

import java.util.HashSet;
import java.util.Set;

public class BinderTelecomCallService {
    private static final String SYSTEM_SERVICE_PACKAGE = "com.qualcomm.qti.qtisystemservice";
    private IBinder mBinder;
    private ServiceConnection mServiceConnection;
    private final Set<CallBack> mCallbacks = new HashSet<>();
    private Context mContext;

    interface CallBack {
        void onSuccess(IBinder iBinder);
        void onFailure();
    }

    final class BinderTarget {
        void bind(CallBack callback) {
            if (mServiceConnection == null) {
                mCallbacks.add(callback);
                bindService();
            } else {
                callback.onSuccess(mBinder);
            }
        }
    }

    public BinderTelecomCallService(Context context) {
        mContext = context;
    }

    private void bindService() {
        if (mContext != null) {
            mContext.bindService(new Intent().setComponent(new ComponentName(SYSTEM_SERVICE_PACKAGE,
                SYSTEM_SERVICE_PACKAGE + ".TelecomCallService")),
                new BinderServiceConnection(), Context.BIND_AUTO_CREATE);
        }
    }

    public void unbindService() {
        if (mContext != null && mServiceConnection != null) {
            mContext.unbindService(mServiceConnection);
        }
    }

    private final class BinderServiceConnection implements ServiceConnection {

        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            mServiceConnection = this;
            mBinder = service;
            for (CallBack cb : mCallbacks) {
                cb.onSuccess(mBinder);
            }
            mCallbacks.clear();
        }

        @Override
        public void onServiceDisconnected(ComponentName name) {
            mServiceConnection = null;
            mBinder = null;
            for (CallBack cb : mCallbacks) {
                cb.onFailure();
            }
            mCallbacks.clear();
        }
    }
}
