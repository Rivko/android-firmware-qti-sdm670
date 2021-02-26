/*
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 *
 * Copyright (c) 2013 The Android Open Source Project
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
 */

package org.codeaurora.ims;

import org.codeaurora.ims.parser.ConfInfo;


import com.android.ims.ImsCallProfile;
import com.android.ims.ImsConferenceState;
import com.android.ims.ImsException;
import com.android.ims.ImsReasonInfo;
import com.android.ims.ImsServiceClass;
import com.android.ims.ImsStreamMediaProfile;
import com.android.ims.ImsSuppServiceNotification;
import com.android.ims.internal.ImsCallSession;
import com.android.ims.internal.IImsCallSession;
import com.android.ims.internal.IImsCallSessionListener;
import com.android.ims.internal.IImsVideoCallProvider;
import com.android.internal.telephony.CommandsInterface;
import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.gsm.SuppServiceNotification;

import android.app.PendingIntent;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.content.res.Resources;
import android.content.ServiceConnection;
import android.os.AsyncResult;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.SystemProperties;
import android.telecom.VideoProfile;
import android.telephony.ServiceState;
import android.widget.Toast;
import android.os.SystemProperties;
import android.widget.Toast;
import android.media.ToneGenerator;
import android.media.AudioManager;
import android.telecom.Connection.VideoProvider;
import android.telephony.ims.stub.ImsCallSessionImplBase;

import java.util.concurrent.CopyOnWriteArrayList;
import java.util.ArrayList;
import java.util.List;

import com.android.internal.util.Preconditions;
import com.qualcomm.ims.utils.Config;
import com.qualcomm.ims.utils.Log;
import com.qualcomm.ims.vt.ImsVideoCallProviderImpl;
import com.qualcomm.ims.vt.LowBatteryHandler;
import com.qualcomm.ims.utils.Log;
import vendor.qti.hardware.radio.ims.V1_0.CallFailCause;
import vendor.qti.hardware.radio.ims.V1_0.RttMode;

import org.codeaurora.ims.utils.QtiImsExtUtils;
import org.codeaurora.ims.QtiCallConstants;
import org.codeaurora.ims.QtiVideoCallDataUsage;

public class ImsCallSessionImpl extends ImsCallSessionImplBase {
    private static final int EVENT_DIAL = 1;
    private static final int EVENT_ACCEPT = 2;
    private static final int EVENT_HANGUP = 3;
    private static final int EVENT_HOLD = 4;
    private static final int EVENT_RESUME = 5;
    private static final int EVENT_CONFERENCE = 6;
    private static final int EVENT_REJECT = 7;
    private static final int EVENT_DEFLECT = 8;
    private static final int EVENT_ADD_PARTICIPANT = 9;
    private static final int EVENT_RINGBACK_TONE = 10;
    private static final int EVENT_REMOVE_PARTICIPANT = 11;
    private static final int EVENT_CLOSE_SESSION = 12;
    private static final int EVENT_RTT_MESSAGE_RECEIVED = 13;
    private static final int EVENT_SEND_RTT_MESSAGE = 14;
    private static final int EVENT_SEND_RTT_MODIFY_REQUEST = 15;
    private static final int EVENT_RTT_UPGRADE_CONFIRM_DONE = 16;

    private static final int DEFAULT_CALL_INDEX = -1;

    public static final int SUPP_NOTIFICATION_TYPE_MO = 0;
    public static final int SUPP_NOTIFICATION_TYPE_MT = 1;

    public static final int SUPP_SVC_CODE_INVALID   = -1;
    public static final int SUPP_SVC_CODE_MT_HOLD   = 2;
    public static final int SUPP_SVC_CODE_MT_RESUME = 3;

    public static final String PROPERTY_DBG_ENCRYPTION_OVERRIDE
            = "persist.dbg.call_encrypt_ovr";
    public static final int PROPERTY_DBG_ENCRYPTION_OVERRIDE_DEFAULT = 0;

    //Constants for sip error codes.
    public static final int SIP_FORBIDDEN = 403;
    public static final int SIP_REQUEST_TIMEOUT = 408;
    public static final int SIP_TEMPORARILY_UNAVAILABLE = 480;
    public static final int SIP_SERVER_INTERNAL_ERROR = 500;
    public static final int SIP_SERVER_NOT_IMPLEMENTED = 501;
    public static final int SIP_SERVER_BAD_GATEWAY = 502;
    public static final int SIP_SERVICE_UNAVAILABLE = 503;
    public static final int SIP_SERVER_VERSION_UNSUPPORTED = 505;
    public static final int SIP_SERVER_MESSAGE_TOOLARGE = 513;
    public static final int SIP_SERVER_PRECONDITION_FAILURE = 580;

    private ImsSenderRxr mCi;
    private DriverCallIms mDc = null;
    //Initialize mCallId as DEFAULT_CALL_INDEX. It will use to hangup DIALed call
    //before call indication reached.
    private int mCallId = DEFAULT_CALL_INDEX;
    private Context mContext;
    private ImsCallProfile mLocalCallProfile = new ImsCallProfile(
                ImsCallProfile.SERVICE_TYPE_NORMAL, ImsCallProfile.CALL_TYPE_VT_NODIR);
    private ImsCallProfile mRemoteCallProfile = new ImsCallProfile(
                ImsCallProfile.SERVICE_TYPE_NORMAL, ImsCallProfile.CALL_TYPE_VT_NODIR);
    private ImsCallProfile mCallProfile = new ImsCallProfile();
    public ImsCallSessionListenerProxy mListenerProxy = new ImsCallSessionListenerProxy();
    private int mState = ImsCallSession.State.IDLE;
    private boolean mInCall;
    private Handler mHandler = new ImsCallSessionImplHandler();
    private String mCallee = null; //Remote party's number
    private boolean mIsConfInProgress = false;
    private ImsCallSessionImpl newSession = null; // new session for conference calls
    private int mDisconnCause = ImsReasonInfo.CODE_UNSPECIFIED;
    private int mMtSuppSvcCode = SUPP_SVC_CODE_INVALID;
    private int mPhoneId = QtiCallConstants.INVALID_PHONE_ID;
    private ConfInfo mConfInfo = null;
    private ImsConferenceState mImsConferenceState = null;
    private boolean mRingbackToneRequest = false;

    private ImsCallModification mImsCallModification;
    ImsVideoCallProviderImpl mImsVideoCallProviderImpl;

    private List<Listener> mListeners = new CopyOnWriteArrayList<>();
    private boolean mIsVideoAllowed = false;
    private boolean mIsVoiceAllowed = false;

    private ImsServiceClassTracker mTracker = null;
    private boolean mStateChangeReportingAllowed = true;
    public boolean mIsConferenceHostSession = false;
    private boolean mIsCallTerminatedDueToLowBattery = false;
    private boolean mIsLowBattery = false;
    private int mVoWifiQuality = QtiCallConstants.VOWIFI_QUALITY_EXCELLENT;

    public interface Listener {
        void onDisconnected(ImsCallSessionImpl session);
        void onClosed(ImsCallSessionImpl session);
        void onUnsolCallModify(ImsCallSessionImpl session, CallModify callModify);
    }

    private ImsCallSessionImpl(Context context, ImsSenderRxr senderRxr, int phoneId,
                               ImsServiceClassTracker tracker, boolean isVideoCapable) {
        mCi = senderRxr;
        mContext = context;
        mTracker = tracker;
        mPhoneId = phoneId;

        final boolean shallCreateVideoProvider = isVideoCapable ||
                isConfigEnabled(R.bool.config_ovr_create_video_call_provider);
        maybeCreateVideoProvider(shallCreateVideoProvider);
        mIsVideoAllowed = isVideoCapable;
        mCi.registerForRttMessage(mHandler, EVENT_RTT_MESSAGE_RECEIVED, null);
    }

    /**
     * The function initializes VideoCallProvider if it's not already initialized
     * and if video calling is supported / enabled. VideoCallProvider won't be de-initialized if
     * it is initialized and video calling is disabled.
     * @param isVideoCallingEnabled True if video calling is enabled, false otherwise.
     */
    private void maybeCreateVideoProvider(boolean isVideoCallingEnabled) {
        if ((QtiImsExtUtils.isRttSupported(mPhoneId, mContext) || isVideoCallingEnabled) &&
                (mImsCallModification == null)) {
            mImsCallModification = new ImsCallModification(this, mContext, mCi);
        }

        if (isVideoCallingEnabled && mImsVideoCallProviderImpl==null) {
            Log.i(this, "maybeCreateVideoProvider: Creating VideoCallProvider");
            mImsVideoCallProviderImpl = new ImsVideoCallProviderImpl(this, mImsCallModification);
            addListener(mImsVideoCallProviderImpl);
        }
    }

    // Contructor for MO Call
    public ImsCallSessionImpl(ImsCallProfile profile, IImsCallSessionListener listener,
            ImsSenderRxr senderRxr, Context context, ImsServiceClassTracker tracker,
                              boolean isVideoCapable, int phoneId) {
        this(context, senderRxr, phoneId, tracker, isVideoCapable);

        setListener(listener);
        mListenerProxy.mListener = listener;
        mCallProfile = profile;
        mTracker = tracker;
        mConfInfo = new ConfInfo();
        mCi.registerForRingbackTone(mHandler, EVENT_RINGBACK_TONE, null);
    }

    // Constructor for MT call and Conference Call
    public ImsCallSessionImpl(DriverCallIms call, ImsSenderRxr senderRxr, Context context,
            ImsServiceClassTracker tracker, boolean isVideoCapable, int phoneId) {
        this(context, senderRxr, phoneId, tracker, isVideoCapable);

        //TODO update member variables in this class based on dc
        mDc = new DriverCallIms(call);
        mCallId = mDc.index;
        mCallee = call.number;

        updateImsCallProfile(mDc);
        setCapabilitiesInProfiles(mDc);

        mConfInfo = new ConfInfo();
    }

    /**
     * Registers call listener.
     * @param listener Listener to registered
     * @see ImsCallSessionImpl#Listener
     */
    public void addListener(Listener listener) {
        if (!isSessionValid()) return;

        if (listener == null) {
            throw new IllegalArgumentException("listener is null.");
        }

        // Note: This will do linear search, O(N).
        // This is acceptable since arrays size is small.
        synchronized (mListeners) {
            if (!mListeners.contains(listener)) {
                mListeners.add(listener);
            } else {
                Log.e(this, "Duplicate listener, " + listener);
            }
        }
    }

    /**
     * Unregisters call listener.
     * @param listener Listener to unregistered
     * @see ImsCallSessionImpl#Listener
     */
    public void removeListener(Listener listener) {
        if (!isSessionValid()) return;

        if (listener == null) {
            throw new IllegalArgumentException("listener is null.");
        }

        // Note: This will do linear search, O(N).
        // This is acceptable since arrays size is small.
        synchronized (mListeners) {
            if (mListeners.contains(listener)) {
                mListeners.remove(listener);
            } else {
                Log.e(this, "Listener not found, " + listener);
            }
        }
    }

    public ImsVideoCallProviderImpl getImsVideoCallProviderImpl() {
        if (!isSessionValid()) return null;
        return mImsVideoCallProviderImpl;
    }

    private void notifySessionDisconnected() {
        synchronized (mListeners) {
            for( Listener l : mListeners) {
                l.onDisconnected(this);
            }
        }
    }

    private void notifySessionClosed() {
        synchronized (mListeners) {
            for( Listener l : mListeners) {
                l.onClosed(this);
            }
        }
    }

    public void notifyUnsolCallModify(CallModify callModify) {
        if (!isSessionValid()) return;

        synchronized (mListeners) {
            for (Listener l : mListeners) {
                l.onUnsolCallModify(this, callModify);
            }
        }
    }

    /* package-private */
    void onReceivedModifyCall(CallModify callModify) {
        if (mImsCallModification == null) {
            Log.e(this, "onReceivedModifyCall: Ignoring session modification request.");
            return;
        }
        mImsCallModification.onReceivedModifyCall(callModify);
    }

    /* package-private */
    ImsCallModification getImsCallModification() {
        if (!isSessionValid()) return null;
        return mImsCallModification;
    }

    static boolean isServiceAllowed(int srvType, ServiceStatus[] ability) {
        boolean allowed = false;
        if (ability != null) {
            for (ServiceStatus srv : ability) {
                if (srv != null && srv.type == srvType) {
                    if (srv.status == ImsQmiIF.STATUS_PARTIALLY_ENABLED ||
                            srv.status == ImsQmiIF.STATUS_ENABLED) {
                        allowed = true;
                    }
                    break;
                }
            }
        }
        return allowed;
    }

    /**
     * Utility to get restrict cause from Service Status Update
     * @param srvType - VoLTE, VT
     * @param ability - Consolidated Service Status Ability
     * @return cause - restrict cause for the service type
     */
    private int getRestrictCause(int srvType, ServiceStatus[] ability) {
        int cause = CallDetails.CALL_RESTRICT_CAUSE_NONE;
        if (ability != null) {
            for (ServiceStatus srv : ability) {
                if (srv != null && srv.type == srvType && srv.accessTechStatus != null &&
                        srv.accessTechStatus.length > 0) {
                    cause = srv.accessTechStatus[0].restrictCause;
                    break;
                }
            }
        }
        return cause;
    }

