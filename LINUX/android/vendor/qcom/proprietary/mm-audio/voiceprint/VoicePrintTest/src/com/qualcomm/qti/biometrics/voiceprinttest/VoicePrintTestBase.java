/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.biometrics.voiceprinttest;

import java.io.File;
import java.io.FileInputStream;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;

import com.qualcomm.qti.biometrics.voiceprint.sdk.OnResultListener;
import com.qualcomm.qti.biometrics.voiceprint.sdk.VoicePrintError;
import com.qualcomm.qti.biometrics.voiceprint.sdk.VoicePrintManager;
import com.qualcomm.qti.biometrics.voiceprint.sdk.OnEnrollListener;
import com.qualcomm.qti.biometrics.voiceprint.sdk.OnErrorListener;
import com.qualcomm.qti.biometrics.voiceprint.sdk.OnMatchListener;
import com.qualcomm.qti.biometrics.voiceprinttest.audio.AudioRecordManager;
import com.qualcomm.qti.biometrics.voiceprinttest.audio.OnAudioStateChangeListener;
import com.qualcomm.qti.biometrics.voiceprinttest.audio.OnAudioStateChangeListener.AudioState;

import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.media.AudioFormat;
import android.os.Bundle;
import android.test.AndroidTestCase;
import android.util.Log;

public class VoicePrintTestBase extends AndroidTestCase {

    private static final String TAG = "VoicePrintTestBase";

    private VoicePrintManager mVoicePrintManager;

    private AudioRecordManager mAudioRecordManager;

    private VoicePrintTestLog mTestLog;

    public static String identifier = "";

    public static String test_name = "";

    public static String phrase = "";

    public static String match_pcm_file = "";

    public static String enroll_pcm_folder = "";

    public static byte[] randomData = new byte[1024];

    public static String threshold = "";

    private boolean mStartMic = false;

    private boolean mStopMic = false;

    private Context mContext;

    private boolean mTestPassed = false;

    private boolean mErrorOccured = false;

    private final class EnrollListener implements OnEnrollListener {
        private final CountDownLatch done = new CountDownLatch(1);

        public boolean await() throws InterruptedException {
            return done.await(VoicePrintTestConst.COUNTDOWNLATCH_AWAIT_PERIOD,
                    TimeUnit.MILLISECONDS);
        }

        @Override
        public void onError(int errorCode) {
            Log.e(TAG, "EnrollListener got onError callback: " + errorCode);
            mTestLog.writeTestLog("EnrollListener got onError callback: " + errorCode + "\n");
            mErrorOccured = true;
            done.countDown();
        }

        @Override
        public void onEnrollComplete(boolean success, Bundle extra) {
            if (success) {
                Log.d(TAG, "Enroll is successful. Extra: " + extra.toString());
                mTestLog.writeTestLog("Enroll is successful. Extra: " + extra.toString() + "\n");
            } else {
                Log.e(TAG, "Enroll is failed. Extra: " + extra.toString());
                mTestLog.writeTestLog("Enroll is failed. Extra: " + extra.toString() + "\n");
                mErrorOccured = true;
            }
            done.countDown();
        }
    }

    private final class ErrorListener implements OnErrorListener {
        private final CountDownLatch done = new CountDownLatch(1);

        public boolean await() throws InterruptedException {
            return done.await(VoicePrintTestConst.COUNTDOWNLATCH_AWAIT_PERIOD,
                    TimeUnit.MILLISECONDS);
        }

        @Override
        public void onError(int errorCode) {
            Log.e(TAG, "ErrorListener got onError callback: " + errorCode);
            mTestLog.writeTestLog("ErrorListener got onError callback: " + errorCode + "\n");
            mErrorOccured = true;
            done.countDown();
        }
    }

    private final class ResultListener implements OnResultListener {
        private final CountDownLatch done = new CountDownLatch(1);

        public boolean await() throws InterruptedException {
            Log.d(TAG, "await is called.");
            return done.await(VoicePrintTestConst.COUNTDOWNLATCH_AWAIT_PERIOD,
                    TimeUnit.MILLISECONDS);
        }

        @Override
        public void onError(int errorCode) {
            Log.e(TAG, "ResultListener got onError callback: " + errorCode);
            mTestLog.writeTestLog("ResultListener got onError callback: " + errorCode + "\n");
            mErrorOccured = true;
            done.countDown();
        }

        @Override
        public void onResult(int code, Bundle extras) {
            Log.d(TAG, "onResult is called.");
            // No bundle information
            if (code == OnResultListener.SUCCESS) {
                Log.d(TAG, "onResult success. Code: " + code);
            } else {
                Log.e(TAG, "onResult fail. Code: " + code);
                mTestLog.writeTestLog("onResult fail. Code: " + code + "\n");
                mErrorOccured = true;
            }
            done.countDown();
        }
    };

