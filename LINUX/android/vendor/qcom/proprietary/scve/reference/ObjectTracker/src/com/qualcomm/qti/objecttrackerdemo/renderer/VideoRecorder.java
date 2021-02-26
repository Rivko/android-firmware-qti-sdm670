/*
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.objecttrackerdemo.renderer;

import java.io.File;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

import com.qualcomm.qti.khronos.EGL15;
import com.qualcomm.qti.khronos.EGL15Helper;
import com.qualcomm.qti.khronos.EGLAndroidPresentationTime;
import com.qualcomm.qti.khronos.GLES30Helper;
import com.qualcomm.qti.khronos.EGL15Helper.EGLContextToken;
import com.qualcomm.qti.objecttracker.misc.CoordinateSpace;
import com.qualcomm.qti.objecttracker.misc.Orientation;
import com.qualcomm.qti.objecttracker.tracker.ObjectTracker;
import com.qualcomm.qti.objecttrackerdemo.renderer.primitives.Renderer;
import com.qualcomm.qti.objecttrackerdemo.renderer.primitives.TextureFormat;

import android.annotation.SuppressLint;
import android.content.Context;
import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.CamcorderProfile;
import android.media.MediaCodec;
import android.media.MediaCodec.BufferInfo;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.media.MediaMuxer;
import android.media.MediaRecorder;
import android.net.Uri;
import android.opengl.GLES30;
import android.os.Build;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.util.Log;
import android.view.Display;
import android.view.Surface;

/**
 * This class provides video recording functionality for the output of the {@link GLFrameTranslator}
 */
@SuppressWarnings("deprecation")
public class VideoRecorder implements GLFrameSink {
    /**
     * Classes may implement this interface to receive callback notifications for
     * {@link VideoRecorder} events
     */
    public static interface RecordingEventListener {
        /**
         * This method will be invoked when video recording is started
         */
        void onRecordingStarted();

        /**
         * This method will be invoked if there is an error during video recording
         */
        void onRecordingError();

        /**
         * This method will be invoked when video recording successfully completes
         * @param video a {@link File} pointing to the encoded video content
         */
        void onRecordingFinished(File video);
    }

    private static final String TAG = "VideoRecorder";
    private static final String VIDEO_MIME_TYPE = "video/avc";
    private static final String AUDIO_MIME_TYPE = "audio/mp4a-latm";
    private static final int AUDIO_SAMPLING_FREQ = 44100;
    private static final int AUDIO_STREAM_BYTES_PER_SEC = 1 * AUDIO_SAMPLING_FREQ * 2; //44.1kHz mono 16-bit PCM
    private static final int AUDIO_RECORD_BUFFER_LEN = (AUDIO_STREAM_BYTES_PER_SEC * 100) / 1000;
    private static final int AUDIO_SAMPLE_BUFFER_LEN = (AUDIO_STREAM_BYTES_PER_SEC * 10) / 1000;
    private static final int NUM_AUDIO_SAMPLE_BUFFERS = 8;
    private static final int MEDIA_CODEC_DEQUEUE_MAX_RETRIES = 100;
    private static final int END_OF_STREAM_TIMEOUT_US = 10000;

    private final Context context;
    private final RendererFactory rendererFactory;
    private final ObjectTracker tracker;
    private final Display displayInfo;
    private final RecordingEventListener listener;
    private final Handler uiThreadHandler;

    // These members may only be accessed from the context of the video recorder thread
    private boolean resourcesCreated;
    private CoordinateSpace cameraSpace;
    private CoordinateSpace outputSpace;
    private TextureFormat textureFormat;
    private boolean recording;
    private File outputFile;
    private int muxerTracksRemaining;
    private MediaMuxer muxer;
    private long currentTimestampUs;
    private VideoEncoder videoEncoder;
    private AudioEncoder audioEncoder;
    private EGLContextToken eglToken;
    private Renderer renderer;
    private GLFrameRendererAdapter rendererAdapter;
    private int frameTexture;
    private int trackedObjectTexture;
    private final BufferInfo muxerBufferInfo = new BufferInfo();

