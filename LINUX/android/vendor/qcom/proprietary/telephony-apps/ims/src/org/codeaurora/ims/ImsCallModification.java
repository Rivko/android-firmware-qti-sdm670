/*
 * Copyright (c) 2015, 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution
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
 */

package org.codeaurora.ims;

import android.content.Context;
import android.os.AsyncResult;
import android.os.Handler;
import android.os.Message;
import android.telecom.VideoProfile;

import com.android.internal.annotations.VisibleForTesting;

import com.android.internal.telephony.CallManager;
import com.android.internal.telephony.CallStateException;
import com.android.ims.ImsCallProfile;
import com.qualcomm.ims.utils.Log;
import com.qualcomm.ims.vt.LowBatteryHandler;

import org.codeaurora.ims.QtiCallConstants;
import org.codeaurora.ims.utils.QtiImsExtUtils;

import vendor.qti.hardware.radio.ims.V1_0.RttMode;

import java.util.Map;

public class ImsCallModification {
    public ImsCallSessionImpl mImsCallSessionImpl;
    private ImsSenderRxr mCi;
    private Context mContext;
    private CallModify callModifyRequest = null;
    public int mIndex = -1; // index in ImsCallTracker.connections[], -1 if unassigned
    private int mPrevVideoCallType = CallDetails.CALL_TYPE_UNKNOWN;
    private boolean mAvpRetryAllowed = true;
    private int mAvpCallType = CallDetails.CALL_TYPE_UNKNOWN;
    static final int EVENT_AVP_UPGRADE_DONE = 5;
    static final int EVENT_MODIFY_CALL_INITIATE_DONE = 6;
    static final int EVENT_VIDEO_PAUSE_DONE = 7;
    static final int EVENT_MODIFY_CALL_CONFIRM_DONE = 8;
    static final int EVENT_VIDEO_PAUSE_RETRY = 9;

    private int mMultiTaskRetryCount = 0;
    private static final int MAX_MULTITASK_RETRIES = 1;
    static final int PAUSE_DELAY_MILLIS = 3 * 1000;
    Handler mHandler;

    private enum PauseState {
        NONE, PAUSE, RESUME
    };

    private PauseState mPendingVTMultitask = PauseState.NONE;

    // Indicates if pause request sent from local UE succeeded
    private boolean mIsLocallyPaused = false;

    // Indicates if user pressed home button and app is in background
    private boolean mIsVideoPauseRequested = false;

    public boolean isLocallyPaused() {
        return mIsLocallyPaused;
    }

    public ImsCallModification(ImsCallSessionImpl imsCallSessionImpl, Context context,
            ImsSenderRxr senderRxr) {
        Log.v(this, "ImsCallModification instance created imsCallSessionImpl=" +
                imsCallSessionImpl);
        mCi = senderRxr;
        mContext = context;
        mImsCallSessionImpl = imsCallSessionImpl;
        mHandler = new ImsCallModificationHandler();
    }

    @VisibleForTesting
    public Handler getHandler() {
        return mHandler;
    }

    private boolean isVTMultitaskRequest(int callType) {
        return callType == CallDetails.CALL_TYPE_VT_PAUSE
                || callType == CallDetails.CALL_TYPE_VT_RESUME;
    }

    private void triggerOrQueueVTMultitask(int callType) {
        Log.i(this, "triggerOrQueueVTMultitask callType= " + callType + " conn= " + this);
        boolean isPauseRequested = callType == CallDetails.CALL_TYPE_VT_PAUSE;
        // Update the pending pause flag
        mPendingVTMultitask = isPauseRequested ? PauseState.PAUSE : PauseState.RESUME;
        mIsVideoPauseRequested = isPauseRequested;
        if (callModifyRequest == null) {
            processPendingVTMultitask();
        }
    }

    private int toRilCallType(int callType) {
        int rilCallType = CallDetails.CALL_TYPE_UNKNOWN;
        if (callType == CallDetails.CALL_TYPE_VT_PAUSE) {
            rilCallType = CallDetails.CALL_TYPE_VT_NODIR;
        } else if (callType == CallDetails.CALL_TYPE_VT_RESUME) {
            rilCallType = mPrevVideoCallType;
        } else {
            Log.e(this, "toRilCallType unexpected calltype for VT multitask="
                    + callType);
        }
        return rilCallType;
    }

