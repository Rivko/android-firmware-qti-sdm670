/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.callenhancement.legacyapi;

import android.content.ContentProvider;
import android.content.ContentValues;
import android.content.Intent;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;

import com.qualcomm.qti.callenhancement.CallRecorder;
import com.qualcomm.qti.callenhancement.QLog;
import com.qualcomm.qti.callenhancement.TelecomCallServiceDelegate;
import com.qualcomm.qti.callenhancement.ThreadRequestHelper;

public class CallLegacyApiProvider extends ContentProvider {
    private static final String TAG = CallLegacyApiProvider.class.getSimpleName();
    private static final String METHOD_START_CALL_RECORD = "start_call_record";
    private static final String METHOD_STOP_CALL_RECORD = "stop_call_record";
    private static final String METHOD_IS_CALL_RECORD_RUNNING = "is_call_record_running";
    private static final String METHOD_IS_CALL_RECORD_AVAILABLE = "is_call_record_available";
    private static final String METHOD_GET_CALL_RECORD_DURATION = "get_call_record_duration";

    private static final String EXTRA_RESULT = "result";

    private static final int EVT_CALL_RECORD_START = 1;
    private static final int EVT_CALL_RECORD_STOP = 2;
    private static final int EVT_CALL_RECORD_IS_RUNNING = 3;
    private static final int EVT_CALL_RECORD_IS_AVAILABLE = 4;
    private static final int EVT_CALL_RECORD_GET_DURATION = 5;

    @Override
    public boolean onCreate() {
        // TODO: Implement this to initialize your content provider on startup.
        CallRecorder.setup(getContext(), TelecomCallServiceDelegate.getInstance(getContext()));
        return false;
    }

    @Override
    public Bundle call(String method, String arg, Bundle extras) {
        QLog.d(this, "method:" + method + ", arg:" + arg + ", extra:" + extras);
        Bundle bundle = new Bundle();
        Object rlt = null;
        if (METHOD_START_CALL_RECORD.equals(method)) {
            rlt = ThreadRequestHelper.sendRequest(mHandler, EVT_CALL_RECORD_START, extras);
            bundle.putBoolean(EXTRA_RESULT, (boolean)rlt);
        } else if (METHOD_STOP_CALL_RECORD.equals(method)) {
            rlt = ThreadRequestHelper.sendRequest(mHandler, EVT_CALL_RECORD_STOP, extras);
            bundle.putBoolean(EXTRA_RESULT, (boolean)rlt);
        } else if (METHOD_IS_CALL_RECORD_RUNNING.equals(method)) {
            rlt = ThreadRequestHelper.sendRequest(mHandler, EVT_CALL_RECORD_IS_RUNNING, extras);
            bundle.putBoolean(EXTRA_RESULT, (boolean)rlt);
        } else if (METHOD_IS_CALL_RECORD_AVAILABLE.equals(method)) {
            rlt = ThreadRequestHelper.sendRequest(mHandler, EVT_CALL_RECORD_IS_AVAILABLE, extras);
            bundle.putBoolean(EXTRA_RESULT, (boolean)rlt);
        } else if (METHOD_GET_CALL_RECORD_DURATION.equals(method)) {
            rlt = ThreadRequestHelper.sendRequest(mHandler, EVT_CALL_RECORD_GET_DURATION, extras);
            bundle.putLong(EXTRA_RESULT, (long) rlt);
        }
        return bundle;
    }

    private final Handler mHandler = new CallHandler();

    private final class CallHandler extends Handler {
        public CallHandler() {
            super(Looper.getMainLooper());
        }

        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case EVT_CALL_RECORD_START:
                    QLog.d(this, "EVT_CALL_RECORD_START");
                    CallRecorder.getInstance().startRecordingCall();
                    ThreadRequestHelper.populateResult(msg, true);
                    break;
                case EVT_CALL_RECORD_STOP:
                    QLog.d(this, "EVT_CALL_RECORD_STOP");
                    CallRecorder.getInstance().stopRecordingCall(false);
                    ThreadRequestHelper.populateResult(msg, true);
                    break;
                case EVT_CALL_RECORD_IS_RUNNING:
                    QLog.d(this, "EVT_CALL_RECORD_IS_RUNNING");
                    ThreadRequestHelper.populateResult(msg,
                            CallRecorder.getInstance().isCallRecording());
                    break;
                case EVT_CALL_RECORD_IS_AVAILABLE:
                    QLog.d(this, "EVT_CALL_RECORD_IS_AVAILABLE");
                    ThreadRequestHelper.populateResult(msg,
                            CallRecorder.getInstance().canCallRecord());
                    break;
                case EVT_CALL_RECORD_GET_DURATION:
                    QLog.d(this, "EVT_CALL_RECORD_GET_DURATION");
                    ThreadRequestHelper.populateResult(msg,
                            CallRecorder.getInstance().getRecordedCallDuration());
                    break;
            }
        }
    }

    @Override
    public Cursor query(Uri uri, String[] projection, String selection, String[] selectionArgs,
            String sortOrder) {
        QLog.d(this, "Unsupported operation, query");
        return null;
    }

    @Override
    public String getType(Uri uri) {
        QLog.d(this, "Unsupported operation, getType");
        return null;
    }

    @Override
    public Uri insert(Uri uri, ContentValues values) {
        QLog.d(this, "Unsupported operation, insert");
        return null;
    }

    @Override
    public int delete(Uri uri, String selection, String[] selectionArgs) {
        QLog.d(this, "Unsupported operation, delete");
        return 0;
    }

    @Override
    public int update(Uri uri, ContentValues values, String selection, String[] selectionArgs) {
        QLog.d(this, "Unsupported operation, update");
        return 0;
    }
}
