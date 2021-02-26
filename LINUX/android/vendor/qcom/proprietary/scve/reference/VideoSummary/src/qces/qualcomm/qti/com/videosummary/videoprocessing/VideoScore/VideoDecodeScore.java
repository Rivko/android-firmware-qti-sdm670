/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.videoprocessing.VideoScore;

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
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.Callable;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;

import qces.qualcomm.qti.com.videosummary.Util.Constants;
import qces.qualcomm.qti.com.videosummary.Util.Utils;
import qces.qualcomm.qti.com.videosummary.highlights.HighlightsActivity;
import qces.qualcomm.qti.com.videosummary.providers.QueryHelper;
import qces.qualcomm.qti.com.videosummary.settings.SettingsActivity;
import qces.qualcomm.qti.com.videosummary.videoprocessing.VideoFrameUtils.FrameYUV420SP;
import qces.qualcomm.qti.com.videosummary.videoprocessing.VideoFrameUtils.RotateYUV420SP;
import qces.qualcomm.qti.com.videosummary.videoprocessing.cvmanager.CVManagerJNILib;
import qces.qualcomm.qti.com.videosummary.watchdog.Watchdog;

/*
 * This class decodes video frame by frame to ByteBuffers
 * It uses features introduced in API Level 21, which are
 * codec.getOutputBuffer() and MediaCodec Callback capability
 */
public class VideoDecodeScore implements Runnable {

    private Object decodeScoreTaskObject = new Object();

    private static final String TAG = VideoDecodeScore.class.getSimpleName();

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

    private List<Integer> videoScoreArray;

    private float videoDurationMilliSec;
    private long firstPtsUsec;
    private long secondPtsUsec;
    private int totalFramesEstimate;
    private long timeStampMillis;
    private int currentUriIndex;
    private int numUris;
    private int progress;

    public VideoDecodeScore(Context context, Uri videoUri, long timeStampMillis, int currentUriIndex, int numUris) {
        this.videoUri = videoUri;
        this.context = context;
        this.timeStampMillis = timeStampMillis;
        this.currentUriIndex = currentUriIndex;
        this.numUris = numUris;

        cvNativeInit();
        Log.d(TAG, "CVNativeInit finished");
        Log.d(TAG, "Starting video decode of: " + videoUri.toString());

        videoScoreArray = new ArrayList<>();

    }

    public int blockingCall(int retryCount) {
        Log.i("Test", "start calling cvNativeInit");
        Watchdog.getWatchdogInstance().watchdogPet("before CVNativeInit");
        long start = System.currentTimeMillis();

        int res = CVManagerJNILib.CVNativeInit();
        byte[] scveMode = CVManagerJNILib.SCVE_MODES[SettingsActivity.getPrefSCVETestMode(context)].getBytes();
        res |= CVManagerJNILib.CVNativeSet(scveMode);

        long end = System.currentTimeMillis();
        Watchdog.getWatchdogInstance().watchdogPet("after CVNativeInit");
        Utils.appendCvInitTime(context, retryCount, start, end - start);
        Log.i("Test", "end calling cvNativeInit");
        return res;
    }

    int retryCount = 0;

