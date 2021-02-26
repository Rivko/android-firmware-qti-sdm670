/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.clevercapture.hardware;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import android.app.Activity;
import android.graphics.ImageFormat;
import android.graphics.Matrix;
import android.graphics.Point;
import android.graphics.Rect;
import android.graphics.RectF;
import android.hardware.Camera;
import android.hardware.Camera.CameraInfo;
import android.hardware.Camera.Parameters;
import android.hardware.Camera.Size;
import android.hardware.camera2.CameraMetadata;
import android.hardware.camera2.CaptureRequest;
import android.util.Log;

import com.qualcomm.qti.clevercaptureui.R;
import com.qualcomm.qti.clevercaptureui.CameraPreviewView;
import com.qualcomm.qti.clevercaptureui.preferences.CameraPreferenceManager;

/**
 * This class handles all the camera interactions, using the legacy {@link Camera} APIs
 */
@SuppressWarnings("deprecation")
public class CameraActorLegacy extends CameraActor {
    private static final String LOG_TAG = CameraActorLegacy.class.getSimpleName();
    private static final String KEY_QC_ZSL = "zsl";
    private static final String SUPPORTED_VALUES_SUFFIX = "-values";
    private static final String ZSL_ON = "on";
    private static final String KEY_QC_ZSL_BURST_LOOKBACK = "capture-burst-retroactive";

    private Camera camera;
    private final CameraInfo cameraInfo = new CameraInfo();
    private Parameters cameraParameters;
    private String cameraFocusMode;
    private CameraPreviewView cameraView;
    private int cameraDisplayOrientation;
    private Size previewSize;
    private Size pictureSize;
    private Point thumbnailSize = new Point();
    private float focalLength;
    private boolean isAeAwbLocked;

    private boolean isZoomSupported;
    private int maxZoom;
    private int zoom = 1;

    private int maxRegionsAE;
    private int maxRegionsAF;
    //private int maxRegionsAWB;
    private RectF roiRect = new RectF(0, 0, 1, 1);
    private Rect roiSize = new Rect();
    private Matrix matrix = new Matrix();

    private long autoFocusStartTime;
    private boolean isAutoFocused;
    private Camera.AutoFocusCallback autoFocusCallback = new Camera.AutoFocusCallback() {
        @Override
        public void onAutoFocus(boolean success, Camera camera) {
            isAutoFocused = success;

            Log.d(LOG_TAG, "auto focus result " + success);
        }
    };

    private boolean isFocusing;
    private Camera.AutoFocusMoveCallback focusMoveCallback = new Camera.AutoFocusMoveCallback() {
        @Override
        public void onAutoFocusMoving(boolean start, Camera camera) {
            isFocusing = start;

            if(start) {
                Log.d(LOG_TAG, "focus started");
            } else {
                Log.d(LOG_TAG, "focus stopped");
            }
        }
    };
    private OnKeyFrameCapturedListener keyframeCapturedListener;
    private Camera.PictureCallback picCallback = new Camera.PictureCallback() {
        @Override
        public void onPictureTaken(byte[] data, Camera camera) {
            if(keyframeCapturedListener != null) {
                keyframeCapturedListener.onKeyFrameCaptured(data);
            } else {
                Log.d(LOG_TAG, "we received jpegdata but no listenter found");
            }
        }
    };

    /**
     * Construct a {@link CameraActorLegacy} instance, which implements the {@link CameraActor}
     * contract with the legacy Android {@link Camera} APIs
     * @param parentActivity {@link Activity} which owns this {@link CameraActorLegacy}
     */
    public CameraActorLegacy(Activity parentActivity) {
        super(parentActivity);
    }

