/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package org.codeaurora.ims;

import android.telephony.PhoneNumberUtils;

import com.android.ims.ImsReasonInfo;
import com.android.ims.ImsSsData;
import com.android.ims.ImsUtInterface;
import com.android.internal.telephony.CommandsInterface;

import com.google.protobuf.micro.ByteStringMicro;

import com.qualcomm.ims.utils.Log;
import vendor.qti.hardware.radio.ims.V1_0.AddressInfo;
import vendor.qti.hardware.radio.ims.V1_0.BlockReasonDetails;
import vendor.qti.hardware.radio.ims.V1_0.BlockReasonType;
import vendor.qti.hardware.radio.ims.V1_0.BlockStatus;
import vendor.qti.hardware.radio.ims.V1_0.CallDomain;
import vendor.qti.hardware.radio.ims.V1_0.CallFailCause;
import vendor.qti.hardware.radio.ims.V1_0.CallForwardInfo;
import vendor.qti.hardware.radio.ims.V1_0.CallFwdTimerInfo;
import vendor.qti.hardware.radio.ims.V1_0.CallInfo;
import vendor.qti.hardware.radio.ims.V1_0.CallModifyInfo;
import vendor.qti.hardware.radio.ims.V1_0.CallModifyFailCause;
import vendor.qti.hardware.radio.ims.V1_0.CallState;
import vendor.qti.hardware.radio.ims.V1_0.CallType;
import vendor.qti.hardware.radio.ims.V1_0.CbNumInfo;
import vendor.qti.hardware.radio.ims.V1_0.CbNumListInfo;
import vendor.qti.hardware.radio.ims.V1_0.ClipStatus;
import vendor.qti.hardware.radio.ims.V1_0.ConferenceCallState;
import vendor.qti.hardware.radio.ims.V1_0.ConfigFailureCause;
import vendor.qti.hardware.radio.ims.V1_0.ConfigInfo;
import vendor.qti.hardware.radio.ims.V1_0.ConfigItem;
import vendor.qti.hardware.radio.ims.V1_0.ConfParticipantOperation;
import vendor.qti.hardware.radio.ims.V1_0.DialRequest;
import vendor.qti.hardware.radio.ims.V1_0.EctType;
import vendor.qti.hardware.radio.ims.V1_0.ExplicitCallTransferInfo;
import vendor.qti.hardware.radio.ims.V1_0.Extra;
import vendor.qti.hardware.radio.ims.V1_0.ExtraType;
import vendor.qti.hardware.radio.ims.V1_0.FacilityType;
import vendor.qti.hardware.radio.ims.V1_0.HangupRequestInfo;
import vendor.qti.hardware.radio.ims.V1_0.HandoverInfo;
import vendor.qti.hardware.radio.ims.V1_0.HandoverType;
import vendor.qti.hardware.radio.ims.V1_0.ImsSubConfigInfo;
import vendor.qti.hardware.radio.ims.V1_0.IpPresentation;
import vendor.qti.hardware.radio.ims.V1_0.MessageDetails;
import vendor.qti.hardware.radio.ims.V1_0.MessagePriority;
import vendor.qti.hardware.radio.ims.V1_0.MessageSummary;
import vendor.qti.hardware.radio.ims.V1_0.MessageType;
import vendor.qti.hardware.radio.ims.V1_0.MessageWaitingIndication;
import vendor.qti.hardware.radio.ims.V1_0.NotificationType;
import vendor.qti.hardware.radio.ims.V1_0.ParticipantStatusInfo;
import vendor.qti.hardware.radio.ims.V1_0.RadioTechType;
import vendor.qti.hardware.radio.ims.V1_0.RegFailureReasonType;
import vendor.qti.hardware.radio.ims.V1_0.RegistrationInfo;
import vendor.qti.hardware.radio.ims.V1_0.RegState;
import vendor.qti.hardware.radio.ims.V1_0.RttMode;
import vendor.qti.hardware.radio.ims.V1_0.ServiceClassStatus;
import vendor.qti.hardware.radio.ims.V1_0.ServiceStatusInfo;
import vendor.qti.hardware.radio.ims.V1_0.SipErrorInfo;
import vendor.qti.hardware.radio.ims.V1_0.SsRequestType;
import vendor.qti.hardware.radio.ims.V1_0.SsServiceType;
import vendor.qti.hardware.radio.ims.V1_0.SsTeleserviceType;
import vendor.qti.hardware.radio.ims.V1_0.StatusForAccessTech;
import vendor.qti.hardware.radio.ims.V1_0.StatusType;
import vendor.qti.hardware.radio.ims.V1_0.SuppServiceNotification;
import vendor.qti.hardware.radio.ims.V1_0.SuppServiceStatus;
import vendor.qti.hardware.radio.ims.V1_0.ToneOperation;
import vendor.qti.hardware.radio.ims.V1_0.TtyMode;
import vendor.qti.hardware.radio.ims.V1_0.VoWiFiCallQualityInfo;

import java.util.ArrayList;
import java.util.Arrays;

public class ImsRadioUtils {
    private static String TAG = "ImsRadioUtils";

    private static final int INVALID_CONNID = 0;

    /**
     * Converts CallDetails object to the ImsRadio CallDetails.
     *
     * @param imsRadioCallDetails ImsRadio call details
     * @param callDetails IMS call details
     *
     */
    public static void callDetailsToHal(
        vendor.qti.hardware.radio.ims.V1_0.CallDetails imsRadioCallDetails, CallDetails callDetails) {
        imsRadioCallDetails.callType = callTypeToHal(callDetails.call_type);
        imsRadioCallDetails.callDomain = callDomainToHal(callDetails.call_domain);

        if (RttMode.RTT_MODE_INVALID != callDetails.getRttMode()) {
            imsRadioCallDetails.rttMode = callDetails.getRttMode();
        } else {
            imsRadioCallDetails.rttMode = RttMode.RTT_MODE_DISABLED;
        }
        imsRadioCallDetails.extrasLength = (callDetails.extras != null) ?
            callDetails.extras.length : 0;
        for (int i = 0; i < imsRadioCallDetails.extrasLength; i++) {
           imsRadioCallDetails.extras.add(i, callDetails.extras[i]);
        }
    }

    /**
     * Converts clir Mode to IP presentation
     *
     * @param clirMode caller identification mode
     * @return presentation hal IP presentation
     *
     */
    public static int getIpPresentation(int clirMode) {
        switch (clirMode) {
            case CommandsInterface.CLIR_SUPPRESSION:
                return IpPresentation.IP_PRESENTATION_NUM_RESTRICTED;
            case CommandsInterface.CLIR_INVOCATION:
            case CommandsInterface.CLIR_DEFAULT:
                return IpPresentation.IP_PRESENTATION_NUM_ALLOWED;
            default:
                return IpPresentation.IP_PRESENTATION_INVALID;
        }
    }

    /**
     * Get the conference uri from call details
     *
     * @param callDetails IMS call details
     * @return isConferenceUri true/false if it is a conference call or not
     *
     */
    public static boolean getIsConferenceUri(CallDetails callDetails) {
        boolean isConferenceUri = false;
        if (callDetails != null && callDetails.extras != null) {
            String value = callDetails.getValueForKeyFromExtras(callDetails.extras,
                CallDetails.EXTRAS_IS_CONFERENCE_URI);
            if (value != null && Boolean.valueOf(value)) {
                isConferenceUri = true;
            }
        }
        return isConferenceUri;
    }

    public static DriverCallIms buildDriverCallImsFromHal(CallInfo call) {
        DriverCallIms dc = new DriverCallIms();
        if (call.state != CallState.CALL_STATE_INVALID) {
            dc.state = DriverCallIms.stateFromCall(callStateFromHal(call.state));
        }

        if (call.index != Integer.MAX_VALUE) {
            dc.index = call.index;
        }

        if (call.toa != Integer.MAX_VALUE) {
            dc.TOA = call.toa;
        }

        if (call.hasIsMpty) {
            dc.isMpty = call.isMpty;
        }

        if (call.hasIsMT) {
            dc.isMT = call.isMT;
        }

        if (call.als != Integer.MAX_VALUE) {
            dc.als = call.als;
        }

        if (call.hasIsVoice) {
            dc.isVoice = call.isVoice;
        }

        if (call.hasIsVoicePrivacy) {
            dc.isVoicePrivacy = call.isVoicePrivacy;
        }

        dc.numberPresentation = DriverCallIms.presentationFromCLIP(
                call.numberPresentation);
        dc.name = call.name;
        dc.namePresentation = DriverCallIms.presentationFromCLIP(
                call.namePresentation);

        if (call.hasIsEncrypted) {
            dc.isEncrypted = call.isEncrypted;
        }

        dc.historyInfo = call.historyInfo;

        if (call.hasIsVideoConfSupported) {
            dc.mConfSupported = DriverCallIms.CONF_SUPPORT_INDICATED |
                    (call.isVideoConfSupported ?
                    DriverCallIms.CONF_VIDEO_SUPPORTED :
                    DriverCallIms.CONF_SUPPORT_NONE);
        }

        if (call.hasCallDetails) {
            dc.callDetails = getCallDetails(call.callDetails);
        }

        dc.number = call.number;
        dc.number = PhoneNumberUtils.stringFromStringAndTOA(dc.number, dc.TOA);

        dc.callFailCause =
                new ImsReasonInfo(ImsReasonInfo.CODE_UNSPECIFIED, 0);

        if (call.hasFailCause) {
            String networkError = null;
            // Check for an error message from the network.
            if (call.failCause.hasErrorDetails) {
                networkError = call.failCause.errorDetails.errorString;
            }

            final int failCause = call.failCause.failCause;
            // Check if the CallFailCauseResponse has an error code.
            if (failCause != CallFailCause.CALL_FAIL_INVALID) {
                dc.callFailCause.mCode
                             = getImsReasonForCallFailCause(failCause);
                if (call.failCause.hasErrorDetails &&
                        (call.failCause.errorDetails.errorCode != Integer.MAX_VALUE)) {
                    dc.callFailCause.mExtraCode = call.failCause.errorDetails.errorCode;
                } else {
                    log("CallFailCauseResponse has no int error code!");
                }

                dc.mCallFailReason = failCause;
                // If there is a network error, propagate it through
                // the ImsReasonInfo object.
                if (networkError != null) {
                    dc.callFailCause.mExtraMessage = networkError;
                }
            } else {
                log("CallFailCauseResponse failCause is Invalid");
            }
        }

        return dc;
    }

    public static ServiceStatusInfo buildServiceStatusInfo(int srvType, int network, int enabled,
            int restrictCause) {
        StatusForAccessTech statusForAccessTech = new StatusForAccessTech();
        statusForAccessTech.networkMode = ImsRadioUtils.mapRadioTechToHal(network);
        statusForAccessTech.status = ImsRadioUtils.statusTypeToHal(enabled);
        statusForAccessTech.restrictCause = restrictCause;
        statusForAccessTech.hasRegistration = false;

        ServiceStatusInfo serviceStatusInfo = new ServiceStatusInfo();
        serviceStatusInfo.hasIsValid = true;
        serviceStatusInfo.isValid = true;
        serviceStatusInfo.callType = ImsRadioUtils.callTypeToHal(srvType);
        serviceStatusInfo.accTechStatus.add(statusForAccessTech);

        return serviceStatusInfo;
    }

