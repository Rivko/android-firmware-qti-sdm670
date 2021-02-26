/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.internal.telephony;

import android.content.Context;
import android.os.AsyncResult;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.os.SystemProperties;
import android.telephony.Rlog;

import android.telephony.PreciseCallState;
import android.telephony.ServiceState;
import com.android.internal.telephony.GsmCdmaCall;
import com.android.internal.telephony.imsphone.ImsPhone;
import com.android.internal.telephony.imsphone.ImsPhoneCall;
import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.PhoneSwitcher;
import com.android.internal.telephony.SubscriptionController;

/*
 * To allow internet data on non dds subscription, during ims call on it,
 * QtiDdsController class will trigger dds switch to non dds subscription.
 * On call end, dds will be reverted to user preferred subscription.
 */
public class QtiDdsSwitchController {
    private final String LOG_TAG = "QtiDdsSwitchController";
    private final SubscriptionController mSubscriptionController;
    private final Context mContext;
    private final int mNumPhones;
    private final Phone[] mPhones;
    private final GsmCdmaCall[] mFgCsCalls;
    private final GsmCdmaCall[] mBgCsCalls;
    private final GsmCdmaCall[] mRiCsCalls;
    private final ImsPhone[] mImsPhones;
    private final ImsPhoneCall[] mFgImsCalls;
    private final ImsPhoneCall[] mBgImsCalls;
    private final ImsPhoneCall[] mRiImsCalls;
    private final Handler mDdsSwitchHandler;

    /* Flag to track active call */
    private boolean mIsCallActive = false;

    /* Flag to indicate if call state change can be processed */
    private boolean mNotifyCallState = false;

    /* Flag to send dds switch reason as temp in oem hook message */
    public static boolean mTempDdsSwitchRequired = false;

    /* Save user preference for dds sub, to revert on call end */
    private int mUserDdsSubId = -1;

    /* Adb property to enable/disable feature */
    private final String PROPERTY_TEMP_DDSSWITCH = "persist.radio.enable_temp_dds";
    private boolean isPropertyEnabled = SystemProperties.getBoolean(PROPERTY_TEMP_DDSSWITCH, false);

    /* Read L+L status */
    private boolean isLplusLSupported = false;

    /* Integer constants for event handling */
    private final int EVENT_PRECISE_CS_CALL_STATE_CHANGED = 101;
    private final int EVENT_PRECISE_IMS_CALL_STATE_CHANGED = 102;

    /*
     * Constructor
     */
    public QtiDdsSwitchController(int numPhones, Context context,
            SubscriptionController subscriptionController, Looper looper,
            Phone[] phones, boolean status) {
        mNumPhones = numPhones;
        mContext = context;
        mSubscriptionController = subscriptionController;
        mDdsSwitchHandler = new DdsSwitchHandler(looper);
        mPhones = phones;
        mImsPhones = new ImsPhone[mNumPhones];
        mFgCsCalls = new GsmCdmaCall[mNumPhones];
        mBgCsCalls = new GsmCdmaCall[mNumPhones];
        mRiCsCalls = new GsmCdmaCall[mNumPhones];
        mFgImsCalls = new ImsPhoneCall[mNumPhones];
        mBgImsCalls = new ImsPhoneCall[mNumPhones];
        mRiImsCalls = new ImsPhoneCall[mNumPhones];
        isLplusLSupported = status;

        // Register for ims call start on powerup
        for (int i=0; i < mNumPhones; i++) {
            mImsPhones[i] = (ImsPhone)mPhones[i].getImsPhone();
            if (mPhones[i] != null && mImsPhones[i] != null) {
                mFgCsCalls[i] = (GsmCdmaCall) mPhones[i].getForegroundCall();
                mBgCsCalls[i] = (GsmCdmaCall) mPhones[i].getBackgroundCall();
                mRiCsCalls[i] = (GsmCdmaCall) mPhones[i].getRingingCall();

                mFgImsCalls[i] = mImsPhones[i].getForegroundCall();
                mBgImsCalls[i] = mImsPhones[i].getBackgroundCall();
                mRiImsCalls[i] = mImsPhones[i].getRingingCall();
            }
        }
    }

    /*
     * Method to check if ims call is on non dds sub and trigger dds switch
     */
    private void onCallStarted(int phoneId) {
        // Update user preference before changing DDS
        mUserDdsSubId = mSubscriptionController.getDefaultDataSubId();
        int callSubId = mSubscriptionController.getSubId(phoneId)[0];
        // Trigger dds switch only if call is on non dds sub
        mTempDdsSwitchRequired = true;
        log("Trigger temporary DDS switch to sub: " + callSubId);
        mSubscriptionController.setDefaultDataSubId(callSubId);
    }

    /*
     * Method to revert dds to user preferred dds sub on ims call end
     */
    private void onCallEnded(int phoneId) {
        mTempDdsSwitchRequired = false;
        // If user preferred dds sub is invalid, skip dds revert
        if (mSubscriptionController.isActiveSubId(mUserDdsSubId)) {
            log("Set DDS to actual sub: " + mUserDdsSubId);
            mSubscriptionController.setDefaultDataSubId(mUserDdsSubId);
        } else {
            log("User dds sub is invalid, skip dds reset");
        }
    }

    /*
     * Method to send dds switch reason as temporary on call start
     */
    public static boolean isTempDdsSwitchRequired() {
        return mTempDdsSwitchRequired;
    }