    /**
     * Update local driver call copy with the new update
     * @param dcUpdate - the new Driver Call Update
     * @return true if the update is different from previous update
     */
    private boolean updateLocalDc(DriverCallIms dcUpdate) {
        boolean hasChanged = false;
        if (mDc == null) {
            mDc = new DriverCallIms(dcUpdate);
            hasChanged = true;
        } else {
            int changedProperties = mDc.update(dcUpdate);

            // Notify listeners if the call is changed to multiparty call
            if (mStateChangeReportingAllowed) {
                if ((changedProperties & DriverCallIms.UPDATE_IS_MPTY) != 0) {
                    if (mListenerProxy != null) {
                        mListenerProxy.callSessionMultipartyStateChanged((IImsCallSession) this,
                                mDc.isMpty);
                    }
                    if (mDc.isMpty) {
                        // This code gets executed for conference participant side where
                        // call type changes from normal to mpty.
                        updateMergeStatus(ImsCallUtils.SESSION_MERGE_COMPLETED);
                    }
                }
            }

            hasChanged = changedProperties != DriverCallIms.UPDATE_NONE;
        }
        Log.i(this, "updateLocalDc is " + hasChanged);
        return hasChanged;
    }

    public void muteStateReporting() {
        Log.i(this, "Call session state reporting muted! session=" + this);
        mStateChangeReportingAllowed = false;
    }

    public void unMuteStateReporting() {
        Log.i(this, "Call session state reporting unmuted. session=" + this);
        mStateChangeReportingAllowed = true;
    }

    public void updateMergeStatus(int status) {
        Log.i(this, "updateMergeStatus status: " + status);
        if (mImsVideoCallProviderImpl != null) {
            mImsVideoCallProviderImpl.updateMergeStatus(status);
        }
    }

    /**
     * Update the current ImsCallSession object based on Driver Call
     * @param dcUpdate
     */
    public void updateCall(DriverCallIms dcUpdate) {
        //TODO - update member variables before calling notification
        Log.i(this, "updateCall for " + dcUpdate);

        if (!isSessionValid()) return;

        updateImsCallProfile(dcUpdate);
        setCapabilitiesInProfiles(dcUpdate);

        if (mImsCallModification != null) {
            mImsCallModification.update(dcUpdate);
        }

        if (dcUpdate.isMpty) {
            mCallProfile.setCallExtraBoolean(
                    QtiImsExtUtils.QTI_IMS_INCOMING_CONF_EXTRA_KEY, false);
        }

        switch (dcUpdate.state) {
            case ACTIVE:
                if (mDc == null) {
                    // TODO:: PHANTOM CALL!!
                    Log.e(this, "Phantom call!");
                    mState = ImsCallSession.State.ESTABLISHED;
                    mCallId = dcUpdate.index;
                    if (mStateChangeReportingAllowed) {
                        mListenerProxy.callSessionStarted((IImsCallSession) this, mCallProfile);
                    }
                } else if (mDc.state == DriverCallIms.State.DIALING ||
                        mDc.state == DriverCallIms.State.ALERTING ||
                        mDc.state == DriverCallIms.State.INCOMING ||
                        mDc.state == DriverCallIms.State.WAITING) {
                    mState = ImsCallSession.State.ESTABLISHED;
                    mDc.state = DriverCallIms.State.ACTIVE;
                    // Extract Call Details into ImsCallProfile.
                    extractCallDetailsIntoCallProfile(dcUpdate);
                    if (mStateChangeReportingAllowed) {
                        mListenerProxy.callSessionStarted((IImsCallSession) this, mCallProfile);
                    }
                }
                // Check if the call is being resumed from a HOLDING state.
                else if (mDc.state == DriverCallIms.State.HOLDING) {
                    Log.i(this, "Call being resumed.");
                    mIsConfInProgress = false;

                    if (mStateChangeReportingAllowed) {
                        mListenerProxy.callSessionResumed((IImsCallSession)this, mCallProfile);
                    }
                } else {
                    Log.i(this, "Call resumed skipped, conf status = " + mIsConfInProgress);
                }
                break;
            case HOLDING:
                if (mDc.state != DriverCallIms.State.HOLDING) {
                    Log.i(this, "Call being held.");
                    if (mStateChangeReportingAllowed) {
                        mListenerProxy.callSessionHeld((IImsCallSession)this, mCallProfile);
                    }
                }
                break;
            case DIALING:
                if (mDc == null) { // No DC yet for pending MO
                    Log.i(this, "MO Dialing call!");
                    mCallId = dcUpdate.index;
                    if (mStateChangeReportingAllowed) {
                        mListenerProxy.callSessionProgressing((IImsCallSession) this,
                                new ImsStreamMediaProfile());
                    }
                }
                handleRetryErrorNotify(dcUpdate);
                break;
            case ALERTING:
                //TODO: Stream media profile with more details.
                mState = ImsCallSession.State.NEGOTIATING;
                if (mDc == null) {
                    Log.i(this, "MO Alerting call!");
                    mCallId = dcUpdate.index;
                }
                if (mDc.state != DriverCallIms.State.ALERTING) {
                    // Extract Call Details into ImsCallProfile.
                    extractCallDetailsIntoCallProfile(dcUpdate);
                    ImsStreamMediaProfile mediaProfile = new ImsStreamMediaProfile();
                    if (mRingbackToneRequest == true) {
                            mediaProfile.mAudioDirection = ImsStreamMediaProfile.DIRECTION_INACTIVE;
                    }
                    mediaProfile.mRttMode = dcUpdate.callDetails.rttMode;
                    if (mStateChangeReportingAllowed) {
                        mListenerProxy.callSessionProgressing((IImsCallSession)this, mediaProfile);
                    }
                }
                handleRetryErrorNotify(dcUpdate);
                break;
            case INCOMING:
            case WAITING:
                // Extract Call Details into ImsCallProfile.
                extractCallDetailsIntoCallProfile(dcUpdate);
                //Send the incoming conference call information through extras
                if (dcUpdate.isMpty) {
                    mCallProfile.setCallExtraBoolean(
                            QtiImsExtUtils.QTI_IMS_INCOMING_CONF_EXTRA_KEY, dcUpdate.isMpty);
                    Log.i(this, "Setting the multi party flag to false");
                    dcUpdate.isMpty = false;
                }
                //TODO: Send Incoming call intent
                break;
            case END:
                mState = ImsCallSession.State.TERMINATED;
                //Propagate error code through extras to UI
                final int errorCode = (mIsCallTerminatedDueToLowBattery &&
                        !QtiImsExtUtils.allowVideoCallsInLowBattery(mPhoneId, mContext)) ?
                        QtiCallConstants.CALL_FAIL_EXTRA_CODE_LOCAL_LOW_BATTERY :
                        dcUpdate.callFailCause.mCode;
                mCallProfile.setCallExtraInt(QtiCallConstants.EXTRAS_KEY_CALL_FAIL_EXTRA_CODE,
                        errorCode);
                if (errorCode == QtiCallConstants.CALL_FAIL_EXTRA_CODE_LTE_3G_HA_FAILED) {
                    Log.i(this, "Call was ended as LTE to 3G/2G handover was not feasible.");
                }
                Log.i(this, "Sip callFailCause:" + dcUpdate.callFailCause);
                if (mDisconnCause == ImsReasonInfo.CODE_UNSPECIFIED) {
                    if (dcUpdate != null && mStateChangeReportingAllowed) {
                        // Pass the fail cause as is to frameworks in case of
                        // MultiEndpoint scenarios
                        if (mDc.isMT && (mDc.state == DriverCallIms.State.INCOMING ||
                                    mDc.state == DriverCallIms.State.WAITING) &&
                                    !isMultiEndpointFailCause(dcUpdate.callFailCause.mCode)) {
                            mListenerProxy.callSessionTerminated((IImsCallSession) this,
                                    new ImsReasonInfo(ImsReasonInfo.CODE_USER_TERMINATED_BY_REMOTE,
                                    dcUpdate.callFailCause.mExtraCode,
                                    dcUpdate.callFailCause.mExtraMessage));
                        } else if (dcUpdate.callFailCause.mCode ==
                                ImsReasonInfo.CODE_USER_TERMINATED) {
                            // This case corresponds to a terminate by remote.
                            mListenerProxy.callSessionTerminated((IImsCallSession) this,
                                    new ImsReasonInfo(ImsReasonInfo.CODE_USER_TERMINATED_BY_REMOTE,
                                    dcUpdate.callFailCause.mExtraCode,
                                    dcUpdate.callFailCause.mExtraMessage));
                        } else {
                            mListenerProxy.callSessionTerminated((IImsCallSession) this,
                                    dcUpdate.callFailCause);
                        }
                    }
                } else {
                    if (mStateChangeReportingAllowed) {
                        mListenerProxy.callSessionTerminated((IImsCallSession) this,
                                new ImsReasonInfo(mDisconnCause, dcUpdate.callFailCause.mExtraCode,
                                dcUpdate.callFailCause.mExtraMessage));
                    }
                }
                notifySessionDisconnected();
                break;
        }
        // Notify listeners of call updated when anything changes in the call.
        final boolean areStatesSame = mDc != null && dcUpdate != null
                && mDc.state == dcUpdate.state;
        if (updateLocalDc(dcUpdate)) {
            maybeTriggerCallSessionUpdate(dcUpdate, areStatesSame);
        }
    }

    private boolean isMultiEndpointFailCause(int code) {
        return (code == ImsReasonInfo.CODE_ANSWERED_ELSEWHERE ||
                code == ImsReasonInfo.CODE_CALL_PULL_OUT_OF_SYNC ||
                code == ImsReasonInfo.CODE_CALL_END_CAUSE_CALL_PULL);
    }

    private boolean maybeTriggerCallSessionUpdate(DriverCallIms dcUpdate, boolean areStatesSame) {
        final boolean isCallNotEnded = dcUpdate.state!= DriverCallIms.State.END;

        if (areStatesSame && isCallNotEnded) {
            Log.d(this, "Call details updated. currentCallDetails= "
                    + mDc.callDetails + " to newCallDetails= " + dcUpdate.callDetails);
            mListenerProxy.callSessionUpdated(this, mCallProfile);
        }
        return areStatesSame && isCallNotEnded;
    }

    private boolean maybeTriggerCallSessionUpdate(DriverCallIms dcUpdate) {
        final boolean areStatesSame = mDc != null && dcUpdate != null
                && mDc.state == dcUpdate.state;
        return maybeTriggerCallSessionUpdate(dcUpdate, areStatesSame);
    }

    public void updateOrientationMode(int mode) {
        Log.v(this, "updateOrientationMode: orientation mode - " + mode);
        mCallProfile.setCallExtraInt(QtiCallConstants.ORIENTATION_MODE_EXTRA_KEY, mode);
        final boolean isCallSessionUpdated = maybeTriggerCallSessionUpdate(mDc);
        Log.v(this, "updateOrientationMode: isCallSessionUpdated - " + isCallSessionUpdated);
    }

    public void updateVideoCallDataUsageInfo(QtiVideoCallDataUsage dataUsage) {
        if (!(Config.isConfigEnabled(mContext, R.bool.config_video_call_datausage_enable))) {
            return;
        }
        mCallProfile.mCallExtras.putParcelable(QtiCallConstants.VIDEO_CALL_DATA_USAGE_KEY,
                dataUsage);
        final boolean isCallSessionUpdated = maybeTriggerCallSessionUpdate(mDc);
        Log.v(this, "updateVideoCallDataUsageInfo: isCallSessionUpdated - " +
                isCallSessionUpdated);
    }

    private boolean maybeUpdateLowBatteryStatus() {
        if (!mStateChangeReportingAllowed) {
            Log.w(this, "merge is in progress so ignore low battery update");
            return false;
        }

        final boolean isLowBattery = LowBatteryHandler.getInstance().isLowBattery();
        final boolean hasChanged = isLowBattery != mIsLowBattery;
        Log.v(this, "maybeUpdateLowBatteryStatus isLowBattery: " + isLowBattery +
                " mIsLowBattery: " + mIsLowBattery);

        if (hasChanged) {
            mIsLowBattery = isLowBattery;
            mCallProfile.setCallExtraBoolean(
                    QtiCallConstants.LOW_BATTERY_EXTRA_KEY, isLowBattery);
        }
        return hasChanged;
    }

    public void updateLowBatteryStatus() {
        Log.v(this, "updateLowBatteryStatus");

        if (maybeUpdateLowBatteryStatus()) {
            maybeTriggerCallSessionUpdate(mDc);
        }
    }

