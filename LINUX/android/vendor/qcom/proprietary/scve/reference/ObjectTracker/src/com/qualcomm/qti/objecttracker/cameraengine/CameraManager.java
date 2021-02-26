/*
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.objecttracker.cameraengine;

import java.io.IOException;
import java.util.Collections;
import java.util.List;

import com.qualcomm.qti.khronos.EGL15Helper;
import com.qualcomm.qti.khronos.GLES30Helper;
import com.qualcomm.qti.khronos.GLOES2EGLImage;
import com.qualcomm.qti.khronos.EGL15Helper.EGLContextToken;
import com.qualcomm.qti.objecttracker.cameraengine.preferences.CameraPreferenceManager;
import com.qualcomm.qti.objecttracker.misc.CoordinateSpace;

import android.content.Context;
import android.graphics.Rect;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.hardware.Camera.CameraInfo;
import android.opengl.GLES30;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;

/**
 * This class manages the {@link Camera}; it handles configuration, state management, and all
 * camera actions.
 */
@SuppressWarnings("deprecation")
public class CameraManager {
    /**
     * This interface describes the methods a class must implement to use with {@link CameraManager}
     * for video recording.
     */
    public static interface VideoRecorderControl {
        /**
         * This method will be invoked when the {@link CameraManager} is ready for video recording
         * to begin.
         */
        void startRecording();

        /**
         * This method will be invoked when the {@link CameraManager} is ready for video recording
         * to complete. It will only be invoked after {@link #startRecording()}.
         */
        void stopRecording();
    }

    /**
     * The mode that the {@link CameraManager} is currently operating in
     */
    public enum Mode {
        /**
         * The {@link CameraManager} is closed; all calls to {@link CameraManager} other than
         * {@link CameraManager#open()} will fail.
         */
        CLOSED,

        /**
         * The {@link CameraManager} is open, but preview is not running. It can be queried for
         * camera properties, but snapshots & video recording are not available.
         */
        OPEN,

        /**
         * The {@link CameraManager} is open and preview is running. All {@link CameraManager}
         * functionality is available.
         */
        IDLE,

        /**
         * The {@link CameraManager} is open and in the process of taking a snapshot. Snapshot and
         * video record functionality are unavailable until the snapshot is complete.
         */
        TAKING_SNAPSHOT,

        /**
         * The {@link CameraManager} is open and in the process of recording a video. Snapshot
         * functionality is unavailable until video recording is stopped (via
         * {@link CameraManager#stopVideoRecording()})
         */
        RECORDING_VIDEO;

        private boolean isCameraOpened() {
            return (this != CLOSED);
        }

        private boolean isPreviewRunning() {
            if (this == CLOSED || this == OPEN) {
                return false;
            }

            return true;
        }
    }

    private static final String TAG = "CameraManager";

    private final Context context;
    private final FrameDispatcher dispatcher = new FrameDispatcher();
    private final List<Camera.Area> cameraAreaList = Collections.singletonList(
            new Camera.Area(new Rect(), 1000));

    // These members are only written from the UI thread, but may be read from arbitrary thread
    // contexts. UI thread writes & reads from arbitrary threads must be protected by lock.
    private final Object lock = new Object();
    private Mode mode;

    // These members are valid only when the CameraManager is opened
    private int cameraId;
    private Camera camera;
    private Camera.Parameters cameraParameters;
    private CoordinateSpace previewSpace;
    private CoordinateSpace pictureSpace;
    private boolean useAutoExposureLock;
    private boolean useMeteringArea;
    private boolean useFocusArea;
    private boolean useAutoFocus;

    // These members are valid only when the preview is running
    private EGLContextToken eglContext;
    private SurfaceTexture dummySurfaceTexture;
    private int dummyTexture;

    // These members are valid only when video recording is active
    private VideoRecorderControl recorder;

    /**
     * Construct a {@link CameraManager}
     * @param context Application {@link Context}
     * @throws IllegalArgumentException if context is null
     */
    public CameraManager(Context context) {
        if (context == null) {
            throw new IllegalArgumentException("context cannot be null");
        }

        this.context = context;
        mode = Mode.CLOSED; // N.B. in constructor, synchronization not required
        previewSpace = CoordinateSpace.EMPTY;
        pictureSpace = CoordinateSpace.EMPTY;
    }

    /**
     * Get the {@link FrameDispatcher} associated with this {@link CameraManager}
     * @return the {@link FrameDispatcher} associated with this {@link CameraManager}
     */
    public FrameDispatcher getFrameDispatcher() {
        return dispatcher;
    }

