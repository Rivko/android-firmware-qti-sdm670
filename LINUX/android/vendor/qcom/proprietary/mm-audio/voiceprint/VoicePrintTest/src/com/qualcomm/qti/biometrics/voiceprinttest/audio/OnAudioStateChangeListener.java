/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.biometrics.voiceprinttest.audio;

/**
 * The listener interface for receiving onAudioStateChange events. The class
 * that is interested in processing a onAudioStateChange event implements this
 * interface, and the object created with that class is registered with a
 * component using the component's
 * <code>addOnAudioStateChangeListener<code> method. When
 * the onAudioStateChange event occurs, that object's appropriate
 * method is invoked.
 *
 * @see AudioRecordManager
 */
public interface OnAudioStateChangeListener {

    public enum AudioState {
        Idle,
        Recording,
        Stopping
    }

    public enum AudioError {
        Unknown,
        /**
         * Unable to start audio record (may be in use).
         */
        AudioRecordError,

        PhoneCallInProgress,

        AlreadyRunning,

        MaxLengthReached,

    }

    /**
     * On state change.
     */
    public void onStateChange(AudioState state);

    public void onError(AudioError error);

    public void onAudioCapture(byte[] buffer, float energy);

}