    // Access to these members must be protected by lock
    private final Object lock = new Object();
    private HandlerThread thread;
    private Handler handler;

    /**
     * Construct a new {@link VideoRecorder}
     * @param context the application {@link Context}
     * @param rendererFactory a {@link RendererFactory} which can be used to create renderers for
     *      the current app state
     * @param tracker an {@link ObjectTracker} from which to receive tracked object updates
     * @param displayInfo Information about the {@link Display} on which the app is running
     * @param listener a {@link RecordingEventListener} on which recording event notifications
     *      will be delivered
     * @throws IllegalArgumentException if context, rendererFactory, tracker, displayInfo, or
     *      listener are null
     */
    public VideoRecorder(Context context, RendererFactory rendererFactory, ObjectTracker tracker,
            Display displayInfo, RecordingEventListener listener) {
        if (context == null) {
            throw new IllegalArgumentException("context cannot be null");
        } else if (rendererFactory == null) {
            throw new IllegalArgumentException("rendererFactory cannot be null");
        } else if (tracker == null) {
            throw new IllegalArgumentException("tracker may not be null");
        } else if (displayInfo == null) {
            throw new IllegalArgumentException("displayInfo cannot be null");
        } else if (listener == null) {
            throw new IllegalArgumentException("listener cannot be null");
        }

        this.context = context;
        this.rendererFactory = rendererFactory;
        this.tracker = tracker;
        this.displayInfo = displayInfo;
        this.listener = listener;

        uiThreadHandler = new Handler(Looper.getMainLooper());
    }

    /**
     * Start recording. This may only be called after the {@link GLFrameTranslator} to which this
     * object is attached is running.
     */
    public void startRecording() {
        synchronized (lock) {
            if (handler == null) {
                throw new RuntimeException("Cannot start recording when VideoRecorder not running");
            }

            handler.post(new Runnable() {
                @Override
                public void run() {
                    doStartRecording();
                }
            });
        }
    }

    /**
     * Stop recording. This may only be called after the {@link GLFrameTranslator} to which this
     * object is attached is running. If the {@link GLFrameTranslator} is stopped, recording will
     * be stopped automatically.
     */
    public void stopRecording() {
        synchronized (lock) {
            if (handler == null) {
                throw new RuntimeException("Cannot stop recording when VideoRecorder not running");
            }

            handler.post(new Runnable() {
                @Override
                public void run() {
                    doStopRecording();
                }
            });
        }
    }

    /** {@inheritDoc} */
    @Override
    public void onInitialize(final CoordinateSpace cameraSpace, final TextureFormat textureFormat) {
        synchronized (lock) {
            thread = new HandlerThread(TAG);
            thread.start();
            handler = new Handler(thread.getLooper());
            handler.post(new Runnable() {
                @Override
                public void run() {
                    doInitialize(cameraSpace, textureFormat);
                }
            });
        }
    }

    /** {@inheritDoc} */
    @Override
    public void onRelease() {
        synchronized (lock) {
            handler.post(new Runnable() {
                @Override
                public void run() {
                    doRelease();
                }
            });

            thread.quitSafely();
            try {
                thread.join();
            } catch (InterruptedException e) {
                Log.i(TAG, "Interrupted while waiting for thread to terminate", e);
                Thread.currentThread().interrupt();
            }
        }
    }

    /** {@inheritDoc} */
    @Override
    public void onFrame(final GLFrame frame) {
        synchronized (lock) {
            handler.post(new Runnable() {
                @Override
                public void run() {
                    doFrame(frame);
                }
            });
        }
    }

    // N.B. Must be called in context of video recorder thread
    private void doInitialize(CoordinateSpace cameraSpace, TextureFormat textureFormat) {
        Log.i(TAG, "Video recorder initialized");

        this.cameraSpace = cameraSpace;
        final boolean rotated = Orientation.relativeOrientation(cameraSpace.orientation,
                Orientation.fromSurfaceRotation(displayInfo.getRotation())).isRightAngle();
        final int rotatedWidth = (rotated ? cameraSpace.height : cameraSpace.width);
        final int rotatedHeight = (rotated ? cameraSpace.width : cameraSpace.height);
        outputSpace = CoordinateSpace.forDisplay(rotatedWidth, rotatedHeight, displayInfo);

        this.textureFormat = textureFormat;
        recording = false;

        resourcesCreated = createRecordingResources();
        if (!resourcesCreated) {
            Log.e(TAG, "Failed to create recording resources");
            releaseRecordingResources();
        }
    }

