/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.callenhancement;

import android.content.Context;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.os.RemoteException;
import android.provider.Settings;
import android.telecom.DisconnectCause;
import android.telecom.ParcelableCall;
import android.widget.Toast;

import com.android.internal.telephony.BlockChecker;

import com.qualcomm.qti.qtisystemservice.ICallStateListener;

public class CallDurationController extends CallSateObserver{
    private static CallDurationController mInstance;
    private static final int EVT_SHOW_DURATION = 1;
    private Context mContext;
    private ParcelableCall mForgroundCall;

    private CallDurationController(Context context, TelecomCallServiceDelegate telCallDelegate) {
        super(telCallDelegate);
        mContext = context.getApplicationContext();
    }

    public static void setup(Context context, TelecomCallServiceDelegate telCallServiceDelegate) {
        if (mInstance == null &&  context != null && context.getResources()
                .getBoolean(R.bool.call_duration_prompt_enabled)) {
            mInstance = new CallDurationController(context, telCallServiceDelegate);
        }
    }

    public static CallDurationController getInstance(Context context) {
        if (mInstance == null || context != null && context.getResources()
                .getBoolean(R.bool.call_duration_prompt_enabled)) {
            throw new RuntimeException("You did not set up firstly");
        }
        return mInstance;
    }

    public static void teardown() {
        if (mInstance != null) {
            mInstance.cleanup();
        }
    }

    private Handler mHandler = new Handler(Looper.getMainLooper()) {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case EVT_SHOW_DURATION:
                    Toast.makeText(mContext, getDuration((long)msg.obj),
                            Toast.LENGTH_LONG).show();
                    break;
            }
        }
    };

    private String getDuration(long duration) {
        long minutes = 0, seconds = 0;
        if (duration != 0) {
            duration = (System.currentTimeMillis() - duration) / 1000;
        }
        if (duration >= 60) {
            minutes = duration / 60;
        }
        seconds = duration % 60;
        QLog.d(this, "show duration = " + duration);
        return mContext.getResources().getString(R.string.duration) + minutes +
                mContext.getResources().getString(R.string.mins) + seconds +
                mContext.getResources().getString(R.string.secs);
    }

    @Override
    protected void onCallSateChanged(ParcelableCall call, int newState, int oldState) {
        if (call == null) return;
        if (Settings.System.getInt(mContext.getContentResolver(), "show_call_duration", 1) != 1) {
            QLog.d(this, "not need show duration, setting is disabled ");
            return;
        }
        String number = call.getHandle() == null ? "" : call.getHandle().getSchemeSpecificPart();
        if (newState == CALL_STATE_ACTIVE) {
            mForgroundCall = mTelecomCallServiceDelegate.getForegroundCall();
        }
        if (newState == CALL_STATE_DISCONNECTED
                && !BlockChecker.isBlocked(mContext, number)) {
            QLog.d(this, "try to show duration");
            if (mForgroundCall != null && call != null
                    && mForgroundCall.getId().equals(call.getId())) {
                DisconnectCause cause = call.getDisconnectCause();
                if (cause.getCode() == DisconnectCause.REMOTE
                        || cause.getCode() == DisconnectCause.LOCAL) {
                    mHandler.obtainMessage(EVT_SHOW_DURATION,
                            call.getConnectTimeMillis()).sendToTarget();;
                }
            }
        }
    }
}
