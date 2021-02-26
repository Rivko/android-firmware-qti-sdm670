/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.clevercapture.hardware;

import java.nio.ByteBuffer;
import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.List;

import com.qualcomm.qti.clevercaptureui.R;
import com.qualcomm.qti.clevercaptureui.CameraPreviewView;
import com.qualcomm.qti.clevercaptureui.preferences.CameraPreferenceManager;

import android.annotation.TargetApi;
import android.app.Activity;
import android.content.Context;
import android.graphics.ImageFormat;
import android.graphics.Matrix;
import android.graphics.Point;
import android.graphics.Rect;
import android.graphics.RectF;
import android.graphics.SurfaceTexture;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.CameraMetadata;
import android.hardware.camera2.CaptureRequest;
import android.hardware.camera2.CaptureResult;
import android.hardware.camera2.TotalCaptureResult;
import android.hardware.camera2.params.MeteringRectangle;
import android.media.Image;
import android.media.ImageReader;
import android.os.Build;
import android.util.Log;
import android.util.Range;
import android.util.Size;
import android.view.Surface;

/**
 * This class handles all the camera interactions, using the new Lollipop Camera2 APIs
 */
@TargetApi(Build.VERSION_CODES.LOLLIPOP)
public class CameraActorCamera2 extends CameraActor {
    private static final String TAG = CameraActorCamera2.class.getSimpleName();

    private boolean started;
    private CameraCharacteristics characteristics;
    private int cameraId;
    private CameraDevice camera;
    private CameraCaptureSession captureSession;
    private CameraPreviewView cameraView;
    //private Point previewSize = new Point();
    private int cameraDisplayOrientation;
    private Surface previewSurface;
    private ImageReader pictureReader;
    private KeyFrameCaptureCallback pictureReaderCallback;
    //private CaptureRequest pictureCaptureRequest;
    private CaptureRequest.Builder pictureCaptureRequestBuilder;
    CaptureRequest.Builder previewCaptureRequestBuilder;
    private boolean isAeAwbLocked;
    private boolean isCapture;

    private Size pictureSize;
    private Size previewSize;
    private Point thumbnailSize = new Point();

    private int burstNum;
    private int burstInterval;
    private List<CaptureRequest> picCaptureRequestList = new ArrayList<CaptureRequest>();

    private float minZoom = 1;
    private float maxZoom = 1;
    private float zoom = 1;
    private float lastzoom = zoom;

    private Rect activeArraySize = new Rect();
    private Rect zoomSize = new Rect();

    private Integer iso = 800;
    private int afState = CameraMetadata.CONTROL_AF_STATE_PASSIVE_UNFOCUSED;

    private int maxRegionsAE;
    private int maxRegionsAF;
    private int maxRegionsAWB;
    private RectF roiRect = new RectF(0, 0, 1, 1);
    private Rect roiSize = new Rect();
    private Matrix matrix = new Matrix();

    private long mCaptureStartTime;

