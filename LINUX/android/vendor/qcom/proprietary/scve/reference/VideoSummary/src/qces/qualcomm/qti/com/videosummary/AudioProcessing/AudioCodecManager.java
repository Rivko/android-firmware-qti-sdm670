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
import android.media.MediaMetadataRetriever;
import android.media.MediaMuxer;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;

import java.io.File;
import java.io.IOException;
import java.nio.ByteBuffer;

import qces.qualcomm.qti.com.videosummary.Util.Constants;
import qces.qualcomm.qti.com.videosummary.highlights.HighlightsActivity;
import qces.qualcomm.qti.com.videosummary.highlights.HighlightsModel;
import qces.qualcomm.qti.com.videosummary.providers.QueryHelper;


public class AudioCodecManager implements Runnable {

    private static final String TAG = AudioCodecManager.class.getSimpleName();

    private final AudioTranscodeSharedData audioTranscodeSharedData;
    private final HighlightsModel item;

    public static final String AUDIO_CODEC_MSG = "audioCodecMsg";
    public static final int CODEC_MSG_PROCESS_AUDIO_DONE = 1;
    public static final int CODEC_MSG_MUX_VIDEO_AUDIO_DONE = 2;
    public final MyHandler mHandler;


    private Object codecManagerTaskObject = new Object();

    /* Pulls encoded AV from source */
    private MediaExtractor audioExtractor;

    private Context context;

    private MediaCodec decoder;
    private MediaCodec encoder;

    private String extractorMime;
    private MediaFormat extractorAudioFormat;
    private long audioClipLengthOrig;
    private long audioClipLengthTail;
    private String audioAssetFileName;
    private long videoClipLength;
    private AudioDecoderCallBack audioDecoderCallBack;
    private AudioEncoderCallback audioEncoderCallBack;

    public class MyHandler extends Handler {

        public MyHandler(Looper l) {
            super(l);
        }

        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);

            Bundle data = msg.getData();
            int command = data.getInt(AUDIO_CODEC_MSG);

