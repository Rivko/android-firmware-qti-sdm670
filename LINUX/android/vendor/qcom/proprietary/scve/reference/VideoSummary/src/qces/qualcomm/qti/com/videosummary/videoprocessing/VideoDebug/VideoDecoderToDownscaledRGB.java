/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.videoprocessing.VideoDebug;

import android.content.Context;
import android.media.MediaCodec;
import android.media.MediaCodec.BufferInfo;
import android.media.MediaCodecInfo;
import android.media.MediaExtractor;
import android.media.MediaFormat;
import android.media.MediaMetadataRetriever;
import android.net.Uri;
import android.util.Log;

import java.io.IOException;
import java.nio.ByteBuffer;

import qces.qualcomm.qti.com.videosummary.Util.Utils;
import qces.qualcomm.qti.com.videosummary.videoprocessing.VideoFrameUtils.FrameUtils;
import qces.qualcomm.qti.com.videosummary.videoprocessing.VideoFrameUtils.FrameYUV420SP;
import qces.qualcomm.qti.com.videosummary.videoprocessing.VideoFrameUtils.RotateYUV420SP;
import qces.qualcomm.qti.com.videosummary.videoprocessing.cvmanager.CVManagerJNILib;
import qces.qualcomm.qti.com.videosummary.videoprocessing.fastcv.FastCVJNILib;

/*
 * This class decodes video frame by frame to ByteBuffers
 * It uses features introduced in API Level 21, which are
 * codec.getOutputBuffer() and MediaCodec Callback capability
 */
public class VideoDecoderToDownscaledRGB implements Runnable {

    private static final String TAG = VideoDecoderToDownscaledRGB.class.getSimpleName();

    /* Pulls encoded AV from source */
    private MediaExtractor extractor;
    /* Used in this case to decode video frame returned by the MediaExtractor */
    private MediaCodec codec;
    /* Path to the encoded AV source */
    private Uri videoUri;
    private Context context;

    private int decodeCount;
    private int inputWidth;
    private int inputHeight;
    private FrameYUV420SP rotatedFrame;
    private int rotationDegrees;

    private int outWidth;
    private int outHeight;

    public VideoDecoderToDownscaledRGB(Context context, Uri videoUri) {
        this.videoUri = videoUri;
        this.context = context;

        Utils.sendMessage(context, TAG, "Calling CVNativeInit");

        int result = CVManagerJNILib.CVNativeInit();

        Utils.sendMessage(context, TAG, "CVNativeInit finished");
        Utils.sendMessage(context, TAG, "Starting video decode of: " + videoUri.toString());

    }

