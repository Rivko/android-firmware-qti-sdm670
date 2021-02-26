/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.AudioProcessing;

import android.media.MediaCodec;
import android.media.MediaFormat;
import android.media.MediaMuxer;
import android.os.Bundle;
import android.os.Message;
import android.util.Log;

import java.nio.ByteBuffer;

import qces.qualcomm.qti.com.videosummary.highlights.HighlightsActivity;
import qces.qualcomm.qti.com.videosummary.videoprocessing.VideoServices.AudioProcManagerService;
import qces.qualcomm.qti.com.videosummary.watchdog.Watchdog;

/**
 * Created by matthewf on 8/12/15.
 */
public class AudioEncoderCallback extends MediaCodec.Callback {

    private static final String TAG = AudioEncoderCallback.class.getSimpleName();
    private static final long PROCESS_TAIL_PERCENTAGE_OF_TOTAL_AUDIO_PROCESSING = 100;

    private final AudioTranscodeSharedData sharedData;
    private final MediaMuxer mMuxer;
    private final AudioCodecManager audioCodecManager;

    private boolean encodeEOS_sent = false;
    private long ptsUsec;
    private int progress;
    private boolean muxerStarted = false;
    private int muxerTrackIndex;

    public AudioEncoderCallback(AudioCodecManager audioCodecManager, AudioTranscodeSharedData transcodeData, MediaMuxer muxer) {

        this.audioCodecManager = audioCodecManager;
        this.sharedData = transcodeData;
        this.mMuxer = muxer;

    }

    //Used for watchdog petting
    static long previousPtsUsec = 0;

    @Override
    public void onInputBufferAvailable(MediaCodec codec, int index) {
        //Log.d(TAG, "Encoder input buffer " + index + " available");
        int encodeFrameCnt = sharedData.getEncodeFrameCount();

        ByteBuffer bufFromQueue = sharedData.getAudioByteBufferFromQueue();

        if (bufFromQueue != null) {

            ByteBuffer inputBuf = codec.getInputBuffer(index);
            inputBuf.put(bufFromQueue);

            int size = bufFromQueue.limit();

            ptsUsec = sharedData.getPtsTimeFromQueue();
            codec.queueInputBuffer(index, 0, size, ptsUsec, 0);

            Log.d(TAG, "        Encoder input frame: " + encodeFrameCnt + " with ptsUsec: " + ptsUsec + " size: " + size);

            sharedData.incrementEncodedFrameCount();
            
            int calcProgress = (int) (ptsUsec * PROCESS_TAIL_PERCENTAGE_OF_TOTAL_AUDIO_PROCESSING / sharedData.getDesiredLengthUs());
            if (calcProgress != progress) {
                progress = calcProgress;
                HighlightsActivity.sendProgressUpdate(sharedData.getContext(), sharedData.getTimeStampMillis(), -2, -1, progress);//Assembling
                Log.d(TAG, "        Encoder input sending progress update");
            }
            
            //petting the watchdog once in 1 sec as approx and watch dog timeout/bark is 5sec, & will bite after 5 barks;
            if (ptsUsec - previousPtsUsec >= 1000000) {
                previousPtsUsec = ptsUsec;
                Watchdog.getWatchdogInstance().watchdogPet(TAG + " Audio Assembling progress:" + progress);
            }

        } else if (ptsUsec == sharedData.lastDecodeSampleTime && !encodeEOS_sent && sharedData.decodeEOS_sent) {

            Log.d(TAG, "        Encoder input: sending EOS");
            codec.queueInputBuffer(index, 0, 0, ptsUsec,
                    MediaCodec.BUFFER_FLAG_END_OF_STREAM);
            encodeEOS_sent = true;

        } else {
            //queueing the input buffer with no data so that we can get back this input buffer later
            codec.queueInputBuffer(index, 0, 0, 0, 0);

        }
    }

    @Override
    public void onOutputBufferAvailable(MediaCodec codec, int index, MediaCodec.BufferInfo info) {

        //Log.d(TAG, "Encoder output buffer " + index + " available");

        ByteBuffer encodedData = codec.getOutputBuffer(index);

        if (info.size != 2) //use this to ignore the first bytes recvd which are 2 codec config bytes
            mMuxer.writeSampleData(muxerTrackIndex, encodedData, info);

        boolean EOS = (info.flags & MediaCodec.BUFFER_FLAG_END_OF_STREAM) != 0;

        if (EOS) {

            Log.d(TAG, "            Encoder output: got EOS, done.");
            releaseEncoderResources(codec);
            sendCodecManagerMsg(AudioCodecManager.CODEC_MSG_PROCESS_AUDIO_DONE);
            return;
        }

        codec.releaseOutputBuffer(index, false);

    }

    @Override
    public void onError(MediaCodec codec, MediaCodec.CodecException e) {
        Log.d(TAG, "MediaCodec Encoder Exception: " + e.getDiagnosticInfo());
    }

    @Override
    public void onOutputFormatChanged(MediaCodec codec, MediaFormat format) {

        Log.d(TAG, "MediaCodec Encoder format changed: " + format.getString(MediaFormat.KEY_MIME));

        if (!muxerStarted) {
            MediaFormat newFormat = codec.getOutputFormat();
            muxerTrackIndex = mMuxer.addTrack(newFormat);
            mMuxer.start();
            muxerStarted = true;
        }

    }


    public void releaseEncoderResources(MediaCodec encoder) {

        if (mMuxer != null) {
            mMuxer.stop();
            mMuxer.release();
        }
        if (encoder != null) {
            encoder.stop();
            encoder.release();
        }

        Log.d(TAG, "Audio Encoder Callback released resources.");


    }

    private void sendCodecManagerMsg(int msgCode) {
        Message m = AudioProcManagerService.audioCodecManager.mHandler.obtainMessage();
        Bundle b = new Bundle();
        b.putInt(AudioCodecManager.AUDIO_CODEC_MSG, msgCode);
        m.setData(b);
        AudioProcManagerService.audioCodecManager.mHandler.sendMessage(m);
    }
}