    /*
     * Method to reset flag once temporary dds switch is completed
     */
    public void resetTempDdsSwitchRequired() {
        // Reset this flag once temp DDS switch is triggered
        // as it will be used to send DDS switch reason
        mTempDdsSwitchRequired = false;
    }

    /*
     * Method to update L+L status
     */
    public void updateLplusLStatus(boolean status) {
        isLplusLSupported = status;
        log("updateLplusLStatus - status: " + status);
        if (isLplusLSupported) {
            for (int i=0; i < mNumPhones; i++) {
                log("Register for call state change on phone: " + i);
                mPhones[i].registerForPreciseCallStateChanged(mDdsSwitchHandler,
                        EVENT_PRECISE_CS_CALL_STATE_CHANGED, i);
                mImsPhones[i].registerForPreciseCallStateChanged(mDdsSwitchHandler,
                        EVENT_PRECISE_IMS_CALL_STATE_CHANGED, i);
            }
        } else {
            for (int i=0; i < mNumPhones; i++) {
                log("unregister for call state change on phone: " + i);
                mPhones[i].unregisterForPreciseCallStateChanged(mDdsSwitchHandler);
                mImsPhones[i].unregisterForPreciseCallStateChanged(mDdsSwitchHandler);
            }
        }
    }

    /*
     * Method to update adb property value
     */
    private boolean isFeatureEnabled() {
        isPropertyEnabled = SystemProperties.getBoolean(PROPERTY_TEMP_DDSSWITCH, false);
        log("isPropertyEnabled: " + isPropertyEnabled);
        return isPropertyEnabled;
    }

    /*
     * Method to check if any of the calls are active
     */
    private boolean isCallActive(int phoneId) {
        return (mFgCsCalls[phoneId].getState() == GsmCdmaCall.State.ACTIVE ||
                mBgCsCalls[phoneId].getState() == GsmCdmaCall.State.ACTIVE ||
                mRiCsCalls[phoneId].getState() == GsmCdmaCall.State.ACTIVE ||
                mFgImsCalls[phoneId].getState() == ImsPhoneCall.State.ACTIVE ||
                mBgImsCalls[phoneId].getState() == ImsPhoneCall.State.ACTIVE ||
                mRiImsCalls[phoneId].getState() == ImsPhoneCall.State.ACTIVE);
    }

    /*
     * Method to check if all the calls are ended
     */
    private boolean isCallIdle(int phoneId) {
        return (mFgCsCalls[phoneId].isIdle() &&
                mBgCsCalls[phoneId].isIdle() &&
                mRiCsCalls[phoneId].isIdle() &&
                mFgImsCalls[phoneId].isIdle() &&
                mBgImsCalls[phoneId].isIdle() &&
                mRiImsCalls[phoneId].isIdle());
    }

    /*
     * Method to validate if call is on non dds sub
     */
    private boolean isCallOnNonDds(int phoneId) {
        int ddsPhoneId = mSubscriptionController.getPhoneId(mUserDdsSubId);
        return (phoneId != ddsPhoneId);
    }

    /*
     * Method to check for active and idle calls
     */
    private void processCallStateChanged(int phoneId) {
        if (!mIsCallActive && isCallActive(phoneId)) {
            log("notifyCallStateChanged: call active on non dds");
            mIsCallActive = true;
            onCallStarted(phoneId);
        } else if (isCallIdle(phoneId)) {
            mNotifyCallState = false;
            //If there is no active call, dont trigger dds reset
            if (mIsCallActive) {
                log("notifyCallStateChanged: call disconnected on non dds");
                mIsCallActive = false;
                onCallEnded(phoneId);
            }
        } else {
            log("ignore call state change");
        }
    }

    /*
     * Handler Class
     */
    private class DdsSwitchHandler extends Handler {
        public DdsSwitchHandler(Looper looper) {
            super(looper);
        }

        @Override
        public void handleMessage(Message msg) {
            AsyncResult ar = (AsyncResult)msg.obj;
            int phoneId = (int)ar.userObj;
            switch (msg.what) {
                case EVENT_PRECISE_CS_CALL_STATE_CHANGED:
                case EVENT_PRECISE_IMS_CALL_STATE_CHANGED: {
                    // Read feature status and validate calling sub during call start
                    if (!mNotifyCallState) {
                        mUserDdsSubId = mSubscriptionController.getDefaultDataSubId();

                        // Block dds switch for 1x, as concurrent voice and data
                        // will not be supported
                        int voiceRat = mPhones[phoneId].getServiceState().
                                getRilVoiceRadioTechnology();
                        mNotifyCallState = isFeatureEnabled() &&
                                isCallOnNonDds(phoneId) &&
                                (voiceRat != ServiceState.RIL_RADIO_TECHNOLOGY_UNKNOWN &&
                                (voiceRat != ServiceState.RIL_RADIO_TECHNOLOGY_1xRTT &&
                                voiceRat != ServiceState.RIL_RADIO_TECHNOLOGY_IS95A &&
                                voiceRat != ServiceState.RIL_RADIO_TECHNOLOGY_IS95B));
                        log("mUserDdsSubId - " + mUserDdsSubId + ", voiceRat - " + voiceRat);
                    }

                    if (mNotifyCallState) {
                        log("EVENT_PRECISE_CALL_STATE_CHANGED");
                        processCallStateChanged(phoneId);
                    }
                    break;
                }
            }
        }
    }

    protected void log(String l) {
        Rlog.d(LOG_TAG, l);
    }

}
