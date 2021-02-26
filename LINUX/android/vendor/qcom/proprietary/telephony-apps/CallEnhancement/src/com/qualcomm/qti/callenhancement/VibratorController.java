/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.callenhancement;

import android.content.Context;
import android.os.RemoteException;
import android.os.SystemClock;
import android.os.SystemVibrator;
import android.os.Vibrator;
import android.provider.Settings;
import android.telecom.ParcelableCall;

import com.qualcomm.qti.qtisystemservice.ICallStateListener;

public class VibratorController extends CallSateObserver{
    private static VibratorController mInstance;
    private Context mContext;

    private VibratorController(Context context, TelecomCallServiceDelegate telCallServiceDelegate) {
        super(telCallServiceDelegate);
        mContext = context.getApplicationContext();

    }

    public static void setup(Context context, TelecomCallServiceDelegate telCallServiceDelegate) {
        if (mInstance == null) {
            mInstance = new VibratorController(context, telCallServiceDelegate);
        }
    }

    public static VibratorController getInstance() {
        if (mInstance == null) throw new RuntimeException("You must set up fristly");
        return mInstance;
    }

    @Override
    protected void onCallSateChanged(ParcelableCall call, int newState, int oldState) {
        if (newState == CALL_STATE_ACTIVE && oldState == CALL_STATE_DIALING) {
            QLog.d(this, "try to vibrate device");
            vibrateAfterCallConnected();
        }
    }

    private void vibrateAfterCallConnected() {
        int defaultVibrateEnabled = mContext.getResources()
                .getInteger(R.integer.config_default_vibrate_after_connected);
        if (Settings.System.getInt(mContext.getContentResolver(), "vibrate_on_accepted",
                defaultVibrateEnabled) == 1) {
            Vibrator systemVibrator = new SystemVibrator();
            int nVibratorLength = 100;
            QLog.d(this, "vibrating");
            systemVibrator.vibrate(nVibratorLength);
            SystemClock.sleep(nVibratorLength);
            systemVibrator.cancel();
        }
    }
}