    private CameraCaptureSession.CaptureCallback mCaptureCallback =
            new CameraCaptureSession.CaptureCallback() {

        //private void process(CaptureResult result) {
        //    afState = result.get(CaptureResult.CONTROL_AF_STATE);
        //    Log.d(TAG, "process afState=" + afState);
        //}

        @Override
        public void onCaptureCompleted(CameraCaptureSession session,
                CaptureRequest request, TotalCaptureResult result) {
            int tmpAfState = result.get(CaptureResult.CONTROL_AF_STATE);
            if(tmpAfState != afState) {
                Log.d(TAG, "AF state changed from " + afState + " to " + tmpAfState);
                afState = tmpAfState;

                if( isCapture &&
                    getFocusMode() == CameraMetadata.CONTROL_AF_MODE_AUTO &&
                    afState == CameraMetadata.CONTROL_AF_STATE_FOCUSED_LOCKED ) {
                    Log.i(TAG, "AUTO AF MODE, focus done, lock focus now");
                    previewCaptureRequestBuilder.set(CaptureRequest.CONTROL_AF_TRIGGER,
                            CameraMetadata.CONTROL_AF_TRIGGER_IDLE);
                    try {
                        captureSession.setRepeatingRequest(previewCaptureRequestBuilder.build(), mCaptureCallback, null);
                    } catch (CameraAccessException e) {
                        Log.i(TAG, "setRepeatingRequest fail");
                        e.printStackTrace();
                    }
                }
            }
            //Log.d(TAG, "onCaptureCompleted afState=" + afState);

            //int afMode = result.get(CaptureResult.CONTROL_AF_MODE);
            //Log.d(TAG, "onCaptureCompleted afMode=" + afMode);

            //int afTrigger = result.get(CaptureResult.CONTROL_AF_TRIGGER);
            //Log.d(TAG, "onCaptureCompleted afTrigger=" + afTrigger);

            //MeteringRectangle[] rects = result.get(CaptureResult.CONTROL_AF_REGIONS);
            //for(MeteringRectangle rect : rects) {
            //    Log.d(TAG, rect.toString());
            //}

            //process(result);
        }

        @Override
        public void onCaptureProgressed(CameraCaptureSession session,
                CaptureRequest request, CaptureResult partialResult) {
            //afState = partialResult.get(CaptureResult.CONTROL_AF_STATE);
            //Log.d(TAG, "onCaptureProgressed afState=" + afState);
            //process(partialResult);
        }

        @Override
        public void onCaptureStarted(CameraCaptureSession session,
                CaptureRequest request, long timestamp, long frameNumber) {
            //Log.d(TAG, "onCaptureStarted timestamp=" + timestamp + " frameNumber=" + frameNumber);
        }

    };

    /**
     * Construct a {@link CameraActorCamera2} instance
     * @param parentActivity {@link Activity} which owns this {@link CameraActorCamera2} instance
     */
    public CameraActorCamera2(Activity parentActivity) {
        super(parentActivity);

        isCapture = false;
    }

    /** {@inheritDoc} */
    @Override
    public boolean startCamera() {
        Log.i(TAG, "Starting camera actor");
        started = true;

        cameraId = CameraPreferenceManager.getSelectedCameraId(parentActivity);
        burstNum = CameraPreferenceManager.getBurstNumber(parentActivity);
        burstInterval = CameraPreferenceManager.getBurstInterval(parentActivity);

        try {
            CameraManager cameraManager = (CameraManager)parentActivity.getSystemService(
                    Context.CAMERA_SERVICE);
            characteristics = cameraManager.getCameraCharacteristics(Integer.toString(cameraId));
            cameraManager.openCamera(Integer.toString(cameraId), cameraDeviceStateCallback, null);
        } catch (CameraAccessException e) {
            Log.e(TAG, "Failed to retrieve camera characteristics", e);
            return false;
        }

        final boolean facingFront = (characteristics.get(CameraCharacteristics.LENS_FACING) ==
                CameraCharacteristics.LENS_FACING_FRONT);
        cameraDisplayOrientation = getCameraDisplayOrientation(
                characteristics.get(CameraCharacteristics.SENSOR_ORIENTATION), facingFront);
        matrix.setRotate((float)cameraDisplayOrientation, 0.5f, 0.5f);

        pictureSize = CameraPreferenceManager.getCamera2PictureSize(parentActivity, cameraId,
                characteristics);
        pictureReader = ImageReader.newInstance(pictureSize.getWidth(), pictureSize.getHeight(),
                ImageFormat.JPEG, 2);
        pictureReaderCallback = new KeyFrameCaptureCallback();
        pictureReader.setOnImageAvailableListener(pictureReaderCallback, null);
        Log.i(TAG, "Set picture size to [" + pictureSize.getWidth() + "x" +
                pictureSize.getHeight() + "]");

        previewSize = CameraPreferenceManager.getCamera2PreviewSize(parentActivity, cameraId,
                characteristics);
        Size[] supportedPreviewSizes = characteristics.get(CameraCharacteristics.
                SCALER_STREAM_CONFIGURATION_MAP).getOutputSizes(SurfaceTexture.class);
        for(Size option : supportedPreviewSizes) {
            Log.d(TAG, "preview size supports " + option.getWidth() + " X " + option.getHeight() + " ratio " + (float)option.getHeight()/(float)option.getWidth());
        }
        //previewSize = selectPreviewSize(supportedPreviewSizes, pictureSize);
        //previewSize.x = 1280;
        //previewSize.y = 720;
        Log.i(TAG, "Set preview size to [" + previewSize.getWidth() + "x" + previewSize.getHeight() + "]");

        Size[] thumbnailSizes = characteristics.get(CameraCharacteristics.JPEG_AVAILABLE_THUMBNAIL_SIZES);
        thumbnailSize = selectPreviewSize(thumbnailSizes, pictureSize);
        thumbnailSize.x = 320;
        thumbnailSize.y = 240;
        Log.i(TAG, "Set thumbnail size to [" + thumbnailSize.x + "x" + thumbnailSize.y + "]");

        maxZoom = characteristics.get(CameraCharacteristics.SCALER_AVAILABLE_MAX_DIGITAL_ZOOM);
        Log.i(TAG, "maxZoom=" + maxZoom);
        activeArraySize = characteristics.get(CameraCharacteristics.SENSOR_INFO_ACTIVE_ARRAY_SIZE);
        Log.i(TAG, "activeArraySize=" + activeArraySize.toString());

        Range<Integer> isoRange = characteristics.get(CameraCharacteristics.SENSOR_INFO_SENSITIVITY_RANGE);
        Log.i(TAG, "isoRange " + isoRange.toString());

        maxRegionsAE = characteristics.get(CameraCharacteristics.CONTROL_MAX_REGIONS_AE);
        Log.i(TAG, "maxRegionsAE=" + maxRegionsAE);

        maxRegionsAF = characteristics.get(CameraCharacteristics.CONTROL_MAX_REGIONS_AF);
        Log.i(TAG, "maxRegionsAF=" + maxRegionsAF);

        maxRegionsAWB = characteristics.get(CameraCharacteristics.CONTROL_MAX_REGIONS_AWB);
        Log.i(TAG, "maxRegionsAWB=" + maxRegionsAWB);

        cameraView = (CameraPreviewView)parentActivity.findViewById(R.id.camera_preview_view);
        // N.B. Camera2 returns frames via SurfaceTexture which already account for sensor rotation
        final int displayOrientation = getCameraDisplayOrientation(0, facingFront);
        cameraView.setPreviewFrameRotation(displayOrientation);
        cameraView.setPreviewFrameSize(getWidth(), getHeight());

        isAeAwbLocked = CameraPreferenceManager.getEnableAeAWB(parentActivity);

        return true;
    }