    private static int getImsReasonForCallFailCause(int failCause) {
        log("Call fail cause= " + failCause);

        switch (failCause) {
            // SIP Codes
            case CallFailCause.CALL_FAIL_SIP_REDIRECTED:
                return ImsReasonInfo.CODE_SIP_REDIRECTED;
            case CallFailCause.CALL_FAIL_SIP_BAD_REQUEST:
                return ImsReasonInfo.CODE_SIP_BAD_REQUEST;
            case CallFailCause.CALL_FAIL_SIP_FORBIDDEN:
                return ImsReasonInfo.CODE_SIP_FORBIDDEN;
            case CallFailCause.CALL_FAIL_SIP_NOT_FOUND:
                return ImsReasonInfo.CODE_SIP_NOT_FOUND;
            case CallFailCause.CALL_FAIL_SIP_NOT_SUPPORTED:
                return ImsReasonInfo.CODE_SIP_NOT_SUPPORTED;
            case CallFailCause.CALL_FAIL_SIP_REQUEST_TIMEOUT:
                return ImsReasonInfo.CODE_SIP_REQUEST_TIMEOUT;
            case CallFailCause.CALL_FAIL_SIP_TEMPORARILY_UNAVAILABLE:
                return ImsReasonInfo.CODE_SIP_TEMPRARILY_UNAVAILABLE;
            case CallFailCause.CALL_FAIL_SIP_BAD_ADDRESS:
                return ImsReasonInfo.CODE_SIP_BAD_ADDRESS;
            case CallFailCause.CALL_FAIL_SIP_BUSY:
                return ImsReasonInfo.CODE_SIP_BUSY;
            case CallFailCause.CALL_FAIL_SIP_REQUEST_CANCELLED:
                return ImsReasonInfo.CODE_SIP_REQUEST_CANCELLED;
            case CallFailCause.CALL_FAIL_SIP_NOT_ACCEPTABLE:
                return ImsReasonInfo.CODE_SIP_NOT_ACCEPTABLE;
            case CallFailCause.CALL_FAIL_SIP_NOT_REACHABLE:
                return ImsReasonInfo.CODE_SIP_NOT_REACHABLE;
            case CallFailCause.CALL_FAIL_SIP_SERVER_INTERNAL_ERROR:
                return ImsReasonInfo.CODE_SIP_SERVER_INTERNAL_ERROR;
            //TODO: Check if commented cases are needed.
            //case CallFailCause.CALL_FAIL_SIP_SERVER_NOT_IMPLEMENTED:
            //    return ImsReasonInfo.CODE_SIP_SERVER_NOT_IMPLEMENTED;
            //case CallFailCause.CALL_FAIL_SIP_SERVER_BAD_GATEWAY:
            //    return ImsReasonInfo.CODE_SIP_SERVER_BAD_GATEWAY;
            case CallFailCause.CALL_FAIL_SIP_SERVICE_UNAVAILABLE:
                return ImsReasonInfo.CODE_SIP_SERVICE_UNAVAILABLE;
            case CallFailCause.CALL_FAIL_SIP_SERVER_TIMEOUT:
                return ImsReasonInfo.CODE_SIP_SERVER_TIMEOUT;
            //case CallFailCause.CALL_FAIL_SIP_SERVER_VERSION_UNSUPPORTED:
            //    return ImsReasonInfo.CODE_SIP_SERVER_VERSION_UNSUPPORTED;
            //case CallFailCause.CALL_FAIL_SIP_SERVER_MESSAGE_TOOLARGE:
            //    return ImsReasonInfo.CODE_SIP_SERVER_MESSAGE_TOOLARGE;
            //case CallFailCause.CALL_FAIL_SIP_SERVER_PRECONDITION_FAILURE:
            //    return ImsReasonInfo.CODE_SIP_SERVER_PRECONDITION_FAILURE;
            case CallFailCause.CALL_FAIL_SIP_USER_REJECTED:
                return ImsReasonInfo.CODE_SIP_USER_REJECTED;
            case CallFailCause.CALL_FAIL_SIP_GLOBAL_ERROR:
                return ImsReasonInfo.CODE_SIP_GLOBAL_ERROR;
            // Media Codes
            case CallFailCause.CALL_FAIL_MEDIA_INIT_FAILED:
                return ImsReasonInfo.CODE_MEDIA_INIT_FAILED;
            case CallFailCause.CALL_FAIL_MEDIA_NO_DATA:
                return ImsReasonInfo.CODE_MEDIA_NO_DATA;
            case CallFailCause.CALL_FAIL_MEDIA_NOT_ACCEPTABLE:
                return ImsReasonInfo.CODE_MEDIA_NOT_ACCEPTABLE;
            case CallFailCause.CALL_FAIL_MEDIA_UNSPECIFIED_ERROR:
                return ImsReasonInfo.CODE_MEDIA_UNSPECIFIED;
            case CallFailCause.CALL_FAIL_NORMAL:
                return ImsReasonInfo.CODE_USER_TERMINATED;
            case CallFailCause.CALL_FAIL_BUSY:
                return ImsReasonInfo.CODE_SIP_BUSY;
            case CallFailCause.CALL_FAIL_NETWORK_UNAVAILABLE:
                return ImsReasonInfo.CODE_SIP_TEMPRARILY_UNAVAILABLE;
            case CallFailCause.CALL_FAIL_ANSWERED_ELSEWHERE:
                return ImsReasonInfo.CODE_ANSWERED_ELSEWHERE;
            case CallFailCause.CALL_FAIL_FDN_BLOCKED:
                return ImsReasonInfo.CODE_FDN_BLOCKED;
            case CallFailCause.CALL_FAIL_IMEI_NOT_ACCEPTED:
                return ImsReasonInfo.CODE_IMEI_NOT_ACCEPTED;
            case CallFailCause.CALL_FAIL_CS_RETRY_REQUIRED:
                return ImsReasonInfo.CODE_LOCAL_CALL_CS_RETRY_REQUIRED;
            case CallFailCause.CALL_FAIL_HO_NOT_FEASIBLE:
                return ImsReasonInfo.CODE_LOCAL_HO_NOT_FEASIBLE;
            case CallFailCause.CALL_FAIL_LOW_BATTERY:
                return ImsReasonInfo.CODE_LOW_BATTERY;
            case CallFailCause.CALL_FAIL_EMERGENCY_TEMP_FAILURE:
                return ImsReasonInfo.CODE_EMERGENCY_TEMP_FAILURE;
            case CallFailCause.CALL_FAIL_EMERGENCY_PERM_FAILURE:
                return ImsReasonInfo.CODE_EMERGENCY_PERM_FAILURE;
            case CallFailCause.CALL_FAIL_PULL_OUT_OF_SYNC:
                return ImsReasonInfo.CODE_CALL_PULL_OUT_OF_SYNC;
            case CallFailCause.CALL_FAIL_CAUSE_CALL_PULLED:
                return ImsReasonInfo.CODE_CALL_END_CAUSE_CALL_PULL;
            case CallFailCause.CALL_FAIL_ACCESS_CLASS_BLOCKED:
                return ImsReasonInfo.CODE_ACCESS_CLASS_BLOCKED;
            case CallFailCause.CALL_FAIL_DIAL_MODIFIED_TO_SS:
                return ImsReasonInfo.CODE_DIAL_MODIFIED_TO_SS;
            case CallFailCause.CALL_FAIL_DIAL_MODIFIED_TO_USSD:
                return ImsReasonInfo.CODE_DIAL_MODIFIED_TO_USSD;
            case CallFailCause.CALL_FAIL_DIAL_MODIFIED_TO_DIAL:
                return ImsReasonInfo.CODE_DIAL_MODIFIED_TO_DIAL;
            case CallFailCause.CALL_FAIL_DIAL_MODIFIED_TO_DIAL_VIDEO:
                return ImsReasonInfo.CODE_DIAL_MODIFIED_TO_DIAL_VIDEO;
            case CallFailCause.CALL_FAIL_DIAL_VIDEO_MODIFIED_TO_SS:
                return ImsReasonInfo.CODE_DIAL_VIDEO_MODIFIED_TO_SS;
            case CallFailCause.CALL_FAIL_DIAL_VIDEO_MODIFIED_TO_USSD:
                return ImsReasonInfo.CODE_DIAL_VIDEO_MODIFIED_TO_USSD;
            case CallFailCause.CALL_FAIL_DIAL_VIDEO_MODIFIED_TO_DIAL:
                return ImsReasonInfo.CODE_DIAL_VIDEO_MODIFIED_TO_DIAL;
            case CallFailCause.CALL_FAIL_DIAL_VIDEO_MODIFIED_TO_DIAL_VIDEO:
                return ImsReasonInfo.CODE_DIAL_VIDEO_MODIFIED_TO_DIAL_VIDEO;
            case CallFailCause.CALL_FAIL_SIP_ERROR:
            case CallFailCause.CALL_FAIL_UNOBTAINABLE_NUMBER:
            case CallFailCause.CALL_FAIL_CONGESTION:
            case CallFailCause.CALL_FAIL_INCOMPATIBILITY_DESTINATION:
            case CallFailCause.CALL_FAIL_CALL_BARRED:
            case CallFailCause.CALL_FAIL_FEATURE_UNAVAILABLE:
            case CallFailCause.CALL_FAIL_ERROR_UNSPECIFIED:
            default:
                return ImsReasonInfo.CODE_UNSPECIFIED;
        }
    }

    public static CallDetails getCallDetails(
            vendor.qti.hardware.radio.ims.V1_0.CallDetails inCallDetails) {
        CallDetails outCallDetails = new CallDetails();

        if (inCallDetails.callType != CallType.CALL_TYPE_INVALID) {
            outCallDetails.call_type = callTypeFromHal(inCallDetails.callType);
        }

        if (inCallDetails.callDomain != CallDomain.CALL_DOMAIN_INVALID) {
            outCallDetails.call_domain = callDomainFromHal(inCallDetails.callDomain);
        }

        if (inCallDetails.callSubstate != Integer.MAX_VALUE) {
            outCallDetails.callsubstate = toCallSubstateConstants(
                    inCallDetails.callSubstate);
        }

        if (inCallDetails.mediaId != Integer.MAX_VALUE) {
            outCallDetails.callMediaId = inCallDetails.mediaId;
        }

        outCallDetails.extras = new String[inCallDetails.extras.size()];
        outCallDetails.extras = inCallDetails.extras.toArray(outCallDetails.extras);

        outCallDetails.localAbility = copySrvStatusList(inCallDetails.localAbility);
        outCallDetails.peerAbility = copySrvStatusList(inCallDetails.peerAbility);

        if (inCallDetails.causeCode != Integer.MAX_VALUE) {
            outCallDetails.causeCode = inCallDetails.causeCode;
        }

        if (inCallDetails.rttMode != RttMode.RTT_MODE_INVALID) {
            outCallDetails.rttMode = inCallDetails.rttMode;
        }

        if (!inCallDetails.sipAlternateUri.isEmpty()) {
            outCallDetails.sipAlternateUri = inCallDetails.sipAlternateUri;
        }

        log("Call Details = " + outCallDetails);

        return outCallDetails;
    }

    private static ServiceStatus[] copySrvStatusList(ArrayList<ServiceStatusInfo> fromList) {
        ServiceStatus[] toList = null;
        if (fromList != null) {
            int listLen = fromList.size();
            log("Num of SrvUpdates = " + listLen);
            toList = new ServiceStatus[listLen];
            for (int i = 0; i < listLen; i++) {
                ServiceStatusInfo fromInfo = fromList.get(i);
                if (fromInfo != null) {
                    toList[i] = new ServiceStatus();
                    if (fromInfo.hasIsValid) {
                        toList[i].isValid = fromInfo.isValid;
                    }
                    toList[i].type = callTypeFromHal(fromInfo.callType);
                    if (fromInfo.accTechStatus.size() > 0) {
                        toList[i].accessTechStatus = unpackAccTechStatus(fromInfo);
                    } else {
                        toList[i].accessTechStatus = new ServiceStatus.StatusForAccessTech[1];
                        toList[i].accessTechStatus[0] = new ServiceStatus.StatusForAccessTech();
                        ServiceStatus.StatusForAccessTech act = toList[i].accessTechStatus[0];
                        act.networkMode = ImsQmiIF.RADIO_TECH_LTE;

                        if (fromInfo.status != StatusType.STATUS_INVALID) {
                            act.status = statusTypeFromHal(fromInfo.status);
                        }
                        if (fromInfo.restrictCause != Integer.MAX_VALUE) {
                            act.restrictCause = fromInfo.restrictCause;
                        }
                    }
                    if (fromInfo.status == StatusType.STATUS_ENABLED &&
                            fromInfo.restrictCause != CallDetails.CALL_RESTRICT_CAUSE_NONE) {
                        log("Partially Enabled Status due to Restrict Cause");
                        toList[i].status = ImsQmiIF.STATUS_PARTIALLY_ENABLED;
                    } else {
                        if (fromInfo.status != StatusType.STATUS_INVALID) {
                            toList[i].status = statusTypeFromHal(fromInfo.status);
                        }
                    }
                    if (RttMode.RTT_MODE_INVALID != fromInfo.rttMode) {
                        toList[i].rttMode = fromInfo.rttMode;
                    } else {
                        toList[i].rttMode = RttMode.RTT_MODE_DISABLED;
                    }
                    log("RTT: copySrvStatusList rtt mode = " + fromInfo.rttMode);
                    int userdataLen = fromInfo.userdata.size();
                    if (userdataLen > 0) {
                        toList[i].userdata = new byte[userdataLen];
                        for(int j = 0; j < userdataLen; ++j) {
                            toList[i].userdata[j] = fromInfo.userdata.get(j).byteValue();
                        }
                    }
                } else {
                    log("Null service status in list");
                }
            }
        }
        return toList;
    }
    /*
     *   Unpacks the status for access tech from Hal into proto object. The proto object is used to
     *   fill the array of ServiceStatus.StatusForAccessTech.
     *
     *   @param info Service status info from hal.
     *
     *   @return Array of ServiceStatus.StatusForAccessTech.
     *
     */
    private static ServiceStatus.StatusForAccessTech[] unpackAccTechStatus(ServiceStatusInfo info) {
        int statusListLen = info.accTechStatus.size();

        ServiceStatus.StatusForAccessTech[] statusForAccessTech =
                new ServiceStatus.StatusForAccessTech[statusListLen];

        for (int j = 0; j < statusListLen; j++) {
            ImsQmiIF.StatusForAccessTech update = statusForAccessTechFromHal(
                    info.accTechStatus.get(j));
            statusForAccessTech[j] = new ServiceStatus.StatusForAccessTech();
            statusForAccessTech[j].networkMode = update.getNetworkMode();
            statusForAccessTech[j].status = update.getStatus();
            statusForAccessTech[j].restrictCause = update.getRestrictionCause();
            if (update.getRegistered() != null) { // Registered is
                                                  // optional field
                statusForAccessTech[j].registered = update.getRegistered().getState();
            } else {
                statusForAccessTech[j].registered = ImsQmiIF.Registration.NOT_REGISTERED;
                Log.e(ImsRadioUtils.class, "Registered not sent");
            }
            log(" networkMode = " + statusForAccessTech[j].networkMode +
                    " status = " + statusForAccessTech[j].status +
                    " restrictCause = " + statusForAccessTech[j].restrictCause +
                    " registered = " + statusForAccessTech[j].registered);
       }

       return statusForAccessTech;
    }

    public static ImsQmiIF.StatusForAccessTech statusForAccessTechFromHal(
            StatusForAccessTech inStatus) {
        ImsQmiIF.StatusForAccessTech outStatus = null;

        if (inStatus != null) {
            outStatus = new ImsQmiIF.StatusForAccessTech();
            if (inStatus.networkMode != RadioTechType.RADIO_TECH_INVALID) {
                outStatus.setNetworkMode(radioTechFromHal(inStatus.networkMode));
            }

            if (inStatus.status != StatusType.STATUS_INVALID) {
                outStatus.setStatus(statusTypeFromHal(inStatus.status));
            }

            if (inStatus.restrictCause != Integer.MAX_VALUE) {
                outStatus.setRestrictionCause(inStatus.restrictCause);
            }

            if (inStatus.hasRegistration) {
                outStatus.setRegistered(registrationFromHal(inStatus.registration));
            }
        }

        return outStatus;
    }

    public static ImsQmiIF.Registration registrationFromHal(RegistrationInfo inRegistration) {
        ImsQmiIF.Registration outRegistration = null;

        if (inRegistration != null) {
            outRegistration = new ImsQmiIF.Registration();
            if (inRegistration.state != RegState.INVALID) {
                outRegistration.setState(regStateFromHal(inRegistration.state));
            }

            if (inRegistration.errorCode != Integer.MAX_VALUE) {
                outRegistration.setErrorCode(inRegistration.errorCode);
            }

            outRegistration.setErrorMessage(inRegistration.errorMessage);

            if (inRegistration.radioTech != RadioTechType.RADIO_TECH_INVALID) {
                outRegistration.setRadioTech(radioTechFromHal(inRegistration.radioTech));
            }

            outRegistration.setPAssociatedUris(inRegistration.pAssociatedUris);
        }

        return outRegistration;
    }

    private static int toCallSubstateConstants(int callSubstate) {
        switch (callSubstate) {
            case ImsQmiIF.CALL_SUBSTATE_AUDIO_CONNECTED_SUSPENDED:
                return QtiCallConstants.CALL_SUBSTATE_AUDIO_CONNECTED_SUSPENDED;
            case ImsQmiIF.CALL_SUBSTATE_VIDEO_CONNECTED_SUSPENDED:
                return QtiCallConstants.CALL_SUBSTATE_VIDEO_CONNECTED_SUSPENDED;
            case ImsQmiIF.CALL_SUBSTATE_AVP_RETRY:
                return QtiCallConstants.CALL_SUBSTATE_AVP_RETRY;
            case ImsQmiIF.CALL_SUBSTATE_MEDIA_PAUSED:
                return QtiCallConstants.CALL_SUBSTATE_MEDIA_PAUSED;
            case ImsQmiIF.CALL_SUBSTATE_NONE:
            default:
                return QtiCallConstants.CALL_SUBSTATE_NONE;
        }
    }

    public static int regStateFromHal(int state) {
        switch (state) {
            case RegState.REGISTERED:
                return ImsQmiIF.Registration.REGISTERED;
            case RegState.REGISTERING:
                return ImsQmiIF.Registration.REGISTERING;
            case RegState.NOT_REGISTERED:
            default:
                return ImsQmiIF.Registration.NOT_REGISTERED;
        }
    }

    public static int regStateToHal(int state) {
        switch (state) {
            case ImsQmiIF.Registration.REGISTERED:
                return RegState.REGISTERED;
            case ImsQmiIF.Registration.NOT_REGISTERED:
                return RegState.NOT_REGISTERED;
            case ImsQmiIF.Registration.REGISTERING:
                return RegState.REGISTERING;
            default:
                return RegState.INVALID;
        }
    }

