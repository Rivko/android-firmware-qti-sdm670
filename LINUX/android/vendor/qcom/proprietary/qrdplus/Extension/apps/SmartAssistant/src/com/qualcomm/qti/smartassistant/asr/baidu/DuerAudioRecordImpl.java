/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
package com.qualcomm.qti.smartassistant.asr.baidu;

import android.media.AudioAttributes;
import android.media.AudioFormat;
import android.media.AudioRecord;
import android.os.Handler;
import android.os.Process;

import com.baidu.duer.dcs.systeminterface.BaseAudioRecorder;
import com.baidu.duer.dcs.common.util.FileUtil;
import com.qualcomm.qti.smartassistant.utils.LogUtils;
import com.qualcomm.qti.smartassistant.utils.FileUtils;
import com.qualcomm.qti.smartassistant.wrapper.AudioAttributesBuilderWrapper;
import com.qualcomm.qti.smartassistant.wrapper.AudioRecordWrapper;

import java.nio.ByteBuffer;

public class DuerAudioRecordImpl extends BaseAudioRecorder {
    private static final String TAG = DuerAudioRecordImpl.class.getSimpleName();
    private static final int SAMPLE_RATE_HZ = 16000;
    private static final int MAX_AUDIO_RECORDING_TIME = 10;
    private static final int MAX_AUDIO_RECORDING_BYTES =
            MAX_AUDIO_RECORDING_TIME * SAMPLE_RATE_HZ * 2;
    private static final int BUFFER_SIZE = 640;
    private final Handler handler = new Handler();
    private AudioRecord audioRecord;
    private Thread recordThread;
    private int mAudioSampleRate = 16000;
    private int mAudioSource = 1;
    private int mAudioFormat = 2;
    private int mAudioChannel = 16;
    private volatile boolean isStartRecord = false;
    private AudioRecordMode mAudioRecordMode = AudioRecordMode.NORMAL;
    private int mCaptureSession;
    private int mSoundModelHandle;
    private int totalBytesToRead = MAX_AUDIO_RECORDING_BYTES;

    public enum AudioRecordMode{
        NORMAL,
        ONE_SHOT
    }

    public DuerAudioRecordImpl() {
        setAudio(16000, 1, 2, 16);
    }

    private void setAudio(int sampleRate, int source, int format, int channel) {
        mAudioSampleRate = sampleRate;
        mAudioSource = source;
        mAudioFormat = format;
        mAudioChannel = channel;
    }

    public void setAudioRecordMode(AudioRecordMode mode, int captureSession, int soundModelHandle){
        mAudioRecordMode = mode;
        mCaptureSession = captureSession;
        mSoundModelHandle = soundModelHandle;
    }

    @Override
    public void startRecord() {
        Process.setThreadPriority(-16);
        updateAudioRecordMode();
        this.isStartRecord = true;
        this.recordThread = new Thread() {
            public void run() {
                super.run();
                if (audioRecord.getState() != AudioRecord.STATE_INITIALIZED){
                    LogUtils.e(TAG, "audio record initialization check failed");
                    return;
                }
                LogUtils.v(TAG, "startRecording ");
                try {
                    audioRecord.startRecording();
                } catch (IllegalStateException var4) {
                    var4.printStackTrace();
                    LogUtils.e(TAG, "startRecording IllegalStateException ", var4);
                    isStartRecord = false;
                }
                ByteBuffer totalBuffer = ByteBuffer.allocate(totalBytesToRead);
                int curBytesRead = 0;
                while(isStartRecord && curBytesRead < totalBytesToRead) {
                    try {
                        byte[] buffer = new byte[640];
                        int readBytes = audioRecord.read(buffer, 0, 640);
                        if(readBytes > 0) {
                            fireData(buffer);
                            curBytesRead += readBytes;
                            if (totalBuffer != null){
                                totalBuffer.put(buffer,0, readBytes);
                            }
                        }
                    } catch (Exception var3) {
                        isStartRecord = false;
                    }
                }
                if (curBytesRead > 0 && FileUtils.getIsSavePcmDataSetting()) {
                    LogUtils.v(TAG, "curBytesRead = " + curBytesRead);
                    String directory = mAudioRecordMode == AudioRecordMode.ONE_SHOT ?
                            FileUtils.LOOK_AHEAD_VOICE_COMMANDS_PATH :
                            FileUtils.NORMAL_VOICE_COMMANDS_PATH;
                    String filePath = directory + "/"+ String.valueOf(System.currentTimeMillis())+
                            FileUtils.PATH_RECORDINGS_FILE_EXT;
                    FileUtils.writeBufferToWavFile(totalBuffer.array(),curBytesRead,filePath,
                            false);
                }
                setAudioRecordMode(AudioRecordMode.NORMAL, -1, -1);
                audioRecord.stop();
                audioRecord.release();
                audioRecord = null;
                LogUtils.v(TAG, "audioRecorder released ");
            }
        };
        this.recordThread.start();
    }