    /**
     * Get the {@link Mode} in which the {@link CameraManager} is currently operating
     * @return a {@link Mode} value
     */
    public Mode getMode() {
        return mode;
    }

    /**
     * Open the {@link CameraManager}. It must be opened for any other operation (e.g. {@link
     * #startPreview()}, {@link #takeSnapshot()}, etc) to be performed.
     * @return true if the camera was opened, false if it failed (for e.g., if another app is
     *      currently using the camera)
     * @throws RuntimeException if {@link CameraManager} is already open
     */
    public boolean open() {
        if (mode.isCameraOpened()) {
            throw new RuntimeException("CameraManager is already open");
        }

        cameraId = CameraPreferenceManager.getSelectedCameraId(context);
        Camera.CameraInfo cameraInfo = new CameraInfo();
        Camera.getCameraInfo(cameraId, cameraInfo);

        try {
            camera = Camera.open(cameraId);
        } catch (RuntimeException e) {
            Log.e(TAG, "Failed opening camera", e);
            return false;
        }

        cameraParameters = camera.getParameters();
        CameraPreferenceManager.applyCameraPreferences(context, cameraId, cameraParameters);
        Camera.Size previewSize = cameraParameters.getPreviewSize();
        cameraParameters.setJpegQuality(100);
        cameraParameters.setRotation(0);
        Camera.Size picSize = selectPictureSize(cameraParameters);
        cameraParameters.setPictureSize(picSize.width, picSize.height);
        Log.v(TAG, "Selected [" + picSize.width + "x" + picSize.height + "] for picture size");
        int[] fpsRange = selectPreviewFpsRange(cameraParameters);
        cameraParameters.setPreviewFpsRange(fpsRange[0], fpsRange[1]);
        Log.v(TAG, "Selected [" + fpsRange[0] + "-" + fpsRange[1] + "] for preview FPS range");

        useAutoExposureLock = cameraParameters.isAutoExposureLockSupported();
        useMeteringArea = (cameraParameters.getMaxNumMeteringAreas() > 0);
        useFocusArea = (cameraParameters.getMaxNumFocusAreas() > 0);
        useAutoFocus = Camera.Parameters.FOCUS_MODE_AUTO.equals(cameraParameters.getFocusMode());

        if (useAutoExposureLock) {
            cameraParameters.setAutoExposureLock(false);
        }
        if (useMeteringArea) {
            cameraParameters.setMeteringAreas(null);
        }
        if (useFocusArea) {
            cameraParameters.setFocusAreas(null);
        }

        camera.setParameters(cameraParameters);

        previewSpace = CoordinateSpace.forCamera(previewSize.width, previewSize.height, cameraInfo);
        pictureSpace = CoordinateSpace.forCamera(picSize.width, picSize.height, cameraInfo);

        synchronized (lock) {
            mode = Mode.OPEN;
        }

        return true;
    }

    /**
     * Close the {@link CameraManager}. It must have been previously {@link #open()}ed.
     * @throw {@link RuntimeException} if {@link CameraManager} is not currently open
     */
    public void close() {
        if (!mode.isCameraOpened()) {
            throw new RuntimeException("CameraManager is not open");
        }

        if (mode.isPreviewRunning()) {
            stopPreview();
        }

        synchronized (lock) {
            mode = Mode.CLOSED;
        }

        camera.release();
        camera = null;

        cameraId = 0;
        cameraParameters = null;
        previewSpace = CoordinateSpace.EMPTY;
        pictureSpace = CoordinateSpace.EMPTY;
        useAutoExposureLock = false;
        useMeteringArea = false;
        useFocusArea = false;
    }

    /**
     * Get the preview frame {@link CoordinateSpace}
     * @return the preview frame {@link CoordinateSpace}
     * @throws RuntimeException if {@link CameraManager} is not {@link #open()}ed
     */
    public CoordinateSpace getPreviewFrameSpace() {
        if (!mode.isCameraOpened()) {
            throw new RuntimeException("Cannot get preview space when CameraManager is not open");
        }
        return previewSpace;
    }

    /**
     * Get the picture frame {@link CoordinateSpace}
     * @return the picture frame {@link CoordinateSpace}
     * @throws RuntimeException if {@link CameraManager} is not {@link #open()}ed
     */
    public CoordinateSpace getPictureFrameSpace() {
        if (!mode.isCameraOpened()) {
            throw new RuntimeException("Cannot get picture space when CameraManager is not open");
        }
        return pictureSpace;
    }