    private void createAndSendMultiTaskRequest(int rilCallType) {
        mHandler.removeMessages(EVENT_VIDEO_PAUSE_RETRY);
        Message message = mHandler.obtainMessage(EVENT_VIDEO_PAUSE_DONE);
        modifyCallInitiate(message, rilCallType, null);
    }

    private int pendingPauseStatetoRilCallType() {
        int rilCallType = CallDetails.CALL_TYPE_UNKNOWN;
        if (mPendingVTMultitask == PauseState.PAUSE) {
            rilCallType = CallDetails.CALL_TYPE_VT_NODIR;
        }
        else if (mPendingVTMultitask == PauseState.RESUME) {
            rilCallType = mPrevVideoCallType;
        }
        return rilCallType;
    }

    private boolean areCallTypesSame(int newCallType) {
        return (newCallType == mImsCallSessionImpl.getInternalCallType());
    }

    private boolean isIndexValid() {
        final int index = Integer.parseInt(mImsCallSessionImpl.getCallId());
        return (index >= 0);
    }

    private boolean validateOutgoingModifyConnectionType(Message msg, int newCallType) {
        Log.i(this, "validateOutgoingModifyConnectionType newCallType=" + newCallType);

        final boolean modifyToCurrCallType = areCallTypesSame(newCallType);
        final boolean isIndexValid = isIndexValid();
        final boolean isLowBattery = LowBatteryHandler.getInstance().isLowBattery();

        boolean isValid = isIndexValid && !modifyToCurrCallType;
        if (!ImsCallUtils.isCarrierOneSupported()) {
            isValid &= !isLowBattery;
        }

        Log.i(this, "validateOutgoingModifyConnectionType modifyToCurrCallType = " +
                modifyToCurrCallType + " isIndexValid = " + isIndexValid +
                " isLowBattery = " + isLowBattery);
        if (msg == null) {return isValid;}

        if (!isValid) {
            Exception exception = null;
            if (isLowBattery) {
                exception = new ImsRilException(
                        QtiCallConstants.SESSION_MODIFY_REQUEST_FAILED_LOW_BATTERY,
                        "Low Battery");
            } else if (!modifyToCurrCallType) {
                exception = new Exception("Can't modify call.");
            }
            msg.obj = AsyncResult.forMessage(msg, null, exception);
            msg.sendToTarget();
        }
        return isValid;
    }

    private boolean validateIncomingModifyConnectionType(int newCallType) {
        Log.i(this, "validateIncomingModifyConnectionType newCallType = " + newCallType);

        final boolean modifyToCurrCallType = areCallTypesSame(newCallType);
        final boolean isIndexValid = isIndexValid();
        // For RJIL, incoming modify requests should be allowed under low battery scenarios
        boolean isLowBattery = false;
        /* For SKT operator, Do not honour upgrade requests received
           when UE is under low battery */
        if (!QtiImsExtUtils.allowVideoCallsInLowBattery(mImsCallSessionImpl.getPhoneId(),
                mContext)) {
            isLowBattery = LowBatteryHandler.getInstance().isLowBattery();
        }

        //For CMCC, ignore the modify event during MO ALTERTING stage(video CRBT come).
        boolean isVideoCRBT = false;
        if (mImsCallSessionImpl.getInternalState() == DriverCallIms.State.ALERTING
              && newCallType == CallDetails.CALL_TYPE_VT_RX
              && QtiImsExtUtils.isCarrierConfigEnabled(mImsCallSessionImpl.getPhoneId(),
              mContext, "config_enable_video_crbt")) {
            Log.i(this, "validateIncomingModifyConnectionType isVideoCRBT = true");
            isVideoCRBT = true;
        }

        final boolean isValid = !isLowBattery && isIndexValid && !modifyToCurrCallType
            && !isVideoCRBT;

        Log.i(this, "validateIncomingModifyConnectionType modifyToCurrCallType = " +
                modifyToCurrCallType + " isIndexValid = " + isIndexValid +
                " isLowBattery = " + isLowBattery);
        return isValid;
    }

    public static boolean hasError(CallModify callModify) {
        return (callModify != null && callModify.error());
    }

