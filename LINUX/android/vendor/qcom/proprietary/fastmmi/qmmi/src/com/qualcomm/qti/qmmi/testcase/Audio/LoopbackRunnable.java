/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.testcase.Audio;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioRecord;
import android.media.AudioTrack;
import android.media.MediaRecorder;

import com.qualcomm.qti.qmmi.framework.BaseRunnable;
import com.qualcomm.qti.qmmi.utils.LogUtils;

public class LoopbackRunnable extends BaseRunnable {

    final static int SAMPLE_RATE = 8000;
    private AudioRecord audioRecord = null;
    private AudioTrack audioTrack = null;

    public void run() {

        LogUtils.logi("LoopBack started");
        setRunning(true);

        int bufferSize = AudioRecord
                .getMinBufferSize(SAMPLE_RATE, AudioFormat.CHANNEL_IN_MONO,
                        AudioFormat.ENCODING_PCM_16BIT);
        bufferSize = Math.max(bufferSize, AudioTrack.getMinBufferSize(
                SAMPLE_RATE, AudioFormat.CHANNEL_IN_MONO,
                AudioFormat.ENCODING_PCM_16BIT));

        if (audioRecord == null) {
            audioRecord = new AudioRecord(
                    MediaRecorder.AudioSource.MIC, SAMPLE_RATE,
                    AudioFormat.CHANNEL_IN_MONO,
                    AudioFormat.ENCODING_PCM_16BIT, bufferSize);
        }
        if (audioTrack == null) {
            audioTrack = new AudioTrack(
                    AudioManager.STREAM_VOICE_CALL, SAMPLE_RATE,
                    AudioFormat.CHANNEL_OUT_MONO,
                    AudioFormat.ENCODING_PCM_16BIT, bufferSize,
                    AudioTrack.MODE_STREAM);
        }

        audioTrack.setPlaybackRate(SAMPLE_RATE);

        audioRecord.startRecording();
        audioTrack.play();

        byte[] buffer = new byte[bufferSize];
        while (getRunning()) {
            int readSize = audioRecord.read(buffer, 0, bufferSize);
            if (readSize > 0)
                audioTrack.write(buffer, 0, readSize);
        }

        audioRecord.stop();
        audioRecord.release();
        audioRecord = null;

        audioTrack.stop();
        audioTrack.release();
        audioTrack = null;
    }

}