    void cvNativeInit() {

        ExecutorService executor = Executors.newCachedThreadPool();
        Callable<Object> task = new Callable<Object>() {
            public Object call() {
                Log.i("Test", "start blocking call");
                int res = blockingCall(retryCount);
                Log.i("Test", "end blocking call");
                return new Integer(res);
            }
        };
        Future<Object> future;
        Integer res = null;
        do {
            retryCount++;
            Log.i("Test", "Trying blocking call, remaining trys = " + retryCount);
            future = executor.submit(task);

            try {
                res = (Integer) future.get(5, TimeUnit.SECONDS);
            } catch (TimeoutException ex) {
                Log.i("Test", "TimeoutException");
                // handle the timeout
            } catch (InterruptedException e) {
                // handle the interrupts
                Log.i("Test", "InterruptedException");
            } catch (ExecutionException e) {
                // handle other exceptions
                Log.i("Test", "ExecutionException");
            } finally {
                Log.i("Test", "finally cancelling the blocking call");
                future.cancel(true); // may or may not desire this
                Log.i("Test", "isCancelled = " + future.isCancelled() + " isDone = " + future.isDone());
                if (future.isCancelled()) {
                    //Log.i("Test", "start calling CV deinit");
                    //CVManagerJNILib.CVNativeDeInit();
                    //Log.i("Test", "end calling CV deinit");
                }
                if (res == null) {
                    Log.i("Test", "result is null");
                } else {
                    Log.i("Test", "result is " + res.toString());
                }
            }
        } while (future.isCancelled() && (retryCount < 3));
        if (future.isCancelled() && retryCount >= 3) {
            throw new UnsupportedOperationException("CvInit failed for all 3 trys");
        }
        Log.i("Test", "CV Init completed with result " + res);

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

                format.setInteger(MediaFormat.KEY_COLOR_FORMAT, MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420SemiPlanar);

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

        String durationStr = metadataRetriever.extractMetadata(
                MediaMetadataRetriever.METADATA_KEY_DURATION);

        if (durationStr != null) {
            try {
                videoDurationMilliSec = Float.parseFloat(durationStr);
            } catch (NumberFormatException e) {
                Log.e(TAG, "Failed parsing video duration [" + durationStr + "]", e);
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

                    int sampleSize = extractor.readSampleData(buffer, 0);
                    if (sampleSize < 0) {
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

                if (outIndex >= 0) {
                    ByteBuffer outputFrame = codec.getOutputBuffer(outIndex);
                    MediaFormat format = codec.getOutputFormat();

                    if (info.size == 0) {
                        Log.d(TAG, "got empty frame");
                    } else {

                        final int stride = (format.containsKey("stride") ?
                                format.getInteger("stride") : inputWidth);
                        final int uvOffset = (format.containsKey("slice-height") ?
                                format.getInteger("slice-height") : inputHeight) * stride;

                        final FrameYUV420SP frame = new FrameYUV420SP(outputFrame,
                                inputWidth, inputHeight, info.offset, stride, uvOffset, stride);
                        RotateYUV420SP.rotate(frame, rotatedFrame, rotationDegrees);

                        final int[] scores = CVManagerJNILib.CVNativeProcess(rotatedFrame.buf,
                                rotatedFrame.width, rotatedFrame.height,
                                rotatedFrame.offsetY, rotatedFrame.strideY,
                                rotatedFrame.offsetUV, rotatedFrame.strideUV);

                        Utils.sendMessage(context, TAG, " -- decoded frame: " + decodeCount + " scores[0]: " + scores[0] + " scores[1]: " + scores[1] + " scores[2]: " + scores[2]);
                        byte firstCat = (byte) ((scores[3] >> 16) & 0xFF);
                        byte secondCat = (byte) (scores[3] & 0xFF);

                        Utils.sendMessage(context, TAG, " scores[3]: first cat: " + Utils.getASDCategoryName(firstCat) + " second cat: " + Utils.getASDCategoryName(secondCat));

                        for (int i = 0; i < Constants.MAX_SCORE_MODULES; i++) {
                            videoScoreArray.add(scores[i]);
                        }

                        if (decodeCount == 1)
                            firstPtsUsec = info.presentationTimeUs;
                        else if (decodeCount == 2) {
                            secondPtsUsec = info.presentationTimeUs;
                            totalFramesEstimate = (int) ((videoDurationMilliSec * 1000) / (secondPtsUsec - firstPtsUsec));
                            Log.d(TAG, "Estimating total frame cnt = " + totalFramesEstimate);
                        }


                        if (totalFramesEstimate > 0) {
                            int calcProgress = (decodeCount * 100 / totalFramesEstimate);
                            if (calcProgress != progress) {
                                progress = calcProgress;
                                HighlightsActivity.sendProgressUpdate(context, timeStampMillis, currentUriIndex, numUris, progress);//Scoring
                                Log.d(TAG, "Sending to UI decode+score progress = " + progress);
                            }
                        }

                        //petting the watchdog once in 50frames as approx each fram wil take 30ms for scoring and watch dog timeout is 5sec = 5000ms
                        if (decodeCount % 50 == 0) {
                            Watchdog.getWatchdogInstance().watchdogPet(TAG + " Scoring: progress=" + progress);
                        }
                        decodeCount++;

                    }

                    //render to surface set to false so that we get bytebuffer directly
                    codec.releaseOutputBuffer(outIndex, false);

                }

                    /* All decoded frames have been rendered, we can stop playing now */
                if ((info.flags & MediaCodec.BUFFER_FLAG_END_OF_STREAM) != 0) {
                    Log.d(TAG, "OutputBuffer BUFFER_FLAG_END_OF_STREAM");
                    releaseDecodingResources();
                    byte[] faceTable = CVManagerJNILib.CVNativeGetFaceTable();

                    Log.d(TAG, "video done scoring, so doing DeInit of CVManager");
                    CVManagerJNILib.CVNativeDeInit();

                    QueryHelper.insertIntoScoreKeeper(context, videoUri, videoScoreArray, faceTable);

                    threadContinue();
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

        codec.start();

        //block here until all done
        threadWait();

        Log.d(TAG, "Started video codec");


    }

    private void threadWait() {
        synchronized (decodeScoreTaskObject) {
            try {
                decodeScoreTaskObject.wait();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }

    private void threadContinue() {
        synchronized (decodeScoreTaskObject) {
            try {
                decodeScoreTaskObject.notifyAll();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    public void stop() {
        releaseDecodingResources();
        threadContinue();
    }

    private void releaseDecodingResources() {
        if (codec != null) {
            codec.stop();
            codec.release();
            codec = null;
        }
        if (extractor != null) {
            extractor.release();
            extractor = null;
        }
        rotatedFrame = null;

        Log.d(TAG, "Task released resources.");
    }
}
