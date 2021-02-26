/*
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package org.codeaurora.ims;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.List;
import java.util.Iterator;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.regex.Pattern;
import java.util.regex.Matcher;

import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.AsyncResult;
import android.os.Handler;
import android.os.Message;
import android.telephony.VoLteServiceState;

import com.android.ims.ImsCallProfile;
import com.android.ims.ImsManager;
import com.android.ims.ImsReasonInfo;
import com.android.ims.ImsServiceClass;
import com.android.ims.ImsSuppServiceNotification;
import com.android.ims.internal.IImsCallSessionListener;
import com.android.ims.internal.IImsRegistrationListener;
import com.android.ims.internal.ImsCallSession;
import com.android.ims.internal.IImsCallSession;
import com.android.internal.annotations.VisibleForTesting;
import com.android.internal.telephony.Call;
import com.qualcomm.ims.utils.Log;

import org.codeaurora.ims.DriverCallIms;
import org.codeaurora.ims.ImsSenderRxr;
import org.codeaurora.ims.utils.QtiImsExtUtils;

public class ImsServiceClassTracker implements ImsCallSessionImpl.Listener{
    private int mServiceId; /* Key for Unique element in the object */
    private int mServiceClass; /* Key for Unique element in the object */
    private PendingIntent mIncomingCallIntent;
    // Call Id, Dial String --> Call Session
    private Map<String, ImsCallSessionImpl> mCallList;
    private ArrayList<ImsCallSessionImpl> mPendingSessionList;
    private List<ICallListListener> mCallListListeners;
    private static int currentMaxServiceId = 0;
    private ImsSenderRxr mCi = null;
    private Context mContext;

    private boolean mIsVideoSupported = false;
    private boolean mIsVoiceSupported = false;

    private Handler mHandler;
    private static final int EVENT_CONFERENCE = 1;
    private ImsCallSessionListenerProxy mConfHostListener;
    private ConferenceResult mConferenceResult;
    private ImsServiceSub mServiceSub;
    private boolean mNeedIgnoreCalls = false;
    private Call.SrvccState mSrvccStateFromIms = Call.SrvccState.NONE;

    /**
     * Action for the incoming call intent for the Phone app.
     * Internal use only.
     * @hide
     */
    private static final String ACTION_IMS_INCOMING_CALL = "com.android.ims.volte.incoming_call";

    /**
     * Part of the ACTION_IMS_INCOMING_CALL intents.
     * An integer value; service identifier obtained from {@link ImsManager#open}.
     * Internal use only.
     * @hide
     */
    public static final String EXTRA_SERVICE_ID = "android:imsServiceId";

    /**
     * Part of the ACTION_IMS_INCOMING_CALL intents.
     * An boolean value; Flag to indicate that the incoming call is a normal call or call for USSD.
     * The value "true" indicates that the incoming call is for USSD.
     * Internal use only.
     * @hide
     */
    public static final String EXTRA_USSD = "android:ussd";

    public static final String CONF_URI_DC_NUMBER = "Conference Call";

    //Constructor
    public ImsServiceClassTracker(int serviceClass, PendingIntent intent,
            ImsSenderRxr ci, Context context, ImsServiceSub serviceSub) {
        mServiceClass = serviceClass;
        mIncomingCallIntent = intent;
        mServiceId = createServiceId();
        mCi = ci;
        mContext = context;
        mCallList = new HashMap<String, ImsCallSessionImpl>();
        mPendingSessionList = new ArrayList<ImsCallSessionImpl>();
        mCallListListeners = new CopyOnWriteArrayList<>();
        mServiceSub = serviceSub;
    }

    private static class HandoverInfo {
        private int mType = 0;
        private int mSrcTech = 0;
        private int mTargetTech = 0;
        private int mExtraType = 0;
        private byte[] mExtraInfo = null;
        private String mErrorCode = null;
        private String mErrorMessage = null;
    }

    private static int createServiceId() {
        return ++currentMaxServiceId; //TODO: check roundtrip
    }

    /**
     * Get the service Id associated with this tracker object
     * @return service id
     */
    public int getServiceId() {
        return mServiceId;
    }

    /**
     * Get the service Class associated with this tracker object
     * @return service class
     */
    public int getServiceClass() {
        return mServiceClass;
    }

    /**
     * Get the call Count associated with this tracker object
     * @return call Count
     */
    @VisibleForTesting
    public int getCallCount() {
        return mCallList.size();
    }

    /**
     * Creates a intent for incoming call
     * @param callId - Unique ID for the incoming call
     * @param isUssd - If this is a USSD request
     * @return Intent
     */
    private Intent createIncomingCallIntent(String callId, boolean isUssd, boolean isUnknown) {
        Intent intent = new Intent();
        intent.putExtra(ImsManager.EXTRA_CALL_ID, callId);
        intent.putExtra(ImsManager.EXTRA_USSD, isUssd);
        intent.putExtra(ImsManager.EXTRA_SERVICE_ID, getServiceId());
        intent.putExtra(ImsManager.EXTRA_IS_UNKNOWN_CALL, isUnknown);
        return intent;
    }

    public void updateFeatureCapabilities(boolean isVideoSupported, boolean isVoiceSupported) {
        Log.i(this,"updateFeatureCapabilities video " + isVideoSupported + " voice " + isVoiceSupported);
        mIsVideoSupported = isVideoSupported;
        mIsVoiceSupported = isVoiceSupported;
        synchronized(mCallList) {
            for (Map.Entry<String, ImsCallSessionImpl> e : mCallList.entrySet()) {
                final ImsCallSessionImpl session = e.getValue();
                session.updateFeatureCapabilities(isVideoSupported, isVoiceSupported);
            }
        }
    }

    public void updateLowBatteryStatus() {
        synchronized(mCallList) {
            for (Map.Entry<String, ImsCallSessionImpl> e : mCallList.entrySet()) {
                final ImsCallSessionImpl session = e.getValue();
                session.updateLowBatteryStatus();
            }
        }
    }

    private class ConferenceResult {
        ConferenceResult() {
        }

        ImsCallSessionImpl activeCall;
        ImsCallSessionImpl heldCall;
        boolean shouldHaveTransientSession = true;
    }

    private void cleanupConferenceAttempt() {
        mConfHostListener = null;
        mConferenceResult = null;
    }

    public void handleConferenceResult(AsyncResult ar) {
        if (ar != null) {
            if (ar.exception != null) {
                // If the response is a failure,  we know that both the calls
                // have failed to get merged into the conference call.
                ImsReasonInfo reason = ImsCallUtils.getImsReasonInfo(ar);
                mConfHostListener.callSessionMergeFailed(
                        (IImsCallSession) mConferenceResult.activeCall, reason);
                cleanupConferenceAttempt();
                for (Iterator<Map.Entry<String, ImsCallSessionImpl>> it =
                    mCallList.entrySet().iterator(); it.hasNext();) {
                        Map.Entry<String, ImsCallSessionImpl> e = it.next();
                        ImsCallSessionImpl cs = e.getValue();
                        cs.unMuteStateReporting();
                        cs.updateMergeStatus(ImsCallUtils.SESSION_MERGE_FAILED);
                        cs.mIsConferenceHostSession = false;
                }
                return;
            } else {
                processConferenceResult();
            }
        }
    }

    public void processConferenceResult() {
        Log.i(this, "Conference response received. Processing final result.");

        boolean foundActiveCall = false;
        boolean foundHeldCall = false;

        // Check the states of existing calls. By this point of time, the call list
        // should be up to date with the final states of all the call sessions. If
        // sessions have ended as part of the merge process, they will not be present.
        for (Iterator<Map.Entry<String, ImsCallSessionImpl>> it =
                mCallList.entrySet().iterator(); it.hasNext();) {
            Map.Entry<String, ImsCallSessionImpl> e = it.next();
            ImsCallSessionImpl cs = e.getValue();

            if (cs.getDcState() == DriverCallIms.State.ACTIVE) {
                // Check for the active call. If scenarios where the calls have swapped,
                // the swapped active call will be detected in the Holding state check.
                mConferenceResult.activeCall = cs;

                foundActiveCall = true;
            } else if (cs.getDcState() == DriverCallIms.State.HOLDING) {
                foundHeldCall = true;
            }
        }

        if (!foundActiveCall) {
            //TODO: The only possible valid scenario might be all calls ending.
            //      Discussion required on how to handle this scenario.
        }

        // If there are no held calls, it implies both the calls got merged into the
        // conference, and hence ended.
        // NOTE: We cannot figure out genuine call drops here.
        if (!foundHeldCall) {
            mConferenceResult.heldCall = null;
        }

        // Call the merge success callback with the active call for 3 way conference
        // This active call is used to create transient session
        // For all other scenario pass null as the session would already exist
        if (mConferenceResult.shouldHaveTransientSession) {
            mConfHostListener.callSessionMergeComplete(
                    (IImsCallSession) mConferenceResult.activeCall);
        } else {
            mConfHostListener.callSessionMergeComplete(null);
        }

        // Un-mute all call sessions still present.
        for (Iterator<Map.Entry<String, ImsCallSessionImpl>> it =
                mCallList.entrySet().iterator(); it.hasNext();) {
            Map.Entry<String, ImsCallSessionImpl> e = it.next();

            ImsCallSessionImpl cs = e.getValue();
            cs.unMuteStateReporting();
            cs.updateMergeStatus(ImsCallUtils.SESSION_MERGE_COMPLETED);
            cs.mIsConferenceHostSession = false;

        }

        // Reset all conference call flow variables.
        cleanupConferenceAttempt();
    }

    public void sendConferenceRequest(ImsCallSessionImpl hostSession) {
        Log.i(this, "Conference request being requested by session = " + hostSession);

        mConferenceResult = new ConferenceResult();
        hostSession.mIsConferenceHostSession = true;

        // Cache the host session's listener before sending the request. If the host
        // session ends as part of the conference call flow, its listener will still
        // be required to call the callSessionMerged callback. Simply having a reference
        // will not work as it will be set to null once the call session ends.
        mConfHostListener = hostSession.mListenerProxy;

        // Block the intermediate call state reporting for all the call sessions
        // till conference merge is in progress. Also, cache a snapshot of the call
        // sessions. This will be used while processing the final call session states
        // after the conference merge process ends.
        for (Iterator<Map.Entry<String, ImsCallSessionImpl>> it =
                mCallList.entrySet().iterator(); it.hasNext();) {

            Map.Entry<String, ImsCallSessionImpl> e = it.next();
            ImsCallSessionImpl cs = e.getValue();

            cs.muteStateReporting();

            if (cs.getDcState() == DriverCallIms.State.ACTIVE) {
                mConferenceResult.activeCall = cs;
            } else if (cs.getDcState() == DriverCallIms.State.HOLDING) {
                mConferenceResult.heldCall = cs;
            }

            if (cs.isMultipartyCall()) {
                mConferenceResult.shouldHaveTransientSession = false;
            }
        }

        // Send the conference request to service sub.
        mServiceSub.sendConferenceRequest(this);
        hostSession.updateMergeStatus(ImsCallUtils.SESSION_MERGE_STARTED);
    }

    /*
     * Calculates overall srvcc state and determines whether new calls
     * need to be ignored.
     * When SRVCC state on IMS/CS pipe is COMPLETED, we need to ignore
     * any new calls arriving on ims pipe, as subsequent call staus indications
     * for the current calls are not expected on IMS.
     * Flag will be reset when call is received in either DIALING or INCOMING state
     * as UE is again on IMS and call status indications are expected on IMS
     */
    public void calculateOverallSrvccState(int[] ret) {
        int state = -1;
        if (ret != null && ret.length != 0) {
            state = ret[0];
        }
        Log.i(this,"calculateOverallSrvccState imsSrvccState = " + mSrvccStateFromIms +
                " CS SRVCC state = " + state);
        if (state != VoLteServiceState.HANDOVER_COMPLETED) {
            //IMS srvcc is completed or CS srvcc status is started or failed/cancel.
            //In such cases, reset mSrvccStateFromIms.
            mSrvccStateFromIms = Call.SrvccState.NONE;
        } else if (state == VoLteServiceState.HANDOVER_COMPLETED) {
            mNeedIgnoreCalls = true;
        }
    }

    /**
     * Method to provide the currently available calls related information
     * and explictly marking the individual call state as END.
     */
    public Object getCallsListToClear() {
        ArrayList<DriverCallIms> response = null;
        DriverCallIms dc;

        if (mCallList.size() > 0) {
            response = new ArrayList<DriverCallIms> ();

            synchronized(mCallList) {
                for (Iterator<Map.Entry<String, ImsCallSessionImpl>> it =
                        mCallList.entrySet().iterator(); it.hasNext();) {
                    Map.Entry<String, ImsCallSessionImpl> e = it.next();
                    ImsCallSessionImpl callSession = (ImsCallSessionImpl) e.getValue();
                    dc = new DriverCallIms();
                    dc.state = DriverCallIms.State.END;
                    dc.index = Integer.parseInt(callSession.getCallId());
                    dc.callDetails = new CallDetails();
                    dc.callDetails.call_type = callSession.getInternalCallType();
                    dc.callDetails.call_domain = callSession.getCallDomain();
                    dc.callFailCause =
                            new ImsReasonInfo(ImsReasonInfo.CODE_LOCAL_SERVICE_UNAVAILABLE, 0);
                    response.add(dc);
                }
            }
            Collections.sort(response);
        }

        return response;
    }

    /**
     * Handle a list of calls updated by the IMS stack
     * @param dcList
     */
    public void handleCalls(ArrayList<DriverCallIms> dcList) {
        Map <String, DriverCallIms> dcMap = new HashMap<String, DriverCallIms>();
        // First pass is to look at every call in dc and update the Call Session List
        final boolean disableVideo = shallDisableVideo(dcList);
        for (int i = 0; dcList!= null && i < dcList.size(); i++) {
            ImsCallSessionImpl callSession = null;
            DriverCallIms dc = maybeDisableVideo(dcList.get(i), disableVideo);
            if (mPendingSessionList != null) {
                synchronized(mPendingSessionList) {
                    for (Iterator<ImsCallSessionImpl> it = mPendingSessionList.iterator();
                            it.hasNext();) {
                        ImsCallSessionImpl s = it.next();
                        if (dc.state == DriverCallIms.State.DIALING) {
                            // Add to call list as we now have call id, remove from
                            // temp list
                            Log.i(this, "Found match call session in temp list, s = " + s);
                            Log.i(this, "Index in call list is " + dc.index);
                            addCall(dc.index, s, false);
                            // Remove from mPendingSessionList
                            it.remove();
                        }
                    }
                }
            }
            synchronized(mCallList) {
                callSession = mCallList.get(Integer.toString(dc.index));
            }
            // reset the mNeedIgnoreCalls to false for subsequent calls
            // only if its MO, MT or end. For all other states, ignore the update.
            if (((dc.state == DriverCallIms.State.DIALING) ||
                    (dc.state == DriverCallIms.State.INCOMING) ||
                    (dc.state == DriverCallIms.State.END)) && mNeedIgnoreCalls) {
                mNeedIgnoreCalls = false;
            }
            Log.i(this, "SRVCC mNeedIgnoreCalls = " + mNeedIgnoreCalls);
            if (callSession != null){
                // Pending MO, active call updates
                // update for a existing call - no callID but MO number for a dial request
                // Call collision scenario
                callSession.updateCall(dc);
            } else if (!mNeedIgnoreCalls) {
                boolean isUnknown = false;
                if (dc.state == DriverCallIms.State.END) {
                    //This is an unknown call probably already cleaned up as part of SRVCC
                    //just ignore this dc and continue with the dc list
                    continue;
                }
                callSession = new ImsCallSessionImpl(dc, mCi, mContext, this, mIsVideoSupported,
                        mServiceSub.getPhoneId());
                callSession.addListener(this);
                callSession.updateFeatureCapabilities(mIsVideoSupported, mIsVoiceSupported);
                mServiceSub.onNewCall();
                // TODO: This functionality of informing ServiceSub of a new call needs to
                // be revisited from a design point of view.
                if (dc.isMT) {
                    Log.i(this, "MT Call creating a new call session");
                    sendIncomingCallIntent(callSession, dc.index, false);
                } else if (dc.isMpty && dc.state == DriverCallIms.State.DIALING) {
                    Log.i(this, "Conference Call creating a new call session");
                    isUnknown = true;
                    //Get the old session
                    synchronized(mCallList) {
                        for (Iterator<Map.Entry<String, ImsCallSessionImpl>> it =
                                mCallList.entrySet().iterator(); it.hasNext();) {
                            Map.Entry<String, ImsCallSessionImpl> e = it.next();
                            ImsCallSessionImpl oldSession = (ImsCallSessionImpl) e.getValue();
                            if ( oldSession.mIsConferenceHostSession ) {
                                Log.i(this, "Set New Session as Id " + callSession.getCallId());
                                callSession.setConfInfo(oldSession.getConfInfo());
                                oldSession.setNewSession(callSession);
                                oldSession.reportNewConferenceCallSession(callSession);
                                // This is new conference call created as part of 3 way merge.
                                isUnknown = false;
                                break;
                            }
                        }
                    }
                    if (isUnknown) {
                        Log.i(this, "Phantom conference call Scenario");
                    } else {
                        addCall(dc.index, callSession, true);
                    }
                    callSession.updateConfSession(dc);
                } else if (dc.state != DriverCallIms.State.END) {
                   Log.i(this, "MO phantom Call Scenario. State = " + dc.state);
                   isUnknown = true;
                }
                if (isUnknown) {
                    sendIncomingCallIntent(callSession, dc.index, true);
                }
            }
            // If state is not END then add call to list of active calls
            if ( dc.state != DriverCallIms.State.END && !mNeedIgnoreCalls) {
                dcMap.put(Integer.toString(dc.index), dc);
            }

        }

        // Second pass to check if all Call Sessions are still active, dc will not contain
        // a disconnected call in the dc List, remove any call sessions that are not present
        // in dc List.
        synchronized(mCallList) {
            for (Iterator<Map.Entry<String, ImsCallSessionImpl>> it =
                    mCallList.entrySet().iterator(); it.hasNext();) {
                Map.Entry<String, ImsCallSessionImpl> e = it.next();
                if (dcMap.get(e.getValue().getCallId()) == null) { // Call Dropped!
                    // CallStartFailed/CallTerminated are triggered during updateCall
                    // when call state is END. Also callsession close is triggered by the
                    // component that creates the session.
                    // Here just remove the session from tracker.
                    it.remove();
                    notifyCallRemoved(e.getValue());
                }
            }
        }
    }

    private boolean shallDisableVideo(ArrayList<DriverCallIms> dcList) {
        if (QtiImsExtUtils.canHoldVideoCall(getPhoneId(), mContext)) {
            return false;
        }
        boolean hasActiveVoiceCall = false;
        boolean hasHeldVoiceCall = false;

        for (DriverCallIms dc : dcList ) {
            hasActiveVoiceCall |= dc.state == DriverCallIms.State.ACTIVE &&
                    ImsCallUtils.isVoiceCall(dc.callDetails.call_type);
            hasHeldVoiceCall |= dc.state == DriverCallIms.State.HOLDING &&
                    ImsCallUtils.isVoiceCall(dc.callDetails.call_type);
        }

        return (hasActiveVoiceCall && hasHeldVoiceCall);
    }

    private DriverCallIms maybeDisableVideo(DriverCallIms dc, final boolean disableVideo) {
        if (!disableVideo || dc == null || dc.callDetails == null ||
                dc.callDetails.localAbility == null) {
            return dc;
        }

        boolean isVideoDisabled = !ImsCallSessionImpl.isServiceAllowed(
                CallDetails.CALL_TYPE_VT, dc.callDetails.localAbility);
        if (isVideoDisabled == disableVideo) {
            return dc;
        }

        for (ServiceStatus srv : dc.callDetails.localAbility) {
            if (srv.type == CallDetails.CALL_TYPE_VT) {
                srv.status = ImsQmiIF.STATUS_DISABLED;
            }
        }
        return dc;
    }

    @Override
    public void onDisconnected(ImsCallSessionImpl session) {
    }

    @Override
    public void onClosed(ImsCallSessionImpl session) {
        Log.i(this, "onClosed for session " + session);
        // For all the states after DIALING state is reported by RIL
        if (mCallList != null) {
            synchronized(mCallList) {
                for (Iterator<Map.Entry<String, ImsCallSessionImpl>> it
                        = mCallList.entrySet().iterator(); it.hasNext();) {
                    Map.Entry<String, ImsCallSessionImpl> e = it.next();
                    Log.i (this, "List is " + e.getValue() + " session is " + session);
                    if (e.getValue() == session) {
                        // Here just remove the session from tracker.
                        Log.i(this, "Removing session on close " + session);
                        it.remove();
                        notifyCallRemoved(e.getValue());
                    }
                }
            }
        }
        // When there is a pending session waiting for a call object with DIALING state
        if (mPendingSessionList != null) {
            synchronized(mPendingSessionList) {
                for (Iterator<ImsCallSessionImpl> it = mPendingSessionList.iterator();
                        it.hasNext();) {
                    ImsCallSessionImpl s = it.next();
                    if (s == session) {
                        // Here just remove the session from tracker.
                        Log.i(this, "Removing session on close " + session);
                        it.remove();
                        notifyCallRemoved(s);
                    }
                }
            }
        }
    }

    @Override
    public void onUnsolCallModify(ImsCallSessionImpl session, CallModify callModify) {
    }

    public void handleModifyCallRequest(CallModify cm) {
        if (cm != null) {
            ImsCallSessionImpl callSession = null;
            synchronized(mCallList) {
                callSession = mCallList.get(Integer.toString(cm.call_index));
            }
            if (callSession != null) {
                callSession.onReceivedModifyCall(cm);
            } else {
                Log.e(this,"handleModifyCallRequest Error: callSession is null");
            }
        } else {
            Log.e(this,"handleModifyCallRequest Error: Null Call Modify request ");
        }
    }

    /**
     * Create a call session
     * @param profile - ImsCallProfile associated with this call
     * @param listener - listner for the call session
     * @return IImsCallSession object or null on failure
     */
    public ImsCallSessionImpl createCallSession(ImsCallProfile profile,
            IImsCallSessionListener listener) {
        ImsCallSessionImpl session = new ImsCallSessionImpl(profile, listener, mCi, mContext,
                this, mIsVideoSupported, mServiceSub.getPhoneId());
        session.addListener(this);
        session.updateFeatureCapabilities(mIsVideoSupported, mIsVoiceSupported);
        mServiceSub.onNewCall();
        synchronized(mPendingSessionList) {
            mPendingSessionList.add(session);
            notifyCallAdded(session);
        }
        return session;
    }

    /**
     * Get a call session associated with the callId
     * @param callId
     * @return IImsCallSession object
     */
    public ImsCallSessionImpl getCallSession(String callId) {
        ImsCallSessionImpl session = null;
        synchronized(mCallList) {
            session = mCallList.get(callId);
        }
        return session;
    }

    /**
     * Handle the call state changes for incoming (MT) Hold/Resume as part of
     * the UNSOL_SUPP_SVC_NOTIFICATION message.
     * TODO: Handle other supp_svc info here?
     *
     * @param info
     */
    public void handleSuppSvcUnsol(ImsQmiIF.SuppSvcNotification info) {
        Log.i(this, "handleSuppSvcUnsol connId= " + info.getConnId());
        synchronized (mCallList) {
            ImsCallSessionImpl callSession =
                    callSession = mCallList.get(Integer.toString(info.getConnId()));
            if (callSession != null) {
                boolean startOnHoldLocalTone = false;
                ImsSuppServiceNotification suppServiceInfo = new ImsSuppServiceNotification();
                suppServiceInfo.notificationType = info.getNotificationType();
                suppServiceInfo.code = info.getCode();
                suppServiceInfo.index = info.getIndex();
                suppServiceInfo.number = info.getNumber();
                final String forwardedCallHistory = info.getHistoryInfo();
                if (forwardedCallHistory != null && !forwardedCallHistory.isEmpty() ) {
                    suppServiceInfo.history = forwardedCallHistory.split("\r\n");
                }
                if (info.hasHoldTone()) {
                    startOnHoldLocalTone = info.getHoldTone();
                }
                Log.i(this, "handleSuppSvcUnsol suppNotification= " + suppServiceInfo +
                      " startOnHoldLocalTone = " + startOnHoldLocalTone);
                callSession.updateSuppServiceInfo(suppServiceInfo, startOnHoldLocalTone);
            } else {
                Log.e(this, "No call session found for number: "/* + info.getNumber()*/);
            }
        }
    }

    /**
     * Handles Conference refresh Info notified through UNSOL_REFRESH_CONF_INFO message
     * @param ar - the AsyncResult object that contains the refresh Info information
     */
    public void handleRefreshConfInfo(ImsQmiIF.ConfInfo confRefreshInfo) {
        byte[] confInfoBytes = null;
        int state = -1;
        ImsCallSessionImpl callSession = null;
        if (confRefreshInfo != null) {
            final com.google.protobuf.micro.ByteStringMicro refreshConfInfoUri = confRefreshInfo
                    .getConfInfoUri();
            if (refreshConfInfoUri != null
                    && refreshConfInfoUri.size() >= 1) {
                confInfoBytes = new byte[refreshConfInfoUri.size()];
                refreshConfInfoUri.copyTo(confInfoBytes, 0);
                if (confRefreshInfo.hasConfCallState()) {
                    state = confRefreshInfo.getConfCallState();
                } else {
                    /*
                     * defaulting to foreground call for backward compatibility before call state
                     * was added to the interface
                     */
                    state = ImsQmiIF.FOREGROUND;
                }
                callSession = getCallSessionWithMptyBitSet(state);
            }
        }

        Log.i(this, "handleRefreshConfInfo: confCallState = " + state + ", callSession = "
                + callSession /*+ ", confInfoBytes: " + confInfoBytes*/);
        /*
         * UE subscribes for conference xml as soon as it establishes session with conference
         * server.Multiparty bit will be updated only through Get_current_calls after all the
         * participants are merged to the call. So refresh info can be received during the interval
         * in which the conference request is sent and before the conference call reflects in the
         * Get_current-calls
         */
        if (confInfoBytes != null && callSession != null) {
            Log.i(this, "Update UI for Conference");
            callSession.notifyConfInfo(confInfoBytes);
        } else {
            Log.e(this, "No CallSession with Multiparty bit set is found. Some Error!!!");
        }
    }

    /**
     * Handle the TTY mode changes as part of the UNSOL_TTY_NOTIFICATION message.
     *
     * @param mode the mode informed via the indication
     */
    public void handleTtyModeChangeUnsol(int mode) {
        ImsCallSessionImpl callSession = null;

        // Check if any call session is active.
        synchronized(mCallList) {
            for (Iterator<Map.Entry<String, ImsCallSessionImpl>> it =
                    mCallList.entrySet().iterator(); it.hasNext();) {
                Map.Entry<String, ImsCallSessionImpl> e = it.next();
                if (e.getValue().getState() == ImsCallSession.State.ESTABLISHED) { // Call active
                    callSession = (ImsCallSessionImpl) e.getValue();
                    callSession.notifyTtyModeChange(mode);
                    break;
                }
            }
        }

        if (callSession == null) {
            Log.e(this, "No Active call session found for TTY mode change");
        }
    }

    private ImsCallSessionImpl getCallSessionWithMptyBitSet(int state) {
        synchronized(mCallList) {

            // Use the cached active session as during conference one of the sessions
            // will receive END as part of merge
            if ((mConferenceResult != null) && mConferenceResult.shouldHaveTransientSession) {
                Log.i(this, "getCallSessionWithMptyBitSet session = "
                        + mConferenceResult.activeCall);
                return mConferenceResult.activeCall;
            }

            for (Iterator<Map.Entry<String, ImsCallSessionImpl>> it = mCallList.entrySet()
                    .iterator(); it.hasNext();) {
                Map.Entry<String, ImsCallSessionImpl> e = it.next();
                ImsCallSessionImpl session = (ImsCallSessionImpl) e.getValue();
                DriverCallIms.State dcState = session.getDriverCallState();
                Log.i(this, "getCallSessionWithMptyBitSet:: ImsCallSession state = "
                        + session.getState() + ", isMultiparty = " + session.isMultiparty());

                if (session.isMultiparty() == true) {
                    Log.i(this, "ImsCallSession found with Multiparty bit set");
                    if ((dcState == DriverCallIms.State.DIALING ||
                            dcState == DriverCallIms.State.ALERTING ||
                            dcState == DriverCallIms.State.ACTIVE)
                            && (state == ImsQmiIF.FOREGROUND)) {
                        Log.i(this, "Foreground Conference callSession found");
                        return session;
                    } else if ((dcState == DriverCallIms.State.HOLDING)
                            && (state == ImsQmiIF.BACKGROUND)) {
                        Log.i(this, "Background Conference callSession found");
                        return session;
                    } else if ((dcState == DriverCallIms.State.INCOMING ||
                            dcState == DriverCallIms.State.WAITING)
                            && (state == ImsQmiIF.RINGING)) {
                        Log.i(this, "Ringing Conference callSession found");
                        return session;
                    }
                }
            }

            // Deprecated code
            for (Iterator<Map.Entry<String, ImsCallSessionImpl>> it = mCallList.entrySet()
                    .iterator(); it.hasNext();) {
                Map.Entry<String, ImsCallSessionImpl> e = it.next();
                ImsCallSessionImpl session = (ImsCallSessionImpl) e.getValue();
                if (session.isConfInProgress()) {
                    if (state == ImsQmiIF.FOREGROUND) {
                        Log.i(this, "Foreground ImsCallSession found during Conference setup");
                        return session;
                    }
                } else {
                    if (state == ImsQmiIF.BACKGROUND) {
                        Log.i(this, "Background ImsCallSession found");
                    }
                }
            }
            return null;
        }
    }

    /**
     * Gets list of call sessions that are in the given state.
     * @param state State of the call.
     * @return List of call session objects that have {@code state}
     */
    /* package */
    List<ImsCallSessionImpl> getCallSessionByState(DriverCallIms.State state) {
        List<ImsCallSessionImpl> sessionList = new ArrayList<ImsCallSessionImpl>();
        if (state == null) return sessionList;

        synchronized(mPendingSessionList) {
            for (ImsCallSessionImpl session : mPendingSessionList) {
                if (session.getInternalState() == state) {
                    sessionList.add(session);
                }
            }
        }

        synchronized(mCallList) {
            for (Map.Entry<String, ImsCallSessionImpl> e : mCallList.entrySet()) {
                final ImsCallSessionImpl session = e.getValue();
                if (session.getInternalState() == state) {
                    sessionList.add(session);
                }
            }
        }
        return sessionList;
    }

    /**
     * Gets a call session with give media id.
     * @param mediaId Media id of the session to be searched.
     * @return Call session with {@code mediaId}
     */
    public ImsCallSessionImpl findSessionByMediaId(int mediaId) {
        synchronized(mCallList) {
            for (Map.Entry<String, ImsCallSessionImpl> e : mCallList.entrySet()) {
                final ImsCallSessionImpl session = e.getValue();
                if (session.getMediaId() == mediaId) {
                    return session;
                }
            }
        }
        return null;
    }

    public void handleHandover(ImsQmiIF.Handover handover) {
        Log.i(this, "in handleHandover");
        HandoverInfo response = new ImsServiceClassTracker.HandoverInfo();
        response.mType = handover.getType();

        if (handover.hasSrcTech()) {
            response.mSrcTech = handover.getSrcTech();
        }

        if (handover.hasTargetTech()) {
            response.mTargetTech = handover.getTargetTech();
        }

        if (!handover.hasSrcTech() && !handover.hasTargetTech()) {
            handleSrvccStateChanged(response.mType);
            return;
        }
        if (handover.hasHoExtra() && handover.getHoExtra() != null) {
            ImsQmiIF.Extra extra = handover.getHoExtra();
            if (extra.hasType()) {
                response.mExtraType = extra.getType();
            }

            if (extra.hasExtraInfo()) {
                response.mExtraInfo = new byte[extra.getExtraInfo().size()];
                extra.getExtraInfo().copyTo(response.mExtraInfo, 0);
            }
        }
        if (handover.hasErrorCode()) {
            response.mErrorCode = handover.getErrorCode();
        }

        if (handover.hasErrorMessage()) {
            response.mErrorMessage = handover.getErrorMessage();
        }

        Log.i(this, "hoState: " + response.mType + " srcTech: " + response.mSrcTech +
                " tarTech: " + response.mTargetTech + " extraType: " + response.mExtraType +
                " extraInfo: " + response.mExtraInfo + " ErrorCode: " +
                response.mErrorCode + " errorMessage: " + response.mErrorMessage);

        boolean showHandoverToast = false;
        synchronized(mCallList) {
            for (Iterator<Map.Entry<String, ImsCallSessionImpl>> it = mCallList.entrySet()
                    .iterator(); it.hasNext();) {
                Map.Entry<String, ImsCallSessionImpl> e = it.next();
                ImsCallSessionImpl callSession = (ImsCallSessionImpl) e.getValue();
                if (callSession != null) {
                    callSession.handleHandover(response.mType, response.mSrcTech,
                            response.mTargetTech, response.mExtraType, response.mExtraInfo,
                            response.mErrorCode, response.mErrorMessage);
                    if (!showHandoverToast && callSession.isInCall()) {
                        showHandoverToast = true;
                    }
                } else {
                    Log.i(this, "No more call sessions found");
                }
            }
        }

        if (showHandoverToast &&
                response.mType == ImsQmiIF.COMPLETE_SUCCESS &&
                (response.mSrcTech == ImsQmiIF.RADIO_TECH_WIFI ||
                response.mSrcTech == ImsQmiIF.RADIO_TECH_IWLAN) &&
                response.mTargetTech == ImsQmiIF.RADIO_TECH_LTE) {
            Log.i(this, "Switching to LTE network for better quality");
        }
    }

    /**
     * Registers call list listener.
     * @param listener Listener to registered
     * @see ICallListListener
     */
    /* package */void addListener(ICallListListener listener) {
        if (listener == null) {
            throw new IllegalArgumentException("addListener error: listener is null.");
        }

        // Note: This will do linear search, O(N).
        // This is acceptable since arrays size is small.
        if (!mCallListListeners.contains(listener)) {
            mCallListListeners.add(listener);
        } else {
            Log.e(this,"addListener error: Duplicate listener, " + listener);
        }
    }

    /**
     * Unregisters call list listener. Note: Only {@code ImsServiceClass.MMTEL} is supported.
     * @param listener Listener to unregistered
     * @see ICallListListener
     */
    /* package */void removeListener(ICallListListener listener) {
        if (listener == null) {
            throw new IllegalArgumentException("removeListener error: listener is null.");
        }

        // Note: This will do linear search, O(N).
        // This is acceptable since arrays size is small.
        if (mCallListListeners.contains(listener)) {
            mCallListListeners.remove(listener);
        } else {
            Log.e(this,"removeListener error: Listener not found, " + listener);
        }
    }

    /**
     * Sets the incoming call pending intent
     * @param intent - PendingIntent
     */
    /* package */void setIncomingCallIntent(PendingIntent intent) {
        mIncomingCallIntent = intent;
    }

    // TODO Create CallList class and hide listeners, registration, notification in that class.
    private void notifyCallAdded(ImsCallSessionImpl session) {
        for (ICallListListener listener : mCallListListeners) {
            listener.onCallSessionAdded(session);
        }
    }

    private void addCall(Integer id, ImsCallSessionImpl session, boolean notify) {
        synchronized (mCallList) {
            mCallList.put(id.toString(), session);
        }
        if (notify) {
            notifyCallAdded(session);
        }
    }

    private void notifyCallRemoved(ImsCallSessionImpl session) {
        for (ICallListListener listener : mCallListListeners) {
            listener.onCallSessionRemoved(session);
        }
    }

    public void sendIncomingCallIntent(ImsCallSessionImpl session, int index, boolean isUnknown) {
        try {
            addCall(index, session, true);
            mIncomingCallIntent.send(mContext, ImsManager.INCOMING_CALL_RESULT_CODE,
                    createIncomingCallIntent(Integer.toString(index), false, isUnknown));
        } catch (PendingIntent.CanceledException e) {
            Log.e (this, "Incoming Call intent Canceled " + e);
        }
    }

    private void handleSrvccStateChanged(int state) {
        switch (state) {
            case ImsQmiIF.START:
                mSrvccStateFromIms = Call.SrvccState.STARTED;
                break;
            case ImsQmiIF.COMPLETE_SUCCESS:
                mSrvccStateFromIms = Call.SrvccState.COMPLETED;
                mNeedIgnoreCalls = true;
                break;
            case ImsQmiIF.COMPLETE_FAIL:
                mSrvccStateFromIms = Call.SrvccState.FAILED;
                break;
            case ImsQmiIF.CANCEL:
                mSrvccStateFromIms = Call.SrvccState.CANCELED;
                break;
        }
        Log.i (this, "handleSrvccStateChanged mSrvccStateFromIms = " + mSrvccStateFromIms +
                ", mNeedIgnoreCalls = " + mNeedIgnoreCalls);
    }

    /**
     * Update the WiFi quality indication to call sessions
     *
     * @param quality the quality value received via the indication
     */
    public void updateVoWiFiCallQuality(int quality) {
        ImsCallSessionImpl callSession = null;

        /* Update the existing call sessions with quality value */
        synchronized(mCallList) {
            for (Iterator<Map.Entry<String, ImsCallSessionImpl>> it =
                    mCallList.entrySet().iterator(); it.hasNext();) {
                Map.Entry<String, ImsCallSessionImpl> e = it.next();
                callSession = (ImsCallSessionImpl) e.getValue();
                callSession.updateVoWiFiCallQuality(quality);
            }
        }
    }

    public int getPhoneId() {
        return mServiceSub.getPhoneId();
    }

    @VisibleForTesting
    public Call.SrvccState getSrvccState() {
        return mSrvccStateFromIms;
    }

    @VisibleForTesting
    public boolean getIgnoreCalls() {
        return mNeedIgnoreCalls;
    }
}
