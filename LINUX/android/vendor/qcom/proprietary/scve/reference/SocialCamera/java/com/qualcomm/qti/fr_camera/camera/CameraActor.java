/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.fr_camera.camera;

import android.content.Context;
import android.graphics.ImageFormat;
import android.graphics.RectF;
import android.graphics.SurfaceTexture;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.CaptureFailure;
import android.hardware.camera2.CaptureRequest;
import android.hardware.camera2.CaptureResult;
import android.hardware.camera2.TotalCaptureResult;
import android.hardware.camera2.params.StreamConfigurationMap;
import android.media.Image;
import android.media.ImageNV21;
import android.media.ImageReader;
import android.media.MultiConsumerImage;
import android.media.MultiConsumerImageNV21;
import android.os.Handler;
import android.os.Looper;
import android.os.SystemClock;
import android.util.Log;
import android.util.Size;
import android.view.Surface;

import com.qualcomm.qti.fr_camera.camera.preferences.PreviewSize;
import com.qualcomm.qti.fr_camera.metrics.EventCounter;
import com.qualcomm.qti.fr_camera.metrics.EventFrequencyCounter;
import com.qualcomm.qti.fr_camera.metrics.Metrics;
import com.qualcomm.qti.fr_camera.camera.preferences.CameraPreferenceManager;
import com.qualcomm.qti.fr_camera.coordinate.CoordinateSpace;
import com.qualcomm.qti.fr_camera.coordinate.CoordinateTranslator;
import com.qualcomm.qti.fr_camera.coordinate.Orientation;

import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Objects;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Future;
import java.util.concurrent.TimeUnit;

/**
 * This class wraps the {@link android.hardware.camera2} interface in a friendlier class, which
 * handles error situations and configuring the {@link CameraDevice} appropriately.
 */
public class CameraActor {
    /**
     * Classes may implement this interface to receive notifications of {@link CameraActor} events
     */
    public interface EventListener {
        /**
         * This method will be invoked if a {@link CameraDevice} error is received. After this
         * method is invoked, the {@link CameraActor} will be placed in the closed state.
         * @param error an error code, as defined by {@link CameraDevice.StateCallback}
         */
        void onCameraError(int error);

        /**
         * This method will be invoked if the {@link CameraDevice} is disconnected by the system
         * while the {@link CameraActor} is {@link #open(String)}ed. After this method is invoked,
         * the {@link CameraActor} will be placed in the closed state.
         */
        void onCameraDisconnected();

        /**
         * This method will be invoked when the camera preview is configured (ie at some point after
         * {@link #startPreview()} is invoked, before preview frames begin to be captured). It will
         * only be invoked if a preview {@link SurfaceTexture} has been provided (by
         * {@link #setPreviewSurfaceTexture(SurfaceTexture)}).
         * @param coordinateSpace a {@link CoordinateSpace} describing the preview frames produced
         *                        by the active preview capture session
         * @param surfaceTextureOrientation the {@link Orientation} of the preview frames as they
         *                                  are provided to the {@link SurfaceTexture} set via
         *                                  {@link #setPreviewSurfaceTexture(SurfaceTexture)}.
         */
        void onPreviewConfigured(CoordinateSpace coordinateSpace,
                                 Orientation surfaceTextureOrientation);
    }

    /**
     * Classes may implement this interface to receive notifications of autofocus events
     */
    public interface AutoFocusEventListener {
        /**
         * This method will be invoked when an autofocus operation is started. Exactly when an
         * autofocus operation is started depends on the camera and the current autofocus mode.
         * {@link #setAeAfRegion(RectF)} will often trigger an autofocus. The camera may also
         * trigger an autofocus automatically if the current AF mode is either CONTINUOUS_PICTURE or
         * CONTINUOUS_VIDEO. After this method is invoked, it will be (eventually) followed by an
         * invocation of either {@link #onAutoFocusCancelled()}, {@link #onAutoFocusComplete()}, or
         * {@link #onAutoFocusFailed()}.
         */
        void onAutoFocusStarted();

        /**
         * This method will be invoked when an autofocus operation is cancelled (for eg, if the
         * camera preview is stopped, the AF mode is changed, a new AF operation is started, etc).
         */
        void onAutoFocusCancelled();

        /**
         * This method will be invoked when an autofocus operation completes successfully
         */
        void onAutoFocusComplete();

        /**
         * This method will be invoked when an autofocus operation completes, but the camera was not
         * able to acquire sharp focus
         */
        void onAutoFocusFailed();
    }

    /**
     * Classes may implement this interface to receive notifications of still image capture events
     */
    public interface StillImageCaptureCallback {
        /**
         * This method will be invoked if a still image capture is aborted before it finishes. It
         * may be invoked before or after {@link #onCaptureStarting()}. If this method is invoked,
         * neither {@link #onCaptured(ByteBuffer, boolean)} nor {@link #onCaptureFailed()} will
         * be invoked.
         */
        void onCaptureAborted();

        /**
         * This method will be invoked when a still image capture starts. It will be (eventually) be
         * followed by a call to {@link #onCaptured(ByteBuffer, boolean)}, {@link #onCaptureFailed()},
         * or {@link #onCaptureAborted()}.
         */
        void onCaptureStarting();

        /**
         * This method will be invoked when a still image capture completes successfully. It will be
         * invoked after {@link #onCaptureStarting()}, and if this method is called, neither
         * {@link #onCaptureFailed()} nor {@link #onCaptureAborted()} will be invoked.
         * @param jpeg a {@link ByteBuffer} containing the captured image JPEG data
         * @param isFacingFront
         */
        void onCaptured(ByteBuffer jpeg, boolean isFacingFront);

        /**
         * This method will be invoked when a still image capture fails. It will be invoked after
         * {@link #onCaptureStarting()}, and if this method is called, neither
         * {@link #onCaptured(ByteBuffer, boolean)} nor {@link #onCaptureAborted()} will be invoked.
         */
        void onCaptureFailed();
    }

    /**
     * Classes may implement this interface to receive preview frames encoded as
     * {@link ImageFormat#YUV_420_888} {@link Image}s
     */
    public interface PreviewFrameCaptureCallback {
        /**
         * This method will be invoked each time a new preview frame is available
         * @param image the preview frame encoded as a {@link ImageFormat#YUV_420_888} {@link Image}
         */
        void onCaptured(ImageNV21 image);
    }