    /** {@inheritDoc} */
    @Override
    public boolean startCamera() {
        int jpegQuality = CameraPreferenceManager.getImageCompressLevel(parentActivity);
        int cameraId = CameraPreferenceManager.getSelectedCameraId(parentActivity);

        try {
            camera = Camera.open(cameraId);
            //For release version, switch to HAL1
            //camera = Camera.openLegacy(cameraId, 0x100);
        } catch (Exception e) {
            Log.d(LOG_TAG, "Camera open failed: " + e.getMessage());
            camera = null;
        }
        if (camera == null) {
            return false;
        }

        Camera.getCameraInfo(cameraId, cameraInfo);
        cameraDisplayOrientation = getCameraDisplayOrientation(cameraInfo.orientation,
                cameraInfo.facing == CameraInfo.CAMERA_FACING_FRONT);

        camera.enableShutterSound(false);
        cameraParameters = camera.getParameters();
        CameraPreferenceManager.applyLegacyCameraPreferences(parentActivity, cameraId, cameraParameters);
        cameraFocusMode = cameraParameters.getFocusMode();
        String supportedZslModes = cameraParameters.get(KEY_QC_ZSL + SUPPORTED_VALUES_SUFFIX);
        if (supportedZslModes != null && supportedZslModes.contains(ZSL_ON)) {
            cameraParameters.set(KEY_QC_ZSL, ZSL_ON);
            Log.i(LOG_TAG, "Set ZSL to [" + ZSL_ON + "]");
            //cameraParameters.set(KEY_QC_ZSL_BURST_LOOKBACK, 2); // N+1 == 1 frame of lookback
            //Log.i(LOG_TAG, "Set ZSL_BURST_LOOKBACK to [" + 2 + "]");
        }

        // Match preview to selected picture size with best try effort
        pictureSize = cameraParameters.getPictureSize();
        previewSize = cameraParameters.getPreviewSize();
        //List<Size> supportedPreviewSizes = cameraParameters.getSupportedPreviewSizes();
        //previewSize = selectPreviewSize(supportedPreviewSizes, pictureSize);
        //previewSize.x = 1280;
        //previewSize.y = 720;
        //cameraParameters.setPreviewSize(previewSize.x, previewSize.y);
        Log.i(LOG_TAG, "Set picture size to [" + pictureSize.width + "x" + pictureSize.height + "]");
        Log.i(LOG_TAG, "Set preview size to [" + previewSize.width + "x" + previewSize.height + "]");

        List<Size> supportedThumbnailSizes = cameraParameters.getSupportedJpegThumbnailSizes();
        thumbnailSize = selectPreviewSize(supportedThumbnailSizes, pictureSize);
        cameraParameters.setJpegThumbnailQuality(jpegQuality);
        cameraParameters.setJpegThumbnailSize(thumbnailSize.x, thumbnailSize.y);
        Log.i(LOG_TAG, "Set thumbnail size to [" + thumbnailSize.x + "x" + thumbnailSize.y + "]");

        isZoomSupported = cameraParameters.isZoomSupported();
        if(isZoomSupported) {
            maxZoom = cameraParameters.getMaxZoom();
            Log.i(LOG_TAG, "maxZoom=" + maxZoom);
        }

        cameraParameters.setJpegQuality(jpegQuality);
        cameraParameters.setRotation(cameraDisplayOrientation);
        Log.i(LOG_TAG, "cameraDisplayOrientation=" + cameraDisplayOrientation);
        matrix.setRotate((float)cameraDisplayOrientation, 0.5f, 0.5f);
        cameraParameters.setPreviewFormat(ImageFormat.NV21);

        maxRegionsAE = cameraParameters.getMaxNumMeteringAreas();
        maxRegionsAF = cameraParameters.getMaxNumFocusAreas();
        //maxRegionsAWB = 0;
        Log.i(LOG_TAG, "maxRegionsAE=" + maxRegionsAE + " maxRegionsAF=" + maxRegionsAF);

        // camera 3A settings without clevercapture session
        cameraParameters.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE);
        if (cameraParameters.isAutoExposureLockSupported()) {
            cameraParameters.setAutoExposureLock(false);
        }
        if (cameraParameters.isAutoWhiteBalanceLockSupported()) {
            cameraParameters.setAutoWhiteBalanceLock(false);
        }

