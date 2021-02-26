/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.sta.device;

import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.Semaphore;
import java.util.concurrent.TimeUnit;

import android.annotation.TargetApi;
import android.content.Context;
import android.content.res.Configuration;
import android.graphics.Bitmap;
import android.graphics.ImageFormat;
import android.graphics.Matrix;
import android.graphics.RectF;
import android.graphics.SurfaceTexture;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.CameraMetadata;
import android.hardware.camera2.CaptureFailure;
import android.hardware.camera2.CaptureRequest;
import android.hardware.camera2.TotalCaptureResult;
import android.media.Image;
import android.media.ImageReader;
import android.media.MediaActionSound;
import android.os.Build;
import android.os.Handler;
import android.os.HandlerThread;
import android.util.DisplayMetrics;
import android.util.Log;
import android.util.Size;
import android.view.Display;
import android.view.Surface;
import android.view.TextureView.SurfaceTextureListener;
import android.view.View;

import com.qualcomm.qti.sta.data.ViewSize;
import com.qualcomm.qti.sta.engine.JniManager.FrameSession;

/**
 * Class for implementing the camera2 apis
 */
@TargetApi(Build.VERSION_CODES.LOLLIPOP)
public class Camera2Impl extends CameraHandler implements SurfaceTextureListener {
    private static final String LOG_TAG = "Camera2Impl";

    private final CameraDevice.StateCallback cameraDeviceStateCallback = new CameraDevice.StateCallback() {
        /**
         * {@inheritDoc}
         */
        @Override
        public void onOpened(CameraDevice cameraDevice) {
            openCloseLock.release();
            Log.i(LOG_TAG, "Camera State: opened");
            camera = cameraDevice;
            startPreview();

        }

        /**
         * {@inheritDoc}
         */
        @Override
        public void onError(CameraDevice camera, int error) {
            Log.e(LOG_TAG, "Camera State: Failed to open, error [" + error + "]");
            openCloseLock.release();
            wrapUp();
        }

        /**
         * {@inheritDoc}
         */
        @Override
        public void onDisconnected(CameraDevice camera) {
            Log.i(LOG_TAG, "Camera State: disconnected");
            openCloseLock.release();
            wrapUp();
        }

        /**
         * {@inheritDoc}
         */
        @Override
        public void onClosed(CameraDevice camera) {
            Log.i(LOG_TAG, "Camera State: closed");
        }
    };
    private final CameraCaptureSession.StateCallback previewSessionCallback = new CameraCaptureSession.StateCallback() {
        /**
         * {@inheritDoc}
         */
        @Override
        public void onConfigured(CameraCaptureSession session) {
            try {
                onPreviewSessionCreated(session);
            } catch (CameraAccessException e) {
                Log.e(LOG_TAG, "Preview session: Failed to start", e);
                wrapUp();
            }
        }

        /**
         * {@inheritDoc}
         */
        @Override
        public void onConfigureFailed(CameraCaptureSession session) {
            Log.e(LOG_TAG, "Preview session: Failed to configure");
            wrapUp();
        }

        /**
         * {@inheritDoc}
         */
        @Override
        public void onClosed(CameraCaptureSession session) {
            Log.i(LOG_TAG, "Preview session: closed");
        }
    };

    private class FrameCaptureCallback implements ImageReader.OnImageAvailableListener {
        /**
         * {@inheritDoc}
         */
        @Override
        public void onImageAvailable(ImageReader reader) {
            if (frameReader == null) {
                Log.d(LOG_TAG, "session is closing");
                return;
            }

            Image image = reader.acquireLatestImage();
            if (image == null) {
                Log.e(LOG_TAG, "image read is null");
                return;
            }

            ByteBuffer yPlane = image.getPlanes()[0].getBuffer();
            ByteBuffer uvPlane = image.getPlanes()[2].getBuffer();

            int yPlaneSize = yPlane.remaining();
            yPlane.get(frameBytes, 0, yPlaneSize);
            uvPlane.get(frameBytes, yPlaneSize, uvPlane.remaining());
            image.close();

            FrameSession session = cameraListener.onFrameAvailable(frameBytes, previewSize.width,
                    previewSize.height);
            if (!session.isActive()) {// re-register callback
                Log.d(LOG_TAG, "session inactive");
                closeFrameReader();
            }
        }
    }

    private class PictureSessionCallback extends CameraCaptureSession.StateCallback {
        private final ImageReader reader;
        private final Handler handler;

