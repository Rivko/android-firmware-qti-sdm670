/*
 * Copyright (c) 2015, 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package org.codeaurora.ims;

import android.os.AsyncResult;
import android.os.SystemProperties;
import android.telecom.VideoProfile;
import android.telecom.Connection.VideoProvider;

import com.android.ims.ImsCallProfile;
import com.android.ims.ImsConfig;
import com.android.ims.ImsReasonInfo;
import com.android.internal.telephony.Call;
import com.android.internal.telephony.CarrierAppUtils;
import com.android.internal.telephony.DriverCall;
import com.qualcomm.ims.utils.Log;

import org.codeaurora.ims.ImsRilException;
import org.codeaurora.ims.QtiCallConstants;

public class ImsCallUtils {
    private static String TAG = "ImsCallUtils";
    private static final int MIN_VIDEO_CALL_PHONE_NUMBER_LENGTH = 7;
    public static final int CONFIG_TYPE_INVALID = -1;
    public static final int CONFIG_TYPE_INT = 1;
    public static final int CONFIG_TYPE_STRING = 2;

    public static final int SESSION_MERGE_STARTED = 1;
    public static final int SESSION_MERGE_COMPLETED = 2;
    public static final int SESSION_MERGE_FAILED = 3;

    public static final int QTI_CONFIG_START =
            ImsConfig.ConfigConstants.PROVISIONED_CONFIG_END + 1;

    public static final int QTI_CONFIG_SMS_APP = QTI_CONFIG_START;
    public static final int QTI_CONFIG_VVM_APP = QTI_CONFIG_START + 1;
    public static final int QTI_CONFIG_END = QTI_CONFIG_VVM_APP; // The last value.

    /** Checks if a call type is any valid video call type with or without direction
     */
    public static boolean isVideoCall(int callType) {
        return callType == CallDetails.CALL_TYPE_VT
                || callType == CallDetails.CALL_TYPE_VT_TX
                || callType == CallDetails.CALL_TYPE_VT_RX
                || callType == CallDetails.CALL_TYPE_VT_PAUSE
                || callType == CallDetails.CALL_TYPE_VT_RESUME
                || callType == CallDetails.CALL_TYPE_VT_NODIR;
    }

    public static boolean isVoiceCall(int callType) {
        return callType == CallDetails.CALL_TYPE_VOICE;
    }

    /** Check if call type is valid for lower layers
     */
    public static boolean isValidRilModifyCallType(int callType){
        return callType == CallDetails.CALL_TYPE_VT
                || callType == CallDetails.CALL_TYPE_VT_TX
                || callType == CallDetails.CALL_TYPE_VT_RX
                || callType == CallDetails.CALL_TYPE_VOICE
                || callType == CallDetails.CALL_TYPE_VT_NODIR;
    }

    /** Checks if videocall state transitioned to Video Paused state
     * @param conn - Current connection object
     * @param dc - Latest DriverCallIms instance
     */
    public static boolean isVideoPaused(ImsCallSessionImpl conn, DriverCallIms dc) {
        int currCallType = conn.getInternalCallType();
        DriverCallIms.State currCallState = conn.getInternalState();
        int nextCallType = dc.callDetails.call_type;
        DriverCallIms.State nextCallState = dc.state;

        boolean isHoldingPaused = isVideoCall(currCallType)
                && (currCallState == DriverCallIms.State.HOLDING)
                && isVideoCallTypeWithoutDir(nextCallType)
                && (nextCallState == DriverCallIms.State.ACTIVE);
        boolean isActivePaused =  (isVideoCall(currCallType)
                && (currCallState == DriverCallIms.State.ACTIVE)
                && isVideoCallTypeWithoutDir(nextCallType)
                && (nextCallState == DriverCallIms.State.ACTIVE));
        return isHoldingPaused || isActivePaused;
    }

    /** Detects active video call
     */
    public static boolean canVideoPause(ImsCallSessionImpl conn) {
        return isVideoCall(conn.getInternalCallType()) && conn.isCallActive();
    }

    /** Checks if videocall state transitioned to Video Resumed state
     * @param conn - Current connection object
     * @param dc - Latest DriverCallIms instance
     */
    public static boolean isVideoResumed(ImsCallSessionImpl conn, DriverCallIms dc) {
        int currCallType = conn.getInternalCallType();
        DriverCallIms.State currCallState = conn.getInternalState();
        int nextCallType = dc.callDetails.call_type;
        DriverCallIms.State nextCallState = dc.state;

        return (isVideoCallTypeWithoutDir(currCallType)
                && (currCallState == DriverCallIms.State.ACTIVE)
                && isVideoCall(nextCallType)
                && (nextCallState == DriverCallIms.State.ACTIVE));
    }

    /** Checks if AVP Retry needs to be triggered during dialing
     * @param conn - Current connection object
     * @param dc - Latest DriverCallIms instance
     */
    public static boolean isAvpRetryDialing(ImsCallSessionImpl conn, DriverCallIms dc) {
        int currCallType = conn.getInternalCallType();
        DriverCallIms.State currCallState = conn.getInternalState();
        int nextCallType = dc.callDetails.call_type;
        DriverCallIms.State nextCallState = dc.state;

        boolean dialingAvpRetry = (isVideoCall(currCallType)
                    && (currCallState == DriverCallIms.State.DIALING || currCallState == DriverCallIms.State.ALERTING)
                    && isVideoCallTypeWithoutDir(nextCallType)
                    && nextCallState == DriverCallIms.State.ACTIVE);
        return (conn.getImsCallModification().isAvpRetryAllowed() && dialingAvpRetry);
    }

    /** Checks if AVP Retry needs to be triggered during upgrade
     * @param conn - Current connection object
     * @param dc - Latest DriverCallIms instance
     */
    public static boolean isAvpRetryUpgrade(ImsCallSessionImpl conn, DriverCallIms dc) {
        int currCallType = conn.getInternalCallType();
        DriverCallIms.State currCallState = conn.getInternalState();
        int nextCallType = dc.callDetails.call_type;
        DriverCallIms.State nextCallState = dc.state;

        boolean upgradeAvpRetry = (currCallType == CallDetails.CALL_TYPE_VOICE
                    && currCallState == DriverCallIms.State.ACTIVE
                    && isVideoCallTypeWithoutDir(nextCallType)
                    && nextCallState == DriverCallIms.State.ACTIVE);
        return (conn.getImsCallModification().isAvpRetryAllowed() && upgradeAvpRetry);
    }

    /** Checks if a call type is video call type with direction
     * @param callType
     */
    public static boolean isVideoCallTypeWithDir(int callType) {
        return callType == CallDetails.CALL_TYPE_VT
                || callType == CallDetails.CALL_TYPE_VT_RX
                || callType == CallDetails.CALL_TYPE_VT_TX;
    }

    /** Checks if a incoming call is video call
     * @param callSession - Current connection object
     */
    public static boolean isIncomingVideoCall(ImsCallSessionImpl callSession) {
        return (isVideoCall(callSession.getInternalCallType()) &&
                (callSession.getInternalState() == DriverCallIms.State.INCOMING ||
                callSession.getInternalState() == DriverCallIms.State.WAITING));
    }

   /** Checks if a call is incoming call
     * @param callSession - Current connection object
     */
    public static boolean isIncomingCall(ImsCallSessionImpl callSession) {
        return (callSession.getInternalState() == DriverCallIms.State.INCOMING ||
                callSession.getInternalState() == DriverCallIms.State.WAITING);
    }

    /** Checks if a call is not CS video call
     * @param details - Current call details
     */
    public static boolean isNotCsVideoCall(CallDetails details) {
        return isVideoCall(details.call_type) &&
                (details.call_domain != CallDetails.CALL_DOMAIN_CS);
    }

    /** Checks if a call type is video call type without direction
     * @param callType
     */
    public static boolean isVideoCallTypeWithoutDir(int callType) {
        return callType == CallDetails.CALL_TYPE_VT_NODIR;
    }

    public static int convertVideoStateToCallType(int videoState) {
        int callType = CallDetails.CALL_TYPE_VOICE;
        switch (videoState) {
            case VideoProfile.STATE_AUDIO_ONLY:
                callType = CallDetails.CALL_TYPE_VOICE;
                break;
            case VideoProfile.STATE_RX_ENABLED:
                callType = CallDetails.CALL_TYPE_VT_RX;
                break;
            case VideoProfile.STATE_TX_ENABLED:
                callType = CallDetails.CALL_TYPE_VT_TX;
                break;
            case VideoProfile.STATE_BIDIRECTIONAL:
                callType = CallDetails.CALL_TYPE_VT;
                break;
            case VideoProfile.STATE_PAUSED:
                callType = CallDetails.CALL_TYPE_VT_NODIR;
                break;
        }
        return callType;
    }

    public static int convertCallTypeToVideoState(int callType) {
        int videoState = VideoProfile.STATE_AUDIO_ONLY;
        switch (callType) {
            case CallDetails.CALL_TYPE_VOICE:
                videoState = VideoProfile.STATE_AUDIO_ONLY;
                break;
            case CallDetails.CALL_TYPE_VT_RX:
                videoState = VideoProfile.STATE_RX_ENABLED;
                break;
            case CallDetails.CALL_TYPE_VT_TX:
                videoState = VideoProfile.STATE_TX_ENABLED;
                break;
            case CallDetails.CALL_TYPE_VT:
                videoState = VideoProfile.STATE_BIDIRECTIONAL;
                break;
            case CallDetails.CALL_TYPE_VT_PAUSE:
            case CallDetails.CALL_TYPE_VT_NODIR:
                videoState = VideoProfile.STATE_PAUSED;
                break;
        }
        return videoState;
    }

    public static int convertToInternalCallType(int imsCallProfileCallType) {
        int internalCallType = CallDetails.CALL_TYPE_UNKNOWN;
        switch (imsCallProfileCallType) {
            case ImsCallProfile.CALL_TYPE_VOICE:
            case ImsCallProfile.CALL_TYPE_VOICE_N_VIDEO:
                internalCallType = CallDetails.CALL_TYPE_VOICE;
                break;
            case ImsCallProfile.CALL_TYPE_VT:
            case ImsCallProfile.CALL_TYPE_VIDEO_N_VOICE:
                internalCallType = CallDetails.CALL_TYPE_VT;
                break;
            case ImsCallProfile.CALL_TYPE_VT_NODIR:
                internalCallType = CallDetails.CALL_TYPE_VT_NODIR;
                break;
            case ImsCallProfile.CALL_TYPE_VT_TX:
                internalCallType = CallDetails.CALL_TYPE_VT_TX;
                break;
            case ImsCallProfile.CALL_TYPE_VS_RX:
                internalCallType = CallDetails.CALL_TYPE_VT_RX;
                break;
            default:
                Log.e(TAG, "convertToInternalCallType invalid calltype " + imsCallProfileCallType);
                break;
        }
        return internalCallType;
    }

    public static VideoProfile convertToVideoProfile(int callType, int callQuality) {
        VideoProfile videoCallProfile = new VideoProfile(
                convertCallTypeToVideoState(callType), callQuality);
        // TODO in future - add quality to CallDetails
        return videoCallProfile;
    }

    public static int convertImsErrorToUiError(int error) {
        if (error == CallModify.E_CANCELLED) {
            return VideoProvider.SESSION_MODIFY_REQUEST_TIMED_OUT;
        } else if (error == CallModify.E_SUCCESS || error == CallModify.E_UNUSED) {
            return VideoProvider.SESSION_MODIFY_REQUEST_SUCCESS;
        } else if (error == QtiCallConstants.SESSION_MODIFY_REQUEST_FAILED_LOW_BATTERY) {
            return QtiCallConstants.SESSION_MODIFY_REQUEST_FAILED_LOW_BATTERY;
        } else {
            return VideoProvider.SESSION_MODIFY_REQUEST_FAIL;
        }
    }

    /**
     * Utility method to detect if call type has changed
     * @return boolean - true if both driver calls are not null and call type has changed
     */
    public static boolean hasCallTypeChanged(DriverCallIms dc, DriverCallIms dcUpdate) {
        return (dc != null && dcUpdate != null &&
                dc.callDetails.call_type != dcUpdate.callDetails.call_type);
    }

    /**
     * Utility method to get modify call error code from exception
     * @return int - session modify request errors. Valid values are
     *         {@link VideoProvider#SESSION_MODIFY_REQUEST_FAIL},
     *         {@link VideoProvider#SESSION_MODIFY_REQUEST_INVALID},
     *         {@link VideoProvider#SESSION_MODIFY_REQUEST_TIMED_OUT},
     *         {@link VideoProvider#SESSION_MODIFY_REQUEST_REJECTED_BY_REMOTE},
     *         {@link QtiCallConstants#SESSION_MODIFY_REQUEST_FAILED_LOW_BATTERY}
     *
     */
    public static int getUiErrorCode(Throwable ex) {
        int status = VideoProvider.SESSION_MODIFY_REQUEST_FAIL;
        if (ex instanceof ImsRilException) {
            ImsRilException imsRilException = (ImsRilException) ex;
            status = getUiErrorCode(imsRilException.getErrorCode());
        }
        return status;
    }

    public static int getUiErrorCode(int imsErrorCode) {
        int status = VideoProvider.SESSION_MODIFY_REQUEST_SUCCESS;
        switch (imsErrorCode) {
            case ImsQmiIF.E_SUCCESS:
            case ImsQmiIF.E_UNUSED:
                status = VideoProvider.SESSION_MODIFY_REQUEST_SUCCESS;
                break;
            case ImsQmiIF.E_CANCELLED:
                status = VideoProvider.SESSION_MODIFY_REQUEST_TIMED_OUT;
                break;
            case ImsQmiIF.E_REJECTED_BY_REMOTE:
                status = VideoProvider.SESSION_MODIFY_REQUEST_REJECTED_BY_REMOTE;
                break;
            case ImsQmiIF.E_INVALID_PARAMETER:
                status = VideoProvider.SESSION_MODIFY_REQUEST_INVALID;
                break;
            case QtiCallConstants.SESSION_MODIFY_REQUEST_FAILED_LOW_BATTERY:
                status = QtiCallConstants.SESSION_MODIFY_REQUEST_FAILED_LOW_BATTERY;
                break;
            default:
                status = VideoProvider.SESSION_MODIFY_REQUEST_FAIL;
        }
        return status;
    }

    public static boolean isConfigRequestValid(int item, int requestType) {
        int configType = CONFIG_TYPE_INVALID;
        /** Checks if item is within range including vendor configuration
        */
        if (ImsConfig.ConfigConstants.PROVISIONED_CONFIG_START <= item
                && item <= QTI_CONFIG_END) {
            switch (item) {
                case ImsConfig.ConfigConstants.VOCODER_AMRMODESET:
                case ImsConfig.ConfigConstants.VOCODER_AMRWBMODESET:
                case ImsConfig.ConfigConstants.DOMAIN_NAME:
                case ImsConfig.ConfigConstants.LBO_PCSCF_ADDRESS:
                case ImsConfig.ConfigConstants.SMS_PSI:
                    configType = CONFIG_TYPE_STRING;
                    break;
                default:
                    configType = CONFIG_TYPE_INT;
            }
        }
        return configType == requestType;
    }

    public static int convertImsConfigToProto(int config) {
        switch(config) {
            case ImsConfig.ConfigConstants.VOCODER_AMRMODESET:
                return ImsQmiIF.CONFIG_ITEM_VOCODER_AMRMODESET;
            case ImsConfig.ConfigConstants.VOCODER_AMRWBMODESET:
                return ImsQmiIF.CONFIG_ITEM_VOCODER_AMRWBMODESET;
            case ImsConfig.ConfigConstants.SIP_SESSION_TIMER:
                return ImsQmiIF.CONFIG_ITEM_SIP_SESSION_TIMER;
            case ImsConfig.ConfigConstants.MIN_SE:
                return ImsQmiIF.CONFIG_ITEM_MIN_SESSION_EXPIRY;
            case ImsConfig.ConfigConstants.CANCELLATION_TIMER:
                return ImsQmiIF.CONFIG_ITEM_CANCELLATION_TIMER;
            case ImsConfig.ConfigConstants.TDELAY:
                return ImsQmiIF.CONFIG_ITEM_T_DELAY;
            case ImsConfig.ConfigConstants.SILENT_REDIAL_ENABLE:
                return ImsQmiIF.CONFIG_ITEM_SILENT_REDIAL_ENABLE;
            case ImsConfig.ConfigConstants.SIP_T1_TIMER:
                return ImsQmiIF.CONFIG_ITEM_SIP_T1_TIMER;
            case ImsConfig.ConfigConstants.SIP_T2_TIMER:
                return ImsQmiIF.CONFIG_ITEM_SIP_T2_TIMER;
            case ImsConfig.ConfigConstants.SIP_TF_TIMER:
                return ImsQmiIF.CONFIG_ITEM_SIP_TF_TIMER;
            case ImsConfig.ConfigConstants.VLT_SETTING_ENABLED:
                return ImsQmiIF.CONFIG_ITEM_VLT_SETTING_ENABLED;
            case ImsConfig.ConfigConstants.LVC_SETTING_ENABLED:
                return ImsQmiIF.CONFIG_ITEM_LVC_SETTING_ENABLED;
            case ImsConfig.ConfigConstants.DOMAIN_NAME:
                return ImsQmiIF.CONFIG_ITEM_DOMAIN_NAME;
            case ImsConfig.ConfigConstants.SMS_FORMAT:
                return ImsQmiIF.CONFIG_ITEM_SMS_FORMAT;
            case ImsConfig.ConfigConstants.SMS_OVER_IP:
                return ImsQmiIF.CONFIG_ITEM_SMS_OVER_IP;
            case ImsConfig.ConfigConstants.PUBLISH_TIMER:
                return ImsQmiIF.CONFIG_ITEM_PUBLISH_TIMER;
            case ImsConfig.ConfigConstants.PUBLISH_TIMER_EXTENDED:
                return ImsQmiIF.CONFIG_ITEM_PUBLISH_TIMER_EXTENDED;
            case ImsConfig.ConfigConstants.CAPABILITIES_CACHE_EXPIRATION:
                return ImsQmiIF.CONFIG_ITEM_CAPABILITIES_CACHE_EXPIRATION;
            case ImsConfig.ConfigConstants.AVAILABILITY_CACHE_EXPIRATION:
                return ImsQmiIF.CONFIG_ITEM_AVAILABILITY_CACHE_EXPIRATION;
            case ImsConfig.ConfigConstants.CAPABILITIES_POLL_INTERVAL:
                return ImsQmiIF.CONFIG_ITEM_CAPABILITIES_POLL_INTERVAL;
            case ImsConfig.ConfigConstants.SOURCE_THROTTLE_PUBLISH:
                return ImsQmiIF.CONFIG_ITEM_SOURCE_THROTTLE_PUBLISH;
            case ImsConfig.ConfigConstants.MAX_NUMENTRIES_IN_RCL:
                return ImsQmiIF.CONFIG_ITEM_MAX_NUM_ENTRIES_IN_RCL;
            case ImsConfig.ConfigConstants.CAPAB_POLL_LIST_SUB_EXP:
                return ImsQmiIF.CONFIG_ITEM_CAPAB_POLL_LIST_SUB_EXP;
            case ImsConfig.ConfigConstants.GZIP_FLAG:
                return ImsQmiIF.CONFIG_ITEM_GZIP_FLAG;
            case ImsConfig.ConfigConstants.EAB_SETTING_ENABLED:
                return ImsQmiIF.CONFIG_ITEM_EAB_SETTING_ENABLED;
            case ImsConfig.ConfigConstants.VOICE_OVER_WIFI_ROAMING:
                return ImsQmiIF.CONFIG_ITEM_VOICE_OVER_WIFI_ROAMING;
            case ImsConfig.ConfigConstants.VOICE_OVER_WIFI_MODE:
                return ImsQmiIF.CONFIG_ITEM_VOICE_OVER_WIFI_MODE;
            case ImsConfig.ConfigConstants.VOICE_OVER_WIFI_SETTING_ENABLED:
                return ImsQmiIF.CONFIG_ITEM_VWF_SETTING_ENABLED;
            case ImsConfig.ConfigConstants.LBO_PCSCF_ADDRESS:
                return ImsQmiIF.CONFIG_ITEM_LBO_PCSCF_ADDRESS;
            case ImsConfig.ConfigConstants.MOBILE_DATA_ENABLED:
                return ImsQmiIF.CONFIG_ITEM_MOBILE_DATA_ENABLED;
            case ImsConfig.ConfigConstants.VOLTE_USER_OPT_IN_STATUS:
                return ImsQmiIF.CONFIG_ITEM_VOLTE_USER_OPT_IN_STATUS;
            case ImsConfig.ConfigConstants.KEEP_ALIVE_ENABLED:
                return ImsQmiIF.CONFIG_ITEM_KEEP_ALIVE_ENABLED;
            case ImsConfig.ConfigConstants.REGISTRATION_RETRY_BASE_TIME_SEC:
                return ImsQmiIF.CONFIG_ITEM_REGISTRATION_RETRY_BASE_TIME_SEC;
            case ImsConfig.ConfigConstants.REGISTRATION_RETRY_MAX_TIME_SEC:
                return ImsQmiIF.CONFIG_ITEM_REGISTRATION_RETRY_MAX_TIME_SEC;
            case ImsConfig.ConfigConstants.SPEECH_START_PORT:
                return ImsQmiIF.CONFIG_ITEM_SPEECH_START_PORT;
            case ImsConfig.ConfigConstants.SPEECH_END_PORT:
               return ImsQmiIF.CONFIG_ITEM_SPEECH_END_PORT;
            case ImsConfig.ConfigConstants.SIP_INVITE_REQ_RETX_INTERVAL_MSEC:
               return ImsQmiIF.CONFIG_ITEM_SIP_INVITE_REQ_RETX_INTERVAL_MSEC;
            case ImsConfig.ConfigConstants.SIP_INVITE_RSP_WAIT_TIME_MSEC:
               return ImsQmiIF.CONFIG_ITEM_SIP_INVITE_RSP_WAIT_TIME_MSEC;
            case ImsConfig.ConfigConstants.SIP_INVITE_RSP_RETX_WAIT_TIME_MSEC:
               return ImsQmiIF.CONFIG_ITEM_SIP_INVITE_RSP_RETX_WAIT_TIME_MSEC;
            case ImsConfig.ConfigConstants.SIP_NON_INVITE_REQ_RETX_INTERVAL_MSEC:
               return ImsQmiIF.CONFIG_ITEM_SIP_NON_INVITE_REQ_RETX_INTERVAL_MSEC;
            case ImsConfig.ConfigConstants.SIP_NON_INVITE_TXN_TIMEOUT_TIMER_MSEC:
               return ImsQmiIF.CONFIG_ITEM_SIP_NON_INVITE_TXN_TIMEOUT_TIMER_MSEC;
            case ImsConfig.ConfigConstants.SIP_INVITE_RSP_RETX_INTERVAL_MSEC:
               return ImsQmiIF.CONFIG_ITEM_SIP_INVITE_RSP_RETX_INTERVAL_MSEC;
            case ImsConfig.ConfigConstants.SIP_ACK_RECEIPT_WAIT_TIME_MSEC:
               return ImsQmiIF.CONFIG_ITEM_SIP_ACK_RECEIPT_WAIT_TIME_MSEC;
            case ImsConfig.ConfigConstants.SIP_ACK_RETX_WAIT_TIME_MSEC:
               return ImsQmiIF.CONFIG_ITEM_SIP_ACK_RETX_WAIT_TIME_MSEC;
            case ImsConfig.ConfigConstants.SIP_NON_INVITE_REQ_RETX_WAIT_TIME_MSEC:
               return ImsQmiIF.CONFIG_ITEM_SIP_NON_INVITE_REQ_RETX_WAIT_TIME_MSEC;
            case ImsConfig.ConfigConstants.SIP_NON_INVITE_RSP_RETX_WAIT_TIME_MSEC:
               return ImsQmiIF.CONFIG_ITEM_SIP_NON_INVITE_RSP_RETX_WAIT_TIME_MSEC;
            case ImsConfig.ConfigConstants.AMR_WB_OCTET_ALIGNED_PT:
               return ImsQmiIF.CONFIG_ITEM_AMR_WB_OCTET_ALIGNED_PT;
            case ImsConfig.ConfigConstants.AMR_WB_BANDWIDTH_EFFICIENT_PT:
               return ImsQmiIF.CONFIG_ITEM_AMR_WB_BANDWIDTH_EFFICIENT_PT;
            case ImsConfig.ConfigConstants.AMR_OCTET_ALIGNED_PT:
               return ImsQmiIF.CONFIG_ITEM_AMR_OCTET_ALIGNED_PT;
            case ImsConfig.ConfigConstants.AMR_BANDWIDTH_EFFICIENT_PT:
               return ImsQmiIF.CONFIG_ITEM_AMR_BANDWIDTH_EFFICIENT_PT;
            case ImsConfig.ConfigConstants.DTMF_WB_PT:
               return ImsQmiIF.CONFIG_ITEM_DTMF_WB_PT;
            case ImsConfig.ConfigConstants.DTMF_NB_PT:
               return ImsQmiIF.CONFIG_ITEM_DTMF_NB_PT;
            case ImsConfig.ConfigConstants.AMR_DEFAULT_MODE:
               return ImsQmiIF.CONFIG_ITEM_AMR_DEFAULT_MODE;
            case ImsConfig.ConfigConstants.SMS_PSI:
               return ImsQmiIF.CONFIG_ITEM_SMS_PSI;
            case ImsConfig.ConfigConstants.VIDEO_QUALITY:
                return ImsQmiIF.CONFIG_ITEM_VIDEO_QUALITY;
            case ImsConfig.ConfigConstants.TH_LTE1:
                return ImsQmiIF.CONFIG_ITEM_THRESHOLD_LTE1;
            case ImsConfig.ConfigConstants.TH_LTE2:
                return ImsQmiIF.CONFIG_ITEM_THRESHOLD_LTE2;
            case ImsConfig.ConfigConstants.TH_LTE3:
                return ImsQmiIF.CONFIG_ITEM_THRESHOLD_LTE3;
            case ImsConfig.ConfigConstants.TH_1x:
                return ImsQmiIF.CONFIG_ITEM_THRESHOLD_1x;
            case ImsConfig.ConfigConstants.VOWT_A:
                return ImsQmiIF.CONFIG_ITEM_THRESHOLD_WIFI_A;
            case ImsConfig.ConfigConstants.VOWT_B:
                return ImsQmiIF.CONFIG_ITEM_THRESHOLD_WIFI_B;
            case ImsConfig.ConfigConstants.T_EPDG_LTE:
                return ImsQmiIF.CONFIG_ITEM_T_EPDG_LTE;
            case ImsConfig.ConfigConstants.T_EPDG_WIFI:
                return ImsQmiIF.CONFIG_ITEM_T_EPDG_WIFI;
            case ImsConfig.ConfigConstants.T_EPDG_1X:
                return ImsQmiIF.CONFIG_ITEM_T_EPDG_1x;
            case ImsConfig.ConfigConstants.VICE_SETTING_ENABLED:
                return ImsQmiIF.CONFIG_ITEM_VCE_SETTING_ENABLED;
            case ImsConfig.ConfigConstants.RTT_SETTING_ENABLED:
                return ImsQmiIF.CONFIG_ITEM_RTT_SETTING_ENABLED;
            case QTI_CONFIG_SMS_APP:
                return ImsQmiIF.CONFIG_ITEM_SMS_APP;
            case QTI_CONFIG_VVM_APP:
                return ImsQmiIF.CONFIG_ITEM_VVM_APP;
            case ImsConfig.ConfigConstants.CAPABILITY_DISCOVERY_ENABLED:
                return ImsQmiIF.CONFIG_ITEM_CAPABILITY_DISCOVERY_ENABLED;

            default:
                throw new IllegalArgumentException();
        }
    }

    public static boolean isActive(DriverCallIms dc) {
        return (dc != null && dc.state == DriverCallIms.State.ACTIVE);
    }

    /**
     * Maps the proto exception error codes passed from lower layers (RIL) to internal error codes.
     */
    public static int toUiErrorCode(ImsRilException ex) {
        switch (ex.getErrorCode()) {
            case ImsQmiIF.E_HOLD_RESUME_FAILED:
                return QtiCallConstants.ERROR_CALL_SUPP_SVC_FAILED;
            case ImsQmiIF.E_HOLD_RESUME_CANCELED:
                return QtiCallConstants.ERROR_CALL_SUPP_SVC_CANCELLED;
            case ImsQmiIF.E_REINVITE_COLLISION:
                return QtiCallConstants.ERROR_CALL_SUPP_SVC_REINVITE_COLLISION;
            default:
                return QtiCallConstants.ERROR_CALL_CODE_UNSPECIFIED;
        }
    }

    /**
     * Maps the proto exception error codes passed from lower layers (RIL) to Ims error codes.
     */
    public static int toImsErrorCode(ImsRilException ex) {
        switch (ex.getErrorCode()) {
            case ImsQmiIF.E_HOLD_RESUME_FAILED:
                return ImsReasonInfo.CODE_SUPP_SVC_FAILED;
            case ImsQmiIF.E_HOLD_RESUME_CANCELED:
                return ImsReasonInfo.CODE_SUPP_SVC_CANCELLED;
            case ImsQmiIF.E_REINVITE_COLLISION:
                return ImsReasonInfo.CODE_SUPP_SVC_REINVITE_COLLISION;
            default:
                return ImsReasonInfo.CODE_UNSPECIFIED;
        }
    }

    /**
     * Check is carrier one supported or not
     */
    public static boolean isCarrierOneSupported() {
        String property = SystemProperties.get("persist.radio.atel.carrier");
        return "405854".equals(property);
    }

    /**
     * Checks if the vidoCall number is valid,ignore the special characters and invalid length
     *
     * @param number the number to call.
     * @return true if the number is valid
     *
     * @hide
     */
     public static boolean isVideoCallNumValid(String number) {
         if (number == null || number.isEmpty()
                 || number.length() < MIN_VIDEO_CALL_PHONE_NUMBER_LENGTH) {
             Log.w(TAG, "Phone number invalid!");
             return false;
         }

         /*if the first char is "+",then remove it,
          * this for support international call
         */
         if(number.startsWith("+")) {
            number = number.substring(1, number.length());
         }
         //Check non-digit characters '#', '+', ',', and ';'
         if (number.contains("#") || number.contains("+") ||
                 number.contains(",") || number.contains(";") ||
                 number.contains("*")) {
             return false;
         }
         return true;
     }

     /**
     * Creates ImsReasonInfo object from proto structure.
     */
    public static ImsReasonInfo getImsReasonInfo(ImsQmiIF.SipErrorInfo error,
                                                 int errorCode) {
        if (error == null) {
            return new ImsReasonInfo(errorCode, 0);
        } else {
            Log.d(TAG, "Sip error code:" + error.getSipErrorCode() +
                " error string :" + error.getSipErrorString());
            return new ImsReasonInfo(errorCode, error.getSipErrorCode(),
                error.getSipErrorString());
        }
    }

    /**
     * Creates ImsReasonInfo object from async result.
     */
    public static ImsReasonInfo getImsReasonInfo(AsyncResult ar)
    {
        ImsQmiIF.SipErrorInfo sipErrorInfo = null;
        int code = ImsReasonInfo.CODE_UNSPECIFIED;

        if(ar != null) {
            sipErrorInfo = (ImsQmiIF.SipErrorInfo) ar.result;
            if(ar.exception != null) {
                code = ImsCallUtils.toImsErrorCode((ImsRilException)ar.exception);
            }
        }
        return getImsReasonInfo(sipErrorInfo, code);
    }
}