    private static final String TAG = "SocialCamera";
    private static final byte JPEG_QUALITY = 80;
    private static final int PREVIEW_MAX_PIXELS = 1920 * 1080;
    private static final int PREVIEW_MAX_PIXELS_HI_RES = 3840 * 2160;
    private static final Metrics metrics = Metrics.getInstance();

    private enum State {
        UNINITIALIZED, OPENING, OPENED, PREVIEW_STARTING, PREVIEW, CLOSING, CLOSED
    }

    private enum PendingAction {
        NONE,
        START_PREVIEW, // On OPENED/PREVIEW entered, will start/restart preview
        STOP_PREVIEW, // On PREVIEW entered, will stop preview
        TAKE_PICTURE, // On focus complete while in PREVIEW state, will start still image capture
        CLOSE // On OPENED entered, will close CameraActor
    }

    // These members are always valid
    private final Context context;
    private final CameraManager cameraManager;
    private SurfaceTexture previewSurfaceTexture;
    private PreviewFrameCaptureCallback previewFrameCaptureCallback;
    private CoordinateSpace previewRequestedCoordinateSpace = CoordinateSpace.EMPTY;
    private EventListener eventListener;
    private AutoFocusEventListener autoFocusEventListener;
    private State state = State.UNINITIALIZED;
    private PendingAction pendingAction = PendingAction.NONE;

    // These fields are valid after OPENING, and until CLOSED
    private CameraCharacteristics cameraCharacteristics;
    private CameraPreferenceManager cameraPreferenceManager;
    private PreviewSize previewSizeMode;
    private final EventFrequencyCounter previewFpsMetric = new EventFrequencyCounter("FPS(Preview)",
            EventCounter.DEFAULT_POLL_PERIOD, EventCounter.DEFAULT_POLL_PERIOD_TIME_UNIT,
            1, TimeUnit.SECONDS, 334, TimeUnit.MILLISECONDS, 20);
    private final EventFrequencyCounter bufferFpsMetric = new EventFrequencyCounter("FPS(Buffer)",
            EventCounter.DEFAULT_POLL_PERIOD, EventCounter.DEFAULT_POLL_PERIOD_TIME_UNIT,
            1, TimeUnit.SECONDS, 334, TimeUnit.MILLISECONDS, 20);
    private boolean debugSavePreviewFrames;
    private boolean debugSavePictures;
    private CameraDebugHelper debugHelper;

    // These fields are valid after OPENED, and until CLOSED
    private CameraDevice camera;
    private final ArrayList<StillImageCaptureCallback> pendingStillImageCaptureRequests =
            new ArrayList<>();
    private final HashMap<Long, PictureCaptureCallback> issuedStillImageCaptureRequests =
            new HashMap<>();

    // These fields are valid after PREVIEW_STARTING, and until CLOSED
    private boolean isHighResPreview;
    private CoordinateSpace previewFrameCoordinateSpace;
    private CoordinateSpace previewOutputCoordinateSpace;
    private CoordinateTranslator previewOutputToActiveArrayTranslator;
    private Surface previewSurface;
    private PreviewFrameProcessor previewFrameProcessor;
    private PreviewFrameCaptureCallback activePreviewFrameCaptureCallback;
    private ImageReader pictureReader;

    // These fields are valid after PREVIEW, and until CLOSING
    private CameraCaptureSession session;
    private AeAfController aeAfController;
    private CaptureRequest.Builder previewRequestBuilder;
    private CaptureRequest.Builder pictureRequestBuilder;

    /**
     * Construct a new {@link CameraActor}
     * @param context the current {@link Context}
     * @throws NullPointerException if context is null
     */
    public CameraActor(Context context) {
        Objects.requireNonNull(context, "context may not be null");
        this.context = context;
        cameraManager = (CameraManager)context.getSystemService(Context.CAMERA_SERVICE);
    }

    /**
     * Set the {@link SurfaceTexture} to render preview frames to.
     * <br/><i>Note: this must be invoked before {@link #startPreview()} to affect the preview
     * output.</i>
     * @param st the {@link SurfaceTexture} onto which to render preview frames, or null to skip
     *           rendering of preview frames
     */
    public void setPreviewSurfaceTexture(SurfaceTexture st) {
        this.previewSurfaceTexture = st;
    }

    /**
     * Set the {@link ImageReader.OnImageAvailableListener} to received preview frames (encoded as
     * {@link ImageFormat#YUV_420_888} {@link Image}s).
     * <br/><i>Note: this must be invoked before {@link #startPreview()} to affect the preview
     * output.</i>
     * @param callback a {@link PreviewFrameCaptureCallback} to which to send preview frames, or
     *                 null to clear the {@link PreviewFrameCaptureCallback}
     */
    public void setPreviewCaptureCallback(PreviewFrameCaptureCallback callback) {
        previewFrameCaptureCallback = callback;
    }

    /**
     * Set the target preview coordinate space. The {@link CameraActor} will select the most
     * appropriate preview size based on this request.
     * <br/><i>Note: this must be invoked before {@link #startPreview()} to affect the preview
     * output.</i>
     */
    public void setRequestedPreviewCoordinateSpace(CoordinateSpace coordinateSpace) {
        Objects.requireNonNull(coordinateSpace, "coordinateSpace may not be null");
        this.previewRequestedCoordinateSpace = coordinateSpace;
    }

    /**
     * Set the {@link EventListener} which will be notified of {@link CameraActor} events
     * @param listener an {@link EventListener} to notify, or null to clear the active
     *                 {@link EventListener}
     */
    public void setEventListener(EventListener listener) {
        eventListener = listener;
    }

    /**
     * Set the {@link AutoFocusEventListener} which will be notified of autofocus events
     * @param listener an {@link AutoFocusEventListener} to notify, or null to clear the active
     *                 {@link AutoFocusEventListener}
     */
    public void setAutoFocusEventListener(AutoFocusEventListener listener) {
        autoFocusEventListener = listener;
    }