    /**
     * Start the camera preview. The preview is not rendered to the screen by default; to do so,
     * one would need to register a {@link FrameSink} with the {@link FrameDispatcher} (which can
     * be obtained via {@link #getFrameDispatcher()}).
     * @throws RuntimeException if {@link CameraManager} is not {@link #open()}ed, or if the preview
     *      is already running
     */
    public void startPreview() {
        if (!mode.isCameraOpened()) {
            throw new RuntimeException("Cannot start preview when CameraManager is not open");
        }

        if (mode.isPreviewRunning()) {
            throw new RuntimeException("Preview is already running");
        }

        // Create an EGL context on which to bind a dummy SurfaceTexture, which will be used to
        // consume updates from the camera. These updates will ultimately be ignored, since we get
        // our camera frames via PreviewCallback.onPreviewFrame().
        eglContext = EGL15Helper.initializeGLES30EGLContext();
        EGL15Helper.makeEGLContextCurrent(eglContext);
        dummyTexture = GLES30Helper.generateTexture(GLOES2EGLImage.GL_TEXTURE_EXTERNAL_OES);
        dummySurfaceTexture = new SurfaceTexture(dummyTexture);
        dummySurfaceTexture.setOnFrameAvailableListener(
                new SurfaceTexture.OnFrameAvailableListener() {
            @Override
            public void onFrameAvailable(SurfaceTexture surfaceTexture) {
                synchronized (lock) {
                    if (mode.isPreviewRunning()) {
                        EGL15Helper.makeEGLContextCurrent(eglContext);
                        dummySurfaceTexture.updateTexImage();
                    }
                }
            }
        });

        try {
            camera.setPreviewTexture(dummySurfaceTexture);
        } catch (IOException e) {
            Log.e(TAG, "Failed to create a SurfaceTexture to act as the preview target", e);
            throw new RuntimeException("Failed starting preview", e);
        }

        dispatcher.start(camera, previewSpace);

        synchronized (lock) {
            mode = Mode.IDLE;
        }

        camera.startPreview();
        camera.cancelAutoFocus();
    }

    /**
     * Stop the camera preview.
     * @throws RuntimeException if {@link CameraManager} is not {@link #open()}ed, or if the preview
     *      is not running
     */
    public void stopPreview() {
        if (!mode.isCameraOpened()) {
            throw new RuntimeException("Cannot stop preview when CameraManager is not open");
        } else if (!mode.isPreviewRunning()) {
            throw new RuntimeException("Preview is not running");
        }

        if (mode == Mode.RECORDING_VIDEO) {
            stopVideoRecording();
        }

        camera.stopPreview();

        synchronized (lock) {
            mode = Mode.OPEN;
        }

        dispatcher.stop();

        /*
        try {
            camera.setPreviewTexture(null);
        } catch (IOException e) {
            Log.e(TAG, "Failed clearing the SurfaceTexture preview target", e);
            throw new RuntimeException("Failed stopping preview", e);
        }
        */

        EGL15Helper.makeEGLContextCurrent(eglContext);
        dummySurfaceTexture.release();
        dummySurfaceTexture = null;
        GLES30.glDeleteTextures(1, new int[] { dummyTexture }, 0);
        dummyTexture = 0;
        EGL15Helper.releaseEGLContext(eglContext);
        eglContext = null;
    }

    /**
     * Update the autofocus area for the current camera (if focus areas are supported).
     * <p><i>NOTE: this will not take effect until {@link #applyAutoFocusAutoExposure()} is
     * invoked</i>
     * @param area the autofocus area (in camera frame coordinates) to set for the current camera.
     *      If null, the autoexposure metering area will be cleared.
     */
    public void updateAutoFocusArea(Rect area) {
        if (!mode.isPreviewRunning()) {
            Log.w(TAG, "Cannot autofocus when preview is not running");
            return;
        } else if (!useFocusArea) {
            return;
        }

        if (area == null) {
            cameraParameters.setFocusAreas(null);
        } else if (area.left < 0 || area.top < 0 || area.right > previewSpace.width ||
                area.bottom > previewSpace.height) {
            Log.w(TAG, "Illegal focus area " + area.toShortString() + " set, clearing focus area");
            cameraParameters.setFocusAreas(null);
        } else {
            float horzScale = 2000f / previewSpace.width;
            float vertScale = 2000f / previewSpace.height;
            Camera.Area cameraArea = cameraAreaList.get(0);
            cameraArea.rect.left = (int)(area.left * horzScale - 1000f);
            cameraArea.rect.right = (int)(area.right * horzScale - 1000f);
            cameraArea.rect.top = (int)(area.top * vertScale - 1000f);
            cameraArea.rect.bottom = (int)(area.bottom * vertScale - 1000f);
            cameraParameters.setFocusAreas(cameraAreaList);
        }
    }