            switch (command) {

                case CODEC_MSG_PROCESS_AUDIO_DONE:

                    Thread muxAudio = new Thread(new AudioMuxWithVideo(context, item.timeStampMillis,
                            Uri.fromFile(new File(item.videoFilePath)), videoClipLength, audioClipLengthOrig, audioClipLengthTail,
                            audioAssetFileName));
                    muxAudio.start();

                    break;

                case CODEC_MSG_MUX_VIDEO_AUDIO_DONE:
                    QueryHelper.updateHighlights(context, item.videoFilePath, HighlightsModel.CardType.SUMMARY);
                    threadContinue();
                    break;

            }

        }

    }

    public AudioCodecManager(Context context, HighlightsModel item) {
        //use currentUriIndex = -2 for sending progress bar updates for audio.
        HighlightsActivity.sendProgressUpdate(context, item.timeStampMillis, -2, -1, 0);
        this.context = context;
        this.item = item;

        doAudioCalculations();

        audioTranscodeSharedData = new AudioTranscodeSharedData(context, item.timeStampMillis, audioClipLengthTail);

        mHandler = new MyHandler(Looper.getMainLooper());
    }


    private void doAudioCalculations() {

        audioAssetFileName = Constants.THEME[item.getTheme()];

        MediaMetadataRetriever metadataRetriever = new MediaMetadataRetriever();
        metadataRetriever.setDataSource(item.videoFilePath);
        String durationStr = metadataRetriever.extractMetadata(
                MediaMetadataRetriever.METADATA_KEY_DURATION);
        videoClipLength = (long) (Float.parseFloat(durationStr) * 1000);

        try {
            AssetFileDescriptor assetFD = context.getAssets().openFd(audioAssetFileName);
            metadataRetriever.setDataSource(assetFD.getFileDescriptor(), assetFD.getStartOffset(), assetFD.getLength());
        } catch (IOException e) {
            e.printStackTrace();
        }

        durationStr = metadataRetriever.extractMetadata(
                MediaMetadataRetriever.METADATA_KEY_DURATION);

        audioClipLengthOrig = 0;
        audioClipLengthTail = 0;
        if (durationStr != null) {
            try {
                audioClipLengthOrig = (long) (Float.parseFloat(durationStr) * 1000);
            } catch (NumberFormatException e) {
                Log.e(TAG, "Failed parsing video duration [" + durationStr + "]", e);
            }
        }

        if (videoClipLength < audioClipLengthOrig)
            audioClipLengthTail = videoClipLength;
        else
            audioClipLengthTail = videoClipLength % audioClipLengthOrig; //get the remainder length for the tail
    }

    @Override
    public void run() {

        setupDecoder();
        setupEncoder();

        decoder.start();

        threadWait();

    }

    public void stop() {

        audioDecoderCallBack.releaseDecoderResources(decoder);
        audioEncoderCallBack.releaseEncoderResources(encoder);
        threadContinue();

    }

    private void setupDecoder() {

        /* Initialize and set the datasource of the media audioExtractor */
        audioExtractor = new MediaExtractor();

        MediaMetadataRetriever metadataRetriever = new MediaMetadataRetriever();

        try {
            AssetFileDescriptor assetFD = context.getAssets().openFd(audioAssetFileName);
            audioExtractor.setDataSource(assetFD.getFileDescriptor(), assetFD.getStartOffset(), assetFD.getLength());
            metadataRetriever.setDataSource(assetFD.getFileDescriptor(), assetFD.getStartOffset(), assetFD.getLength());
        } catch (IOException e) {
            e.printStackTrace();
        }

        String durationStr = metadataRetriever.extractMetadata(
                MediaMetadataRetriever.METADATA_KEY_DURATION);

        if (durationStr != null) {
            try {
                audioTranscodeSharedData.audioDurationMicroSec = Float.parseFloat(durationStr) * 1000;
            } catch (NumberFormatException e) {
                Log.e(TAG, "Failed parsing video duration [" + durationStr + "]", e);
            }
        }

        String bitrateStr = metadataRetriever.extractMetadata(
                MediaMetadataRetriever.METADATA_KEY_BITRATE);

        if (bitrateStr != null && !bitrateStr.equals("0")) {
            try {
                audioTranscodeSharedData.audioBitrate = Integer.parseInt(bitrateStr);
            } catch (NumberFormatException e) {
                Log.e(TAG, "Failed parsing video duration [" + durationStr + "]", e);
            }
        }

        /*
         * Iterate over the available tracks and choose the video track. Choose
         * the decoder by type and configure the decoder
         */
        for (int i = 0; i < audioExtractor.getTrackCount(); i++) {

            extractorAudioFormat = audioExtractor.getTrackFormat(i);
            extractorMime = extractorAudioFormat.getString(MediaFormat.KEY_MIME);

            if (extractorMime.startsWith("audio/")) {
                audioExtractor.selectTrack(i);

                try {
                    decoder = MediaCodec.createDecoderByType(extractorMime);
                } catch (IOException e) {
                    e.printStackTrace();
                }

                audioDecoderCallBack = new AudioDecoderCallBack(audioExtractor, audioTranscodeSharedData);
                decoder.setCallback(audioDecoderCallBack);
                decoder.configure(extractorAudioFormat, null, null, 0);

            }

        }


    }

    private void setupEncoder() {

        try {
            encoder = MediaCodec.createEncoderByType(extractorMime);
        } catch (IOException e) {
            e.printStackTrace();
        }

        File f = new File(context.getFilesDir().toString(), "tail_temp.m4a");

        MediaMuxer mMuxer;
        try {
            mMuxer = new MediaMuxer(f.getAbsolutePath(), MediaMuxer.OutputFormat.MUXER_OUTPUT_MPEG_4);
        } catch (IOException ioe) {
            throw new RuntimeException("MediaMuxer creation failed", ioe);
        }


        extractorAudioFormat.setLong(MediaFormat.KEY_DURATION, audioTranscodeSharedData.getDesiredLengthUs());

        //adts header fix
        byte[] bytes = new byte[]{(byte) 0x11, (byte) 0x90};
        ByteBuffer bb = ByteBuffer.wrap(bytes);
        extractorAudioFormat.setByteBuffer("csd-0", bb);

        extractorAudioFormat.setInteger(MediaFormat.KEY_BIT_RATE, audioTranscodeSharedData.audioBitrate);

        audioEncoderCallBack = new AudioEncoderCallback(this, audioTranscodeSharedData, mMuxer);
        encoder.setCallback(audioEncoderCallBack);

        encoder.configure(extractorAudioFormat, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);

        encoder.start();


    }

    private void threadWait() {

        synchronized (codecManagerTaskObject) {
            try {
                codecManagerTaskObject.wait();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }

    public void threadContinue() {

        synchronized (codecManagerTaskObject) {
            try {
                codecManagerTaskObject.notifyAll();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }

    }


}
