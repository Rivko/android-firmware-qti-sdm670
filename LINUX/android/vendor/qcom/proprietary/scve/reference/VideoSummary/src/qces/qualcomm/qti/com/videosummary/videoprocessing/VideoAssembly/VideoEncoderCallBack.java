/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.videoprocessing.VideoAssembly;

import android.media.MediaCodec;
import android.media.MediaFormat;
import android.media.MediaMuxer;
import android.os.Bundle;
import android.os.Message;
import android.util.Log;

import java.nio.ByteBuffer;

import qces.qualcomm.qti.com.videosummary.highlights.HighlightsActivity;
import qces.qualcomm.qti.com.videosummary.videoprocessing.VideoFrameUtils.FrameUtils;
import qces.qualcomm.qti.com.videosummary.videoprocessing.VideoServices.ScoreOrAssemblyService;
import qces.qualcomm.qti.com.videosummary.watchdog.Watchdog;

/**
 * Created by matthewf on 8/12/15.
 */
public class VideoEncoderCallBack extends MediaCodec.Callback {

    private static final String TAG = VideoEncoderCallBack.class.getSimpleName();

    private MediaMuxer mMuxer;
    private int mTrackIndex = -1;

    private final VideoTranscodeSharedData sharedData;

    private boolean encodeEOS_sent = false;

    public VideoEncoderCallBack(MediaMuxer mMuxer, VideoTranscodeSharedData transcodeData) {

        this.mMuxer = mMuxer;
        this.sharedData = transcodeData;

    }

    @Override
    public void onInputBufferAvailable(MediaCodec codec, int index) {
        //Log.d(TAG, "Encoder input buffer " + index + " available");
        int encodeFrameCnt = sharedData.getEncodeFrameCount();

        ByteBuffer bufFromQueue = sharedData.getFrameFromQueue();

        if (bufFromQueue != null) {

            ByteBuffer inputBuf = codec.getInputBuffer(index);
            inputBuf.put(bufFromQueue);

            long ptsUsec = FrameUtils.computePresentationTime(encodeFrameCnt, sharedData.getFrameRate());

            Log.d(TAG, "        Encoder input frame: " + encodeFrameCnt + " with ptsUsec: " + ptsUsec + " queueSize: " + sharedData.getFrameQueueSize());

            int size = bufFromQueue.limit();
            codec.queueInputBuffer(index, 0, size, ptsUsec, 0);

            sharedData.incrementEncodedFrameCount();


        } else if (sharedData.checkTranscodingAllDone() && !encodeEOS_sent) {
            encodeEOS_sent = true;
            Log.d(TAG, "        Encoder input: sending EOS");
            codec.queueInputBuffer(index, 0, 0, FrameUtils.computePresentationTime(sharedData.getEncodeFrameCount(), sharedData.getFrameRate()),
                    MediaCodec.BUFFER_FLAG_END_OF_STREAM);

        } else {
            //queueing the input buffer with no data so that we can get back this input buffer later
            codec.queueInputBuffer(index, 0, 0, 0, 0);

        }
    }

    private void sendCodecManagerMsg(int msgCode) {
        Message m = ScoreOrAssemblyService.videoCodecManager.mHandler.obtainMessage();
        Bundle b = new Bundle();
        b.putInt(VideoCodecManager.CODEC_MSG, msgCode);
        m.setData(b);
        ScoreOrAssemblyService.videoCodecManager.mHandler.sendMessage(m);
    }


    @Override
    public void onOutputBufferAvailable(MediaCodec codec, int index, MediaCodec.BufferInfo info) {

        //Log.d(TAG, "Encoder output buffer " + index + " available");

        ByteBuffer encodedData = codec.getOutputBuffer(index);

        boolean EOS = (info.flags & MediaCodec.BUFFER_FLAG_END_OF_STREAM) != 0;

        if (EOS) {
            Log.d(TAG, "            Encoder output: got EOS, done.");
            releaseEncoderResources(codec);
            sendCodecManagerMsg(VideoCodecManager.CODEC_MSG_ENCODE_VIDEO_DONE);
            return;
        }

        encodedData.position(info.offset);
        encodedData.limit(info.offset + info.size);

        if (info.presentationTimeUs > 0) //for glitch seen between 2 decodes where presentation time is zero
        {
            mMuxer.writeSampleData(mTrackIndex, encodedData, info);
        }

        codec.releaseOutputBuffer(index, false);

    }

    @Override
    public void onError(MediaCodec codec, MediaCodec.CodecException e) {
        Log.d(TAG, "MediaCodec Encoder Exception: " + e.getDiagnosticInfo());
    }

    @Override
    public void onOutputFormatChanged(MediaCodec codec, MediaFormat format) {

        if (sharedData.isMuxerStarted() == false) {

            MediaFormat newFormat = codec.getOutputFormat();
            // once output format is obtained, use it to start the muxer
            mTrackIndex = mMuxer.addTrack(newFormat);

            sharedData.setMuxerStarted(true);

            mMuxer.start();
        }


    }


    public void releaseEncoderResources(MediaCodec encoder) {

        if (mMuxer != null) {
            mMuxer.stop();
            mMuxer.release();
            mMuxer = null;
        }
        if (encoder != null) {
            encoder.stop();
            encoder.release();
        }

        Log.d(TAG, "Task released resources.");
    }
}