    private void setCapabilitiesInProfiles(DriverCallIms dcUpdate) {
        if (mLocalCallProfile != null) {
            mLocalCallProfile.mMediaProfile = new ImsStreamMediaProfile();
            mLocalCallProfile.mMediaProfile.mAudioQuality = mapAudioCodecFromExtras(
                    dcUpdate.callDetails.getValueForKeyFromExtras(dcUpdate.callDetails.extras,
                            CallDetails.EXTRAS_CODEC));
            setLocalProfileCallType(dcUpdate);
            mLocalCallProfile.mMediaProfile.setRttMode(getRttMode(
                    dcUpdate.callDetails.call_type, dcUpdate.callDetails.localAbility));
        }
        if (mRemoteCallProfile != null) {
            setRemoteProfileCallType(dcUpdate);
            if (dcUpdate.callDetails.peerAbility != null) {
                mRemoteCallProfile.mMediaProfile = new ImsStreamMediaProfile();
                mRemoteCallProfile.mRestrictCause = getRestrictCause (
                        mCallProfile.mCallType == ImsCallProfile.CALL_TYPE_VT ?
                                CallDetails.CALL_TYPE_VT : CallDetails.CALL_TYPE_VOICE,
                                dcUpdate.callDetails.peerAbility);
                mRemoteCallProfile.mMediaProfile.setRttMode(getRttMode(
                    dcUpdate.callDetails.call_type, dcUpdate.callDetails.peerAbility));
            }
        }
    }

    private void setLocalProfileCallType(DriverCallIms dcUpdate) {
        boolean isLocalVideoServiceAllowed = isServiceAllowed(CallDetails.CALL_TYPE_VT,
                dcUpdate.callDetails.localAbility);
        boolean isLocalVoiceServiceAllowed = isServiceAllowed(CallDetails.CALL_TYPE_VOICE,
                dcUpdate.callDetails.localAbility);

        if (QtiImsExtUtils.shallRemoveModifyCallCapability(mPhoneId, mContext) &&
                (dcUpdate.state == DriverCallIms.State.HOLDING)) {
            /*
             * Remove local Voice/VT capabilities to ensure that "modify call"
             * option is not shown on local UE for Voice/VT calls when the call
             * is locally put on HOLD.
             */
            isLocalVoiceServiceAllowed = isLocalVideoServiceAllowed = false;
        }

        if (isLocalVideoServiceAllowed && isLocalVoiceServiceAllowed
                &&  mIsVideoAllowed && mIsVoiceAllowed) {
            mLocalCallProfile.mCallType = ImsCallProfile.CALL_TYPE_VIDEO_N_VOICE;
        } else if (isLocalVideoServiceAllowed
                && mIsVideoAllowed) {
            mLocalCallProfile.mCallType = ImsCallProfile.CALL_TYPE_VT;
        } else if (isLocalVoiceServiceAllowed
                && mIsVoiceAllowed) {
            mLocalCallProfile.mCallType = ImsCallProfile.CALL_TYPE_VOICE;
        } else {
            mLocalCallProfile.mCallType = ImsCallProfile.CALL_TYPE_VT_NODIR;
        }
    }

    private void setRemoteProfileCallType(DriverCallIms dcUpdate) {
        boolean isRemoteVideoServiceAllowed = isServiceAllowed(CallDetails.CALL_TYPE_VT,
                    dcUpdate.callDetails.peerAbility);
        boolean isRemoteVoiceServiceAllowed = isServiceAllowed(CallDetails.CALL_TYPE_VOICE,
                dcUpdate.callDetails.peerAbility);

        if (QtiImsExtUtils.shallRemoveModifyCallCapability(mPhoneId, mContext) &&
                (getMtSuppSvcCode() == SUPP_SVC_CODE_MT_HOLD)) {
            isRemoteVoiceServiceAllowed = isRemoteVideoServiceAllowed = false;
        }

        if (isRemoteVideoServiceAllowed && isRemoteVoiceServiceAllowed) {
            mRemoteCallProfile.mCallType = ImsCallProfile.CALL_TYPE_VIDEO_N_VOICE;
        } else if (isRemoteVideoServiceAllowed) {
            mRemoteCallProfile.mCallType = ImsCallProfile.CALL_TYPE_VT;
        } else if (isRemoteVoiceServiceAllowed) {
            mRemoteCallProfile.mCallType = ImsCallProfile.CALL_TYPE_VOICE;
        } else {
            mRemoteCallProfile.mCallType = ImsCallProfile.CALL_TYPE_VT_NODIR;
        }
    }

    public void updateConfSession(DriverCallIms dc) {
        Log.i(this, "updateConfSession for " + dc);

        if (!isSessionValid()) return;

        if (dc.state == DriverCallIms.State.ACTIVE && dc.isMpty) {
            mState = ImsCallSession.State.ESTABLISHED;
            mCallId = mDc.index;
        }
    }

    private void setMtSuppSvcCode(int code) {
        mMtSuppSvcCode = code;
    }

    private int getMtSuppSvcCode() {
        return mMtSuppSvcCode;
    }

    /**
     * Call appropriate callbacks for updating call info based on
     * UNSOL_SUPP_SVC_NOTIFICATION for the call.
     *
     * @param code
     */
    /** package-private */
    void updateSuppServiceInfo(ImsSuppServiceNotification suppSvcNotification,
            boolean startOnHoldLocalTone) {
        Log.i(this, "updateSuppSvcInfo: suppSvcNotification= " + suppSvcNotification +
                " startOnHoldLocalTone = " + startOnHoldLocalTone);

        if (!isSessionValid()) return;

        //Handle MO and MT notification types separately as same values have different
        //codes based on notification type
        if (suppSvcNotification.notificationType == SUPP_NOTIFICATION_TYPE_MO) {
            mListenerProxy.callSessionSuppServiceReceived(this, suppSvcNotification);
        } else if (suppSvcNotification.notificationType == SUPP_NOTIFICATION_TYPE_MT) {
            boolean isChanged = false;
            setMtSuppSvcCode(suppSvcNotification.code);

            switch (suppSvcNotification.code) {
                case SUPP_SVC_CODE_MT_HOLD:
                    // Call put on hold by remote party.
                    if (startOnHoldLocalTone) {
                        mCallProfile.mMediaProfile.mAudioDirection =
                                ImsStreamMediaProfile.DIRECTION_INACTIVE;
                    }
                    mListenerProxy.callSessionHoldReceived((IImsCallSession) this, mCallProfile);

                    if (QtiImsExtUtils.shallRemoveModifyCallCapability(mPhoneId, mContext) &&
                            mRemoteCallProfile.mCallType != ImsCallProfile.CALL_TYPE_VT_NODIR) {
                       /*
                        * Remove peer Voice/VT capabilities to ensure that "modify call"
                        * option is not shown on remote UE for Voice/VT calls when the call
                        * is put on HOLD by remote party
                        */
                        mRemoteCallProfile.mCallType = ImsCallProfile.CALL_TYPE_VT_NODIR;
                        isChanged = true;
                    }
                    break;
                case SUPP_SVC_CODE_MT_RESUME:
                    // Held call retrieved by remote party.
                    if (mCallProfile.mMediaProfile.mAudioDirection ==
                            ImsStreamMediaProfile.DIRECTION_INACTIVE) {
                        mCallProfile.mMediaProfile.mAudioDirection =
                                ImsStreamMediaProfile.DIRECTION_SEND_RECEIVE;
                    }
                    mListenerProxy.callSessionResumeReceived((IImsCallSession) this, mCallProfile);

                    if (QtiImsExtUtils.shallRemoveModifyCallCapability(mPhoneId, mContext) &&
                        mRemoteCallProfile.mCallType == ImsCallProfile.CALL_TYPE_VT_NODIR) {
                        //Restore the original remote capabilities
                        setRemoteProfileCallType(mDc);
                        isChanged = true;
                    }

                    break;
                default:
                    Log.i(this, "Non-Hold/Resume supp svc code received.");
                    mListenerProxy.callSessionSuppServiceReceived(this, suppSvcNotification);
                    break;
            }

            if (isChanged) {
                 mListenerProxy.callSessionUpdated(this, mCallProfile);
            }
        }
    }

    /**
     * Call appropriate callbacks for notifying TTY mode info based on
     * UNSOL_TTY_NOTIFICATION for the call.
     *
     * @param mode
     */
    public void notifyTtyModeChange(int mode) {
        Log.i(this, "TTY mode = " + mode);

        if (!isSessionValid()) return;

        if (mListenerProxy != null) {
            // TTY mode notified by remote party.
            mListenerProxy.callSessionTtyModeReceived((IImsCallSession) this, mode);
        } else {
            Log.e(this, "notifyTtyModeChange ListenerProxy null! ");
        }
    }

    public void handleRetryErrorNotify(DriverCallIms dc) {
        int sipErrorCode = 0;
        switch (dc.mCallFailReason) {
            case CallFailCause.CALL_FAIL_SIP_FORBIDDEN:
                sipErrorCode = SIP_FORBIDDEN;
                break;
            case CallFailCause.CALL_FAIL_SIP_REQUEST_TIMEOUT:
                sipErrorCode = SIP_REQUEST_TIMEOUT;
                break;
            case CallFailCause.CALL_FAIL_SIP_TEMPORARILY_UNAVAILABLE:
                sipErrorCode = SIP_TEMPORARILY_UNAVAILABLE;
                break;
            case CallFailCause.CALL_FAIL_SIP_SERVER_INTERNAL_ERROR:
                sipErrorCode = SIP_SERVER_INTERNAL_ERROR;
                break;
            case CallFailCause.CALL_FAIL_SIP_SERVER_NOT_IMPLEMENTED:
                sipErrorCode = SIP_SERVER_NOT_IMPLEMENTED;
                break;
            case CallFailCause.CALL_FAIL_SIP_SERVER_BAD_GATEWAY:
                sipErrorCode = SIP_SERVER_BAD_GATEWAY;
                break;
            case CallFailCause.CALL_FAIL_SIP_SERVICE_UNAVAILABLE:
                sipErrorCode = SIP_SERVICE_UNAVAILABLE;
                break;
            case CallFailCause.CALL_FAIL_SIP_SERVER_VERSION_UNSUPPORTED:
                sipErrorCode = SIP_SERVER_VERSION_UNSUPPORTED;
                break;
            case CallFailCause.CALL_FAIL_SIP_SERVER_MESSAGE_TOOLARGE:
                sipErrorCode = SIP_SERVER_MESSAGE_TOOLARGE;
                break;
            case CallFailCause.CALL_FAIL_SIP_SERVER_PRECONDITION_FAILURE:
                sipErrorCode = SIP_SERVER_PRECONDITION_FAILURE;
                break;
            default:
                break;
        }
        if(sipErrorCode != 0) {
            Log.i(this, "Retry Error Notify " + sipErrorCode);
            String additionalCallInfo = ""+ImsCallProfile.EXTRA_ADDITIONAL_CALL_INFO+"=" +
                    sipErrorCode;
            dc.callDetails.addExtra(additionalCallInfo);
            mCallProfile.setCallExtraInt(ImsCallProfile.EXTRA_ADDITIONAL_CALL_INFO,
                sipErrorCode);
            //Display toast
            final boolean displayCsRetryToast = mContext.getResources().
                    getBoolean(R.bool.config_carrier_display_csretry_toast);
            if (displayCsRetryToast) {
                String msg = "LTE HD voice is unavailable. 3G voice call will be connected." +
                        "SIP Error code: " + sipErrorCode;
                Toast.makeText(mContext, msg, Toast.LENGTH_LONG).show();
            }
        }
    }

    public void handleHandover(int hoType, int srcTech, int targetTech,
            int extraType, byte[] extraInfo, String errorCode, String errorMessage) {
        Log.i(this, "hoType : " + hoType + "srcTech: " + srcTech +
                " targetTech: " + targetTech);

        if (!isSessionValid()) return;

        final int error = parseErrorCode(errorCode);

        switch (hoType) {
            case ImsQmiIF.START:
                break;
            case ImsQmiIF.COMPLETE_SUCCESS:
                mListenerProxy.callSessionHandover((IImsCallSession)this, srcTech,
                        targetTech, new ImsReasonInfo(error, 0, errorMessage));
                /**
                 * When a wifi call is handed over to LTE, then reset
                 * the wifi quality indication to none.
                 */
                if (getRadioTechFromCallMode(srcTech) == ServiceState.RIL_RADIO_TECHNOLOGY_IWLAN &&
                        getRadioTechFromCallMode(targetTech) ==
                        ServiceState.RIL_RADIO_TECHNOLOGY_LTE) {
                    maybeUpdateVoWifiCallQualityExtra(QtiCallConstants.VOWIFI_QUALITY_NONE, true);
                }
                break;
            case ImsQmiIF.CANCEL:
            case ImsQmiIF.COMPLETE_FAIL:
                Log.i(this, "HO Failure for WWAN->IWLAN " + extraType + extraInfo);
                if (extraType == ImsQmiIF.LTE_TO_IWLAN_HO_FAIL) {
                    mCallProfile.setCallExtraInt(CallDetails.EXTRAS_HANDOVER_INFORMATION,
                            CallDetails.EXTRA_TYPE_LTE_TO_IWLAN_HO_FAIL);
                }
                alertForHandoverFailed();
                mListenerProxy.callSessionHandoverFailed((IImsCallSession)this, srcTech,
                        targetTech, new ImsReasonInfo(error, 0, errorMessage));
                break;
            case ImsQmiIF.NOT_TRIGGERED:
                alertForHandoverFailed();
                mListenerProxy.callSessionHandoverFailed((IImsCallSession)this, srcTech,
                        targetTech, new ImsReasonInfo(error, 0, errorMessage));
                break;
            case ImsQmiIF.NOT_TRIGGERED_MOBILE_DATA_OFF:
                mListenerProxy.callSessionMayHandover((IImsCallSession)this, srcTech,
                        targetTech);
                break;
            default:
                Log.e(this, "Unhandled hoType: " + hoType);
        }
    }