    // N.B. Must be called in context of video recorder thread
    private void doRelease() {
        if (recording) {
            doStopRecording();
        }

        releaseRecordingResources();

        cameraSpace = null;
        outputSpace = null;
        textureFormat = null;

        Log.i(TAG, "Video recorder released");
    }

    // N.B. Must be called in context of video recorder thread
    private void doFrame(GLFrame frame) {
        if (!recording) {
            frame.release();
            return;
        }

        rendererAdapter.doInputFrame(frame);
        frame.release();

        // Flush the video encoder to the muxer, and if there is space for new frames, render to the
        // encoder input
        if (flushEncoderToMuxer(videoEncoder, false)) {
            rendererAdapter.render();
            EGLAndroidPresentationTime.eglPresentationTimeANDROID(eglToken.display,
                    eglToken.surface, currentTimestampUs * 1000l);
            EGL15.eglSwapBuffers(eglToken.display, eglToken.surface);
        }

        // Flush the audio encoder to the muxer, and if there is space for new frames, render to the
        // encoder input
        if (flushEncoderToMuxer(audioEncoder, false)) {
            ByteBuffer[] codecBuffers = audioEncoder.codec.getInputBuffers();
            int i;

            while (!audioEncoder.filledBufferQueue.isEmpty() &&
                    (i = audioEncoder.codec.dequeueInputBuffer(0)) != -1) {
                ByteBuffer inputBuffer = audioEncoder.filledBufferQueue.peek();
                codecBuffers[i].clear();
                if (inputBuffer.remaining() > codecBuffers[i].remaining()) {
                    codecBuffers[i].limit(inputBuffer.remaining());
                }
                codecBuffers[i].put(inputBuffer);
                codecBuffers[i].flip();

                long ts = currentTimestampUs;
                currentTimestampUs += (codecBuffers[i].limit() * 1000000l +
                        AUDIO_STREAM_BYTES_PER_SEC - 1) / AUDIO_STREAM_BYTES_PER_SEC;
                audioEncoder.codec.queueInputBuffer(i, 0, codecBuffers[i].limit(), ts, 0);

                if (inputBuffer.remaining() == 0) {
                    audioEncoder.filledBufferQueue.remove();
                    audioEncoder.freeBufferQueue.add(inputBuffer);
                }
            }
        }
    }

    // N.B. Must be called in context of video recorder thread
    private void doStartRecording() {
        if (!resourcesCreated) {
            Log.e(TAG, "Cannot start recording if resource creation failed");
            uiThreadHandler.post(notifyListenerOnRecordingError);
            return;
        } else if (recording) {
            Log.w(TAG, "Requested to start recording when already recording");
            return;
        }

        videoEncoder.codec.start();
        audioEncoder.codec.start();
        audioEncoder.audioRecorder.startRecording();
        audioEncoder.audioRecorderThread = new Thread(audioEncoder, TAG + "_AudioRecorder");
        audioEncoder.audioRecorderThread.start();

        recording = true;

        Log.i(TAG, "Video recording started");

        uiThreadHandler.post(notifyListenerOnRecordingStarted);
    }

    // N.B. Must be called in context of video recorder thread
    private void doStopRecording() {
        if (!recording) {
            Log.w(TAG, "Requested to stop recording when not recording");
            return;
        }

        recording = false;

        flushEncoderToMuxer(audioEncoder, true);
        flushEncoderToMuxer(videoEncoder, true);
        try {
            muxer.stop(); // has a tendency to throw IllegalStateException if anything went wrong
            Util.notifyMediaScannerOfContent(context, Uri.fromFile(outputFile));
            uiThreadHandler.post(notifyListenerOnRecordingFinished);
        } catch (IllegalStateException e) {
            uiThreadHandler.post(notifyListenerOnRecordingError);
        }

        audioEncoder.audioRecorderThread.interrupt();
        try {
            audioEncoder.audioRecorderThread.join();
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        }
        audioEncoder.filledBufferQueue.drainTo(audioEncoder.freeBufferQueue);

        audioEncoder.codec.stop();
        videoEncoder.codec.stop();

        Log.i(TAG, "Video recording stopped");
    }