    public static int radioTechFromHal(int radioTech) {
        switch (radioTech) {
            case RadioTechType.RADIO_TECH_ANY:
                return ImsQmiIF.RADIO_TECH_ANY;
            case RadioTechType.RADIO_TECH_UNKNOWN:
                return ImsQmiIF.RADIO_TECH_UNKNOWN;
            case RadioTechType.RADIO_TECH_GPRS:
                return ImsQmiIF.RADIO_TECH_GPRS;
            case RadioTechType.RADIO_TECH_EDGE:
                return ImsQmiIF.RADIO_TECH_EDGE;
            case RadioTechType.RADIO_TECH_UMTS:
                return ImsQmiIF.RADIO_TECH_UMTS;
            case RadioTechType.RADIO_TECH_IS95A:
                return ImsQmiIF.RADIO_TECH_IS95A;
            case RadioTechType.RADIO_TECH_IS95B:
                return ImsQmiIF.RADIO_TECH_IS95B;
            case RadioTechType.RADIO_TECH_1xRTT:
                return ImsQmiIF.RADIO_TECH_1xRTT;
            case RadioTechType.RADIO_TECH_EVDO_0:
                return ImsQmiIF.RADIO_TECH_EVDO_0;
            case RadioTechType.RADIO_TECH_EVDO_A:
                return ImsQmiIF.RADIO_TECH_EVDO_A;
            case RadioTechType.RADIO_TECH_HSDPA:
                return ImsQmiIF.RADIO_TECH_HSDPA;
            case RadioTechType.RADIO_TECH_HSUPA:
                return ImsQmiIF.RADIO_TECH_HSUPA;
            case RadioTechType.RADIO_TECH_HSPA:
                return ImsQmiIF.RADIO_TECH_HSPA;
            case RadioTechType.RADIO_TECH_EVDO_B:
                return ImsQmiIF.RADIO_TECH_EVDO_B;
            case RadioTechType.RADIO_TECH_EHRPD:
                return ImsQmiIF.RADIO_TECH_EHRPD;
            case RadioTechType.RADIO_TECH_LTE:
                return ImsQmiIF.RADIO_TECH_LTE;
            case RadioTechType.RADIO_TECH_HSPAP:
                return ImsQmiIF.RADIO_TECH_HSPAP;
            case RadioTechType.RADIO_TECH_GSM:
                return ImsQmiIF.RADIO_TECH_GSM;
            case RadioTechType.RADIO_TECH_TD_SCDMA:
                return ImsQmiIF.RADIO_TECH_TD_SCDMA;
            case RadioTechType.RADIO_TECH_WIFI:
                return ImsQmiIF.RADIO_TECH_WIFI;
            case RadioTechType.RADIO_TECH_IWLAN:
                return ImsQmiIF.RADIO_TECH_IWLAN;
            default:
                return ImsQmiIF.RADIO_TECH_UNKNOWN;
        }
    }

    public static int callTypeToHal(int callType) {
        switch (callType) {
            case ImsQmiIF.CALL_TYPE_VOICE:
                return CallType.CALL_TYPE_VOICE;
            case ImsQmiIF.CALL_TYPE_VT_TX:
                return CallType.CALL_TYPE_VT_TX;
            case ImsQmiIF.CALL_TYPE_VT_RX:
                return CallType.CALL_TYPE_VT_RX;
            case ImsQmiIF.CALL_TYPE_VT:
                return CallType.CALL_TYPE_VT;
            case ImsQmiIF.CALL_TYPE_VT_NODIR:
                return CallType.CALL_TYPE_VT_NODIR;
            case ImsQmiIF.CALL_TYPE_CS_VS_TX:
                return CallType.CALL_TYPE_CS_VS_TX;
            case ImsQmiIF.CALL_TYPE_CS_VS_RX:
                return CallType.CALL_TYPE_CS_VS_RX;
            case ImsQmiIF.CALL_TYPE_PS_VS_TX:
                return CallType.CALL_TYPE_PS_VS_TX;
            case ImsQmiIF.CALL_TYPE_PS_VS_RX:
                return CallType.CALL_TYPE_PS_VS_RX;
            case ImsQmiIF.CALL_TYPE_SMS:
                return CallType.CALL_TYPE_SMS;
            case ImsQmiIF.CALL_TYPE_UT:
                return CallType.CALL_TYPE_UT;
            case ImsQmiIF.CALL_TYPE_UNKNOWN:
            default:
                return CallType.CALL_TYPE_UNKNOWN;
        }
    }

    public static int callTypeFromHal(int callType) {
        switch (callType) {
            case CallType.CALL_TYPE_VOICE:
                return ImsQmiIF.CALL_TYPE_VOICE;
            case CallType.CALL_TYPE_VT_TX:
                return ImsQmiIF.CALL_TYPE_VT_TX;
            case CallType.CALL_TYPE_VT_RX:
                return ImsQmiIF.CALL_TYPE_VT_RX;
            case CallType.CALL_TYPE_VT:
                return ImsQmiIF.CALL_TYPE_VT;
            case CallType.CALL_TYPE_VT_NODIR:
                return ImsQmiIF.CALL_TYPE_VT_NODIR;
            case CallType.CALL_TYPE_CS_VS_TX:
                return ImsQmiIF.CALL_TYPE_CS_VS_TX;
            case CallType.CALL_TYPE_CS_VS_RX:
                return ImsQmiIF.CALL_TYPE_CS_VS_RX;
            case CallType.CALL_TYPE_PS_VS_TX:
                return ImsQmiIF.CALL_TYPE_PS_VS_TX;
            case CallType.CALL_TYPE_PS_VS_RX:
                return ImsQmiIF.CALL_TYPE_PS_VS_RX;
            case CallType.CALL_TYPE_SMS:
                return ImsQmiIF.CALL_TYPE_SMS;
            case CallType.CALL_TYPE_UT:
                return ImsQmiIF.CALL_TYPE_UT;
            case CallType.CALL_TYPE_UNKNOWN:
            case CallType.CALL_TYPE_INVALID:
            default:
                return ImsQmiIF.CALL_TYPE_UNKNOWN;
        }
    }

    public static int ipPresentationToHal(int presentation) {
        switch (presentation) {
            case ImsQmiIF.IP_PRESENTATION_NUM_ALLOWED:
                return IpPresentation.IP_PRESENTATION_NUM_ALLOWED;
            case ImsQmiIF.IP_PRESENTATION_NUM_RESTRICTED:
                return IpPresentation.IP_PRESENTATION_NUM_RESTRICTED;
            case ImsQmiIF.IP_PRESENTATION_NUM_DEFAULT:
                 return IpPresentation.IP_PRESENTATION_NUM_DEFAULT;
            default:
                return IpPresentation.IP_PRESENTATION_INVALID;
        }
    }

    public static ImsQmiIF.SipErrorInfo sipErrorFromHal(SipErrorInfo errorInfo) {
        ImsQmiIF.SipErrorInfo sipErrorInfo = new ImsQmiIF.SipErrorInfo();
        if (errorInfo.errorCode != Integer.MAX_VALUE) {
            sipErrorInfo.setSipErrorCode(errorInfo.errorCode);
        }
        sipErrorInfo.setSipErrorString(errorInfo.errorString);
        return sipErrorInfo;
    }

    public static DialRequest buildDialRequest(String address, int clirMode,
        CallDetails callDetails, boolean isEncrypted) {
        /**
          * DialRequest is the dial request struct containing params that are passed through
          * the HIDL interface dial. Populate the dial request struct from
          * the params passed into the dial API. {@see DialRequest}
          */
        DialRequest dialRequest = new DialRequest();
        if (address != null) {
            dialRequest.address = address;
        }
        dialRequest.clirMode = clirMode;

        dialRequest.presentation = getIpPresentation(clirMode);

        if (callDetails != null) {
            dialRequest.hasCallDetails = true;
            callDetailsToHal(dialRequest.callDetails, callDetails);
        }

        final boolean isConferenceUri = ImsRadioUtils.getIsConferenceUri(callDetails);
        if (isConferenceUri) {
            dialRequest.isConferenceUri = isConferenceUri;
            dialRequest.hasIsConferenceUri = isConferenceUri;
        }
        final boolean isCallPull = (callDetails != null) ? callDetails.getCallPull() : false;
        if (isCallPull) {
            dialRequest.isCallPull = isCallPull;
            dialRequest.hasIsCallPull = isCallPull;
        }

        dialRequest.hasIsEncrypted = true;
        dialRequest.isEncrypted = isEncrypted;
        return dialRequest;
    }

    public static HangupRequestInfo buildHangupRequest(int connectionId, String userUri,
        String confUri, boolean mpty, int failCause, String errorInfo) {
        HangupRequestInfo hangup = new HangupRequestInfo();
        /* If Calltracker has a matching local connection the connection id will be used.
         * if there is no matching connection object and if it is a remotely added participant
         * then connection id will not be present hence 0
         */
        if (connectionId != INVALID_CONNID) {
            hangup.connIndex = connectionId;
        } else {
            hangup.connIndex = Integer.MAX_VALUE;
        }

        if (userUri != null) {
            hangup.connUri = userUri;
        }
        hangup.hasMultiParty = true;
        hangup.multiParty = mpty;
        hangup.conf_id = Integer.MAX_VALUE;
        if (failCause != Integer.MAX_VALUE) {
            hangup.hasFailCauseResponse = true;
            if (errorInfo != null && !errorInfo.isEmpty()) {
                Log.v(TAG, "hangupWithReason errorInfo = " + errorInfo);
                for(Byte b: errorInfo.getBytes()) {
                    hangup.failCauseResponse.errorinfo.add(0, b);
                }
            }
            int callFailCause = getCallFailCauseForImsReason(failCause);
            hangup.failCauseResponse.failCause = callFailCause;
            Log.v(TAG, "hangupWithReason callFailCause=" + callFailCause);
            // Check for unsupported call end reason. If so, set
            // the errorInfo string to the reason code, similar to KK.
            if (callFailCause == CallFailCause.CALL_FAIL_MISC) {
                for(Byte b: Integer.toString(failCause).getBytes()) {
                    hangup.failCauseResponse.errorinfo.add(0, b);
                }
            }
            Log.v(TAG, "hangupWithReason MISC callFailCause, errorInfo=" + failCause);
        }
        return hangup;
    }

    public static int getCallFailCauseForImsReason(int imsReason) {
        Log.i(TAG, "imsReason= " + imsReason);
        int failCause;

        switch (imsReason) {
            case ImsReasonInfo.CODE_USER_DECLINE:
                failCause = CallFailCause.CALL_FAIL_USER_REJECT;
                break;
            case ImsReasonInfo.CODE_USER_TERMINATED:
                failCause = CallFailCause.CALL_FAIL_USER_BUSY;
                break;
            case ImsReasonInfo.CODE_LOW_BATTERY:
                failCause = CallFailCause.CALL_FAIL_LOW_BATTERY;
                break;
            case ImsReasonInfo.CODE_BLACKLISTED_CALL_ID:
                failCause = CallFailCause.CALL_FAIL_BLACKLISTED_CALL_ID;
                break;

            default:
                Log.i(TAG, "Unsupported imsReason for ending call. Passing end cause as 'misc'.");
                return CallFailCause.CALL_FAIL_MISC;
        }
        return failCause;
    }

    public static int statusTypeToHal(int status) {
        switch (status) {
            case ImsQmiIF.STATUS_DISABLED:
                return StatusType.STATUS_DISABLED;
            case ImsQmiIF.STATUS_PARTIALLY_ENABLED:
                return StatusType.STATUS_PARTIALLY_ENABLED;
            case ImsQmiIF.STATUS_ENABLED:
                return StatusType.STATUS_ENABLED;
            case ImsQmiIF.STATUS_NOT_SUPPORTED:
                return StatusType.STATUS_NOT_SUPPORTED;
            default:
                return StatusType.STATUS_INVALID;
        }
    }

    public static int statusTypeFromHal(int status) {
        switch (status) {
            case StatusType.STATUS_DISABLED:
                return ImsQmiIF.STATUS_DISABLED;
            case StatusType.STATUS_PARTIALLY_ENABLED:
                return ImsQmiIF.STATUS_PARTIALLY_ENABLED;
            case StatusType.STATUS_ENABLED:
                return ImsQmiIF.STATUS_ENABLED;
            case StatusType.STATUS_NOT_SUPPORTED:
            default:
                return ImsQmiIF.STATUS_NOT_SUPPORTED;
        }
    }

    public static int mapRadioTechToHal(int radioTech) {
        switch (radioTech) {
            case ImsQmiIF.RADIO_TECH_ANY:
                return RadioTechType.RADIO_TECH_ANY;
            case ImsQmiIF.RADIO_TECH_UNKNOWN:
                return RadioTechType.RADIO_TECH_UNKNOWN;
            case ImsQmiIF.RADIO_TECH_GPRS:
                return RadioTechType.RADIO_TECH_GPRS;
            case ImsQmiIF.RADIO_TECH_EDGE:
                return RadioTechType.RADIO_TECH_EDGE;
            case ImsQmiIF.RADIO_TECH_UMTS:
                return RadioTechType.RADIO_TECH_UMTS;
            case ImsQmiIF.RADIO_TECH_IS95A:
                return RadioTechType.RADIO_TECH_IS95A;
            case ImsQmiIF.RADIO_TECH_IS95B:
                return RadioTechType.RADIO_TECH_IS95B;
            case ImsQmiIF.RADIO_TECH_1xRTT:
                return RadioTechType.RADIO_TECH_1xRTT;
            case ImsQmiIF.RADIO_TECH_EVDO_0:
                return RadioTechType.RADIO_TECH_EVDO_0;
            case ImsQmiIF.RADIO_TECH_EVDO_A:
                return RadioTechType.RADIO_TECH_EVDO_A;
            case ImsQmiIF.RADIO_TECH_HSDPA:
                return RadioTechType.RADIO_TECH_HSDPA;
            case ImsQmiIF.RADIO_TECH_HSUPA:
                return RadioTechType.RADIO_TECH_HSUPA;
            case ImsQmiIF.RADIO_TECH_HSPA:
                return RadioTechType.RADIO_TECH_HSPA;
            case ImsQmiIF.RADIO_TECH_EVDO_B:
                return RadioTechType.RADIO_TECH_EVDO_B;
            case ImsQmiIF.RADIO_TECH_EHRPD:
                return RadioTechType.RADIO_TECH_EHRPD;
            case ImsQmiIF.RADIO_TECH_LTE:
                return RadioTechType.RADIO_TECH_LTE;
            case ImsQmiIF.RADIO_TECH_HSPAP:
                return RadioTechType.RADIO_TECH_HSPAP;
            case ImsQmiIF.RADIO_TECH_GSM:
                return RadioTechType.RADIO_TECH_GSM;
            case ImsQmiIF.RADIO_TECH_TD_SCDMA:
                return RadioTechType.RADIO_TECH_TD_SCDMA;
            case ImsQmiIF.RADIO_TECH_WIFI:
                return RadioTechType.RADIO_TECH_WIFI;
            case ImsQmiIF.RADIO_TECH_IWLAN:
                return RadioTechType.RADIO_TECH_IWLAN;
            default:
                return RadioTechType.RADIO_TECH_INVALID;
        }
    }

