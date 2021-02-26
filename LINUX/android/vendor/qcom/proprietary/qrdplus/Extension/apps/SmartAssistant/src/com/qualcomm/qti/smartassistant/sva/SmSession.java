/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
package com.qualcomm.qti.smartassistant.sva;

import android.hardware.soundtrigger.SoundTrigger;

public class SmSession {

    public enum SmState {
        UNLOADED,
        LOADED,
        STARTED,
        STOPPED,
    }

    public final String mSmName;
    public final SoundTrigger.KeyphraseRecognitionExtra[] mKpRecognitionExtra;
    private SmState mSessionState = SmState.UNLOADED;
    public SoundTrigger.RecognitionConfig mRecognitionConfig = null;
    public int mSessionId = -1;

    public SmSession(String smName,
                     SoundTrigger.KeyphraseRecognitionExtra[] kpRecognitionExtra,
                     SmState smState, int sessionId) {
        mSmName = smName;
        mKpRecognitionExtra = kpRecognitionExtra;
        mSessionState = smState;
        mSessionId = sessionId;
    }

    public void setRecognitionConfig(SoundTrigger.RecognitionConfig recognitionConfig) {
        mRecognitionConfig = recognitionConfig;
    }

    public void removeRecognitionConfig() {
        mRecognitionConfig = null;
    }

    public void setSessionState(SmState state) {
        mSessionState = state;
    }

    public SmState getSessionState() {
        return mSessionState;
    }

    public String getSmName() {
        return mSmName;
    }

    public SoundTrigger.KeyphraseRecognitionExtra[] getKpRecognitionExtra() {
        return mKpRecognitionExtra;
    }

    public SoundTrigger.RecognitionConfig getRecognitionConfig() {
        return mRecognitionConfig;
    }

    public int getSessionId() {
        return mSessionId;
    }
}
