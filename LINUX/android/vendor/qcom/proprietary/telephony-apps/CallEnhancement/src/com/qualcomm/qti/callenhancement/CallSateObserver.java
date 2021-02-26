/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.callenhancement;

import android.os.RemoteException;
import android.telecom.ParcelableCall;

import com.qualcomm.qti.qtisystemservice.ICallStateListener;

public abstract class CallSateObserver implements ICallSateDefinition{
    protected abstract void onCallSateChanged(ParcelableCall call, int newState, int oldState);

    protected TelecomCallServiceDelegate mTelecomCallServiceDelegate;

    public CallSateObserver(TelecomCallServiceDelegate telecomCallServiceDelegate) {
        mTelecomCallServiceDelegate = telecomCallServiceDelegate;
        mTelecomCallServiceDelegate.registerCallSateListener(mICallStateListener);
    }

    private ICallStateListener mICallStateListener = new ICallStateListener.Stub() {
        @Override
        public void onCallStateChanged(ParcelableCall call, int newState, int oldState)
                throws RemoteException {
            onCallSateChanged(call, newState, oldState);
        }
    };

    public void cleanup() {
        mTelecomCallServiceDelegate.unregisterCallStateListener(mICallStateListener);
    }
}