        private PictureSessionCallback(ImageReader reader, Handler handler) {
            this.reader = reader;
            this.handler = handler;
        }

        /**
         * {@inheritDoc}
         */
        @Override
        public void onConfigured(CameraCaptureSession session) {
            Log.d(LOG_TAG, "Picture Session: configured");
            try {
                final CaptureRequest.Builder captureBuilder = camera
                        .createCaptureRequest(CameraDevice.TEMPLATE_STILL_CAPTURE);
                setSessionParameter(captureBuilder, SessionType.SNAPSHOT);
                captureBuilder.addTarget(reader.getSurface());
                final CameraCaptureSession.CaptureCallback captureListener = new CameraCaptureSession.CaptureCallback() {
                    @Override
                    public void onCaptureCompleted(CameraCaptureSession session,
                            CaptureRequest request, TotalCaptureResult result) {
                        Log.d(LOG_TAG, "Image taken");
                    }
                };

                new MediaActionSound().play(MediaActionSound.SHUTTER_CLICK);
                session.capture(captureBuilder.build(), captureListener, handler);
            } catch (CameraAccessException e) {
                e.printStackTrace();
            }
        }

        /**
         * {@inheritDoc}
         */
        @Override
        public void onConfigureFailed(CameraCaptureSession session) {
            Log.d(LOG_TAG, "Picture Session: configured Failed");
        }
    };

    @SuppressWarnings("NullableProblems")
    private final CameraCaptureSession.CaptureCallback previewCaptureCallback = new CameraCaptureSession.CaptureCallback() {
        /**
         * {@inheritDoc}
         */
        @Override
        public void onCaptureCompleted(CameraCaptureSession session, CaptureRequest request,
                TotalCaptureResult result) {
        }

        /**
         * {@inheritDoc}
         */
        @Override
        public void onCaptureFailed(CameraCaptureSession session, CaptureRequest request,
                CaptureFailure failure) {
            super.onCaptureFailed(session, request, failure);
            Log.d(LOG_TAG, "Focus Lock Unsuccessful");
        }
    };
    ImageReader.OnImageAvailableListener pictureReadCallback = new ImageReader.OnImageAvailableListener() {
        /**
         * {@inheritDoc}
         */
        @Override
        public void onImageAvailable(ImageReader reader) {
            Log.d(LOG_TAG, "fetching Image");
            final Bitmap snapshot = fetchSnapshot(reader);
            Handler mainHandler = new Handler(context.getMainLooper());
            mainHandler.post(new Runnable() {
                /**
                 * {@inheritDoc}
                 */
                @Override
                public void run() {
                    if (cameraListener != null) {
                        cameraListener.onSnapshot(snapshot);
                    }
                }
            });
        }

        private Bitmap fetchSnapshot(ImageReader reader) {
            Bitmap snapshot = null;
            Image image = null;
            try {
                image = reader.acquireNextImage();
                if (image == null) {
                    Log.e(LOG_TAG, "pictureReader did not return an image");
                    return null;
                }
                ByteBuffer buffer = image.getPlanes()[0].getBuffer();
                byte[] data = new byte[buffer.capacity()];
                buffer.get(data);

                snapshot = getExifRotatedBitmap(data);

            } finally {
                if (image != null) {
                    image.close();
                }
            }
            return snapshot;
        }
    };

    private enum SessionType {
        PREVIEW, SNAPSHOT;
    }

    private CameraDevice camera = null;
    private final Camera2Preview previewView;
    private CameraCharacteristics characteristics;
    private String cameraId = null;
    private Surface previewSurface;
    private CameraCaptureSession captureSession;
    private int surfaceRotation = 0;
    private int sensorRotation = 0;
    private ViewSize displaySize = new ViewSize();
    private final Semaphore openCloseLock = new Semaphore(1);
    private ImageReader frameReader = null;
    private ImageReader pictureReader = null;
    private FrameCaptureCallback frameCaptureCallback = null;
    private byte[] frameBytes = null;
    private int focusMode = CaptureRequest.CONTROL_AF_MODE_CONTINUOUS_PICTURE;

    private HandlerThread captureThread = null;
    private Handler captureHandler = null;

    /**
     * Constructor for the camera2 implementation
     *
     * @param previewView
     *            preview View
     * @param context
     *            application context
     * @param listener
     *            camera listener
     */
    public Camera2Impl(Context context, CameraListener listener) {
        super(context, listener);

        previewView = new Camera2Preview(context);
        previewView.setSurfaceTextureListener(this);
    }