    @Override
    public void stopRecord() {
        LogUtils.v(TAG, "stopRecord ");
        this.isStartRecord = false;
        if(this.recordThread != null) {
            this.recordThread.interrupt();
            this.recordThread = null;
        }

    }

    public void release() {
        this.stopRecord();
        this.handler.removeCallbacksAndMessages((Object)null);
    }

    private void updateAudioRecordMode(){
        if (mAudioRecordMode == AudioRecordMode.NORMAL){
            updateNormalAudioRecord();
        }else if (mAudioRecordMode == AudioRecordMode.ONE_SHOT){
            updateOneShotAudioRecord();
        }
    }

    private void updateNormalAudioRecord(){
        totalBytesToRead = MAX_AUDIO_RECORDING_BYTES;
        int bufferSize = AudioRecord.getMinBufferSize(mAudioSampleRate, mAudioChannel, mAudioFormat);
        LogUtils.d(TAG, "min bufferSize:" + bufferSize);
        audioRecord = new AudioRecord(this.mAudioSource, mAudioSampleRate,
                mAudioChannel, mAudioFormat, bufferSize);
    }

    private void updateOneShotAudioRecord(){
        /* Open AudioRecord Session using sessionId returned by detection event */
        AudioAttributesBuilderWrapper audAttBldr = new AudioAttributesBuilderWrapper();
        audAttBldr.setUsage(AudioAttributes.USAGE_MEDIA);
        audAttBldr.setContentType(AudioAttributes.CONTENT_TYPE_SPEECH);
        audAttBldr.setInternalCapturePreset(1999); // %%% MediaRecorder.HOTWORD
        // %%% HOTWORD is current private so hardcored value is used
        AudioAttributes audAtt = audAttBldr.build();

        int minBuffSize = AudioRecord.getMinBufferSize(16000, AudioFormat.CHANNEL_IN_MONO, AudioFormat.ENCODING_PCM_16BIT);
        LogUtils.v(TAG, "LookAheadBufferTask getMinBuffSize returns " + minBuffSize);

        AudioFormat.Builder audFormatBldr = new AudioFormat.Builder();
        audFormatBldr.setEncoding(AudioFormat.ENCODING_PCM_16BIT);
        audFormatBldr.setSampleRate(16000);
        audFormatBldr.setChannelMask(AudioFormat.CHANNEL_IN_MONO);
        AudioFormat audFormat = audFormatBldr.build();

        audioRecord = new AudioRecordWrapper(audAtt, audFormat, minBuffSize, mCaptureSession).
                getAudioRecordInstance();
        final int configOneShotBufferTime = FileUtils.getOneShotBufferTimeSetting() / 1000;
        final int oneShotBufferTime = configOneShotBufferTime >
                MAX_AUDIO_RECORDING_TIME ? MAX_AUDIO_RECORDING_TIME :
                configOneShotBufferTime;
        totalBytesToRead = oneShotBufferTime * SAMPLE_RATE_HZ * 2;
    }
}
