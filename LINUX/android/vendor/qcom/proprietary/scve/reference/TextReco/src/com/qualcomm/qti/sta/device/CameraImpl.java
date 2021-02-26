/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.sta.device;

import java.io.IOException;
import java.util.List;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.PixelFormat;
import android.hardware.Camera;
import android.hardware.Camera.AutoFocusCallback;
import android.hardware.Camera.CameraInfo;
import android.hardware.Camera.Parameters;
import android.hardware.Camera.PictureCallback;
import android.hardware.Camera.Size;
import android.media.MediaActionSound;
import android.os.Handler;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Display;
import android.view.Gravity;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.View;
import android.widget.FrameLayout;
import android.widget.FrameLayout.LayoutParams;

import com.qualcomm.qti.sta.data.ViewSize;
import com.qualcomm.qti.sta.engine.JniManager.FrameSession;

@SuppressWarnings("deprecation")
/**
 * Class to handle the camera
 */
public class CameraImpl extends CameraHandler implements Camera.PreviewCallback,
        SurfaceHolder.Callback {
    private static final String LOG_TAG = "CameraImpl";
    private static final int FOCUS_FAILED_TIMER = 1000;

    private enum AutoFocusState {
        IDLE, IN_PROGRESS, DONE;
    }

    private Handler snapshotHandler = new Handler();
    private SurfaceHolder surfaceHolder = null;
    private boolean surfaceCreated = false;
    private Camera camera = null;
    private final CameraPreview previewView;
    private int cameraId = -1;

    private byte[] buffer1 = null;
    private byte[] buffer2 = null;

    private AutoFocusState autoFocusState = AutoFocusState.IDLE;
    AutoFocusCallback autoFocusCallback = new AutoFocusCallback() {
        /**
         * {@inheritDoc}
         */
        @Override
        public void onAutoFocus(boolean success, Camera camera) {
            Log.d(LOG_TAG, "Auto Focus: status " + success);
            if (success) {
                if (cameraListener != null) {
                    Log.d(LOG_TAG, "TS: start snapshot at " + System.currentTimeMillis());
                    camera.takePicture(null, null, pictureCallback);
                    new MediaActionSound().play(MediaActionSound.SHUTTER_CLICK);
                }
                autoFocusState = AutoFocusState.DONE;
            } else {
                snapshotHandler.postDelayed(tryAutoFocusRunnable, FOCUS_FAILED_TIMER);
            }
        }
    };
    final Runnable tryAutoFocusRunnable = new Runnable() {
        /**
         * {@inheritDoc}
         */
        @Override
        public void run() {
            Log.d(LOG_TAG, "Auto Focus: Trying again");
            if (autoFocusState == AutoFocusState.IN_PROGRESS && camera != null) {
                camera.autoFocus(autoFocusCallback);
            }
        }
    };
    PictureCallback pictureCallback = new PictureCallback() {
        /**
         * {@inheritDoc}
         */
        @Override
        public void onPictureTaken(byte[] data, Camera camera) {
            Log.d(LOG_TAG, "Picture taken, Auto Focus: cancel");
            camera.cancelAutoFocus();
            autoFocusState = AutoFocusState.IDLE;

            Bitmap snapshot = getExifRotatedBitmap(data);
            if (cameraListener != null) {
                cameraListener.onSnapshot(snapshot);
            }
        }
    };

    /**
     * Constructor for the camera handler class
     *
     * @param context
     *            application context
     * @param listener
     *            camera event listener
     */
    public CameraImpl(Context context, CameraListener listener) {
        super(context, listener);

        previewView = new CameraPreview(context);
        surfaceHolder = previewView.getHolder();
        surfaceHolder.addCallback(this);
    }

    /**
     * {@inheritDoc}
     */
    public boolean initialize(Display display) {
        if (!openBackCamera()) {// open the back camera
            Log.d(LOG_TAG, "Camera null");
            camera = null;
            return false;
        }

        // get the camera information and camera parameters
        CameraInfo cameraInfo = new CameraInfo();
        Camera.getCameraInfo(cameraId, cameraInfo);
        viewRotation = getCameraDisplayOrientation(display, cameraInfo.orientation);

        // set orientation
        Parameters parameters = camera.getParameters();
        List<String> focusModes = parameters.getSupportedFocusModes();
        if (focusModes.contains(Parameters.FOCUS_MODE_CONTINUOUS_PICTURE)) {
            continuousFocusSupported = true;
            parameters.setFocusMode(Parameters.FOCUS_MODE_CONTINUOUS_PICTURE);
        }

        previewSize = pickPreviewSize(display);
        pictureSize = pickPictureSize();

        parameters.setPreviewSize(previewSize.width, previewSize.height);
        parameters.setPictureSize(pictureSize.width, pictureSize.height);

        Log.d(LOG_TAG, "previewSize: " + previewSize.width + "," + previewSize.height
                + " pictureSize: " + pictureSize.width + "," + pictureSize.height);

        parameters.setRotation(viewRotation);
        camera.setParameters(parameters);
        camera.setDisplayOrientation(viewRotation);

        Log.i(LOG_TAG, "Camera initialized with orientation: " + viewRotation);

        updateCoordinateSpaces();
        updateViewRotation(display);
        previewView.setVisibility(View.VISIBLE);
        return true;
    }

    private void updateSurfaceParameters(Display display) {
        DisplayMetrics metrics = new DisplayMetrics();
        display.getMetrics(metrics);

        Log.d(LOG_TAG, "Screen size: " + metrics.widthPixels + "," + metrics.heightPixels);

        ViewSize returnSize = null;
        float horizontalRatio;
        float verticalRatio;
        if (viewRotation == 90 || viewRotation == 270) {
            horizontalRatio = (float) previewSize.width / (float) metrics.heightPixels;
            verticalRatio = (float) previewSize.height / (float) metrics.widthPixels;
            surfaceViewRatio = Math.max(horizontalRatio, verticalRatio);
            int newWidth = (int) (previewSize.height / surfaceViewRatio);
            int newHeight = (int) (previewSize.width / surfaceViewRatio);
            returnSize = new ViewSize(newWidth, newHeight);

        } else {
            horizontalRatio = (float) previewSize.width / (float) metrics.widthPixels;
            verticalRatio = (float) previewSize.height / (float) metrics.heightPixels;
            surfaceViewRatio = Math.max(horizontalRatio, verticalRatio);
            int newWidth = (int) (previewSize.width / surfaceViewRatio);
            int newHeight = (int) (previewSize.height / surfaceViewRatio);
            returnSize = new ViewSize(newWidth, newHeight);
        }

        surfaceSize = returnSize;
    }

    protected void updateViewRotation(Display display) {
        updateSurfaceParameters(display);

        FrameLayout.LayoutParams layoutParameters = new FrameLayout.LayoutParams(
                LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
        layoutParameters.width = surfaceSize.width;
        layoutParameters.height = surfaceSize.height;
        layoutParameters.gravity = Gravity.CENTER;
        previewView.setLayoutParams(layoutParameters);
    }

    /**
     * {@inheritDoc}
     */
    public ViewSize getMatchingPreviewSize(float aspectRatioMin, float aspectRatioMax) {
        Parameters parameters = camera.getParameters();
        List<Size> previewSizes = parameters.getSupportedPreviewSizes();
        Size selected = null;
        Size selectedUnMatched = null;
        for (Size previewSize : previewSizes) {
            // Log.d(LOG_TAG, "Supported preview size: " + previewSize.width +
            // " X " + previewSize.height);
            if (previewSize.width >= MIN_WIDTH && previewSize.height >= MIN_HEIGHT) {
                float aspect = (float) previewSize.width / (float) previewSize.height;
                if (selected == null
                        || (selected.width >= previewSize.width || selected.height > previewSize.height)) {
                    selectedUnMatched = previewSize;
                    if (aspect >= aspectRatioMin && aspect <= aspectRatioMax) {
                        selected = previewSize;
                    }
                }
            }
        }
        if (selected == null) {
            selected = selectedUnMatched;
            Log.d(LOG_TAG, "PreviewSize: Assigning size outside max min but near to OCR limits");
        }

        // No matching aspect ratios, just select the first picture size
        if (selected == null) {
            selected = previewSizes.get(0);
            Log.d(LOG_TAG, "PreviewSize: Assigning first size");
        }
        return new ViewSize(selected.width, selected.height);
    }

    /**
     * {@inheritDoc}
     */
    public ViewSize getMatchingPictureSize(float aspectRatioMin, float aspectRatioMax) {
        Parameters parameters = camera.getParameters();
        List<Size> pictureSizes = parameters.getSupportedPictureSizes();
        // Select the largest picture size that meets the aspect ratio requirements
        Size selected = null;
        Size selectedUnMatched = null;
        for (Size pictureSize : pictureSizes) {
            // Log.d(LOG_TAG, "Supported picture size: " + pictureSize.width +
            // " X " + pictureSize.height);
            if (pictureSize.width > MIN_WIDTH && pictureSize.height > MIN_HEIGHT) {
                float aspect = (float) pictureSize.width / (float) pictureSize.height;
                if (selected == null
                        || (pictureSize.width >= selected.width || pictureSize.height > selected.height)) {
                    if (selectedUnMatched == null
                            || (pictureSize.width >= selectedUnMatched.width || pictureSize.height > selectedUnMatched.height)) {
                        selectedUnMatched = pictureSize;
                    }
                    if (aspect >= aspectRatioMin && aspect <= aspectRatioMax) {
                        selected = pictureSize;
                    }
                }
            }
        }
        if (selected == null) {
            selected = selectedUnMatched;
            Log.d(LOG_TAG, "PictureSize: Assigning size outside max min but near to OCR limits");
        }

        // No matching aspect ratios, just select the first picture size
        if (selected == null) {
            selected = pictureSizes.get(0);
        }

        return new ViewSize(selected.width, selected.height);
    }

    /**
     * {@inheritDoc}
     */
    public void wrapUp() {
        super.wrapUp();
        clear();

        previewView.setVisibility(View.GONE);
        if (camera != null) {
            Log.i(LOG_TAG, "Camera Wrapping");;
            camera.stopPreview();
            camera.release();
            camera = null;

            Log.i(LOG_TAG, "Camera Wrapped");
        }

        flashLightOn = false;
    }

    private boolean openBackCamera() {
        Camera.CameraInfo cameraInfo = new Camera.CameraInfo();
        int maximumCameras = Camera.getNumberOfCameras();

        for (int index = 0; index < maximumCameras; index++) {
            Camera.getCameraInfo(index, cameraInfo);
            if (cameraInfo.facing == Camera.CameraInfo.CAMERA_FACING_BACK) {
                try {
                    camera = Camera.open(index);
                    cameraId = index;
                    return true;

                } catch (RuntimeException e) {
                    Log.e(LOG_TAG, "Camera failed to open: " + e.getLocalizedMessage());
                    return false;
                }
            }
        }
        return false;
    }

    private int getCameraDisplayOrientation(Display display, int sensorRotation) {
        int dpyRotation;
        switch (display.getRotation()) {
        case Surface.ROTATION_0:
            dpyRotation = 0;
            break;
        case Surface.ROTATION_90:
            dpyRotation = 90;
            break;
        case Surface.ROTATION_180:
            dpyRotation = 180;
            break;
        case Surface.ROTATION_270:
            dpyRotation = 270;
            break;
        default:
            throw new RuntimeException("Unexpected rotation value");
        }

        return (sensorRotation - dpyRotation + 360) % 360;
    }

    /**
     * {@inheritDoc} /
     */
    @Override
    public void onPreviewFrame(byte[] data, Camera camera) {
        // Log.d(LOG_TAG, "Frame: new frame start");
        FrameSession session = cameraListener.onFrameAvailable(data, previewSize.width,
                previewSize.height);
        if (session.isActive()) {// re-register callbacks
            byte[] availableBuffer = (session.getImageBuffer() != buffer1) ? buffer1 : buffer2;
            camera.addCallbackBuffer(availableBuffer);
        } else {
            Log.d(LOG_TAG, "no preview callback, session not active");
        }
        // Log.d(LOG_TAG, "Frame: new frame ends");
    }

    /**
     * This method sets the callbacks for listening to the preview frames
     *
     * @param enable
     *            true to set the callbacks, false otherwise
     */
    private void setCallback(boolean enable) {
        if (enable) {// with buffer requires add buffer.
            Log.d(LOG_TAG, "Enable callback");
            camera.addCallbackBuffer(buffer1);
            camera.setPreviewCallbackWithBuffer(this);

        } else {
            Log.d(LOG_TAG, "disable callback");
            camera.addCallbackBuffer(null);
            camera.setPreviewCallbackWithBuffer(null);
        }
    }

    /**
     * {@inheritDoc}
     */
    public boolean toggleFlash() {
        if (camera == null) {
            Log.e(LOG_TAG, "Camera is not initialized");
            return false;
        }
        flashLightOn = !flashLightOn;
        Log.d(LOG_TAG, "Auto Focus: flash " + flashLightOn);

        Parameters params = camera.getParameters();
        params.setFlashMode((flashLightOn) ? Parameters.FLASH_MODE_TORCH
                : Parameters.FLASH_MODE_OFF);
        camera.setParameters(params);

        return true;
    }

    /**
     * {@inheritDoc}
     */
    public void toggleMacroFocusMode() {
        if (camera == null) {
            Log.e(LOG_TAG, "Camera is not initialized");
            return;
        }
        Parameters parameters = camera.getParameters();
        String currentFocusMode = parameters.getFocusMode();
        if (!currentFocusMode.equals(Parameters.FOCUS_MODE_MACRO)) {
            parameters.setFocusMode(Parameters.FOCUS_MODE_MACRO);
            Log.d(LOG_TAG, "Focus Mode: " + currentFocusMode + " => " + Parameters.FOCUS_MODE_MACRO);
        } else if (continuousFocusSupported) {
            parameters.setFocusMode(Parameters.FOCUS_MODE_CONTINUOUS_PICTURE);
            Log.d(LOG_TAG, "Focus Mode: " + currentFocusMode + " => "
                    + Parameters.FOCUS_MODE_CONTINUOUS_PICTURE);
        } else {
            parameters.setFocusMode(Parameters.FOCUS_MODE_AUTO);
            Log.d(LOG_TAG, "Focus Mode: " + currentFocusMode + " => " + Parameters.FOCUS_MODE_AUTO);
        }
        camera.setParameters(parameters);
    }

    /**
     * {@inheritDoc}
     */
    public boolean isMacroFocusMode() {
        if (camera == null) {
            return false;
        }
        Parameters parameters = camera.getParameters();
        String currentFocusMode = parameters.getFocusMode();
        return currentFocusMode.equals(Parameters.FOCUS_MODE_MACRO);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void removeCaptureCallbacks() {
        if (camera == null) {
            return;
        }
        setCallback(false);

        Log.d(LOG_TAG, "Stop looking, Auto Focus: cancel");
        camera.cancelAutoFocus();

        autoFocusState = AutoFocusState.IDLE;
        snapshotHandler.removeCallbacksAndMessages(null);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void startFrameCycle() {
        if (camera == null) {
            return;
        }
        Log.d(LOG_TAG, "Start Sending frames");
        createFrameBuffer();
        setCallback(true);
    }

    private void createFrameBuffer() {
        if (camera == null) {
            return;
        }

        if(buffer1 == null || buffer2 == null) {
            Parameters parameters = camera.getParameters();
            int pformat = parameters.getPreviewFormat();
            PixelFormat info = new PixelFormat();
            PixelFormat.getPixelFormatInfo(pformat, info);

            int bufferSize = ((previewSize.width * previewSize.height * info.bitsPerPixel) >> 3);
            buffer1 = new byte[bufferSize];
            buffer2 = new byte[bufferSize];
        }
    }

    private void startPreview() {
        if (camera == null || !surfaceCreated) {
            return;
        }

        Log.d(LOG_TAG, "Start preview");
        camera.startPreview();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void startSnapshot() {
        if (camera == null) {
            return;
        }
        Parameters parameters = camera.getParameters();
        String currentFocusMode = parameters.getFocusMode();
        if (currentFocusMode.equals(Parameters.FOCUS_MODE_CONTINUOUS_PICTURE)) {
            Log.d(LOG_TAG, "TS: start snapshot at " + System.currentTimeMillis());
            camera.takePicture(null, null, pictureCallback);
            new MediaActionSound().play(MediaActionSound.SHUTTER_CLICK);
            autoFocusState = AutoFocusState.DONE;

        } else {
            Log.d(LOG_TAG, "Focusing: start");
            autoFocusState = AutoFocusState.IN_PROGRESS;
            camera.autoFocus(autoFocusCallback);
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        try {
            Log.d(LOG_TAG, "surfaceCreated");
            surfaceCreated = true;
            camera.setPreviewDisplay(holder);
            startPreview();

        } catch (IOException e) {
            Log.d(LOG_TAG, "preview failed on creation: " + e.getMessage());
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        Log.d(LOG_TAG, "surfaceDestroyed");
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        Log.d(LOG_TAG, "surfaceChanged");
        if (surfaceHolder.getSurface() == null) {
            return;
        }

        try {
            camera.stopPreview();
        } catch (Exception e) {
            Log.d(LOG_TAG, "Camera not running");
        }

        try {
            camera.setPreviewDisplay(surfaceHolder);
            startPreview();

        } catch (Exception e) {
            Log.d(LOG_TAG, "preview failed on surface changed: " + e.getMessage());
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean isCameraInitialized() {
        return (camera != null);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public View getView() {
        return previewView;
    }
}