    private void closeActiveSession() {
        if (captureSession != null) {
            Log.d(LOG_TAG, "Clearing active capture session");
            captureSession.close();
            captureSession = null;
        }
    }

    private void closeFrameReader() {
        if (frameReader != null) {
            Log.d(LOG_TAG, "Clearing Frame reader");
            frameReader.close();
            frameReader = null;
        }
    }

    private void closePictureReader() {
        if (pictureReader != null) {
            Log.d(LOG_TAG, "Clearing Picture reader");
            pictureReader.close();
            pictureReader = null;
        }
    }

    private void closePreviewSurface() {
        if (previewSurface != null) {
            Log.d(LOG_TAG, "Clearing Preview Surface");
            previewSurface.release();
            previewSurface = null;
        }
    }

    private void stopCaptureThread() {
        if (captureHandler != null) {
            captureHandler.getLooper().quit();
            Log.d(LOG_TAG, "Wait for capture thread");
            try {
                captureThread.join();
                Log.d(LOG_TAG, "capture thread joined");

            } catch (InterruptedException e) {
                Log.w(LOG_TAG, "Interrupted while waiting for capture thread to finish");
                Thread.currentThread().interrupt();
            }
            captureHandler = null;
            captureThread = null;
        }
    }

    private void wrapUpCamera() {
        if (camera != null) {
            camera.close();
            camera = null;
            Log.i(LOG_TAG, "Camera Wrapped");
        }
    }

    private void startCaptureThread() {
        if (captureHandler == null) {
            captureThread = new HandlerThread("Camera2Picture");
            captureThread.start();
            captureHandler = new Handler(captureThread.getLooper());
        }
    }

    /**
     * clean up method for camera actor
     */
    public void wrapUp() {
        super.wrapUp();

        clear();
        stopCaptureThread();

        Log.i(LOG_TAG, "Camera Wrapping");
        try {
            openCloseLock.acquire();
            closePreviewSurface();
            closeActiveSession();
            closeFrameReader();
            closePictureReader();
            wrapUpCamera();
            flashLightOn = false;
            focusMode = CaptureRequest.CONTROL_AF_MODE_CONTINUOUS_PICTURE;

        } catch (InterruptedException e) {
            throw new RuntimeException("Interrupted while trying to lock camera closing.", e);
        } finally {
            openCloseLock.release();
        }
    }

    private static String getFrontFacingCameraId(CameraManager cManager)
            throws CameraAccessException {
        for (final String cameraId : cManager.getCameraIdList()) {
            CameraCharacteristics characteristics = cManager.getCameraCharacteristics(cameraId);
            int cOrientation = characteristics.get(CameraCharacteristics.LENS_FACING);
            if (cOrientation == CameraCharacteristics.LENS_FACING_BACK)
                return cameraId;
        }
        return null;
    }

    /**
     * Initializes the camera
     *
     * @param display
     *            window display
     * @return true if initialized, false otherwise
     */
    public boolean initialize(Display display) {
        try {
            if (!openCloseLock.tryAcquire(2500, TimeUnit.MILLISECONDS)) {
                throw new RuntimeException("Time out waiting to lock camera opening.");
            }

            if (camera != null) {
                Log.d(LOG_TAG, "already initializing or initialized");
                return true;
            }
            startCaptureThread();
            DisplayMetrics metrics = new DisplayMetrics();
            display.getMetrics(metrics);
            displaySize.set(metrics.widthPixels, metrics.heightPixels);

            CameraManager cameraManager = (CameraManager) context
                    .getSystemService(Context.CAMERA_SERVICE);
            cameraId = getFrontFacingCameraId(cameraManager);

            // get the camera information and camera parameters
            characteristics = cameraManager.getCameraCharacteristics(cameraId);
            calcRotationParameters(display);

            int[] supportedModes = characteristics
                    .get(CameraCharacteristics.CONTROL_AF_AVAILABLE_MODES);
            for (int i = 0; i < supportedModes.length; i++) {
                if (supportedModes[i] == CaptureRequest.CONTROL_AF_MODE_CONTINUOUS_PICTURE) {
                    continuousFocusSupported = true;
                    break;
                }
            }

            previewSize = pickPreviewSize(display);
            pictureSize = pickPictureSize();
            Log.i(LOG_TAG, "Orientation: " + viewRotation + " previewSize: "
                    + previewSize.width + "X" + previewSize.height + " pictureSize: "
                    + pictureSize.width + "X" + pictureSize.height);

            cameraManager.openCamera(cameraId, cameraDeviceStateCallback, null);
            updateCoordinateSpaces();
            calcSurfaceSizeAndRatio(display);

            // set the aspect ratio
            int orientation = context.getResources().getConfiguration().orientation;
            if (orientation == Configuration.ORIENTATION_LANDSCAPE) {
                previewView.setAspectRatio(previewSize.width, previewSize.height);
            } else {
                previewView.setAspectRatio(previewSize.height, previewSize.width);
            }
            return true;
        } catch (CameraAccessException e) {
            Log.e(LOG_TAG, "Failed to retrieve camera characteristics", e);

        } catch (InterruptedException exp) {
            exp.printStackTrace();
            Log.e(LOG_TAG, "Interrupted");
        }
        return false;
    }