    // UNSOL_MODIFY_CALL
    public void onReceivedModifyCall(CallModify callModify) {
        Log.i(this, "onReceivedCallModify(" + callModify + ")");

        boolean ret = false;
        boolean rttChanged = false;
        // Is this a new modify call request or an error notification for
        // the previous modify call request?
        if (callModify.error()) {
            if (this.callModifyRequest != null
                    && this.callModifyRequest.call_index == callModify.call_index
                    && this.callModifyRequest.call_details.call_type
                        == callModify.call_details.call_type
                    && this.callModifyRequest.call_details.call_domain
                        == callModify.call_details.call_domain
                    && !this.callModifyRequest.error()) {
                // Update the previous request.
                this.callModifyRequest.error = callModify.error;
                ret = true;
            } else {
                Log.e(this, "onReceivedModifyCall: Call Modify request not found."
                        + "Dropping the Modify Call Request Failed. Cached Request: "
                        + this.callModifyRequest + ", Received Request:" + callModify);
                ret = false;
            }
        } else {
            // Check if RTT is set in CallDetails
            rttChanged = isUpgradeRequestForRtt(callModify.call_details);

            // This a new modify call request.
            ret = validateIncomingModifyConnectionType(callModify.call_details.call_type);

            if (this.callModifyRequest == null) {
                this.callModifyRequest = callModify;
            } else {
                Log.e(this, "videocall onReceivedModifyCall: not notifying user about"
                        + " incoming modify call request as there is pending callModifyRequest="
                        + callModifyRequest);
                ret = false;
            }

            boolean callModified = ret || rttChanged;
            if (!callModified) {
                // reject/clear modify request if call cannot be modifed to request callType
                rejectConnectionTypeChange(null);
            }
        }

        final boolean isLowBattery = LowBatteryHandler.getInstance().isLowBattery();
        if (isLowBattery && !ImsCallUtils.isCarrierOneSupported()) {
            callModify.error = QtiCallConstants.SESSION_MODIFY_REQUEST_FAILED_LOW_BATTERY;
        }

        if (rttChanged) {
            mImsCallSessionImpl.notifyRttModifyRequest(callModify.call_details);
            clearPendingModify();
        } else {
            Log.d(this, "RTT: onReceivedModifyCall: No RTT modification");
        }

        if (ret || isLowBattery) {
            /*
             * notify only in below cases:
             * 1. when call can be modified to the requested callType
             * 2. to diplay a toast message as a result of modify request time-out
             */
            mImsCallSessionImpl.notifyUnsolCallModify(callModify);
        } else {
            Log.d(this, "onReceivedModifyCall: No notify for CallModify ");
        }

        if (hasError(callModify)) {
            clearPendingModify();
        }
    }

    private boolean isOldAndNewPauseRequestSame() {
        boolean ret = false;
        if (callModifyRequest != null) {
            Log.e(this, "isOldAndNewPauseRequestSame Unexpectedly callModifyRequest:" +
                    callModifyRequest);
            return ret;
        }
        ret = (mIsLocallyPaused && mPendingVTMultitask == PauseState.PAUSE)
                || (!mIsLocallyPaused && mPendingVTMultitask == PauseState.RESUME);
        Log.i(this, "isOldAndNewPauseRequestSame " + ret);
        return ret;
    }

    private void clearPendingVTMultiTask() {
        Log.i(this, "clearPendingVTMultiTask imsconn=" + this);
        mPendingVTMultitask = PauseState.NONE;
    }

    private boolean shouldRetryVideoPause() {
        return (mMultiTaskRetryCount <= MAX_MULTITASK_RETRIES);
    }

    private void processPendingVTMultitask() {
        Log.i(this, "processPendingVTMultitask mPendingVTMultitask=" + mPendingVTMultitask);
        if (isOldAndNewPauseRequestSame()) {
            Log.i(this, "Old and new Pause Request is Same so clearing Pending VT multitask");
            clearPendingVTMultiTask();
        } else if (mPendingVTMultitask != PauseState.NONE) {
            if (callModifyRequest == null) {
                createAndSendMultiTaskRequest(pendingPauseStatetoRilCallType());
                clearPendingVTMultiTask();
            } else {
                Log.e(this, "processPendingVTMultitask callModifyRequest not null");
            }

        }
    }

