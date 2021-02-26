/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.fr_camera.fr;

import android.content.Context;
import android.graphics.ImageFormat;
import android.media.Image;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.Message;
import android.util.Log;

import com.qualcomm.qti.fr_camera.metrics.EventCounter;
import com.qualcomm.qti.fr_camera.metrics.EventFrequencyCounter;
import com.qualcomm.qti.fr_camera.metrics.Metrics;

import java.util.Objects;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicBoolean;

/**
 * This class wraps {@link FaceRecognizer} in a worker thread. The {@link EventListener} interface
 * should be used to obtain the results of face recognition.
 */
public class FaceRecognizerWorker {
    /**
     * Classes may implement this interface to receive the results of face recognition
     */
    public interface EventListener {
        /**
         * This method will be invoked in response to {@link #process(Image, boolean)}ing an image
         * @param faces a {@link Face}[] containing the results of detection (and optionally
         *              recognition)
         * @param wasRecognitionRun true if recognition was run for this frame, false if only
         *                          detection was run
         */
        void onFaceRecognitionResults(Face[] faces, boolean wasRecognitionRun);
    }

    private static final String TAG = "SocialCamera";
    private static final Metrics metrics = Metrics.getInstance();
    private static final int MSG_INITIALIZE = 0x2000;
    private static final int MSG_TERMINATE = 0x2001;
    private static final int MSG_PROCESS = 0x2002;

    //The below 3 saves the width, ht and stride from the current as its needed during "remove users from face db" to create FaceRecognizer()
    public static int PreviousWidth = -1;
    public static int PreviousHeight = -1;
    public static int PreviousRowStride = -1;

    private final Context context;
    private final EventListener listener;
    private final Handler handler;
    private final Handler uiHandler;
    private final EventFrequencyCounter frFpsMetric = new EventFrequencyCounter("FPS(FR)",
            EventCounter.DEFAULT_POLL_PERIOD, EventCounter.DEFAULT_POLL_PERIOD_TIME_UNIT,
            1, TimeUnit.SECONDS, 1, TimeUnit.SECONDS, 60);

    private boolean stopped;
    private final AtomicBoolean isProcessing = new AtomicBoolean();

    /**
     * Construct a new {@link FaceRecognizerWorker}
     * @param context the active {@link Context}
     * @param listener an {@link EventListener} to receive recognition results
     * @throws NullPointerException if context or listener are null
     */
    public FaceRecognizerWorker(Context context, EventListener listener) {
        Objects.requireNonNull(context, "context may not be null");
        Objects.requireNonNull(listener, "listener may not be null");

        this.context = context;
        this.listener = listener;

        HandlerThread thread = new HandlerThread(TAG);
        thread.start();
        handler = new Handler(thread.getLooper(), handlerCallback);
        handler.sendEmptyMessage(MSG_INITIALIZE);

        uiHandler = new Handler(Looper.getMainLooper());
    }

    /**
     * Stop this {@link FaceRecognizerWorker}. After invoking this method, no more images will be
     * processed (though there could still be 1 image in the middle of processing, and the
     * {@link EventListener} will be notifed when processing this image is complete, even after stop
     * has been invoked.
     */
    public void stop() {
        if (!stopped) {
            stopped = true;
            handler.sendMessageAtFrontOfQueue(handler.obtainMessage(MSG_TERMINATE));
        }
    }