    private int parseErrorCode(String errorCode) {
        // CD-04 is the operator specific error code for the call drop case where the user is at
        // the edge of Wifi coverage on a Wifi call and there is no LTE network available to
        // handover to.
        if ("CD-04".equals(errorCode)) {
            return ImsReasonInfo.CODE_CALL_DROP_IWLAN_TO_LTE_UNAVAILABLE;
        }
        return ImsReasonInfo.CODE_UNSPECIFIED;
    }

    //Handler for events tracking requests sent to ImsSenderRxr
    private class ImsCallSessionImplHandler extends Handler {
        ImsCallSessionImplHandler() {
            super();
        }

        @Override
        public void handleMessage(Message msg) {
            Log.i (this, "Message received: what = " + msg.what);
            if (!isSessionValid()) return;

            AsyncResult ar;

            switch (msg.what) {
                case EVENT_DIAL:
                    ar = (AsyncResult) msg.obj;
                    if (ar != null && ar.exception != null) {
                        Log.i(this, "Dial error");
                        //TODO: The Reason should convey more granular information
                        if(ar.userObj != null) {
                            int errorCode = ImsReasonInfo.CODE_UNSPECIFIED;
                            if ( ar.exception instanceof ImsException ) {
                                errorCode = ((ImsException) ar.exception).getCode();
                            }
                            if (errorCode == ImsReasonInfo.CODE_LOCAL_LOW_BATTERY ||
                                    errorCode == QtiCallConstants.
                                    CALL_FAIL_EXTRA_CODE_LOCAL_VALIDATE_NUMBER) {
                                mCallProfile.setCallExtraInt(QtiCallConstants.
                                         EXTRAS_KEY_CALL_FAIL_EXTRA_CODE, errorCode);
                                errorCode = ImsReasonInfo.CODE_UNSPECIFIED;
                            }
                            mListenerProxy.callSessionStartFailed((IImsCallSession)ar.userObj,
                                    new ImsReasonInfo(errorCode, 0, "Dial Failed"));
                        }
                    }
                    break;
                case EVENT_ADD_PARTICIPANT:
                    ar = (AsyncResult) msg.obj;
                    if (ar != null && ar.exception != null) {
                        Log.i(this, "Add Participant error");
                        //TODO: The Reason should convey more granular information
                        if(ar.userObj != null) {
                            mListenerProxy.callSessionInviteParticipantsRequestFailed(
                                    (IImsCallSession)ar.userObj,
                                    ImsCallUtils.getImsReasonInfo(ar));
                        }
                    } else {
                        mListenerProxy.callSessionInviteParticipantsRequestDelivered(
                                (IImsCallSession)ar.userObj);
                    }
                    break;
                case EVENT_ACCEPT:
                    ar = (AsyncResult) msg.obj;
                    if (ar != null && ar.exception != null) {
                        Log.i(this, "Accept error: " + ar.exception);
                    }
                    break;
                case EVENT_HANGUP:
                    ar = (AsyncResult) msg.obj;
                    if (ar != null && ar.exception != null) {
                        Log.i(this, "Hangup error: " + ar.exception);
                    }
                    mDisconnCause = ImsReasonInfo.CODE_USER_TERMINATED;
                    break;
                case EVENT_HOLD:
                    ar = (AsyncResult) msg.obj;
                    if (ar != null && ar.exception != null) {
                        Log.i(this, "Hold error");
                        maybeDisplaySuppServiceErrorMsg(
                                mContext.getResources().getString(R.string.call_hold_label),
                                (ImsRilException) ar.exception);
                        // TODO: We need to check if ImsReasonInfo error code is set here to
                        // ImsReasonInfo.CODE_LOCAL_CALL_TERMINATED, we should not override the
                        // code. Rather pass the hold failed error code in extra error code.
                        if(ar.userObj != null) {
                            mListenerProxy.callSessionHoldFailed((IImsCallSession)ar.userObj,
                                    ImsCallUtils.getImsReasonInfo(ar));
                        }
                    }
                    break;
                case EVENT_RESUME:
                    ar = (AsyncResult) msg.obj;
                    if (ar != null && ar.exception != null) {
                        Log.i(this, "Resume error");
                        maybeDisplaySuppServiceErrorMsg(
                                mContext.getResources().getString(R.string.call_resume_label),
                                (ImsRilException) ar.exception);
                        //TODO: The Reason should convey more granular information
                        if(ar.userObj != null) {
                            mListenerProxy.callSessionResumeFailed((IImsCallSession)ar.userObj,
                                    ImsCallUtils.getImsReasonInfo(ar));
                        }
                    }
                    break;
                case EVENT_REJECT:
                    ar = (AsyncResult) msg.obj;
                    if (ar != null && ar.exception != null) {
                        Log.i(this, "Reject error: " + ar.exception);
                    } else {
                        mDisconnCause = ImsReasonInfo.CODE_LOCAL_CALL_DECLINE;
                    }
                    break;
                case EVENT_DEFLECT:
                    ar = (AsyncResult) msg.obj;
                    if (ar != null && ar.exception != null) {
                        Log.i(this, "Deflect error");
                        //TODO: The Reason should convey more granular information
                        if(ar.userObj != null) {
                        /* TODO: Remove this comment added for bringup
                            mListenerProxy.callSessionDeflectFailed((IImsCallSession)ar.userObj,
                                    new ImsReasonInfo(ImsReasonInfo.CODE_UNSPECIFIED, 0,
                                            "Deflect Failed")); */
                        }
                    } else {
                        Log.i(this, "Deflect success");
                        if(ar.userObj != null) {
                        /* TODO: Remove this comment added for bringup
                            mListenerProxy.callSessionDeflected((IImsCallSession)ar.userObj);
                        */
                        }
                    }
                    break;
                case EVENT_RINGBACK_TONE:
                    ar = (AsyncResult) msg.obj;
                    if (ar != null) {
                        mRingbackToneRequest = (boolean) (ar.result);
                        Log.i(this, "EVENT_RINGBACK_TONE, playTone = " + mRingbackToneRequest);
                        if (mDc != null && mDc.state == DriverCallIms.State.ALERTING) {
                            ImsStreamMediaProfile mediaProfile = new ImsStreamMediaProfile();
                            if (mRingbackToneRequest == true) {
                                mediaProfile.mAudioDirection =
                                        ImsStreamMediaProfile.DIRECTION_INACTIVE;
                            }
                            mediaProfile.mRttMode = mDc.callDetails.rttMode;
                            mListenerProxy.callSessionProgressing(
                                    ImsCallSessionImpl.this, mediaProfile);
                        }
                    }
                    break;
                case EVENT_REMOVE_PARTICIPANT:
                     //TODO need to trigger callbacks in success and failure cases
                    break;
                case EVENT_CLOSE_SESSION:
                    doClose();
                    break;
                case EVENT_RTT_MESSAGE_RECEIVED:
                    ar = (AsyncResult) msg.obj;
                    if (ar != null && ar.exception != null) {
                        Log.e(this, "RTT: Message exception: " + ar.exception);
                    } else {
                        notifyReceivedRttMessage(ar.result);
                    }
                    break;
                case EVENT_SEND_RTT_MESSAGE:
                    ar = (AsyncResult) msg.obj;
                    if (ar != null && ar.exception != null) {
                        Log.e(this, "RTT: Send message exception: " + ar.exception);
                    } else {
                        Log.d(this, "RTT: EVENT_SEND_RTT_MESSAGE received");
                    }
                    break;
                case EVENT_SEND_RTT_MODIFY_REQUEST:
                    ar = (AsyncResult) msg.obj;
                    notifyRttModifyResponse(ar);
                    break;
                case EVENT_RTT_UPGRADE_CONFIRM_DONE:
                    ar = (AsyncResult) msg.obj;
                    if (ar != null && ar.exception != null) {
                        Log.e(this, "RTT:  upgarde response exception: " + ar.exception);
                    } else {
                        Log.d(this, "RTT: EVENT_RTT_UPGRADE_CONFIRM_DONE received");
                    }
                    break;
            }
        }
    }

    private boolean isSessionValid() {
        boolean isValid = (mState != ImsCallSession.State.INVALID);
        if (!isValid) {
            Log.e(this, "Session is closed");
        }
        return isValid;
    }
    /**
     * Utility function that returns
     * TRUE if there is an ongoing session,
     * FALSE otherwise
     */
    private boolean isImsCallSessionAlive() {
        return !(mState == ImsCallSession.State.TERMINATED ||
                 mState == ImsCallSession.State.TERMINATING ||
                 mState == ImsCallSession.State.IDLE ||
                 mState == ImsCallSession.State.INVALID);
    }

    private void doClose() {
        Log.i(this, "doClose!");

        if (isImsCallSessionAlive()) {
            Log.i(this, "Received Session Close request while it is alive");
        }

        if ( mState != ImsCallSession.State.INVALID ) {
            if (mImsCallModification != null) {
                mImsCallModification.close();
            }
            if (!mIsConferenceHostSession) {
                if (mListenerProxy != null) {
                    mListenerProxy.dispose();
                    mListenerProxy = null;
                }
            } else {
                Log.i(this, "Not clearing listener, ongoing merge.");
            }
            if (mDc != null && mDc.isMT == false && mCi != null) {
                mCi.unregisterForRingbackTone(mHandler);
            }
            notifySessionClosed();
            synchronized (mListeners) {
                mListeners.clear();
            }
            mCi = null;
            mDc = null;
            mCallId = DEFAULT_CALL_INDEX;
            mLocalCallProfile = null;
            mRemoteCallProfile = null;
            mCallProfile = null;
            mState = ImsCallSession.State.INVALID;
            mInCall = false;
            mHandler = null;
            mIsConfInProgress = false;
            newSession = null;
            mImsVideoCallProviderImpl = null;
            mImsCallModification = null;
            mCallee = null;
            mConfInfo = null;
            mIsLowBattery = false;
            mPhoneId = QtiCallConstants.INVALID_PHONE_ID;
        }
    }

    /**
     * Closes the object. This object is not usable after being closed.
     * This function is called when onCallTerminated is triggered from client side.
     */
    public void close() {
        Log.i(this, "Close!");
        if (mHandler != null) {
            mHandler.obtainMessage(EVENT_CLOSE_SESSION).sendToTarget();
        }
    }

    /**
     * Gets the call ID of the session.
     *
     * @return the call ID
     */
    public String getCallId() {
        return Integer.toString(mCallId);
    }

    /**
     * Gets the media ID of the session.
     *
     * @return the media ID
     */

    public int getMediaId() {
        if (!isSessionValid()) return CallDetails.MEDIA_ID_UNKNOWN;
        return (mDc != null ? mDc.callDetails.callMediaId : CallDetails.MEDIA_ID_UNKNOWN);
    }

    /**
     * Checks if mediaId of the call session is valid
     *
     * @return true if the mediaId is valid
     */

    public boolean hasMediaIdValid() {
        if (!isSessionValid()) return false;
        return (mDc != null ? mDc.callDetails.hasMediaIdValid() : false);
    }

    /**
     * Gets the call profile that this session is associated with
     *
     * @return the call profile that this session is associated with
     */
    public ImsCallProfile getCallProfile() {
        if (!isSessionValid()) return null;
        return mCallProfile;
    }

    /**
     * Gets the local call profile that this session is associated with
     *
     * @return the local call profile that this session is associated with
     */
    public ImsCallProfile getLocalCallProfile() {
        if (!isSessionValid()) return null;
        return mLocalCallProfile;
    }

    /**
     * Gets the remote call profile that this session is associated with
     *
     * @return the remote call profile that this session is associated with
     */
    public ImsCallProfile getRemoteCallProfile() {
        if (!isSessionValid()) return null;
        return mRemoteCallProfile;
    }

    /**
     * Gets the value associated with the specified property of this session.
     *
     * @return the string value associated with the specified property
     */
    public String getProperty(String name) {
        if (!isSessionValid()) return null;

        String value = null;

        if (mCallProfile != null) {
            value = mCallProfile.getCallExtra(name);
        } else {
            Log.e (this, "Call Profile null! ");
        }
        return value;
    }

    /**
     * Gets the session state. The value returned must be one of the states in
     * {@link ImsCallSession#State}.
     *
     * @return the session state
     */
    public int getState() {
        if (!isSessionValid()) return ImsCallSession.State.INVALID;
        return mState;
    }

