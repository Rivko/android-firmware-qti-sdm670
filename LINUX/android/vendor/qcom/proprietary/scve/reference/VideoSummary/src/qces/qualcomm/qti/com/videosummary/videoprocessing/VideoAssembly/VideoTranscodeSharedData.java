/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.videoprocessing.VideoAssembly;

import android.content.Context;
import android.media.MediaMetadataRetriever;
import android.net.Uri;

import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BlockingQueue;

import qces.qualcomm.qti.com.videosummary.Util.Constants;
import qces.qualcomm.qti.com.videosummary.highlights.HighlightsModel;
import qces.qualcomm.qti.com.videosummary.videoprocessing.VideoFrameUtils.FrameUtils;
import qces.qualcomm.qti.com.videosummary.videoprocessing.VideoFrameUtils.FrameYUV420SP;

/**
 * Created by matthewf on 8/12/15.
 */
public class VideoTranscodeSharedData {

    private final BlockingQueue<ByteBuffer> frameQueue;

    public int totalDecodeOutputFrames;
    private int decodeKeepFrames;

    private int frameRate;
    private boolean muxerStarted;
    private int encodeFrameCount;

    private String decodeMimeType;

    private ArrayList<Uri> videoUris;
    private int currentDecodeVideoIndex;
    private Context context;

    private long timeStampMillis;
    private String thumbImgPath;
    private String mp4FilePath;
    private Constants.THEME_TYPE theme;
    public boolean thumbNailCreated;
    public int totalFrameCntToEncode;

    public int rotationDegrees;
    public FrameYUV420SP rotatedFrame;

    public int extractorWidth;
    public int extractorHeight;

    public int finalEncodeWidth;
    public int finalEncodeHeight;

    public int decodeStride;
    public int decodeUV_Offset;

    public boolean[] keepFrames;

    private long videoLengthUs = 0;

    public VideoTranscodeSharedData(Context context, HighlightsModel item, boolean[] keepFrames, int numKeepFrames) {

        decodeKeepFrames = 0;

        frameQueue = new ArrayBlockingQueue<>(12);

        frameRate = 30;
        encodeFrameCount = 0;
        muxerStarted = false;

        this.context = context;
        currentDecodeVideoIndex = 0;

        this.timeStampMillis = item.timeStampMillis;
        mp4FilePath = item.videoFilePath;
        thumbImgPath = item.thumbImgFilePath;

        this.theme = Constants.THEME_TYPE.values()[item.getTheme()];

        this.keepFrames = keepFrames;
        totalFrameCntToEncode = numKeepFrames;
    }

    public boolean keepCurrentDecodeOutputFrame() {
        return keepFrames[totalDecodeOutputFrames];
    }

    public void createRotateDstFrame() {

        boolean isRightAngle = (rotationDegrees == 90 || rotationDegrees == 270);
        int outputWidth = (isRightAngle ? extractorHeight : extractorWidth);
        int outputHeight = (isRightAngle ? extractorWidth : extractorHeight);
        int outStride = outputWidth;
        rotatedFrame = new FrameYUV420SP(outputWidth, outputHeight, outStride);

    }

    public long getVideoLengthUs() {

        if (videoLengthUs == 0) {
            MediaMetadataRetriever metadataRetriever = new MediaMetadataRetriever();
            metadataRetriever.setDataSource(mp4FilePath);
            String durationStr = metadataRetriever.extractMetadata(
                    MediaMetadataRetriever.METADATA_KEY_DURATION);
            return (long) Float.parseFloat(durationStr) * 1000;
        } else
            return videoLengthUs;

    }


    public String getThumbImgPath() {
        return thumbImgPath;
    }

    public String getMp4FilePath() {
        return mp4FilePath;
    }

    public String getAudioAssetFileName() {
        return Constants.THEME[theme.ordinal()];
    }

    public long getTimeStampMillis() {
        return timeStampMillis;
    }

    public boolean isMuxerStarted() {
        return muxerStarted;
    }

    public void setMuxerStarted(boolean muxerStarted) {
        this.muxerStarted = muxerStarted;
    }

    public void addToQueue(ByteBuffer inputBuf) {

        ByteBuffer buffer = FrameUtils.cloneByteBuffer(inputBuf);

        try {
            frameQueue.put(buffer); //put - blocks if queue is full
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        decodeKeepFrames++;
    }

    public ByteBuffer getFrameFromQueue() {
        if (!frameQueue.isEmpty()) { //if empty return null

            try {
                ByteBuffer frame = frameQueue.take(); //take will block if queue is empty
                return frame;

            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

        return null;
    }

    public int getFrameQueueSize() {
        return frameQueue.size();
    }

    public int getFrameRate() {
        return frameRate;
    }

    public int getEncodeFrameCount() {
        return encodeFrameCount;
    }

    public void incrementEncodedFrameCount() {
        encodeFrameCount++;
    }

    public boolean checkTranscodingAllDone() {
        if (decodeKeepFrames == encodeFrameCount && decodeKeepFrames == totalFrameCntToEncode)
            return true;
        else
            return false;
    }

    public String getDecodeMimeType() {
        return decodeMimeType;
    }

    public void setDecodeMimeType(String decodeMimeType) {
        this.decodeMimeType = decodeMimeType;
    }

    public Uri getCurrentDecodeVideoUri() {
        return videoUris.get(currentDecodeVideoIndex);
    }

    public void incrementDecodeVideoIndex() {
        currentDecodeVideoIndex++;
    }

    public int getCurrentDecodeVideoIndex() {
        return currentDecodeVideoIndex;
    }

    public boolean isDecoderOnLastVideo() {
        if (currentDecodeVideoIndex == videoUris.size() - 1)
            return true;
        else
            return false;
    }

    public void setVideoUris(ArrayList<Uri> videoUris) {
        this.videoUris = videoUris;

    }

    public Context getContext() {
        return this.context;
    }


}