    /**
     * Submit an {@link Image} for processing. Only a single {@link Image} will be processed at a
     * time; any additional Images submitted while another is being processed will be immediately
     * {@link Image#close()}d.
     * @param image the {@link ImageFormat#YUV_420_888} or {@link ImageFormat#NV21}-encoded
     *              {@link Image} to process. The first image to be processed will fix the image
     *              size for this {@link FaceRecognizerWorker}; all subsequent {@link Image}s
     *              provided to process must be of identical dimensions.
     * @param recognize true to perform face recognition on this {@link Image}, false to only
     *                  perform face detection
     * @throws NullPointerException if image is null
     * @throws IllegalArgumentException if image is not {@link ImageFormat#YUV_420_888} or
     *         {@link ImageFormat#NV21}
     * @throws IllegalStateException if {@link #stop()} has been invoked
     */
    public void process(Image image, boolean recognize) {
        Objects.requireNonNull(image, "image may not be null");

        if (image.getFormat() != ImageFormat.YUV_420_888 && image.getFormat() != ImageFormat.NV21) {
            throw new IllegalArgumentException("image format must be YUV_420_888 or NV21");
        } else if (stopped) {
            throw new IllegalStateException("Cannot invoke process when stopped");
        }

        if (!isProcessing.getAndSet(true)) {
            handler.sendMessage(handler.obtainMessage(MSG_PROCESS, recognize ? 1 : 0, 0, image));
        } else {
            image.close();
        }
    }

    @Override
    protected void finalize() throws Throwable {
        super.finalize();
        stop();
    }

    @SuppressWarnings("FieldCanBeLocal")
    private final Handler.Callback handlerCallback = new Handler.Callback() {
        private FaceRecognizer faceRecognizer;
        private int width;
        private int height;

        @Override
        public boolean handleMessage(Message msg) {
            boolean handled = true;

            switch (msg.what) {
                case MSG_INITIALIZE:
                    metrics.addMetric(frFpsMetric);
                    break;

                case MSG_TERMINATE:
                    if (faceRecognizer != null) {
                        faceRecognizer.close();
                        faceRecognizer = null;
                    }
                    metrics.removeMetric(frFpsMetric);
                    //noinspection ConstantConditions
                    Looper.myLooper().quit();
                    break;

                case MSG_PROCESS:
                {
                    Image image = (Image)msg.obj;
                    Image.Plane y = image.getPlanes()[0];
                    final boolean recognize = (msg.arg1 == 1);

                    if (faceRecognizer == null) {
                        Log.v(TAG, "Constructing FaceRecognizer in worker thread");

                        //The below 3 saves the width, ht and stride from the current as its needed during "remove users from face db" to create FaceRecognizer()
                        PreviousWidth = width = image.getWidth();
                        PreviousHeight = height = image.getHeight();
                        PreviousRowStride = y.getRowStride();

                        FaceRecognizer.ScveMode scveMode = FaceRecognizer.ScveMode.DEFAULT;
                        if (FacePreferences.isCPUOffloadEnabled(context)) {
                            scveMode = FaceRecognizer.ScveMode.CPU_OFF_LOAD;
                            Log.d(TAG, "CPU_OFF_LOAD mode");
                        }
                        faceRecognizer = new FaceRecognizer(context,
                                scveMode, FaceRecognizer.FaceRecoMode.VIDEO,
                                FaceRecognizer.FrameFormat.GRAYSCALE, width, height,
                                new int[] { y.getRowStride() }, 30);
                    }

                    try {
                        if (image.getWidth() != width || image.getHeight() != height) {
                            Log.w(TAG, "Image has changed dimensions; not recognizing faces");
                            break;
                        }

                        final Face[] faces = faceRecognizer.detectFaces(y.getBuffer(), recognize);
                        frFpsMetric.trigger();
                        postOnFaceRecognitionResults(faces, recognize);
                    } catch (FaceRecognizerException e) {
                        Log.w(TAG, "Error while recognizing faces, skipping image", e);
                        postOnFaceRecognitionResults(new Face[0], recognize);
                    } finally {
                        isProcessing.set(false);
                        image.close();
                    }
                    break;
                }

                default:
                    handled = false;
                    break;
            }

            return handled;
        }

        private void postOnFaceRecognitionResults(final Face[] faces, final boolean recognize) {
            uiHandler.post(new Runnable() {
                @Override
                public void run() {
                    listener.onFaceRecognitionResults(faces, recognize);
                }
            });
        }
    };
}
