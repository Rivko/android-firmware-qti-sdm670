/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
package com.qualcomm.qti.smartassistant.asr;

import android.content.Context;

import org.json.JSONException;

public interface IVoiceSdk {

    void initializeSdk(Context context);

    void addVoiceSdkListener(IVoiceSdkListener listener);

    void removeVoiceSdkListener(IVoiceSdkListener listener);

    void startRecognition(boolean captureAvailable, int captureSession, int soundModelHandle);

    void stopRecognition();

    void updateContacts(String contacts);

    void releaseSdk();
}
