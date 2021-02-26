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
import android.os.Binder;
import android.os.IBinder;
import android.os.RemoteException;
import android.telecom.ParcelableCall;

import com.qualcomm.qti.qtisystemservice.ICallStateListener;
import com.qualcomm.qti.qtisystemservice.ITelecomCallService;

public class TelecomCallServiceDelegate extends BinderTelecomCallService{
    private ITelecomCallService mITelecomCallService;
    private static TelecomCallServiceDelegate mInstance;

    private TelecomCallServiceDelegate(Context context) {
        super(context);
        bind();
    }

    public static TelecomCallServiceDelegate getInstance(Context context) {
        if (mInstance == null) {
            mInstance = new TelecomCallServiceDelegate(context.getApplicationContext());
        }
        return mInstance;
    }

    private BinderTarget mProxy = new BinderTarget();

    private void bind() {
        mProxy.bind(new CallBack() {
            @Override
            public void onSuccess(IBinder o) {
                mITelecomCallService = ITelecomCallService.Stub.asInterface(o);
                QLog.d(TelecomCallServiceDelegate.this, "onSuccess11");
            }

            @Override
            public void onFailure() {
                mITelecomCallService = null;
                QLog.d(TelecomCallServiceDelegate.this, "failed, and rebind");
                bind();
            }
        });
    }

    public boolean hasAnyActiveCall() {
        try {
            return mITelecomCallService != null && mITelecomCallService.hasAnyActiveCall();
        } catch (RemoteException e) {
            e.printStackTrace();
        }
        return false;
    }

    public boolean hasAnyHeldCall() {
        try {
            return mITelecomCallService != null && mITelecomCallService.hasAnyHeldCall();
        } catch (RemoteException e) {
            e.printStackTrace();
        }
        return false;
    }

    public ParcelableCall getForegroundCall() {
        try {
            return mITelecomCallService != null ?
                    mITelecomCallService.getForegroundCall() : null;
        } catch (RemoteException e) {
            e.printStackTrace();
        }
        return null;
    }

    public void registerCallSateListener(final ICallStateListener listener) {
        mProxy.bind(new CallBack() {
            @Override
            public void onSuccess(IBinder o) {
                ITelecomCallService systemService = ITelecomCallService.Stub.asInterface(o);
                try {
                    systemService.registerCallStateChanged(listener);
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }

            @Override
            public void onFailure() {
            }
        });
    }

    public void unregisterCallStateListener(final ICallStateListener listener) {
        mProxy.bind(new CallBack() {
            @Override
            public void onSuccess(IBinder o) {
                ITelecomCallService systemService = ITelecomCallService.Stub.asInterface(o);
                try {
                    systemService.unregisterCallStateChanged(listener);
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }

            @Override
            public void onFailure() {
            }
        });
    }

}
