/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 *
 * Copyright (C) 2006 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

package org.codeaurora.ims;

import android.content.Context;
import android.os.Message;
import android.os.RegistrantList;
import android.os.Registrant;
import android.os.Handler;
import android.os.AsyncResult;
import android.os.PowerManager;
import android.os.PowerManager.WakeLock;
import android.telephony.TelephonyManager;

import com.qualcomm.ims.utils.Log;

/**
 * {@hide}
 */
public abstract class ImsPhoneBaseCommands implements ImsPhoneCommandsInterface {
    //***** Instance Variables
    protected Context mContext;
    protected RadioState mState = RadioState.RADIO_UNAVAILABLE;
    protected Object mStateMonitor = new Object();

    protected RegistrantList mRadioStateChangedRegistrants;
    protected RegistrantList mOnRegistrants;
    protected RegistrantList mAvailRegistrants;
    protected RegistrantList mOffOrNotAvailRegistrants;
    protected RegistrantList mNotAvailRegistrants;
    protected RegistrantList mCallStateRegistrants;
    protected RegistrantList mImsNetworkStateChangedRegistrants;
    protected RegistrantList mVoicePrivacyOnRegistrants;
    protected RegistrantList mVoicePrivacyOffRegistrants;
    protected RegistrantList mRingbackToneRegistrants;
    protected RegistrantList mExitEmergencyCallbackModeRegistrants;
    protected RegistrantList mRefreshViceInfoRegistrants;

    protected Registrant mSsnRegistrant;
    protected Registrant mEmergencyCallbackModeRegistrant;
    protected Registrant mRingRegistrant;

    // Type of Phone, GSM or CDMA. Set by CDMAPhone or GSMPhone.
    protected int mPhoneType;

    protected WakeLock mNotifyWakeLock;

    public ImsPhoneBaseCommands(Context context) {
        mContext = context;  // May be null (if so we won't log statistics)

        if (mContext != null) {
            PowerManager pm = (PowerManager) mContext.getSystemService(Context.POWER_SERVICE);
            if (pm != null) {
                mNotifyWakeLock = pm.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK,
                       WakeLockRegistrant.WAKELOCK_NAME);
                if (mNotifyWakeLock != null) {
                    Log.v(this, "Constructor: wakelock initialised");
                    mNotifyWakeLock.setReferenceCounted(false);
                }
            }
        } else {
            Log.e(this, "Constructor: Context is null");
        }