    /**
     * Open this {@link CameraActor} (and underlying {@link CameraDevice}). This must be invoked
     * before {@link #startPreview()}, {@link #stopPreview()}, or {@link #close()} may be invoked,
     * and may only be invoked once. To change the {@link CameraDevice} owned by the
     * {@link CameraActor}, a new {@link CameraActor} must be created.
     * @throws NullPointerException if cameraId is null
     * @throws IllegalArgumentException if cameraId is an empty String
     * @throws IllegalStateException if open has previously been invoked
     */
    public void open(String cameraId) {
        Objects.requireNonNull(cameraId, "cameraId may not be null");
        if (cameraId.isEmpty()) {
            throw new IllegalArgumentException("cameraId may not be empty");
        }

        if (state != State.UNINITIALIZED) {
            throw new IllegalStateException("Already opened (state: " + state.name() + ")");
        }

        Log.i(TAG, "Opening");

        try {
            //noinspection ResourceType
            cameraManager.openCamera(cameraId, cameraDeviceStateCallback, null);
            cameraCharacteristics = cameraManager.getCameraCharacteristics(cameraId);
        } catch (CameraAccessException e) {
            Log.e(TAG, "Camera device is not currently available", e);
            // N.B. don't handle exception here; it will be handled in the CameraDevice's
            // StateCallback
            return;
        }

        cameraPreferenceManager = new CameraPreferenceManager(context, cameraId);
        cameraPreferenceManager.setEventListener(cameraPreferenceEventListener);
        previewSizeMode = cameraPreferenceManager.getPreviewSize();

        metrics.addMetric(previewFpsMetric);
        metrics.addMetric(bufferFpsMetric);
        debugSavePictures = cameraPreferenceManager.getDebugSavePictures();
        debugSavePreviewFrames = cameraPreferenceManager.getDebugSavePreviewFrames();
        if (debugSavePreviewFrames || debugSavePictures) {
            debugHelper = new CameraDebugHelper(context,
                    cameraPreferenceManager.getDebugAllowSkipPreviewFrames());
        }

        pendingAction = PendingAction.NONE;
        enterState(State.OPENING);
    }

    /**
     * Close this {@link CameraActor} (and underlying {@link CameraDevice}). This may be invoked at
     * any time after {@link #open(String)} has been invoked. After invoking this method, no further
     * invocations of {@link #startPreview()} or {@link #stopPreview()} should be made.
     * @throws IllegalStateException if {@link #open(String)} has not been invoked
     */
    public void close() {
        switch (state) {
            case UNINITIALIZED:
                throw new IllegalStateException("Close may not be called before open");

            case OPENING:
                if (pendingAction == PendingAction.CLOSE) {
                    Log.v(TAG, "Close request already pending; ignoring");
                } else {
                    Log.i(TAG, "Close requested while opening; deferring until opened");
                    pendingAction = PendingAction.CLOSE; // Override any other pending action
                }
                break;

            case OPENED:
            case PREVIEW_STARTING:
            case PREVIEW:
                Log.i(TAG, "Closing (from state [" + state.name() + "])");
                pendingAction = PendingAction.NONE;
                closeInternal();
                break;

            case CLOSING:
            case CLOSED:
                Log.v(TAG, "Close requested while already closing or closed; ignoring");
                break;

            default:
                throw new IllegalStateException("Unexpected state: " + state.name());
        }
    }

    /**
     * Start capturing preview frames. This may be invoked after {@link #open(String)} has been
     * invoked. If preview is already running, it will be ignored.
     * @throws IllegalStateException if {@link #open(String)} has not been invoked
     */
    public void startPreview() {
        switch (state) {
            case UNINITIALIZED:
                throw new IllegalStateException("startPreview may not be called before open");

            case OPENING:
                if (pendingAction != PendingAction.CLOSE) {
                    Log.i(TAG, "StartPreview request while opening; deferring until opened");
                    pendingAction = PendingAction.START_PREVIEW;
                } else {
                    Log.v(TAG, "StartPreview requested while close pending; ignoring");
                }
                break;

            case OPENED:
                Log.i(TAG, "Starting preview");
                startPreviewInternal(false);
                break;

            case PREVIEW_STARTING:
                if (pendingAction == PendingAction.STOP_PREVIEW) {
                    Log.i(TAG, "StartPreview request while preview is starting and a stopPreview " +
                            "request is pending; preview will be restarted");
                    pendingAction = PendingAction.START_PREVIEW; // treated as a restart request
                    break;
                }
                // FALL-THROUGH

            case PREVIEW:
                Log.v(TAG, "StartPreview requested while already running; ignoring");
                break;

            case CLOSING:
            case CLOSED:
                Log.w(TAG, "startPreview requested while closing or closed; ignoring");
                break;

            default:
                throw new IllegalStateException("Unexpected state: " + state.name());
        }
    }

    /**
     * Stop capturing preview frames. This may be invoked after {@link #open(String)} has been
     * invoked. If preview is not running, it will be ignored.
     * @throws IllegalStateException if {@link #open(String)} has not been invoked
     */
    public void stopPreview() {
        switch (state) {
            case UNINITIALIZED:
                throw new IllegalStateException("stopPreview may not be called before open");

            case OPENING:
                if (pendingAction == PendingAction.START_PREVIEW) {
                    Log.i(TAG, "StopPreview request while opening and a startPreview request is " +
                            "pending; preview will not be started");
                    pendingAction = PendingAction.NONE;
                    break;
                }
                // FALL-THROUGH

            case OPENED:
                Log.v(TAG, "StopPreview requested while preview not running; ignoring");
                break;

            case PREVIEW_STARTING:
                Log.i(TAG, "StopPreview request while preview is starting; deferring until " +
                        "preview is started");
                pendingAction = PendingAction.STOP_PREVIEW;
                break;

            case PREVIEW:
                Log.i(TAG, "Stopping preview");
                stopPreviewInternal();
                break;

            case CLOSING:
            case CLOSED:
                Log.w(TAG, "stopPreview requested while closing or closed; ignoring");
                break;

            default:
                throw new IllegalStateException("Unexpected state: " + state.name());
        }
    }