    /**
     * Update the autoexposure metering area for the current camera (if metering areas are
     * supported).
     * <p><i>NOTE: this will not take effect until {@link #applyAutoFocusAutoExposure()} is
     * invoked</i>
     * @param area the autoexposure metering area (in camera frame coordinates) to set for the
     *      current camera. If null, the autoexposure metering area will be cleared.
     */
    public void updateAutoExposureMeteringArea(Rect area) {
        if (!mode.isPreviewRunning()) {
            Log.w(TAG, "Cannot set autoexposure metering area when preview is not running");
            return;
        } else if (!useMeteringArea) {
            return;
        }

        if (area == null) {
            cameraParameters.setMeteringAreas(null);
        } else if (area.left < 0 || area.top < 0 || area.right > previewSpace.width ||
                area.bottom > previewSpace.height) {
            Log.w(TAG, "Illegal autoexposure metering area " + area.toShortString() +
                    " set, clearing metering area");
            cameraParameters.setMeteringAreas(null);
        } else {
            float horzScale = 2000f / previewSpace.width;
            float vertScale = 2000f / previewSpace.height;
            Camera.Area cameraArea = cameraAreaList.get(0);
            cameraArea.rect.left = (int)(area.left * horzScale - 1000f);
            cameraArea.rect.right = (int)(area.right * horzScale - 1000f);
            cameraArea.rect.top = (int)(area.top * vertScale - 1000f);
            cameraArea.rect.bottom = (int)(area.bottom * vertScale - 1000f);
            cameraParameters.setMeteringAreas(cameraAreaList);
        }
    }

    /**
     * Update the autoexposure lock for the current camera (if autoexposure lock is supported).
     * <p><i>NOTE: this will not take effect until {@link #applyAutoFocusAutoExposure()} is
     * invoked</i>
     * @param toggle True to request that the camera pause the autoexposure calculation, false to
     *      allow the camera to resume autoexposure calculations
     */
    public void updateAutoExposureLock(boolean toggle) {
        if (!mode.isPreviewRunning()) {
            Log.w(TAG, "Cannot set autoexposure metering area when preview is not running");
            return;
        } else if (!useAutoExposureLock) {
            return;
        }

        cameraParameters.setAutoExposureLock(toggle);
    }

    /**
     * Apply any changes to autofocus and autoexposure settings to the camera
     */
    public void applyAutoFocusAutoExposure() {
        if (!mode.isPreviewRunning()) {
            Log.w(TAG, "Cannot apply focus/metering area changes when preview is not running");
            return;
        } else if (!useFocusArea && !useMeteringArea) {
            // Neither auto-focus areas nor metering areas supported; nothing to apply
            return;
        }

        camera.setParameters(cameraParameters);
    }

    /**
     * Request that the camera do an autofocus operation (if autofocus is enabled for the current
     * focus mode)
     */
    public void doAutoFocus() {
        if (!mode.isPreviewRunning()) {
            Log.w(TAG, "Cannot autofocus when preview is not running");
            return;
        } else if (!useAutoFocus) {
            return;
        }

        camera.autoFocus(null);
    }

    /**
     * Request that the default focus be set for the current focus mode (ie cancels the results of
     * a previous {@link #doAutoFocus()} operation).
     */
    public void restoreDefaultFocus() {
        if (!mode.isPreviewRunning()) {
            Log.w(TAG, "Cannot set default focus when preview is not running");
            return;
        } else if (!useAutoFocus) {
            return;
        }

        camera.cancelAutoFocus();
    }