    // N.B. Must be called in context of video recorder thread
    @SuppressLint("InlinedApi")
    private boolean createRecordingResources() {
        try {
            outputFile = Util.generateMediaFile(Util.MediaFileTypes.MPEG_4);
            if (outputFile == null) {
                throw new IOException("Failed creating output file for video recording");
            }

            muxer = new MediaMuxer(outputFile.getCanonicalPath(),
                    MediaMuxer.OutputFormat.MUXER_OUTPUT_MPEG_4);
            muxerTracksRemaining = 0;
            currentTimestampUs = 0;
        } catch (IOException e) {
            Log.e(TAG, "Failed creating media muxer", e);
            return false;
        }

        CamcorderProfile camProfile = CamcorderProfile.get(CamcorderProfile.QUALITY_1080P);

        try {
            videoEncoder = new VideoEncoder();
            videoEncoder.format = MediaFormat.createVideoFormat(VIDEO_MIME_TYPE,
                    outputSpace.width, outputSpace.height);
            videoEncoder.format.setInteger(MediaFormat.KEY_BIT_RATE, camProfile.videoBitRate);
            videoEncoder.format.setInteger(MediaFormat.KEY_COLOR_FORMAT,
                    MediaCodecInfo.CodecCapabilities.COLOR_FormatSurface);
            videoEncoder.format.setInteger(MediaFormat.KEY_FRAME_RATE, 30);
            videoEncoder.format.setInteger(MediaFormat.KEY_I_FRAME_INTERVAL, 10);
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
                videoEncoder.format.setLong(MediaFormat.KEY_REPEAT_PREVIOUS_FRAME_AFTER, 100000l);
            }
            videoEncoder.codec = MediaCodec.createEncoderByType(VIDEO_MIME_TYPE);
            videoEncoder.codec.configure(videoEncoder.format, null, null,
                    MediaCodec.CONFIGURE_FLAG_ENCODE);
            videoEncoder.videoCodecSurface = videoEncoder.codec.createInputSurface();
            ++muxerTracksRemaining;
        } catch (Exception e) {
            Log.e(TAG, "Failed creating video encoder for " + VIDEO_MIME_TYPE, e);
            return false;
        }

        try {
            audioEncoder = new AudioEncoder();
            audioEncoder.format = MediaFormat.createAudioFormat(AUDIO_MIME_TYPE,
                    AUDIO_SAMPLING_FREQ, 1);
            audioEncoder.format.setInteger(MediaFormat.KEY_BIT_RATE, camProfile.audioBitRate);
            audioEncoder.codec = MediaCodec.createEncoderByType(AUDIO_MIME_TYPE);
            audioEncoder.codec.configure(audioEncoder.format, null, null,
                    MediaCodec.CONFIGURE_FLAG_ENCODE);
            ++muxerTracksRemaining;
        } catch (Exception e) {
            Log.e(TAG, "Failed creating audio encoder for " + AUDIO_MIME_TYPE, e);
            return false;
        }

        int audioBufferSize = Math.max(AudioRecord.getMinBufferSize(AUDIO_SAMPLING_FREQ,
                AudioFormat.CHANNEL_IN_MONO, AudioFormat.ENCODING_PCM_16BIT),
                AUDIO_RECORD_BUFFER_LEN);
        audioEncoder.audioRecorder = new AudioRecord(MediaRecorder.AudioSource.DEFAULT,
                AUDIO_SAMPLING_FREQ, AudioFormat.CHANNEL_IN_MONO, AudioFormat.ENCODING_PCM_16BIT,
                audioBufferSize);
        if (audioEncoder.audioRecorder.getState() != AudioRecord.STATE_INITIALIZED) {
            Log.e(TAG, "Failed creating audio recorder");
            return false;
        }