        mRadioStateChangedRegistrants = new WakeLockRegistrantList(mNotifyWakeLock);
        mOnRegistrants = new WakeLockRegistrantList(mNotifyWakeLock);
        mAvailRegistrants = new WakeLockRegistrantList(mNotifyWakeLock);
        mOffOrNotAvailRegistrants = new WakeLockRegistrantList(mNotifyWakeLock);
        mNotAvailRegistrants = new WakeLockRegistrantList(mNotifyWakeLock);
        mCallStateRegistrants = new WakeLockRegistrantList(mNotifyWakeLock);
        mImsNetworkStateChangedRegistrants = new WakeLockRegistrantList(mNotifyWakeLock);
        mVoicePrivacyOnRegistrants = new WakeLockRegistrantList(mNotifyWakeLock);
        mVoicePrivacyOffRegistrants = new WakeLockRegistrantList(mNotifyWakeLock);
        mRingbackToneRegistrants = new WakeLockRegistrantList(mNotifyWakeLock);
        mExitEmergencyCallbackModeRegistrants = new WakeLockRegistrantList(mNotifyWakeLock);
        mRefreshViceInfoRegistrants = new WakeLockRegistrantList(mNotifyWakeLock);
    }

    //***** CommandsInterface implementation

    @Override
    public RadioState getRadioState() {
        return mState;
    }

    @Override
    public void registerForRadioStateChanged(Handler h, int what, Object obj) {
        Registrant r = new WakeLockRegistrant (h, what, obj, mNotifyWakeLock);

        synchronized (mStateMonitor) {
            mRadioStateChangedRegistrants.add(r);
            r.notifyRegistrant();
        }
    }

    @Override
    public void unregisterForRadioStateChanged(Handler h) {
        synchronized (mStateMonitor) {
            mRadioStateChangedRegistrants.remove(h);
        }
    }

    public void registerForImsNetworkStateChanged(Handler h, int what, Object obj) {
        Registrant r = new WakeLockRegistrant (h, what, obj, mNotifyWakeLock);
        mImsNetworkStateChangedRegistrants.add(r);
    }

    public void unregisterForImsNetworkStateChanged(Handler h) {
        mImsNetworkStateChangedRegistrants.remove(h);
    }

    @Override
    public void registerForOn(Handler h, int what, Object obj) {
        Registrant r = new WakeLockRegistrant (h, what, obj, mNotifyWakeLock);

        synchronized (mStateMonitor) {
            mOnRegistrants.add(r);

            if (mState.isOn()) {
                r.notifyRegistrant(new AsyncResult(null, null, null));
            }
        }
    }
    @Override
    public void unregisterForOn(Handler h) {
        synchronized (mStateMonitor) {
            mOnRegistrants.remove(h);
        }
    }


    @Override
    public void registerForAvailable(Handler h, int what, Object obj) {
        Registrant r = new WakeLockRegistrant (h, what, obj, mNotifyWakeLock);

        synchronized (mStateMonitor) {
            mAvailRegistrants.add(r);

            if (mState.isAvailable()) {
                r.notifyRegistrant(new AsyncResult(null, null, null));
            }
        }
    }

    @Override
    public void unregisterForAvailable(Handler h) {
        synchronized(mStateMonitor) {
            mAvailRegistrants.remove(h);
        }
    }

    @Override
    public void registerForNotAvailable(Handler h, int what, Object obj) {
        Registrant r = new WakeLockRegistrant (h, what, obj, mNotifyWakeLock);

        synchronized (mStateMonitor) {
            mNotAvailRegistrants.add(r);

            if (!mState.isAvailable()) {
                r.notifyRegistrant(new AsyncResult(null, null, null));
            }
        }
    }

    @Override
    public void unregisterForNotAvailable(Handler h) {
        synchronized (mStateMonitor) {
            mNotAvailRegistrants.remove(h);
        }
    }

    @Override
    public void registerForOffOrNotAvailable(Handler h, int what, Object obj) {
        Registrant r = new WakeLockRegistrant (h, what, obj, mNotifyWakeLock);

        synchronized (mStateMonitor) {
            mOffOrNotAvailRegistrants.add(r);

            if (mState == RadioState.RADIO_OFF || !mState.isAvailable()) {
                r.notifyRegistrant(new AsyncResult(null, null, null));
            }
        }
    }
    @Override
    public void unregisterForOffOrNotAvailable(Handler h) {
        synchronized(mStateMonitor) {
            mOffOrNotAvailRegistrants.remove(h);
        }
    }

    @Override
    public void registerForCallStateChanged(Handler h, int what, Object obj) {
        Registrant r = new WakeLockRegistrant (h, what, obj, mNotifyWakeLock);

        mCallStateRegistrants.add(r);
    }

    @Override
    public void unregisterForCallStateChanged(Handler h) {
        mCallStateRegistrants.remove(h);
    }

    @Override
    public void setOnSuppServiceNotification(Handler h, int what, Object obj) {
        mSsnRegistrant = new WakeLockRegistrant (h, what, obj, mNotifyWakeLock);
    }

    @Override
    public void unSetOnSuppServiceNotification(Handler h) {
        mSsnRegistrant.clear();
    }

    @Override
    public void setEmergencyCallbackMode(Handler h, int what, Object obj) {
        mEmergencyCallbackModeRegistrant = new WakeLockRegistrant (h, what, obj, mNotifyWakeLock);
    }

    @Override
    public void setOnCallRing(Handler h, int what, Object obj) {
        mRingRegistrant = new WakeLockRegistrant (h, what, obj, mNotifyWakeLock);
    }

    @Override
    public void unSetOnCallRing(Handler h) {
        mRingRegistrant.clear();
    }

    @Override
    public void registerForInCallVoicePrivacyOn(Handler h, int what, Object obj) {
        Registrant r = new WakeLockRegistrant (h, what, obj, mNotifyWakeLock);
        mVoicePrivacyOnRegistrants.add(r);
    }

    @Override
    public void unregisterForInCallVoicePrivacyOn(Handler h){
        mVoicePrivacyOnRegistrants.remove(h);
    }

    @Override
    public void registerForInCallVoicePrivacyOff(Handler h, int what, Object obj) {
        Registrant r = new WakeLockRegistrant (h, what, obj, mNotifyWakeLock);
        mVoicePrivacyOffRegistrants.add(r);
    }

    @Override
    public void unregisterForInCallVoicePrivacyOff(Handler h){
        mVoicePrivacyOffRegistrants.remove(h);
    }

    @Override
    public void registerForRingbackTone(Handler h, int what, Object obj) {
        Registrant r = new WakeLockRegistrant (h, what, obj, mNotifyWakeLock);
        mRingbackToneRegistrants.add(r);
    }

    @Override
    public void unregisterForRingbackTone(Handler h) {
        mRingbackToneRegistrants.remove(h);
    }

    @Override
    public void registerForExitEmergencyCallbackMode(Handler h, int what, Object obj) {
        Registrant r = new WakeLockRegistrant (h, what, obj, mNotifyWakeLock);
        mExitEmergencyCallbackModeRegistrants.add(r);
    }

    @Override
    public void unregisterForExitEmergencyCallbackMode(Handler h) {
        mExitEmergencyCallbackModeRegistrants.remove(h);
    }

    //***** Protected Methods
    /**
     * Store new RadioState and send notification based on the changes
     *
     * This function is called only by RIL.java when receiving unsolicited
     * RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED
     *
     * RadioState has 3 values : RADIO_OFF, RADIO_UNAVAILABLE, RADIO_ON.
     *
     * @param newState new RadioState decoded from RIL_UNSOL_RADIO_STATE_CHANGED
     */
    protected void setRadioState(RadioState newState) {
        RadioState oldState;

        synchronized (mStateMonitor) {
            oldState = mState;
            mState = newState;

            if (oldState == mState) {
                // no state transition
                return;
            }

            mRadioStateChangedRegistrants.notifyRegistrants();

            if (mState.isAvailable() && !oldState.isAvailable()) {
                mAvailRegistrants.notifyRegistrants();
                onRadioAvailable();
            }

            if (!mState.isAvailable() && oldState.isAvailable()) {
                mNotAvailRegistrants.notifyRegistrants();
            }

            if (mState.isOn() && !oldState.isOn()) {
                mOnRegistrants.notifyRegistrants();
            }

            if ((!mState.isOn() || !mState.isAvailable())
                && !((!oldState.isOn() || !oldState.isAvailable()))
            ) {
                mOffOrNotAvailRegistrants.notifyRegistrants();
            }
        }
    }

    protected void onRadioAvailable() {
    }

    public void setUiccSubscription(int slotId, int appIndex, int subId, int subStatus,
            Message response) {
    }

    public void registerForViceRefreshInfo(Handler h, int what, Object obj) {
        Registrant r = new WakeLockRegistrant (h, what, obj, mNotifyWakeLock);
        mRefreshViceInfoRegistrants.add(r);
    }
}
