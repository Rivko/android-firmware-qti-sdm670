/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.panoramaui.hardware;

import java.nio.ByteBuffer;
import java.util.ArrayDeque;
import java.util.ArrayList;

import android.annotation.TargetApi;
import android.app.Activity;
import android.content.Context;
import android.graphics.ImageFormat;
import android.graphics.Point;
import android.graphics.PointF;
import android.graphics.SurfaceTexture;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.CaptureFailure;
import android.hardware.camera2.CaptureRequest;
import android.hardware.camera2.TotalCaptureResult;
import android.media.Image;
import android.media.ImageReader;
import android.os.Build;
import android.util.Log;
import android.util.Size;
import android.view.Surface;

import com.qualcomm.qti.panoramaui.R;
import com.qualcomm.qti.panoramaui.AeAfUIHandler;
import com.qualcomm.qti.panoramaui.CameraPreviewView;
import com.qualcomm.qti.panoramaui.data.AppError;
import com.qualcomm.qti.panoramaui.hardware.AeAfHandler.AeAfType;
import com.qualcomm.qti.panoramaui.preferences.CameraPreferenceManager;

/**
 * This class handles all the camera interactions, using the new Lollipop Camera2 APIs
 */
@TargetApi(Build.VERSION_CODES.LOLLIPOP)
public class CameraActorCamera2 extends CameraActor {
    private static final String TAG = CameraActorCamera2.class.getSimpleName();
    private static final int PREVIEW_MAX_SIZE = 1920 * 1080;

    private boolean started;
    private CameraCharacteristics characteristics;
    private int cameraId;
    private CameraDevice camera;
    private CameraCaptureSession captureSession;
    private CameraPreviewView cameraView;
    private Point previewSize = new Point();
    private int cameraDisplayOrientation;
    private Surface previewSurface;
    private ImageReader pictureReader;
    private KeyFrameCaptureCallback pictureReaderCallback;
    private CaptureRequest pictureCaptureRequest;

    private AeAfCamera2 aeAfHandler = null;

    @SuppressWarnings("NullableProblems")
    private final CameraCaptureSession.CaptureCallback previewCaptureCallback = new CameraCaptureSession.CaptureCallback() {

        @Override
        public void onCaptureCompleted(CameraCaptureSession session, CaptureRequest request,
                TotalCaptureResult result) {
            if (aeAfHandler != null) {
                aeAfHandler.processResult(result);
            }
        }

        @Override
        public void onCaptureFailed(CameraCaptureSession session, CaptureRequest request,
                CaptureFailure failure) {
            super.onCaptureFailed(session, request, failure);
            Log.d(TAG, "Focus Lock Unsuccessful");
        }
    };

    /**
     * Construct a {@link CameraActorCamera2} instance
     *
     * @param parentActivity
     *            {@link Activity} which owns this {@link CameraActorCamera2} instance
     * @param aeAfEventListener
     *            AeAFEvent listener
     */
    public CameraActorCamera2(Activity parentActivity, AeAfUIHandler aeAfUIHandler) {
        super(parentActivity, aeAfUIHandler);
    }

    /** {@inheritDoc} */
    @Override
    public AppError startCamera(boolean zslEnabled) {
        Log.i(TAG, "Starting camera actor");
        started = true;
        zslOn = zslEnabled;
        cameraId = CameraPreferenceManager.getSelectedCameraId(parentActivity);

        try {
            CameraManager cameraManager = (CameraManager) parentActivity
                    .getSystemService(Context.CAMERA_SERVICE);
            characteristics = cameraManager.getCameraCharacteristics(Integer.toString(cameraId));
            if (zslEnabled && !checkZSLSupported()) {
                Log.i(TAG, "no zsl support, falling back to no-zsl mode");
                zslOn = false;
            }
            cameraManager.openCamera(Integer.toString(cameraId), cameraDeviceStateCallback, null);
        } catch (CameraAccessException e) {
            Log.e(TAG, "Failed to retrieve camera characteristics", e);
            return AppError.CAMERA_FAILED;
        }

        final boolean facingFront = (characteristics.get(CameraCharacteristics.LENS_FACING) == CameraCharacteristics.LENS_FACING_FRONT);
        cameraDisplayOrientation = getCameraDisplayOrientation(
                characteristics.get(CameraCharacteristics.SENSOR_ORIENTATION), facingFront);
        Log.d(TAG, "Camera orientation: " + cameraDisplayOrientation);

        Size pictureSize = CameraPreferenceManager.getCamera2PictureSize(parentActivity, cameraId,
                characteristics);
        pictureReader = ImageReader.newInstance(pictureSize.getWidth(), pictureSize.getHeight(),
                ImageFormat.JPEG, 2);
        pictureReaderCallback = new KeyFrameCaptureCallback();
        pictureReader.setOnImageAvailableListener(pictureReaderCallback, null);
        Log.i(TAG, "Set picture size to [" + pictureSize.getWidth() + "x" + pictureSize.getHeight()
                + "]");
        Size[] supportedPreviewSizes = characteristics.get(
                CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP).getOutputSizes(
                SurfaceTexture.class);
        previewSize = selectPreviewSize(supportedPreviewSizes, pictureSize);
        Log.i(TAG, "Set preview size to [" + previewSize.x + "x" + previewSize.y + "]");

        cameraView = (CameraPreviewView) parentActivity.findViewById(R.id.camera_preview_view);
        // N.B. Camera2 returns frames via SurfaceTexture which already account for sensor rotation
        final int displayOrientation = getCameraDisplayOrientation(0, facingFront);
        cameraView.setPreviewFrameRotation(displayOrientation);
        cameraView.setPreviewFrameSize(getWidth(), getHeight());

        boolean shouldLock3A = CameraPreferenceManager.getEnable3A(parentActivity);
        aeAfHandler = new AeAfCamera2(aeAfUIHandler, characteristics, shouldLock3A, this);

        return AppError.NONE;
    }

