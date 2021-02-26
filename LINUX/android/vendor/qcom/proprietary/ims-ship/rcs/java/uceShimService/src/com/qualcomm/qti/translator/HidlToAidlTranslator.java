/*********************************************************************
 Copyright (c) 2017 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
**********************************************************************/

package com.qualcomm.qti.translator;


import java.util.ArrayList;
import java.util.Iterator;

import com.android.ims.internal.uce.common.StatusCode;
import com.android.ims.internal.uce.common.CapInfo;

import com.android.ims.internal.uce.presence.*;
import com.android.ims.internal.uce.options.OptionsCmdId;
import com.android.ims.internal.uce.options.OptionsSipResponse;
import com.android.ims.internal.uce.options.OptionsCapInfo;
import com.android.ims.internal.uce.options.OptionsCmdStatus;


import com.qualcomm.qti.uceservice.V1_0.PresenceCmdId;
import com.qualcomm.qti.uceservice.V1_0.ResInstantceState;
import com.qualcomm.qti.uceservice.V1_0.UceStatusCode;

import android.util.Log;
import static java.lang.Math.toIntExact;

public class HidlToAidlTranslator
{
    private final static String logTAG = "HIDL to AIDL Translator";

/* Translator method common to PresListener and OptionsListener class  */

//PresListener : serviceAvailable & serviceUnavailable use below method
//Ex : m_statusCode.setStatusCode(hidlToAidlObj.getAidlStatusCode(status));
public int getAidlStatusCode(int statusCode) {
    int retAidlStatusCode = getStatusValue(statusCode);
    return retAidlStatusCode;
}

/* Translator methods pertaining to PresListener class */

//Ex: m_publishTrigger.setPublishTrigeerType(hidlToAidlObj.getAidlPresenceTriggerTypeValue(publishTrigger));
public int getAidlPresenceTriggerTypeValue(int publishTrigger) {
    int retAidlPresTriggerType = getPresenceTriggerTypeValue(publishTrigger);
    return retAidlPresTriggerType;
}

//Ex: m_pCmdStatus = hidlToAidlObj.getAidlPresCmdStatus(pCmdStatus);
public PresCmdStatus getAidlPresCmdStatus(com.qualcomm.qti.uceservice.V1_0.PresCmdStatus
                                          pCmdStatus) {
    PresCmdStatus aidlPresCmdStatus = new PresCmdStatus();
    PresCmdId aidlPresCmdId = new PresCmdId();
    StatusCode aidlStatusCode = new StatusCode();
    // Set Cmd Id
    aidlPresCmdId.setCmdId(getPresCmdIdValue(pCmdStatus.mCmdId));
    aidlPresCmdStatus.setCmdId(aidlPresCmdId);
    //Set User data
    aidlPresCmdStatus.setUserData((int)pCmdStatus.mUserData);
    //Set Status Code
    aidlStatusCode.setStatusCode(getStatusValue(pCmdStatus.mStatus));
    aidlPresCmdStatus.setStatus(aidlStatusCode);
    //Set Request Id
    aidlPresCmdStatus.setRequestId(pCmdStatus.mRequestId);

    return aidlPresCmdStatus;
}

//Ex: m_pSipResponse = hidlToAidlObj.getAidlPresSipResponse(pSipResponse);
public PresSipResponse getAidlPresSipResponse(com.qualcomm.qti.uceservice.V1_0.PresSipResponse
                                              pSipResponse) {
    PresSipResponse aidlPresSipResponse = new PresSipResponse();
    PresCmdId aidlPresCmdId = new PresCmdId();
    aidlPresCmdId.setCmdId(getPresCmdIdValue(pSipResponse.mCmdId));
    aidlPresSipResponse.setCmdId(aidlPresCmdId);
    aidlPresSipResponse.setRequestId(pSipResponse.mRequestId);
    aidlPresSipResponse.setSipResponseCode(pSipResponse.mSipResponseCode);
    aidlPresSipResponse.setRetryAfter(pSipResponse.mRetryAfter);
    aidlPresSipResponse.setReasonPhrase(pSipResponse.mReasonPhrase);

    return aidlPresSipResponse;
}

//Ex: m_pTupleInfo = hidlToAidlObj.getAidlTupleInfoArray(pTupleInfoArray);
//mAidlPresenceListener.capInfoReceived(m_pPresentityURI, m_pTupleInfo);
public PresTupleInfo[] getAidlTupleInfoArray(ArrayList<com.qualcomm.qti.uceservice.V1_0.PresTupleInfo>
                                           pTupleInfoArray) {
    Iterator i = pTupleInfoArray.iterator();
    int sizeofTupleArray = pTupleInfoArray.size();
    PresTupleInfo aidlTupleInfo [] = new PresTupleInfo[sizeofTupleArray];

    int j = 0;
    while(i.hasNext()) {
        com.qualcomm.qti.uceservice.V1_0.PresTupleInfo pHidlTupleInfo =
            (com.qualcomm.qti.uceservice.V1_0.PresTupleInfo)i.next();
        com.android.ims.internal.uce.presence.PresTupleInfo tupleInfoObj = new
            com.android.ims.internal.uce.presence.PresTupleInfo();
        tupleInfoObj.setFeatureTag(pHidlTupleInfo.mFeatureTag);
        tupleInfoObj.setContactUri(pHidlTupleInfo.mContactUri);
        tupleInfoObj.setTimestamp(pHidlTupleInfo.mTimestamp);
        aidlTupleInfo[j] = tupleInfoObj;
        j++;
    }

    return aidlTupleInfo;
}

//Ex: m_pRlmiInfo = hidlToAidlObj.getAidlPresRlmiInfo(pRlmiInfo);
public PresRlmiInfo getAidlPresRlmiInfo(com.qualcomm.qti.uceservice.V1_0.PresRlmiInfo
                                        pRlmiInfo) {
    PresRlmiInfo aidlPresRlmiInfo = new PresRlmiInfo();
    PresSubscriptionState aidlPresSubscriptionState = new PresSubscriptionState();

    aidlPresRlmiInfo.setUri(pRlmiInfo.mUri);
    aidlPresRlmiInfo.setVersion(pRlmiInfo.mVersion);
    aidlPresRlmiInfo.setFullState(pRlmiInfo.mFullState);
    aidlPresRlmiInfo.setListName(pRlmiInfo.mListname);
    aidlPresRlmiInfo.setRequestId(pRlmiInfo.mRequestId);
    aidlPresSubscriptionState.setPresSubscriptionState(
        getPresSubscriptionState(pRlmiInfo.mPresSubscriptionState));
    aidlPresRlmiInfo.setPresSubscriptionState(aidlPresSubscriptionState);
    aidlPresRlmiInfo.setSubscriptionExpireTime(pRlmiInfo.mSubscriptionExpireTime);
    aidlPresRlmiInfo.setSubscriptionTerminatedReason(pRlmiInfo.mSubscriptionTerminatedReason);

    return aidlPresRlmiInfo;
}

//Ex: m_pResInfo = hidlToAidlObj.getAidlPresResInfo(pResInfoArray);
public PresResInfo[] getAidlPresResInfo(ArrayList<com.qualcomm.qti.uceservice.V1_0.PresResInfo>
                                      pResInfoArray) {
    Iterator i = pResInfoArray.iterator();
    int sizeOfResInfoArray = pResInfoArray.size();
    PresResInfo aidlPresResInfo[] = new PresResInfo[sizeOfResInfoArray];
    int j = 0;

    while(i.hasNext()) {
        com.qualcomm.qti.uceservice.V1_0.PresResInfo hidlPresResInfo =
            (com.qualcomm.qti.uceservice.V1_0.PresResInfo)i.next();
        PresResInfo resInfoObj = new PresResInfo();
        resInfoObj.setResUri(hidlPresResInfo.mResUri);
        resInfoObj.setDisplayName(hidlPresResInfo.mDisplayName);
        PresResInstanceInfo aidlPresResInstanceInfo = new PresResInstanceInfo();
        aidlPresResInstanceInfo.setResInstanceState(getResInstanceStateValue(hidlPresResInfo.mInstanceInfo.mResInstanceState));
        aidlPresResInstanceInfo.setResId(hidlPresResInfo.mInstanceInfo.mId);
        aidlPresResInstanceInfo.setReason(hidlPresResInfo.mInstanceInfo.mReason);
        aidlPresResInstanceInfo.setPresentityUri(hidlPresResInfo.mInstanceInfo.mPresentityUri);

        int sizeOfTupleInfoArray = hidlPresResInfo.mInstanceInfo.mTupleInfoArray.size();
        PresTupleInfo aidlPresTupleInfo[] = new PresTupleInfo[sizeOfTupleInfoArray];
        aidlPresTupleInfo = getAidlTupleInfoArray(hidlPresResInfo.mInstanceInfo.mTupleInfoArray);
        aidlPresResInstanceInfo.setTupleInfo(aidlPresTupleInfo);

        resInfoObj.setInstanceInfo(aidlPresResInstanceInfo);
        aidlPresResInfo[j] = resInfoObj;
        j++;
    }

    return aidlPresResInfo;
}

/* Translator methods pertaining to OptionsListener class */

public OptionsSipResponse getAidlOptionsSipResponse(com.qualcomm.qti.uceservice.V1_0.OptionsSipResponse
                                                    hidlOptionsSipResponse) {
    OptionsSipResponse aidlOptionsSipResponse = new OptionsSipResponse();

    OptionsCmdId aidlOptionsCmdId = new OptionsCmdId();
    aidlOptionsCmdId.setCmdId(getOptionsCmdIdValue(hidlOptionsSipResponse.mCmdId));

    aidlOptionsSipResponse.setCmdId(aidlOptionsCmdId);
    aidlOptionsSipResponse.setRequestId(hidlOptionsSipResponse.mRequestId);
    aidlOptionsSipResponse.setSipResponseCode(toIntExact(hidlOptionsSipResponse.mSipResponseCode));
    aidlOptionsSipResponse.setReasonPhrase(hidlOptionsSipResponse.mReasonPhrase);
    aidlOptionsSipResponse.setRetryAfter(toIntExact(hidlOptionsSipResponse.mRetryAfter));

    return aidlOptionsSipResponse;
}

public OptionsCapInfo getAidlOptionsCapInfo(com.qualcomm.qti.uceservice.V1_0.OptionsCapabilityInfo
                                            hidlCapInfo) {
    OptionsCapInfo aidlOptionsCapInfo = new OptionsCapInfo();
    aidlOptionsCapInfo.setSdp(hidlCapInfo.sdp);

    CapInfo aidlCapInfo = new CapInfo();
    aidlCapInfo = getAidlCapInfo(hidlCapInfo.capInfo);

    aidlOptionsCapInfo.setCapInfo(aidlCapInfo);

    return aidlOptionsCapInfo;
}

public OptionsCmdStatus getAidlOptionsCmdStatus(com.qualcomm.qti.uceservice.V1_0.OptionsCmdStatus hidlOptionsCmdStatus) {
    OptionsCmdStatus aidlOptionsCMDStatus = new OptionsCmdStatus();
    OptionsCmdId aidlOptionsCmdId = new OptionsCmdId();
    CapInfo aidlCapInfo = new CapInfo();
    StatusCode aidlStatusCode = new StatusCode();
    aidlOptionsCmdId.setCmdId(getOptionsCmdIdValue(hidlOptionsCmdStatus.mCmdId));
    aidlOptionsCMDStatus.setCmdId(aidlOptionsCmdId);
    aidlOptionsCMDStatus.setUserData(toIntExact(hidlOptionsCmdStatus.mUserData));
    aidlStatusCode.setStatusCode(getStatusValue(hidlOptionsCmdStatus.mStatus));
    aidlOptionsCMDStatus.setStatus(aidlStatusCode);
    aidlCapInfo = getAidlCapInfo(hidlOptionsCmdStatus.mCapInfo.capInfo);
    aidlOptionsCMDStatus.setCapInfo(aidlCapInfo);

    return aidlOptionsCMDStatus;
}

/* Utility Functions to map HIDL enum value to AIDL enum value */
 private int getStatusValue(int statusCode) {
        int retStatusCode = StatusCode.UCE_SERVICE_UNKNOWN;
        switch(statusCode) {
        case UceStatusCode.UCE_SUCCESS:
            retStatusCode = StatusCode.UCE_SUCCESS;
            break;

        case UceStatusCode.UCE_FAILURE:
            retStatusCode = StatusCode.UCE_FAILURE;
            break;

        case UceStatusCode.UCE_SUCCESS_ASYNC_UPDATE:
            retStatusCode = StatusCode.UCE_SUCCESS_ASYC_UPDATE;
            break;

        case UceStatusCode.UCE_INVALID_SERVICE_HANDLE:
            retStatusCode = StatusCode.UCE_INVALID_SERVICE_HANDLE;
            break;

        case UceStatusCode.UCE_INVALID_LISTENER_HANDLE:
            retStatusCode = StatusCode.UCE_INVALID_LISTENER_HANDLE;
            break;

        case UceStatusCode.UCE_INVALID_PARAM:
            retStatusCode = StatusCode.UCE_INVALID_PARAM;
            break;

        case UceStatusCode.UCE_FETCH_ERROR:
            retStatusCode = StatusCode.UCE_FETCH_ERROR;
            break;

        case UceStatusCode.UCE_REQUEST_TIMEOUT:
            retStatusCode = StatusCode.UCE_REQUEST_TIMEOUT;
            break;

        case UceStatusCode.UCE_INSUFFICIENT_MEMORY:
            retStatusCode = StatusCode.UCE_INSUFFICIENT_MEMORY;
            break;

        case UceStatusCode.UCE_LOST_NET:
            retStatusCode = StatusCode.UCE_LOST_NET;
            break;

        case UceStatusCode.UCE_NOT_SUPPORTED:
            retStatusCode = StatusCode.UCE_NOT_SUPPORTED;
            break;

        case UceStatusCode.UCE_NOT_FOUND:
            retStatusCode = StatusCode.UCE_NOT_FOUND;
            break;

        case UceStatusCode.UCE_SERVICE_UNAVAILABLE:
            retStatusCode = StatusCode.UCE_SERVICE_UNAVAILABLE;
            break;

        case UceStatusCode.UCE_NO_CHANGE_IN_CAP:
            retStatusCode = StatusCode.UCE_NO_CHANGE_IN_CAP;
            break;

        case UceStatusCode.UCE_INVALID_FEATURE_TAG:
            //retStatusCode = StatusCode.UCE_INVALID_FEATURE_TAG;
            break;

        case UceStatusCode.UCE_SERVICE_UNKNOWN:
            retStatusCode = StatusCode.UCE_SERVICE_UNKNOWN;
            break;

        default:
            retStatusCode = StatusCode.UCE_SERVICE_UNKNOWN;
            break;
        }

        return retStatusCode;
    }

private int getPresenceTriggerTypeValue(int publishTrigger) {
    int retPresTriggerType = PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_UNKNOWN;

    switch( publishTrigger ) {
    case com.qualcomm.qti.uceservice.V1_0.PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_ETAG_EXPIRED:
    retPresTriggerType = PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_ETAG_EXPIRED;
    break;

    case com.qualcomm.qti.uceservice.V1_0.PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_MOVE_TO_LTE_VOPS_DISABLED:
    retPresTriggerType = PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_MOVE_TO_LTE_VOPS_DISABLED;
    break;

    case com.qualcomm.qti.uceservice.V1_0.PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_MOVE_TO_LTE_VOPS_ENABLED:
    retPresTriggerType = PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_MOVE_TO_LTE_VOPS_ENABLED;
    break;

    case com.qualcomm.qti.uceservice.V1_0.PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_MOVE_TO_EHRPD:
    retPresTriggerType = PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_MOVE_TO_EHRPD;
    break;

    case com.qualcomm.qti.uceservice.V1_0.PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_MOVE_TO_HSPAPLUS:
    retPresTriggerType = PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_MOVE_TO_HSPAPLUS;
    break;

    case com.qualcomm.qti.uceservice.V1_0.PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_MOVE_TO_3G:
    retPresTriggerType = PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_MOVE_TO_3G;
    break;

    case com.qualcomm.qti.uceservice.V1_0.PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_MOVE_TO_2G:
    retPresTriggerType = PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_MOVE_TO_2G;
    break;

    case com.qualcomm.qti.uceservice.V1_0.PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_MOVE_TO_WLAN:
    retPresTriggerType = PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_MOVE_TO_WLAN;
    break;

    case com.qualcomm.qti.uceservice.V1_0.PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_MOVE_TO_IWLAN:
    retPresTriggerType = PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_MOVE_TO_IWLAN;
    break;

    case com.qualcomm.qti.uceservice.V1_0.PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_UNKNOWN:
    retPresTriggerType = PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_UNKNOWN;
    break;

    default:
    retPresTriggerType = PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_UNKNOWN;
    break;
    }

    return retPresTriggerType;
}

private int getPresCmdIdValue(int mCmdId) {
    int retCmdId = PresCmdId.UCE_PRES_CMD_UNKNOWN;

    switch( mCmdId ) {
    case PresenceCmdId.UCE_PRES_CMD_GET_VERSION:
    retCmdId = PresCmdId.UCE_PRES_CMD_GET_VERSION;
    break;

    case PresenceCmdId.UCE_PRES_CMD_PUBLISHMYCAP:
    retCmdId = PresCmdId.UCE_PRES_CMD_PUBLISHMYCAP;
    break;

    case PresenceCmdId.UCE_PRES_CMD_GETCONTACTCAP:
    retCmdId = PresCmdId.UCE_PRES_CMD_GETCONTACTCAP;
    break;

    case PresenceCmdId.UCE_PRES_CMD_GETCONTACTLISTCAP:
    retCmdId = PresCmdId.UCE_PRES_CMD_GETCONTACTLISTCAP;
    break;

    case PresenceCmdId.UCE_PRES_CMD_SETNEWFEATURETAG:
    retCmdId = PresCmdId.UCE_PRES_CMD_SETNEWFEATURETAG;
    break;

    case PresenceCmdId.UCE_PRES_CMD_REENABLE_SERVICE:
    retCmdId = PresCmdId.UCE_PRES_CMD_REENABLE_SERVICE;
    break;

    case PresenceCmdId.UCE_PRES_CMD_UNKNOWN:
    retCmdId = PresCmdId.UCE_PRES_CMD_UNKNOWN;
    break;

    default:
    retCmdId = PresCmdId.UCE_PRES_CMD_UNKNOWN;
    break;
    }

    return retCmdId;
}

private int getPresSubscriptionState(int mPresSubscriptionState) {
    int retPresSubscriptionState = PresSubscriptionState.UCE_PRES_SUBSCRIPTION_STATE_UNKNOWN;

    switch( mPresSubscriptionState ) {
    case com.qualcomm.qti.uceservice.V1_0.PresSubscriptionState.UCE_PRES_SUBSCRIPTION_STATE_ACTIVE:
    retPresSubscriptionState = PresSubscriptionState.UCE_PRES_SUBSCRIPTION_STATE_ACTIVE;
    break;

    case com.qualcomm.qti.uceservice.V1_0.PresSubscriptionState.UCE_PRES_SUBSCRIPTION_STATE_PENDING:
    retPresSubscriptionState = PresSubscriptionState.UCE_PRES_SUBSCRIPTION_STATE_PENDING;
    break;

    case com.qualcomm.qti.uceservice.V1_0.PresSubscriptionState.UCE_PRES_SUBSCRIPTION_STATE_TERMINATED:
    retPresSubscriptionState = PresSubscriptionState.UCE_PRES_SUBSCRIPTION_STATE_TERMINATED;
    break;

    case com.qualcomm.qti.uceservice.V1_0.PresSubscriptionState.UCE_PRES_SUBSCRIPTION_STATE_UNKNOWN:
    retPresSubscriptionState = PresSubscriptionState.UCE_PRES_SUBSCRIPTION_STATE_UNKNOWN;
    break;

    default:
    retPresSubscriptionState = PresSubscriptionState.UCE_PRES_SUBSCRIPTION_STATE_UNKNOWN;
    break;
    }

    return retPresSubscriptionState;
}

private int getResInstanceStateValue(int instanceState) {
    int retInstanceState = PresResInstanceInfo.UCE_PRES_RES_INSTANCE_STATE_UNKNOWN;

    switch( instanceState ) {
    case ResInstantceState.UCE_PRES_RES_INSTANCE_STATE_ACTIVE:
    retInstanceState = PresResInstanceInfo.UCE_PRES_RES_INSTANCE_STATE_ACTIVE;
    break;

    case ResInstantceState.UCE_PRES_RES_INSTANCE_STATE_PENDING:
    retInstanceState = PresResInstanceInfo.UCE_PRES_RES_INSTANCE_STATE_PENDING;
    break;

    case ResInstantceState.UCE_PRES_RES_INSTANCE_STATE_TERMINATED:
    retInstanceState = PresResInstanceInfo.UCE_PRES_RES_INSTANCE_STATE_TERMINATED;
    break;

    case ResInstantceState.UCE_PRES_RES_INSTANCE_STATE_UNKNOWN:
    retInstanceState = PresResInstanceInfo.UCE_PRES_RES_INSTANCE_STATE_UNKNOWN;
    break;

    case ResInstantceState.UCE_PRES_RES_INSTANCE_UNKNOWN:
    retInstanceState = PresResInstanceInfo.UCE_PRES_RES_INSTANCE_UNKNOWN;
    break;

    default:
    retInstanceState = PresResInstanceInfo.UCE_PRES_RES_INSTANCE_STATE_UNKNOWN;
    break;
    }

    return retInstanceState;
}

private int getOptionsCmdIdValue(int cmdId) {
    int retCmdId = com.android.ims.internal.uce.options.OptionsCmdId.UCE_OPTIONS_CMD_UNKNOWN;

    switch( cmdId ) {
    case OptionsCmdId.UCE_OPTIONS_CMD_GETMYCDINFO:
    retCmdId = com.android.ims.internal.uce.options.OptionsCmdId.UCE_OPTIONS_CMD_GETMYCDINFO;
    break;

    case OptionsCmdId.UCE_OPTIONS_CMD_SETMYCDINFO:
    retCmdId = com.android.ims.internal.uce.options.OptionsCmdId.UCE_OPTIONS_CMD_SETMYCDINFO;
    break;

    case OptionsCmdId.UCE_OPTIONS_CMD_GETCONTACTCAP:
    retCmdId = com.android.ims.internal.uce.options.OptionsCmdId.UCE_OPTIONS_CMD_SETMYCDINFO;
    break;

    case OptionsCmdId.UCE_OPTIONS_CMD_GETCONTACTLISTCAP:
    retCmdId = com.android.ims.internal.uce.options.OptionsCmdId.UCE_OPTIONS_CMD_GETCONTACTLISTCAP;
    break;

    case OptionsCmdId.UCE_OPTIONS_CMD_RESPONSEINCOMINGOPTIONS:
    retCmdId = com.android.ims.internal.uce.options.OptionsCmdId.UCE_OPTIONS_CMD_RESPONSEINCOMINGOPTIONS;
    break;

    case OptionsCmdId.UCE_OPTIONS_CMD_GET_VERSION:
    retCmdId = com.android.ims.internal.uce.options.OptionsCmdId.UCE_OPTIONS_CMD_GET_VERSION;
    break;

    case OptionsCmdId.UCE_OPTIONS_CMD_UNKNOWN:
    retCmdId = com.android.ims.internal.uce.options.OptionsCmdId.UCE_OPTIONS_CMD_UNKNOWN;
    break;

    default:
    retCmdId = com.android.ims.internal.uce.options.OptionsCmdId.UCE_OPTIONS_CMD_UNKNOWN;
    break;
    }

    return retCmdId;
}
/* utility function to map HIDL CapInfo to AIDL CapInfo */
private CapInfo getAidlCapInfo(com.qualcomm.qti.uceservice.V1_0.CapabilityInfo hidlCapInfo){
    CapInfo aidlCapInfo = new CapInfo();

    aidlCapInfo.setImSupported(hidlCapInfo.mImSupported);
    aidlCapInfo.setFtSupported(hidlCapInfo.mFtSupported);
    aidlCapInfo.setFtThumbSupported(hidlCapInfo.mFtThumbSupported);
    aidlCapInfo.setFtSnFSupported(hidlCapInfo.mFtSnFSupported);
    aidlCapInfo.setFtHttpSupported(hidlCapInfo.mFtHttpSupported);
    aidlCapInfo.setIsSupported(hidlCapInfo.mIsSupported);
    aidlCapInfo.setVsDuringCSSupported(hidlCapInfo.mVsDuringCSSupported);
    aidlCapInfo.setVsSupported(hidlCapInfo.mVsSupported);
    aidlCapInfo.setSpSupported(hidlCapInfo.mSpSupported);
    aidlCapInfo.setCdViaPresenceSupported(hidlCapInfo.mCdViaPresenceSupported);
    aidlCapInfo.setIpVoiceSupported(hidlCapInfo.mIpVoiceSupported);
    aidlCapInfo.setIpVideoSupported(hidlCapInfo.mIpVideoSupported);
    aidlCapInfo.setGeoPullFtSupported(hidlCapInfo.mGeoPullFtSupported);
    aidlCapInfo.setGeoPullSupported(hidlCapInfo.mGeoPullSupported);
    aidlCapInfo.setGeoPushSupported(hidlCapInfo.mGeoPushSupported);
    aidlCapInfo.setSmSupported(hidlCapInfo.mSmSupported);
    aidlCapInfo.setFullSnFGroupChatSupported(hidlCapInfo.mFullSnFGroupChatSupported);
    aidlCapInfo.setRcsIpVoiceCallSupported(hidlCapInfo.mRcsIpVoiceCallSupported);
    aidlCapInfo.setRcsIpVideoCallSupported(hidlCapInfo.mRcsIpVideoCallSupported);
    aidlCapInfo.setRcsIpVideoOnlyCallSupported(hidlCapInfo.mRcsIpVideoOnlyCallSupported);
    //aidlCapInfo.setExts(hidlCapInfo.mExts.toArray(new String[hidlCapInfo.mExts.size()]));
    aidlCapInfo.setCapTimestamp(hidlCapInfo.mCapTimestamp);

    return aidlCapInfo;
}

}
