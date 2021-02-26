/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.audio;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioRecord;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.telephony.PhoneStateListener;
import android.telephony.TelephonyManager;
import android.util.Log;

import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.Consts;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import android.media.AudioTrack;
import java.util.Arrays;

/**
 * The Class AudioRecordManager.
 */
public class AudioRecordManager {

    /**
     * The log TAG.
     */
    private static final String TAG = "VoicePrint";

    /**
     * The service.
     */
    private static AudioRecordManager mInstance = null;
    private BroadcastReceiver mBroadcastReceiver;

    /**
     * Context variable that initiated this Singleton.
     */
    private Context context = null;

    /**
     * The headset attached flag.
     */
    private boolean mHeadsetAttached = false;

    /**
     * The audio record and playback thread.
     */
    private AudioRecordThread mThread = null;

    /**
     * The audio sample rate.
     */
    private int mAudioSampleRate = Consts.DEFAULT_AUDIO_SAMPLE_RATE;

    /**
     * The source type.
     */
    private int mSourceType = Consts.DEFAULT_AUDIO_SOURCE;

    /**
     * The input channel config.
     */
    private int mInputChannelConfig = AudioFormat.CHANNEL_IN_MONO;

    /**
     * Raw input channel.
     */
    private int mInputChannels = 1;


    private boolean mInPhoneCall = false;
    private String mHeadsetName;
    private boolean mHeadsetHasMicrophone;
    private AudioManager mAudioManager;
    private OnAudioStateChangeListener mListener;

    private OnAudioStateChangeListener.AudioState mAudioState = OnAudioStateChangeListener.AudioState.Idle;
    private OnAudioStateChangeListener.AudioState mState;
    private int mAudioFormat = AudioFormat.ENCODING_PCM_16BIT;

    final static int MSG_DATA_UPDATE = 0;
    final static int MSG_STATE_CHANGE = 1;

    /**
     * Gets the service.
     *
     * @return the service
     */
    public static AudioRecordManager getInstance() {
        return mInstance;
    }

    public static void instantiate(Context context) {
        mInstance = new AudioRecordManager(context);
    }