    private void onCameraStarted(CameraDevice cameraDevice) throws CameraAccessException {
        camera = cameraDevice;

        SurfaceTexture previewTexture = cameraView.getSurfaceTexture();
        previewTexture.setDefaultBufferSize(previewSize.x, previewSize.y);
        previewSurface = new Surface(previewTexture);

        ArrayList<Surface> surfaces = new ArrayList<Surface>();
        surfaces.add(previewSurface);
        surfaces.add(pictureReader.getSurface());
        camera.createCaptureSession(surfaces, cameraCaptureSessionStateCallback, null);
    }

    private void onCaptureSessionCreated(CameraCaptureSession session) throws CameraAccessException {
        captureSession = session;
        issuePreviewCaptureRequest(AeAfType.CONTINUOUS);
    }

    private void issuePreviewCaptureRequest(AeAfType aeAfType) throws CameraAccessException {
        CaptureRequest.Builder previewCaptureRequestBuilder = camera
                .createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW);
        previewCaptureRequestBuilder.addTarget(previewSurface);
        CameraPreferenceManager.applyCamera2CameraPreferences(parentActivity, cameraId,
                characteristics, previewCaptureRequestBuilder);

        aeAfHandler.configure(previewCaptureRequestBuilder, aeAfType);

        captureSession.setRepeatingRequest(previewCaptureRequestBuilder.build(),
                previewCaptureCallback, null);

