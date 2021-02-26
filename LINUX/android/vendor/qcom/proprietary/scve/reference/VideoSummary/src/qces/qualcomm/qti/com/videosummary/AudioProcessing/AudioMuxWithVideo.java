/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.AudioProcessing;

import android.content.Context;
import android.content.res.AssetFileDescriptor;
import android.media.MediaCodec;
import android.media.MediaExtractor;
import android.media.MediaFormat;
import android.media.MediaMuxer;
import android.net.Uri;
import android.os.Bundle;
import android.os.Message;
import android.util.Log;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.ByteBuffer;

import qces.qualcomm.qti.com.videosummary.Util.Utils;
import qces.qualcomm.qti.com.videosummary.highlights.HighlightsActivity;
import qces.qualcomm.qti.com.videosummary.videoprocessing.VideoServices.AudioProcManagerService;
import qces.qualcomm.qti.com.videosummary.watchdog.Watchdog;

/**
 * Created by matthewf on 10/7/15.
 */
public class AudioMuxWithVideo implements Runnable {

    private static final String TAG = AudioMuxWithVideo.class.getSimpleName();
    private static final int MAX_SAMPLE_SIZE = 1024 * 1024; //1MB
    private final long timestampMillis;
    private Uri videoUri;
    private long videoLengthUs;
    private long audioClipLengthOrig;
    private long audioClipLengthNew;

    private String audioAssetFileName;
    private Context context;

    public AudioMuxWithVideo(Context context, long timestampMillis, Uri videoUri, long videoLengthUs, long audioClipLengthOrig, long audioClipLengthNew, String audioAssetFileName) {
        this.context = context;
        this.timestampMillis = timestampMillis;
        this.videoUri = videoUri;
        this.videoLengthUs = videoLengthUs;
        this.audioClipLengthOrig = audioClipLengthOrig;
        this.audioClipLengthNew = audioClipLengthNew;
        this.audioAssetFileName = audioAssetFileName;
    }

    //Used for watchdog petting
    static long previousPtsUsec = 0;

