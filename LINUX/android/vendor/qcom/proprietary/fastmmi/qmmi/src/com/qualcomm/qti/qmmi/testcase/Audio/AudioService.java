/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.testcase.Audio;

import android.content.Intent;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioRecord;
import android.media.AudioSystem;
import android.media.MediaPlayer;
import android.media.MediaRecorder;

import com.qualcomm.qti.qmmi.R;
import com.qualcomm.qti.qmmi.bean.TestCase;
import com.qualcomm.qti.qmmi.framework.BaseRunnable;
import com.qualcomm.qti.qmmi.framework.BaseService;
import com.qualcomm.qti.qmmi.model.HidlManager;
import com.qualcomm.qti.qmmi.utils.FileUtils;
import com.qualcomm.qti.qmmi.utils.LogUtils;
import com.qualcomm.qti.qmmi.utils.ShellUtils;
import com.qualcomm.qti.qmmi.utils.Utils;

import java.io.BufferedOutputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

public class AudioService extends BaseService {

    public final static String RECORD_FILE_FOR_PRIMARY_MIC = "primary_mic_record.pcm";
    public final static String RECORD_FILE_FOR_HEADSET_MIC = "headset_mic_record.pcm";
    public final static String RECORD_FILE_FOR_NOISE_MIC = "noise_mic_record.pcm";
    public final static String RECORD_FILE_FOR_TOP_MIC = "top_mic_record.pcm";
    private AudioManager mAudioManager = null;
    private HidlManager hidlManager = null;
    private Thread mThread = null;
    private BaseRunnable mRunnable = null;
    private TestCase mTestCase = null;
    private String CASE_NAME = null;

    //silent control
    private int mPreRingMode = -1;

    public static final int USE_SPEAKER = 0;
    public static final int USE_HANDSET = 1;
    public static final int USE_HEADSET = 2;

    /*
    *   AUDIO_SPEAKER_PLAYTONE,
        AUDIO_HEADSET_PLAYTONE,
        AUDIO_HANDSET_PLAYTONE,
        AUDIO_SPEAKER_PLAY_FILE,
        AUDIO_HEADSET_PLAY_FILE,
        AUDIO_HANDSET_PLAY_FILE,
        AUDIO_PRIMARY_MIC_RECORD,
        AUDIO_HEADSET_MIC_RECORD,
        AUDIO_NOISE_MIC_RECORD,
        AUDIO_TOP_MIC_RECORD,

        AUDIO_HANDSET_PRIMARY_MIC_LOOPBACK,
        AUDIO_SPEAKER_PRIMARY_MIC_LOOPBACK,
        AUDIO_HANDSET_PRIMARY_MIC_LOOPBACK,
        AUDIO_HANDSET_HEADSET_MIC_LOOPBACK,
        AUDIO_HEADSET_SPEAKER_HEADSET_MIC_LOOPBACK,
        AUDIO_HEADSET_SPEAKER_PRIMARY_MIC_LOOPBACK,
        AUDIO_HEADSET_MIC_HEADSET_SPEAKER_LOOPBACK,
        AUDIO_HANDSET_MIC_HANDSET_SPEAKER_LOOPBACK
    * */
    public static String[] supported_methods = {
            /***/
            "speaker_play_tone",
            /***/
            "headset_play_tone",
            /***/
            "handset_play_tone",
            /***/
            "speaker_play_file",
            /***/
            "headset_play_file",
            /***/
            "handset_play_file",
            /***/
            "primary_mic_record",
            /***/
            "headset_mic_record",
            /***/
            "noise_mic_record",
            /***/
            "top_mic_record",
            /***/
            "speaker_primary_mic_loopback",
            /***/
            "handset_primary_mic_loopback",
            /***/
            "handset_headset_mic_loopback",
            /***/
            "headset_speaker_headset_mic_loopback",
            /***/
            "headset_speaker_primary_mic_loopback",
            /***/
            "headset_mic_headset_speaker_loopback",
            /***/
            "handset_mic_handset_speaker_loopback",
            /***/
            "mm_audio_ftm"
    };

