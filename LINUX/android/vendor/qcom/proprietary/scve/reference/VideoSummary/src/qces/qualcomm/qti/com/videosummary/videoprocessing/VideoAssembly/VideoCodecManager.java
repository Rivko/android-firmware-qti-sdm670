/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.videoprocessing.VideoAssembly;

import android.content.Context;
import android.media.MediaCodec;
import android.media.MediaCodecInfo;
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

import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;

import qces.qualcomm.qti.com.videosummary.MediaInfoGenerator.MediaInfo;
import qces.qualcomm.qti.com.videosummary.highlights.HighlightsModel;


public class VideoCodecManager implements Runnable {

    private static final String TAG = VideoCodecManager.class.getSimpleName();

    public static final int FINAL_ENCODED_BIT_RATE = 10000000;
    public static final int FINAL_ENCODED_FRAME_RATE = 30;
    public static final int FINAL_ENCODED_I_FRAME_INTERVAL = 1;
    private final HighlightsModel highlightsItem;

    private int frame_format_encode;
    private int frame_format_decode;

    private static final String ENCODER_MIME_TYPE = MediaFormat.MIMETYPE_VIDEO_AVC;

    private Object codecManagerTaskObject = new Object();

    /* Pulls encoded AV from source */
    private MediaExtractor extractor;

    private ArrayList<Uri> videoUris;
    private Context context;

    private final VideoTranscodeSharedData videoTranscodeData;

    public MyHandler mHandler;

    public static final String CODEC_MSG = "codecMsg";
    public static final int CODEC_MSG_DECODE_VIDEO_DONE = 1;
    public static final int CODEC_MSG_ENCODE_VIDEO_DONE = 2;

    private MediaCodec decoder;
    private MediaCodec encoder;
    private VideoEncoderCallBack vidEncCallBack;
    private VideoDecoderCallBack vidDecCallBack;

    public class MyHandler extends Handler {

        public MyHandler(Looper l) {
            super(l);
        }

        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);

            Bundle data = msg.getData();
            int command = data.getInt(CODEC_MSG);