    public static int callDomainFromHal(int callDomain) {
        switch (callDomain) {
            case CallDomain.CALL_DOMAIN_UNKNOWN:
                return ImsQmiIF.CALL_DOMAIN_UNKNOWN;
            case CallDomain.CALL_DOMAIN_CS:
                return ImsQmiIF.CALL_DOMAIN_CS;
            case CallDomain.CALL_DOMAIN_PS:
                return ImsQmiIF.CALL_DOMAIN_PS;
            case CallDomain.CALL_DOMAIN_AUTOMATIC:
                return ImsQmiIF.CALL_DOMAIN_AUTOMATIC;
            case CallDomain.CALL_DOMAIN_NOT_SET:
            case CallDomain.CALL_DOMAIN_INVALID:
            default:
                return ImsQmiIF.CALL_DOMAIN_NOT_SET;
        }
    }

    public static int callDomainToHal(int callDomain) {
        switch (callDomain) {
            case ImsQmiIF.CALL_DOMAIN_UNKNOWN:
                return CallDomain.CALL_DOMAIN_UNKNOWN;
            case ImsQmiIF.CALL_DOMAIN_CS:
                return CallDomain.CALL_DOMAIN_CS;
            case ImsQmiIF.CALL_DOMAIN_PS:
                return CallDomain.CALL_DOMAIN_PS;
            case ImsQmiIF.CALL_DOMAIN_AUTOMATIC:
                return CallDomain.CALL_DOMAIN_AUTOMATIC;
            case ImsQmiIF.CALL_DOMAIN_NOT_SET:
            default:
                return CallDomain.CALL_DOMAIN_NOT_SET;
        }
    }

    public static int ringbackToneFromHal(int tone) {
        switch (tone) {
            case ToneOperation.START:
                return ImsQmiIF.RingBackTone.START;
            case ToneOperation.STOP:
            case ToneOperation.INVALID:
            default:
                return ImsQmiIF.RingBackTone.STOP;
        }
    }

    public static void log(String msg) {
        Log.i(ImsRadioUtils.class, msg);
    }

    protected static Object handleSrvStatus(ArrayList<ServiceStatusInfo> inList) {

        if (inList == null) {
            log("inList is null.");
            return null;
        }

        ServiceStatus[] outList =  copySrvStatusList(inList);
        ArrayList<ServiceStatus> response = new ArrayList<ServiceStatus>(Arrays.asList(outList));

        return response;
    }

    public static ImsQmiIF.Handover handoverFromHal(HandoverInfo inHandover) {

        if (inHandover == null) {
            return null;
        }

        ImsQmiIF.Handover outHandover = new ImsQmiIF.Handover();

        if (inHandover.type != HandoverType.INVALID) {
            outHandover.setType(handoverTypeFromHal(inHandover.type));
        }

        if (inHandover.srcTech != RadioTechType.RADIO_TECH_INVALID) {
            outHandover.setSrcTech(radioTechFromHal(inHandover.srcTech));
        }

        if (inHandover.targetTech != RadioTechType.RADIO_TECH_INVALID) {
            outHandover.setTargetTech(radioTechFromHal(inHandover.targetTech));
        }

        if (inHandover.hasHoExtra) {
            outHandover.setHoExtra(extraFromHal(inHandover.hoExtra));
        }

        outHandover.setErrorCode(inHandover.errorCode);
        outHandover.setErrorMessage(inHandover.errorMessage);

        return outHandover;
    }

    public static int handoverTypeFromHal(int inType) {
        switch (inType) {
            case HandoverType.START:
                return ImsQmiIF.START;
            case HandoverType.COMPLETE_SUCCESS:
                return ImsQmiIF.COMPLETE_SUCCESS;
            case HandoverType.COMPLETE_FAIL:
                return ImsQmiIF.COMPLETE_FAIL;
            case HandoverType.CANCEL:
                return ImsQmiIF.CANCEL;
            case HandoverType.NOT_TRIGGERED_MOBILE_DATA_OFF:
                return ImsQmiIF.NOT_TRIGGERED_MOBILE_DATA_OFF;
            default:
                return ImsQmiIF.NOT_TRIGGERED;
        }
    }

    private static int callStateFromHal(int inCallState) {
        switch (inCallState) {
            case CallState.CALL_ACTIVE:
                return ImsQmiIF.CALL_ACTIVE;
            case CallState.CALL_HOLDING:
                return ImsQmiIF.CALL_HOLDING;
            case CallState.CALL_DIALING:
                return ImsQmiIF.CALL_DIALING;
            case CallState.CALL_ALERTING:
                return ImsQmiIF.CALL_ALERTING;
            case CallState.CALL_INCOMING:
                return ImsQmiIF.CALL_INCOMING;
            case CallState.CALL_WAITING:
                return ImsQmiIF.CALL_WAITING;
            case CallState.CALL_END:
            default:
                return ImsQmiIF.CALL_END;
        }
    }

    public static CallForwardInfo buildCallForwardInfo(int cfReason, int serviceClass,
            String number, int action, int timeSeconds) {
        CallForwardInfo cfInfo = new CallForwardInfo();

        cfInfo.status = action;
        cfInfo.reason = cfReason;
        cfInfo.serviceClass = serviceClass;
        cfInfo.toa = PhoneNumberUtils.toaFromString(number);
        if (number != null) {
            cfInfo.number = number;
        }
        cfInfo.hasCallFwdTimerStart = false;
        cfInfo.hasCallFwdTimerEnd = false;
        cfInfo.timeSeconds = timeSeconds;

        return cfInfo;
    }

    public static void buildCallFwdTimerInfo(CallFwdTimerInfo callFwdTimerInfo, int hour,
            int minute) {
        callFwdTimerInfo.year = Integer.MAX_VALUE;
        callFwdTimerInfo.month = Integer.MAX_VALUE;
        callFwdTimerInfo.day = Integer.MAX_VALUE;
        callFwdTimerInfo.hour = hour;
        callFwdTimerInfo.minute = minute;
        callFwdTimerInfo.second = Integer.MAX_VALUE;
        callFwdTimerInfo.timezone = Integer.MAX_VALUE;
    }

    public static Object buildCFStatusResponseFromHal( ArrayList<CallForwardInfo> inCfInfoList) {
        com.android.internal.telephony.CallForwardInfo outCfInfoList[] = null;

        if (inCfInfoList != null) {
            int infoListSize = inCfInfoList.size();
            outCfInfoList = new com.android.internal.telephony.CallForwardInfo[infoListSize];
            ImsCallForwardTimerInfo timerInfo[] = new ImsCallForwardTimerInfo[infoListSize];

            for (int i = 0; i < infoListSize; i++) {
                outCfInfoList[i] = new com.android.internal.telephony.CallForwardInfo();
                CallForwardInfo inCfInfo = inCfInfoList.get(i);

                if (inCfInfo.status != Integer.MAX_VALUE) {
                    outCfInfoList[i].status = inCfInfo.status;
                }

                if (inCfInfo.reason != Integer.MAX_VALUE) {
                    outCfInfoList[i].reason = inCfInfo.reason;
                }

                if (inCfInfo.serviceClass != Integer.MAX_VALUE) {
                    outCfInfoList[i].serviceClass = inCfInfo.serviceClass;
                }

                if (inCfInfo.toa != Integer.MAX_VALUE) {
                    outCfInfoList[i].toa = inCfInfo.toa;
                }

                outCfInfoList[i].number = inCfInfo.number;

                if (inCfInfo.timeSeconds != Integer.MAX_VALUE) {
                    outCfInfoList[i].timeSeconds = inCfInfo.timeSeconds;
                }

                if (inCfInfo.hasCallFwdTimerStart && inCfInfo.hasCallFwdTimerEnd) {
                    log("responseQueryCallForward for Timer");
                    CallFwdTimerInfo startCallTimerInfo = inCfInfo.callFwdTimerStart;
                    timerInfo[i] = new ImsCallForwardTimerInfo();
                    if (inCfInfo.status != Integer.MAX_VALUE) {
                        timerInfo[i].status = inCfInfo.status;
                    }

                    if (inCfInfo.reason != Integer.MAX_VALUE) {
                        timerInfo[i].reason = inCfInfo.reason;
                    }

                    if (inCfInfo.serviceClass != Integer.MAX_VALUE) {
                        timerInfo[i].serviceClass = inCfInfo.serviceClass;
                    }

                    timerInfo[i].number = inCfInfo.number;

                    if (startCallTimerInfo.hour != Integer.MAX_VALUE) {
                        timerInfo[i].startHour = startCallTimerInfo.hour;
                    }

                    if (startCallTimerInfo.minute != Integer.MAX_VALUE) {
                        timerInfo[i].startMinute = startCallTimerInfo.minute;
                    }
                    CallFwdTimerInfo endCallTimerInfo = inCfInfo.callFwdTimerEnd;

                    if (endCallTimerInfo.hour != Integer.MAX_VALUE) {
                        timerInfo[i].endHour = endCallTimerInfo.hour;
                    }

                    if (endCallTimerInfo.minute != Integer.MAX_VALUE) {
                        timerInfo[i].endMinute = endCallTimerInfo.minute;
                    }
                    return timerInfo;
                }
            }
        } else {
            outCfInfoList = new com.android.internal.telephony.CallForwardInfo[0];
            log("inCfInfoList is null.");
        }

        return outCfInfoList;
    }

    private static ImsQmiIF.Extra extraFromHal(Extra inExtra) {
        ImsQmiIF.Extra outExtra = new ImsQmiIF.Extra();

        if (inExtra.type == ExtraType.LTE_TO_IWLAN_HO_FAIL) {
            outExtra.setType(ImsQmiIF.LTE_TO_IWLAN_HO_FAIL);
        }

        ByteStringMicro extraInfo = ByteStringMicro.copyFrom(getByteArray(inExtra.extraInfo));
        outExtra.setExtraInfo(extraInfo);
        return outExtra;
    }

    public static int serviceClassStatusFromHal(int inServiceStatus) {
        switch(inServiceStatus) {
            case ServiceClassStatus.ENABLED:
                return ImsQmiIF.ENABLED;
            case ServiceClassStatus.DISABLED:
            default:
                return ImsQmiIF.DISABLED;
        }
    }

    public static ImsQmiIF.SuppSvcNotification suppServiceNotificationFromHal(
            SuppServiceNotification inNotification) {
        if (inNotification == null) {
            return null;
        }

        ImsQmiIF.SuppSvcNotification outNotification = new ImsQmiIF.SuppSvcNotification();

        if (inNotification.notificationType != NotificationType.INVALID) {
            outNotification.setNotificationType(notificationTypeFromHal(
                    inNotification.notificationType));
        }

        if (inNotification.code != Integer.MAX_VALUE) {
            outNotification.setCode(inNotification.code);
        }

        if (inNotification.index != Integer.MAX_VALUE) {
            outNotification.setIndex(inNotification.index);
        }

        if (inNotification.type != Integer.MAX_VALUE) {
            outNotification.setType(inNotification.type);
        }

        if (inNotification.connId != Integer.MAX_VALUE) {
            outNotification.setConnId(inNotification.connId);
        }

        outNotification.setNumber(inNotification.number);
        outNotification.setHistoryInfo(inNotification.historyInfo);

        if (inNotification.hasHoldTone) {
            outNotification.setHoldTone(inNotification.holdTone);
        }

        return outNotification;
    }

    private static int notificationTypeFromHal(int inNotificationType) {
        switch (inNotificationType) {
            case NotificationType.MO:
                return ImsQmiIF.MO;
            case NotificationType.MT:
                return ImsQmiIF.MT;
            default:
                return Integer.MAX_VALUE;
        }
    }

    public static byte[] getByteArray(ArrayList<Byte> inList) {

        if (inList == null) {
            return null;
        }

        byte[] outArray = new byte[inList.size()];

        for(int i = 0; i < outArray.length; ++i) {
            outArray[i] = inList.get(i).byteValue();
        }

        return outArray;
    }

    public static int FacilityTypeFromHal(int facilityType) {
         switch (facilityType) {
            case FacilityType.FACILITY_CLIP:
                return ImsQmiIF.FACILITY_CLIP;
            case FacilityType.FACILITY_COLP:
                return ImsQmiIF.FACILITY_COLP;
            case FacilityType.FACILITY_BAOC:
                return ImsQmiIF.FACILITY_BAOC;
            case FacilityType.FACILITY_BAOICxH:
                return ImsQmiIF.FACILITY_BAOICxH;
            case FacilityType.FACILITY_BAIC:
                return ImsQmiIF.FACILITY_BAIC;
            case FacilityType.FACILITY_BAICr:
                return ImsQmiIF.FACILITY_BAICr;
            case FacilityType.FACILITY_BA_ALL:
                return ImsQmiIF.FACILITY_BA_ALL;
            case FacilityType.FACILITY_BA_MO:
                return ImsQmiIF.FACILITY_BA_MO;
            case FacilityType.FACILITY_BA_MT:
                return ImsQmiIF.FACILITY_BA_MT;
            case FacilityType.FACILITY_BS_MT:
                return ImsQmiIF.FACILITY_BS_MT;
            case FacilityType.FACILITY_BAICa:
                return ImsQmiIF.FACILITY_BAICa;
            case FacilityType.FACILITY_BAOIC:
                return ImsQmiIF.FACILITY_BAOIC;
            case FacilityType.FACILITY_INVALID:
            default:
                return ImsQmiIF.FACILITY_BA_ALL;
        }
    }

    public static int facilityTypeToHal(int facilityType) {
         switch (facilityType) {
            case ImsQmiIF.FACILITY_CLIP:
                return FacilityType.FACILITY_CLIP;
            case ImsQmiIF.FACILITY_COLP:
                return FacilityType.FACILITY_COLP;
            case ImsQmiIF.FACILITY_BAOC:
                return FacilityType.FACILITY_BAOC;
            case ImsQmiIF.FACILITY_BAOIC:
                return FacilityType.FACILITY_BAOIC;
            case ImsQmiIF.FACILITY_BAOICxH:
                return FacilityType.FACILITY_BAOICxH;
            case ImsQmiIF.FACILITY_BAIC:
                return FacilityType.FACILITY_BAIC;
            case ImsQmiIF.FACILITY_BAICr:
                return FacilityType.FACILITY_BAICr;
            case ImsQmiIF.FACILITY_BA_ALL:
                return FacilityType.FACILITY_BA_ALL;
            case ImsQmiIF.FACILITY_BA_MO:
                return FacilityType.FACILITY_BA_MO;
            case ImsQmiIF.FACILITY_BA_MT:
                return FacilityType.FACILITY_BA_MT;
            case ImsQmiIF.FACILITY_BS_MT:
                return FacilityType.FACILITY_BS_MT;
            case ImsQmiIF.FACILITY_BAICa:
                return FacilityType.FACILITY_BAICa;
            default:
                return FacilityType.FACILITY_INVALID;
        }
    }