    // Clears the callModifyRequest object
    private void clearPendingModify() {
        Log.i(this, "clearPendingModify imsconn=" + this);
        callModifyRequest = null;
    }

    @VisibleForTesting
    public CallModify getPendingModify() {
        return callModifyRequest;
    }

    @VisibleForTesting
    public void setPendingModify(CallModify callModify) {
        callModifyRequest = callModify;
    }

    public boolean isAvpRetryAllowed() {
        return mAvpRetryAllowed;
    }

    public void update(DriverCallIms dc) {
        Log.i(this, "update dc=" + dc);
        int videoPauseState = CallDetails.VIDEO_PAUSE_STATE_RESUMED;
        // int callType = currentDc.callDetails.call_type;
        // CallDetails callDetails = new CallDetails(callType,
        // CallDetails.CALL_DOMAIN_AUTOMATIC,null);
        if (ImsCallUtils.isActive(dc) &&
                ImsCallUtils.isVideoCallTypeWithDir(dc.callDetails.call_type)) {
            // Entered video call with peer so do not allow AVP retry for future
            mAvpRetryAllowed = false;
        }
        if (!ImsCallUtils.isVideoCall(dc.callDetails.call_type)) {
            Log.i(this, "videocall: update: Not a videocall CASE");
            clearPendingVTMultiTask();
            mMultiTaskRetryCount = 0;
            mHandler.removeMessages(EVENT_VIDEO_PAUSE_RETRY);
        }
        boolean isAvpRetryDialingCase = ImsCallUtils.isAvpRetryDialing(mImsCallSessionImpl, dc);
        if (isAvpRetryDialingCase) {
            // Update AVP call type with current call type for dialed video call
            // mAvpCallType = callDetails.call_type;
            mAvpCallType = mImsCallSessionImpl.getInternalCallType();
        }
        boolean isAvpRetryUpgradeCase = ImsCallUtils.isAvpRetryUpgrade(mImsCallSessionImpl, dc);
        if (isAvpRetryDialingCase || isAvpRetryUpgradeCase) {
            Log.i(this, "videocall AVP RETRY CASE dc= " + dc + " conn= " + this);
            retryAvpUpgrade(mImsCallSessionImpl.getInternalState() == DriverCallIms.State.ACTIVE);
        } else if (ImsCallUtils.isVideoPaused(mImsCallSessionImpl, dc)) {
            // Display toast Video Paused
            Log.i(this, "videocall Video Paused CASE");
        } else if (ImsCallUtils.isVideoResumed(mImsCallSessionImpl, dc)) {
            // Display toast Video Resumed
            Log.i(this, "videocall Video Resumed CASE");
        }

        updatePreviousVTCallType();

        CallDetails newDetails = new CallDetails(dc.callDetails);

        /*
         * //TODO confirm and remove - not necessary for L if (!callDetails.equals(dc.callDetails))
         * { Log.i(LOG_TAG, "[Connection " + mImsCallSessionImpl.getCallId() +
         * "] Updating call Details to " + newDetails); callDetails = newDetails; // Update call
         * type to notify multitasking state to UI callDetails.setVideoPauseState(videoPauseState);
         * // If video is paused then update UI call type with previous calltype if (videoPauseState
         * == CallDetails.VIDEO_PAUSE_STATE_PAUSED) { log("Updating UI CallType from " +
         * callDetails.call_type + " to mPrevVideoCallType=" + mPrevVideoCallType);
         * callDetails.call_type = mPrevVideoCallType; } }
         */
        // Reset the mIsLocallyPaused flag if call is not active videocall
        if (!ImsCallUtils.canVideoPause(mImsCallSessionImpl)) {
            mIsLocallyPaused = false;
            mIsVideoPauseRequested = false;
        }
    }

    private void notifyVideoPaused(int videoState) {
        VideoProfile vcp = new VideoProfile(videoState, VideoProfile.QUALITY_DEFAULT);
        mImsCallSessionImpl.getImsVideoCallProviderImpl().receiveSessionModifyRequest(vcp);
    }