    public AudioRecordManager(Context context) {
        this.context = context;

        // Handle phone calls
        CallStateListener callListener = new CallStateListener();
        TelephonyManager telephonyManager = (TelephonyManager) context
                                            .getSystemService(Context.TELEPHONY_SERVICE);
        telephonyManager.listen(callListener,
                                PhoneStateListener.LISTEN_CALL_STATE);

        IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(Intent.ACTION_HEADSET_PLUG);

        // Receive intents
        mBroadcastReceiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                if (intent == null) {
                    Log.e(TAG, "Intent received is NULL!");
                    return;
                }

                // do something based on the intent's action
                String action = intent.getAction();
                Log.d(TAG, "Received intent " + action);

                if (action.equals(Intent.ACTION_HEADSET_PLUG)) {
                    handleHeadsetIntent(intent);
                }
            }

        };
    }

    private void handleAudioStateChange(Intent intent) {

        if (intent == null) {
            Log.d(TAG, "AudioStateChange intent is null?");
            return;
        }

        mInPhoneCall = true;
    }

    private void handleHeadsetIntent(Intent intent) {
        mHeadsetAttached = (intent.getIntExtra("state", 0) == 1);
        mHeadsetName = (intent.getStringExtra("name"));
        mHeadsetHasMicrophone = (intent
                                 .getIntExtra("microphone", 0) == 1);

        Log.d(TAG, "Headset is plugged=" + mHeadsetAttached
              + ", name=" + mHeadsetName + " hasMic="
              + mHeadsetHasMicrophone);
    }

    private AudioRecordManager() {
        // Must instantiate
    }

    public boolean isRecording() {
        return (mThread != null &&
                mState == OnAudioStateChangeListener.AudioState.Recording);
    }

    public void startRecording() {
        Log.d(TAG, "Starting record thread...");

        if (mInPhoneCall) {
            onError(OnAudioStateChangeListener.AudioError.PhoneCallInProgress);
            return;
        }

        if (mThread == null) {
            mAudioManager = (AudioManager) context
                            .getSystemService(Context.AUDIO_SERVICE);
            mThread = new AudioRecordThread();
            mThread.start();
        } else {
            Log.d(TAG, "Thread already running.");

            onError(OnAudioStateChangeListener.AudioError.AlreadyRunning);

        }
    }

    public void startRecording(OnAudioStateChangeListener listener) {
        mListener = listener;
        startRecording();
    }

    /**
     * Stop.
     */
    public synchronized void stopRecording() {
        // Log.d(TAG, "Stopping thread...");
        Log.d(TAG, "Stopping record thread...");

        setState(OnAudioStateChangeListener.AudioState.Stopping);


        try {
            if (mThread != null) {
                mThread.stopThread();

                try {
                    mThread.join(500);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                mThread = null;
            }
        } catch (Exception e) {
            e.printStackTrace();
            onError(OnAudioStateChangeListener.AudioError.Unknown);

        } finally {
            setState(OnAudioStateChangeListener.AudioState.Idle);
        }
    }

    public OnAudioStateChangeListener.AudioState getState() {
        return mState;
    }


    private void onError(OnAudioStateChangeListener.AudioError error) {
        if (mListener != null) {
            mListener.onError(error);
        }
    }

    /**
     * The listener interface for receiving callState events. The class that is
     * interested in processing a callState event implements this interface, and
     * the object created with that class is registered with a component using
     * the component's <code>addCallStateListener<code> method. When
     * the callState event occurs, that object's appropriate
     * method is invoked.
     */
    private class CallStateListener extends PhoneStateListener {

        /*
         * (non-Javadoc)
         *
         * @see android.telephony.PhoneStateListener#onCallStateChanged(int,
         * java.lang.String)
         */
        @Override
        public void onCallStateChanged(int state, String incomingNumber) {
            mInPhoneCall = (TelephonyManager.CALL_STATE_OFFHOOK == state);

            Log.d(TAG,
                  "Phone state is "
                  + state
                  + ((incomingNumber != null && incomingNumber
                      .length() > 0) ? " from " + incomingNumber
                     : "."));

            // Stop recording if phone call started
            if (mInPhoneCall && isRecording()) {
                onError(OnAudioStateChangeListener.AudioError.PhoneCallInProgress);
                stopRecording();
            }
        }
    }

    /**
     * Sets the headset.
     *
     * @param attached the new headse
     */
    public void setHeadset(boolean attached) {
        mHeadsetAttached = attached;
        Log.d(TAG, "Headset attached = " + mHeadsetAttached);
    }

    /**
     * Sets the source type.
     *
     * @param type the new source type
     */
    public void setSourceType(int type) {
        mSourceType = type;
    }


    public static short[] byteToShort(byte[] byteArray) {
        short[] shortOut = new short[byteArray.length / 2];
        ByteBuffer byteBuffer = ByteBuffer.wrap(byteArray);
        byteBuffer.order(ByteOrder.LITTLE_ENDIAN).asShortBuffer().get(shortOut);
        return shortOut;
    }


    private float getEnergy(byte[] bytes) {

        short[] samples = byteToShort(bytes);

        double sum = 0.0f;
        for (int i = 0; i < samples.length; i++) {
            sum += Math.abs((double) samples[i]);
        }


        return (float) Math.log(sum);
    }

    private void onAudioCapture(int index, byte[] buffer) {
        byte[] audioBuffer = new byte[buffer.length];
        System.arraycopy(buffer, 0, audioBuffer, 0, buffer.length);
        Message msg = mHandler.obtainMessage(MSG_DATA_UPDATE, index, 0, audioBuffer);
        mHandler.sendMessage(msg);
    }

    private void setState(OnAudioStateChangeListener.AudioState state) {

        if (mState == state) {
            return;
        }

        Message msg = mHandler.obtainMessage(MSG_STATE_CHANGE, 0, 0, state);
        mHandler.sendMessage(msg);
    }

    /**
     * The handler.
     */
    Handler mHandler = new Handler(Looper.getMainLooper()) {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
            case MSG_DATA_UPDATE:
                // Do not waste processing time if no listener.
                if (mListener == null ||
                        msg == null ||
                        msg.obj == null) {
                    return;
                }

                if (msg.obj != null) {
                    byte[] buffer = (byte[]) msg.obj;

                    if (mListener != null) {
                        mListener.onAudioCapture(msg.arg1, buffer, getEnergy(buffer));
                    }
                }
                break;
            case MSG_STATE_CHANGE:
                if (mListener != null) {
                    mState = (OnAudioStateChangeListener.AudioState) msg.obj;
                    mListener.onStateChange(mState);
                }
                break;
            default:
                Log.d(TAG, "Not handling msg " + msg.what);
                break;
            }
        }
    };

    /**
     * The Class AudioRecordThread.
     */
    private class AudioRecordThread extends Thread {

        /**
         * The audio record object.
         */
        private AudioRecord mAudioRecord = null;

        /**
         * The to stop.
         */
        private boolean mToStop;

        /**
         * The data.
         */
        private byte[] mData;

        private int counter;


        /**
         * Instantiates a new audio record thread.
         */
        public AudioRecordThread() {
            mToStop = false;
        }


        /**
         * Returns the AudioRecord's getMinBufferSize, in byte units, based on the current
         * configuration of the sample rate, input config channel, and audio format.
         * Changing any of those parameters will not take effect until a recording has restarted if
         * it was running while a value was set.
         *
         * @return
         * @see android.media.AudioRecord
         */
        public int getMinBufferSize() {
            return AudioRecord.getMinBufferSize(
                       mAudioSampleRate,
                       mInputChannelConfig,
                       mAudioFormat);
        }


        //private AudioTrack mAudioTrack = null;

        /*
         * (non-Javadoc)
         *
         * @see java.lang.Thread#run()
         */
        public void run() {

            try {
                Log.d(TAG, "Running audio record thread");

                int minBufferSize = getMinBufferSize();

                Log.d(TAG, "minBufferSize=" + minBufferSize);

                mData = new byte[minBufferSize];

                counter = 0;    // reset

                mAudioRecord = new AudioRecord(mSourceType,
                                               mAudioSampleRate, mInputChannelConfig,
                                               AudioFormat.ENCODING_PCM_16BIT, minBufferSize);

                // For debugging only
                /*mAudioTrack = new AudioTrack(AudioManager.STREAM_VOICE_CALL,
                                             mAudioSampleRate, AudioFormat.CHANNEL_OUT_MONO,
                                             AudioFormat.ENCODING_PCM_16BIT, minBufferSize,
                                             AudioTrack.MODE_STREAM);
                */

                if (mAudioRecord.getState() == AudioRecord.STATE_UNINITIALIZED) {
                    Log.e(TAG, "Unable to initialize the AudioRecord instance");
                    onError(OnAudioStateChangeListener.AudioError.AudioRecordError);

                } else {
                    mAudioRecord.startRecording();
                    //mAudioTrack.play();
                    setState(OnAudioStateChangeListener.AudioState.Recording);

                    while (!mToStop) {
                        mAudioRecord.read(mData, 0, mData.length);

                        //mAudioTrack.write(mData, 0, mData.length);
                        onAudioCapture(counter++, mData);
                    }
                }
                Log.i(TAG, "Stopping AudioRecordThread");

                // Stop the recording
                mAudioRecord.stop();
                mAudioRecord.release();


            } catch (Exception ex) {
                Log.d(TAG, "Exception in thread. " + ex.getLocalizedMessage());
                ex.printStackTrace();
            } finally {
                mAudioRecord = null;
            }
        }

        /**
         * Stop thread.
         */
        public void stopThread() {
            Log.d(TAG, "Attempting to stop audio record thread");
            mToStop = true;
        }

    }

    /**
     * Sets the audio sample rate.
     *
     * @param audioSampleRate the new audio sample rate
     */
    public void setAudioSampleRate(int audioSampleRate) {
        mAudioSampleRate = audioSampleRate;

    }

    public int getAudioSampleRate() {
        return mAudioSampleRate;
    }

    /**
     * Sets the input channels.
     *
     * @param channels the new input channels
     */
    public void setInputChannels(int channels) {
        mInputChannels = channels;
        mInputChannelConfig = channels == 2 ? AudioFormat.CHANNEL_IN_STEREO
                              : AudioFormat.CHANNEL_IN_MONO;
    }

    public int getInputChannelConfig() {
        return mInputChannelConfig;
    }

    public int getAudioFormat() {
        return mAudioFormat;
    }
}