    @Override
    public void run() {

        HighlightsActivity.sendProgressUpdate(context, timestampMillis, -2, -1, 0);//Assembling

        MediaExtractor audioExtractor;
        MediaExtractor audioExtractorTail;
        MediaExtractor videoExtractor;

        audioExtractor = new MediaExtractor();
        audioExtractorTail = new MediaExtractor();
        videoExtractor = new MediaExtractor();

        try {

            AssetFileDescriptor assetFD = context.getAssets().openFd(audioAssetFileName);

            audioExtractor.setDataSource(assetFD.getFileDescriptor(), assetFD.getStartOffset(), assetFD.getLength());
            audioExtractorTail.setDataSource(context.getFilesDir().toString() + "/tail_temp.m4a");

            videoExtractor.setDataSource(context, videoUri, null);

        } catch (IOException e) {
            e.printStackTrace();
        }

        int audioLoopCntCalc = 0;
        //if the processed audio clip has equal length as the video, then we don't loop the original at all
        //otherwise we calculate the number of loops of the original to mux in before muxing in the tail at the end
        if (audioClipLengthNew < videoLengthUs) {
            audioLoopCntCalc = (int) (videoLengthUs / audioClipLengthOrig);
        }

        MediaFormat audioFormat = audioExtractor.getTrackFormat(0);
        MediaFormat videoFormat = videoExtractor.getTrackFormat(0);

        videoExtractor.selectTrack(0);
        audioExtractor.selectTrack(0);
        audioExtractorTail.selectTrack(0);

        MediaMuxer mMuxer;

        String tempFileName = videoUri.getPath() + "_temp.mp4";

        try {
            mMuxer = new MediaMuxer(tempFileName, MediaMuxer.OutputFormat.MUXER_OUTPUT_MPEG_4);
        } catch (IOException ioe) {
            throw new RuntimeException("MediaMuxer creation failed", ioe);
        }

        int videoTrackIndex = mMuxer.addTrack(videoFormat);
        int audioTrackIndex = mMuxer.addTrack(audioFormat);

        mMuxer.start();

        ByteBuffer extractorByteBuffer = ByteBuffer.allocateDirect(MAX_SAMPLE_SIZE);

        MediaCodec.BufferInfo mBufferInfo = new MediaCodec.BufferInfo();

        int videoSampleSize = videoExtractor.readSampleData(extractorByteBuffer, 0);
        //////////////////////////////////////////////////
        //first we write the video track to the muxer file
        //////////////////////////////////////////////////
        while (videoSampleSize > 0) {
            mBufferInfo.size = videoSampleSize;
            mBufferInfo.presentationTimeUs = videoExtractor.getSampleTime();
            int videoFlag = videoExtractor.getSampleFlags();

            if (videoFlag == MediaExtractor.SAMPLE_FLAG_SYNC)
                mBufferInfo.flags = MediaCodec.BUFFER_FLAG_KEY_FRAME;
            else
                mBufferInfo.flags = 0;

            videoExtractor.advance();
            mMuxer.writeSampleData(videoTrackIndex, extractorByteBuffer, mBufferInfo);
            videoSampleSize = videoExtractor.readSampleData(extractorByteBuffer, 0);
            Log.d(TAG, "videoSampleSize = " + videoSampleSize + " Time = " + mBufferInfo.presentationTimeUs);
        }

        long lastVideoPts = mBufferInfo.presentationTimeUs;

        ////////////////////////////////////////////
        //then we write the original audio for loops
        //and the modified tail audio for the end
        ////////////////////////////////////////////

        boolean onTail = false;
        //enable looping if the audio track runs out before the video sample time ends
        boolean addAudio = true;
        long lastAudioPts = 0;
        long firstNonZeroPts = 0;
        int audioLoopCnt = 0;


        if (audioLoopCntCalc == 0)
            onTail = true;

        while (addAudio) {

            if (onTail) {
                mBufferInfo.size = audioExtractorTail.readSampleData(extractorByteBuffer, 0);
                if (mBufferInfo.size > 0)
                    mBufferInfo.presentationTimeUs = audioExtractorTail.getSampleTime() + lastAudioPts;
                audioExtractorTail.advance();
            } else {
                mBufferInfo.size = audioExtractor.readSampleData(extractorByteBuffer, 0);
                if (mBufferInfo.size > 0)
                    mBufferInfo.presentationTimeUs = audioExtractor.getSampleTime() + lastAudioPts;
                audioExtractor.advance();
            }

            if (mBufferInfo.presentationTimeUs > lastVideoPts)
                addAudio = false;
            else if (mBufferInfo.size > 0) {
                Log.d(TAG, "muxer writing " + mBufferInfo.size + " bytes for pts time = " + mBufferInfo.presentationTimeUs);
                if (mBufferInfo.presentationTimeUs > 0 && firstNonZeroPts == 0)
                    firstNonZeroPts = mBufferInfo.presentationTimeUs + 1;
                mMuxer.writeSampleData(audioTrackIndex, extractorByteBuffer, mBufferInfo);

                //petting the watchdog once in 1 sec as approx and watch dog timeout/bark is 5sec, & will bite after 5 barks;
                if (mBufferInfo.presentationTimeUs - previousPtsUsec >= 55000000) {
                    previousPtsUsec = mBufferInfo.presentationTimeUs;
                    Watchdog.getWatchdogInstance().watchdogPet(TAG + " Audio muxer writing for pts:" + mBufferInfo.presentationTimeUs);
            }

            } else if (mBufferInfo.size < 0) {
                Log.d(TAG, "-- audio file ran out of samples at loop count = " + audioLoopCnt);
                lastAudioPts = mBufferInfo.presentationTimeUs + firstNonZeroPts;

                if (audioLoopCnt == audioLoopCntCalc)
                    addAudio = false;
                else if (audioLoopCnt == audioLoopCntCalc - 1 && audioLoopCntCalc > 0) {
                    Log.d(TAG, "-- switching to audio extractor for processed tail audio");
                    onTail = true;
                } else {
                    Log.d(TAG, "-- seeking to beginning of this audio file for the loop");
                    audioExtractor.seekTo(0, MediaExtractor.SEEK_TO_CLOSEST_SYNC);
                }

                audioLoopCnt++;
            }
        }


        if (mMuxer != null) {
            mMuxer.stop();
            mMuxer.release();
        }

        if (videoExtractor != null)
            videoExtractor.release();
        if (audioExtractor != null)
            audioExtractor.release();

        Utils.deleteFile(videoUri.getPath());

        //and copy the temp file to the original
        InputStream in;
        OutputStream out;
        try {
            in = new FileInputStream(tempFileName);
            out = new FileOutputStream(videoUri.getPath());
            Utils.copyFile(in, out);
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }

        //then delete temp file
        Utils.deleteFile(tempFileName);
        //and delete temporary tail file
        Utils.deleteFile(context.getFilesDir().toString() + "/tail_temp.m4a");

        sendCodecManagerMsg(AudioCodecManager.CODEC_MSG_MUX_VIDEO_AUDIO_DONE);
        
    }

    private void sendCodecManagerMsg(int msgCode) {
        Message m = AudioProcManagerService.audioCodecManager.mHandler.obtainMessage();
        Bundle b = new Bundle();
        b.putInt(AudioCodecManager.AUDIO_CODEC_MSG, msgCode);
        m.setData(b);
        AudioProcManagerService.audioCodecManager.mHandler.sendMessage(m);
    }


}