    private void onCameraStarted(CameraDevice cameraDevice) throws CameraAccessException {
        camera = cameraDevice;

        SurfaceTexture previewTexture = cameraView.getSurfaceTexture();
        previewTexture.setDefaultBufferSize(previewSize.getWidth(), previewSize.getHeight());
        previewSurface = new Surface(previewTexture);

        ArrayList<Surface> surfaces = new ArrayList<Surface>();
        surfaces.add(previewSurface);
        surfaces.add(pictureReader.getSurface());
        camera.createCaptureSession(surfaces, cameraCaptureSessionStateCallback, null);
    }

    private void onCaptureSessionCreated(CameraCaptureSession session)
            throws CameraAccessException {
        captureSession = session;

        issuePreviewCaptureRequest(isCapture);

        pictureCaptureRequestBuilder = camera.createCaptureRequest(
                CameraDevice.TEMPLATE_ZERO_SHUTTER_LAG);
        pictureCaptureRequestBuilder.addTarget(pictureReader.getSurface());

        int jpegQuality = CameraPreferenceManager.getImageCompressLevel(parentActivity);
        pictureCaptureRequestBuilder.set(CaptureRequest.JPEG_QUALITY, (byte)jpegQuality);
        Log.i(TAG, "Set JPEG quality to [" + jpegQuality + "]");

        pictureCaptureRequestBuilder.set(CaptureRequest.JPEG_THUMBNAIL_QUALITY, (byte)jpegQuality);
        pictureCaptureRequestBuilder.set(CaptureRequest.JPEG_THUMBNAIL_SIZE, new Size(thumbnailSize.x, thumbnailSize.y));
        Log.i(TAG, "Set thumbnailSize to [" + thumbnailSize.x + "x" + thumbnailSize.y + "]");

        pictureCaptureRequestBuilder.set(CaptureRequest.JPEG_ORIENTATION, cameraDisplayOrientation);
        Log.i(TAG, "Set JPEG orientation to [" + cameraDisplayOrientation + "]");
        //pictureCaptureRequest = pictureCaptureRequestBuilder.build();

        //pictureCaptureRequestBuilder.set(CaptureRequest.SENSOR_SENSITIVITY, iso);
        //pictureCaptureRequestBuilder.set(CaptureRequest.CONTROL_SCENE_MODE, CameraMetadata.CONTROL_SCENE_MODE_SPORTS);

        for(int i = 0; i < burstNum * burstInterval; i++) {
            picCaptureRequestList.add(pictureCaptureRequestBuilder.build());
        }
    }