    public static ImsQmiIF.SuppSvcResponse suppSvcStatusResponseFromHal(
        SuppServiceStatus suppServiceStatus) {
        ImsQmiIF.SuppSvcResponse suppSvcResponse = new ImsQmiIF.SuppSvcResponse();

        if (suppServiceStatus.status != ServiceClassStatus.INVALID) {
            suppSvcResponse.setStatus(serviceClassStatusFromHal(suppServiceStatus.status));
        }

        if (suppServiceStatus.facilityType != FacilityType.FACILITY_INVALID) {
            suppSvcResponse.setFacilityType(FacilityTypeFromHal(suppServiceStatus.facilityType));
        }

        if (suppServiceStatus.failureCause.length() > 0) {
            suppSvcResponse.setFailureCause(suppServiceStatus.failureCause);
        }

        for (int i = 0; i < suppServiceStatus.cbNumListInfo.size(); i++) {
            ImsQmiIF.CbNumListType cbNumListType = new ImsQmiIF.CbNumListType();
            CbNumListInfo inCbNumListInfo = suppServiceStatus.cbNumListInfo.get(i);
            if (inCbNumListInfo.serviceClass != Integer.MAX_VALUE) {
                ImsQmiIF.ServiceClass toServiceClass = new ImsQmiIF.ServiceClass();
                toServiceClass.setServiceClass(inCbNumListInfo.serviceClass);
                cbNumListType.setServiceClass(toServiceClass);
            }

            if (inCbNumListInfo.cbNumInfo != null) {
               for (int j = 0; j < inCbNumListInfo.cbNumInfo.size(); j++) {
                    ImsQmiIF.CbNumList cbNumList = new ImsQmiIF.CbNumList();
                    cbNumList.setStatus(inCbNumListInfo.cbNumInfo.get(j).status);
                    cbNumList.setNumber(inCbNumListInfo.cbNumInfo.get(j).number);
                    cbNumListType.addCbNumList(cbNumList);
                }
            }
            suppSvcResponse.addCbNumListType(cbNumListType);
        }

        if (suppServiceStatus.hasErrorDetails) {
            suppSvcResponse.setErrorDetails(sipErrorFromHal(suppServiceStatus.errorDetails));
        }
        return suppSvcResponse;
    }

    public static ByteStringMicro getByteStringMicro(ArrayList<Byte> inList) {
        byte[] ret = getByteArray(inList);
        final ByteStringMicro confUri = ByteStringMicro.copyFrom(ret);
        return confUri;
    }


    public static int ipPresentationFromHal(int presentation) {
        switch(presentation) {
            case IpPresentation.IP_PRESENTATION_NUM_ALLOWED:
                return ImsQmiIF.IP_PRESENTATION_NUM_ALLOWED;
            case IpPresentation.IP_PRESENTATION_NUM_RESTRICTED:
                return ImsQmiIF.IP_PRESENTATION_NUM_RESTRICTED;
            case IpPresentation.IP_PRESENTATION_NUM_DEFAULT:
            default:
                 return ImsQmiIF.IP_PRESENTATION_NUM_DEFAULT;
        }
    }

    public static int ttyModeFromHal(int ttyMode) {
        switch(ttyMode) {
            case TtyMode.TTY_MODE_OFF:
                return ImsQmiIF.TTY_MODE_OFF;
            case TtyMode.TTY_MODE_FULL:
                return ImsQmiIF.TTY_MODE_FULL;
            case TtyMode.TTY_MODE_HCO:
                return ImsQmiIF.TTY_MODE_HCO;
            case TtyMode.TTY_MODE_VCO:
                return ImsQmiIF.TTY_MODE_VCO;
            default:
                 return ImsQmiIF.TTY_MODE_OFF;
        }
    }

    public static int ttyModeToHal(int ttyMode) {
        switch(ttyMode) {
            case ImsQmiIF.TTY_MODE_FULL:
                return TtyMode.TTY_MODE_FULL;
            case ImsQmiIF.TTY_MODE_HCO:
                return TtyMode.TTY_MODE_HCO;
            case ImsQmiIF.TTY_MODE_VCO:
                return TtyMode.TTY_MODE_VCO;
            case ImsQmiIF.TTY_MODE_OFF:
                return TtyMode.TTY_MODE_OFF;
            default:
                 return TtyMode.TTY_MODE_INVALID;
        }
    }

    public static int clipStatusFromHal(int clipStatus) {
        switch(clipStatus) {
            case ClipStatus.NOT_PROVISIONED:
                return ImsQmiIF.NOT_PROVISIONED;
            case ClipStatus.PROVISIONED:
                return ImsQmiIF.PROVISIONED;
            case ClipStatus.STATUS_UNKNOWN:
            default:
                return ImsQmiIF.STATUS_UNKNOWN;
        }
    }

    public static int conferenceCallStateFromHal(int conferenceCallState) {
         switch(conferenceCallState) {
            case ConferenceCallState.RINGING:
                return ImsQmiIF.RINGING;
            case ConferenceCallState.FOREGROUND:
                return ImsQmiIF.FOREGROUND;
            case ConferenceCallState.BACKGROUND:
            default:
                return ImsQmiIF.BACKGROUND;
        }
    }

    public static CallModifyInfo buildCallModifyInfo(CallModify callModify) {
        log("buildCallModifyInfo callModify= " + callModify);
        CallModifyInfo callModifyInfo = new CallModifyInfo();
        callModifyInfo.callIndex = callModify.call_index;

        if (callModify.call_details != null) {
            callModifyInfo.hasCallDetails = true;
            callDetailsToHal(callModifyInfo.callDetails, callModify.call_details);
        }

        //CallModifyFailCause is not used for outgoing requests.
        callModifyInfo.failCause = CallModifyFailCause.E_INVALID;

        return callModifyInfo;
    }

    public static CallModify callModifyFromHal(CallModifyInfo callModifyInfo) {
        CallModify callModify = new CallModify();

        if (callModifyInfo.hasCallDetails) {
            callModify.call_details = getCallDetails(callModifyInfo.callDetails);
        }

        if (callModifyInfo.callIndex != Integer.MAX_VALUE) {
            callModify.call_index = callModifyInfo.callIndex;
        }

        callModify.error = ImsQmiIF.E_SUCCESS;
        if (callModifyInfo.failCause != CallModifyFailCause.E_INVALID) {
            callModify.error = callModifyFailCauseFromHal(callModifyInfo.failCause);
        }

        return callModify;
    }

    private static int callModifyFailCauseFromHal(int failCause) {
        switch (failCause) {
            case CallModifyFailCause.E_SUCCESS:
                return ImsQmiIF.E_SUCCESS;
            case CallModifyFailCause.E_RADIO_NOT_AVAILABLE:
                return ImsQmiIF.E_RADIO_NOT_AVAILABLE;
            case CallModifyFailCause.E_GENERIC_FAILURE:
                return ImsQmiIF.E_GENERIC_FAILURE;
            case CallModifyFailCause.E_REQUEST_NOT_SUPPORTED:
                return ImsQmiIF.E_REQUEST_NOT_SUPPORTED;
            case CallModifyFailCause.E_CANCELLED:
                return ImsQmiIF.E_CANCELLED;
            case CallModifyFailCause.E_UNUSED:
                return ImsQmiIF.E_UNUSED;
            case CallModifyFailCause.E_INVALID_PARAMETER:
                return ImsQmiIF.E_INVALID_PARAMETER;
            case CallModifyFailCause.E_REJECTED_BY_REMOTE:
                return ImsQmiIF.E_REJECTED_BY_REMOTE;
            case CallModifyFailCause.E_IMS_DEREGISTERED:
                return ImsQmiIF.E_IMS_DEREGISTERED;
            case CallModifyFailCause.E_NETWORK_NOT_SUPPORTED:
                return ImsQmiIF.E_NETWORK_NOT_SUPPORTED;
            case CallModifyFailCause.E_HOLD_RESUME_FAILED:
                return ImsQmiIF.E_HOLD_RESUME_FAILED;
            case CallModifyFailCause.E_HOLD_RESUME_CANCELED:
                return ImsQmiIF.E_HOLD_RESUME_CANCELED;
            case CallModifyFailCause.E_REINVITE_COLLISION:
                return ImsQmiIF.E_REINVITE_COLLISION;
            default:
                return ImsQmiIF.E_SUCCESS;
        }
    }

    public static ConfigInfo buildConfigInfo(int item, boolean boolValue, int intValue,
            String stringValue, int errorCause) {
        ConfigInfo configInfo = new ConfigInfo();

        configInfo.item = configInfoItemToHal(item);
        configInfo.hasBoolValue = true;
        configInfo.boolValue = boolValue;
        configInfo.intValue = intValue;
        if (stringValue != null) {
            configInfo.stringValue = stringValue;
        }
        configInfo.errorCause = configFailureCauseToHal(errorCause);

        return configInfo;
    }

    public static ImsQmiIF.ConfigMsg configInfoFromHal(ConfigInfo configInfo) {

        if (configInfo == null) {
            return null;
        }

        ImsQmiIF.ConfigMsg config = new ImsQmiIF.ConfigMsg();

        config.setItem(configInfoItemFromHal(configInfo.item));

        if (configInfo.hasBoolValue) {
            config.setBoolValue(configInfo.boolValue);
        }

        if (configInfo.intValue != Integer.MAX_VALUE) {
            config.setIntValue(configInfo.intValue);
        }

        config.setStringValue(configInfo.stringValue);

        if (configInfo.errorCause != ConfigFailureCause.CONFIG_FAILURE_INVALID) {
            config.setErrorCause(configFailureCauseFromHal(configInfo.errorCause));
        }

        return config;
    }

