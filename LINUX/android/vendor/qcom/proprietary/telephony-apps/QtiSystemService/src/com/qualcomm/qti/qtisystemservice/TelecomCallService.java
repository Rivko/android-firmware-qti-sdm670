/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qtisystemservice;

import android.app.Service;
import android.content.Intent;
import android.telecom.ParcelableCall;
import android.os.Handler;
import android.os.IBinder;
import android.os.Looper;
import android.os.Message;
import android.os.RemoteException;


public class TelecomCallService extends Service {
    private static final int EVT_CALL_HAS_ANY_CALL_ACTIVE = 1;
    private static final int EVT_CALL_HAS_ANY_CALL_HELD = 2;
    private static final int EVT_CALL_REGISTER_CALL_STATE_LISTENER = 3;
    private static final int EVT_CALL_UNREGISTER_CALL_STATE_LISTENER = 4;
    private static final int EVT_CALL_GET_FOREGOUND_CALL = 5;

    private CallsManagerWrapper mCallsManagerWrapper;

    @Override
    public void onCreate() {
        QLog.d(this, "onCreate");
        mCallsManagerWrapper = CallsManagerWrapper.getInstance(this);
    }

    @Override
    public IBinder onBind(Intent intent) {
        return mTelecomCallBinder;
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        return START_STICKY;
    }

    private static final class ThreadRequest {
        Object argument; // reserve
        Object result;
        public ThreadRequest(Object args) {
            argument = args;
        }
    }

    Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case EVT_CALL_HAS_ANY_CALL_ACTIVE:
                    populateResult(msg, mCallsManagerWrapper.hasAnyActiveCall());
                    break;
                case EVT_CALL_HAS_ANY_CALL_HELD:
                    populateResult(msg, mCallsManagerWrapper.hasAnyHeldCall());
                    break;
                case EVT_CALL_REGISTER_CALL_STATE_LISTENER: {
                    ICallStateListener listener = (ICallStateListener) msg.obj;
                    if (listener != null) {
                        mCallsManagerWrapper.registerListener(listener);
                    } else {
                        QLog.d(this, "failed with null object for register");
                    }
                    break;
                }
                case EVT_CALL_UNREGISTER_CALL_STATE_LISTENER: {
                    ICallStateListener listener = (ICallStateListener) msg.obj;
                    if (listener != null) {
                        mCallsManagerWrapper.unregisterListener(listener);
                    } else {
                        QLog.d(this, "failed with null object for unregister");
                    }
                    break;
                }
                case EVT_CALL_GET_FOREGOUND_CALL:
                    populateResult(msg, mCallsManagerWrapper.getForegroundCall());
                    break;
            }
        }
    };

    IBinder mTelecomCallBinder  = new ITelecomCallService.Stub() {

        @Override
        public boolean hasAnyActiveCall() throws RemoteException {
            return (Boolean) sendRequest(EVT_CALL_HAS_ANY_CALL_ACTIVE, new ThreadRequest(null));
        }

        @Override
        public boolean hasAnyHeldCall() throws RemoteException {
            return (Boolean) sendRequest(EVT_CALL_HAS_ANY_CALL_HELD, new ThreadRequest(null));
        }

        @Override
        public void registerCallStateChanged(ICallStateListener listener) {
            mHandler.obtainMessage(EVT_CALL_REGISTER_CALL_STATE_LISTENER, listener).sendToTarget();
        }

        @Override
        public void unregisterCallStateChanged(ICallStateListener listener) {
            mHandler.obtainMessage(EVT_CALL_UNREGISTER_CALL_STATE_LISTENER, listener).sendToTarget();
        }

        @Override
        public ParcelableCall getForegroundCall() {
            return (ParcelableCall) sendRequest(EVT_CALL_GET_FOREGOUND_CALL, new ThreadRequest(null));
        }
    };

    private Object sendRequest(int cmd, Object args) {
        if (Looper.myLooper() == mHandler.getLooper()) {
            throw new RuntimeException("This method will deadlock if called from main thread");
        }

        ThreadRequest request = new ThreadRequest(args);
        mHandler.obtainMessage(cmd, request).sendToTarget();

        // Wait for the request to complete
        synchronized (request) {
            while (request.result == null) {
                try {
                    request.wait();
                } catch (InterruptedException e) {
                    // Do nothing, go back and wait until the request is complete
                }
            }
        }
        return request.result;
    }

    private void populateResult(Message msg, Object result) {
        ThreadRequest request = (ThreadRequest) msg.obj;
        do {
            request.result = result;
            if (request.result == null) request.result = new Object();
        } while (false);

        // Wake up the requesting thread
        synchronized (request) {
            request.notifyAll();
        }
    }

}