    /**
     * Set the AE/AF region of interest. This may be a single point (specified by an empty rect), or
     * a region. This should only be invoked while preview is running.
     * @param aeAfRegion a {@link RectF} containing the point or area of interest to use for AE/AF
     *                   calculations, or null to clear the set AE/AF region. This region should be
     *                   specified in terms of the output {@link CoordinateSpace} (as reported via
     *                   {@link EventListener#onPreviewConfigured(CoordinateSpace, Orientation)}).
     * @throws IllegalStateException if {@link #open(String)} has not been invoked
     */
    public void setAeAfRegion(RectF aeAfRegion) {
        switch (state) {
            case UNINITIALIZED:
                throw new IllegalStateException("setAeAfRegion may not be called before open");

            case PREVIEW:
                if (aeAfRegion == null) {
                    Log.i(TAG, "Clearing AE/AF region");
                    aeAfController.setAeAfRegion(null);
                } else {
                    RectF activeArrayAeAfRegion = new RectF(aeAfRegion);
                    previewOutputToActiveArrayTranslator.mapRect(activeArrayAeAfRegion,
                            activeArrayAeAfRegion);
                    Log.i(TAG, "Setting AE/AF region to preview:[" + aeAfRegion.toShortString() +
                            "] (activeArray:[" + activeArrayAeAfRegion.toShortString() + "])");
                    aeAfController.setAeAfRegion(activeArrayAeAfRegion);
                }
                break;

            case OPENING:
            case OPENED:
            case PREVIEW_STARTING:
            case CLOSING:
            case CLOSED:
                Log.w(TAG, "setAeAfRegion while preview is not running; ignoring");
                break;

            default:
                throw new IllegalStateException("Unexpected state: " + state.name());
        }
    }

    /**
     * Initiate a still image capture. This should only be invoked while preview is running.
     * @param callback a {@link StillImageCaptureCallback} to notify of capture events
     * @throws NullPointerException if callback is null
     * @throws IllegalStateException if {@link #open(String)} has not been invoked
     */
    public void takePicture(StillImageCaptureCallback callback) {
        Objects.requireNonNull(callback, "callback may not be null");

        switch (state) {
            case UNINITIALIZED:
                throw new IllegalStateException("takePicture may not be called before open");

            case PREVIEW:
                if (aeAfController.deferStillImageCaptureForFocus()) {
                    Log.i(TAG, "takePicture requested while autofocus required or in progress; " +
                            "defer until autofocus completion");
                    // N.B. deferStillImageCaptureForFocus already requested AF be run, if necessary
                    pendingAction = PendingAction.TAKE_PICTURE;
                    pendingStillImageCaptureRequests.add(callback);
                } else {
                    Log.i(TAG, "Capturing still image");
                    takePictureInternal(callback);
                }
                break;

            case OPENING:
            case OPENED:
            case PREVIEW_STARTING:
            case CLOSING:
            case CLOSED:
                Log.w(TAG, "takePicture while preview is not running; ignoring");
                break;

            default:
                throw new IllegalStateException("Unexpected state: " + state.name());
        }
    }

    private void closeInternal() {
        releaseCameraResources();
        metrics.removeMetric(previewFpsMetric);
        metrics.removeMetric(bufferFpsMetric);
        enterState(State.CLOSING);
    }

    public Size getPreviewResolution() {
        if (previewFrameCoordinateSpace == null) {
            return null;
        }
        return new Size(previewFrameCoordinateSpace.width, previewFrameCoordinateSpace.height);
    }

    public Size getPictureResolution() {
        if (cameraPreferenceManager == null) {
            return null;
        }
        return cameraPreferenceManager.getPictureSize();
    }

    public boolean isPortrait() {
        if (previewOutputCoordinateSpace.orientation == Orientation.O_0 ||
                previewOutputCoordinateSpace.orientation == Orientation.O_180 )
            return false;

        return true;
    }

    private void startPreviewInternal(boolean configureFailed) {
        isHighResPreview = (!configureFailed && previewSizeMode == PreviewSize.HIGH_RESOLUTION);

        try {
            final Size pictureSize = cameraPreferenceManager.getPictureSize();
            ArrayList<Surface> surfaces = new ArrayList<>();

            if (previewSurfaceTexture != null || previewFrameCaptureCallback != null ||
                    debugSavePreviewFrames) {
                resolvePreviewSize((float)pictureSize.getWidth() / (float)pictureSize.getHeight());

                Log.v(TAG, "Selected preview size: " + previewFrameCoordinateSpace.width + "x" +
                        previewFrameCoordinateSpace.height);
                if (eventListener != null) {
                    eventListener.onPreviewConfigured(previewOutputCoordinateSpace,
                            Orientation.O_0);
                }

                previewFrameProcessor = new PreviewFrameProcessor(
                        previewOutputCoordinateSpace.orientation);
                CoordinateSpace previewTextureCoordinateSpace = previewFrameCoordinateSpace
                        .createForOrientation(Orientation.O_0);
                previewFrameProcessor.setInputSurfaceTextureSize(
                        previewTextureCoordinateSpace.width,
                        previewTextureCoordinateSpace.height);

                if (previewSurfaceTexture != null) {
                    previewFrameProcessor.setOutputSurfaceTexture(previewSurfaceTexture);
                }

                if (previewFrameCaptureCallback != null || debugSavePreviewFrames) {
                    activePreviewFrameCaptureCallback = previewFrameCaptureCallback;
                    previewFrameProcessor.setCallback(previewFrameProcessorCallback,
                            new Handler(Looper.getMainLooper()));
                }

                previewFrameProcessor.start();

                SurfaceTexture st = previewFrameProcessor.getInputSurfaceTexture();
                st.setDefaultBufferSize(previewFrameCoordinateSpace.width,
                        previewFrameCoordinateSpace.height);

                Surface previewSurface = new Surface(st);
                surfaces.add(previewSurface);

                previewRequestBuilder = camera.createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW);
                previewRequestBuilder.addTarget(previewSurface);
            }

            pictureReader = ImageReader.newInstance(pictureSize.getWidth(), pictureSize.getHeight(),
                    ImageFormat.JPEG, 1);
            pictureReader.setOnImageAvailableListener(pictureAvailableListener, null);
            surfaces.add(pictureReader.getSurface());

            pictureRequestBuilder = camera.createCaptureRequest(
                    CameraDevice.TEMPLATE_STILL_CAPTURE);
            pictureRequestBuilder.addTarget(pictureReader.getSurface());

            camera.createCaptureSession(surfaces, cameraCaptureSessionStateCallback, null);
        } catch (CameraAccessException e) {
            Log.e(TAG, "Error while starting preview", e);
            // N.B. don't handle exception here; it will be handled in the CameraDevice's
            // StateCallback
            return;
        }