    void enterSilent() {
        if (mAudioManager != null) {
            mPreRingMode = mAudioManager.getRingerMode();
            mAudioManager.setRingerMode(AudioManager.RINGER_MODE_SILENT);
        } else {
            //every time return to unavailable
            mPreRingMode = -1;
        }
    }

    void exitSilent() {
        if (mAudioManager != null && mPreRingMode >= 0) {
            mAudioManager.setRingerMode(mPreRingMode);
        }
    }

    boolean isMethodSupported(String method) {
        for (String m : supported_methods) {
            if (method.equalsIgnoreCase(m))
                return true;
        }
        return false;
    }

    private boolean initService() {
        mAudioManager = (AudioManager) getSystemService(AUDIO_SERVICE);
        if (mAudioManager == null) {
            LogUtils.loge("get audio service fail");
            return false;
        }

        hidlManager = HidlManager.getInstance();
        if (hidlManager == null) {
            LogUtils.loge("No hidl manager found");
        }

        return true;
    }

    private void startPlayAudioFile(final String sourceFilePath) {
        LogUtils.logi("start play sourceFilePath=" + sourceFilePath);

        if (sourceFilePath == null || !FileUtils.isFileExist(sourceFilePath)) {
            LogUtils.loge("file not exist, " + sourceFilePath);
            return;
        }

        mRunnable = new BaseRunnable() {
            @Override
            public void run() {

                try {
                    MediaPlayer mediaPlayer = new MediaPlayer();
                    mediaPlayer.setDataSource(sourceFilePath);
                    mediaPlayer.prepare();

                    mediaPlayer.setLooping(true);
                    mediaPlayer.start();

                    updateView(CASE_NAME, getApplicationContext().getResources().getString(R.string.audio_hear_sound));
                    LogUtils.logi("play ongoing");
                    setRunning(true);
                    while (getRunning()) {

                    }

                    mediaPlayer.stop();
                    mediaPlayer.release();
                    mediaPlayer = null;

                    LogUtils.logi("play stopped");
                } catch (Exception e) {
                    LogUtils.loge("exception found");
                }
            }
        };

        mThread = new Thread(mRunnable);
        mThread.start();
    }