    private static int configInfoItemToHal(int item) {
        switch (item) {
            case ImsQmiIF.CONFIG_ITEM_NONE:
                return ConfigItem.CONFIG_ITEM_NONE;
            case ImsQmiIF.CONFIG_ITEM_VOCODER_AMRMODESET:
                return ConfigItem.CONFIG_ITEM_VOCODER_AMRMODESET;
            case ImsQmiIF.CONFIG_ITEM_VOCODER_AMRWBMODESET:
                return ConfigItem.CONFIG_ITEM_VOCODER_AMRWBMODESET;
            case ImsQmiIF.CONFIG_ITEM_SIP_SESSION_TIMER:
                return ConfigItem.CONFIG_ITEM_SIP_SESSION_TIMER;
            case ImsQmiIF.CONFIG_ITEM_MIN_SESSION_EXPIRY:
                return ConfigItem.CONFIG_ITEM_MIN_SESSION_EXPIRY;
            case ImsQmiIF.CONFIG_ITEM_CANCELLATION_TIMER:
                return ConfigItem.CONFIG_ITEM_CANCELLATION_TIMER;
            case ImsQmiIF.CONFIG_ITEM_T_DELAY:
                return ConfigItem.CONFIG_ITEM_T_DELAY;
            case ImsQmiIF.CONFIG_ITEM_SILENT_REDIAL_ENABLE:
                return ConfigItem.CONFIG_ITEM_SILENT_REDIAL_ENABLE;
            case ImsQmiIF.CONFIG_ITEM_SIP_T1_TIMER:
                return ConfigItem.CONFIG_ITEM_SIP_T1_TIMER;
            case ImsQmiIF.CONFIG_ITEM_SIP_T2_TIMER:
                return ConfigItem.CONFIG_ITEM_SIP_T2_TIMER;
            case ImsQmiIF.CONFIG_ITEM_SIP_TF_TIMER:
                return ConfigItem.CONFIG_ITEM_SIP_TF_TIMER;
            case ImsQmiIF.CONFIG_ITEM_VLT_SETTING_ENABLED:
                return ConfigItem.CONFIG_ITEM_VLT_SETTING_ENABLED;
            case ImsQmiIF.CONFIG_ITEM_LVC_SETTING_ENABLED:
                return ConfigItem.CONFIG_ITEM_LVC_SETTING_ENABLED;
            case ImsQmiIF.CONFIG_ITEM_DOMAIN_NAME:
                return ConfigItem.CONFIG_ITEM_DOMAIN_NAME;
            case ImsQmiIF.CONFIG_ITEM_SMS_FORMAT:
                return ConfigItem.CONFIG_ITEM_SMS_FORMAT;
            case ImsQmiIF.CONFIG_ITEM_SMS_OVER_IP:
                return ConfigItem.CONFIG_ITEM_SMS_OVER_IP;
            case ImsQmiIF.CONFIG_ITEM_PUBLISH_TIMER:
                return ConfigItem.CONFIG_ITEM_PUBLISH_TIMER;
            case ImsQmiIF.CONFIG_ITEM_PUBLISH_TIMER_EXTENDED:
                return ConfigItem.CONFIG_ITEM_PUBLISH_TIMER_EXTENDED;
            case ImsQmiIF.CONFIG_ITEM_CAPABILITIES_CACHE_EXPIRATION:
                return ConfigItem.CONFIG_ITEM_CAPABILITIES_CACHE_EXPIRATION;
            case ImsQmiIF.CONFIG_ITEM_AVAILABILITY_CACHE_EXPIRATION:
                return ConfigItem.CONFIG_ITEM_AVAILABILITY_CACHE_EXPIRATION;
            case ImsQmiIF.CONFIG_ITEM_CAPABILITIES_POLL_INTERVAL:
                return ConfigItem.CONFIG_ITEM_CAPABILITIES_POLL_INTERVAL;
            case ImsQmiIF.CONFIG_ITEM_SOURCE_THROTTLE_PUBLISH:
                return ConfigItem.CONFIG_ITEM_SOURCE_THROTTLE_PUBLISH;
            case ImsQmiIF.CONFIG_ITEM_MAX_NUM_ENTRIES_IN_RCL:
                return ConfigItem.CONFIG_ITEM_MAX_NUM_ENTRIES_IN_RCL;
            case ImsQmiIF.CONFIG_ITEM_CAPAB_POLL_LIST_SUB_EXP:
                return ConfigItem.CONFIG_ITEM_CAPAB_POLL_LIST_SUB_EXP;
            case ImsQmiIF.CONFIG_ITEM_GZIP_FLAG:
                return ConfigItem.CONFIG_ITEM_GZIP_FLAG;
            case ImsQmiIF.CONFIG_ITEM_EAB_SETTING_ENABLED:
                return ConfigItem.CONFIG_ITEM_EAB_SETTING_ENABLED;
            case ImsQmiIF.CONFIG_ITEM_MOBILE_DATA_ENABLED:
                return ConfigItem.CONFIG_ITEM_MOBILE_DATA_ENABLED;
            case ImsQmiIF.CONFIG_ITEM_VOICE_OVER_WIFI_ENABLED:
                return ConfigItem.CONFIG_ITEM_VOICE_OVER_WIFI_ENABLED;
            case ImsQmiIF.CONFIG_ITEM_VOICE_OVER_WIFI_ROAMING:
                return ConfigItem.CONFIG_ITEM_VOICE_OVER_WIFI_ROAMING;
            case ImsQmiIF.CONFIG_ITEM_VOICE_OVER_WIFI_MODE:
                return ConfigItem.CONFIG_ITEM_VOICE_OVER_WIFI_MODE;
            case ImsQmiIF.CONFIG_ITEM_CAPABILITY_DISCOVERY_ENABLED:
                return ConfigItem.CONFIG_ITEM_CAPABILITY_DISCOVERY_ENABLED;
            case ImsQmiIF.CONFIG_ITEM_EMERGENCY_CALL_TIMER:
                return ConfigItem.CONFIG_ITEM_EMERGENCY_CALL_TIMER;
            case ImsQmiIF.CONFIG_ITEM_SSAC_HYSTERESIS_TIMER:
                return ConfigItem.CONFIG_ITEM_SSAC_HYSTERESIS_TIMER;
            case ImsQmiIF.CONFIG_ITEM_VOLTE_USER_OPT_IN_STATUS:
                return ConfigItem.CONFIG_ITEM_VOLTE_USER_OPT_IN_STATUS;
            case ImsQmiIF.CONFIG_ITEM_LBO_PCSCF_ADDRESS:
                return ConfigItem.CONFIG_ITEM_LBO_PCSCF_ADDRESS;
            case ImsQmiIF.CONFIG_ITEM_KEEP_ALIVE_ENABLED:
                return ConfigItem.CONFIG_ITEM_KEEP_ALIVE_ENABLED;
            case ImsQmiIF.CONFIG_ITEM_REGISTRATION_RETRY_BASE_TIME_SEC:
                return ConfigItem.CONFIG_ITEM_REGISTRATION_RETRY_BASE_TIME_SEC;
            case ImsQmiIF.CONFIG_ITEM_REGISTRATION_RETRY_MAX_TIME_SEC:
                return ConfigItem.CONFIG_ITEM_REGISTRATION_RETRY_MAX_TIME_SEC;
            case ImsQmiIF.CONFIG_ITEM_SPEECH_START_PORT:
                return ConfigItem.CONFIG_ITEM_SPEECH_START_PORT;
            case ImsQmiIF.CONFIG_ITEM_SPEECH_END_PORT:
                return ConfigItem.CONFIG_ITEM_SPEECH_END_PORT;
            case ImsQmiIF.CONFIG_ITEM_SIP_INVITE_REQ_RETX_INTERVAL_MSEC:
                return ConfigItem.CONFIG_ITEM_SIP_INVITE_REQ_RETX_INTERVAL_MSEC;
            case ImsQmiIF.CONFIG_ITEM_SIP_INVITE_RSP_WAIT_TIME_MSEC:
                return ConfigItem.CONFIG_ITEM_SIP_INVITE_RSP_WAIT_TIME_MSEC;
            case ImsQmiIF.CONFIG_ITEM_SIP_INVITE_RSP_RETX_WAIT_TIME_MSEC:
                return ConfigItem.CONFIG_ITEM_SIP_INVITE_RSP_RETX_WAIT_TIME_MSEC;
            case ImsQmiIF.CONFIG_ITEM_SIP_NON_INVITE_REQ_RETX_INTERVAL_MSEC:
                return ConfigItem.CONFIG_ITEM_SIP_NON_INVITE_REQ_RETX_INTERVAL_MSEC;
            case ImsQmiIF.CONFIG_ITEM_SIP_NON_INVITE_TXN_TIMEOUT_TIMER_MSEC:
                return ConfigItem.CONFIG_ITEM_SIP_NON_INVITE_TXN_TIMEOUT_TIMER_MSEC;
            case ImsQmiIF.CONFIG_ITEM_SIP_INVITE_RSP_RETX_INTERVAL_MSEC:
                return ConfigItem.CONFIG_ITEM_SIP_INVITE_RSP_RETX_INTERVAL_MSEC;
            case ImsQmiIF.CONFIG_ITEM_SIP_ACK_RECEIPT_WAIT_TIME_MSEC:
                return ConfigItem.CONFIG_ITEM_SIP_ACK_RECEIPT_WAIT_TIME_MSEC;
            case ImsQmiIF.CONFIG_ITEM_SIP_ACK_RETX_WAIT_TIME_MSEC:
                return ConfigItem.CONFIG_ITEM_SIP_ACK_RETX_WAIT_TIME_MSEC;
            case ImsQmiIF.CONFIG_ITEM_SIP_NON_INVITE_REQ_RETX_WAIT_TIME_MSEC:
                return ConfigItem.CONFIG_ITEM_SIP_NON_INVITE_REQ_RETX_WAIT_TIME_MSEC;
            case ImsQmiIF.CONFIG_ITEM_SIP_NON_INVITE_RSP_RETX_WAIT_TIME_MSEC:
                return ConfigItem.CONFIG_ITEM_SIP_NON_INVITE_RSP_RETX_WAIT_TIME_MSEC;
            case ImsQmiIF.CONFIG_ITEM_AMR_WB_OCTET_ALIGNED_PT:
                return ConfigItem.CONFIG_ITEM_AMR_WB_OCTET_ALIGNED_PT;
            case ImsQmiIF.CONFIG_ITEM_AMR_WB_BANDWIDTH_EFFICIENT_PT:
                return ConfigItem.CONFIG_ITEM_AMR_WB_BANDWIDTH_EFFICIENT_PT;
            case ImsQmiIF.CONFIG_ITEM_AMR_OCTET_ALIGNED_PT:
                return ConfigItem.CONFIG_ITEM_AMR_OCTET_ALIGNED_PT;
            case ImsQmiIF.CONFIG_ITEM_AMR_BANDWIDTH_EFFICIENT_PT:
                return ConfigItem.CONFIG_ITEM_AMR_BANDWIDTH_EFFICIENT_PT;
            case ImsQmiIF.CONFIG_ITEM_DTMF_WB_PT:
                return ConfigItem.CONFIG_ITEM_DTMF_WB_PT;
            case ImsQmiIF.CONFIG_ITEM_DTMF_NB_PT:
                return ConfigItem.CONFIG_ITEM_DTMF_NB_PT;
            case ImsQmiIF.CONFIG_ITEM_AMR_DEFAULT_MODE:
                return ConfigItem.CONFIG_ITEM_AMR_DEFAULT_MODE;
            case ImsQmiIF.CONFIG_ITEM_SMS_PSI:
                return ConfigItem.CONFIG_ITEM_SMS_PSI;
            case ImsQmiIF.CONFIG_ITEM_VIDEO_QUALITY:
                return ConfigItem.CONFIG_ITEM_VIDEO_QUALITY;
            case ImsQmiIF.CONFIG_ITEM_THRESHOLD_LTE1:
                return ConfigItem.CONFIG_ITEM_THRESHOLD_LTE1;
            case ImsQmiIF.CONFIG_ITEM_THRESHOLD_LTE2:
                return ConfigItem.CONFIG_ITEM_THRESHOLD_LTE2;
            case ImsQmiIF.CONFIG_ITEM_THRESHOLD_LTE3:
                return ConfigItem.CONFIG_ITEM_THRESHOLD_LTE3;
            case ImsQmiIF.CONFIG_ITEM_THRESHOLD_1x:
                return ConfigItem.CONFIG_ITEM_THRESHOLD_1x;
            case ImsQmiIF.CONFIG_ITEM_THRESHOLD_WIFI_A:
                return ConfigItem.CONFIG_ITEM_THRESHOLD_WIFI_A;
            case ImsQmiIF.CONFIG_ITEM_THRESHOLD_WIFI_B:
                return ConfigItem.CONFIG_ITEM_THRESHOLD_WIFI_B;
            case ImsQmiIF.CONFIG_ITEM_T_EPDG_LTE:
                return ConfigItem.CONFIG_ITEM_T_EPDG_LTE;
            case ImsQmiIF.CONFIG_ITEM_T_EPDG_WIFI:
                return ConfigItem.CONFIG_ITEM_T_EPDG_WIFI;
            case ImsQmiIF.CONFIG_ITEM_T_EPDG_1x:
                return ConfigItem.CONFIG_ITEM_T_EPDG_1x;
            case ImsQmiIF.CONFIG_ITEM_VWF_SETTING_ENABLED:
                return ConfigItem.CONFIG_ITEM_VWF_SETTING_ENABLED;
            case ImsQmiIF.CONFIG_ITEM_VCE_SETTING_ENABLED:
                return ConfigItem.CONFIG_ITEM_VCE_SETTING_ENABLED;
            case ImsQmiIF.CONFIG_ITEM_SMS_APP:
                return ConfigItem.CONFIG_ITEM_SMS_APP;
            case ImsQmiIF.CONFIG_ITEM_VVM_APP:
                return ConfigItem.CONFIG_ITEM_VVM_APP;
            case ImsQmiIF.CONFIG_ITEM_RTT_SETTING_ENABLED:
                return ConfigItem.CONFIG_ITEM_RTT_SETTING_ENABLED;
            default:
                return ConfigItem.CONFIG_ITEM_INVALID;
        }
    }