        enterState(State.PREVIEW_STARTING);
    }

    private void stopPreviewInternal() {
        try {
            session.abortCaptures(); // N.B. Don't close; see javadoc for rationale
        } catch (CameraAccessException e) {
            Log.e(TAG, "Error while stopping preview", e);
            // N.B. don't handle exception here; it will be handled in the CameraDevice's
            // StateCallback
            return;
        }

        releaseCaptureResources();

        enterState(State.OPENED);
    }

    private void takePictureInternal(StillImageCaptureCallback callback) {
        try {
            session.capture(pictureRequestBuilder.build(), new PictureCaptureCallback(
                            cameraCharacteristics, callback), null);
        } catch (CameraAccessException e) {
            Log.e(TAG, "Error while issuing picture capture request", e);
            // N.B. don't handle exception here; it will be handled in the CameraDevice's
            // StateCallback
            //noinspection UnnecessaryReturnStatement
            return;
        }
    }

    private void configureAndIssuePreviewCaptureRequest() {
        if (previewRequestBuilder != null) {
            cameraPreferenceManager.applyCameraPreferences(previewRequestBuilder, true);
            aeAfController.configurePreviewCaptureRequest(previewRequestBuilder);

            try {
                int seqId = session.setRepeatingRequest(previewRequestBuilder.build(),
                        previewCaptureCallback, null);
                aeAfController.setPreviewCaptureRequestSequenceId(seqId);
            } catch(CameraAccessException e){
                Log.e(TAG, "Error while issuing preview capture request", e);
                // N.B. don't handle exception here; it will be handled in the CameraDevice's
                // StateCallback
                return;
            }
        }

        configurePictureCaptureRequest();
    }

    private void configurePictureCaptureRequest() {
        cameraPreferenceManager.applyCameraPreferences(pictureRequestBuilder, false);
        aeAfController.configurePictureCaptureRequest(pictureRequestBuilder);

        Log.v(TAG, "Setting [" + CaptureRequest.JPEG_QUALITY.getName() + "] to [" +
                JPEG_QUALITY + "]");
        pictureRequestBuilder.set(CaptureRequest.JPEG_QUALITY, JPEG_QUALITY);

        final Integer lensFacing = cameraCharacteristics.get(CameraCharacteristics.LENS_FACING);
        assert lensFacing != null;

        final Orientation jpegOrientation = previewFrameCoordinateSpace.orientation.subtract(
                lensFacing != CameraCharacteristics.LENS_FACING_FRONT ?
                        previewOutputCoordinateSpace.orientation :
                        previewOutputCoordinateSpace.orientation.reverse());

        Log.v(TAG, "Setting [" + CaptureRequest.JPEG_ORIENTATION.getName() + "] to [" +
                jpegOrientation.degrees + "]");
        pictureRequestBuilder.set(CaptureRequest.JPEG_ORIENTATION, jpegOrientation.degrees);
    }

    private void enterState(State state) {
        this.state = state;

        switch (pendingAction) {
            case NONE:
                break;

            case START_PREVIEW:
                if (state == State.OPENED) {
                    pendingAction = PendingAction.NONE;
                    Log.v(TAG, "Issuing deferred startPreview request");
                    startPreviewInternal(false);
                } else if (state == State.PREVIEW) {
                    // N.B. Do not clear pendingAction; it is needed when OPENED is reached
                    Log.v(TAG, "Stopping preview for deferred restart request");
                    stopPreviewInternal();
                }
                break;

            case STOP_PREVIEW:
                if (state == State.PREVIEW) {
                    pendingAction = PendingAction.NONE;
                    Log.v(TAG, "Issuing deferred stopPreview request");
                    stopPreviewInternal();
                }
                break;

            case TAKE_PICTURE:
                Log.v(TAG, "takePicture request pending when changing camera states; discarding " +
                        "pending takePicture requests");
                pendingAction = PendingAction.NONE;
                for (StillImageCaptureCallback c : pendingStillImageCaptureRequests) {
                    c.onCaptureAborted();
                }
                pendingStillImageCaptureRequests.clear();
                break;

            case CLOSE:
                if (state != State.OPENED) {
                    throw new IllegalStateException("Close pending while not in OPENED state");
                }

                pendingAction = PendingAction.NONE;
                Log.i(TAG, "Issuing deferred close request");
                closeInternal();
                break;

            default:
                throw new IllegalStateException("Unknown pending action: " + pendingAction.name());
        }
    }

    private void releaseCaptureResources() {
        if (aeAfController != null) {
            aeAfController.close();
            aeAfController = null;
        }
        // N.B. Don't close session here; leaving it open allows the camera to potentially reuse it.
        // It will be closed in releaseCameraResources()
        previewRequestBuilder = null;
        if (previewSurface != null) {
            previewSurface.release();
            previewSurface = null;
        }
        if (previewFrameProcessor != null) {
            previewFrameProcessor.close();
            previewFrameProcessor = null;
        }
        activePreviewFrameCaptureCallback = null;
        pictureRequestBuilder = null;
        if (pictureReader != null) {
            pictureReader.close();
            pictureReader = null;
        }
        previewFrameCoordinateSpace = null;
        previewOutputCoordinateSpace = null;
        previewOutputToActiveArrayTranslator = null;
    }

    private void releaseCameraResources() {
        releaseCaptureResources();

        if (session != null) {
            session.close();
            session = null;
        }

        for (StillImageCaptureCallback c : pendingStillImageCaptureRequests) {
            c.onCaptureAborted();
        }
        pendingStillImageCaptureRequests.clear();
        // N.B. no need to also abort items on issuedStillImageCaptureRequests list here; closing
        // the session will automatically cancel any outstanding picture capture requests, and
        // PictureCaptureCallback will call onCaptureAborted
        issuedStillImageCaptureRequests.clear();

        cameraPreferenceManager = null;
        cameraCharacteristics = null;
        camera.close();
        // N.B. Don't set camera to null here; we want to keep the garbage collector from clearing
        // it until we have received confirmation of close.
    }

    private void resolvePreviewSize(float pictureAspectRatio) {
        final Integer sensorOrientation = cameraCharacteristics.get(
                CameraCharacteristics.SENSOR_ORIENTATION);
        assert sensorOrientation != null;

        // For standard-res preview, try and match the preview size to the requested preview
        // dimensions (after adjusting for aspect ratio). For high-res preview, first try the
        // preview size closest to PREVIEW_MAX_PIXELS_HI_RES (while respecting aspect ratio). This
        // is not guaranteed to be a valid stream configuration on all devices. In that case,
        // configuring the capture session will fail, and startPreviewInternal will be invoked
        // again, with isHighResPreview set to false.
        final int targetArea;
        final int maxArea;
        if (isHighResPreview) {
            // Ignore requested preview size, get as close to PREVIEW_MAX_PIXELS_HI_RES as possible
            // for the picture aspect ratio
            targetArea = PREVIEW_MAX_PIXELS_HI_RES;
            maxArea = PREVIEW_MAX_PIXELS_HI_RES;
        } else {
            // Clip requested preview size to match the picture aspect ratio
            CoordinateSpace rotatedPreviewSpace = previewRequestedCoordinateSpace
                    .createForOrientation(Orientation.fromDegrees(sensorOrientation));
            if (pictureAspectRatio >= rotatedPreviewSpace.aspect) {
                targetArea = (int)((rotatedPreviewSpace.width * rotatedPreviewSpace.width) /
                        pictureAspectRatio);
            } else {
                targetArea = (int)((rotatedPreviewSpace.height * rotatedPreviewSpace.height) *
                        pictureAspectRatio);
            }
            maxArea = PREVIEW_MAX_PIXELS;
        }

        final StreamConfigurationMap scm = cameraCharacteristics.get(
                CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP);
        assert scm != null;
        final Size[] sizes = scm.getOutputSizes(SurfaceTexture.class);

        // Select the best available preview size, progressively loosening the tolerances
        Size previewSizeSelected = selectPreviewSize(sizes, pictureAspectRatio, 0.05f, targetArea,
                0.5f, maxArea);
        if (previewSizeSelected == null) {
            previewSizeSelected = selectPreviewSize(sizes, pictureAspectRatio, 0.1f, targetArea,
                    0.5f, maxArea);
        }
        if (previewSizeSelected == null) {
            previewSizeSelected = selectPreviewSize(sizes, pictureAspectRatio, 0.15f, targetArea,
                    0.5f, maxArea);
        }
        if (previewSizeSelected == null) {
            previewSizeSelected = selectPreviewSize(sizes, pictureAspectRatio, 0.05f, targetArea,
                    0.66f, maxArea);
        }
        if (previewSizeSelected == null) {
            previewSizeSelected = selectPreviewSize(sizes, pictureAspectRatio, 0.10f, targetArea,
                    0.66f, maxArea);
        }
        if (previewSizeSelected == null) {
            previewSizeSelected = selectPreviewSize(sizes, pictureAspectRatio, 0.15f, targetArea,
                    0.66f, maxArea);
        }
        if (previewSizeSelected == null) {
            previewSizeSelected = selectPreviewSize(sizes, pictureAspectRatio, 0.25f, targetArea,
                    0.8f, maxArea);
        }
        if (previewSizeSelected == null) {
            previewSizeSelected = selectPreviewSize(sizes, pictureAspectRatio, 0.5f, targetArea,
                    0.8f, maxArea);
        }
        if (previewSizeSelected == null) {
            previewSizeSelected = sizes[0];
        }

        previewFrameCoordinateSpace = CoordinateSpace.forCameraFrame(previewSizeSelected,
                cameraCharacteristics);
        previewOutputCoordinateSpace = previewFrameCoordinateSpace.createForOrientation(
                previewRequestedCoordinateSpace.orientation);
        previewOutputToActiveArrayTranslator = new CoordinateTranslator(
                previewOutputCoordinateSpace,
                CoordinateSpace.forCameraActiveArray(cameraCharacteristics));
    }

    private static Size selectPreviewSize(Size[] sizes, float targetAspectRatio,
                                          float aspectRatioTolerance, int targetArea,
                                          float areaTolerance, int areaMax) {
        final float minAspectRatio = targetAspectRatio * (1f - aspectRatioTolerance);
        final float maxAspectRatio = targetAspectRatio * (1f + aspectRatioTolerance);
        final int minArea = Math.min((int)(targetArea * (1f - areaTolerance)), areaMax);
        final int maxArea = Math.min((int)(targetArea * (1f + areaTolerance)), areaMax);

        Size candidate = null;
        int candidateAreaDelta = Integer.MAX_VALUE;

        for (Size size : sizes) {
            final int width = size.getWidth();
            final int height = size.getHeight();

            float aspectRatio = (float)width / (float)height;
            if (aspectRatio < minAspectRatio || aspectRatio > maxAspectRatio) {
                continue;
            }

            int area = width * height;
            if (area < minArea || area > maxArea) {
                continue;
            }

            final int areaDelta = Math.abs(area - targetArea);
            if (areaDelta < candidateAreaDelta) {
                candidate = size;
                candidateAreaDelta = areaDelta;
            }
        }

        return candidate;
    }

    private void handleOpened(CameraDevice camera) {
        if (state == State.CLOSING || state == State.CLOSED) {
            Log.w(TAG, "Request to handle opened while closing or closed, ignoring");
            return;
        }

        Log.v(TAG, "CameraDevice [" + camera.getId() + "] opened");
        this.camera = camera;
        enterState(State.OPENED);
    }

    private void handleClosed() {
        Log.v(TAG, "CameraDevice [" + camera.getId() + "] closed");
        camera = null;
        pendingAction = PendingAction.NONE;
        enterState(State.CLOSED);
    }

    // N.B. requires camera, since no guarantee handleOpened() was ever invoked
    private void handleDisconnected(CameraDevice camera) {
        if (state == State.CLOSING || state == State.CLOSED) {
            Log.w(TAG, "Request to handle disconnected while closing or closed, ignoring");
            return;
        }

        Log.v(TAG, "CameraDevice [" + camera.getId() + "] disconnected");

        this.camera = camera;
        closeInternal();

        if (eventListener != null) {
            eventListener.onCameraDisconnected();
        }
    }

    // N.B. requires camera, since no guarantee handleOpened() was ever invoked
    private void handleError(CameraDevice camera, int error) {
        if (state == State.CLOSING || state == State.CLOSED) {
            Log.w(TAG, "Request to handle error while closing or closed, ignoring");
            return;
        }

        Log.e(TAG, "CameraDevice [" + camera.getId() + "] error: " + error);

        this.camera = camera;
        closeInternal();

        if (eventListener != null) {
            eventListener.onCameraError(error);
        }
    }

    private void handleCameraCaptureSessionConfigured(CameraCaptureSession session) {
        if (state == State.CLOSING || state == State.CLOSED) {
            Log.w(TAG, "Request to handle camera capture session configured while closing or " +
                    "closed, ignoring");
            return;
        }

        Log.v(TAG, "Camera capture session configured");
        this.session = session;
        aeAfController = new AeAfController(cameraCharacteristics, aeAfEventListener);
        enterState(State.PREVIEW);

        // As long as enterState() left us in the preview active states, issue the preview capture
        // request. This is an optimization to prevent issuing a request that would just be aborted
        // by a state transition.
        if (state == State.PREVIEW) {
            configureAndIssuePreviewCaptureRequest();
        }
    }

    private void handleCameraCaptureSessionHiResConfigureFailed() {
        if (state != State.PREVIEW_STARTING) {
            Log.w(TAG, "Request to handle hi-res camera capture session configuration failed " +
                    "while preview not starting; ignoring");
            return;
        }

        Log.i(TAG, "Configuring camera capture session for hi-res preview capture failed; " +
                "falling back to standard resolution preview capture");
        releaseCaptureResources();
        startPreviewInternal(true);
    }

    private void handlePreviewCaptureResult(TotalCaptureResult previewResult) {
        if (state != State.PREVIEW) {
            Log.v(TAG, "Received preview capture result when preview not running; ignoring");
            return;
        }

        aeAfController.processPreviewCaptureResult(previewResult);
    }

    private void handleCaptureSessionPreferenceChanged() {
        switch (state) {
            case PREVIEW_STARTING:
                Log.v(TAG, "Capture session config changed while preview is starting; preview " +
                        "will be restarted");
                pendingAction = PendingAction.START_PREVIEW;
                break;

            case PREVIEW:
                Log.v(TAG, "Capture session config changed while preview is running; restarting " +
                        "preview");
                pendingAction = PendingAction.START_PREVIEW;
                stopPreviewInternal();
                break;


            case CLOSING:
            case CLOSED:
            case UNINITIALIZED:
            case OPENING:
            case OPENED:
                Log.v(TAG, "Received capture session preference changed when preview not " +
                        "starting or running; ignoring");
                break;

            default:
                throw new IllegalStateException("Unknown state: " + state.name());
        }
    }

    private void handlePictureCaptureRequestPreferenceChanged() {
        if (state != State.PREVIEW) {
            Log.v(TAG, "Received picture capture request preference changed when preview not " +
                    "running; ignoring");
            return;
        }

        Log.v(TAG, "Received picture capture request preference changed; reconfiguring picture " +
                "capture request");
        configurePictureCaptureRequest();
    }

    private void handleCaptureRequestPreferenceChanged() {
        if (state != State.PREVIEW) {
            Log.v(TAG, "Received capture request preference changed when preview not running; " +
                    "ignoring");
            return;
        }

        Log.v(TAG, "Received capture request preference changed; reconfiguring capture request");
        configureAndIssuePreviewCaptureRequest();
    }

    private void handleAeAfControllerRequestsReconfigureCaptureRequest() {
        if (state != State.PREVIEW) {
            Log.v(TAG, "Received AE/AF reconfigure capture request when preview not running; " +
                    "ignoring");
            return;
        }

        Log.v(TAG, "Received AE/AF reconfigure capture request");
        configureAndIssuePreviewCaptureRequest();
    }

    private void handleAutoFocusComplete() {
        if (state != State.PREVIEW) {
            Log.v(TAG, "Received AF complete when preview not running; ignoring");
            return;
        }

        if (pendingAction == PendingAction.TAKE_PICTURE) {
            pendingAction = PendingAction.NONE;
            for (StillImageCaptureCallback c : pendingStillImageCaptureRequests) {
                takePictureInternal(c);
            }
            pendingStillImageCaptureRequests.clear();
        }
    }

    private void handlePreviewFrameImage(ImageNV21 im) {
        if (state != State.PREVIEW) {
            Log.v(TAG, "Received preview frame image when preview not running; ignoring");
            return;
        }

        if (debugSavePreviewFrames) {
            MultiConsumerImageNV21 mcim = new MultiConsumerImageNV21(im);
            im = mcim.duplicate();
            debugHelper.saveImageAsync(mcim, true);
        }

        if (activePreviewFrameCaptureCallback != null) {
            activePreviewFrameCaptureCallback.onCaptured(im);
        } else {
            im.close();
        }
    }

    @SuppressWarnings("NullableProblems")
    private final CameraDevice.StateCallback cameraDeviceStateCallback =
            new CameraDevice.StateCallback() {
        @Override
        public void onOpened(CameraDevice camera) {
            handleOpened(camera);
        }

        @Override
        public void onDisconnected(CameraDevice camera) {
            handleDisconnected(camera);
        }

        @Override
        public void onError(CameraDevice camera, int error) {
            handleError(camera, error);
        }

        @Override
        public void onClosed(CameraDevice camera) {
            handleClosed();
        }
    };

    @SuppressWarnings("NullableProblems")
    private final CameraCaptureSession.StateCallback cameraCaptureSessionStateCallback =
            new CameraCaptureSession.StateCallback() {
        @Override
        public void onConfigured(CameraCaptureSession session) {
            handleCameraCaptureSessionConfigured(session);
        }

        @Override
        public void onConfigureFailed(CameraCaptureSession session) {
            if (isHighResPreview) {
                handleCameraCaptureSessionHiResConfigureFailed();
                return;
            }

            Log.e(TAG, "Configuring camera session failed");
            handleError(camera, CameraDevice.StateCallback.ERROR_CAMERA_DEVICE);
        }
    };

    @SuppressWarnings("NullableProblems")
    private final CameraCaptureSession.CaptureCallback previewCaptureCallback =
            new CameraCaptureSession.CaptureCallback() {
        private long timestampOffset = Long.MIN_VALUE;

        @Override
        public void onCaptureCompleted(CameraCaptureSession session, CaptureRequest request,
                                       TotalCaptureResult result) {
            final Long timestamp = result.get(CaptureResult.SENSOR_TIMESTAMP);
            assert timestamp != null;

            // N.B. Once established, the timestampOffset should be permanently valid, since it is
            // monotonically increasing for a given CameraDevice, and an instance of CameraActor is
            // always bound to a single CameraDevice.
            if (timestampOffset == Long.MIN_VALUE) {
                final Integer timestampSource = cameraCharacteristics.get(
                        CameraCharacteristics.SENSOR_INFO_TIMESTAMP_SOURCE);
                assert timestampSource != null;
                if (timestampSource ==
                        CameraCharacteristics.SENSOR_INFO_TIMESTAMP_SOURCE_REALTIME) {
                    timestampOffset = 0;
                } else {
                    timestampOffset = SystemClock.elapsedRealtimeNanos() - timestamp;
                }
            }

            previewFpsMetric.trigger(timestamp + timestampOffset);

            handlePreviewCaptureResult(result);
        }
    };

    private final CameraPreferenceManager.EventListener cameraPreferenceEventListener =
            new CameraPreferenceManager.EventListener() {
        @Override
        public void onCaptureSessionPreferenceChanged() {
            handleCaptureSessionPreferenceChanged();
        }

        @Override
        public void onPictureCaptureRequestPreferenceChanged() {
            handlePictureCaptureRequestPreferenceChanged();
        }

        @Override
        public void onCaptureRequestPreferenceChanged() {
            handleCaptureRequestPreferenceChanged();
        }
    };

    private final AeAfController.EventListener aeAfEventListener =
            new AeAfController.EventListener() {
        @Override
        public void onReconfigureCapture() {
            handleAeAfControllerRequestsReconfigureCaptureRequest();
        }

        @Override
        public void onFocusing() {
            if (autoFocusEventListener != null) {
                autoFocusEventListener.onAutoFocusStarted();
            }
        }

        @Override
        public void onFocusingCancelled() {
            // N.B. Don't call handleAutoFocusComplete here; focus will only be cancelled if another
            // one is starting or if the cameraActor is stopping. In either case, no need to process
            // autofocus complete actions.

            if (autoFocusEventListener != null) {
                autoFocusEventListener.onAutoFocusCancelled();
            }
        }

        @Override
        public void onFocused() {
            handleAutoFocusComplete();

            if (autoFocusEventListener != null) {
                autoFocusEventListener.onAutoFocusComplete();
            }
        }

        @Override
        public void onFocusFailed() {
            handleAutoFocusComplete();

            if (autoFocusEventListener != null) {
                autoFocusEventListener.onAutoFocusFailed();
            }
        }
    };

    @SuppressWarnings("NullableProblems")
    private class PictureCaptureCallback extends CameraCaptureSession.CaptureCallback {
        private final StillImageCaptureCallback callback;
        private final boolean isFacingFront;
        private boolean captureStarted;
        private long captureStartedTimestamp = -1;

        public PictureCaptureCallback(CameraCharacteristics cameraCharacteristics,
                                      StillImageCaptureCallback callback) {
            this.callback = callback;

            Integer lensFacing = cameraCharacteristics.get(CameraCharacteristics.LENS_FACING);
            assert lensFacing != null;
            this.isFacingFront = (lensFacing == CameraCharacteristics.LENS_FACING_FRONT);
        }

        @Override
        public void onCaptureStarted(CameraCaptureSession session, CaptureRequest request,
                                     long timestamp, long frameNumber) {
            if (!captureStarted) {
                Log.i(TAG, "Still image capture [" + timestamp + "] started");
                captureStarted = true;
                captureStartedTimestamp = timestamp;
                issuedStillImageCaptureRequests.put(captureStartedTimestamp, this);

                callback.onCaptureStarting();
            } else {
                Log.e(TAG, "Received capture started callback when already started");
            }
        }

        @Override
        public void onCaptureFailed(CameraCaptureSession session, CaptureRequest request,
                                    CaptureFailure failure) {
            if (captureStarted) {
                final int reason = failure.getReason();
                Log.w(TAG, "Still image capture [" + captureStartedTimestamp + "] failed. Reason=" +
                        reason);
                issuedStillImageCaptureRequests.remove(captureStartedTimestamp);

                if (failure.getReason() == CaptureFailure.REASON_ERROR) {
                    callback.onCaptureFailed();
                } else {
                    callback.onCaptureAborted();
                }
            } else {
                Log.e(TAG, "Received capture failed callback before capture started");
            }
        }

        @Override
        public void onCaptureSequenceAborted(CameraCaptureSession session, int sequenceId) {
            if (!captureStarted) {
                Log.w(TAG, "Still image capture with sequence ID [" + sequenceId +
                        "] aborted before it was issued");
                callback.onCaptureAborted();
            } else {
                Log.e(TAG, "Received capture sequence aborted callback after sequence started");
            }
        }

        public void onCapturedImageAvailable(Image im) {
            final ByteBuffer src = im.getPlanes()[0].getBuffer();
            src.position(0);

            final ByteBuffer buf;
                         buf = ByteBuffer.allocateDirect(src.limit());
            buf.put(src);
            buf.position(0);
            callback.onCaptured(buf, isFacingFront);
        }
    }

    private final ImageReader.OnImageAvailableListener pictureAvailableListener =
            new ImageReader.OnImageAvailableListener() {
        @Override
        public void onImageAvailable(ImageReader reader) {
            for (Image im = reader.acquireNextImage(); im != null; im = reader.acquireNextImage()) {
                final long timestamp = im.getTimestamp();

                final Future<?> debugSavePictureFuture;
                if (debugSavePictures) {
                    MultiConsumerImage mcim = new MultiConsumerImage(im);
                    im = mcim.duplicate();
                    debugSavePictureFuture = debugHelper.saveImageAsync(mcim, false);
                } else {
                    debugSavePictureFuture = null;
                }

                PictureCaptureCallback callback = issuedStillImageCaptureRequests.remove(timestamp);
                if (callback != null) {
                    Log.i(TAG, "Still image capture [" + timestamp + "] completed");
                    callback.onCapturedImageAvailable(im);
                } else {
                    Log.w(TAG, "No still image capture request found with timestamp [" +
                            timestamp + "]");
                }
                im.close();

                if (debugSavePictureFuture != null) {
                    try {
                        debugSavePictureFuture.get();
                    } catch (InterruptedException e) {
                        Log.w(TAG, "Interrupted while waiting for picture to be saved", e);
                        Thread.currentThread().interrupt();
                        break;
                    } catch (ExecutionException e) {
                        throw new RuntimeException(e);
                    }
                }
            }
        }
    };

    private final PreviewFrameProcessor.Callback previewFrameProcessorCallback =
            new PreviewFrameProcessor.Callback() {
        @Override
        public void onPreviewFrameRotationComplete(PreviewFrameProcessor previewFrameProcessor,
                                                   ImageNV21 im) {
            bufferFpsMetric.trigger(im.getTimestamp());

            if (previewFrameProcessor != CameraActor.this.previewFrameProcessor) {
                Log.w(TAG, "Ignoring stale PreviewFrameProcessor callback");
                im.close();
                return;
            }

            handlePreviewFrameImage(im);
        }
    };
}
