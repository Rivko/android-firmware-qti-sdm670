/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
package com.qualcomm.qti.smartassistant.asr;

import java.util.List;

public interface IVoiceSdkListener {

    void onVoiceStart();

    void onVoiceStop();

    void onNetworkException();

    void onRenderVoiceInputText(String text);

    void onHtmlPayload(String url);

    void onPhoneCallByName(List<CandidateInfo> candidateInfos);

    void onSendSmsByName(List<CandidateInfo> candidateInfos, String message);
}
