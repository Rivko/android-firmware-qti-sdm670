/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.videoprocessing.VideoAssembly;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.media.MediaCodec;
import android.media.MediaExtractor;
import android.media.MediaFormat;
import android.os.Bundle;
import android.os.Message;
import android.util.Log;

import java.nio.ByteBuffer;

import qces.qualcomm.qti.com.videosummary.highlights.HighlightsActivity;
import qces.qualcomm.qti.com.videosummary.videoprocessing.VideoFrameUtils.FrameUtils;
import qces.qualcomm.qti.com.videosummary.videoprocessing.VideoFrameUtils.FrameYUV420SP;
import qces.qualcomm.qti.com.videosummary.videoprocessing.VideoFrameUtils.RotateYUV420SP;
import qces.qualcomm.qti.com.videosummary.videoprocessing.VideoServices.ScoreOrAssemblyService;
import qces.qualcomm.qti.com.videosummary.videoprocessing.fastcv.FastCVJNILib;
import qces.qualcomm.qti.com.videosummary.watchdog.Watchdog;

/**
 * Created by matthewf on 8/12/15.
 */
public class VideoDecoderCallBack extends MediaCodec.Callback {

    private static final String TAG = VideoDecoderCallBack.class.getSimpleName();

    private static final int THUMBNAIL_PIXEL_WIDTH = 640;
    private static final int THUMBNAIL_PIXEL_HEIGHT = 360;

    private static final int FRAME_QUE_SIZE_TO_THROTTLE_DECODER = 4;

    //private Context context;
    private MediaExtractor extractor;

    private final VideoTranscodeSharedData sharedData;

    private boolean decodeEOS_sent = false;

    private boolean readFirstFrameVideoInfo = true;

    private int progress;
    private int xOffset;


    public VideoDecoderCallBack(MediaExtractor extractor, VideoTranscodeSharedData sharedData) {

        this.extractor = extractor;
        this.sharedData = sharedData;

    }

    @Override
    public void onInputBufferAvailable(MediaCodec codec, int inIndex) {

        if (inIndex >= 0) {

            if (sharedData.getFrameQueueSize() < FRAME_QUE_SIZE_TO_THROTTLE_DECODER) {

                ByteBuffer buffer = codec.getInputBuffer(inIndex);
                int sampleSize = extractor.readSampleData(buffer, 0);

                if (sampleSize < 0 || (sharedData.checkTranscodingAllDone() && !decodeEOS_sent)) {
                    decodeEOS_sent = true;
                    //we're at end of file so submit EOS to the decoder input
                    Log.d(TAG, "Decoder input EOS reached");
                    codec.queueInputBuffer(inIndex, 0, 0, 0,
                            MediaCodec.BUFFER_FLAG_END_OF_STREAM);

                } else {

                    long sampleTime = extractor.getSampleTime();

                    codec.queueInputBuffer(inIndex, 0, sampleSize,
                            sampleTime, 0);
                    extractor.advance();

                }
            } else {
                Log.d(TAG, "Decoder input: frameQueue size is " + sharedData.getFrameQueueSize() + " so not queueing the buffer at index: " + inIndex);

                //queueing the input buffer with no data so that we can get back this input buffer later
                codec.queueInputBuffer(inIndex, 0, 0, 0, 0);

            }
        }

    }

    @Override
    public void onOutputBufferAvailable(MediaCodec codec, int outIndex, MediaCodec.BufferInfo info) {

        if (outIndex >= 0 && info.size > 0) {

            MediaFormat format = codec.getOutputFormat();
            ByteBuffer outputFrame;

            if (readFirstFrameVideoInfo)
                doOnFirstDecodedFrame(format);

            if (sharedData.keepCurrentDecodeOutputFrame()) {

                outputFrame = codec.getOutputBuffer(outIndex);
                processFrame(outputFrame);

                if (!sharedData.thumbNailCreated) {
                    createThumbnail(sharedData.rotatedFrame);
                }

            } else {
                Log.d(TAG, "    Decoder output frame skipped: " + sharedData.totalDecodeOutputFrames);
            }

            sharedData.totalDecodeOutputFrames++;

            int calcProgress = (sharedData.totalDecodeOutputFrames * 100 / sharedData.keepFrames.length);
            if (calcProgress != progress) {
                progress = calcProgress;
                HighlightsActivity.sendProgressUpdate(sharedData.getContext(), sharedData.getTimeStampMillis(), -1, -1, progress);//Assembling
                Log.d(TAG, "        Encoder input sending progress update");
            }

            //petting the watchdog once in 100frames as approx each frame will take ~15 to 20ms for assembling and watch dog timeout is 5sec = 5000ms
            if (sharedData.totalDecodeOutputFrames % 100 == 0) {
                Watchdog.getWatchdogInstance().watchdogPet(TAG + " Assembling progress:" + progress);
            }

            //render to surface set to false so that we get bytebuffer directly
            codec.releaseOutputBuffer(outIndex, false);

        }

        boolean EOS = (info.flags & MediaCodec.BUFFER_FLAG_END_OF_STREAM) != 0;

        if (EOS) {
            Log.d(TAG, "            Decoder output: got EOS, done.");
            releaseDecoderResources(codec);
            sendCodecManagerMsg(VideoCodecManager.CODEC_MSG_DECODE_VIDEO_DONE);
            return;
        }

    }

    public void releaseDecoderResources(MediaCodec decoder) {
        if (decoder != null) {
            decoder.stop();
            decoder.release();
        }
        if (extractor != null) {
            extractor.release();
            extractor = null;
        }
    }

