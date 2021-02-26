/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

package com.qualcomm.qti.sva.wrapper;

import android.media.AudioAttributes;
import android.media.AudioFormat;
import android.media.AudioRecord;

public class AudioRecordWrapper {
    private AudioRecord mAudioRecord;

    public AudioRecordWrapper(AudioAttributes attributes, AudioFormat format,
                       int bufferSizeInBytes,
                       int sessionId) throws IllegalArgumentException {
        mAudioRecord =
                new AudioRecord(attributes, format, bufferSizeInBytes, sessionId);
    }

    public AudioRecord getAudioRecordInstance() {
        return mAudioRecord;
    }
}