    private final class MatchListener implements OnMatchListener {
        private final CountDownLatch done = new CountDownLatch(1);

        public boolean await() throws InterruptedException {
            return done.await(VoicePrintTestConst.COUNTDOWNLATCH_AWAIT_PERIOD,
                    TimeUnit.MILLISECONDS);
        }

        @Override
        public void onError(int errorCode) {
            Log.e(TAG, "MatchListener got onError callback: " + errorCode);
            mTestLog.writeTestLog("MatchListener got onError callback: " + errorCode + "\n");
            mErrorOccured = true;
            done.countDown();
        }

        @Override
        public void onMatchStatus(boolean success, Bundle extra) {
            if (success) {
                Log.d(TAG, "Match is successful. Extra: " + extra.toString());
                mTestLog.writeTestLog("Match is successful. Extra: " + extra.toString() + "\n");
            } else {
                Log.e(TAG, "Match is failed. Extra: " + extra.toString());
                mTestLog.writeTestLog("Match is failed. Extra: " + extra.toString() + "\n");
                mErrorOccured = true;
            }
            done.countDown();
        }
    };

    public MicReceiver mReceiver = new MicReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getExtras().getString("mic");
            if (action.equals("mic_start")) {
                Log.d(TAG, "Received intent to start mic");
                mStartMic = true;
            } else if (action.equals("mic_stop")) {
                Log.d(TAG, "Received intent to stop mic");
                mStopMic = true;
            }
        }
    };

    private void failTest(String msg) {
        Log.e(TAG, msg);
        mTestLog.writeTestLog(msg + "\n");
        fail(msg);
    }

    @Override
    protected void setUp() throws Exception {
        Log.d(TAG, this.getName() + " setUp");
        super.setUp();

        if (mVoicePrintManager != null) {
            mVoicePrintManager.close();
        }
        mContext = getContext();
        mVoicePrintManager = VoicePrintManager.newInstance(getContext());
        Thread.sleep(500);
        mAudioRecordManager = new AudioRecordManager(getContext());
        mTestLog = new VoicePrintTestLog(0, test_name);
        mTestPassed = false;
        mErrorOccured = false;

        mTestLog.writeTestLog(test_name + "\n " + this.getName() + "\n");
    }

    @Override
    protected void tearDown() throws Exception {
        Log.d(TAG, this.getName() + " tearDown");
        super.tearDown();
        if (!mTestPassed) {
            mTestLog.writeTestLog("fail\n");
        } else {
            mTestLog.writeTestLog("pass\n");
        }
        Log.d(TAG, mTestLog.getTestLog());
        mTestLog.saveTestLog();
    }

    public void testMatchData() throws Exception {
        Log.d(TAG, this.getName() + " " + identifier);

        final ErrorListener errorListener = new ErrorListener();
        final ResultListener resultListener = new ResultListener();
        final MatchListener matchListener = new MatchListener();
        byte[] data;
        File pcmFile = null;

        int ret = mVoicePrintManager.match(identifier, matchListener);
        Thread.sleep(3000);
        if (VoicePrintError.NO_ERROR != ret) {
            failTest("match returns error: " + ret);
        } else if (mErrorOccured) {
            failTest("Match failed. See log for details");
        }

        Log.d(TAG, "startRecording");

        mVoicePrintManager.onStartAudioCapture(VoicePrintTestConst.AUDIO_SAMPLE_RATE,
                AudioFormat.CHANNEL_IN_MONO, AudioFormat.ENCODING_PCM_16BIT, errorListener);

        Thread.sleep(500);
        if (errorListener.await()) {
            failTest("Error occured. See log for details");
        }

        try {
            pcmFile = new File(match_pcm_file);
        } catch (Exception e) {
            failTest("Match pcm file: " + match_pcm_file + " has exception");
        }

        if (match_pcm_file != null) {
            Log.d(TAG, "Using data from " + match_pcm_file);
            FileInputStream pcmFileIn = new FileInputStream(pcmFile);
            int startByteCnt = 0, fileLength = (int) pcmFile.length();
            data = new byte[1280];
            while (startByteCnt + data.length < fileLength) {
                pcmFileIn.read(data, 0, data.length);
                startByteCnt += data.length;
                mVoicePrintManager.addFrame(data);
                Thread.sleep(100);
            }
            pcmFileIn.read(data, 0, fileLength - startByteCnt);
            mVoicePrintManager.addFrame(data);
            pcmFileIn.close();
        } else {
            Log.d(TAG, "Using random data");
            data = randomData;
            mVoicePrintManager.addFrame(data);
        }

        Thread.sleep(200);
        mVoicePrintManager.onStopAudioCapture(resultListener);
        Thread.sleep(5000);

        if (matchListener.await()) {
            mTestPassed = !mErrorOccured;
            if (!mTestPassed) {
                failTest("Match failed. See log for details");
            }
        } else {
            failTest("Timeout without receive callback");
        }
    }

    public void testEnrollData() throws Exception {
        Log.d(TAG, this.getName() + " " + identifier);

        EnrollListener enrollListener = new EnrollListener();
        ErrorListener errorListener = new ErrorListener();
        ResultListener resultListener = new ResultListener();
        byte[] data;
        File pcmFolder = null;


        int ret = mVoicePrintManager.enroll(identifier, phrase, VoicePrintManager.THRESHOLD_HIGH, enrollListener);
        Thread.sleep(3000);
        if (VoicePrintError.NO_ERROR != ret) {
            failTest("enroll returns error: " + ret);
        } else if (mErrorOccured) {
            failTest("Enroll failed. See log for details");
        }

        try {
            pcmFolder = new File(enroll_pcm_folder);
            if (!pcmFolder.isDirectory()) {
                failTest("enroll pcm folder path: " + enroll_pcm_folder + " is not a directory");
            }
        } catch (Exception e) {
            failTest("enroll pcm folder path: " + enroll_pcm_folder + " caused exception");
        }

        File[] pcmFiles = pcmFolder.listFiles();

        for (int i = 0; i < pcmFiles.length; i++) {
            Log.d(TAG, "onStartAudioCapture");
            mVoicePrintManager.onStartAudioCapture(VoicePrintTestConst.AUDIO_SAMPLE_RATE,
                    AudioFormat.CHANNEL_IN_MONO, AudioFormat.ENCODING_PCM_16BIT, errorListener);
            Thread.sleep(300);

            if (errorListener.await()) {
                failTest("Error occured. See log for details");
            }

            File pcmFile = pcmFiles[i];
            Log.d(TAG, "Using data from " + pcmFile.getAbsolutePath());
            mTestLog.writeTestLog("pcm file: " + pcmFile.getAbsolutePath() + "\n");

            FileInputStream pcmFileIn = new FileInputStream(pcmFile);
            int startByteCnt = 0, fileLength = (int) pcmFile.length();
            data = new byte[1280];
            int cnt = 0;
            while (startByteCnt + data.length < fileLength) {
                pcmFileIn.read(data, 0, data.length);
                startByteCnt += data.length;
                mVoicePrintManager.addFrame(cnt++, data);
                Thread.sleep(10);
            }
            pcmFileIn.read(data, 0, fileLength - startByteCnt);
            mVoicePrintManager.addFrame(cnt, data);
            pcmFileIn.close();

            mVoicePrintManager.onStopAudioCapture(resultListener);
            Thread.sleep(5000);
            if (!resultListener.await()) {
                failTest("Timeout without receiving onResult");
            }
        }

        mVoicePrintManager.completeEnroll(errorListener);
        Thread.sleep(3000);
        if (errorListener.await()) {
            failTest("Error occured. See log for details");
        }

        if (enrollListener.await()) {
            mTestPassed = !mErrorOccured;
            if (!mTestPassed) {
                failTest("Enroll failed. See log for details");
            }
        } else {
            failTest("Timeout without receiving onEnroll");
        }
    }

    public void testMatchMic() throws Exception {
        Log.d(TAG, this.getName() + " " + identifier);

        final ErrorListener errorListener = new ErrorListener();
        final ResultListener resultListener = new ResultListener();
        final MatchListener matchListener = new MatchListener();
        long timeRequested, timeDiff;
        AudioState state;

        IntentFilter filter = new IntentFilter(
                "com.qualcomm.qti.biometrics.voiceprinttest.intent.TEST");
        mContext.registerReceiver(mReceiver, filter);

        OnAudioStateChangeListener matchAudioStateChangeListener = new OnAudioStateChangeListener() {

            @Override
            public void onStateChange(AudioState state) {
                Log.d(TAG, "onStateChange: " + state);

                if (state == AudioState.Recording) {
                    int ret = mVoicePrintManager.match(identifier, matchListener);
                    if (VoicePrintError.NO_ERROR != ret) {
                        failTest("match returns error: " + ret);
                    }
                    mVoicePrintManager.onStartAudioCapture(VoicePrintTestConst.AUDIO_SAMPLE_RATE,
                            AudioFormat.CHANNEL_IN_MONO, AudioFormat.ENCODING_PCM_16BIT,
                            errorListener);
                } else if (state == AudioState.Stopping) {
                    mVoicePrintManager.onStopAudioCapture(resultListener);
                }
            }

            @Override
            public void onError(AudioError error) {
                Log.e(TAG, "AudioStateListener error: " + error);
                mTestLog.writeTestLog("AudioStateListener error: " + error + "\n");
                mErrorOccured = true;
            }

            @Override
            public void onAudioCapture(byte[] buffer, float energy) {
                Log.d(TAG, "onAudioCapture");
                mVoicePrintManager.addFrame(buffer);
            }
        };

        state = mAudioRecordManager.getState();
        if (state != OnAudioStateChangeListener.AudioState.Idle) {
            failTest("Expect audio state " + OnAudioStateChangeListener.AudioState.Idle + " actual audio state " + state);
        }

        timeRequested = System.currentTimeMillis();
        timeDiff = 0;
        while (!mStartMic) {
            if (timeDiff < VoicePrintTestConst.MIC_REQUEST_TIMEOUT) {
                Log.d(TAG, "waiting for microphone starts " + timeDiff);
                Thread.sleep(100);
                timeDiff = System.currentTimeMillis() - timeRequested;
            } else {
                Log.d(TAG, "Never received microphone start request");
                mStartMic = true;
            }
        }

        mStartMic = false;
        mAudioRecordManager.startRecording(matchAudioStateChangeListener);

        Log.d(TAG, "startRecording");
        Thread.sleep(3000);
        state = mAudioRecordManager.getState();
        if (state != OnAudioStateChangeListener.AudioState.Recording) {
            failTest("Expect audio state " + OnAudioStateChangeListener.AudioState.Recording + " actual audio state " + state);
        } else if (errorListener.await()) {
            failTest("Error occured. See log for details");
        } else if (mErrorOccured) {
            failTest("Match failed. See log for details");
        }

        timeRequested = System.currentTimeMillis();
        timeDiff = 0;
        while (!mStopMic) {
            if (timeDiff < VoicePrintTestConst.MIC_REQUEST_TIMEOUT) {
                Log.d(TAG, "waiting for microphone stops " + timeDiff);
                Thread.sleep(100);
                timeDiff = System.currentTimeMillis() - timeRequested;
            } else {
                Log.d(TAG, "Never received microphone stop request");
                mStopMic = true;
            }
        }

        mStopMic = false;
        mAudioRecordManager.stopRecording();
        mContext.unregisterReceiver(mReceiver);
        Thread.sleep(3000);

        if (matchListener.await()) {
            mTestPassed = !mErrorOccured;
            if (!mTestPassed) {
                failTest("Match failed. See log for details");
            }
        } else {
            failTest("Timeout without receive callback");
        }
    }

    public void testEnrollMic() throws Exception {
        Log.d(TAG, this.getName() + " " + identifier);

        mAudioRecordManager = new AudioRecordManager(getContext());

        EnrollListener enrollListener = new EnrollListener();
        ErrorListener completeEnrollErrorListener = new ErrorListener();
        final ErrorListener audioErrorListener = new ErrorListener();
        final ResultListener resultListener = new ResultListener();
        long timeRequested, timeDiff;
        AudioState state;

        IntentFilter filter = new IntentFilter(
                "com.qualcomm.qti.biometrics.voiceprinttest.intent.TEST");
        mContext.registerReceiver(mReceiver, filter);

        state = mAudioRecordManager.getState();
        if (state != OnAudioStateChangeListener.AudioState.Idle) {
            failTest("Expect audio state " + OnAudioStateChangeListener.AudioState.Idle + " actual audio state " + state);
        }

        int ret = mVoicePrintManager.enroll(identifier, phrase, VoicePrintManager.THRESHOLD_HIGH, enrollListener);
        if (VoicePrintError.NO_ERROR != ret) {
            failTest("enroll returns error: " + ret);
        }

        OnAudioStateChangeListener enrollAudioStateChangeListener = new OnAudioStateChangeListener() {

            @Override
            public void onStateChange(AudioState state) {
                Log.d(TAG, "onStateChange: " + state);
                if (state == AudioState.Recording) {
                    mVoicePrintManager.onStartAudioCapture(VoicePrintTestConst.AUDIO_SAMPLE_RATE,
                            AudioFormat.CHANNEL_IN_MONO, AudioFormat.ENCODING_PCM_16BIT,
                            audioErrorListener);
                } else if (state == AudioState.Stopping) {
                    mVoicePrintManager.onStopAudioCapture(resultListener);
                }
            }

            @Override
            public void onError(AudioError error) {
                Log.e(TAG, "AudioStateListener error: " + error);
                mTestLog.writeTestLog("AudioStateListener error: " + error + "\n");
            }

            @Override
            public void onAudioCapture(byte[] buffer, float energy) {
                Log.d(TAG, "onAudioCapture");
                mVoicePrintManager.addFrame(buffer);
            }
        };

        timeRequested = System.currentTimeMillis();
        timeDiff = 0;
        while (!mStartMic) {
            if (timeDiff < VoicePrintTestConst.MIC_REQUEST_TIMEOUT) {
                Log.d(TAG, "waiting for microphone starts " + timeDiff);
                Thread.sleep(100);
                timeDiff = System.currentTimeMillis() - timeRequested;
            } else {
                Log.d(TAG, "Never received microphone start request");
                mStartMic = true;
            }
        }

        mStartMic = false;

        Log.d(TAG, "startRecording");
        mAudioRecordManager.startRecording(enrollAudioStateChangeListener);
        Thread.sleep(3000);

        state = mAudioRecordManager.getState();
        if (state != OnAudioStateChangeListener.AudioState.Recording) {
            failTest("Expect audio state " + OnAudioStateChangeListener.AudioState.Recording + " actual audio state " + state);
        } else if (audioErrorListener.await()) {
            failTest("Error occured. See log for details");
        } else if (mErrorOccured) {
            failTest("Enroll failed. See log for details");
        }

        timeRequested = System.currentTimeMillis();
        timeDiff = 0;
        while (!mStopMic) {
            if (timeDiff < VoicePrintTestConst.MIC_REQUEST_TIMEOUT) {
                Log.d(TAG, "waiting for microphone stops " + timeDiff);
                Thread.sleep(100);
                timeDiff = System.currentTimeMillis() - timeRequested;
            } else {
                Log.d(TAG, "Never received microphone stop request");
                mStopMic = true;
            }
        }

        mStopMic = false;

        mAudioRecordManager.stopRecording();
        mContext.unregisterReceiver(mReceiver);
        mVoicePrintManager.completeEnroll(completeEnrollErrorListener);
        Thread.sleep(3000);

        if (completeEnrollErrorListener.await()) {
            failTest("Error occured. See log for details");
        } else if (!resultListener.await()) {
            failTest("Timeout without receive onResult callback");
        } else if (enrollListener.await()) {
            failTest("Timeout without receive onEnroll callback");
        } else {
            mTestPassed = !mErrorOccured;
            if (!mTestPassed) {
                failTest("Enroll failed. See log for details");
            }
        }
    }

    public void testDelete() throws Exception {
        Log.d(TAG, this.getName() + " " + identifier);

        ResultListener resultListener = new ResultListener();

        int ret = mVoicePrintManager.delete(identifier, resultListener);
        if (VoicePrintError.NO_ERROR != ret) {
            failTest("delete returns error: " + ret);
        }

        Thread.sleep(1000);
        if (resultListener.await()) {
            mTestPassed = !mErrorOccured;
            if (!mTestPassed) {
                failTest("Delete failed. See log for details");
            }
        } else {
            failTest("Timeout without receiving onResult");
        }
    }

    public void testDeleteAll() throws Exception {
        Log.d(TAG, this.getName() + " " + identifier);

        ResultListener resultListener = new ResultListener();

        int ret = mVoicePrintManager.deleteAll(resultListener);
        if (VoicePrintError.NO_ERROR != ret) {
            failTest("deleteAll returns error: " + ret);
        }

        Thread.sleep(1000);
        if (resultListener.await()) {
            mTestPassed = !mErrorOccured;
            if (!mTestPassed) {
                failTest("DeleteAll failed. See log for details");
            }
        } else {
            failTest("Timeout without receiving onResult");
        }
    }

    public void testSetThreshold() throws Exception {
        Log.d(TAG, this.getName() + " " + identifier);

        int thresholdVal = VoicePrintManager.THRESHOLD_HIGH;
        if (threshold != "") {
            thresholdVal = Integer.parseInt(threshold);
        }

        int ret = mVoicePrintManager.setThreshold(identifier, thresholdVal);
        if (VoicePrintError.NO_ERROR == ret) {
            Log.d(TAG, "setThreshold to " + thresholdVal);
            mTestLog.writeTestLog("setThreshold to " + thresholdVal + "\n");
            mTestPassed = true;
        } else {
            failTest("setThreshold returns error: " + ret);
        }
    }
}