    @Override
    public void run() {
        /* Initialize and set the datasource of the media extractor */
        extractor = new MediaExtractor();

        try {
            extractor.setDataSource(context, videoUri, null);
        } catch (IOException e) {
            e.printStackTrace();
        }

        /*
         * Iterate over the available tracks and choose the video track. Choose
         * the codec by type and configure the codec
         */
        for (int i = 0; i < extractor.getTrackCount(); i++) {
            MediaFormat format = extractor.getTrackFormat(i);

            String mime = format.getString(MediaFormat.KEY_MIME);
            if (mime.startsWith("video/")) {

                inputWidth = format.getInteger(MediaFormat.KEY_WIDTH);
                inputHeight = format.getInteger(MediaFormat.KEY_HEIGHT);

                extractor.selectTrack(i);
                try {
                    codec = MediaCodec.createDecoderByType(mime);
                } catch (IOException e) {
                    e.printStackTrace();
                }
                /*
                 * format - format of the track. surface - surface to render the
                 * decoded frame. null - AV source is not encrypted. 0 - flags
                 * are required for encoder only.
                 */
                //force format (Android L-MR1+ only)
                format.setInteger(MediaFormat.KEY_COLOR_FORMAT, MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420SemiPlanar);
                //format.setInteger(MediaFormat.KEY_COLOR_FORMAT, MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420Planar);

                codec.configure(format, null, null, 0);

                Log.d(TAG, "Found a video track.");
                break;
            }
        }

        if (codec == null) {
            Log.e(TAG, "No video track found");
            return;
        }

        MediaMetadataRetriever metadataRetriever = new MediaMetadataRetriever();
        metadataRetriever.setDataSource(context, videoUri);
        String rotationStr = metadataRetriever.extractMetadata(
                MediaMetadataRetriever.METADATA_KEY_VIDEO_ROTATION);
        rotationDegrees = 0;
        if (rotationStr != null && !rotationStr.isEmpty()) {
            try {
                rotationDegrees = Integer.parseInt(rotationStr);
            } catch (NumberFormatException e) {
                Log.e(TAG, "Failed parsing video rotation [" + rotationStr + "]", e);
                rotationDegrees = 0;
            }
        }

        if (rotationDegrees != 0 && rotationDegrees != 90 && rotationDegrees != 180 &&
                rotationDegrees != 270) {
            Log.e(TAG, "Invalid video rotation [" + rotationDegrees + "], using 0");
            rotationDegrees = 0;
        }

        final boolean isRightAngle = (rotationDegrees == 90 || rotationDegrees == 270);
        final int outputWidth = (isRightAngle ? inputHeight : inputWidth);
        final int outputHeight = (isRightAngle ? inputWidth : inputHeight);
        rotatedFrame = new FrameYUV420SP(outputWidth, outputHeight);

        codec.setCallback(new MediaCodec.Callback() {
            @Override
            public void onInputBufferAvailable(MediaCodec mediaCodec, int inIndex) {

                if (inIndex >= 0) {
                    // if API level >= 21, get input buffer here
                    ByteBuffer buffer = mediaCodec.getInputBuffer(inIndex);
                    /*
                     * Use the media extractor to extract a sample into the
                     * codec's input buffer.
                     */
                    int sampleSize = extractor.readSampleData(buffer, 0);
                    if (sampleSize < 0) {
                        /*
                         * We shouldn't stop the playback at this point, just
                         * pass the EOS flag to codec, we will get it again
                         * from the dequeueOutputBuffer
                         */
                        mediaCodec.queueInputBuffer(inIndex, 0, 0, 0,
                                MediaCodec.BUFFER_FLAG_END_OF_STREAM);

                    } else {
                        /* enqueue the input buffer with encoded data */
                        mediaCodec.queueInputBuffer(inIndex, 0, sampleSize,
                                extractor.getSampleTime(), 0);
                        /* Advance extractor to the next sample */
                        extractor.advance();
                    }
                }

            }

            @Override
            public void onOutputBufferAvailable(MediaCodec mediaCodec, int outIndex, BufferInfo info) {
                // doStuff();
                if (outIndex >= 0) {
                    ByteBuffer outputFrame = codec.getOutputBuffer(outIndex);
                    MediaFormat format = codec.getOutputFormat();

                    if (info.size == 0) {
                        Log.d(TAG, "got empty frame");
                    } else {
                        final int stride = (format.containsKey("stride") ?
                                format.getInteger("stride") : inputWidth);
                        final int sliceHeight = (format.containsKey("slice-height") ?
                                format.getInteger("slice-height") : inputHeight);
                        final int uvOffset = sliceHeight * stride;

                        final FrameYUV420SP frame = new FrameYUV420SP(outputFrame,
                                inputWidth, inputHeight, info.offset, stride, uvOffset, stride);
                        RotateYUV420SP.rotate(frame, rotatedFrame, rotationDegrees);
                        final int[] score = CVManagerJNILib.CVNativeProcess(rotatedFrame.buf,
                                rotatedFrame.width, rotatedFrame.height,
                                rotatedFrame.offsetY, rotatedFrame.strideY,
                                rotatedFrame.offsetUV, rotatedFrame.strideUV);

                        Utils.sendMessage(context, TAG, " -- decoded frame: " + decodeCount + " score: " + score);


                        if (decodeCount < 20)
                            createThumbnail(rotatedFrame, score);

                        decodeCount++;

                    }


                    //render to surface set to false so that we get bytebuffer directly
                    codec.releaseOutputBuffer(outIndex, false);

                }

                    /* All decoded frames have been rendered, we can stop playing now */
                if ((info.flags & MediaCodec.BUFFER_FLAG_END_OF_STREAM) != 0) {
                    Log.d(TAG, "OutputBuffer BUFFER_FLAG_END_OF_STREAM");
                    Utils.sendMessage(context, TAG, "EOS reached -- releasing MediaCodec, MediaFormat objects and calling CVNativeDeInit()");
                    releaseResources();
                    CVManagerJNILib.CVNativeDeInit();
                    return;
                }


            }

            @Override
            public void onError(MediaCodec mediaCodec, MediaCodec.CodecException e) {
                Log.d(TAG, "MediaCodec Exception: " + e.getDiagnosticInfo());
            }

            @Override
            public void onOutputFormatChanged(MediaCodec mediaCodec, MediaFormat mediaFormat) {
                Log.d(TAG, "MediaCodec MediaFormat Changed");
            }
        });

        /*
         * Start the codec. Get the array of input buffers to fill with
         * encoded AV for the codec. Get the array of output buffers that will
         * be filled with decoded AV by the codec.
         */
        codec.start();


        Log.d(TAG, "Succesfully started video codec");


    }

    private void createThumbnail(FrameYUV420SP rotFrame, int[] scores) {

        float aspectRatio = (float) rotFrame.width / (float) rotFrame.height;

        //make a thumbnail approx. VGA size
        int outWidth = 640;
        int outHeight = (int) ((float) outWidth / aspectRatio);

        ByteBuffer bBufferClone = FrameUtils.cloneByteBuffer(rotFrame.buf);

        byte[] frameByteArray = new byte[rotFrame.buf.remaining()];
        bBufferClone.get(frameByteArray);

        //this will give us a pixel int[] array of downscaled frame data in RGB8888 format
        int[] rgbFrame = new int[outWidth * outHeight];

        FastCVJNILib.YUV420toRGB8888downscaleByteArrays(frameByteArray, rgbFrame, rotFrame.width,
                rotFrame.height, rotFrame.strideY, rotFrame.strideUV, outWidth, outHeight);

        Utils.sendDebugFrameInfo(context, rgbFrame, outWidth, outHeight, scores, decodeCount);

    }

    private void releaseResources() {
        if (codec instanceof MediaCodec) {
            codec.stop();
            codec.release();
            codec = null;
        }
        if (extractor instanceof MediaExtractor) {
            extractor.release();
            extractor = null;
        }
        rotatedFrame = null;

        Log.d(TAG, "Task released resources.");
    }
}