    private void retryAvpUpgrade(boolean shouldNotifyUser) {
        Log.i(this, "retryAvpUpgrade: AVPF failed so retrying using AVP. mAvpCallType=" +
                mAvpCallType + " shouldNotifyUser=" + shouldNotifyUser);
        // try {
        mIndex = Integer.parseInt(mImsCallSessionImpl.getCallId());
        Message msg = mHandler.obtainMessage(EVENT_AVP_UPGRADE_DONE, shouldNotifyUser);
        modifyCallInitiate(msg, mAvpCallType, null);
        // } catch (CallStateException e) {
        // loge("retryAvpUpgrade CallStateException " + e);
        // }
    }

    private void updatePreviousVTCallType() {
        int callType = mImsCallSessionImpl.getInternalCallType();
        if (callType == CallDetails.CALL_TYPE_VT
                || callType == CallDetails.CALL_TYPE_VT_TX
                || callType == CallDetails.CALL_TYPE_VT_RX) {
            mPrevVideoCallType = callType;
            Log.i(this, "Updating mPrevVideoCallType to " + mPrevVideoCallType);
        }
    }

    // MODIFY_CALL_INITIATE
    public void changeConnectionType(Message msg, int newCallType, Map<String, String> newExtras)
    {
        Log.i(this, "changeConnectionType " + " newCallType=" + newCallType + " newExtras= "
                + newExtras);
        // mImsCallProfile = mImsCallSessionImpl.getCallId().getCallProfile();
        mIndex = Integer.parseInt(mImsCallSessionImpl.getCallId());
        if (isVTMultitaskRequest(newCallType)) {
            // Video pause/resume request
            triggerOrQueueVTMultitask(newCallType);
        } else {
            // Regular upgrade/downgrade request
            if (isAvpRetryAllowed() && ImsCallUtils.isVideoCallTypeWithDir(newCallType)) {
                mAvpCallType = newCallType;
            }

            Message newMsg = mHandler.obtainMessage(EVENT_MODIFY_CALL_INITIATE_DONE, msg);
            if (callModifyRequest == null) {
                if (validateOutgoingModifyConnectionType(newMsg, newCallType)) {
                    modifyCallInitiate(newMsg, newCallType, newExtras);
                }
            } else {
                Log.e(this,
                        "videocall changeConnectionType: not invoking modifyCallInitiate "
                                + "as there is pending callModifyRequest="
                                + callModifyRequest);

                String errorStr = "Pending callModifyRequest so not sending modify request down";
                sendErrorResponse(msg, errorStr);
            }
        }
    }

    private void modifyCallInitiate(Message newMsg, int newCallType, Map<String, String> newExtras)
    {
        if (!ImsCallUtils.isValidRilModifyCallType(newCallType)) {
            Log.e(this, "modifyCallInitiate not a Valid RilCallType" + newCallType);
            return;
        }

        /*
         * CallDetails callDetails = new CallDetails(newCallType, getCallDetails().call_domain,
         * CallDetails.getExtrasFromMap(newExtras));
         */
        CallDetails callDetails = new CallDetails(newCallType, mImsCallSessionImpl.getCallDomain(),
                CallDetails.getExtrasFromMap(newExtras));
        ImsCallProfile callProfile = mImsCallSessionImpl.getCallProfile();
        if (callProfile != null) {
            callDetails.setRttMode(callProfile.mMediaProfile.mRttMode);
        }
        CallModify callModify = new CallModify(callDetails, mIndex);
        // Store the outgoing modify call request in the connection
        if (callModifyRequest != null) {
            Log.i(this, "Overwriting callModifyRequest: " + callModifyRequest + " with callModify:"
                    + callModify);
        }
        callModifyRequest = callModify;
        mCi.modifyCallInitiate(newMsg, callModify);
    }

    private boolean isValidCallModifyConfirmRequest(int callType) {
        boolean allowed = false;
        if (callModifyRequest == null) {
            Log.e(this, "callModifyRequest is null");
            return false;
        }
        //Accept with different call types is supported only for VoLTE to VT upgrade
        if (mImsCallSessionImpl.getInternalCallType() == CallDetails.CALL_TYPE_VOICE &&
                callModifyRequest.call_details.call_type == CallDetails.CALL_TYPE_VT &&
                (callType == CallDetails.CALL_TYPE_VT_RX
                        || callType == CallDetails.CALL_TYPE_VT_TX
                        || callType == CallDetails.CALL_TYPE_VT
                        || callType == CallDetails.CALL_TYPE_VOICE)) {
            allowed = true;
        } else if (callType == callModifyRequest.call_details.call_type) {
            allowed = true;
        }
        return allowed;
    }

