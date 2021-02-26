/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.panoramaui.hardware;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.concurrent.atomic.AtomicInteger;

import android.content.Context;
import android.content.ContextWrapper;
import android.content.SharedPreferences;
import android.media.ExifInterface;
import android.os.AsyncTask;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.preference.PreferenceManager;
import android.util.Log;
import android.util.Pair;

import com.qualcomm.qti.panoramaui.CameraPreviewView;
import com.qualcomm.qti.panoramaui.CameraPreviewView.OutputFrame;
import com.qualcomm.qti.panoramaui.data.AppError;
import com.qualcomm.qti.panoramaui.data.CaptureDirection;
import com.qualcomm.qti.panoramaui.data.CaptureOrientation;
import com.qualcomm.qti.panoramaui.preferences.CameraPreferenceManager;
import com.qualcomm.qti.panoramaui.utils.IirFilter;
import com.qualcomm.qti.panoramaui.utils.ImageUtils;

/**
 * This class implements a {@link PanoramaEngine} based on the standalone SCVE Panorama libraries
 */
/* package */class StandalonePanoramaEngine implements PanoramaEngine {
    private static final String TAG = "StandalonePanoramaEngine";

    // Chebyshev type-2 low-pass filter, 4th order, cutoff freq = 0.12 * (Fs/2)
    private static final float[] SPEED_FILTER_XC = new float[] { 0.029411f, -0.089418f, 0.123845f,
            -0.089418f, 0.029411f };
    private static final float[] SPEED_FILTER_YC = new float[] { 1.000000f, -3.329970f, 4.206944f,
            -2.383985f, 0.510842f };

    private static final float ESTABLISH_DIRECTION_FRACTION = 0.15f;

    // N.B. These constants must match those defined in SCVE native headers
    private static final int SCVE_COLORFORMAT_YUV_NV21 = 1;
    private static final int SCVE_SUCCESS = 0;

    private enum State {
        IDLE, FIRST_PREVIEW_FRAME_PENDING, CAPTURING, COMPLETING, ERROR
    }

    private final CameraActor cameraActor;
    private final SensorActor sensorActor;
    private final CameraPreviewView cameraPreviewView;
    private final Handler mainHandler;

    // These members may only be accessed from the engine thread
    private State state;
    private int captureGeneration;
    private PanoramaActor.EventListener listener;
    private CaptureOrientation orientation;
    private CaptureDirection direction;
    private int establishDirectionDistance;
    private int speedScaleFactor;
    private IirFilter speedFilter;
    private int outstandingKeyFrameCount;
    private int keyframeRotation = -1;
    private int projectionType = 0;
    private boolean performanceLoggingOn;
    private boolean savePreviewFramesOn;
    private boolean saveKeyFramesDataOn;
    private int previewFrameIndex;
    private int keyFrameIndex;

    // These members do not need to be synchronized - they are written only by the main thread,
    // and are guaranteed to exist for the lifetime of the engine thread
    private HandlerThread engineThread;
    private Handler engineHandler;
    private static List<Pair<Integer, Integer>> debugPreviewFramePosition = new ArrayList<Pair<Integer, Integer>>();
    private static List<Pair<Integer, Integer>> debugKeyFramePosition = new ArrayList<Pair<Integer, Integer>>();
    private static List<Pair<String, String>> debugAppSettings = new ArrayList<Pair<String, String>>();

    static {
        System.loadLibrary("panoramaref_jni");
        nClassInit();
    }

    /**
     * Construct a new {@link StandalonePanoramaEngine}
     *
     * @param cameraActor
     *            a {@link CameraActor} from which to retrieve frame details, and to request ZSL
     *            keyframes be captured
     * @param sensorActor
     *            a {@link SensorActor} from which device motion may be measured
     * @param cameraPreviewView
     *            a {@link CameraPreviewView} from which to receive preview frames
     */
    public StandalonePanoramaEngine(CameraActor cameraActor, SensorActor sensorActor,
            CameraPreviewView cameraPreviewView) {
        if (cameraActor == null) {
            throw new IllegalArgumentException("cameraActor cannot be null");
        } else if (sensorActor == null) {
            throw new IllegalArgumentException("sensorActor cannot be null");
        } else if (cameraPreviewView == null) {
            throw new IllegalArgumentException("cameraPreviewView cannot be null");
        }

        this.cameraActor = cameraActor;
        this.sensorActor = sensorActor;
        this.cameraPreviewView = cameraPreviewView;

        mainHandler = new Handler(Looper.getMainLooper());
    }

    /** {@inheritDoc} */
    @Override
    public void init() {
        if (engineThread != null) {
            throw new RuntimeException("StandalonePanoramaEngine is already initialized");
        }

        engineThread = new HandlerThread(TAG);
        engineThread.start();
        engineHandler = new Handler(engineThread.getLooper());
        engineHandler.post(new DoInitRunnable());
    }

    /** {@inheritDoc} */
    @Override
    public void release() {
        if (engineThread == null) {
            throw new IllegalArgumentException("StandalonePanoramaEngine is not initialized");
        }

        engineHandler.post(doRelease);
        engineThread.quitSafely();
        try {
            engineThread.join();
        } catch (InterruptedException e) {
            Log.w(TAG, "Interrupted while waiting for StandalonePanoramaEngine thread to finish");
            Thread.currentThread().interrupt();
        }
        engineHandler = null;
        engineThread = null;
    }

    /** {@inheritDoc} */
    @Override
    public void startCapture(PanoramaActor.EventListener listener) {
        if (listener == null) {
            throw new IllegalArgumentException("listener cannot be null");
        } else if (engineThread == null) {
            throw new RuntimeException("StandalonePanoramaEngine is not initialized");
        }

        engineHandler.post(new DoStartCaptureRunnable(listener));
    }

    /** {@inheritDoc} */
    @Override
    public void completeCapture() {
        if (engineThread == null) {
            throw new RuntimeException("StandalonePanoramaEngine is not initialized");
        }

        engineHandler.post(doCompleteCapture);
    }

    /** {@inheritDoc} */
    @Override
    public void cancelCapture() {
        if (engineThread == null) {
            throw new RuntimeException("StandalonePanoramaEngine is not initialized");
        }

        engineHandler.post(doCancelCapture);
    }

    // N.B. May only be called in the context of the engine thread
    private void handleError(final AppError error, boolean requestTerminateCapture) {
        state = State.ERROR;

        mainHandler.post(new EventListenerRunnable(listener) {
            @Override
            public void run() {
                localListener.onError(error);
            }
        });

        if (requestTerminateCapture) {
            if (error.isImageRecoverable()) {
                engineHandler.post(doCompleteCapture);
            } else {
                engineHandler.post(doCancelCapture);
            }
        }
    }

    // N.B. May only be called in the context of the engine thread
    private void handleError(AppError error) {
        handleError(error, true);
    }

    // As of SCVE 1.3.0, the speed reported by the tracking engine is relative to the internal
    // scaled frame size. This function calculates an "unscaling" factor, to make the reported
    // speed relative to the input frame size.
    private static int calculateSpeedScaleFactor(int contentWidth) {
        // 0->960, downscaled by x2
        // 961->1920, downscaled by x4
        // 1921->3840, downscaled by x8
        // ...
        if (contentWidth == 0) {
            return 1;
        }

        return (1 << (33 - Integer.numberOfLeadingZeros((contentWidth - 1) / 960)));
    }

    // N.B. Runs on the engine thread
    private class DoInitRunnable implements Runnable {
        @Override
        public void run() {
            Context context = cameraPreviewView.getContext();
            performanceLoggingOn = CameraPreferenceManager.getEnableLogging(context);
            savePreviewFramesOn = CameraPreferenceManager.getSavePreviewFrames(context);
            saveKeyFramesDataOn = CameraPreferenceManager.getSaveKeyFrames(context);
            projectionType = CameraPreferenceManager.getProjectionType(context);

            state = State.IDLE;
        }
    }

    // N.B. Runs on the engine thread
    private final Runnable doRelease = new Runnable() {
        @Override
        public void run() {
        }
    };

    // N.B. Runs on the engine thread
    private class DoStartCaptureRunnable extends EventListenerRunnable {
        public DoStartCaptureRunnable(PanoramaActor.EventListener listener) {
            super(listener);
        }

        @Override
        public void run() {
            if (state != State.IDLE) {
                Log.w(TAG, "StandalonePanoramaEngine is already capturing, ignoring start capture");
                return;
            }

            state = State.FIRST_PREVIEW_FRAME_PENDING;
            captureGeneration++;
            orientation = CaptureOrientation.HORIZONTAL;
            direction = CaptureDirection.NOT_ESTABLISHED;
            speedFilter = new IirFilter(SPEED_FILTER_XC, SPEED_FILTER_YC);
            outstandingKeyFrameCount = 0;
            keyframeRotation = -1;
            if(saveKeyFramesDataOn) {
                debugPreviewFramePosition.clear();
                debugKeyFramePosition.clear();
                debugAppSettings.clear();
            }
            StandalonePanoramaEngine.this.listener = localListener;
            cameraPreviewView.setOnFrameAvailableListener(previewFrameListener);
            previewFrameIndex = 0;
            keyFrameIndex = 0;

            Log.i(TAG, "Panorama capture started");

            mainHandler.post(new EventListenerRunnable(listener) {
                @Override
                public void run() {
                    localListener.onCaptureStarted();
                }
            });
        }
    }

    // N.B. Runs on the engine thread
    private final Runnable doCompleteCapture = new Runnable() {
        @Override
        public void run() {
            if (state == State.IDLE) {
                Log.w(TAG, "StandalonePanoramaEngine is not capturing, ignoring complete capture");
                return;
            } else if (state == State.FIRST_PREVIEW_FRAME_PENDING) {
                Log.i(TAG, "StandalonePanoramaEngine never started, converting to cancelCapture");
                engineHandler.post(doCancelCapture);
                return;
            }

            cameraPreviewView.setOnFrameAvailableListener(null);

            if (outstandingKeyFrameCount > 0) {
                Log.i(TAG, outstandingKeyFrameCount + " key frames still outstanding, deferring "
                        + "complete capture");
                if (state == State.CAPTURING) {
                    state = State.COMPLETING;
                }
                return;
            }

            final ByteBuffer[] panoramaJpegData = new ByteBuffer[1];
            int result = nCompletePanorama(panoramaJpegData);
            if (result != SCVE_SUCCESS) {
                final AppError error;
                switch (result) {
                default:
                    error = AppError.PANORAMA_CAPTURING_FAILED;
                    Log.e(TAG, "Failed capturing panorama, error=" + result);
                    break;
                }

                handleError(error, false);

                if (!error.isImageRecoverable()) {
                    engineHandler.post(doCancelCapture);
                    return;
                }
            }

            // N.B. If we get here, we either succeeded, or the error was recoverable
            nInstanceRelease();

            Log.i(TAG, "Panorama capture completed");

            mainHandler.post(new EventListenerRunnable(listener) {
                @Override
                public void run() {
                    localListener.onCaptureCompleted(panoramaJpegData[0]);
                }
            });

            state = State.IDLE;
            listener = null;
            if( saveKeyFramesDataOn) {
                AsyncTask.execute(new DebugSavePositionsAndSettingsRunnable());
            }
        }
    };

    // N.B. Runs on the engine thread
    private final Runnable doCancelCapture = new Runnable() {
        @Override
        public void run() {
            if (state == State.IDLE) {
                Log.w(TAG, "StandalonePanoramaEngine is not capturing, ignoring cancel capture");
                return;
            }

            cameraPreviewView.setOnFrameAvailableListener(null);

            if (state != State.FIRST_PREVIEW_FRAME_PENDING) {
                nCancelPanorama();
                nInstanceRelease();
            }

            Log.v(TAG, "Panorama capture cancelled");

            mainHandler.post(new EventListenerRunnable(listener) {
                @Override
                public void run() {
                    localListener.onCaptureCancelled();
                }
            });

            state = State.IDLE;
            listener = null;
        }
    };

    // N.B. runs on the engine thread
    private void setVerticalCapture(int positionY) {
        orientation = CaptureOrientation.VERTICAL;
        mainHandler.post(new EventListenerRunnable(listener) {
            @Override
            public void run() {
                localListener.onOrientationEstablished(CaptureOrientation.VERTICAL);
            }
        });

        direction = (positionY < 0 ? CaptureDirection.DOWN : CaptureDirection.UP);
        mainHandler.post(new EventListenerRunnable(listener) {
            @Override
            public void run() {
                localListener.onDirectionEstablished(direction);
            }
        });

        Log.v(TAG, "Found capture direction: " + direction);
    }

    // N.B. runs on the engine thread
    private void setHorizontalCapture(int positionX) {
        Log.v(TAG, "Found horizontal capture");
        orientation = CaptureOrientation.HORIZONTAL;
        mainHandler.post(new EventListenerRunnable(listener) {
            @Override
            public void run() {
                localListener.onOrientationEstablished(CaptureOrientation.HORIZONTAL);
            }
        });

        direction = (positionX < 0 ? CaptureDirection.RIGHT : CaptureDirection.LEFT);
        mainHandler.post(new EventListenerRunnable(listener) {
            @Override
            public void run() {
                localListener.onDirectionEstablished(direction);
            }
        });
        Log.v(TAG, "Found capture direction: " + direction);
    }

    // N.B. runs on the engine thread
    private class DoPreviewFrame implements Runnable {
        private final AtomicInteger previewFrameCount = new AtomicInteger();
        private volatile OutputFrame previewFrame;

        private final boolean[] isKeyFrame = new boolean[1];
        private final int[] position = new int[2];
        private final int[] speed = new int[1];
        private final int[] apiDirection = new int[1];

        @Override
        public void run() {
            final OutputFrame frame = previewFrame;

            if (frame == null) {
                throw new RuntimeException("Expected a preview frame, but none found");
            }

            if (state == State.FIRST_PREVIEW_FRAME_PENDING) {
                establishDirectionDistance = (int) ((orientation == CaptureOrientation.HORIZONTAL ? frame.width
                        : frame.height) * ESTABLISH_DIRECTION_FRACTION);

                speedScaleFactor = calculateSpeedScaleFactor(orientation == CaptureOrientation.HORIZONTAL ? frame.width
                        : frame.height);
                speedFilter.resetWithInput(0f);

                previewFrameCount.set(0);

                Log.d(TAG, "initializing tracker");
                if (nInstanceInit(frame.width, frame.height, frame.stride,
                        SCVE_COLORFORMAT_YUV_NV21, projectionType, cameraActor.getFocalLength())) {
                    state = State.CAPTURING;
                    if(saveKeyFramesDataOn) {
                        debugPreviewFramePosition.add(new Pair(frame.width, frame.height));
                    }
                } else {
                    Log.e(TAG, "Failed initializing pano engine");
                    handleError(AppError.PANORAMA_API_FAILED);
                }
            }

            if (state != State.CAPTURING) {
                frame.sem.release();
                previewFrame = null;
                return;
            }

            int result = nProcessPreviewFrame(frame.buffer, isKeyFrame, position, speed,
                    apiDirection);
            //Log.d(TAG, "api Direction: " + apiDirection[0]);

            if (AppError.shouldTerminateCapture(result)) {
                final AppError error;
                switch (result) {
                default:
                    error = AppError.PANORAMA_API_FAILED;
                    Log.e(TAG, "Failed processing preview frame, error=" + result);
                    break;
                }

                handleError(error);
            } else {
                // Since it may take us several frame times to perform tracking, spread the speed
                // (which is actually a pixel delta since the last tracking event) out over all
                // frames
                final int count = previewFrameCount.getAndSet(0);
                final float distributedSpeed = (float) (speed[0] * speedScaleFactor) / count;
                for (int i = 1; i < count; i++) {
                    speedFilter.add(distributedSpeed);
                }
                final float filteredSpeed = speedFilter.add(distributedSpeed);

                mainHandler.post(new EventListenerRunnable(listener) {
                    @Override
                    public void run() {
                        localListener.onSpeedUpdate(filteredSpeed);
                    }
                });

                if (direction == CaptureDirection.NOT_ESTABLISHED) {
                    if (Math.abs(position[0]) > establishDirectionDistance
                            || Math.abs(position[1]) > establishDirectionDistance) {
                        if (apiDirection[0] == 1) {
                            orientation = CaptureOrientation.VERTICAL;
                            setVerticalCapture(position[1]);

                        } else if (apiDirection[0] == 0) {
                            orientation = CaptureOrientation.HORIZONTAL;
                            setHorizontalCapture(position[0]);

                        }

                    }
                }

                if (isKeyFrame[0]) {
                    processKeyFrame(position[0], position[1], frame);
                    if(saveKeyFramesDataOn) {
                        debugKeyFramePosition.add(new Pair(position[0], position[1]));
                    }
                }
            }
            if (savePreviewFramesOn) {
                AsyncTask.execute(new DebugSavePreviewFrameRunnable(keyFrameIndex,
                        previewFrameIndex, frame));
            } else {
                frame.sem.release();
            }
            if (saveKeyFramesDataOn) {
                debugPreviewFramePosition.add(new Pair(position[0], position[1]));
            }
            previewFrameIndex++;
            previewFrame = null;
        }
    }
    private void processKeyFrame(final int x, final int y, final OutputFrame frame) {
        if(cameraActor.isZslBeingUsed()) {
            DoKeyFrame doKeyFrame = new DoKeyFrame(x, y);
                    outstandingKeyFrameCount++;
                    cameraActor.captureKeyFrame(doKeyFrame);

            mainHandler.post(new EventListenerRunnable(listener) {
                @Override
                public void run() {
                    localListener.onKeyFrame(frame.thumb, x, y);
                }
            });
        } else {
            int result = nProcessKeyFrameNV21(frame.buffer, x, y);
            if (AppError.shouldTerminateCapture(result)) {
                final AppError error;
                switch (result) {
                default:
                    error = AppError.PANORAMA_API_FAILED;
                    Log.e(TAG, "Failed processing preview frame, error=" + result);
                    break;
                }
                handleError(error);
            } else {
                    mainHandler.post(new EventListenerRunnable(listener) {
                        @Override
                        public void run() {
                            localListener.onKeyFrame(frame.thumb, x, y);
                        }
                    });
                }
            }

                // N.B. This Runnable will release the semaphore when it is done with the frame
    }

    private final DoPreviewFrame doPreviewFrame = new DoPreviewFrame();

    // N.B. onPictureTaken runs in an arbitrary thread context, run runs on the engine thread
    private class DoKeyFrame implements Runnable, CameraActor.OnKeyFrameCapturedListener {
        private final Handler localEngineHandler = engineHandler;
        private final int localCaptureGeneration = captureGeneration;
        private final int debugLocalKeyFrameIndex = keyFrameIndex;
        private final int x;
        private final int y;
        private volatile byte[] keyFrameJpegData;

        public DoKeyFrame(int x, int y) {
            if (performanceLoggingOn) {
                Log.v(TAG, "PERF: Issuing request for key frame " + debugLocalKeyFrameIndex
                        + " at " + System.nanoTime());
            }

            this.x = x;
            this.y = y;
        }

        @Override
        public void onKeyFrameCaptured(byte[] jpegData) {
            if (performanceLoggingOn) {
                Log.v(TAG, "PERF: Received ZSL data for key frame " + debugLocalKeyFrameIndex
                        + " at " + System.nanoTime());
            }

            keyFrameJpegData = jpegData;
            if(keyframeRotation == -1) {
                Context context = cameraPreviewView.getContext();
                keyframeRotation = getJpgRotation(context, jpegData, "exif_keyframe.jpg");
                Log.d(TAG, "Fetched keyframe rotation: " + keyframeRotation);
            } else {
                Log.d(TAG, "Cached keyframe rotation: " + keyframeRotation);
            }

            // N.B. engine thread may have been stopped by the time we try and dispatch, but if so,
            // it's OK to just drop the key frame here in that case.
            localEngineHandler.post(this);
        }

        @Override
        public void run() {
            if (state == State.IDLE) {
                Log.w(TAG, "StandalonePanoramaEngine is not capturing, ignoring key frame");
                return;
            } else if (localCaptureGeneration != captureGeneration) {
                Log.i(TAG, "Received a key frame for a prior capture generation, discarding");
                return;
            }

            if (performanceLoggingOn) {
                Log.v(TAG, "PERF: Processing key frame " + debugLocalKeyFrameIndex + " at "
                        + System.nanoTime());
            }

            outstandingKeyFrameCount--;

            if (keyFrameJpegData == null) {
                Log.e(TAG, "Received a key frame with no JPEG data");
                handleError(AppError.PANORAMA_CAPTURING_FAILED);
                return;
            }

            int result = nProcessKeyFrame(keyFrameJpegData, x, y, keyframeRotation);
            if (AppError.shouldTerminateCapture(result)) {
                final AppError error;
                switch (result) {
                default:
                    error = AppError.PANORAMA_API_FAILED;
                    Log.e(TAG, "Failed processing key frame, error=" + result);
                    break;
                }

                handleError(error);
            } else {
                Log.v(TAG, "Key frame processed");

                if ((state == State.COMPLETING || state == State.ERROR)
                        && outstandingKeyFrameCount == 0) {
                    engineHandler.post(doCompleteCapture);
                }
            }

            if (saveKeyFramesDataOn) {
                AsyncTask.execute(new DebugSaveKeyFrameRunnable(keyFrameIndex,
                        keyFrameJpegData));
            }
            keyFrameIndex++;
        }
        private int getExifRotation(String jpgFilePath) throws IOException {
            ExifInterface exif = new ExifInterface(jpgFilePath);
            int jpgRotation = exif.getAttributeInt(ExifInterface.TAG_ORIENTATION,
                    ExifInterface.ORIENTATION_UNDEFINED);
            int angle = 0;
            switch (jpgRotation) {
                case ExifInterface.ORIENTATION_ROTATE_90:
                    angle = 90;
                    break;
                case ExifInterface.ORIENTATION_ROTATE_180:
                    angle = 180;
                    break;
                case ExifInterface.ORIENTATION_ROTATE_270:
                    angle = 270;
                    break;
            }
            return angle;
        }
        private String saveSnap(Context context, byte[] jpgData, String fileName) {
            ContextWrapper contextWrapper = new ContextWrapper(context);
            File location = contextWrapper.getDir("imageDir", Context.MODE_PRIVATE);
            if (!location.exists()) {
                boolean result = location.mkdirs();
                Log.d(TAG, "Create Image directory status: " + result);
            }
            File jpgFile = new File(location, fileName);
            Log.d(TAG, "Saving image " + jpgFile.toString());
            try {
                if (jpgFile.exists()) {
                    jpgFile.delete();
                }
                FileOutputStream outputStream = new FileOutputStream(jpgFile);
                outputStream.write(jpgData);
                outputStream.close();
                return jpgFile.toString();
            } catch (IOException e) {
                Log.d(TAG, "getting exif information fetched: " + e.getMessage());
                e.printStackTrace();
            }
            return null;
        }
        private int getJpgRotation(Context context, byte[] jpgData, String fileName) {
            int rotation = 0;
            try {
                String filePath = saveSnap(context, jpgData, fileName);
                rotation = getExifRotation(filePath);
            } catch(IOException exp) {
                exp.printStackTrace();
            }
            return rotation;
        }
    }

    // N.B. Runs in an arbitrary thread context
    private final CameraPreviewView.OnFrameAvailableListener previewFrameListener = new CameraPreviewView.OnFrameAvailableListener() {
        @Override
        public boolean onFrameAvailable(OutputFrame frame) {
            boolean consumed = false;

            doPreviewFrame.previewFrameCount.incrementAndGet();

            if (doPreviewFrame.previewFrame == null) {
                doPreviewFrame.previewFrame = frame;

                if (engineHandler.post(doPreviewFrame)) {
                    consumed = true;
                } else {
                    doPreviewFrame.previewFrame = null;
                }
            }

            return consumed;
        }
    };

    // N.B. Runs in an arbitrary thread context
    private static class DebugSavePreviewFrameRunnable implements Runnable {
        private final int keyFrameId;
        private final int previewFrameId;
        private OutputFrame frame;

        public DebugSavePreviewFrameRunnable(int keyFrameId, int previewFrameId, OutputFrame frame) {
            this.keyFrameId = keyFrameId;
            this.previewFrameId = previewFrameId;
            this.frame = frame;
        }

        @Override
        public void run() {
            ImageUtils.saveDebugPreviewFrame(keyFrameId, previewFrameId, frame);
            frame.sem.release();
        }
    }

    // N.B. Runs in an arbitrary thread context
    private static class DebugSaveKeyFrameRunnable implements Runnable {
        private final int id;
        private final byte[] jpegData;

        public DebugSaveKeyFrameRunnable(int id, byte[] jpegData) {
            this.id = id;
            this.jpegData = jpegData;
        }

        @Override
        public void run() {
            ImageUtils.saveDebugKeyFrame(id, jpegData);
        }
    }
    private class DebugSavePositionsAndSettingsRunnable implements Runnable {
        @Override
        public void run() {
            ImageUtils.saveDebugPreviewFramePosition(debugPreviewFramePosition);
            ImageUtils.saveDebugKeyFramePosition(debugKeyFramePosition);
            Context context = cameraPreviewView.getContext();
            SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(context);
            Map<String, ?> prefMaps = prefs.getAll();
            for(Map.Entry<String, ?> entry : prefMaps.entrySet()) {
                debugAppSettings.add(new Pair(entry.getKey(), entry.getValue().toString()));
            }
            ImageUtils.saveDebugAppSettings(debugAppSettings);
        }
    }

    private abstract static class EventListenerRunnable implements Runnable {
        final PanoramaActor.EventListener localListener;

        public EventListenerRunnable(PanoramaActor.EventListener listener) {
            localListener = listener;
        }
    }

    // ------------------------------------- NATIVE INTERFACE --------------------------------------

    private long nativeContext; // N.B. Do not modify from Java (only from native code)

    private static native void nClassInit();

    private native boolean nInstanceInit(int width, int height, int stride, int colorFormat,
            int projectionType, float focalLength);

    private native void nInstanceRelease();

    private native int nProcessPreviewFrame(ByteBuffer frame, boolean[] isKeyFrame,
            int[] framePosition, int[] movementSpeed, int[] apiDirection);

    private native int nProcessKeyFrame(byte[] keyFrameJpegData, int x, int y, int exifAngle);
    private native int nProcessKeyFrameNV21(ByteBuffer frame, int x, int y);

    private native int nCancelPanorama();

    private native int nCompletePanorama(ByteBuffer[] panoramaJpegData);
}