    /**
     * Gets the Driver call state. The value returned must be one of the states in
     * {@link DriverCallIms#State}.
     *
     * @return the Driver call state
     */
    public DriverCallIms.State getInternalState() {
        if (!isSessionValid()) return DriverCallIms.State.END;

        DriverCallIms.State state = null;
        if (mDc != null) {
            state = mDc.state;
        } else if (mState == ImsCallSession.State.INITIATED) {
            state = DriverCallIms.State.DIALING;
        }
        return state;
    }

    public int getPhoneId() {
        return mPhoneId;
    }

    public int getInternalCallType() {
        if (!isSessionValid()) return CallDetails.CALL_TYPE_UNKNOWN;

        int callType = CallDetails.CALL_TYPE_UNKNOWN;
        if(mDc != null && mDc.callDetails != null){
            callType = mDc.callDetails.call_type;
        } else if (mState == ImsCallSession.State.INITIATED && mCallProfile != null) {
            callType = ImsCallUtils.convertToInternalCallType(mCallProfile.mCallType);
        }
        return callType;
    }

    public int getCallDomain() {
        if (!isSessionValid()) return CallDetails.CALL_DOMAIN_AUTOMATIC;

        int callDomain = CallDetails.CALL_DOMAIN_AUTOMATIC;
        if (mDc != null && mDc.callDetails != null) {
            callDomain = mDc.callDetails.call_domain;
        }
        return callDomain;
    }

    public boolean isMultipartyCall() {
        if (!isSessionValid()) return false;
        return (mDc != null) ? mDc.isMpty : false;
    }

    public DriverCallIms.State getDcState() {
        if (mDc != null) {
            return mDc.state;
        } else {
            Log.i(this, "Null mDc! Returning null!");
            return null;
        }
    }

    /**
     * Gets the Callee address for a MT Call
     *
     * @return the callee address
     */
    public String getCallee() {
        if (!isSessionValid()) return null;
        return mCallee;
    }

    public DriverCallIms.State getDriverCallState() {
        if (!isSessionValid()) return DriverCallIms.State.END;
        return mDc.state;
    }

    /**
     * Determines if the current session is multiparty.
     *
     * @return {@code True} if the session is multiparty.
     */
    public boolean isMultiparty() {
        if (!isSessionValid()) return false;
        if (mDc == null) {
            return false;
        }
        return mDc.isMpty;
    }

    /**
     * Determines if the call transfer to number is allowed.
     *
     * @return {@code True} if allowed.
     */
    public boolean canTransferToNumber() {
        if (!isSessionValid()) return false;
        if (mDc == null) {
            return false;
        }

        /* Blind & assured transfer are ALWAYS together, hence checking for blind is enough */
        return (mDc.mEctMask & QtiImsExtUtils.QTI_IMS_BLIND_TRANSFER) != 0;
    }

    /**
     * Determines if the call transfer to id is allowed.
     *
     * @return {@code True} if allowed.
     */
    public boolean canTransferToId() {
        if (!isSessionValid()) return false;
        if (mDc == null) {
            return false;
        }
        return (mDc.mEctMask & QtiImsExtUtils.QTI_IMS_CONSULTATIVE_TRANSFER) != 0;
    }

    /**
     * Checks if the session is in a call.
     *
     * @return true if the session is in a call
     */
    public boolean isInCall() {
        if (!isSessionValid()) return false;

        boolean isInCall = false;
        switch (mDc.state) {
            case ACTIVE:
            case HOLDING:
            case DIALING:
            case ALERTING:
            case INCOMING:
            case WAITING:
                isInCall = true;
                break;
        }
        return isInCall;
    }

    /**
     * Sets the listener to listen to the session events. A {@link IImsCallSession}
     * can only hold one listener at a time. Subsequent calls to this method
     * override the previous listener.
     *
     * @param listener to listen to the session events of this object
     */
    public void setListener(IImsCallSessionListener listener) {
        if (!isSessionValid()) return;
        mListenerProxy.mListener = listener;
    }

    /**
     * Mutes or unmutes the mic for the active call.
     *
     * @param muted true if the call is muted, false otherwise
     */
    public void setMute(boolean muted) {
        if (!isSessionValid()) return;
        //TODO:
    }

    /**
     * Method used to report a call to the IMS conference server that is
     * created when we want to merge two calls into a conference call.
     * This call is reported through the current foreground call.
     *
     * @param confCallSession The newly created conference call session.
     */
    public void reportNewConferenceCallSession(ImsCallSessionImpl confCallSession) {
        if (confCallSession != null) {
            Log.i(this, "Calling callSessionMergeStarted");
            mListenerProxy.callSessionMergeStarted((IImsCallSession) this,
                    confCallSession,
                    confCallSession.getCallProfile());
        } else {
            Log.e (this,
                    "Null confCallSession! Not calling callSessionMergeStarted");
        }
    }

    private void extractCallDetailsIntoCallProfile(DriverCallIms dcUpdate) {
        if (dcUpdate == null) {
            Log.e(this, "Null dcUpdate in extractCallDetailsIntoCallProfile");
            return;
        }
        updateImsCallProfile(dcUpdate);

        // Check for extra info. Example format provided.
        // Call Details = 0 2
        // Codec=AMR_NB AdditionalCallInfo=
        // P-Asserted-Identity: <sip:+15857470175@10.174.9.1;user=phone>
        // Call type = 0 , domain = 2
        // Extras[0] " Codec=AMR_NB
        // Extras[1]" AdditionalCallInfo=
        // P-Asserted-Identity: <sip:+15857470175@10.174.9.1;user=phone>"
        if (dcUpdate.callDetails.extras != null &&
                dcUpdate.callDetails.extras.length > 0) {
            String key = null;
            String[] keyAndValue = null;
            String[] namespaceAndKey = null;

            for (int i = 0; i < dcUpdate.callDetails.extras.length; i++) {
                if (dcUpdate.callDetails.extras[i] != null) {
                    keyAndValue = dcUpdate.callDetails.extras[i].split("=");
                    // Check for improperly formatted extra string.
                    if (keyAndValue[0] != null) {
                        // Check key string to check if namespace is present.
                        // If so, extract just the key.
                        // Example (key=value): "ChildNum=12345"
                        // Namespace example: "OEM:MyPhoneId=11"
                        if (keyAndValue[0].contains(":")) {
                            namespaceAndKey = keyAndValue[0].split(":");
                            key = namespaceAndKey[1];
                        } else {
                            key = keyAndValue[0];
                        }
                    } else {
                        Log.e(this, "Bad extra string from lower layers!");
                        return;
                    }
                } else {
                    Log.e(this, "Element " + i + " is null in CallDetails Extras!");
                    return;
                }

                /*Log.i(this, "CallDetails Extra key= " + key
                        + " value= " + keyAndValue[1]);*/
                mCallProfile.setCallExtra(key, keyAndValue[1]);
            }
        }

    }

    /**
     * Private Utility to translate Extras Codec to ImsStreamMediaProfile audio quality
     * @param codec string
     * @return int - audio quality
     */
    private static int mapAudioCodecFromExtras(String codec) {
        int audioQuality = ImsStreamMediaProfile.AUDIO_QUALITY_NONE;
        if (codec == null) {
            return ImsStreamMediaProfile.AUDIO_QUALITY_NONE;
        }
        switch (codec) {
            case "QCELP13K":
                audioQuality = ImsStreamMediaProfile.AUDIO_QUALITY_QCELP13K;
                break;
            case "EVRC":
                audioQuality = ImsStreamMediaProfile.AUDIO_QUALITY_EVRC;
                break;
            case "EVRC_B":
                audioQuality = ImsStreamMediaProfile.AUDIO_QUALITY_EVRC_B;
                break;
            case "EVRC_WB":
                audioQuality = ImsStreamMediaProfile.AUDIO_QUALITY_EVRC_WB;
                break;
            case "EVRC_NW":
                audioQuality = ImsStreamMediaProfile.AUDIO_QUALITY_EVRC_NW;
                break;
            case "AMR_NB":
                audioQuality = ImsStreamMediaProfile.AUDIO_QUALITY_AMR;
                break;
            case "AMR_WB":
                audioQuality = ImsStreamMediaProfile.AUDIO_QUALITY_AMR_WB;
                break;
            case "GSM_EFR":
                audioQuality = ImsStreamMediaProfile.AUDIO_QUALITY_GSM_EFR;
                break;
            case "GSM_FR":
                audioQuality = ImsStreamMediaProfile.AUDIO_QUALITY_GSM_FR;
                break;
            case "GSM_HR":
                audioQuality = ImsStreamMediaProfile.AUDIO_QUALITY_GSM_HR;
                break;
            case "G711U":
                audioQuality = ImsStreamMediaProfile.AUDIO_QUALITY_G711U;
                break;
            case "G723":
                audioQuality = ImsStreamMediaProfile.AUDIO_QUALITY_G723;
                break;
            case "G711A":
                audioQuality = ImsStreamMediaProfile.AUDIO_QUALITY_G711A;
                break;
            case "G722":
                audioQuality = ImsStreamMediaProfile.AUDIO_QUALITY_G722;
                break;
            case "G711AB":
                audioQuality = ImsStreamMediaProfile.AUDIO_QUALITY_G711AB;
                break;
            case "G729":
                audioQuality = ImsStreamMediaProfile.AUDIO_QUALITY_G729;
                break;
            case "EVS_NB":
                audioQuality = ImsStreamMediaProfile.AUDIO_QUALITY_EVS_NB;
                break;
            case "EVS_WB":
                audioQuality = ImsStreamMediaProfile.AUDIO_QUALITY_EVS_WB;
                break;
            case "EVS_SWB":
                audioQuality = ImsStreamMediaProfile.AUDIO_QUALITY_EVS_SWB;
                break;
            case "EVS_FB":
                audioQuality = ImsStreamMediaProfile.AUDIO_QUALITY_EVS_FB;
                break;
            default:
                Log.e(ImsCallSessionImpl.class.getSimpleName(), "Unsupported codec " + codec);
                break;
        }
        Log.i(ImsCallSessionImpl.class.getSimpleName(), "AudioQuality is " + audioQuality);
        return audioQuality;
    }

    /**
     * Temporary private Utility to translate ImsCallProfile to CallDetails call type
     * @param callType
     * @return int - call type
     */
    private int mapCallTypeFromProfile(int callType) {
        int type = CallDetails.CALL_TYPE_VOICE;
        switch (callType) {
            case ImsCallProfile.CALL_TYPE_VOICE_N_VIDEO:
                type = CallDetails.CALL_TYPE_UNKNOWN;
                break;
            case ImsCallProfile.CALL_TYPE_VOICE:
                type = CallDetails.CALL_TYPE_VOICE;
                break;
            case ImsCallProfile.CALL_TYPE_VT:
                type = CallDetails.CALL_TYPE_VT;
                break;
            case ImsCallProfile.CALL_TYPE_VT_TX:
                type = CallDetails.CALL_TYPE_VT_TX;
                break;
            case ImsCallProfile.CALL_TYPE_VT_RX:
                type = CallDetails.CALL_TYPE_VT_RX;
                break;
            case ImsCallProfile.CALL_TYPE_VT_NODIR:
                type = CallDetails.CALL_TYPE_VT_NODIR;
                break;
        }
        return type;
    }

    private int getRadioTechFromCallMode(int callMode) {
        int radioTech;
        switch (callMode) {
            case ImsQmiIF.RADIO_TECH_LTE:
                radioTech = ServiceState.RIL_RADIO_TECHNOLOGY_LTE;
            break;
            case ImsQmiIF.RADIO_TECH_WIFI:
            case ImsQmiIF.RADIO_TECH_IWLAN:
                radioTech = ServiceState.RIL_RADIO_TECHNOLOGY_IWLAN;
            break;
            default:
                radioTech = ServiceState.RIL_RADIO_TECHNOLOGY_UNKNOWN;
        }
        return radioTech;
    }