    private void calcSurfaceSizeAndRatio(Display display) {
        float horizontalRatio;
        float verticalRatio;
        if (viewRotation == 90 || viewRotation == 270) {
            horizontalRatio = (float) previewSize.width / (float) displaySize.height;
            verticalRatio = (float) previewSize.height / (float) displaySize.width;
            surfaceViewRatio = Math.max(horizontalRatio, verticalRatio);
            int newWidth = (int) (previewSize.height / surfaceViewRatio);
            int newHeight = (int) (previewSize.width / surfaceViewRatio);
            surfaceSize = new ViewSize(newWidth, newHeight);

        } else {
            horizontalRatio = (float) previewSize.width / (float) displaySize.width;
            verticalRatio = (float) previewSize.height / (float) displaySize.height;
            surfaceViewRatio = Math.max(horizontalRatio, verticalRatio);
            int newWidth = (int) (previewSize.width / surfaceViewRatio);
            int newHeight = (int) (previewSize.height / surfaceViewRatio);
            surfaceSize = new ViewSize(newWidth, newHeight);
        }

    }

    private void startPreview() {
        if (null == camera || !previewView.isAvailable()) {
            // Log.d(LOG_TAG, "Camera View not yet ready");
            return;
        }

        if (previewSurface == null) {
            SurfaceTexture previewTexture = previewView.getSurfaceTexture();
            previewTexture.setDefaultBufferSize(previewSize.width, previewSize.height);
            previewSurface = new Surface(previewTexture);
        }

        ArrayList<Surface> surfaces = new ArrayList<Surface>();
        surfaces.add(previewSurface);
        if (frameReader != null) {
            Log.d(LOG_TAG, "Adding frame reader");
            surfaces.add(frameReader.getSurface());
        }

        try {
            camera.createCaptureSession(surfaces, previewSessionCallback, null);

        } catch (CameraAccessException e) {
            Log.e(LOG_TAG, "Failed to finish starting camera actor", e);
            wrapUp();
        }
    }

    private void onPreviewSessionCreated(CameraCaptureSession session) throws CameraAccessException {
        Log.i(LOG_TAG, "Preview session: configured");
        captureSession = session;

        final CaptureRequest.Builder captureSessionBuilder = camera
                .createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW);
        captureSessionBuilder.addTarget(previewSurface);
        if (frameReader != null) {
            Log.d(LOG_TAG, "Adding frame reader");
            captureSessionBuilder.addTarget(frameReader.getSurface());
        }
        setSessionParameter(captureSessionBuilder, SessionType.PREVIEW);