    /**
     * Approve a request to change the call type. Optionally, provide new extra values.
     * @param newExtras
     * @throws CallStateException
     */
    public void acceptConnectionTypeChange(int callType, Map<String, String> newExtras, Message msg)
    {
        Log.i(this, "Confirming call type change request: " + callModifyRequest);

        if (!isValidCallModifyConfirmRequest(callType)) {
            Log.e(this, "acceptConnectionTypeChange: MODIFY_CALL_CONFIRM with invalid calltype "
                    + callType);
            rejectConnectionTypeChange(msg);
            return;
        }

        if (callModifyRequest != null) {
            callModifyRequest.call_details.call_type = callType;
            if (newExtras != null) {
                callModifyRequest.call_details.setExtrasFromMap(newExtras);
            }
            Message newMsg = mHandler.obtainMessage(EVENT_MODIFY_CALL_CONFIRM_DONE, msg);
            mCi.modifyCallConfirm(newMsg, callModifyRequest);
        }
    }

    private void sendErrorResponse(Message msg, String errStr) {
        RuntimeException exception = new RuntimeException(errStr);
        if (msg != null) {
            msg.obj = AsyncResult.forMessage(msg, null, exception);
            msg.sendToTarget();
        }
    }

    /**
     * Reject a previously received request to change the call type.
     * @throws CallStateException
     */
    public void rejectConnectionTypeChange(Message msg) {
        if (callModifyRequest == null) {
            Log.e(this, "rejectConnectionTypeChange callModifyRequest is null");
            sendErrorResponse(msg, "no existing modify request");
            return;
        } else if (callModifyRequest.error()) {
            Log.i(this, "rejectConnectionTypeChange callModifyRequest timed out.");
            clearPendingModify();
            sendErrorResponse(msg, "modify request timed out");
            return; // Don't notify lower layers. They are aware of this.
        }
        CallDetails callDetails = new CallDetails(mImsCallSessionImpl.getInternalCallType(),
                mImsCallSessionImpl.getCallDomain(),
                null);
        CallModify callModify = new CallModify();
        callModify.call_index  = Integer.parseInt(mImsCallSessionImpl.getCallId());
        callModify.call_details = new CallDetails(callDetails);

        Log.i(this, "Rejecting Change request: " + callModifyRequest + " keep as " + callModify);

        Message newMsg = mHandler.obtainMessage(EVENT_MODIFY_CALL_CONFIRM_DONE, msg);
        mCi.modifyCallConfirm(newMsg, callModify);
    }

    class ImsCallModificationHandler extends Handler {
        public void handleMessage(Message msg) {
            AsyncResult ar;
            switch (msg.what) {
                case EVENT_MODIFY_CALL_INITIATE_DONE:
                case EVENT_MODIFY_CALL_CONFIRM_DONE:
                    onModifyCallDone(msg);
                    break;
                case EVENT_VIDEO_PAUSE_DONE:
                    onVideoPauseDone(msg);
                    break;
                case EVENT_VIDEO_PAUSE_RETRY:
                    onVideoPauseRetry();
                    break;
                case EVENT_AVP_UPGRADE_DONE:
                    onAvpRetry(msg);
                    break;
            }
        }

        private void onModifyCallDone(Message msg) {
            Log.i(this, "onModifyCallDone for event: " +msg.what);
            AsyncResult ar = (AsyncResult) msg.obj;
            if (ar != null && ar.exception != null) {
                Log.e(this, "videocall error during modifyCall");
                // Context needs to come from ImsServiceSub to ImsCallSessionImpl
                // Toast.makeText(mContext, MODIFY_CALL_FAILED_MESSAGE, Toast.LENGTH_SHORT).show();
            }
            Message onComplete = (Message) ar.userObj;
            if (onComplete != null) {
                AsyncResult.forMessage(onComplete, ar.result, ar.exception);
                onComplete.sendToTarget();
            }
            clearPendingModify();
            processPendingVTMultitask();
        }