    /**
     * Update mCallProfile as per Driver call.
     * @param Drivercall
     */
    private void updateImsCallProfile(DriverCallIms dc) {
        if(dc == null) {
            Log.e(this, "updateImsCallProfile called with dc null");
            return;
        }

        if(mCallProfile == null) {
            mCallProfile = new ImsCallProfile();
        }

        // Get the call mode from local ability.
        int callMode = ImsQmiIF.RADIO_TECH_UNKNOWN;
        if (dc.callDetails != null && dc.callDetails.localAbility != null) {

            for (int i = 0; i < dc.callDetails.localAbility.length; i++) {
                ServiceStatus servStatus = dc.callDetails.localAbility[i];
                if (servStatus.type == dc.callDetails.call_type) {
                    callMode = servStatus.accessTechStatus[0].networkMode;
                    // Note: We are expecting only one access tech status per service
                    //       state object, since there can be only one of it per call.
                    break;
                }
            }
            mCallProfile.setCallExtra(ImsCallProfile.EXTRA_CALL_RAT_TYPE,
                    Integer.toString(getRadioTechFromCallMode(callMode)));
        }

        mCallProfile.setCallExtra(ImsCallProfile.EXTRA_OI, dc.number);
        mCallProfile.setCallExtra(ImsCallProfile.EXTRA_CNA, dc.name);
        mCallProfile.setCallExtraInt(ImsCallProfile.EXTRA_OIR,
                ImsCallProfile.presentationToOIR(dc.numberPresentation));
        mCallProfile.setCallExtraInt(ImsCallProfile.EXTRA_CNAP,
                ImsCallProfile.presentationToOIR(dc.namePresentation));
        mCallProfile.setCallExtraInt(QtiCallConstants.CALL_SUBSTATE_EXTRA_KEY,
                dc.callDetails.callsubstate);
        mCallProfile.setCallExtraBoolean(
                QtiCallConstants.CALL_ENCRYPTION_EXTRA_KEY, dc.isEncrypted);
        Log.i(this, "updateImsCallProfile :: Packing encryption=" + dc.isEncrypted
                + " in mCallProfile's extras.");
        if (dc.historyInfo != null && !dc.historyInfo.isEmpty()) {
            mCallProfile.mCallExtras.putStringArrayList(
                    QtiCallConstants.EXTRAS_CALL_HISTORY_INFO,
                    extractHistoryInfoDetails(dc.historyInfo));
        }

        if (dc.mEctMask != 0) {
            mCallProfile.setCallExtraInt(
                    QtiImsExtUtils.QTI_IMS_TRANSFER_EXTRA_KEY, dc.mEctMask);
        }

        /**
         * When the conference support is indicated by lower layers, then
         *      -- for VT calls, set the extra to the value received from lower layers
         *      -- for other calls, set the extra as true i.e. always supported.
         */
        if (dc.isConfSupportIndicated()) {
            mCallProfile.setCallExtraBoolean(
                    QtiCallConstants.CONF_SUPPORT_IND_EXTRA_KEY,
                    ImsCallUtils.isVideoCall(dc.callDetails.call_type)?
                    dc.isVideoConfSupported() : true);
        }

        if (ImsCallUtils.hasCallTypeChanged(mDc, dc) || hasCauseCodeChanged(mDc, dc)) {
            mCallProfile.setCallExtraInt(
                    QtiCallConstants.SESSION_MODIFICATION_CAUSE_EXTRA_KEY,
                    dc.callDetails.causeCode);
        }
        maybeUpdateLowBatteryStatus();
        mCallProfile.setCallExtraInt(
                QtiImsExtUtils.QTI_IMS_PHONE_ID_EXTRA_KEY, mPhoneId);

        switch (dc.callDetails.call_type) {
            case CallDetails.CALL_TYPE_UNKNOWN:
                mCallProfile.mCallType = ImsCallProfile.CALL_TYPE_VOICE_N_VIDEO;
                mCallProfile.mMediaProfile.mVideoDirection
                    = ImsStreamMediaProfile.DIRECTION_INVALID;
                break;
            case CallDetails.CALL_TYPE_VOICE:
                mCallProfile.mCallType = ImsCallProfile.CALL_TYPE_VOICE;
                mCallProfile.mMediaProfile.mVideoDirection
                    = ImsStreamMediaProfile.DIRECTION_INVALID;
                break;
            case CallDetails.CALL_TYPE_VT:
                mCallProfile.mCallType = ImsCallProfile.CALL_TYPE_VT;
                mCallProfile.mMediaProfile.mVideoDirection
                    = ImsStreamMediaProfile.DIRECTION_SEND_RECEIVE;
                break;
            case CallDetails.CALL_TYPE_VT_TX:
                mCallProfile.mCallType = ImsCallProfile.CALL_TYPE_VT_TX;
                mCallProfile.mMediaProfile.mVideoDirection
                    = ImsStreamMediaProfile.DIRECTION_SEND;
                break;
            case CallDetails.CALL_TYPE_VT_RX:
                mCallProfile.mCallType = ImsCallProfile.CALL_TYPE_VT_RX;
                mCallProfile.mMediaProfile.mVideoDirection
                    = ImsStreamMediaProfile.DIRECTION_RECEIVE;
                break;
            case CallDetails.CALL_TYPE_VT_NODIR:
                // Not propagating VT_NODIR call type to UI
                mCallProfile.mMediaProfile.mVideoDirection
                    = ImsStreamMediaProfile.DIRECTION_INACTIVE;
                break;
        }

        // Map Rtt attribute from CallDetails to ImsCallProfile
        switch (dc.callDetails.rttMode) {
            case RttMode.RTT_MODE_DISABLED:
                mCallProfile.mMediaProfile.mRttMode = ImsStreamMediaProfile.RTT_MODE_DISABLED;
                break;
            case RttMode.RTT_MODE_FULL:
                mCallProfile.mMediaProfile.mRttMode = ImsStreamMediaProfile.RTT_MODE_FULL;
                break;
        }

        /**
         * Update the wifi quality indication to the call extras as
         * -- global value <mVoWifiQuality> when
         *      1) a wifi call becomes active
         *      2) a LTE call is handed over to wifi
         *
         * Note that updating the quality indication when a wifi call
         * is handed over to LTE, is taken care in the handleHandover() API.
         */
        if (getRadioTechFromCallMode(callMode) == ServiceState.RIL_RADIO_TECHNOLOGY_IWLAN) {
            maybeUpdateVoWifiCallQualityExtra(mVoWifiQuality, false);
        }
    }

    /**
     * Utility method to detect if call details cause code has changed
     * @return boolean - true if both driver calls are not null and cause code has changed
     */
    public static boolean hasCauseCodeChanged(DriverCallIms dc, DriverCallIms dcUpdate) {
        return (dc != null && dcUpdate != null &&
                dc.callDetails.causeCode != dcUpdate.callDetails.causeCode);
    }

    /**
     * Utility method to return when true when conference call is in progress
     * @return boolean - true if conference is in progress
     */
    public boolean isConfInProgress() {
        if (!isSessionValid()) return false;
        return mIsConfInProgress;
    }

    /**
     * Utility method to set new Call Session for conference call scenario
     */
    public void setNewSession(ImsCallSessionImpl session) {
        if (!isSessionValid()) return;
        newSession = session;
    }

    /**
     * Initiates an IMS call with the specified target and call profile. The session listener is
     * called back upon defined session events. The method is only valid to call when the session
     * state is in {@link ImsCallSession#State#IDLE}.
     * @param callee dialed string to make the call to
     * @param profile call profile to make the call with the specified service type, call type and
     *            media information
     * @see Listener#callSessionStarted, Listener#callSessionStartFailed
     */
    public void start(String callee, ImsCallProfile profile) {
        if (!isSessionValid()) return;

        mCallProfile.mCallType = profile.mCallType;
        mCallProfile.mMediaProfile = profile.mMediaProfile;
        mCallProfile.mMediaProfile.setRttMode(profile.mMediaProfile.mRttMode);

        mState = ImsCallSession.State.INITIATED;
        mCallee = callee;
        //TODO add domain selection from ImsPhone
        //TODO emergency calls -lookup profile

        int clir = profile.getCallExtraInt(ImsCallProfile.EXTRA_OIR);
        int domain = CallDetails.CALL_DOMAIN_AUTOMATIC;
        boolean isEncrypted = false;
        if (SystemProperties.getInt(PROPERTY_DBG_ENCRYPTION_OVERRIDE,
                PROPERTY_DBG_ENCRYPTION_OVERRIDE_DEFAULT) == 1) {
            isEncrypted = true;
        } else {
            Bundle callExtras = profile.mCallExtras.getBundle(ImsCallProfile.EXTRA_OEM_EXTRAS);
            if (callExtras != null) {
                isEncrypted = callExtras
                        .getBoolean(QtiCallConstants.CALL_ENCRYPTION_EXTRA_KEY);
                callExtras.remove(QtiCallConstants.CALL_ENCRYPTION_EXTRA_KEY);
            }
        }

        /* TODO: Remove comment added for bringup
        if (profile.getCallExtraInt(ImsCallProfile.EXTRA_CALL_DOMAIN, -1) != -1) {
            domain = profile.getCallExtraInt(ImsCallProfile.EXTRA_CALL_DOMAIN, -1);
            Log.i(this, "start: domain from extra = " + domain);
        }*/
        CallDetails details = new CallDetails(mapCallTypeFromProfile(profile.mCallType),
                domain,
                null);
        extractProfileExtrasIntoCallDetails(profile, details);

        Log.v(this, "RTT: start rtt mode = " + profile.mMediaProfile.mRttMode);
        details.setRttMode(profile.mMediaProfile.mRttMode);
        details.setCallPull(profile.getCallExtraBoolean(ImsCallProfile.EXTRA_IS_CALL_PULL, false));

        boolean carrierOneDial = isCarrierOneDial(details);
        if (carrierOneDial) {
            mCallProfile.setCallExtraInt(ImsCallProfile.EXTRA_OIR,
                    ImsCallProfile.presentationToOIR(PhoneConstants.PRESENTATION_ALLOWED));
            mCallProfile.setCallExtraInt(ImsCallProfile.EXTRA_CNAP,
                    ImsCallProfile.presentationToOIR(PhoneConstants.PRESENTATION_ALLOWED));
        }

        /* If phoneId is unavailable soon after initiating a call, configuration
         * flag based checks that depend on phoneId at InCallUI are failing. So,
         * Pass phoneId via <QTI_IMS_PHONE_ID_EXTRA_KEY> extra.
         */
        mCallProfile.setCallExtraInt(
                QtiImsExtUtils.QTI_IMS_PHONE_ID_EXTRA_KEY, mPhoneId);
        // Trigger a session update so that extras reach InCallUI
        mListenerProxy.callSessionUpdated(this, mCallProfile);
        if (carrierOneDial && mIsLowBattery) {
            /* For carrier one, When UE is under low battery, video call will be
               placed based on user confirmation with resumePendingCall API */
             Log.d(this, "defer low battery video call dial request");
             return;
        }

        if (!canDial(details)) {
            failDialRequest(ImsReasonInfo.CODE_LOCAL_LOW_BATTERY);
            return;
        }

        if (isConfigEnabled(R.bool.config_regional_number_patterns_video_call)
                && ImsCallUtils.isVideoCallTypeWithDir(details.call_type)
                && !ImsCallUtils.isVideoCallNumValid(mCallee)) {
            failDialRequest(QtiCallConstants.CALL_FAIL_EXTRA_CODE_LOCAL_VALIDATE_NUMBER);
            return;
        }

        mCi.dial(callee, clir, details, isEncrypted,
                mHandler.obtainMessage(EVENT_DIAL, this));
    }

    private ArrayList<String> extractHistoryInfoDetails(String historyInfoString) {
        ArrayList<String> historyInfoStrings = new ArrayList<String>();
        String[] historyInfoHops = historyInfoString.split("[\\r\\n]+");
        // The interface states that the separator is a '\r\n' character. The
        // regex ensures empty strings will not be created.
        for (int i = 0; i < historyInfoHops.length; i++) {
            historyInfoStrings.add(historyInfoHops[i]);
            Log.v(this, "extractHistoryInfoDetails :: historyInfoHops[" + i + "]="
                    + historyInfoHops[i]);
        }
        return historyInfoStrings;
    }

    private boolean isCarrierOneDial(CallDetails details) {
        return (ImsCallUtils.isCarrierOneSupported() && details != null &&
                ImsCallUtils.isVideoCallTypeWithDir(details.call_type) &&
                maybeUpdateLowBatteryStatus());
    }

    private void failDialRequest(int reason) {
        final Message newMsg = mHandler.obtainMessage(EVENT_DIAL, this);
        AsyncResult.forMessage(newMsg, null, new ImsException("Dial Failed",
                reason));
        newMsg.sendToTarget();
    }

    private boolean canDial(CallDetails details) {
        return !(LowBatteryHandler.getInstance().isLowBattery() &&
                !ImsCallUtils.isCarrierOneSupported()) ||
                !ImsCallUtils.isNotCsVideoCall(details);
    }

    public void resumePendingCall(int videoState) {
        Log.d(this, "resumePendingCall VideoState = " + videoState);
        // Modify the callType and go ahead with placing the call
        mCallProfile.mCallType = ImsCallProfile.getCallTypeFromVideoState(videoState);
        start(mCallee, mCallProfile);
    }

    private void extractProfileExtrasIntoCallDetails(ImsCallProfile profile,
            CallDetails details) {
        Bundle callExtras = profile.mCallExtras.getBundle(ImsCallProfile.EXTRA_OEM_EXTRAS);
        if (callExtras != null) {
            String extraString = null;
            String[] extras = new String[callExtras.size()];
            int i = 0;

            // Pack the extras in a 'key=value' format in the extras String[]
            // in CallDetails.
            for (String key : callExtras.keySet()) {
                extraString = new String(key + "=" + (callExtras.get(key) == null ? "" :
                        callExtras.get(key).toString()));
                //Log.i(this, "Packing extra string: " + extraString);
                extras[i] = extraString;
                i++;
            }
            details.setExtras(extras);
        } else {
            Log.i(this, "No extras in ImsCallProfile to map into CallDetails.");
        }
    }