        if (flashLightOn) {
            captureSessionBuilder.set(CaptureRequest.FLASH_MODE, CameraMetadata.FLASH_MODE_TORCH);
        }
        captureSession.setRepeatingRequest(captureSessionBuilder.build(), previewCaptureCallback,
                null);
    }

    private void setSessionParameter(CaptureRequest.Builder builder, SessionType type) {
        if (flashLightOn) {
            Log.d(LOG_TAG, "turning on torch");
            builder.set(CaptureRequest.FLASH_MODE, CameraMetadata.FLASH_MODE_TORCH);
        }

        if (type == SessionType.PREVIEW) {
            builder.set(CaptureRequest.CONTROL_AWB_LOCK, false);

            int finalFocusMode = focusMode;
            if (focusMode == CaptureRequest.CONTROL_AF_MODE_CONTINUOUS_PICTURE
                    && !continuousFocusSupported) {
                finalFocusMode = CaptureRequest.CONTROL_AF_MODE_MACRO;
            }
            //Log.d(LOG_TAG, "Focus mode: " + finalFocusMode);
            builder.set(CaptureRequest.CONTROL_AF_MODE, finalFocusMode);
            builder.set(CaptureRequest.CONTROL_AF_TRIGGER, CaptureRequest.CONTROL_AF_TRIGGER_CANCEL);

        } else {
            builder.set(CaptureRequest.JPEG_ORIENTATION, viewRotation);
            builder.set(CaptureRequest.CONTROL_MODE, CameraMetadata.CONTROL_MODE_AUTO);
        }
    }

    private void calcRotationParameters(Display display) {
        int displayRotation;
        surfaceRotation = display.getRotation();
        switch (surfaceRotation) {
        case Surface.ROTATION_0:
            displayRotation = 0;
            break;
        case Surface.ROTATION_90:
            displayRotation = 90;
            break;
        case Surface.ROTATION_180:
            displayRotation = 180;
            break;
        case Surface.ROTATION_270:
            displayRotation = 270;
            break;
        default:
            throw new RuntimeException("Unexpected rotation value");
        }

        sensorRotation = characteristics.get(CameraCharacteristics.SENSOR_ORIENTATION);
        Log.d(LOG_TAG, "camera sensor: " + sensorRotation + " display: " + displayRotation);
        viewRotation = (sensorRotation - displayRotation + 360) % 360;
    }

    private void updateSurfaceDisplay(int viewWidth, int viewHeight) {
        if (previewView.isAvailable() == false) {
            return;
        }
        Log.d(LOG_TAG, "updateSurfaceDisplay");
        int sensorWidth = previewSize.width;
        int sensorHeight = previewSize.height;
        if(sensorRotation == 90 || sensorRotation == 270) {
            sensorWidth = previewSize.height;
            sensorHeight = previewSize.width;
        }
        int rotate = 0;
        if(Surface.ROTATION_90 == surfaceRotation) {
            rotate = 270;
        } else if(Surface.ROTATION_270 == surfaceRotation) {
            rotate = 90;
        } else if(Surface.ROTATION_180 == surfaceRotation) {
            rotate = 180;
        }
        Matrix matrix = new Matrix();
        RectF viewRect = new RectF(0, 0, viewWidth, viewHeight);
        RectF bufferRect = new RectF(0, 0, sensorWidth, sensorHeight);
        float centerX = viewRect.centerX();
        float centerY = viewRect.centerY();
        if (rotate == 90 || rotate == 270) {
            bufferRect.offset(centerX - bufferRect.centerX(), centerY - bufferRect.centerY());
            matrix.setRectToRect(viewRect, bufferRect, Matrix.ScaleToFit.FILL);
            float scale = Math.max((float) viewHeight / sensorWidth, (float) viewWidth
                    / sensorHeight);
            matrix.postScale(scale, scale, centerX, centerY);

        }
        if (rotate != 0) {
            matrix.postRotate(rotate, centerX, centerY);
        }
        previewView.setTransform(matrix);
        previewView.setX(displaySize.width / 2 - centerX);
        previewView.setY(displaySize.height / 2 - centerY);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onSurfaceTextureAvailable(SurfaceTexture surface, int width, int height) {
        updateSurfaceDisplay(width, height);
        startPreview();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width, int height) {
        updateSurfaceDisplay(width, height);
        startPreview();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
        return false;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onSurfaceTextureUpdated(SurfaceTexture surface) {
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean toggleFlash() {
        flashLightOn = !flashLightOn;
        removeCaptureCallbacks();
        return false;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void toggleMacroFocusMode() {
        focusMode = (focusMode == CaptureRequest.CONTROL_AF_MODE_CONTINUOUS_PICTURE) ? CaptureRequest.CONTROL_AF_MODE_MACRO
                : CaptureRequest.CONTROL_AF_MODE_CONTINUOUS_PICTURE;
        removeCaptureCallbacks();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean isMacroFocusMode() {
        return (focusMode == CaptureRequest.CONTROL_AF_MODE_MACRO);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public ViewSize getMatchingPreviewSize(float aspectRatioMin, float aspectRatioMax) {
        Size[] previewSizes = characteristics.get(
                CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP).getOutputSizes(
                SurfaceTexture.class);
        Size selected = null;
        Size selectedUnMatched = null;
        for (Size previewSize : previewSizes) {
            // Log.d(LOG_TAG, "Supported preview size: " + previewSize.getWidth() + " X "
            // + previewSize.getHeight());
            if (previewSize.getWidth() >= MIN_WIDTH && previewSize.getHeight() >= MIN_HEIGHT) {
                float aspect = (float) previewSize.getWidth() / (float) previewSize.getHeight();
                if (selected == null
                        || (selected.getWidth() >= previewSize.getWidth() || selected.getHeight() > previewSize
                                .getHeight())) {
                    selectedUnMatched = previewSize;
                    if (aspect >= aspectRatioMin && aspect <= aspectRatioMax) {
                        selected = previewSize;
                    }
                }
            }
        }
        if (selected == null) {
            selected = selectedUnMatched;
            Log.i(LOG_TAG, "PreviewSize: Assigning size outside max min but near to OCR limits");
        }

        // No matching aspect ratios, just select the first picture size
        if (selected == null) {
            selected = previewSizes[0];
            Log.i(LOG_TAG, "PreviewSize: Assigning first size");
        }
        return new ViewSize(selected.getWidth(), selected.getHeight());
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public ViewSize getMatchingPictureSize(float aspectRatioMin, float aspectRatioMax) {
        Size[] pictureSizes = characteristics.get(
                CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP).getOutputSizes(
                ImageFormat.JPEG);
        Size selected = null;
        Size selectedUnMatched = null;
        for (Size pictureSize : pictureSizes) {
            // Log.d(LOG_TAG, "Supported picture size: " + pictureSize.getWidth() + " X "
            // + pictureSize.getHeight());
            if (pictureSize.getWidth() > MIN_WIDTH && pictureSize.getHeight() > MIN_HEIGHT) {
                float aspect = (float) pictureSize.getWidth() / (float) pictureSize.getHeight();
                if (selected == null
                        || (pictureSize.getWidth() >= selected.getWidth() || pictureSize
                                .getHeight() > selected.getHeight())) {
                    if (selectedUnMatched == null
                            || (pictureSize.getWidth() >= selectedUnMatched.getWidth() || pictureSize
                                    .getHeight() > selectedUnMatched.getHeight())) {
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
            selected = pictureSizes[0];
        }

        return new ViewSize(selected.getWidth(), selected.getHeight());
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void removeCaptureCallbacks() {
        closeActiveSession();
        closeFrameReader();
        closePictureReader();

        startPreview();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void startFrameCycle() {
        Log.d(LOG_TAG, "start frame Cycle");
        closeActiveSession();
        createFrameReader();
        startPreview();
    }
    private void createFrameReader() {
        if (frameReader == null) {
            Log.d(LOG_TAG, "Creating frameReader");
            frameReader = ImageReader.newInstance(previewSize.width, previewSize.height,
                    ImageFormat.YUV_420_888, 2);
            frameCaptureCallback = new FrameCaptureCallback();
            frameReader.setOnImageAvailableListener(frameCaptureCallback, captureHandler);

            int frameSize = (previewSize.width * previewSize.height * 3) / 2;
            if (frameBytes == null || frameBytes.length != frameSize) {
                frameBytes = new byte[frameSize];
            }
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void startSnapshot() {
        try {
            Log.d(LOG_TAG, "Taking snapshot");
            pictureReader = ImageReader.newInstance(pictureSize.width, pictureSize.height,
                    ImageFormat.JPEG, 1);
            List<Surface> outputSurfaces = new ArrayList<Surface>(2);
            outputSurfaces.add(pictureReader.getSurface());
            outputSurfaces.add(previewSurface);

            pictureReader.setOnImageAvailableListener(pictureReadCallback, captureHandler);

            PictureSessionCallback pictureSessionCallback = new PictureSessionCallback(
                    pictureReader, captureHandler);
            camera.createCaptureSession(outputSurfaces, pictureSessionCallback, captureHandler);

        } catch (CameraAccessException e) {
            e.printStackTrace();
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

    /**
     * gets the status of camera2 API
     *
     * @param context
     *            application context
     * @return true if fully supported, false otherwise
     */
    public static boolean isFullySupported(Context context) {
        CameraManager manager = (CameraManager) context.getSystemService(Context.CAMERA_SERVICE);
        try {
            String id = getFrontFacingCameraId(manager);
            CameraCharacteristics characteristics = manager.getCameraCharacteristics(id);
            int implimentationLevel = characteristics
                    .get(CameraCharacteristics.INFO_SUPPORTED_HARDWARE_LEVEL);
            Log.d(LOG_TAG, "Camera2 API support: " + implimentationLevel);
            if (implimentationLevel == CameraCharacteristics.INFO_SUPPORTED_HARDWARE_LEVEL_FULL) {
                return true;
            }
        } catch (CameraAccessException e) {
            e.printStackTrace();
        }
        return false;

    }
}
