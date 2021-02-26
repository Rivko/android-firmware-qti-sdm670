/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package org.codeaurora.callenhancement;

import org.codeaurora.callenhancement.ICallRecordStateListener;

interface ICallEnhancement {
     void startRecordingCall();
     void stopRecordingCall();
     long getRecordedCallDuration();
     boolean isCallRecording();
     boolean canCallRecord();
     void registerCallRecordStateChanged(ICallRecordStateListener listener);
     void unregisterCallRecordStateChanged(ICallRecordStateListener listener);
}
