/*
 * Copyright (c) 2013-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.sva;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder.AudioSource;
import android.util.Log;


public class ListenAudioRecorder
{
    private static final String TAG = "ListenLog.ListenAudioRecorder";

    public static ListenAudioRecorder getInstance() {
        return new ListenAudioRecorder(
                AudioSource.MIC,
                16000,
                AudioFormat.CHANNEL_IN_MONO,
                AudioFormat.ENCODING_PCM_16BIT);
    }

    // Overloaded getInstance to accommodate session ID.
    public static ListenAudioRecorder getInstance(int inSessionId) {
        return new ListenAudioRecorder(
                AudioSource.MIC,
                16000,
                AudioFormat.CHANNEL_IN_MONO,
                AudioFormat.ENCODING_PCM_16BIT);
    }

    /**
     * INITIALIZING : recorder is in the process of initializing
     * READY : recorder has been initialized, but not yet started
     * RECORDING : recorder is in the process of recording
     * ERROR : recorder needs to be reinitialized
     * STOPPED: recorder needs to be reset
     */
    public enum State {INITIALIZING, READY, RECORDING, ERROR, STOPPED};

    // The interval for outputting the recorded samples to the file
    public static final int TIMER_INTERVAL = 120;
    public static final short NUM_CHANNELS = 1; // mono
    public static final int BITS_PER_SAMPLE = 16; // in bits
    public static final int SAMPLE_RATE = 16000; // 16kHz

    // The recorder
    private AudioRecord     audioRecorder = null;

    // Temp output file path
    private String          tempFilePath = Global.PATH_RECORDINGS_TEMP_FILE;

    private State                   state; // Recorder state. See State enum.


    private int                     bufferSize;

    // Number of frames written to file on each output
    private int                      framePeriod;

    private Thread recordingThread = null;

    /**
     *
     * Instantiates a new recorder.
     * If there are errors, no exception is thrown, but the state is set to ERROR.
     *
     */
    public ListenAudioRecorder(int inAudioSource, int inSampleRate, int inChannelConfig,
            int inAudioFormat)
    {
        Log.v(TAG, "ListenAudioRecorder constructor");
        try
        {
            framePeriod = inSampleRate * TIMER_INTERVAL / 1000;
            bufferSize = framePeriod * 2 * BITS_PER_SAMPLE * NUM_CHANNELS / 8;
            if (bufferSize < AudioRecord.getMinBufferSize(inSampleRate, inChannelConfig, inAudioFormat))
            {
                // Check to make sure buffer size is not smaller than the smallest allowed
                bufferSize = AudioRecord.getMinBufferSize(inSampleRate, inChannelConfig, inAudioFormat);
                // Set frame period and timer interval accordingly
                framePeriod = bufferSize / ( 2 * BITS_PER_SAMPLE * NUM_CHANNELS / 8 );
                Log.v(TAG, "Increasing buffer size to " + Integer.toString(bufferSize));
            }

            audioRecorder = new AudioRecord(inAudioSource, inSampleRate, inChannelConfig, inAudioFormat, bufferSize);
            Log.v(TAG, "ListenAudioRecorder constructor: constructed");

            if (audioRecorder.getState() != AudioRecord.STATE_INITIALIZED) {
                throw new Exception("AudioRecord initialization failed");
            }
            state = State.INITIALIZING;
            Log.v(TAG, "ListenAudioRecorder constructor: finished");
        } catch (Exception e) {
            if (e.getMessage() != null) {
                Log.e(TAG, e.getMessage());
            } else {
                Log.e(TAG, "Unknown error occured while initializing recording");
            }
            state = State.ERROR;
        }
    }


    /**
     *
     * Returns the state of the recorder.
     *
     * @return recorder state
     *
     */
    public State getState()
    {
        return state;
    }

    /**
     *
     * Starts recording and sets the state of the recorder to RECORDING.
     *
     */
    public void start()
    {
        Log.v(TAG, "start");
        Log.v(TAG, "start: in state = READY");
        audioRecorder.startRecording();
        Log.v(TAG, "start: started recording, audioRecorder state= " +
                audioRecorder.getRecordingState());
        state = State.RECORDING;

        recordingThread = new Thread(new Runnable() {

            @Override
            public void run() {
                writeAudioDataToFile();
            }
        },"AudioRecorder Thread");

        recordingThread.start();
    }


    // Wav output is used for ease of debugging recordings
    private void writeAudioDataToFile(){
        Log.v(TAG, "writeAudioDataToFile");
        String filename = tempFilePath;
        FileOutputStream os = null;

        try {
            os = new FileOutputStream(filename, false);
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }

        int read = 0;
        if(os != null){
            while(state == State.RECORDING){
                byte[] data = new byte[bufferSize];
                read = audioRecorder.read(data, 0, bufferSize);
                Log.v(TAG, "writeAudioDataToFile: read = " + read);
                if(AudioRecord.ERROR_INVALID_OPERATION != read) {
                    try {
                        os.write(data);
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
            }

            try {
                os.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    /**
     *
     * Stops recording, releases recorder, and sets the state of the recorder to STOPPED.
     * Finalizes the wave file.
     *
     */
    public void stop(String inFilePath)
    {
        Log.v(TAG, "stop");
        if (state == State.RECORDING)
        {
            audioRecorder.stop();
            audioRecorder.release();
            state = State.STOPPED;
            String filePath = inFilePath;
            copyWavFile(tempFilePath, filePath);
            deleteTempFile();
        } else {
            Log.v(TAG, "stop() called but not recording");
            state = State.ERROR;
        }
    }

    private void copyWavFile(String inFilename, String outFilename){
        FileInputStream in = null;
        FileOutputStream out = null;
        long totalAudioLen = 0;
        long totalDataLen = totalAudioLen + 44;


        byte[] data = new byte[bufferSize];

        try {
            in = new FileInputStream(inFilename);
            out = new FileOutputStream(outFilename);
            totalAudioLen = in.getChannel().size();
            totalDataLen = totalAudioLen + 44;

            byte[] wavHeader = Utils.getWavHeader(totalAudioLen, totalDataLen);
            out.write(wavHeader, 0, 44);

            while(in.read(data) != -1){
                out.write(data);
            }

            in.close();
            out.close();
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
        Global.getInstance().addUserRecording();
    }

    private void deleteTempFile() {
        Utils.deleteFile(tempFilePath);
    }
}