    private void doOnFirstDecodedFrame(MediaFormat decodeFormat) {
        sharedData.decodeStride = (decodeFormat.containsKey("stride") ?
                decodeFormat.getInteger("stride") : sharedData.extractorWidth);
        sharedData.decodeUV_Offset = (decodeFormat.containsKey("slice-height") ?
                decodeFormat.getInteger("slice-height") : sharedData.extractorHeight) * sharedData.decodeStride;

        readFirstFrameVideoInfo = false;
    }

    private void generateVideoFrameFromImgTest() {

        BitmapFactory.Options op = new BitmapFactory.Options();
        op.inPreferredConfig = Bitmap.Config.RGB_565;
        Bitmap srcBmp = BitmapFactory.decodeFile("sdcard/DCIM/xmas.jpg", op);

        Bitmap dstBmp = Bitmap.createBitmap(srcBmp,xOffset+=2,xOffset+=2,1920,1080);
        int bytes = dstBmp.getByteCount();
        ByteBuffer srcBuffer = ByteBuffer.allocateDirect(bytes); //Create a new buffer
        dstBmp.copyPixelsToBuffer(srcBuffer); //Move the byte data to the buffer

        ByteBuffer outBuffer = ByteBuffer.allocateDirect(dstBmp.getWidth() * dstBmp.getHeight() * 3 / 2); //Create a new buffer
        FastCVJNILib.RGB565ByteBufferToYUV420ByteBuffer(srcBuffer, dstBmp.getWidth(), dstBmp.getHeight(), dstBmp.getRowBytes(), outBuffer );

        sharedData.addToQueue(outBuffer);
    }

    private void processFrame(ByteBuffer outputFrame) {

        final FrameYUV420SP inputFrame = new FrameYUV420SP(outputFrame,
                sharedData.extractorWidth, sharedData.extractorHeight, 0,
                sharedData.decodeStride, sharedData.decodeUV_Offset, sharedData.decodeStride);


        Log.d(TAG, "    Decoder output: storing frame " + sharedData.totalDecodeOutputFrames + " with rotation " + sharedData.rotationDegrees
                + " into queue of size = " + sharedData.getFrameQueueSize());

        //even if rotationDegrees is zero we run it anyways so that extra rows between Luma and Chroma blocks are removed
        RotateYUV420SP.rotate(inputFrame, sharedData.rotatedFrame, sharedData.rotationDegrees);

        if (sharedData.rotatedFrame.width == sharedData.finalEncodeWidth &&
                sharedData.rotatedFrame.height == sharedData.finalEncodeHeight)
            //no scaling necessary, store to queue as-is
            sharedData.addToQueue(sharedData.rotatedFrame.buf);
        else {
            //do downscaling + letterboxing
            ByteBuffer newBuf = ByteBuffer.allocateDirect(sharedData.finalEncodeWidth * sharedData.finalEncodeHeight * 3 / 2);
            FastCVJNILib.YUV420SPscaleDownLetterbox(sharedData.rotatedFrame.buf, sharedData.rotatedFrame.width,
                    sharedData.rotatedFrame.height, newBuf, sharedData.finalEncodeWidth, sharedData.finalEncodeHeight, false);
            sharedData.addToQueue(newBuf);
        }
    }

    private void createThumbnail(FrameYUV420SP rotFrame) {

        ByteBuffer bBufferClone = FrameUtils.cloneByteBuffer(rotFrame.buf);

        int thumbWidth;
        int thumbHeight;

        if (rotFrame.width > THUMBNAIL_PIXEL_WIDTH) {
            thumbWidth = THUMBNAIL_PIXEL_WIDTH;
            thumbHeight = THUMBNAIL_PIXEL_HEIGHT;
        } else {
            int intScale = rotFrame.width / 16;
            thumbWidth = intScale * 16;
            thumbHeight = intScale * 9;
        }

        //first do downscaling + letterboxing from (input/output = YUV420SP bytebuffer)
        ByteBuffer newBuf = ByteBuffer.allocateDirect(thumbWidth * thumbHeight * 3 / 2);
        FastCVJNILib.YUV420SPscaleDownLetterbox(bBufferClone, rotFrame.width,
                rotFrame.height, newBuf, thumbWidth, thumbHeight, true);
        Bitmap scaledBitmap = Bitmap.createBitmap(thumbWidth, thumbHeight, Bitmap.Config.ARGB_8888);
        FastCVJNILib.YUV420ByteBufferToRGB8888Bitmap(newBuf, thumbWidth, thumbHeight, thumbWidth, thumbWidth, scaledBitmap);

        FrameUtils.writeBitmapToPng(scaledBitmap, sharedData.getThumbImgPath());

        sharedData.thumbNailCreated = true;
    }

    private void sendCodecManagerMsg(int msgCode) {
        Message m = ScoreOrAssemblyService.videoCodecManager.mHandler.obtainMessage();
        Bundle b = new Bundle();
        b.putInt(VideoCodecManager.CODEC_MSG, msgCode);
        m.setData(b);
        ScoreOrAssemblyService.videoCodecManager.mHandler.sendMessage(m);
    }

    @Override
    public void onError(MediaCodec mediaCodec, MediaCodec.CodecException e) {
        Log.d(TAG, "MediaCodec Decoder Exception: " + e.getDiagnosticInfo());
    }

    @Override
    public void onOutputFormatChanged(MediaCodec mediaCodec, MediaFormat mediaFormat) {
        Log.d(TAG, "MediaCodec Decoder MediaFormat Changed");
    }


}