        eglToken = EGL15Helper.initializeGLES30EGLContext(videoEncoder.videoCodecSurface, true);
        EGL15Helper.makeEGLContextCurrent(eglToken);
        frameTexture = GLES30Helper.generateTexture(textureFormat.glTextureTarget);
        trackedObjectTexture = GLES30Helper.generateTexture(textureFormat.glTextureTarget);

        renderer = rendererFactory.createRenderer(RendererFactory.Usage.VIDEO, cameraSpace,
                outputSpace, textureFormat);
        renderer.init();

        rendererAdapter = new GLFrameRendererAdapter(tracker);
        rendererAdapter.setRenderer(renderer);
        rendererAdapter.setTextures(frameTexture, trackedObjectTexture);

        return true;
    }

    // N.B. Must be called in context of video recorder thread
    private void releaseRecordingResources() {
        GLES30.glDeleteTextures(2, new int[] { frameTexture, trackedObjectTexture }, 0);
            frameTexture = 0;
        trackedObjectTexture = 0;

        if (rendererAdapter != null) {
            rendererAdapter.release();
            rendererAdapter = null;
        }

        if (renderer != null) {
            renderer.release();
            renderer = null;
        }

        if (eglToken != null) {
            EGL15Helper.releaseEGLContext(eglToken);
            eglToken = null;
        }

        if (videoEncoder != null) {
            videoEncoder.release();
            videoEncoder = null;
        }

        if (audioEncoder != null) {
            audioEncoder.release();
            audioEncoder = null;
        }

        if (muxer != null) {
            muxer.release();
            muxer = null;
        }
    }

    // Returns true if the encoder can accept new data, else false
    // N.B. Must be called in context of video recorder thread
    private boolean flushEncoderToMuxer(MediaEncoder encoder, boolean endOfStream) {
        if (muxerTracksRemaining > 0 && encoder.track != -1) {
            // N.B. Wait for other encoders to start
            return false;
        }

        if (endOfStream) {
            if (encoder instanceof VideoEncoder) {
                ((VideoEncoder)encoder).codec.signalEndOfInputStream();
            } else {
                for (int i = 0; i < MEDIA_CODEC_DEQUEUE_MAX_RETRIES; i++) {
                    int bufferIdx = encoder.codec.dequeueInputBuffer(END_OF_STREAM_TIMEOUT_US);
                    if (bufferIdx >= 0) {
                        encoder.codec.queueInputBuffer(bufferIdx, 0, 0, currentTimestampUs,
                                MediaCodec.BUFFER_FLAG_END_OF_STREAM);
                        break;
                    }
                }
            }
        }

        ByteBuffer[] codecBuffers = encoder.codec.getOutputBuffers();
        for (int i = 0; i < MEDIA_CODEC_DEQUEUE_MAX_RETRIES; i++) {
            int bufferIdx = encoder.codec.dequeueOutputBuffer(muxerBufferInfo,
                    endOfStream ? END_OF_STREAM_TIMEOUT_US : 0);
            if (bufferIdx >= 0) {
                if ((muxerBufferInfo.flags & MediaCodec.BUFFER_FLAG_CODEC_CONFIG) != 0) {
                    Log.v(TAG, "Ignoring BUFFER_FLAG_CODEC_CONFIG buffer for encode");
                    muxerBufferInfo.size = 0;
                }

                if ((muxerBufferInfo.flags & MediaCodec.BUFFER_FLAG_END_OF_STREAM) != 0) {
                    // N.B. QCOM AVC video codec generates EOS with an invalid timestamp
                    muxerBufferInfo.presentationTimeUs = currentTimestampUs;
                }

                codecBuffers[bufferIdx].limit(muxerBufferInfo.offset + muxerBufferInfo.size);
                codecBuffers[bufferIdx].position(muxerBufferInfo.offset);
                muxer.writeSampleData(encoder.track, codecBuffers[bufferIdx], muxerBufferInfo);
                encoder.codec.releaseOutputBuffer(bufferIdx, false);

                if ((muxerBufferInfo.flags & MediaCodec.BUFFER_FLAG_END_OF_STREAM) != 0) {
                    if (!endOfStream) {
                        Log.w(TAG, "Media codec generated end of stream unexpectedly");
                    } else {
                        // End of stream requested and achieved. Don't add any more data.
                        return false;
                    }
                }
            } else if (bufferIdx == MediaCodec.INFO_OUTPUT_BUFFERS_CHANGED) {
                codecBuffers = encoder.codec.getOutputBuffers();
            } else if (bufferIdx == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
                if (encoder.track != -1) {
                    Log.w(TAG, "INFO_OUTPUT_FORMAT_CHANGED received, but muxer already " +
                            "configured. Ignoring.");
                } else {
                    encoder.format = encoder.codec.getOutputFormat();
                    encoder.track = muxer.addTrack(encoder.format);
                    --muxerTracksRemaining;
                    if (muxerTracksRemaining == 0) {
                        muxer.start();
                    } else {
                        // Still waiting for other tracks to start, don't start accumulating input
                        return false;
                    }
                }
            } else if (!endOfStream) {
                // MediaCodec.INFO_TRY_AGAIN_LATER, ie no more encoded data at this time, but more
                // will be coming later.
                return true;
            }
        }

        // If we got here, we ran out of retries. This almost certainly means that either something
        // is wrong, or the encoder is still outputting data. Both of these cases mean that now
        // would not be a good time to enqueue more data.
        return false;
    }

    private final Runnable notifyListenerOnRecordingStarted = new Runnable() {
        @Override
        public void run() {
            listener.onRecordingStarted();
        }
    };

    private final Runnable notifyListenerOnRecordingError = new Runnable() {
        @Override
        public void run() {
            listener.onRecordingError();
        }
    };

    private final Runnable notifyListenerOnRecordingFinished = new Runnable() {
        @Override
        public void run() {
            listener.onRecordingFinished(outputFile);
        }
    };

    private static class MediaEncoder {
        MediaFormat format;
        MediaCodec codec;
        int track = -1;

        protected void release() {
            if (codec != null) {
                codec.release();
                codec = null;
            }

            if (format != null) {
                format = null;
            }

            track = -1;
        }
    }

    private static class VideoEncoder extends MediaEncoder {
        Surface videoCodecSurface;

        @Override
        protected void release() {
            super.release();

            if (videoCodecSurface != null) {
                videoCodecSurface.release();
                videoCodecSurface = null;
            }
        }
    }

    private static class AudioEncoder extends MediaEncoder implements Runnable {
        AudioRecord audioRecorder;
        Thread audioRecorderThread;
        final BlockingQueue<ByteBuffer> freeBufferQueue = new LinkedBlockingQueue<ByteBuffer>();
        final BlockingQueue<ByteBuffer> filledBufferQueue = new LinkedBlockingQueue<ByteBuffer>();

        public AudioEncoder() {
            for (int i = 0; i < NUM_AUDIO_SAMPLE_BUFFERS; i++) {
                freeBufferQueue.add(ByteBuffer.allocateDirect(AUDIO_SAMPLE_BUFFER_LEN));
            }
        }

        @Override
        protected void release() {
            super.release();

            if (audioRecorder != null) {
                audioRecorder.release();
                audioRecorder = null;
            }
        }

        // N.B. runs in its own thread context. The lifecycle of audioRecorder exceeds that of the
        // thread in which this method runs, so no synchronization is required.
        @Override
        public void run() {
            Log.v(TAG, "Audio recorder thread starting");

            try {
                while (true) {
                    ByteBuffer audioBuffer = freeBufferQueue.take();
                    audioBuffer.clear();
                    int count = audioRecorder.read(audioBuffer, audioBuffer.remaining());
                    audioBuffer.limit(count);
                    filledBufferQueue.put(audioBuffer);
                }
            } catch (InterruptedException e) {
                // InterruptedException is signal to terminate
            }

            Log.v(TAG, "Audio recorder thread terminating");
        }
    }
}