    /**
     * Initiates an IMS call with the specified participants and call profile.
     * The session listener is called back upon defined session events.
     * The method is only valid to call when the session state is in
     * {@link ImsCallSession#State#IDLE}.
     *
     * @param participants participant list to initiate an IMS conference call
     * @param profile call profile to make the call with the specified service type,
     *      call type and media information
     * @see Listener#callSessionStarted, Listener#callSessionStartFailed
     */
    public void startConference(String[] participants, ImsCallProfile profile) {
        if (!isSessionValid()) return;

        mCallProfile = profile; // TODO update profile and do not replace
        mState = ImsCallSession.State.INITIATED;
        mCallee = participants[0];
        // TODO add domain selection from ImsPhone
        // TODO clir & emergency calls -lookup profile
        final java.util.Map<String, String> extrasMap =
                com.google.android.collect.Maps.newHashMap();
        extrasMap.put(CallDetails.EXTRAS_IS_CONFERENCE_URI,
                Boolean.toString(true));
        String[] mMoExtras = CallDetails.getExtrasFromMap(extrasMap);

        CallDetails details = new CallDetails(mapCallTypeFromProfile(profile.mCallType),
                CallDetails.CALL_DOMAIN_AUTOMATIC, mMoExtras);
        mCi.dial(participants[0], CommandsInterface.CLIR_DEFAULT, details, false,
                mHandler.obtainMessage(EVENT_DIAL, this));
    }

    /**
     * Accepts an incoming call or session update.
     *
     * @param callType call type specified in {@link ImsCallProfile} to be answered
     * @param profile stream media profile {@link ImsStreamMediaProfile} to be answered
     * @see Listener#callSessionStarted
     */
    public void accept(int callType, ImsStreamMediaProfile profile) {
        if (!isSessionValid()) return;

        Log.v(this, "RTT: rttMode: " + mapRttModeFromProfile(profile.mRttMode));
        mCi.acceptCall(mHandler.obtainMessage(EVENT_ACCEPT, this),
                mapCallTypeFromProfile(callType), mapRttModeFromProfile(profile.mRttMode));
    }

    /**
     * Deflects an incoming call to given number.
     *
     * @param deflectNumber number to deflect the call
     */
    public void deflect(String deflectNumber) {
        if (!isSessionValid()) return;

        mCi.deflectCall(mCallId, deflectNumber,
                mHandler.obtainMessage(EVENT_DEFLECT, this));
    }

    /**
     * Rejects an incoming call or session update.
     *
     * @param reason reason code to reject an incoming call
     * @see Listener#callSessionStartFailed
     */
    public void reject(int reason) {
        if (!isSessionValid()) return;

        Log.i(this, "reject " + reason);
        /* If call is rejected when battery is low, send low battery failcause
           to lower layers. Also call end reason to apps need to be reported as low battery */
        mIsCallTerminatedDueToLowBattery = isLowBatteryVideoCall();
        if (mIsCallTerminatedDueToLowBattery &&
                !QtiImsExtUtils.allowVideoCallsInLowBattery(mPhoneId, mContext)) {
            reason = ImsReasonInfo.CODE_LOW_BATTERY;
        }

        mCi.hangupWithReason(mCallId, null, null, false, reason, null,
                mHandler.obtainMessage(EVENT_REJECT, this));
    }

    public boolean isCallActive() {
        if (!isSessionValid()) return false;
        return getInternalState() == DriverCallIms.State.ACTIVE;
    }

    /*
     * This methods return TRUE if call to be terminated is a low battery
     * MO Video call that is pending user input on low battery alert dialogue
     * to proceed further else return FALSE
     */
     private boolean isTerminateLowBatteryCall() {
         /*
          * Return FALSE if below conditions satisfy:
          * 1. CarrierOne is not enabled
          * 2. Session is not a low battery Video session
          * 3. Session state transitioned to DIALING state meaning there is
          *    user input on low battery alert dialog
          */
         if (!ImsCallUtils.isCarrierOneSupported() || !mIsLowBattery ||
                 (getInternalCallType() != CallDetails.CALL_TYPE_VT) ||
                 (mDc != null && mDc.state == DriverCallIms.State.DIALING)) {
             return false;
         }

         //return TRUE if session is still in initiated state
         return (mState == ImsCallSession.State.INITIATED);
     }

    /*
     * Returns whether this call is a video call and is in Low battery
     * scenario.
     */
    private boolean isLowBatteryVideoCall() {
        return mIsLowBattery && ImsCallUtils.isVideoCall(getInternalCallType());
    }

    /**
     * Terminates a call.
     *
     * @see Listener#callSessionTerminated
     */
    public void terminate(int reason) {
        if (!isSessionValid()) return;

        if (isTerminateLowBatteryCall()) {
            Log.i(this, "terminate: fail deferred low battery video call with reason = " + reason);
            /* User is not interested anymore to continue further with the MO low
               battery video call. So, fail the deferred dial request */
            failDialRequest(reason);
            return;
        }

        Log.i(this, "terminate " + reason);

        /* If call is ended with low battery reason, need to pass the call end
           reason to upper layers. */
        if (reason == ImsReasonInfo.CODE_LOW_BATTERY) {
            mIsCallTerminatedDueToLowBattery = true;
        }

         mCi.hangupWithReason(mCallId, null, null, false, reason, null,
                mHandler.obtainMessage(EVENT_HANGUP, this));
    }

    /**
     * Puts a call on hold. When it succeeds, {@link Listener#callSessionHeld} is called.
     *
     * @param profile stream media profile {@link ImsStreamMediaProfile} to hold the call
     * @see Listener#callSessionHeld, Listener#callSessionHoldFailed
     */
    public void hold(ImsStreamMediaProfile profile) {
        if (!isSessionValid()) return;

        Log.i (this, "hold requested.");
        mCi.hold(mHandler.obtainMessage(EVENT_HOLD, this), mDc.index);
    }

    /**
     * Continues a call that's on hold. When it succeeds, {@link Listener#callSessionResumed}
     * is called.
     *
     * @param profile stream media profile {@link ImsStreamMediaProfile} to resume the call
     * @see Listener#callSessionResumed, Listener#callSessionResumeFailed
     */
    public void resume(ImsStreamMediaProfile profile) {
        if (!isSessionValid()) return;

        Log.i (this, "resume requested.");
        mCi.resume(mHandler.obtainMessage(EVENT_RESUME, this), mDc.index);
    }

    /**
     * Merges the active & hold call. When it succeeds, {@link Listener#callSessionMerged} is
     * called.
     * @see Listener#callSessionMerged, Listener#callSessionMergeFailed
     */
    public void merge() {
        if (!isSessionValid()) return;

        // Avoid multiple merge requests to lower layers
        if (!mStateChangeReportingAllowed) {
            Log.w(this, "merge request is already in progress, ignore this merge request");
            return;
        }

        // Request the Service Class Tracker to send a conference request
        // to the lower layers.
        mTracker.sendConferenceRequest(this);
    }

    /**
     * Updates the current call's properties (ex. call mode change: video upgrade / downgrade).
     *
     * @param callType call type specified in {@link ImsCallProfile} to be updated
     * @param profile stream media profile {@link ImsStreamMediaProfile} to be updated
     * @see Listener#callSessionUpdated, Listener#callSessionUpdateFailed
     */
    public void update(int callType, ImsStreamMediaProfile profile) {
        if (!isSessionValid()) return;
    }

    /**
     * Extends this call to the conference call with the specified recipients.
     *
     * @participants participant list to be invited to the conference call after extending the call
     * @see Listener#sessionConferenceExtened, Listener#sessionConferenceExtendFailed
     */
    public void extendToConference(String[] participants) {
        if (!isSessionValid()) return;
        //TODO
    }

    /**
     * Requests the conference server to invite an additional participants to the conference.
     *
     * @participants participant list to be invited to the conference call
     * @see Listener#sessionInviteParticipantsRequestDelivered,
     *      Listener#sessionInviteParticipantsRequestFailed
     */
    public void inviteParticipants(String[] participants) {
        if (!isSessionValid()) return;

        mCallee = participants[0];
        mCi.addParticipant(mCallee, CommandsInterface.CLIR_DEFAULT, null,
                mHandler.obtainMessage(EVENT_ADD_PARTICIPANT, this));
    }

    /**
     * Requests the conference server to remove the specified participants from the conference.
     *
     * @param participants participant list to be removed from the conference call
     * @see Listener#sessionRemoveParticipantsRequestDelivered,
     *      Listener#sessionRemoveParticipantsRequestFailed
     */
    public void removeParticipants(String[] participants) {
        if (!isSessionValid()) return;
        mCallee = participants[0];
        //Log.i(this, "removeParticipants user: " + mCallee);
        mCi.hangupWithReason(0, mCallee, null, true, ImsReasonInfo.CODE_USER_TERMINATED,
                null, mHandler.obtainMessage(EVENT_REMOVE_PARTICIPANT, this));
    }

    /**
     * Sends a DTMF code. According to <a href="http://tools.ietf.org/html/rfc2833">RFC 2833</a>,
     * event 0 ~ 9 maps to decimal value 0 ~ 9, '*' to 10, '#' to 11, event 'A' ~ 'D' to 12 ~ 15,
     * and event flash to 16. Currently, event flash is not supported.
     *
     * @param c the DTMF to send. '0' ~ '9', 'A' ~ 'D', '*', '#' are valid inputs.
     * @param result.
     */
    public void sendDtmf(char c, Message result) {
        if (!isSessionValid()) return;

        mCi.sendDtmf(c, result);
    }

    /**
     * Start a DTMF code. According to <a href="http://tools.ietf.org/html/rfc2833">RFC 2833</a>,
     * event 0 ~ 9 maps to decimal value 0 ~ 9, '*' to 10, '#' to 11, event 'A' ~ 'D' to 12 ~ 15,
     * and event flash to 16. Currently, event flash is not supported.
     *
     * @param c the DTMF to send. '0' ~ '9', 'A' ~ 'D', '*', '#' are valid inputs.
     */
    public void startDtmf(char c) {
        if (!isSessionValid()) return;
        mCi.startDtmf(c, null);
    }

    /**
     * Stop a DTMF code.
     */
    public void stopDtmf() {
        if (!isSessionValid()) return;
        mCi.stopDtmf(null);
    }

    public IImsVideoCallProvider getVideoCallProvider() {
        if (!isSessionValid()) return null;

        if (mImsVideoCallProviderImpl == null) {
            Log.i(this,"getVideoCallProvider: Video call provider is null");
            return null;
        }

        return mImsVideoCallProviderImpl.getInterface();
    }

    /**
     * Sends an USSD message.
     *
     * @param ussdMessage USSD message to send
     */
    public void sendUssd(String ussdMessage) {
        if (!isSessionValid()) return;
        //TODO
    }

    public void notifyConfInfo(byte[] confInfoBytes) {
        if (!isSessionValid()) return;

        mConfInfo.updateConfXmlBytes(confInfoBytes);
        mImsConferenceState = mConfInfo.getConfUriList();
        if (mListenerProxy != null) {
            mListenerProxy.callSessionConferenceStateUpdated((IImsCallSession) this,
                    mImsConferenceState);
        }
    }

    public void setConfInfo(ConfInfo confInfo) {
        if (!isSessionValid()) return;

        this.mConfInfo = confInfo;
    }

    public ConfInfo getConfInfo() {
        if (!isSessionValid()) return null;
        return mConfInfo;
    }

    public void updateFeatureCapabilities(boolean isVideo, boolean isVoice) {
        Log.i(this,"updateFeatureCapabilities video " + isVideo + " voice " + isVoice);
        if (!isSessionValid()) return;

        if (mIsVideoAllowed != isVideo || mIsVoiceAllowed != isVoice) {
            mIsVideoAllowed = isVideo;
            mIsVoiceAllowed = isVoice;
            if (mDc!= null && mDc.state != DriverCallIms.State.END) {
                setCapabilitiesInProfiles(mDc);
                maybeCreateVideoProvider(isVideo);
                if (mStateChangeReportingAllowed) {
                    mListenerProxy.callSessionUpdated(this, mCallProfile);
                }
            }
        }
    }

    /**
     * Displays the supplemetary service error messages if displaying failure toasts are
     * enabled via the config value - call.toast.supp_svc_fail which is set to 1 or 0.
     */
    private void maybeDisplaySuppServiceErrorMsg(String header, ImsRilException ex) {
        if (isSuppSvcToastMsgEnabled()) {
            final String msg = header + getSuppSvcErrorMessage(ImsCallUtils.toUiErrorCode(ex));
            Toast.makeText(mContext, msg, Toast.LENGTH_SHORT).show();
        }
    }

