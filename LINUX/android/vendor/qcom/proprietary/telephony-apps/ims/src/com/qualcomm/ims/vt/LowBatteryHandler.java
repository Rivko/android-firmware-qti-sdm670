/* Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */
package com.qualcomm.ims.vt;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.BatteryManager;
import android.util.Log;
import android.widget.Toast;

import com.android.ims.ImsReasonInfo;

import java.util.List;

import org.codeaurora.ims.DriverCallIms;
import org.codeaurora.ims.ICallListListener;
import org.codeaurora.ims.ImsCallSessionImpl;
import org.codeaurora.ims.ImsCallUtils;
import org.codeaurora.ims.ImsServiceSub;
import org.codeaurora.ims.QtiCallConstants;
import org.codeaurora.ims.utils.QtiImsExtUtils;

/**
 * This class listens for battery status changes and if device is in
 * low battery, for SKT operator this class shall:
 * 1. disconnect all active VT calls.
 * 2. disconnect MO/MT VT calls
 * 3. reject upgrade requests
 * For RJIL operator, calls will not be disconnected but this class informs
 * all the alive sessions that battery is low
 */
public class LowBatteryHandler implements ICallListListener {

    private static String TAG = "VideoCall_LowBattery";
    private static LowBatteryHandler sInstance;
    private Context mContext;
    private ImsServiceSub mServiceSub;
    private boolean mIsLowBattery = false;
    private final boolean isCarrierOneSupported = ImsCallUtils.isCarrierOneSupported();
    private int mPhoneId = QtiCallConstants.INVALID_PHONE_ID;

    private LowBatteryHandler(ImsServiceSub serviceSub, Context context) {
        mContext = context;
        mServiceSub = serviceSub;
        mPhoneId = mServiceSub.getPhoneId();
        mContext.registerReceiver(mBatteryLevel,
                new IntentFilter(Intent.ACTION_BATTERY_CHANGED));
    }

    /**
     * Set active command interface instance
     */
    public void onActiveSubChanged(ImsServiceSub serviceSub) {
        if (serviceSub == null) {
            Log.e(TAG, "setActiveSub: serviceSub is null");
            return;
        }

        if (!mServiceSub.equals(serviceSub)) {
            mContext.unregisterReceiver(mBatteryLevel);
            mServiceSub = serviceSub;
            mPhoneId = mServiceSub.getPhoneId();
            mContext.registerReceiver(mBatteryLevel,
                    new IntentFilter(Intent.ACTION_BATTERY_CHANGED));
        }
    }

    /**
     * Creates an instance of the Low battery handler class.
     *
     * @param serviceSub command interface instance
     * @param context context of this class
     *
     * @return Low battery handler instance
     */
    public static synchronized LowBatteryHandler init(ImsServiceSub serviceSub, Context context) {
        if (sInstance == null) {
            sInstance = new LowBatteryHandler(serviceSub, context);
        } else {
            throw new RuntimeException("LowBatteryHandler: Multiple initialization");
        }
        return sInstance;
    }

    /**
     * Get instance of Low battery handler.
     * @return Low battery handler instance
     */
    public static synchronized LowBatteryHandler getInstance() {
        if (sInstance != null) {
            return sInstance;
        } else {
            throw new RuntimeException("LowBatteryHandler: Not initialized");
        }
    }

    /**
     * Brodcast receiver to handle battery events.
     */
    private BroadcastReceiver mBatteryLevel = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();
            if (Intent.ACTION_BATTERY_CHANGED.equals(action)) {
                boolean allowVideoCallsInLowBattery = QtiImsExtUtils.
                        allowVideoCallsInLowBattery(mPhoneId, mContext);
                mIsLowBattery = isLowBattery(intent);
                if (mIsLowBattery && !allowVideoCallsInLowBattery) {
                    // Disconnect video calls only for SKT.
                    Log.d(TAG, "disconnectVideoCalls on low battery");
                    disconnectVideoCalls();
                }

                if (!allowVideoCallsInLowBattery || isCarrierOneSupported) {
                    // Update low battery status for both RJIL and SKT.
                    updateLowBatteryStatus();
                }
            }
        }
    };

    @Override
    public void onCallSessionAdded(ImsCallSessionImpl callSession) {
        Log.d(TAG, "onCallSessionAdded callSession = " + callSession);

        if (!isCarrierOneSupported) {
            return;
        }

        if (mIsLowBattery && ImsCallUtils.isIncomingCall(callSession)) {
            List<ImsCallSessionImpl> sessionList =
                    mServiceSub.getCallSessionByState(DriverCallIms.State.DIALING);
            if (!sessionList.isEmpty()) {
                /*
                 * There can be a possibility to receive incoming call when MO low
                 * battery video call is deferred waiting to get user input whether
                 * to continue the call as Video call or convert to voice call.
                 * In such cases, allow the MT call by terminating the pending
                 * MO low battery video call.
                 */
                sessionList.get(0).terminate(ImsReasonInfo.CODE_USER_TERMINATED);
            }
        }
    }

    @Override
    public void onCallSessionRemoved(ImsCallSessionImpl callSession) {
        //Dummy
    }

    /*
     * Retuns TRUE if UE is under low battery else FALSE
     */
    private boolean isLowBattery(Intent intent) {
        //get the current battery level
        final int batteryLevel = intent.getIntExtra(BatteryManager.EXTRA_LEVEL, -1);

        //determine whether device is under low battery or not based on battery level
        return (batteryLevel <= (mContext.getResources().getInteger(
                com.android.internal.R.integer.config_lowBatteryWarningLevel)));
    }

    /**
     * To know current battery status of device.
     * @return boolean, true if device battery status is LOW
     *                  false if device battery status is OK
     */
    public boolean isLowBattery() {
        if (isCarrierOneSupported ||
                !(QtiImsExtUtils.allowVideoCallsInLowBattery(mPhoneId, mContext))) {
            return mIsLowBattery;
        }
        return false;
    }

    /**
      * Sends low battery update to all existing sessions
      */
    private void updateLowBatteryStatus() {
        mServiceSub.updateLowBatteryStatus();
    }

    /**
     * To disconnect all active and hold video calls
     */
    private void disconnectVideoCalls() {
        Log.d(TAG, "disconnectVideoCalls ");
        disconnectVideoCalls(DriverCallIms.State.ACTIVE);
        disconnectVideoCalls(DriverCallIms.State.HOLDING);
    }

    /**
     * Disconnect all video calls depends on state.
     */
    private void disconnectVideoCalls(DriverCallIms.State state) {
        for (ImsCallSessionImpl session : mServiceSub.getCallSessionByState(state)) {
            Log.d(TAG, "disconnectVideoCalls session : " + session);
            if (ImsCallUtils.isVideoCall(session.getInternalCallType())) {
                session.terminate(ImsReasonInfo.CODE_LOW_BATTERY);
            }
        }
    }
}
