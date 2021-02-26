/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.callenhancement;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.Binder;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.RemoteException;

import org.codeaurora.callenhancement.ICallEnhancement;
import org.codeaurora.callenhancement.ICallRecordStateListener;

public class CallEnhancementService extends Service {
    private static final int EVT_CALL_RECORD_START = 1;
    private static final int EVT_CALL_RECORD_STOP = 2;
    private static final int EVT_CALL_RECORD_IS_RUNNING = 3;
    private static final int EVT_CALL_RECORD_IS_AVAILABLE = 4;
    private static final int EVT_CALL_RECORD_GET_DURATION = 5;
    private static final int EVT_CALL_RECORD_REGISTER_LISTENER= 6;
    private static final int EVT_CALL_RECORD_UNREGISTER_LISTENER = 7;

    private CallRecorder mCallRecorder;
    private Handler mHandler = new CallRecorderHandler();

    @Override
    public void onCreate() {
        super.onCreate();
        QLog.d(this, "onCreate");
        CallRecorder.setup(this, TelecomCallServiceDelegate.getInstance(this));
        CallDurationController.setup(this,
            TelecomCallServiceDelegate.getInstance(this));
        VibratorController.setup(this,
            TelecomCallServiceDelegate.getInstance(this));
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        QLog.d(this, "onDestroy");
        CallRecorder.getInstance().cleanup();
        CallDurationController.teardown();
        VibratorController.getInstance().cleanup();
    }

    @Override
    public IBinder onBind(Intent intent) {
        // TODO: Return the communication channel to the service.
        return mBinder;
    }

    private Binder mBinder = new ICallEnhancement.Stub() {
        @Override
        public void startRecordingCall() throws RemoteException {
            ThreadRequestHelper.sendRequest(mHandler, EVT_CALL_RECORD_START, null);
        }

        @Override
        public void stopRecordingCall() throws RemoteException {
            ThreadRequestHelper.sendRequest(mHandler, EVT_CALL_RECORD_STOP, null);
        }

        @Override
        public long getRecordedCallDuration() throws RemoteException {
            return (long)ThreadRequestHelper.sendRequest(mHandler,
                    EVT_CALL_RECORD_GET_DURATION, null);
        }

        @Override
        public boolean isCallRecording() throws RemoteException {
            return (boolean)ThreadRequestHelper.sendRequest(mHandler,
                    EVT_CALL_RECORD_IS_RUNNING, null);
        }

        @Override
        public boolean canCallRecord() throws RemoteException {
            return (boolean)ThreadRequestHelper.sendRequest(mHandler,
                    EVT_CALL_RECORD_IS_AVAILABLE, null);
        }

        @Override
        public void registerCallRecordStateChanged(ICallRecordStateListener listener)
                throws RemoteException {
            mHandler.obtainMessage(EVT_CALL_RECORD_REGISTER_LISTENER, listener).sendToTarget();
        }

        @Override
        public void unregisterCallRecordStateChanged(ICallRecordStateListener listener)
                throws RemoteException {
            mHandler.obtainMessage(EVT_CALL_RECORD_UNREGISTER_LISTENER, listener).sendToTarget();
        }
    };

    private final class CallRecorderHandler extends Handler {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case EVT_CALL_RECORD_START:
                    CallRecorder.getInstance().startRecordingCall();
                    ThreadRequestHelper.populateResult(msg, true);
                    break;
                case EVT_CALL_RECORD_STOP:
                    CallRecorder.getInstance().stopRecordingCall(false);
                    ThreadRequestHelper.populateResult(msg, true);
                    break;
                case EVT_CALL_RECORD_IS_RUNNING:
                    ThreadRequestHelper.populateResult(msg,
                            CallRecorder.getInstance().isCallRecording());
                    break;
                case EVT_CALL_RECORD_IS_AVAILABLE:
                    ThreadRequestHelper.populateResult(msg,
                            CallRecorder.getInstance().canCallRecord());
                    break;
                case EVT_CALL_RECORD_GET_DURATION:
                    ThreadRequestHelper.populateResult(msg,
                            CallRecorder.getInstance().getRecordedCallDuration());
                    break;
                case EVT_CALL_RECORD_REGISTER_LISTENER:
                    CallRecorder.getInstance().
                            registerListener((ICallRecordStateListener)msg.obj);
                    break;
                case EVT_CALL_RECORD_UNREGISTER_LISTENER:
                    CallRecorder.getInstance().
                            unregisterListener((ICallRecordStateListener)msg.obj);
                    break;
            }
        }
    }
}