    private void issuePreviewCaptureRequest(boolean isCapture) throws CameraAccessException {
        Log.i(TAG, "issuePreviewCaptureRequest");

        previewCaptureRequestBuilder = camera.createCaptureRequest(
                CameraDevice.TEMPLATE_PREVIEW);
        previewCaptureRequestBuilder.addTarget(previewSurface);
        CameraPreferenceManager.applyCamera2CameraPreferences(parentActivity, cameraId,
                characteristics, previewCaptureRequestBuilder);
        previewCaptureRequestBuilder.set(CaptureRequest.CONTROL_AE_LOCK, isAeAwbLocked);
        previewCaptureRequestBuilder.set(CaptureRequest.CONTROL_AWB_LOCK, isAeAwbLocked);
        Log.i(TAG, "Set AE & AWB locked to [" + isAeAwbLocked + "]");

        int width = activeArraySize.width();
        int height = activeArraySize.height();
        zoomSize.left = (int) ((zoom - 1) * width / (2 * zoom));
        zoomSize.right = (int) ((zoom + 1) * width / (2 * zoom));
        zoomSize.top = (int) ((zoom - 1) * height / (2 * zoom));
        zoomSize.bottom = (int) ((zoom + 1) * height / (2 * zoom));
        Log.i(TAG, "zoomSize=" + zoomSize.toShortString());
        previewCaptureRequestBuilder.set(CaptureRequest.SCALER_CROP_REGION, zoomSize);

        roiSize.left = (int) ((1 - roiRect.left) * zoomSize.left + roiRect.left * zoomSize.right);
        roiSize.right = (int) ((1 - roiRect.right) * zoomSize.left + roiRect.right * zoomSize.right);
        roiSize.top = (int) ((1 - roiRect.top) * zoomSize.top + roiRect.top * zoomSize.bottom);
        roiSize.bottom = (int) ((1 - roiRect.bottom) * zoomSize.top + roiRect.bottom * zoomSize.bottom);
        Log.i(TAG, "roiSize=" + roiSize.toShortString());

        MeteringRectangle[] meters = new MeteringRectangle[1];
        meters[0] = new MeteringRectangle(roiSize, 1000);
        if(maxRegionsAE > 0) {
            Log.i(TAG, "set AE regions " + roiSize.toShortString());
            previewCaptureRequestBuilder.set(CaptureRequest.CONTROL_AE_REGIONS, meters);
        }
        if(maxRegionsAF > 0) {
            Log.i(TAG, "set AF regions " + roiSize.toShortString());
            previewCaptureRequestBuilder.set(CaptureRequest.CONTROL_AF_REGIONS, meters);
        }
        if(maxRegionsAWB > 0) {
            Log.i(TAG, "set AWB regions " + roiSize.toShortString());
            previewCaptureRequestBuilder.set(CaptureRequest.CONTROL_AWB_REGIONS, meters);
        }

        //previewCaptureRequestBuilder.set(CaptureRequest.CONTROL_AE_ANTIBANDING_MODE,
        //        CameraCharacteristics.CONTROL_AE_ANTIBANDING_MODE_AUTO);

        //previewCaptureRequestBuilder.set(CaptureRequest.CONTROL_AF_MODE,
        //        CaptureRequest.CONTROL_AF_MODE_CONTINUOUS_VIDEO);
        //previewCaptureRequestBuilder.set(CaptureRequest.CONTROL_AF_MODE,
        //        CaptureRequest.CONTROL_AF_MODE_CONTINUOUS_PICTURE);

        if(CameraPreferenceManager.getVideoStabilizationCfg(parentActivity) == 1)
        {
            previewCaptureRequestBuilder.set(CaptureRequest.CONTROL_VIDEO_STABILIZATION_MODE,
                    CameraMetadata.CONTROL_VIDEO_STABILIZATION_MODE_ON);
            Log.i(TAG, "set CONTROL_VIDEO_STABILIZATION_MODE on");
        }
        else
        {
            previewCaptureRequestBuilder.set(CaptureRequest.CONTROL_VIDEO_STABILIZATION_MODE,
                    CameraMetadata.CONTROL_VIDEO_STABILIZATION_MODE_OFF);
            Log.i(TAG, "set CONTROL_VIDEO_STABILIZATION_MODE off");
        }

        if(!isCapture) {
            previewCaptureRequestBuilder.set(CaptureRequest.CONTROL_AE_LOCK, false);
            previewCaptureRequestBuilder.set(CaptureRequest.CONTROL_AWB_LOCK, false);
            previewCaptureRequestBuilder.set(CaptureRequest.CONTROL_AF_MODE,
                    CaptureRequest.CONTROL_AF_MODE_CONTINUOUS_PICTURE);

            Log.i(TAG, "No capture started, use default value for AF mode, AE/AWB lock");
        } else {
            Log.i(TAG, "Capture started, use setting value for AF mode, AE/AWB lock");

            if(getFocusMode() == CameraMetadata.CONTROL_AF_MODE_AUTO) {
                Log.i(TAG, "AUTO AF MODE, trigger auto focus once");
                previewCaptureRequestBuilder.set(CaptureRequest.CONTROL_AF_TRIGGER,
                        CameraMetadata.CONTROL_AF_TRIGGER_START);

                mCaptureStartTime = System.currentTimeMillis();
            }
        }

        captureSession.setRepeatingRequest(previewCaptureRequestBuilder.build(), mCaptureCallback, null);
    }

