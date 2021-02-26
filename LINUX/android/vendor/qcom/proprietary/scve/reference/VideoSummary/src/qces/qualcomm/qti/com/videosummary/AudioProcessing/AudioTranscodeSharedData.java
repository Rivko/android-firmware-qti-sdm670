/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.AudioProcessing;

import android.content.Context;

import java.nio.ByteBuffer;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BlockingQueue;


/**
 * Created by matthewf on 8/12/15.
 */
public class AudioTranscodeSharedData {

    private final BlockingQueue<ByteBuffer> audioByteBufferQueue;
    private final BlockingQueue<Long> audioPtsQueue;
    private final long desiredLengthUs;

    public int totalDecodeOutputFrames;

    private boolean muxerStarted;
    private int encodeFrameCount;

    private String decodeMimeType;

    private Context context;

    private long timeStampMillis;
    public float audioDurationMicroSec;
    public long lastDecodeSampleTime;
    public boolean decodeEOS_sent;

    public int audioBitrate = 192000; //default to 192kbps


    public AudioTranscodeSharedData(Context context, long timeStampMillis, long desiredLengthUs) {

        audioByteBufferQueue = new ArrayBlockingQueue<>(8);
        audioPtsQueue = new ArrayBlockingQueue<>(8);

        encodeFrameCount = 0;
        muxerStarted = false;

        this.context = context;
        this.timeStampMillis = timeStampMillis;
        this.desiredLengthUs = desiredLengthUs;

    }

    public long getTimeStampMillis() {
        return timeStampMillis;
    }

    public long getDesiredLengthUs() {
        return desiredLengthUs;
    }

    public void addToQueue(ByteBuffer inputBuf, long presentationTime) {

        try {
            audioByteBufferQueue.put(inputBuf); //put - blocks if queue is full
            audioPtsQueue.put(presentationTime);

        } catch (InterruptedException e) {
            e.printStackTrace();
        }

    }

    public ByteBuffer getAudioByteBufferFromQueue() {
        if (!audioByteBufferQueue.isEmpty()) {
            try {
                return audioByteBufferQueue.take(); //take will block if queue is empty
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        return null;
    }

    public long getPtsTimeFromQueue() {
        if (!audioPtsQueue.isEmpty()) {
            try {
                return audioPtsQueue.take(); //take will block if queue is empty
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        return 0;
    }

    public int getFrameQueueSize() {
        return audioByteBufferQueue.size();
    }

    public int getEncodeFrameCount() {
        return encodeFrameCount;
    }

    public void incrementEncodedFrameCount() {
        encodeFrameCount++;
    }

    public String getDecodeMimeType() {
        return decodeMimeType;
    }

    public void setDecodeMimeType(String decodeMimeType) {
        this.decodeMimeType = decodeMimeType;
    }


    public Context getContext() {
        return this.context;
    }


}