    /**
     * Takes a snapshot. The preview must be running to invoke this operation.
     * @param jpegCallback a {@link Camera.PictureCallback} to invoke when the picture has been
     *      captured. Note that the {@link Camera} parameter to
     *      {@link Camera.PictureCallback#onPictureTaken(byte[], Camera)} will be null.
     * @return true if a snapshot capture operation has been started, else false
     * @throws IllegalArgumentException if jpegCallback is null
     * @throws RuntimeException if {@link CameraManager} is not {@link #open()}ed
     */
    public boolean takeSnapshot(final Camera.PictureCallback jpegCallback) {
        if (jpegCallback == null) {
            throw new IllegalArgumentException("jpegCallback cannot be null");
        } else if (!mode.isCameraOpened()) {
            throw new RuntimeException("Cannot take snapshot when CameraManager is not open");
        } else if (mode != Mode.IDLE) {
            Log.w(TAG, "Can only take a snapshot when camera is idle");
            return false;
        }

        synchronized (lock) {
            mode = Mode.TAKING_SNAPSHOT;
        }

        camera.takePicture(null, null, new Camera.PictureCallback() {
            @Override
            public void onPictureTaken(byte[] data, final Camera camera) {
                jpegCallback.onPictureTaken(data, null);

                // Restart the preview after this callback returns
                Handler h = new Handler(Looper.getMainLooper());
                h.post(new Runnable() {
                    @Override
                    public void run() {
                        if (mode.isCameraOpened()) {
                            mode = Mode.IDLE;
                            camera.startPreview();
                        }
                    }
                });
            }
        });

        return true;
    }

    /**
     * Start recording video. The preview must be running to invoke this operation.
     * @param recorder a {@link VideoRecorderControl} through which the {@link CameraManager} can
     *      control video recording
     * @return true if a video record operation has been started, else false
     * @throws IllegalArgumentException is recorder is null
     * @throws RuntimeException if {@link CameraManager} is not {@link #open()}ed
     */
    public boolean startVideoRecording(VideoRecorderControl recorder) {
        if (recorder == null) {
            throw new IllegalArgumentException("recorder cannot be null");
        } else if (!mode.isCameraOpened()) {
            throw new RuntimeException("Cannot record video when CameraManager is not open");
        } else if (mode != Mode.IDLE) {
            Log.w(TAG, "Can only start video recording when camera is idle");
            return false;
        }

        synchronized (lock) {
            mode = Mode.RECORDING_VIDEO;
        }

        this.recorder = recorder;
        recorder.startRecording();

        return true;
    }

    /**
     * Stop recording video. Video recording must be active to invoke this operation.
     * @throws RuntimeException if {@link CameraManager} is not {@link #open()}ed
     */
    public void stopVideoRecording() {
        if (!mode.isCameraOpened()) {
            throw new RuntimeException("Cannot record video when CameraManager is not open");
        } else if (mode != Mode.RECORDING_VIDEO) {
            Log.w(TAG, "Video is not being recorded");
            return;
        }

        recorder.stopRecording();
        recorder = null;

        synchronized (lock) {
            mode = Mode.IDLE;
        }
    }

    private static Camera.Size selectPictureSize(Camera.Parameters parameters) {
        // Allowed picture aspect ratios fall within +/- 1% of preview aspect ratio
        Camera.Size previewSize = parameters.getPreviewSize();
        final float aspectRatioMin = (float)previewSize.width / (float)previewSize.height * 0.99f;
        final float aspectRatioMax = (float)previewSize.width / (float)previewSize.height * 1.01f;

        // Select the largest picture size that meets the aspect ratio requirements
        List<Camera.Size> pictureSizes = parameters.getSupportedPictureSizes();
        Camera.Size selected = null;
        int selectedSize = 0;
        for (Camera.Size pictureSize : pictureSizes) {
            float aspect = (float)pictureSize.width / (float)pictureSize.height;
            int size = pictureSize.width * pictureSize.height;
            if (aspect >= aspectRatioMin && aspect <= aspectRatioMax && size > selectedSize) {
                selected = pictureSize;
                selectedSize = size;
            }
        }

        // No matching aspect ratios, just select the first picture size
        if (selected == null) {
            selected = pictureSizes.get(0);
        }

        return selected;
    }

    private static int[] selectPreviewFpsRange(Camera.Parameters parameters) {
        final List<int[]> supportedFpsRanges = parameters.getSupportedPreviewFpsRange();
        int[] candidate = null;

        // Find the widest FPS range that includes 30FPS
        for (int[] fpsRange : supportedFpsRanges) {
            if (fpsRange[0] <= 30000 && fpsRange[1] >= 30000 && (candidate == null ||
                    (fpsRange[1] - fpsRange[0]) > candidate[1] - candidate[0])) {
                candidate = fpsRange;
            }
        }

        // Failing that, find the FPS range with the highest max FPS
        if (candidate == null) {
            for (int[] fpsRange : supportedFpsRanges) {
                if (candidate == null || fpsRange[1] > candidate[1]) {
                    candidate = fpsRange;
                }
            }
        }

        return candidate;
    }
}