        if(CameraPreferenceManager.getVideoStabilizationCfg(parentActivity) == 1)
        {
            cameraParameters.setVideoStabilization(true);
            Log.i(LOG_TAG, "setVideoStabilization true");
        }
        else
        {
            cameraParameters.setVideoStabilization(false);
            Log.i(LOG_TAG, "setVideoStabilization false");
        }

        refreshRoi();

        // apply camera parameters
        camera.setParameters(cameraParameters);
        camera.setAutoFocusMoveCallback(focusMoveCallback);

        cameraView = (CameraPreviewView)parentActivity.findViewById(R.id.camera_preview_view);
        cameraView.setPreviewFrameRotation(cameraDisplayOrientation);
        cameraView.setPreviewFrameSize(getWidth(), getHeight());

        try {
            camera.setPreviewTexture(cameraView.getSurfaceTexture());
        } catch (IOException e) {
            Log.e(LOG_TAG, "Failed setting camera preview SurfaceTexture", e);
            throw new RuntimeException("Failed setting camera preview SurfaceTexture", e);
        }

        camera.startPreview();

        focalLength = cameraParameters.getFocalLength();
        Log.i(LOG_TAG, "Lens focal length is [" + focalLength + "]");

        isAeAwbLocked =  CameraPreferenceManager.getEnableAeAWB(parentActivity);

        return true;
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
                previewSize.width : previewSize.height);
    }

    /** {@inheritDoc} */
    @Override
    public int getHeight() {
        return (cameraDisplayOrientation == 0 || cameraDisplayOrientation == 180 ?
                previewSize.height : previewSize.width);
    }

    /** {@inheritDoc} */
    @Override
    public float getFocalLength() {
        return focalLength;
    }

    /** {@inheritDoc} */
    @Override
    public void wrapUp() {
        Log.d(LOG_TAG, "Camera Actor Wrapped");
        if (camera != null) {
            camera.release();
            camera = null;
        }
        cameraView = null;
        //previewSize.set(0, 0);
    }

    /** {@inheritDoc} */
    @Override
    public void lockAeAwb(boolean lock) {
        if (isAeAwbLocked != lock) {
            if (cameraParameters.isAutoExposureLockSupported()) {
                cameraParameters.setAutoExposureLock(lock);
            }
            if (cameraParameters.isAutoWhiteBalanceLockSupported()) {
                cameraParameters.setAutoWhiteBalanceLock(lock);
            }

            camera.setParameters(cameraParameters);

            isAeAwbLocked = lock;
        }
    }

    /** {@inheritDoc} */
    public void captureKeyFrame(final OnKeyFrameCapturedListener jpegCallback) {
        keyframeCapturedListener = jpegCallback;

        handler.post(new Runnable() {
            @Override
            public void run() {
                camera.takePicture(null, null, null, picCallback);
            }
        });
    }

    /**
     * See {@link #selectPreviewSize(Point[], Point)}
     */
    private Point selectPreviewSize(List<Size> supportedPreviewSizes, Size pictureSize) {
        Point[] supportedSizes = new Point[supportedPreviewSizes.size()];
        int i = 0;
        for (Size size : supportedPreviewSizes) {
            supportedSizes[i++] = new Point(size.width, size.height);
        }

        return selectPreviewSize(supportedSizes, new Point(pictureSize.width, pictureSize.height));
    }

    @Override
    public int getBurstNum() {

        return 3;
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
        return 1;
    }

    @Override
    public float getMaxZoom() {
        return isZoomSupported ? maxZoom : 1 ;
    }

    @Override
    public float getZoom() {
        return zoom;
    }

    @Override
    public void setZoom(float zoom) {
        cameraParameters = camera.getParameters();
        if(zoom > 1.0f) {
            cameraParameters.setZoom((int) (zoom - 1));
            Log.d(LOG_TAG, "setZoom " + (int) (zoom - 1));

            this.zoom = (int) zoom;
        } else {
            cameraParameters.setZoom(0);
            Log.d(LOG_TAG, "setZoom 0");

            this.zoom = 1;
        }
        camera.setParameters(cameraParameters);
    }

    @Override
    public boolean isZoomSupported() {
        return true;
    }

    @Override
    public boolean isFocusing() {
        /* AUTO MODE special, maximum 2 seconds protection */
        if(cameraFocusMode == Camera.Parameters.FOCUS_MODE_AUTO) {
            if(!isAutoFocused) {
                long cur = System.currentTimeMillis();

                if((cur - autoFocusStartTime) < 2000) {
                    Log.i(LOG_TAG, "isFocusing true auto focus mode, not focused, less 2 seconds also");
                    return true;
                }
            }
        }

        if( CameraPreferenceManager.getFocusCfg(parentActivity) == 1 && isFocusing )
            return true;

        return false;
    }

    private void refreshRoi() {
        roiSize.left = (int) (roiRect.left * 2000 - 1000);
        roiSize.right = (int) (roiRect.right * 2000 - 1000);
        roiSize.top = (int) (roiRect.top * 2000 - 1000);
        roiSize.bottom = (int) (roiRect.bottom * 2000 - 1000);

        final List<Camera.Area> areas = new ArrayList<Camera.Area>();
        areas.add(new Camera.Area(roiSize, 1000));

        if(maxRegionsAE > 0)
            cameraParameters.setMeteringAreas(areas);
        if(maxRegionsAF > 0)
            cameraParameters.setFocusAreas(areas);

        camera.setParameters(cameraParameters);

        Log.i(LOG_TAG, "refreshRoi roiSize " + roiSize.toShortString());
    }

    @Override
    public void updateRoi(RectF roi) {
        matrix.mapRect(roiRect, roi);
        refreshRoi();
    }

    @Override
    public void removeRoi() {
        roiRect.set(0, 0, 1, 1);
        refreshRoi();
    }