        // Create a new picture CaptureRequest which is compatible with the preview CaptureRequest
        if (zslOn && checkZSLSupported()) {
            CaptureRequest.Builder pictureCaptureRequestBuilder = camera
                    .createCaptureRequest(CameraDevice.TEMPLATE_ZERO_SHUTTER_LAG);
            pictureCaptureRequestBuilder.addTarget(pictureReader.getSurface());
            CameraPreferenceManager.applyCamera2CameraPreferences(parentActivity, cameraId,
                    characteristics, pictureCaptureRequestBuilder);
            int jpegQuality = CameraPreferenceManager.getImageCompressLevel(parentActivity);
            pictureCaptureRequestBuilder.set(CaptureRequest.JPEG_QUALITY, (byte) jpegQuality);
            Log.i(TAG, "Set JPEG quality to [" + jpegQuality + "]");
            pictureCaptureRequestBuilder.set(CaptureRequest.JPEG_ORIENTATION,
                    cameraDisplayOrientation);
            Log.i(TAG, "Set JPEG orientation to [" + cameraDisplayOrientation + "]");
            pictureCaptureRequestBuilder.set(CaptureRequest.CONTROL_AE_LOCK,
                    aeAfHandler.is3aLocked());
            pictureCaptureRequestBuilder.set(CaptureRequest.CONTROL_AWB_LOCK,
                    aeAfHandler.is3aLocked());
            pictureCaptureRequest = pictureCaptureRequestBuilder.build();
        } else {
            Log.d(TAG, "No ZSL Support");
        }
    }

    /** {@inheritDoc} */
    @Override
    public void wrapUp() {
        if (captureSession != null) {
            captureSession.close();
            captureSession = null;
        }

        if (camera != null) {
            camera.close();
            camera = null;
        }

        if (pictureReader != null) {
            pictureReader.close();
            pictureReader = null;
        }

        if (previewSurface != null) {
            previewSurface.release();
            previewSurface = null;
        }

        if (characteristics != null) {
            characteristics = null;
        }
        if (aeAfHandler != null) {
            aeAfHandler.clear();
            aeAfHandler = null;
        }

        previewSize.set(0, 0);
        cameraView = null;

        pictureCaptureRequest = null;

        Log.i(TAG, "Camera actor stopped");
        started = false;
    }

    /** {@inheritDoc} */
    @Override
    public int getCameraOrientation() {
        return cameraDisplayOrientation;
    }

    /** {@inheritDoc} */
    @Override
    public int getWidth() {
        return (cameraDisplayOrientation == 0 || cameraDisplayOrientation == 180 ? previewSize.x
                : previewSize.y);
    }

    /** {@inheritDoc} */
    @Override
    public int getHeight() {
        return (cameraDisplayOrientation == 0 || cameraDisplayOrientation == 180 ? previewSize.y
                : previewSize.x);
    }

    /** {@inheritDoc} */
    @Override
    public float getFocalLength() {
        return characteristics.get(CameraCharacteristics.LENS_INFO_AVAILABLE_FOCAL_LENGTHS)[0];
    }

    /** {@inheritDoc} */
    @Override
    public void onCapture(boolean start) {
        if (camera == null) {
            Log.w(TAG, "onCapture but camera is null");
            return;
        }
        if (!aeAfHandler.is3aLockSupported() || aeAfHandler.is3aLocked() == start) {
            return;
        }
        if (start) {
            aeAfHandler.clear();
        }

        AeAfType aeAftype = (start ? AeAfType.LOCK_AND_FOCUS : AeAfType.CONTINUOUS);
        try {
            issuePreviewCaptureRequest(aeAftype);
        } catch (CameraAccessException e) {
            Log.e(TAG, "Failed reissuing preview capture request for 3A lock change", e);
            wrapUp();
        }
    }

    /** {@inheritDoc} */
    @Override
    public void setAeAfRegion(PointF touch) {
        if (camera == null) {
            Log.w(TAG, "setAeAfRegion but camera is null");
            return;
        }
        if (aeAfHandler.is3aLocked()) {
            Log.d(TAG, "AE AF locked");
            return;
        }
        if (!isTouchValid(touch.y, cameraView.getHeight())) {
            return;
        }
        aeAfHandler.setRegionAt(touch, cameraView.getWidth(), cameraView.getHeight(),
                cameraDisplayOrientation);
        try {
            issuePreviewCaptureRequest(AeAfType.LOCK_AND_FOCUS_REGION);
        } catch (CameraAccessException e) {
            Log.e(TAG, "Failed reissuing preview capture request for 3A lock change", e);
            wrapUp();
        }

    }

    /** {@inheritDoc} */
    @Override
    public void captureKeyFrame(final OnKeyFrameCapturedListener jpegCallback) {
        handler.post(new Runnable() {
            @Override
            public void run() {
                try {
                    if (pictureReaderCallback == null || captureSession == null) {
                        throw new CameraAccessException(CameraAccessException.CAMERA_DISCONNECTED,
                                "Received keyframe capture request after camera actor closed");
                    }

                    pictureReaderCallback.addKeyFrameRequest(jpegCallback);
                    captureSession.capture(pictureCaptureRequest, null, null);
                } catch (CameraAccessException e) {
                    jpegCallback.onKeyFrameCaptured(null);
                    Log.e(TAG, "Failed to issue keyframe capture request", e);
                }
            }
        });
    }

    /**
     * See {@link #selectPreviewSize(Point[], Point)}
     */
    private Point selectPreviewSize(Size[] supportedPreviewSizes, Size pictureSize) {
        ArrayList<Point> supportedSizes = new ArrayList<Point>();
        for (Size size : supportedPreviewSizes) {
            final int width = size.getWidth();
            final int height = size.getHeight();

            if (width * height > PREVIEW_MAX_SIZE) {
                continue;
            }

            supportedSizes.add(new Point(width, height));
        }

        Point[] supportedSizesArray = new Point[supportedSizes.size()];
        return selectPreviewSize(supportedSizes.toArray(supportedSizesArray),
                new Point(pictureSize.getWidth(), pictureSize.getHeight()));
    }

    private final CameraDevice.StateCallback cameraDeviceStateCallback = new CameraDevice.StateCallback() {
        @Override
        public void onOpened(CameraDevice camera) {
            if (!started) {
                Log.w(TAG, "Camera actor is closed, ignoring CameraDevice onOpened");
                return;
            }

            Log.i(TAG, "CameraDevice opened");
            try {
                onCameraStarted(camera);
            } catch (CameraAccessException e) {
                Log.e(TAG, "Failed to finish starting camera actor", e);
                wrapUp();
            }
        }

        @Override
        public void onError(CameraDevice camera, int error) {
            Log.e(TAG, "Failed to open camera device, error [" + error + "]");
            wrapUp();
        }

        @Override
        public void onDisconnected(CameraDevice camera) {
            Log.i(TAG, "CameraDevice disconnected");
            wrapUp();
        }

        @Override
        public void onClosed(CameraDevice camera) {
            Log.i(TAG, "CameraDevice closed");
        }
    };

    private final CameraCaptureSession.StateCallback cameraCaptureSessionStateCallback = new CameraCaptureSession.StateCallback() {
        @Override
        public void onConfigured(CameraCaptureSession session) {
            if (!started) {
                Log.w(TAG, "Camera actor is closed, ignoring CameraCaptureSession onConfigured");
                return;
            }

            Log.i(TAG, "Camera capture session successfully configured");
            try {
                onCaptureSessionCreated(session);
            } catch (CameraAccessException e) {
                Log.e(TAG, "Failed to start camera capture session", e);
                wrapUp();
            }
        }

        @Override
        public void onConfigureFailed(CameraCaptureSession session) {
            Log.e(TAG, "Failed to configure camera capture session");
            wrapUp();
        }

        @Override
        public void onClosed(CameraCaptureSession session) {
            Log.i(TAG, "Camera capture session closed");
        }
    };

    private static class KeyFrameCaptureCallback implements ImageReader.OnImageAvailableListener {
        private final ArrayDeque<OnKeyFrameCapturedListener> keyFrameCapturedListeners = new ArrayDeque<OnKeyFrameCapturedListener>();

        public void addKeyFrameRequest(OnKeyFrameCapturedListener listener) {
            keyFrameCapturedListeners.add(listener);
        }

        @Override
        public void onImageAvailable(ImageReader reader) {
            Image image = reader.acquireNextImage();
            OnKeyFrameCapturedListener listener = keyFrameCapturedListeners.removeFirst();

            if (image == null) {
                Log.e(TAG, "pictureReader did not return an image");
                return;
            } else if (listener == null) {
                Log.e(TAG, "No OnKeyFrameAvailableListener to receive the captured image");
                return;
            }

            ByteBuffer buf = image.getPlanes()[0].getBuffer();
            byte[] jpegData = new byte[buf.limit()];
            buf.get(jpegData);
            image.close();

            listener.onKeyFrameCaptured(jpegData);
        }
    }

    /**
     * ZSL's ability can be queried by CameraCharacteristics.INFO_SUPPORTED_HARDWARE_LEVEL. If
     * LEGACY, ZSL is not supported. If LIMITED or FULL, it is supported
     */
    private boolean checkZSLSupported() {
        boolean isZSLSupported = false;

        if (characteristics.get(CameraCharacteristics.INFO_SUPPORTED_HARDWARE_LEVEL) != CameraCharacteristics.INFO_SUPPORTED_HARDWARE_LEVEL_LEGACY) {
            isZSLSupported = true;
        }
        return isZSLSupported;
    }

    /** {@inheritDoc} */
    @Override
    public void onAutoFocusFailed() {
        try {
            issuePreviewCaptureRequest(AeAfType.CONTINUOUS);
        } catch (CameraAccessException e) {
            Log.e(TAG, "Failed reissuing preview capture request AF Failed", e);
            wrapUp();
        }
    }

    /** {@inheritDoc} */
    @Override
    public void onAutoFocusExpired() {
        try {
            issuePreviewCaptureRequest(AeAfType.CONTINUOUS);
        } catch (CameraAccessException e) {
            Log.e(TAG, "Failed reissuing preview capture request AF Expired", e);
            wrapUp();
        }
    }
    /**
     * gets the status of camera2 API ZSL support
     *
     * @param activity
     *            application activity
     * @return true if ZSL supported, false otherwise
     */
    public static boolean isZSLSupported(Activity activity) {
        CameraManager manager = (CameraManager) activity.getSystemService(Context.CAMERA_SERVICE);
        try {
            int id = CameraPreferenceManager.getSelectedCameraId(activity);
            CameraCharacteristics characteristics = manager.getCameraCharacteristics(String.valueOf(id));
            if (characteristics.get(CameraCharacteristics.INFO_SUPPORTED_HARDWARE_LEVEL) != CameraCharacteristics.INFO_SUPPORTED_HARDWARE_LEVEL_LEGACY) {
                return true;
            }
        } catch (CameraAccessException e) {
            e.printStackTrace();
        }
        return false;

    }
}