    /** {@inheritDoc} */
    @Override
    public void wrapUp() {
        Log.d(TAG, "wrapUp");
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

        //previewSize.set(0, 0);
        cameraView = null;
        isAeAwbLocked = false;
        //pictureCaptureRequest = null;

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
        return (cameraDisplayOrientation == 0 || cameraDisplayOrientation == 180 ?
                previewSize.getWidth() : previewSize.getHeight());
    }

    /** {@inheritDoc} */
    @Override
    public int getHeight() {
        return (cameraDisplayOrientation == 0 || cameraDisplayOrientation == 180 ?
                previewSize.getHeight() : previewSize.getWidth());
    }

    /** {@inheritDoc} */
    @Override
    public float getFocalLength() {
        return characteristics.get(CameraCharacteristics.LENS_INFO_AVAILABLE_FOCAL_LENGTHS)[0];
    }

    /** {@inheritDoc} */
    @Override
    public void lockAeAwb(boolean lock) {
        if (isAeAwbLocked != lock) {
            isAeAwbLocked = lock;

            try {
                issuePreviewCaptureRequest(isCapture);
            } catch (CameraAccessException e) {
                Log.e(TAG, "Failed reissuing preview capture request for AE/AWB lock change", e);
                wrapUp();
            }
        }
    }

    /** {@inheritDoc} */
    @Override
    public void captureKeyFrame(final OnKeyFrameCapturedListener jpegCallback) {
        handler.post(new Runnable() {
            @Override
            public void run() {
                try {
                    //pictureReaderCallback.addKeyFrameRequest(jpegCallback);
                    //captureSession.capture(pictureCaptureRequest, null, null);
                    for(int i = 0; i < burstNum * burstInterval; i++) {
                        pictureReaderCallback.addKeyFrameRequest(jpegCallback);
                    }
                    if(lastzoom != zoom) {
                        pictureCaptureRequestBuilder.set(CaptureRequest.SCALER_CROP_REGION, zoomSize);
                        lastzoom = zoom;

                        picCaptureRequestList.clear();
                        for(int i = 0; i < burstNum * burstInterval; i++) {
                            picCaptureRequestList.add(pictureCaptureRequestBuilder.build());
                        }
                    }

                    captureSession.captureBurst(picCaptureRequestList, null, null);
                } catch (Exception e) {
                    Log.e(TAG, "Failed to issue keyframe capture request", e);
                }
            }
        });
    }