        private void onVideoPauseDone(Message msg) {
            AsyncResult ar;
            Log.i(this, "EVENT_VIDEO_PAUSE_DONE received");
            ar = (AsyncResult) msg.obj;
            if (ar != null) {
                if (ar.exception == null) {
                    // Success means VT Pause or Resume request succeeded.
                    // Update locally paused flag
                    if (callModifyRequest != null) {
                        mIsLocallyPaused = callModifyRequest.call_details.call_type
                                == CallDetails.CALL_TYPE_VT_NODIR;
                    }
                    clearPendingModify();
                    clearMultiTaskRetryCount();
                    processPendingVTMultitask();

                } else {
                    // Pause request failed, retry MAX_MULTITASK_RETRIES times
                    if (shouldRetryVideoPause()) {
                        Log.e(this, "Error during video pause so retry");
                        mHandler.sendMessageDelayed(mHandler.obtainMessage(EVENT_VIDEO_PAUSE_RETRY),
                                PAUSE_DELAY_MILLIS);
                        mMultiTaskRetryCount++;
                    } else {
                        Log.i(this, "Video Pause retry limit reached.");
                        clearMultiTaskRetryCount();
                        clearPendingModify();
                        processPendingVTMultitask();
                    }
                }
            } else {
                Log.e(this, "Error EVENT_VIDEO_PAUSE_DONE ar is null");
            }
        }

        private void onVideoPauseRetry() {
            Log.i(this, "EVENT_VIDEO_PAUSE_RETRY received mMultiTaskRetryCount=" +
                    mMultiTaskRetryCount);
            if (mPendingVTMultitask == PauseState.NONE) {
                if (callModifyRequest != null) {
                    createAndSendMultiTaskRequest(callModifyRequest.call_details.call_type);
                }
            } else {
                Log.i(this, "User pressed home/resume during retry sending new multitask request");
                clearPendingModify();
                clearMultiTaskRetryCount();
                processPendingVTMultitask();
            }
        }

        private void onAvpRetry(Message msg) {
            Log.i(this, "EVENT_AVP_UPGRADE received");
            AsyncResult ar = (AsyncResult) msg.obj;
            if (ar != null && ar.exception != null) {
                if (ar.userObj instanceof Boolean) {
                    boolean shouldNotifyUser = (Boolean) ar.userObj;
                    if (shouldNotifyUser) {
                        Log.e(this, "AVP Retry error when Voice call was upgraded to video call");
                        // owner.phone.notifyAvpUpgradeFailure(((AsyncResult)
                        // msg.obj).exception.toString());

                        // Context can be obtained by changing ImsCallSessionImpl constructor
                        // Toast.makeText(mContext, MODIFY_CALL_FAILED_MESSAGE,
                        // Toast.LENGTH_SHORT).show();
                    } else {
                        Log.e(this, "AVP Retry error when Video call was dialed");
                    }
                }
            }
            clearPendingModify();
            processPendingVTMultitask();
        }

        private int clearMultiTaskRetryCount() {
            Log.i(this, "Clearing MultiTaskRetryCount from " + mMultiTaskRetryCount + " to 0");
            return mMultiTaskRetryCount = 0;
        }
    }

    public boolean isVideoPauseRequested() {
        return mIsVideoPauseRequested;
    }

    // Check if Rtt is set in CallDetails
    private boolean isUpgradeRequestForRtt(CallDetails callDetails) {
        ImsCallProfile callProfile = mImsCallSessionImpl.getCallProfile();
        if (callDetails == null || callProfile.mMediaProfile.isRttCall()) {
            return false;
        }

        Log.i(this, "RTT: isUpgradeRequestForRtt rttMode = " + callDetails.rttMode);
        return ((callDetails.rttMode == RttMode.RTT_MODE_FULL) && isIndexValid());
    }

    public void close() {
        mHandler.removeMessages(EVENT_VIDEO_PAUSE_RETRY);
        if (callModifyRequest != null) {
            callModifyRequest.error = CallModify.E_CANCELLED;
            Log.i(this, "Cancel pending call modification before being closed");
            mImsCallSessionImpl.notifyUnsolCallModify(callModifyRequest);
            clearPendingModify();
        }
    }

}