    private static int configInfoItemFromHal(int item) {
        switch (item) {
            case ConfigItem.CONFIG_ITEM_NONE:
                return ImsQmiIF.CONFIG_ITEM_NONE;
            case ImsQmiIF.CONFIG_ITEM_VOCODER_AMRMODESET:
                return ImsQmiIF.CONFIG_ITEM_VOCODER_AMRMODESET;
            case ConfigItem.CONFIG_ITEM_VOCODER_AMRWBMODESET:
                return ImsQmiIF.CONFIG_ITEM_VOCODER_AMRWBMODESET;
            case ConfigItem.CONFIG_ITEM_SIP_SESSION_TIMER:
                return ImsQmiIF.CONFIG_ITEM_SIP_SESSION_TIMER;
            case ConfigItem.CONFIG_ITEM_MIN_SESSION_EXPIRY:
                return ImsQmiIF.CONFIG_ITEM_MIN_SESSION_EXPIRY;
            case ConfigItem.CONFIG_ITEM_CANCELLATION_TIMER:
                return ImsQmiIF.CONFIG_ITEM_CANCELLATION_TIMER;
            case ConfigItem.CONFIG_ITEM_T_DELAY:
                return ImsQmiIF.CONFIG_ITEM_T_DELAY;
            case ConfigItem.CONFIG_ITEM_SILENT_REDIAL_ENABLE:
                return ImsQmiIF.CONFIG_ITEM_SILENT_REDIAL_ENABLE;
            case ConfigItem.CONFIG_ITEM_SIP_T1_TIMER:
                return ImsQmiIF.CONFIG_ITEM_SIP_T1_TIMER;
            case ConfigItem.CONFIG_ITEM_SIP_T2_TIMER:
                return ImsQmiIF.CONFIG_ITEM_SIP_T2_TIMER;
            case ConfigItem.CONFIG_ITEM_SIP_TF_TIMER:
                return ImsQmiIF.CONFIG_ITEM_SIP_TF_TIMER;
            case ConfigItem.CONFIG_ITEM_VLT_SETTING_ENABLED:
                return ImsQmiIF.CONFIG_ITEM_VLT_SETTING_ENABLED;
            case ConfigItem.CONFIG_ITEM_LVC_SETTING_ENABLED:
                return ImsQmiIF.CONFIG_ITEM_LVC_SETTING_ENABLED;
            case ConfigItem.CONFIG_ITEM_DOMAIN_NAME:
                return ImsQmiIF.CONFIG_ITEM_DOMAIN_NAME;
            case ConfigItem.CONFIG_ITEM_SMS_FORMAT:
                return ImsQmiIF.CONFIG_ITEM_SMS_FORMAT;
            case ConfigItem.CONFIG_ITEM_SMS_OVER_IP:
                return ImsQmiIF.CONFIG_ITEM_SMS_OVER_IP;
            case ConfigItem.CONFIG_ITEM_PUBLISH_TIMER:
                return ImsQmiIF.CONFIG_ITEM_PUBLISH_TIMER;
            case ConfigItem.CONFIG_ITEM_PUBLISH_TIMER_EXTENDED:
                return ImsQmiIF.CONFIG_ITEM_PUBLISH_TIMER_EXTENDED;
            case ConfigItem.CONFIG_ITEM_CAPABILITIES_CACHE_EXPIRATION:
                return ImsQmiIF.CONFIG_ITEM_CAPABILITIES_CACHE_EXPIRATION;
            case ConfigItem.CONFIG_ITEM_AVAILABILITY_CACHE_EXPIRATION:
                return ImsQmiIF.CONFIG_ITEM_AVAILABILITY_CACHE_EXPIRATION;
            case ConfigItem.CONFIG_ITEM_CAPABILITIES_POLL_INTERVAL:
                return ImsQmiIF.CONFIG_ITEM_CAPABILITIES_POLL_INTERVAL;
            case ConfigItem.CONFIG_ITEM_SOURCE_THROTTLE_PUBLISH:
                return ImsQmiIF.CONFIG_ITEM_SOURCE_THROTTLE_PUBLISH;
            case ConfigItem.CONFIG_ITEM_MAX_NUM_ENTRIES_IN_RCL:
                return ImsQmiIF.CONFIG_ITEM_MAX_NUM_ENTRIES_IN_RCL;
            case ConfigItem.CONFIG_ITEM_CAPAB_POLL_LIST_SUB_EXP:
                return ImsQmiIF.CONFIG_ITEM_CAPAB_POLL_LIST_SUB_EXP;
            case ConfigItem.CONFIG_ITEM_GZIP_FLAG:
                return ImsQmiIF.CONFIG_ITEM_GZIP_FLAG;
            case ConfigItem.CONFIG_ITEM_EAB_SETTING_ENABLED:
                return ImsQmiIF.CONFIG_ITEM_EAB_SETTING_ENABLED;
            case ConfigItem.CONFIG_ITEM_MOBILE_DATA_ENABLED:
                return ImsQmiIF.CONFIG_ITEM_MOBILE_DATA_ENABLED;
            case ConfigItem.CONFIG_ITEM_VOICE_OVER_WIFI_ENABLED:
                return ImsQmiIF.CONFIG_ITEM_VOICE_OVER_WIFI_ENABLED;
            case ConfigItem.CONFIG_ITEM_VOICE_OVER_WIFI_ROAMING:
                return ImsQmiIF.CONFIG_ITEM_VOICE_OVER_WIFI_ROAMING;
            case ConfigItem.CONFIG_ITEM_VOICE_OVER_WIFI_MODE:
                return ImsQmiIF.CONFIG_ITEM_VOICE_OVER_WIFI_MODE;
            case ConfigItem.CONFIG_ITEM_CAPABILITY_DISCOVERY_ENABLED:
                return ImsQmiIF.CONFIG_ITEM_CAPABILITY_DISCOVERY_ENABLED;
            case ConfigItem.CONFIG_ITEM_EMERGENCY_CALL_TIMER:
                return ImsQmiIF.CONFIG_ITEM_EMERGENCY_CALL_TIMER;
            case ConfigItem.CONFIG_ITEM_SSAC_HYSTERESIS_TIMER:
                return ImsQmiIF.CONFIG_ITEM_SSAC_HYSTERESIS_TIMER;
            case ConfigItem.CONFIG_ITEM_VOLTE_USER_OPT_IN_STATUS:
                return ImsQmiIF.CONFIG_ITEM_VOLTE_USER_OPT_IN_STATUS;
            case ConfigItem.CONFIG_ITEM_LBO_PCSCF_ADDRESS:
                return ImsQmiIF.CONFIG_ITEM_LBO_PCSCF_ADDRESS;
            case ConfigItem.CONFIG_ITEM_KEEP_ALIVE_ENABLED:
                return ImsQmiIF.CONFIG_ITEM_KEEP_ALIVE_ENABLED;
            case ConfigItem.CONFIG_ITEM_REGISTRATION_RETRY_BASE_TIME_SEC:
                return ImsQmiIF.CONFIG_ITEM_REGISTRATION_RETRY_BASE_TIME_SEC;
            case ConfigItem.CONFIG_ITEM_REGISTRATION_RETRY_MAX_TIME_SEC:
                return ImsQmiIF.CONFIG_ITEM_REGISTRATION_RETRY_MAX_TIME_SEC;
            case ConfigItem.CONFIG_ITEM_SPEECH_START_PORT:
                return ImsQmiIF.CONFIG_ITEM_SPEECH_START_PORT;
            case ConfigItem.CONFIG_ITEM_SPEECH_END_PORT:
                return ImsQmiIF.CONFIG_ITEM_SPEECH_END_PORT;
            case ConfigItem.CONFIG_ITEM_SIP_INVITE_REQ_RETX_INTERVAL_MSEC:
                return ImsQmiIF.CONFIG_ITEM_SIP_INVITE_REQ_RETX_INTERVAL_MSEC;
            case ConfigItem.CONFIG_ITEM_SIP_INVITE_RSP_WAIT_TIME_MSEC:
                return ImsQmiIF.CONFIG_ITEM_SIP_INVITE_RSP_WAIT_TIME_MSEC;
            case ConfigItem.CONFIG_ITEM_SIP_INVITE_RSP_RETX_WAIT_TIME_MSEC:
                return ImsQmiIF.CONFIG_ITEM_SIP_INVITE_RSP_RETX_WAIT_TIME_MSEC;
            case ConfigItem.CONFIG_ITEM_SIP_NON_INVITE_REQ_RETX_INTERVAL_MSEC:
                return ImsQmiIF.CONFIG_ITEM_SIP_NON_INVITE_REQ_RETX_INTERVAL_MSEC;
            case ConfigItem.CONFIG_ITEM_SIP_NON_INVITE_TXN_TIMEOUT_TIMER_MSEC:
                return ImsQmiIF.CONFIG_ITEM_SIP_NON_INVITE_TXN_TIMEOUT_TIMER_MSEC;
            case ConfigItem.CONFIG_ITEM_SIP_INVITE_RSP_RETX_INTERVAL_MSEC:
                return ImsQmiIF.CONFIG_ITEM_SIP_INVITE_RSP_RETX_INTERVAL_MSEC;
            case ConfigItem.CONFIG_ITEM_SIP_ACK_RECEIPT_WAIT_TIME_MSEC:
                return ImsQmiIF.CONFIG_ITEM_SIP_ACK_RECEIPT_WAIT_TIME_MSEC;
            case ConfigItem.CONFIG_ITEM_SIP_ACK_RETX_WAIT_TIME_MSEC:
                return ImsQmiIF.CONFIG_ITEM_SIP_ACK_RETX_WAIT_TIME_MSEC;
            case ConfigItem.CONFIG_ITEM_SIP_NON_INVITE_REQ_RETX_WAIT_TIME_MSEC:
                return ImsQmiIF.CONFIG_ITEM_SIP_NON_INVITE_REQ_RETX_WAIT_TIME_MSEC;
            case ConfigItem.CONFIG_ITEM_SIP_NON_INVITE_RSP_RETX_WAIT_TIME_MSEC:
                return ImsQmiIF.CONFIG_ITEM_SIP_NON_INVITE_RSP_RETX_WAIT_TIME_MSEC;
            case ConfigItem.CONFIG_ITEM_AMR_WB_OCTET_ALIGNED_PT:
                return ImsQmiIF.CONFIG_ITEM_AMR_WB_OCTET_ALIGNED_PT;
            case ConfigItem.CONFIG_ITEM_AMR_WB_BANDWIDTH_EFFICIENT_PT:
                return ImsQmiIF.CONFIG_ITEM_AMR_WB_BANDWIDTH_EFFICIENT_PT;
            case ConfigItem.CONFIG_ITEM_AMR_OCTET_ALIGNED_PT:
                return ImsQmiIF.CONFIG_ITEM_AMR_OCTET_ALIGNED_PT;
            case ConfigItem.CONFIG_ITEM_AMR_BANDWIDTH_EFFICIENT_PT:
                return ImsQmiIF.CONFIG_ITEM_AMR_BANDWIDTH_EFFICIENT_PT;
            case ConfigItem.CONFIG_ITEM_DTMF_WB_PT:
                return ImsQmiIF.CONFIG_ITEM_DTMF_WB_PT;
            case ConfigItem.CONFIG_ITEM_DTMF_NB_PT:
                return ImsQmiIF.CONFIG_ITEM_DTMF_NB_PT;
            case ConfigItem.CONFIG_ITEM_AMR_DEFAULT_MODE:
                return ImsQmiIF.CONFIG_ITEM_AMR_DEFAULT_MODE;
            case ConfigItem.CONFIG_ITEM_SMS_PSI:
                return ImsQmiIF.CONFIG_ITEM_SMS_PSI;
            case ConfigItem.CONFIG_ITEM_VIDEO_QUALITY:
                return ImsQmiIF.CONFIG_ITEM_VIDEO_QUALITY;
            case ConfigItem.CONFIG_ITEM_THRESHOLD_LTE1:
                return ImsQmiIF.CONFIG_ITEM_THRESHOLD_LTE1;
            case ConfigItem.CONFIG_ITEM_THRESHOLD_LTE2:
                return ImsQmiIF.CONFIG_ITEM_THRESHOLD_LTE2;
            case ConfigItem.CONFIG_ITEM_THRESHOLD_LTE3:
                return ImsQmiIF.CONFIG_ITEM_THRESHOLD_LTE3;
            case ConfigItem.CONFIG_ITEM_THRESHOLD_1x:
                return ImsQmiIF.CONFIG_ITEM_THRESHOLD_1x;
            case ConfigItem.CONFIG_ITEM_THRESHOLD_WIFI_A:
                return ImsQmiIF.CONFIG_ITEM_THRESHOLD_WIFI_A;
            case ConfigItem.CONFIG_ITEM_THRESHOLD_WIFI_B:
                return ImsQmiIF.CONFIG_ITEM_THRESHOLD_WIFI_B;
            case ConfigItem.CONFIG_ITEM_T_EPDG_LTE:
                return ImsQmiIF.CONFIG_ITEM_T_EPDG_LTE;
            case ConfigItem.CONFIG_ITEM_T_EPDG_WIFI:
                return ImsQmiIF.CONFIG_ITEM_T_EPDG_WIFI;
            case ConfigItem.CONFIG_ITEM_T_EPDG_1x:
                return ImsQmiIF.CONFIG_ITEM_T_EPDG_1x;
            case ConfigItem.CONFIG_ITEM_VWF_SETTING_ENABLED:
                return ImsQmiIF.CONFIG_ITEM_VWF_SETTING_ENABLED;
            case ConfigItem.CONFIG_ITEM_VCE_SETTING_ENABLED:
                return ImsQmiIF.CONFIG_ITEM_VCE_SETTING_ENABLED;
            case ConfigItem.CONFIG_ITEM_SMS_APP:
                return ImsQmiIF.CONFIG_ITEM_SMS_APP;
            case ConfigItem.CONFIG_ITEM_VVM_APP:
                return ImsQmiIF.CONFIG_ITEM_VVM_APP;
            default:
                return ImsQmiIF.CONFIG_ITEM_NONE;
        }
    }

    private static int configFailureCauseToHal(int errorCause) {
        switch (errorCause) {
            case ImsQmiIF.CONFIG_NO_ERR:
                return ConfigFailureCause.CONFIG_NO_ERR;
            case ImsQmiIF.CONFIG_IMS_NOT_READY:
                return ConfigFailureCause.CONFIG_IMS_NOT_READY;
            case ImsQmiIF.CONFIG_FILE_NOT_AVAILABLE:
                return ConfigFailureCause.CONFIG_FILE_NOT_AVAILABLE;
            case ImsQmiIF.CONFIG_READ_FAILED:
                return ConfigFailureCause.CONFIG_READ_FAILED;
            case ImsQmiIF.CONFIG_WRITE_FAILED:
                return ConfigFailureCause.CONFIG_WRITE_FAILED;
            case ImsQmiIF.CONFIG_OTHER_INTERNAL_ERR:
                return ConfigFailureCause.CONFIG_OTHER_INTERNAL_ERR;
            default:
                return ConfigFailureCause.CONFIG_FAILURE_INVALID;
        }
    }

    public static int configFailureCauseFromHal(int errorCause) {
        switch (errorCause) {
            case ConfigFailureCause.CONFIG_NO_ERR:
                return ImsQmiIF.CONFIG_NO_ERR;
            case ConfigFailureCause.CONFIG_IMS_NOT_READY:
                return ImsQmiIF.CONFIG_IMS_NOT_READY;
            case ConfigFailureCause.CONFIG_FILE_NOT_AVAILABLE:
                return ImsQmiIF.CONFIG_FILE_NOT_AVAILABLE;
            case ConfigFailureCause.CONFIG_READ_FAILED:
                return ImsQmiIF.CONFIG_READ_FAILED;
            case ConfigFailureCause.CONFIG_WRITE_FAILED:
                return ImsQmiIF.CONFIG_WRITE_FAILED;
            case ConfigFailureCause.CONFIG_OTHER_INTERNAL_ERR:
            default:
                return ImsQmiIF.CONFIG_OTHER_INTERNAL_ERR;
        }
    }

    public static CbNumListInfo buildCbNumListInfo(String[] inCbNumList, int serviceClass) {
        CbNumListInfo outCbNumListInfo = new CbNumListInfo();

        if (serviceClass != Integer.MAX_VALUE) {
            outCbNumListInfo.serviceClass = serviceClass;
        }

        if (inCbNumList != null) {
            for (int i = 0; i < inCbNumList.length; ++i) {
                CbNumInfo cbNumInfo = new CbNumInfo();
                cbNumInfo.status = ServiceClassStatus.INVALID;
                if (inCbNumList[i] != null) {
                    cbNumInfo.number = inCbNumList[i];
                }
                outCbNumListInfo.cbNumInfo.add(cbNumInfo);
            }
        }

        return outCbNumListInfo;
    }

    public static int messageTypeFromHal(int type) {
        switch (type) {
            case MessageType.MWI_MSG_VOICE:
                return ImsQmiIF.MWI_MSG_VOICE;
            case MessageType.MWI_MSG_VIDEO:
                return ImsQmiIF.MWI_MSG_VIDEO;
            case MessageType.MWI_MSG_FAX:
                return ImsQmiIF.MWI_MSG_FAX;
            case MessageType.MWI_MSG_PAGER:
                return ImsQmiIF.MWI_MSG_PAGER;
            case MessageType.MWI_MSG_MULTIMEDIA:
                return ImsQmiIF.MWI_MSG_MULTIMEDIA;
            case MessageType.MWI_MSG_TEXT:
                return ImsQmiIF.MWI_MSG_TEXT;
            case MessageType.MWI_MSG_NONE:
            default:
                return ImsQmiIF.MWI_MSG_NONE;
        }
    }

    public static int messagePriorityFromHal(int type) {
        switch (type) {
            case MessagePriority.MWI_MSG_PRIORITY_LOW:
                return ImsQmiIF.MWI_MSG_PRIORITY_LOW;
            case MessagePriority.MWI_MSG_PRIORITY_NORMAL:
                return ImsQmiIF.MWI_MSG_PRIORITY_NORMAL;
            case MessagePriority.MWI_MSG_PRIORITY_URGENT:
                return ImsQmiIF.MWI_MSG_PRIORITY_URGENT;
            case MessagePriority.MWI_MSG_PRIORITY_UNKNOWN:
            default:
                return ImsQmiIF.MWI_MSG_PRIORITY_UNKNOWN;
        }
    }

    public static ImsQmiIF.Mwi messageWaitingIndicationFromHal(MessageWaitingIndication
            messageWaitingIndication) {
        if (messageWaitingIndication == null) {
            return null;
        }

        ImsQmiIF.Mwi mwi = new ImsQmiIF.Mwi();
        for (int i = 0; i < messageWaitingIndication.messageSummary.size(); i++) {
            if (messageWaitingIndication.messageSummary.get(i) != null) {
                mwi.addMwiMsgSummary(messageSummaryFromHal(
                        messageWaitingIndication.messageSummary.get(i)));
            }
        }
        if (!messageWaitingIndication.ueAddress.isEmpty()) {
            mwi.setUeAddress(messageWaitingIndication.ueAddress);
        }
        for (int i = 0; i < messageWaitingIndication.messageDetails.size(); i++) {
            if (messageWaitingIndication.messageDetails.get(i) != null) {
                mwi.addMwiMsgDetail(messageDetailsFromHal(
                        messageWaitingIndication.messageDetails.get(i)));
            }
        }
        return mwi;
    }

    private static ImsQmiIF.MwiMessageSummary messageSummaryFromHal(MessageSummary summary) {
        if (summary == null) {
            return null;
        }

        ImsQmiIF.MwiMessageSummary mwiMessageSummary = new ImsQmiIF.MwiMessageSummary();

        if (summary.type != MessageType.MWI_MSG_INVALID) {
            mwiMessageSummary.setMessageType(messageTypeFromHal(summary.type));
        }

        if (summary.newMessageCount != Integer.MAX_VALUE) {
            mwiMessageSummary.setNewMessage(summary.newMessageCount);
        }

        if (summary.oldMessageCount != Integer.MAX_VALUE) {
            mwiMessageSummary.setOldMessage(summary.oldMessageCount);
        }

        if (summary.newUrgentMessageCount != Integer.MAX_VALUE) {
            mwiMessageSummary.setNewUrgent(summary.newUrgentMessageCount);
        }

        if (summary.oldUrgentMessageCount != Integer.MAX_VALUE) {
            mwiMessageSummary.setOldUrgent(summary.oldUrgentMessageCount);
        }
        return mwiMessageSummary;
    }