            switch (command) {

                case CODEC_MSG_DECODE_VIDEO_DONE:
                    if (!videoTranscodeData.checkTranscodingAllDone() && !videoTranscodeData.isDecoderOnLastVideo()) {
                        Log.d(TAG, "Decode done for video: " + videoTranscodeData.getCurrentDecodeVideoIndex() + ". Starting next video decode");
                        videoTranscodeData.incrementDecodeVideoIndex();
                        setupDecoder();
                    }
                    break;

                case CODEC_MSG_ENCODE_VIDEO_DONE:
                    threadContinue();
                    break;

            }

        }

    }

    public VideoCodecManager(Context context, HighlightsModel item, boolean[] keepFrames, int numKeepFrames) {

        this.videoUris = MediaInfo.getArrayListOfUris(item.compositonMediaList);
        this.context = context;
        this.highlightsItem = item;

        frame_format_encode = MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420SemiPlanar;
        frame_format_decode = MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420SemiPlanar;

        videoTranscodeData = new VideoTranscodeSharedData(context, item, keepFrames, numKeepFrames);
        mHandler = new MyHandler(Looper.getMainLooper());
    }

    private void calculateDimensionForEncode(ArrayList<Uri> uris) {

        HashMap<Integer, Integer> videoSizesKeyWidth = new HashMap<>();
        HashMap<Integer, Integer> videoSizesKeyHeight = new HashMap<>();
        int[] widths = new int[uris.size()];
        int[] heights = new int[uris.size()];
        int i = 0;
        int numPortrait = 0;
        int numLandscape = 0;

        for (Uri uri : uris) {

            MediaMetadataRetriever metadataRetriever = new MediaMetadataRetriever();
            metadataRetriever.setDataSource(context, uri);
            String rotationStr = metadataRetriever.extractMetadata(
                    MediaMetadataRetriever.METADATA_KEY_VIDEO_ROTATION);

            int rotation = 0;
            if (rotationStr != null && !rotationStr.isEmpty()) {
                try {
                    rotation = Integer.parseInt(rotationStr);
                } catch (NumberFormatException e) {
                    Log.e(TAG, "Failed parsing video rotation [" + rotationStr + "]", e);
                }
            }

            int width = 0;
            int height = 0;

            if (rotation == 0 || rotation == 180) {
                width = Integer.parseInt(metadataRetriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_VIDEO_WIDTH));
                height = Integer.parseInt(metadataRetriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_VIDEO_HEIGHT));
            } else {
                width = Integer.parseInt(metadataRetriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_VIDEO_HEIGHT));
                height = Integer.parseInt(metadataRetriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_VIDEO_WIDTH));
            }

            if (width > height)
                numLandscape++;
            else
                numPortrait++;

            widths[i] = width;
            heights[i] = height;
            i++;

            videoSizesKeyWidth.put(width, height);
            videoSizesKeyHeight.put(height, width);
        }

        //sort the widths and heights smallest to largest
        Arrays.sort(widths);
        Arrays.sort(heights);

        boolean finalVideoIsLandscape;

        if (numPortrait > numLandscape) {
            finalVideoIsLandscape = false;
            videoTranscodeData.finalEncodeWidth = widths[0];
            videoTranscodeData.finalEncodeHeight = videoSizesKeyWidth.get(widths[0]);
        } else {
            finalVideoIsLandscape = true;
            videoTranscodeData.finalEncodeWidth = videoSizesKeyHeight.get(heights[0]);
            videoTranscodeData.finalEncodeHeight = heights[0];
        }

        //adjusting for problem where we can't encode to >1080p on 8994 if we're decoding at 4K, so
        //encode at 1080p instead.
        if (finalVideoIsLandscape && videoTranscodeData.finalEncodeWidth > 1920) {
            videoTranscodeData.finalEncodeWidth = 1920;
            videoTranscodeData.finalEncodeHeight = 1080;
        }
        else if (!finalVideoIsLandscape && videoTranscodeData.finalEncodeHeight > 1920) {
            videoTranscodeData.finalEncodeWidth = 1080;
            videoTranscodeData.finalEncodeHeight = 1920;
        }

    }


    @Override
    public void run() {

        videoTranscodeData.setVideoUris(videoUris);

        calculateDimensionForEncode(videoUris);
        setupEncoder();
        setupDecoder();

        threadWait();

    }

    public void stop() {

        vidDecCallBack.releaseDecoderResources(decoder);
        vidEncCallBack.releaseEncoderResources(encoder);
        threadContinue();

    }

    private void setupDecoder() {

        /* Initialize and set the datasource of the media extractor */
        extractor = new MediaExtractor();

        Uri videoUri = videoTranscodeData.getCurrentDecodeVideoUri();

        try {
            extractor.setDataSource(videoTranscodeData.getContext(), videoUri, null);
        } catch (IOException e) {
            e.printStackTrace();
        }

        /*
         * Iterate over the available tracks and choose the video track. Choose
         * the decoder by type and configure the decoder
         */
        for (int i = 0; i < extractor.getTrackCount(); i++) {

            MediaFormat decFormat = extractor.getTrackFormat(i);

            String mime = decFormat.getString(MediaFormat.KEY_MIME);

            if (mime.startsWith("video/")) {

                videoTranscodeData.extractorWidth = decFormat.getInteger(MediaFormat.KEY_WIDTH);
                videoTranscodeData.extractorHeight = decFormat.getInteger(MediaFormat.KEY_HEIGHT);

                extractor.selectTrack(i);

                decFormat.setInteger(MediaFormat.KEY_COLOR_FORMAT, frame_format_decode);

                MediaMetadataRetriever metadataRetriever = new MediaMetadataRetriever();
                metadataRetriever.setDataSource(videoTranscodeData.getContext(), videoUri);
                String rotationStr = metadataRetriever.extractMetadata(
                        MediaMetadataRetriever.METADATA_KEY_VIDEO_ROTATION);
                videoTranscodeData.rotationDegrees = 0;
                if (rotationStr != null && !rotationStr.isEmpty()) {
                    try {
                        videoTranscodeData.rotationDegrees = Integer.parseInt(rotationStr);
                    } catch (NumberFormatException e) {
                        Log.e(TAG, "Failed parsing video rotation [" + rotationStr + "]", e);
                        videoTranscodeData.rotationDegrees = 0;
                    }
                }

                if (videoTranscodeData.rotationDegrees != 0 && videoTranscodeData.rotationDegrees != 90 && videoTranscodeData.rotationDegrees != 180 &&
                        videoTranscodeData.rotationDegrees != 270) {
                    Log.e(TAG, "Invalid video rotation [" + videoTranscodeData.rotationDegrees + "], using 0");
                    videoTranscodeData.rotationDegrees = 0;
                }

                videoTranscodeData.createRotateDstFrame();

                try {
                    decoder = MediaCodec.createDecoderByType(mime);
                } catch (IOException e) {
                    e.printStackTrace();
                }

                vidDecCallBack = new VideoDecoderCallBack(extractor, videoTranscodeData);
                decoder.setCallback(vidDecCallBack);
                decoder.configure(decFormat, null, null, 0);
                decoder.start();

                break;
            }

        }


    }

    private void setupEncoder() {

        MediaMuxer mMuxer;
        try {
            mMuxer = new MediaMuxer(videoTranscodeData.getMp4FilePath(), MediaMuxer.OutputFormat.MUXER_OUTPUT_MPEG_4);
        } catch (IOException ioe) {
            throw new RuntimeException("MediaMuxer creation failed", ioe);
        }

        MediaFormat encFormat = MediaFormat.createVideoFormat(ENCODER_MIME_TYPE, videoTranscodeData.finalEncodeWidth, videoTranscodeData.finalEncodeHeight);

        encFormat.setInteger(MediaFormat.KEY_BIT_RATE, FINAL_ENCODED_BIT_RATE);
        encFormat.setInteger(MediaFormat.KEY_FRAME_RATE, FINAL_ENCODED_FRAME_RATE);
        encFormat.setInteger(MediaFormat.KEY_COLOR_FORMAT, frame_format_encode);
        encFormat.setInteger(MediaFormat.KEY_I_FRAME_INTERVAL, FINAL_ENCODED_I_FRAME_INTERVAL);

        try {
            encoder = MediaCodec.createEncoderByType(ENCODER_MIME_TYPE);
        } catch (IOException e) {
            e.printStackTrace();
        }

        vidEncCallBack = new VideoEncoderCallBack(mMuxer, videoTranscodeData);
        encoder.setCallback(vidEncCallBack);
        encoder.configure(encFormat, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);

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

    private void threadContinue() {

        synchronized (codecManagerTaskObject) {
            try {
                codecManagerTaskObject.notifyAll();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }

    }


}