    /**
     * Checks to see if displaying supplementary service failure toasts are enabled via the
     * config value - call.toast.supp_svc_fail which is set to 1 or 0 (enabled/disabled)
     */
    private boolean isSuppSvcToastMsgEnabled() {

        // Default value to disable call supplementary service toast failures
        final int SUPP_SVC_TOAST_CONFIG_DISABLED = 0;

        // Value to enable call hold/resume toast failures
        final int SUPP_SVC_TOAST_CONFIG_ENABLED = 1;

        final int toastMsgEnabled = android.provider.Settings.Global.getInt(
                mContext.getContentResolver(), "call.toast.supp_svc_fail",
                SUPP_SVC_TOAST_CONFIG_DISABLED);
        return (toastMsgEnabled == SUPP_SVC_TOAST_CONFIG_ENABLED);
    }

    /**
     * Returns the correct error string based on the error code sent from RIL
     */
    private String getSuppSvcErrorMessage(int errorCode) {
        final int resId;
        switch (errorCode) {
            case QtiCallConstants.ERROR_CALL_SUPP_SVC_CANCELLED:
                resId = R.string.error_msg_cancelled;
                break;
            case QtiCallConstants.ERROR_CALL_SUPP_SVC_REINVITE_COLLISION:
                resId = R.string.error_msg_reinvite_collision;
                break;
            case QtiCallConstants.ERROR_CALL_SUPP_SVC_FAILED:
            default:
                resId = R.string.error_msg_failed;
                break;
        }
        return mContext.getResources().getString(resId);
    }

    public String toString() {
        return " callid= " + mCallId + " mediaId=" + getMediaId() + " mState= " + mState + " mDc="
                + mDc + " mCallProfile=" + mCallProfile + " mLocalCallProfile=" + mLocalCallProfile
                + " mRemoteCallProfile=" + mRemoteCallProfile;
    }

    public String toSimpleString() {
        return super.toString();
    }

    /** TODO: Implement this function if needed, may come in handy later
    private DriverCall.State mapSessionToDcState(int state) {

        ImsCallSession states
        public static final int IDLE = 0;
        public static final int INITIATED = 1;
        public static final int NEGOTIATING = 2;
        public static final int ESTABLISHING = 3;
        public static final int ESTABLISHED = 4;

        public static final int RENEGOTIATING = 5;
        public static final int REESTABLISHING = 6;

        public static final int TERMINATING = 7;
        public static final int TERMINATED = 8;

        DC states
        ACTIVE,
        HOLDING,
        DIALING,    // MO call only
        ALERTING,   // MO call only
        INCOMING,   // MT call only
        WAITING;

    } */
    private android.os.Handler mWifiAlertHandler;
    private static final long RESET_TIME = 30 * 60 * 1000;
    private static final int ALERT_HANDOVER = 1;
    private static final int RESET_ALERT_HANDOVER_TIME = 2;
    private static final int ALERT_TIMES_AFTER_DROP_CALL = 3;
    private static final int VOLUME_FOR_ALERT = 40;

    private boolean getResBoolean(String strResName, String strPackage) {
        try {
            Context resCtx = mContext.createPackageContext(strPackage,
                    Context.CONTEXT_IGNORE_SECURITY);
            if (resCtx == null) return false;
            Resources res=resCtx.getResources();
            if (res == null) return false;
            int resID = res.getIdentifier(strResName, "bool", strPackage);
            return res.getBoolean(resID);
        } catch (Exception err) {
            err.printStackTrace();
        }
        return false;
    }

    private void alertForHandoverFailed() {
        //this is related to wifi calling notification, and should use settings exist resource
        //which can be overlay by carrier
        boolean shouldAlert = getResBoolean("config_regional_wifi_calling_notificaion_enable",
            "com.android.settings");
        if (shouldAlert) {
            startAlert();
        }
    }

    private void startAlert() {
        if(mWifiAlertHandler == null){
            mWifiAlertHandler = new android.os.Handler(){
                private int numberOfAlerts = 0;
                public void handleMessage(Message msg){
                    switch (msg.what) {
                    case ALERT_HANDOVER:
                        if(numberOfAlerts == ALERT_TIMES_AFTER_DROP_CALL){
                            return;
                        }
                        this.removeMessages(RESET_ALERT_HANDOVER_TIME);
                        this.sendEmptyMessageDelayed(RESET_ALERT_HANDOVER_TIME, RESET_TIME);
                        startBeepForAlert();
                        numberOfAlerts++;
                        break;

                    case RESET_ALERT_HANDOVER_TIME:
                        numberOfAlerts = 0;
                        break;

                    default:
                        break;
                    }
                }
            };
        }
        mWifiAlertHandler.sendEmptyMessage(ALERT_HANDOVER);
    }

    private void startBeepForAlert(){
        new Thread() {
            public void run() {
                // Used the 40 percentage of maximum volume
                ToneGenerator mTone = new ToneGenerator(
                        AudioManager.STREAM_VOICE_CALL, VOLUME_FOR_ALERT );
                try {
                    mTone.startTone(ToneGenerator.TONE_PROP_ACK);
                    Thread.sleep(1000);
                    mTone.stopTone();
                } catch (Exception e) {
                    Log.i(this, "Exception caught when generator sleep " + e);
                } finally {
                    if (mTone != null) {
                        mTone.release();
                    }
                }
            };
        }.start();
        String TOAST_AFTER_DROPCALL = "Due to network conditions, the call may be dropped";
        Toast.makeText(mContext, TOAST_AFTER_DROPCALL, Toast.LENGTH_LONG).show();
    }

    private boolean isConfigEnabled(int resId) {
        return mContext.getResources().getBoolean(resId);
    }

    /**
     * Updates the wifi quality indication extra and indicates the same to upper layers
     * only if canNotify is true.
     */
    private void maybeUpdateVoWifiCallQualityExtra(int quality, boolean canNotify) {
       if (!QtiImsExtUtils.isVoWiFiCallQualityEnabled(mPhoneId, mContext)) {
           return;
       }

       if (getState() != ImsCallSession.State.ESTABLISHED ) {
           return;
       }

       Log.d(this, "maybeUpdateVoWifiCallQualityExtra Quality : " + quality);
       mCallProfile.setCallExtraInt(QtiCallConstants.VOWIFI_CALL_QUALITY_EXTRA_KEY,
               quality);

       if (canNotify) {
           maybeTriggerCallSessionUpdate(mDc);
       }
    }

    /**
     * For a wifi voice call, when there is a change in the wifi quality indication,
     * updates the wifi quality indication extra and indicates the same to upper layers.
     * @param quality, one of the QtiCallConstants.VOWIFI_QUALITY* values.
     */
    public void updateVoWiFiCallQuality(int quality) {
        if (quality != mVoWifiQuality) {
            mVoWifiQuality = quality;
            maybeUpdateVoWifiCallQualityExtra(quality, true);
        } else {
            Log.v(this, "updateVoWiFiCallQuality Unchanged : " + quality);
        }
    }

    // Sends RTT Message
    public void sendRttMessage(String rttMessage) {
        if (!isSessionValid()) return;

        mCi.sendRttMessage(rttMessage, mHandler.obtainMessage(EVENT_SEND_RTT_MESSAGE, this));
    }

    // RTT Upgrade/Downgrade request
    public void sendRttModifyRequest(ImsCallProfile toProfile) {
        if (!isSessionValid()) return;

        // Map profile to calldetails
        CallDetails details = new CallDetails(mapCallTypeFromProfile(toProfile.mCallType),
                CallDetails.CALL_DOMAIN_AUTOMATIC, null);

        // Copy over the RTT value
        details.setRttMode(toProfile.mMediaProfile.mRttMode);
        Log.v(this, "RTT: sendRttModifyRequest mode = " + toProfile.mMediaProfile.mRttMode);

        if (toProfile.mMediaProfile.mRttMode != RttMode.RTT_MODE_FULL
                && toProfile.mMediaProfile.mRttMode != RttMode.RTT_MODE_DISABLED) {
            throw new IllegalArgumentException("mRttMode is invalid.");
        }

        if (RttMode.RTT_MODE_FULL == toProfile.mMediaProfile.mRttMode) {
            Toast.makeText(mContext, mContext.getResources().getString(R.string.rtt_upgrade_sent),
                    Toast.LENGTH_SHORT).show();
        } else {
            Toast.makeText(mContext, mContext.getResources().getString(R.string.rtt_downgrade_sent),
                    Toast.LENGTH_SHORT).show();
        }
        mCi.modifyCallInitiate(mHandler.obtainMessage(EVENT_SEND_RTT_MODIFY_REQUEST, this),
                new CallModify(details, mCallId));
    }

    /*
     * Response for RTT upgrade request
     * @param response : true - accepted upgrade request
     *                   false - rejected upgrade request
     */
    public void sendRttModifyResponse(boolean response) {
        if (!isSessionValid()) return;

        CallDetails callDetails = new CallDetails(getInternalCallType(), getCallDomain(),
                null);
        CallModify callModify = new CallModify();
        callModify.call_index  = Integer.parseInt(getCallId());
        callModify.call_details = new CallDetails(callDetails);

        Log.v(this, "RTT: sendRttModifyResponse response = " + response);
        callModify.call_details.setRttMode(mapResponseToMode(response));

        Toast.makeText(mContext, mContext.getResources().getString(R.string.rtt_modify_response),
                Toast.LENGTH_SHORT).show();
        mCi.modifyCallConfirm(mHandler.obtainMessage(EVENT_RTT_UPGRADE_CONFIRM_DONE, this),
                callModify);
    }

    /*
     * Utility api to convert RTT attribute from
     * ImsCallProfile(ImsStreamMediaProfile) to CallDetails
     * @param mode : Rtt Mode
     */
    private int mapRttModeFromProfile(int mode) {
        int rttMode = RttMode.RTT_MODE_DISABLED;
        switch (mode) {
            case ImsStreamMediaProfile.RTT_MODE_DISABLED:
                rttMode = RttMode.RTT_MODE_DISABLED;
                break;
            case ImsStreamMediaProfile.RTT_MODE_FULL:
                rttMode = RttMode.RTT_MODE_FULL;
                break;
        }
        Log.v(this, "RTT: mapRttModeFromProfile mode = " + rttMode);
        return rttMode;
    }

    /*
     * Utility api to map upgrade response to RTT value
     * @param response : User response
     *                   true - RTT_MODE_FULL
     *                   false - RTT_MODE_DISABLED
     */
    private int mapResponseToMode(boolean response) {
        return (response ? RttMode.RTT_MODE_FULL: RttMode.RTT_MODE_DISABLED);
    }

    /**
     * Utility to get rtt from Service Status Update
     * @param callType - VoLTE, VT
     * @param list - local Ability
     * @return mode - rtt mode for the service type
     */
    private int getRttMode(int callType, ServiceStatus[] list) {
        int mode = RttMode.RTT_MODE_DISABLED;
        if (list != null) {
            for (ServiceStatus srv : list) {
                if (srv != null && srv.type == callType) {
                    mode = srv.rttMode;
                    break;
                }
            }
        }
        Log.v(this, "RTT: getRttMode rtt mode = " + mode);
        return mode;
    }

    // Received Unsol request for RTT upgrade, propagate to UI
    public void notifyRttModifyRequest(CallDetails callDetails) {
        if (!isSessionValid()) return;

        if (mListenerProxy == null) {
            Log.e(this, "RTT: notifyRttModifyRequest ListenerProxy null");
        }

        Log.v(this, "RTT: notifyRttModifyRequest rttMode = " + callDetails.getRttMode());
        // Create ImsCallProfile to be passed with required rtt attribute set
        ImsCallProfile profile = new ImsCallProfile();
        profile.mMediaProfile.setRttMode(callDetails.getRttMode());

        Toast.makeText(mContext,
                mContext.getResources().getString(R.string.rtt_upgrade_received),
                Toast.LENGTH_SHORT).show();

        mListenerProxy.callSessionRttModifyRequestReceived((IImsCallSession) this, profile);
    }

    // Propogate the received RTT message to UI
    public void notifyReceivedRttMessage(Object result) {
        if (!isCallActive()) {
            Log.e(this, "RTT: notifyReceivedRttMessage not allowed.");
            return;
        }

        String message = (String)result;

        if (message == null) {
            Log.d(this, "notifyReceivedRttMessage rtt msg null");
            return;
        }

        if (mListenerProxy == null) {
            Log.e(this, "notifyReceivedRttMessage ListenerProxy null");
            return;
        }

        Log.v(this, "RTT: notifyReceivedRttMessage rttMessage = " + message);
        mListenerProxy.callSessionRttMessageReceived(message);
    }

    // Propagate the RTT modify response to UI
    public void notifyRttModifyResponse(AsyncResult ar) {
        if (!isSessionValid()) return;

        int status = VideoProvider.SESSION_MODIFY_REQUEST_FAIL;

        if (ar != null && ar.exception != null) {
            Log.e(this, "RTT: modify request exception: " + ar.exception);
            status = ImsCallUtils.getUiErrorCode(ar.exception);
        } else {
            status = VideoProvider.SESSION_MODIFY_REQUEST_SUCCESS;
        }

        if (mListenerProxy == null) {
            Log.e(this, "notifyRttModifyResponse ListenerProxy null");
            return;
        }

        Log.v(this, "RTT: notifyRttModifyResponse status = " + status);
        mListenerProxy.callSessionRttModifyResponseReceived(status);
    }

}