@Override
    public int getPictureWidth() {
        return pictureSize.width;
    }

    @Override
    public int getPictureHeight() {
        return pictureSize.height;
    }

    @Override
    public int getFocusMode() {
        String focusMode = cameraParameters.getFocusMode();

        int fmode = -1;
        if(focusMode.equals(Camera.Parameters.FOCUS_MODE_AUTO)) {
            fmode = 1;
        } else if(focusMode.equals(Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE)) {
            fmode = 4;
        } else if(focusMode.equals(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO)) {
            fmode = 3;
        } else if(focusMode.equals(Camera.Parameters.FOCUS_MODE_INFINITY)) {
            fmode = 0;
        } else if(focusMode.equals(Camera.Parameters.FOCUS_MODE_MACRO)) {
            fmode = 2;
        }
        return fmode;
    }

    @Override
    public void setCaptureState(boolean capture) {
        if(capture) {
            cameraParameters.setFocusMode(cameraFocusMode);
            if (cameraParameters.isAutoExposureLockSupported()) {
                cameraParameters.setAutoExposureLock(isAeAwbLocked);
            }
            if (cameraParameters.isAutoWhiteBalanceLockSupported()) {
                cameraParameters.setAutoWhiteBalanceLock(isAeAwbLocked);
            }

            camera.setParameters(cameraParameters);

            if(cameraFocusMode.equals(Camera.Parameters.FOCUS_MODE_AUTO)) {
                Log.i(LOG_TAG, "AUTO AF MODE, trigger auto focus once");
                autoFocusStartTime = System.currentTimeMillis();
                isAutoFocused = false;
                camera.autoFocus(autoFocusCallback);
            }

            Log.i(LOG_TAG, "start capture, focus mode -> " + cameraFocusMode + " AeAwbLock -> " + isAeAwbLocked);
        } else {
            // default 3A setting when no capture session
            cameraParameters.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE);
            if (cameraParameters.isAutoExposureLockSupported()) {
                cameraParameters.setAutoExposureLock(false);
            }
            if (cameraParameters.isAutoWhiteBalanceLockSupported()) {
                cameraParameters.setAutoWhiteBalanceLock(false);
            }

            camera.setParameters(cameraParameters);

            Log.i(LOG_TAG, "start capture, focus mode -> " + Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE + " AeAwbLock -> " + false);
        }
    }
}
