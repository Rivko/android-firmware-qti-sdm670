/*********************************************************************
 Copyright (c) 2017 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
**********************************************************************/

package com.qualcomm.qti.translator;


import com.qualcomm.qti.uceservice.V1_0.PresenceCapabilityInfo;
import com.qualcomm.qti.uceservice.V1_0.PresServiceInfo;
import com.qualcomm.qti.uceservice.V1_0.OptionsCapabilityInfo;
import com.qualcomm.qti.uceservice.V1_0.CapabilityInfo;

import com.android.ims.internal.uce.common.CapInfo;
import com.android.ims.internal.uce.presence.PresCapInfo;
import com.android.ims.internal.uce.options.OptionsCapInfo;

import android.util.Log;

import java.util.Arrays;


public class AidlToHidlTranslator
{
  private final static String logTAG = "AIDL to HIDL Translator";

/* Translator methods pertaining to PresService class */

public PresenceCapabilityInfo getHidlPresCapInfo(PresCapInfo  aidlPresCapInfo) {
    PresenceCapabilityInfo hidlPresCapInfo = new PresenceCapabilityInfo();

    hidlPresCapInfo.mCapInfo.mImSupported = aidlPresCapInfo.getCapInfo().isImSupported();
    hidlPresCapInfo.mCapInfo.mFtSupported = aidlPresCapInfo.getCapInfo().isFtSupported();
    hidlPresCapInfo.mCapInfo.mFtThumbSupported = aidlPresCapInfo.getCapInfo().isFtThumbSupported();
    hidlPresCapInfo.mCapInfo.mFtSnFSupported = aidlPresCapInfo.getCapInfo().isFtSnFSupported();
    hidlPresCapInfo.mCapInfo.mFtHttpSupported = aidlPresCapInfo.getCapInfo().isFtHttpSupported();
    hidlPresCapInfo.mCapInfo.mIsSupported = aidlPresCapInfo.getCapInfo().isIsSupported();
    hidlPresCapInfo.mCapInfo.mVsDuringCSSupported = aidlPresCapInfo.getCapInfo().isVsDuringCSSupported();
    hidlPresCapInfo.mCapInfo.mVsSupported = aidlPresCapInfo.getCapInfo().isVsSupported();
    hidlPresCapInfo.mCapInfo.mSpSupported = aidlPresCapInfo.getCapInfo().isSpSupported();
    hidlPresCapInfo.mCapInfo.mCdViaPresenceSupported = aidlPresCapInfo.getCapInfo().isCdViaPresenceSupported();
    hidlPresCapInfo.mCapInfo.mIpVoiceSupported = aidlPresCapInfo.getCapInfo().isIpVoiceSupported();
    hidlPresCapInfo.mCapInfo.mIpVideoSupported = aidlPresCapInfo.getCapInfo().isIpVideoSupported();
    hidlPresCapInfo.mCapInfo.mGeoPullFtSupported = aidlPresCapInfo.getCapInfo().isGeoPullFtSupported();
    hidlPresCapInfo.mCapInfo.mGeoPullSupported = aidlPresCapInfo.getCapInfo().isGeoPullSupported();
    hidlPresCapInfo.mCapInfo.mGeoPushSupported = aidlPresCapInfo.getCapInfo().isGeoPushSupported();
    hidlPresCapInfo.mCapInfo.mSmSupported = aidlPresCapInfo.getCapInfo().isSmSupported();
    hidlPresCapInfo.mCapInfo.mFullSnFGroupChatSupported = aidlPresCapInfo.getCapInfo().isFullSnFGroupChatSupported();
    hidlPresCapInfo.mCapInfo.mRcsIpVoiceCallSupported = aidlPresCapInfo.getCapInfo().isRcsIpVoiceCallSupported();
    hidlPresCapInfo.mCapInfo.mRcsIpVideoCallSupported = aidlPresCapInfo.getCapInfo().isRcsIpVideoCallSupported();
    hidlPresCapInfo.mCapInfo.mRcsIpVideoOnlyCallSupported = aidlPresCapInfo.getCapInfo().isRcsIpVideoOnlyCallSupported();
    //hidlPresCapInfo.mCapInfo.mExts.addAll(Arrays.asList(aidlPresCapInfo.getCapInfo().getExts()));
    hidlPresCapInfo.mCapInfo.mCapTimestamp = aidlPresCapInfo.getCapInfo().getCapTimestamp();
    hidlPresCapInfo.mContactUri = "";
    if(aidlPresCapInfo.getContactUri() !=  null){
       hidlPresCapInfo.mContactUri = aidlPresCapInfo.getContactUri();
    }

    return hidlPresCapInfo;
}

public PresServiceInfo getHidlPresSvcInfo(com.android.ims.internal.uce.presence.PresServiceInfo aidlPresServiceInfo) {
    PresServiceInfo hidlPresSvcInfo = new PresServiceInfo();

    hidlPresSvcInfo.mServiceId = aidlPresServiceInfo.getServiceId();
    hidlPresSvcInfo.mServiceDesc = aidlPresServiceInfo.getServiceDesc();
    hidlPresSvcInfo.mServiceVer = aidlPresServiceInfo.getServiceVer();
    hidlPresSvcInfo.mMediaCap = aidlPresServiceInfo.getMediaType();

    return hidlPresSvcInfo;
}

/* Translator methods pertaining to OptionsService class */

public OptionsCapabilityInfo getHidlOptCapInfo(OptionsCapInfo aidlOptCapInfo) {
    OptionsCapabilityInfo hidlOptCapInfo = new OptionsCapabilityInfo();

    getHidlCapInfo(hidlOptCapInfo.capInfo, aidlOptCapInfo.getCapInfo());
    hidlOptCapInfo.sdp = new String();
    hidlOptCapInfo.sdp = aidlOptCapInfo.getSdp();

    return hidlOptCapInfo;
}

public void getHidlCapInfo(CapabilityInfo hidlCapInfo, CapInfo aidlCapInfo) {

    hidlCapInfo.mImSupported = aidlCapInfo.isImSupported();
    hidlCapInfo.mFtSupported = aidlCapInfo.isFtSupported();
    hidlCapInfo.mFtThumbSupported = aidlCapInfo.isFtThumbSupported();
    hidlCapInfo.mFtSnFSupported = aidlCapInfo.isFtSnFSupported();
    hidlCapInfo.mFtHttpSupported = aidlCapInfo.isFtHttpSupported();
    hidlCapInfo.mIsSupported = aidlCapInfo.isIsSupported();
    hidlCapInfo.mVsDuringCSSupported = aidlCapInfo.isVsDuringCSSupported();
    hidlCapInfo.mVsSupported = aidlCapInfo.isVsSupported();
    hidlCapInfo.mSpSupported = aidlCapInfo.isSpSupported();
    hidlCapInfo.mCdViaPresenceSupported = aidlCapInfo.isCdViaPresenceSupported();
    hidlCapInfo.mIpVoiceSupported = aidlCapInfo.isIpVoiceSupported();
    hidlCapInfo.mIpVideoSupported = aidlCapInfo.isIpVideoSupported();
    hidlCapInfo.mGeoPullFtSupported = aidlCapInfo.isGeoPullFtSupported();
    hidlCapInfo.mGeoPullSupported = aidlCapInfo.isGeoPullSupported();
    hidlCapInfo.mGeoPushSupported = aidlCapInfo.isGeoPushSupported();
    hidlCapInfo.mSmSupported = aidlCapInfo.isSmSupported();
    hidlCapInfo.mFullSnFGroupChatSupported = aidlCapInfo.isFullSnFGroupChatSupported();
    hidlCapInfo.mRcsIpVoiceCallSupported = aidlCapInfo.isRcsIpVoiceCallSupported();
    hidlCapInfo.mRcsIpVideoCallSupported = aidlCapInfo.isRcsIpVideoCallSupported();
    hidlCapInfo.mRcsIpVideoOnlyCallSupported = aidlCapInfo.isRcsIpVideoOnlyCallSupported();
    //hidlCapInfo.mExts.addAll(Arrays.asList(aidlCapInfo.getExts()));
    hidlCapInfo.mCapTimestamp = aidlCapInfo.getCapTimestamp();
}

}