    private void stopThread() {
        LogUtils.logi("stop audio test ... ");
        mAudioManager.setMode(AudioManager.MODE_NORMAL);
        AudioSystem.setDeviceConnectionState(
                AudioSystem.DEVICE_OUT_WIRED_HEADSET,
                AudioSystem.DEVICE_STATE_UNAVAILABLE, "", "");
        AudioSystem.setForceUse(AudioSystem.FOR_MEDIA,
                AudioSystem.FORCE_NONE);

        if (mRunnable != null && mThread != null) {
            try {
                mRunnable.stop();
                mThread.join();
                mThread = null;
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }

    private void setAudioParam(int type) {

        if (mAudioManager == null) {
            initService();
        }

        float ratio = 0.0f;
        if (type == USE_SPEAKER) {
            mAudioManager.setMode(AudioManager.MODE_NORMAL);
            AudioSystem.setForceUse(AudioSystem.FOR_MEDIA,
                    AudioSystem.FORCE_SPEAKER);
            ratio = 1.0f;
        } else if (type == USE_HANDSET) {
            mAudioManager.setMode(AudioManager.MODE_IN_COMMUNICATION);
            AudioSystem.setForceUse(AudioSystem.FOR_COMMUNICATION,
                    AudioSystem.FORCE_HEADPHONES);

            ratio = 0.6f;
        } else {
            mAudioManager.setMode(AudioManager.MODE_NORMAL);
            AudioSystem.setDeviceConnectionState(
                    AudioSystem.DEVICE_OUT_WIRED_HEADSET,
                    AudioSystem.DEVICE_STATE_AVAILABLE, "", "");
            AudioSystem.setForceUse(AudioSystem.FOR_MEDIA,
                    AudioSystem.FORCE_WIRED_ACCESSORY);
            ratio = 0.6f;
        }


        mAudioManager.setStreamVolume(AudioManager.STREAM_MUSIC,
                (int) (ratio * mAudioManager
                        .getStreamMaxVolume(AudioManager.STREAM_MUSIC)), 0);
        mAudioManager
                .setStreamVolume(
                        AudioManager.STREAM_VOICE_CALL,
                        (int) (ratio * mAudioManager
                                .getStreamMaxVolume(AudioManager.STREAM_VOICE_CALL)),
                        0);
        mAudioManager.setStreamVolume(AudioManager.STREAM_RING,
                (int) (ratio * mAudioManager
                        .getStreamMaxVolume(AudioManager.STREAM_RING)), 0);
        mAudioManager.setStreamVolume(AudioManager.STREAM_SYSTEM,
                (int) (ratio * mAudioManager
                        .getStreamMaxVolume(AudioManager.STREAM_SYSTEM)), 0);
    }

    /*
      * Play tone via speaker
      * **/
    private void speakerPlayTone() {
        setAudioParam(USE_SPEAKER);
        startPlayAudioFile(Utils.TONE_1KHZ_FILE_PATH);
    }

    /*
    * Play tone via handset
    * **/
    private void handsetPlayTone() {
        setAudioParam(USE_HANDSET);
        startPlayAudioFile(Utils.TONE_1KHZ_FILE_PATH);
    }

    /*
    * Play tone via headset
    * **/
    private void headsetPlayTone() {
        setAudioParam(USE_HEADSET);
        startPlayAudioFile(Utils.TONE_1KHZ_FILE_PATH);
    }
    /*
    * Play file via speaker
    * **/

    private void speakerPlayFile() {
        setAudioParam(USE_SPEAKER);
        startPlayAudioFile(Utils.SOUND_FILE_PATH);
    }

    /*
    * Play file via handset
    * **/
    private void handsetPlayFile() {
        setAudioParam(USE_HANDSET);
        startPlayAudioFile(Utils.SOUND_FILE_PATH);
    }

    /*
    * Play file via headset
    * **/
    private void headsetPlayFile() {
        setAudioParam(USE_HEADSET);
        startPlayAudioFile(Utils.SOUND_FILE_PATH);
    }

    /*
    * start record
    * */
    private void startRecord(final String fileName) {

        mRunnable = new BaseRunnable() {
            @Override
            public void run() {
                int frequency = 8000;
                LogUtils.loge("startRecord  .......................");
                int channelConfiguration = AudioFormat.CHANNEL_IN_MONO;
                int audioEncoding = AudioFormat.ENCODING_PCM_16BIT;
                File recordFile = new File(getFilesDir().getAbsolutePath() + "/" + fileName);

                // Delete any previous recording.
                if (recordFile.exists())
                    recordFile.delete();

                // Create the new file.
                try {
                    recordFile.createNewFile();
                } catch (IOException e) {
                    throw new IllegalStateException("Failed to create " + recordFile.toString());
                }

                try {
                    // Create a DataOuputStream to write the audio data into the saved file.
                    BufferedOutputStream bufferedOutputStream = new BufferedOutputStream(new FileOutputStream(recordFile));
                    DataOutputStream dataOutputStream = new DataOutputStream(bufferedOutputStream);

                    // Create a new AudioRecord object to record the audio.
                    int recordBufferSize = AudioRecord.getMinBufferSize(frequency, channelConfiguration, audioEncoding);
                    AudioRecord audioRecord = new AudioRecord(MediaRecorder.AudioSource.MIC,
                            frequency, channelConfiguration,
                            audioEncoding, recordBufferSize);

                    byte[] buffer = new byte[recordBufferSize];
                    audioRecord.startRecording();
                    updateView(CASE_NAME, getApplicationContext().getResources().getString(R.string.audio_speak_mic));
                    setRunning(true);
                    while (getRunning()) {
                        int bufferReadResult = audioRecord.read(buffer, 0, recordBufferSize);
                        if (bufferReadResult > 0) {
                            dataOutputStream.write(buffer, 0, bufferReadResult);
                        }
                    }
                    LogUtils.loge("Recording stop ........");
                    audioRecord.stop();
                    audioRecord.release();
                    audioRecord = null;
                    dataOutputStream.close();

                } catch (Throwable t) {
                    LogUtils.loge("Recording Failed");
                }
            }
        };

        mThread = new Thread(mRunnable);
        mThread.start();
    }

    /*
   * record audio from primary mic
   * **/
    private void primaryMicRecord() {
        setAudioParam(USE_HANDSET);
        startRecord(RECORD_FILE_FOR_PRIMARY_MIC);
    }

    /*
  * record audio from headset mic
  * **/
    private void headsetMicRecord() {
        setAudioParam(USE_HEADSET);
        startRecord(RECORD_FILE_FOR_HEADSET_MIC);
    }

    /*
  * record audio from noise mic
  * **/
    private void noiseMicRecord() {
        setAudioParam(USE_HANDSET);
        startRecord(RECORD_FILE_FOR_NOISE_MIC);
    }

    /*
  * record audio from top mic
  * **/
    private void topMicRecord() {
        setAudioParam(USE_HANDSET);
        startRecord(RECORD_FILE_FOR_TOP_MIC);
    }

    /*
    *
    *  Handset Loopback
    * */
    public void handsetMicHandsetSpeakerLoopback() {
        LogUtils.logi("start handsetLoopbackTread");
        setAudioParam(USE_HANDSET);

        mRunnable = new LoopbackRunnable();
        mThread = new Thread(mRunnable);
        mThread.start();
        updateView(CASE_NAME, this.getResources().getString(R.string.audio_speak_handset));

    }

    /**
     * Headset Loopback
     **/
    public void headsetMicHeadsetSpeakerLoopback() {
        LogUtils.logi("start headsetLoopbackTread");
        setAudioParam(USE_HEADSET);

        mRunnable = new LoopbackRunnable();
        mThread = new Thread(mRunnable);
        mThread.start();
        updateView(CASE_NAME, this.getResources().getString(R.string.audio_speak_headset));
    }


    /**
     * Headset Loopback
     **/
    public void audioTestByMMAudioFTMDaemon(TestCase testCase) {
        LogUtils.logi("start audioTestByMMAudioFTMDaemon");
        StringBuffer sb = new StringBuffer();

        //add duration
        if (testCase.getParameter().get("duration") != null) {
            sb.append("-d;");
            sb.append(testCase.getParameter().get("duration") + ";");
        }

        //add duration
        if (testCase.getParameter().get("volume") != null) {
            sb.append("-v;");
            sb.append(testCase.getParameter().get("volume") + ";");
        }

        //add fl if fl
        if (testCase.getParameter().get("fl") != null) {
            sb.append("-fl;");
            sb.append(testCase.getParameter().get("fl") + ";");
        }

        //add fh if any
        if (testCase.getParameter().get("fh") != null) {
            sb.append("-fh;");
            sb.append(testCase.getParameter().get("fh") + ";");
        }

        //add txvolume if any
        if (testCase.getParameter().get("txvolume") != null) {
            sb.append("-tv;");
            sb.append(testCase.getParameter().get("txvolume") + ";");
        }

        //add rxvolume if any
        if (testCase.getParameter().get("rxvolume") != null) {
            sb.append("-rv;");
            sb.append(testCase.getParameter().get("rxvolume") + ";");
        }

        //add analyze if any
        if (testCase.getParameter().get("analyze") != null) {
            sb.append("-analyze;");
            sb.append(testCase.getParameter().get("analyze") + ";");
        }

        //add fdiff if any
        if (testCase.getParameter().get("fdiff") != null) {
            sb.append("-fdiff;");
            sb.append(testCase.getParameter().get("fdiff") + ";");
        }

        //add file if any
        if (testCase.getParameter().get("file") != null) {
            sb.append("-file;");
            sb.append(testCase.getParameter().get("file") + ";");
        }

        if (testCase.getParameter().get("tc") == null) {
            LogUtils.loge("No TC param, exit");
            return;
        }

        sb.append("-tc;");
        sb.append(testCase.getParameter().get("tc"));

        if (hidlManager != null) {

            //run the command
            hidlManager.runAudioApp(sb.toString());

            if (testCase.getParameter().get("is_play") != null && testCase.getParameter().get("is_play").equalsIgnoreCase("true")) {
                StringBuffer playBuffer = new StringBuffer();

                //add duration
                if (testCase.getParameter().get("play_duration") != null) {
                    playBuffer.append("-d;");
                    playBuffer.append(testCase.getParameter().get("duration") + ";");
                }

                //add duration
                if (testCase.getParameter().get("play_volume") != null) {
                    playBuffer.append("-v;");
                    playBuffer.append(testCase.getParameter().get("volume") + ";");
                }

                //add file if any
                if (testCase.getParameter().get("file") != null) {
                    playBuffer.append("-file;");
                    playBuffer.append(testCase.getParameter().get("file") + ";");
                }

                //add tc
                if (testCase.getParameter().get("play_tc") == null) {
                    LogUtils.loge("No play TC param, exit");
                    return;
                }
                playBuffer.append("-tc;");
                playBuffer.append(testCase.getParameter().get("play_tc"));

                //play the command
                hidlManager.runAudioApp(playBuffer.toString());
            }
        }
    }


    public boolean isWireOn() {
        return mAudioManager.isWiredHeadsetOn();
    }

    @Override
    public void register() {

    }

    public int onStartCommand(Intent intent, int flags, int startId) {
        LogUtils.logi("onStartCommand");
        initService();
        return super.onStartCommand(intent, flags, startId);
    }

    @Override
    public int stop(TestCase testCase) {
        if (testCase == null) {
            LogUtils.loge("Null param, please check the framework error");
            return -1;
        }

        if (hidlManager != null) {
            hidlManager.stopAudioApp();
        }

        //exist selent,restore pre mode back
        String method = testCase.getMethod();
        if (method != null && isMethodSupported(method)) {
            if (method.equalsIgnoreCase("mm_audio_ftm")) {
                exitSilent();
            }
        }

        stopThread();
        return 0;
    }

    @Override
    public int run(TestCase testCase) {

        mTestCase = testCase;
        if (mTestCase == null) {
            LogUtils.loge("Null param, please check the framework error");
            return -1;
        }

        CASE_NAME = mTestCase.getName();
        String method = mTestCase.getMethod();
        if (method != null && isMethodSupported(method)) {
            if (method.equalsIgnoreCase("speaker_play_tone")) {
                speakerPlayTone();
            } else if (method.equalsIgnoreCase("headset_play_tone")) {
                headsetPlayTone();
            } else if (method.equalsIgnoreCase("handset_play_tone")) {
                handsetPlayTone();
            } else if (method.equalsIgnoreCase("speaker_play_file")) {
                speakerPlayFile();
            } else if (method.equalsIgnoreCase("headset_play_file")) {
                headsetPlayFile();
            } else if (method.equalsIgnoreCase("handset_play_file")) {
                handsetPlayFile();
            } else if (method.equalsIgnoreCase("primary_mic_record")) {
                primaryMicRecord();
            } else if (method.equalsIgnoreCase("headset_mic_record")) {
                headsetMicRecord();
            } else if (method.equalsIgnoreCase("noise_mic_record")) {
                noiseMicRecord();
            } else if (method.equalsIgnoreCase("top_mic_record")) {
                topMicRecord();
            } else if (method.equalsIgnoreCase("handset_mic_handset_speaker_loopback")) {
                LogUtils.logi("Audio Handset loopback run");
                handsetMicHandsetSpeakerLoopback();
            } else if (method.equalsIgnoreCase("headset_mic_headset_speaker_loopback")) {
                LogUtils.logi("Audio Headset loopback run");
                headsetMicHeadsetSpeakerLoopback();
            } else if (method.equalsIgnoreCase("mm_audio_ftm")) {
                enterSilent();
                audioTestByMMAudioFTMDaemon(testCase);
            }
        } else {
            LogUtils.loge("no method , or not support method");
        }
        LogUtils.logi("Audio service run");
        return 0;
    }

}