    /**
     * See {@link #selectPreviewSize(Point[], Point)}
     */
    private Point selectPreviewSize(Size[] supportedPreviewSizes, Size pictureSize) {
        Point[] supportedSizes = new Point[supportedPreviewSizes.length];
        for (int i = 0; i < supportedPreviewSizes.length; i++) {
            supportedSizes[i] = new Point(supportedPreviewSizes[i].getWidth(),
                    supportedPreviewSizes[i].getHeight());
        }

        return selectPreviewSize(supportedSizes, new Point(pictureSize.getWidth(),
                pictureSize.getHeight()));
    }

    private final CameraDevice.StateCallback cameraDeviceStateCallback =
            new CameraDevice.StateCallback() {
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

    private final CameraCaptureSession.StateCallback cameraCaptureSessionStateCallback =
            new CameraCaptureSession.StateCallback() {
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
        private final ArrayDeque<OnKeyFrameCapturedListener> keyFrameCapturedListeners =
                new ArrayDeque<OnKeyFrameCapturedListener>();

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

    @Override
    public int getBurstNum() {
        return burstNum;
    }

    @Override
    public int getThumbnailWidth() {
        return thumbnailSize.x;
    }

    @Override
    public int getThumbnailHeight() {
        return thumbnailSize.y;
    }

    @Override
    public int getBurstInterval() {
        return burstInterval;
    }

    @Override
    public float getMaxZoom() {
        return (maxZoom - minZoom);
    }

    @Override
    public float getZoom() {
        return zoom;
    }

    @Override
    public void setZoom(float zoom) {
        if(zoom >= minZoom && zoom <= maxZoom) {
            this.zoom = zoom;

            try {
                issuePreviewCaptureRequest(isCapture);
            } catch (CameraAccessException e) {
                Log.e(TAG, "Failed reissuing preview capture request for zoom change", e);
                wrapUp();
            }
        }
    }

    @Override
    public boolean isZoomSupported() {
        return true;
    }

    @Override
    public boolean isFocusing() {
        /* AUTO MODE special, maximum 2 seconds protection */
        if(isCapture && getFocusMode() == CameraMetadata.CONTROL_AF_MODE_AUTO) {
            if(afState != CameraMetadata.CONTROL_AF_MODE_OFF) {
                long cur = System.currentTimeMillis();

                if((cur - mCaptureStartTime) < 2000) {
                    Log.i(TAG, "isFocusing true auto focus mode, not focused, less 2 seconds also");
                    return true;
                }
            }
        }

        if( CameraPreferenceManager.getFocusCfg(parentActivity) == 1 &&
            ( afState == CameraMetadata.CONTROL_AF_STATE_PASSIVE_SCAN ||
              afState == CameraMetadata.CONTROL_AF_STATE_ACTIVE_SCAN )) {
            return true;
        }

        return false;
    }

    @Override
    public void updateRoi(RectF roi) {
        matrix.mapRect(roiRect, roi);

        try {
            issuePreviewCaptureRequest(isCapture);
        } catch (CameraAccessException e) {
            Log.e(TAG, "Failed reissuing preview capture request for roi change", e);
            //wrapUp();
        }
    }

    @Override
    public void removeRoi() {
        roiRect.set(0, 0, 1, 1);

        try {
            issuePreviewCaptureRequest(isCapture);
        } catch (CameraAccessException e) {
            Log.e(TAG, "Failed reissuing preview capture request for roi change", e);
            //wrapUp();
        }
    }

    @Override
    public int getPictureWidth() {
        return pictureSize.getWidth();
    }

    @Override
    public int getPictureHeight() {
        return pictureSize.getHeight();
    }

    @Override
    public int getFocusMode() {
        return previewCaptureRequestBuilder.get(CaptureRequest.CONTROL_AF_MODE);
    }

    @Override
    public void setCaptureState(boolean capture) {
        isCapture = capture;

        try {
            issuePreviewCaptureRequest(isCapture);
        } catch (CameraAccessException e) {
            Log.e(TAG, "Failed reissuing preview capture request for capture state change", e);
            //wrapUp();
        }
    }
}