    private static ImsQmiIF.MwiMessageDetails messageDetailsFromHal(MessageDetails details) {
        if (details == null) {
            return null;
        }

        ImsQmiIF.MwiMessageDetails mwiMessageDetails = new ImsQmiIF.MwiMessageDetails();

        if (!details.toAddress.isEmpty()) {
            mwiMessageDetails.setToAddress(details.toAddress);
        }

        if (!details.fromAddress.isEmpty()) {
            mwiMessageDetails.setFromAddress(details.fromAddress);
        }

        if (!details.subject.isEmpty()) {
            mwiMessageDetails.setSubject(details.subject);
        }

        if (!details.date.isEmpty()) {
            mwiMessageDetails.setDate(details.date);
        }

        if (details.priority != MessagePriority.MWI_MSG_PRIORITY_INVALID) {
            mwiMessageDetails.setPriority(messagePriorityFromHal(details.priority));
        }

        if (!details.id.isEmpty()) {
            mwiMessageDetails.setMessageId(details.id);
        }

        if (details.type != MessageType.MWI_MSG_INVALID) {
            mwiMessageDetails.setMessageType(messageTypeFromHal(details.type));
        }
        return mwiMessageDetails;
    }

    public static ExplicitCallTransferInfo buildExplicitCallTransferInfo(int srcCallId, int type,
        String number, int destCallId) {
        ExplicitCallTransferInfo ectInfo = new ExplicitCallTransferInfo();
        ectInfo.callId = srcCallId;
        ectInfo.ectType = ectTypeToHal(type);
        if (number != null) {
            ectInfo.targetAddress = number;
        }
        if (destCallId > 0) {
            ectInfo.targetCallId = destCallId;
        } else {
            ectInfo.targetCallId = Integer.MAX_VALUE;
        }
        return ectInfo;
    }

    private static int ectTypeToHal(int type) {
        switch (type) {
            case ImsQmiIF.BLIND_TRANSFER:
                return EctType.BLIND_TRANSFER;
            case ImsQmiIF.ASSURED_TRANSFER:
                return EctType.ASSURED_TRANSFER;
            case ImsQmiIF.CONSULTATIVE_TRANSFER:
                return EctType.CONSULTATIVE_TRANSFER;
            default:
                return EctType.INVALID;
        }
    }

    public static ImsQmiIF.GeoLocationInfo
            geolocationIndicationFromHal(double lat, double lon) {
        ImsQmiIF.GeoLocationInfo geoInfo = new ImsQmiIF.GeoLocationInfo();
        geoInfo.setLat(lat);
        geoInfo.setLon(lon);
        // Note: The UNSOL from lower layers is actually a 'query' that only
        // has the location coordinates. Address is not required.
        return geoInfo;
    }

    public static ImsQmiIF.ImsSubConfig imsSubconfigFromHal(ImsSubConfigInfo subConfigInfo) {

        if (subConfigInfo == null) {
            return null;
        }

        ImsQmiIF.ImsSubConfig subConfig = new ImsQmiIF.ImsSubConfig();
        if (subConfigInfo.simultStackCount != Integer.MAX_VALUE) {
            subConfig.setSimultStackCount(subConfigInfo.simultStackCount);
        }

        for (int i = 0; i < subConfigInfo.imsStackEnabled.size(); i++) {
            subConfig.addImsStackEnabled(subConfigInfo.imsStackEnabled.get(i));
        }

        return subConfig;
    }

    public static int operationFromHal(int type) {
        switch (type) {
            case ConfParticipantOperation.ADD:
                return ImsQmiIF.ADD;
            case ConfParticipantOperation.REMOVE:
                return ImsQmiIF.REMOVE;
            default:
                return ConfParticipantOperation.INVALID;
        }
    }

    public static ImsQmiIF.ParticipantStatusInfo participantStatusFromHal(
            ParticipantStatusInfo participantStatusInfo) {

        if (participantStatusInfo == null) {
            return null;
        }

        ImsQmiIF.ParticipantStatusInfo participantStatus = new ImsQmiIF.ParticipantStatusInfo();

        if (participantStatusInfo.callId != Integer.MAX_VALUE) {
            participantStatus.setCallId(participantStatusInfo.callId);
        }

        if (participantStatusInfo.operation != ConfParticipantOperation.INVALID) {
            participantStatus.setOperation(operationFromHal(participantStatusInfo.operation));
        }

        participantStatus.setParticipantUri(participantStatusInfo.participantUri);

        if (participantStatusInfo.sipStatus != Integer.MAX_VALUE) {
            participantStatus.setSipStatus(participantStatusInfo.sipStatus);
        }

        if (participantStatusInfo.hasIsEct) {
            participantStatus.setIsEct(participantStatusInfo.isEct);
        }

        return participantStatus;
    }

    public static ImsQmiIF.RegistrationBlockStatus registrationBlockStatusFromHal(
            boolean hasBlockStatusOnWwan, BlockStatus blockStatusOnWwan,
            boolean hasBlockStatusOnWlan, BlockStatus blockStatusOnWlan) {
        ImsQmiIF.RegistrationBlockStatus regBlockStatus = new ImsQmiIF.RegistrationBlockStatus();

        if (hasBlockStatusOnWwan) {
            regBlockStatus.setImsBlockStatusOnWwan(blockStatusFromHal(blockStatusOnWwan));
        }

        if (hasBlockStatusOnWlan) {
            regBlockStatus.setImsBlockStatusOnWlan(blockStatusFromHal(blockStatusOnWlan));
        }

        return regBlockStatus;
    }

    public static ImsQmiIF.BlockStatus blockStatusFromHal(BlockStatus inBlockStatus) {
        if (inBlockStatus == null) {
            return null;
        }

        ImsQmiIF.BlockStatus outBlockStatus = new ImsQmiIF.BlockStatus();

        if (inBlockStatus.blockReason != BlockReasonType.INVALID) {
            outBlockStatus.setBlockReason(blockReasonTypeFromHal(inBlockStatus.blockReason));
        }

        if (inBlockStatus.hasBlockReasonDetails) {
            outBlockStatus.setBlockReasonDetails(blockReasonDetailsFromHal(
                    inBlockStatus.blockReasonDetails));
        }

        return outBlockStatus;
    }

    public static int blockReasonTypeFromHal(int inBlockReason) {
        switch (inBlockReason) {
            case BlockReasonType.PDP_FAILURE:
                return ImsQmiIF.BLOCK_REASON_PDP_FAILURE;
            case BlockReasonType.REGISTRATION_FAILURE:
                return ImsQmiIF.BLOCK_REASON_REGISTRATION_FAILURE;
            case BlockReasonType.HANDOVER_FAILURE:
                return ImsQmiIF.BLOCK_REASON_HANDOVER_FAILURE;
            case BlockReasonType.OTHER_FAILURE:
            case BlockReasonType.INVALID:
            default:
                return ImsQmiIF.BLOCK_REASON_OTHER_FAILURE;
        }
    }

    public static ImsQmiIF.BlockReasonDetails blockReasonDetailsFromHal(
            BlockReasonDetails inBlockReasonDetails) {
        ImsQmiIF.BlockReasonDetails outBlockReasonDetails = new ImsQmiIF.BlockReasonDetails();

        if (inBlockReasonDetails.regFailureReasonType != RegFailureReasonType.INVALID) {
            outBlockReasonDetails.setRegFailureReasonType(regFailureReasonTypeFromHal(
                    inBlockReasonDetails.regFailureReasonType));
        }

        if (inBlockReasonDetails.regFailureReason != Integer.MAX_VALUE) {
            outBlockReasonDetails.setRegFailureReason(inBlockReasonDetails.regFailureReason);
        }

        return outBlockReasonDetails;
    }

    public static int regFailureReasonTypeFromHal(int inRegFailureReasonType) {
        switch (inRegFailureReasonType) {
            case RegFailureReasonType.MOBILE_IP:
                return ImsQmiIF.REG_FAILURE_REASON_TYPE_MOBILE_IP;
            case RegFailureReasonType.INTERNAL:
                return ImsQmiIF.REG_FAILURE_REASON_TYPE_INTERNAL;
            case RegFailureReasonType.CALL_MANAGER_DEFINED:
                return ImsQmiIF.REG_FAILURE_REASON_TYPE_CALL_MANAGER_DEFINED;
            case RegFailureReasonType.TYPE_3GPP_SPEC_DEFINED:
                return ImsQmiIF.REG_FAILURE_REASON_TYPE_3GPP_SPEC_DEFINED;
            case RegFailureReasonType.PPP:
                return ImsQmiIF.REG_FAILURE_REASON_TYPE_PPP;
            case RegFailureReasonType.EHRPD:
                return ImsQmiIF.REG_FAILURE_REASON_TYPE_EHRPD;
            case RegFailureReasonType.IPV6:
                return ImsQmiIF.REG_FAILURE_REASON_TYPE_IPV6;
            case RegFailureReasonType.IWLAN:
                return ImsQmiIF.REG_FAILURE_REASON_TYPE_IWLAN;
            case RegFailureReasonType.HANDOFF:
                return ImsQmiIF.REG_FAILURE_REASON_TYPE_HANDOFF;
            case RegFailureReasonType.UNSPECIFIED:
            case RegFailureReasonType.INVALID:
            default:
                return ImsQmiIF.REG_FAILURE_REASON_TYPE_UNSPECIFIED;
        }
    }

    public static Object voWiFiCallQualityFromHal(int voWiFiCallQuality) {
        int[] ret = new int[1];
        switch (voWiFiCallQuality) {
            case VoWiFiCallQualityInfo.VOWIFI_CALL_QUALITY_EXCELLENT:
                ret[0] = ImsQmiIF.VOWIFI_QUALITY_EXCELLENT;
                break;
            case VoWiFiCallQualityInfo.VOWIFI_CALL_QUALITY_FAIR:
                ret[0] = ImsQmiIF.VOWIFI_QUALITY_FAIR;
                break;
            case VoWiFiCallQualityInfo.VOWIFI_CALL_QUALITY_BAD:
                ret[0] = ImsQmiIF.VOWIFI_QUALITY_BAD;
                break;
            default:
                ret[0] = ImsQmiIF.VOWIFI_QUALITY_NONE;
                break;
        }
        return ret;
    }

    public static int serviceTypeFromRILServiceType(int serviceType) {
        switch (serviceType) {
            case SsServiceType.SS_CFU:
                return ImsSsData.SS_CFU;
            case SsServiceType.SS_CF_BUSY:
                return ImsSsData.SS_CF_BUSY;
            case SsServiceType.SS_CF_NO_REPLY:
                return ImsSsData.SS_CF_NO_REPLY;
            case SsServiceType.SS_CF_NOT_REACHABLE:
                return ImsSsData.SS_CF_NOT_REACHABLE;
            case SsServiceType.SS_CF_ALL:
                return ImsSsData.SS_CF_ALL;
            case SsServiceType.SS_CF_ALL_CONDITIONAL:
                return ImsSsData.SS_CF_ALL_CONDITIONAL;
            case SsServiceType.SS_CFUT:
                return ImsSsData.SS_CFUT;
            case SsServiceType.SS_CLIP:
                return ImsSsData.SS_CLIP;
            case SsServiceType.SS_CLIR:
                return ImsSsData.SS_CLIR;
            case SsServiceType.SS_COLP:
                return ImsSsData.SS_COLP;
            case SsServiceType.SS_COLR:
                return ImsSsData.SS_COLR;
            case SsServiceType.SS_CNAP:
                return ImsSsData.SS_CNAP;
            case SsServiceType.SS_WAIT:
                return ImsSsData.SS_WAIT;
            case SsServiceType.SS_BAOC:
                return ImsSsData.SS_BAOC;
            case SsServiceType.SS_BAOIC:
                return ImsSsData.SS_BAOIC;
            case SsServiceType.SS_BAOIC_EXC_HOME:
                return ImsSsData.SS_BAOIC_EXC_HOME;
            case SsServiceType.SS_BAIC:
                return ImsSsData.SS_BAIC;
            case SsServiceType.SS_BAIC_ROAMING:
                return ImsSsData.SS_BAIC_ROAMING;
            case SsServiceType.SS_ALL_BARRING:
                return ImsSsData.SS_ALL_BARRING;
            case SsServiceType.SS_OUTGOING_BARRING:
                return ImsSsData.SS_OUTGOING_BARRING;
            case SsServiceType.SS_INCOMING_BARRING:
                return ImsSsData.SS_INCOMING_BARRING;
            case SsServiceType.SS_INCOMING_BARRING_DN:
                return ImsSsData.SS_INCOMING_BARRING_DN;
            case SsServiceType.SS_INCOMING_BARRING_ANONYMOUS:
                return ImsSsData.SS_INCOMING_BARRING_ANONYMOUS;
            default:
        }
        return ImsUtInterface.INVALID;
    }

    public static int requestTypeFromRILRequestType(int requestType) {
        switch (requestType) {
            case SsRequestType.SS_ACTIVATION:
                return ImsSsData.SS_ACTIVATION;
            case SsRequestType.SS_DEACTIVATION:
                return ImsSsData.SS_DEACTIVATION;
            case SsRequestType.SS_INTERROGATION:
                return ImsSsData.SS_INTERROGATION;
            case SsRequestType.SS_REGISTRATION:
                return ImsSsData.SS_REGISTRATION;
            case SsRequestType.SS_ERASURE:
                return ImsSsData.SS_ERASURE;
            default:
        }
        return ImsUtInterface.INVALID;
    }

    public static int teleserviceTypeFromRILTeleserviceType(int teleservice) {
        switch (teleservice) {
            case SsTeleserviceType.SS_ALL_TELE_AND_BEARER_SERVICES:
                return ImsSsData.SS_ALL_TELE_AND_BEARER_SERVICES;
            case SsTeleserviceType.SS_ALL_TELESEVICES:
                return ImsSsData.SS_ALL_TELESEVICES;
            case SsTeleserviceType.SS_TELEPHONY:
                return ImsSsData.SS_TELEPHONY;
            case SsTeleserviceType.SS_ALL_DATA_TELESERVICES:
                return ImsSsData.SS_ALL_DATA_TELESERVICES;
            case SsTeleserviceType.SS_SMS_SERVICES:
                return ImsSsData.SS_SMS_SERVICES;
            case SsTeleserviceType.SS_ALL_TELESERVICES_EXCEPT_SMS:
                return ImsSsData.SS_ALL_TELESERVICES_EXCEPT_SMS;
            default:
        }
        return ImsUtInterface.INVALID;
    }

    public static int getUTInterfaceCFReasonFromCommandsInterfaceCFReason(
            int commandsInterfaceCFReason) {
        switch(commandsInterfaceCFReason) {
            case ImsPhoneCommandsInterface.CF_REASON_UNCONDITIONAL:
                return ImsUtInterface.CDIV_CF_UNCONDITIONAL;
            case ImsPhoneCommandsInterface.CF_REASON_BUSY:
                return ImsUtInterface.CDIV_CF_BUSY;
            case ImsPhoneCommandsInterface.CF_REASON_NO_REPLY:
                return ImsUtInterface.CDIV_CF_NO_REPLY;
            case ImsPhoneCommandsInterface.CF_REASON_NOT_REACHABLE:
                return ImsUtInterface.CDIV_CF_NOT_REACHABLE;
            case ImsPhoneCommandsInterface.CF_REASON_ALL:
                return ImsUtInterface.CDIV_CF_ALL;
            case ImsPhoneCommandsInterface.CF_REASON_ALL_CONDITIONAL:
                return ImsUtInterface.CDIV_CF_ALL_CONDITIONAL;
            default:
                break;
        }
        return ImsUtInterface.INVALID;
    }
}
