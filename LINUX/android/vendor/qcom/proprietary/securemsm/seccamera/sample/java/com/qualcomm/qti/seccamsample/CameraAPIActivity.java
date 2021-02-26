/*=============================================================================
Copyright (c) 2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package com.qualcomm.qti.seccamsample;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import android.app.Activity;
import android.content.res.Configuration;
import android.os.Bundle;
import android.os.AsyncTask;
import android.util.Log;
import android.hardware.Camera;
import android.hardware.Camera.CameraInfo;
import android.hardware.Camera.PictureCallback;
import android.widget.Button;
import android.widget.Toast;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.WindowManager;
import android.graphics.ImageFormat;

import com.qualcomm.qti.seccamsample.R;
import com.qualcomm.qti.seccamapi.SecureSurface;
import com.qualcomm.qti.seccamapi.SecureCameraSurface;
import com.qualcomm.qti.seccamapi.SecCamServiceClient;

public class CameraAPIActivity extends CameraActivity implements PictureCallback {

    private Camera previewCamera_;
    private Camera captureCamera_;

    protected SecureCameraSurface secureCameraPreviewSurface_;
    protected SecureCameraSurface secureCameraCaptureSurface_;

    private static final Integer previewNumOfBuffers_ = 2;
    private static final Integer previewWidthHAL1_ = 1280;
    private static final Integer previewHeightHAL1_ = 960;
    private static final Integer previewFormatHAL1_ = SecureSurface.IMAGE_FORMAT_YUV420SP;
    private static final Integer previewWidthHAL3_ = 1280;
    private static final Integer previewHeightHAL3_ = 720;
    private static final Integer previewFormatHAL3_ = ImageFormat.YUV_420_888;
    private static final Integer previewSurfaceNumOfBuffers_ = 3;
    private static final Integer previewCameraID_ = CameraInfo.CAMERA_FACING_FRONT;
    private static Integer previewRotation_ = SecureSurface.ROTATE_90_VERTICAL_FLIP; //rotate source image 90 degrees clockwise + vertical flip

    private static final Integer captureNumOfBuffers_ = 9;
    private static final Integer captureFormat_ = SecureSurface.IMAGE_FORMAT_RAW;
    private static final Integer captureCameraID_ = 2; //2 is the index for the front IR camera on MTP

    private static final boolean useHAL1_ = true;
    // Setting captureCameraEnabled to false will allow to only test secure preview

    //=========================================================================
    private String getMaxRdiResolution() {

        if (captureCamera_ == null) {
            throw new NullPointerException("Capture camera is undefined");
        }

        List<Camera.Size> sizes = new ArrayList<Camera.Size>();

        Camera.Parameters params = captureCamera_.getParameters();
        Integer[][] rdiResolutions = {{0, 0}};
        // Receive the RAW resolution from the sensor
        String maxResolution = params.get("raw-size");

        return maxResolution;
    }

    //=========================================================================
    public static int getCameraRotation(int cameraId, Camera camera) {

        int rotation = 0;
        Camera.CameraInfo info = new Camera.CameraInfo();
        Camera.getCameraInfo(cameraId, info);

        int defaultRotation = (info.orientation) % 360;
        switch (defaultRotation) {
            case 0:
                rotation = SecureSurface.FLIP_HORIZONTALLY;
                break;
            case 90:
                rotation = SecureSurface.ROTATE_90_VERTICAL_FLIP;
                break;
            case 180:
                rotation = SecureSurface.FLIP_VERTICALLY;
                break;
            case 270:
                rotation = SecureSurface.ROTATE_90_HORIZONTAL_FLIP;
                break;
            default:
                rotation = SecureSurface.NO_ROTATION;
        }

        return rotation;
    }

    //=========================================================================
    private class StartTask extends AsyncTask<String, Void, Boolean> {

        protected void onPreExecute() {
            Log.d(getLogTag(), "CameraAPIActivity::StartTask::onPreExecute");
        }

        protected Boolean doInBackground(String... tmpStr) {
            try {
                Log.d(getLogTag(), "CameraAPIActivity::StartTask::doInBackground");

                if (captureCameraEnabled_ && (captureCamera_ == null)) {
                    try {
                        if (useHAL1_)  {
                            // openLegacy is mandatory in order to open the camera in HAL1 API
                            // first parameter is the camera ID, dependent on the cameras available for the target
                            captureCamera_ = Camera.openLegacy(captureCameraID_, Camera.CAMERA_HAL_API_VERSION_1_0);
                        }
                        else {
                            captureCamera_ = Camera.open(captureCameraID_);
                        }
                    } catch (Exception e) {
                        // In case capture camera is unsupported, capture is disabled and only preview will be shown
                        captureCamera_ = null;
                    }

                    if (captureCamera_ != null) {
                        Camera.Parameters params = captureCamera_.getParameters();

                        // In case of RDI, the camera should be configured to use the highest resolution supported by the sensor.
                        String rdiResolution = getMaxRdiResolution();
                        String[] rdiDimensions = rdiResolution.split("x");
                        if (rdiDimensions.length != 2) {
                            throw new Exception("Invalid max resolution obtained from RDI camera " + captureCameraID_);
                        }

                        Integer maxRdiWidth = Integer.parseInt(rdiDimensions[0]);
                        Integer maxRdiHeight = Integer.parseInt(rdiDimensions[1]);

                        // Set a new Secure Camera Surface. The allocated capture buffer size will be (maxRdiWidth x maxRdiHeight).
                        secureCameraCaptureSurface_ = new SecureCameraSurface(captureCameraID_,
                            params, maxRdiWidth, maxRdiHeight, captureFormat_, captureNumOfBuffers_, useHAL1_);

                        // RDI resolution is not supported directly by HAL1, therefore any of the supported values can be set.
                        // This will not affect the actual RDI frame resolution.
                        List<Camera.Size> supportedSizes = params.getSupportedPictureSizes();
                        Integer captureWidth = supportedSizes.get(0).width;
                        Integer captureHeight = supportedSizes.get(0).height;

                        params.setPreviewSize(captureWidth, captureHeight);
                        params.setPreviewFormat(ImageFormat.NV21);
                        captureCamera_.setParameters(params);

                        captureCamera_.setPreviewDisplay(secureCameraCaptureSurface_.getSurfaceHolder());
                        captureCamera_.startPreview();
                    }
                }

                if (previewCamera_ == null) {
                        Integer previewWidth, previewHeight, previewFormat;
                        if (useHAL1_)  {
                            previewCamera_ = Camera.openLegacy(previewCameraID_, Camera.CAMERA_HAL_API_VERSION_1_0);
                            previewWidth = previewWidthHAL1_;
                            previewHeight = previewHeightHAL1_;
                            previewFormat = previewFormatHAL1_;
                        }
                        else {
                            previewCamera_ = Camera.open(previewCameraID_);
                            previewWidth = previewWidthHAL3_;
                            previewHeight = previewHeightHAL3_;
                            previewFormat = previewFormatHAL3_;
                        }
                        Camera.Parameters params = previewCamera_.getParameters();

                        // Get the preview rotation needed, according to the default rotation of the camera
                        previewRotation_ = getCameraRotation(previewCameraID_, previewCamera_);

                        // Validate that the defined preview resolution is supported by the camera
                        boolean isPreviewResolutionValid = false;
                        List<Camera.Size> supportedSizes = params.getSupportedPictureSizes();
                        for (Camera.Size size : supportedSizes){
                            if ((previewWidth == size.width) && (previewHeight == size.height)) {
                                isPreviewResolutionValid = true;
                            }
                        }

                        if (isPreviewResolutionValid == false) {
                            throw new Exception("Resolution (" + previewWidth + "," + previewHeight + ") unsupported by camera " + previewCameraID_);
                        }

                        // Set a new Secure Camera Surface. The allocated preview buffer size will be (previewWidth x previewHeight).
                        secureCameraPreviewSurface_ = new SecureCameraSurface(previewCameraID_,
                            params, previewWidth, previewHeight, previewFormat, previewNumOfBuffers_, useHAL1_);

                        if (frameCallbackEnabled_ == Boolean.TRUE) {
                            secureCameraPreviewSurface_.enableFrameCallback(cameraActivity_, frameCallbackReturnParamsSize_);
                        }

                        params.setPreviewSize(previewWidth, previewHeight);
                        params.setPreviewFormat(ImageFormat.NV21);
                        previewCamera_.setParameters(params);

                        Surface previewSurface = getPreviewView().getHolder().getSurface();
                        // assignPreviewSurface Sets the connection between the Secure Camera Surface, and the Preview Surface
                        secureCameraPreviewSurface_.assignPreviewSurface(surfaceHolder_, previewWidth, previewHeight,
                                previewFormat, previewRotation_, previewSurfaceNumOfBuffers_);
                        previewCamera_.setPreviewDisplay(secureCameraPreviewSurface_.getSurfaceHolder());
                        previewCamera_.startPreview();
                }
            } catch (Exception e) {
                Log.e(LOG_TAG, "StartTask Exception caught", e);
                return Boolean.FALSE;
            }

            return Boolean.TRUE;
        }

        protected void onPostExecute(Boolean result) {
            Log.d(getLogTag(), "CameraAPIActivity::StartTask::onPostExecute");
            startTask_ = null;
        }

    }

    //=========================================================================
    private class EndTask extends AsyncTask<String, Void, Boolean> {

        protected void onPreExecute() {
            Log.d(getLogTag(), "CameraAPIActivity::EndTask::onPreExecute");
        }

        protected Boolean doInBackground(String... tmpStr) {
            Log.d(getLogTag(), "CameraAPIActivity::EndTask::doInBackground");
            try {
                if (previewCamera_ != null) {
                    previewCamera_.stopPreview();
                    previewCamera_.release();
                    previewCamera_ = null;
                }

                if (secureCameraPreviewSurface_ != null) {
                    secureCameraPreviewSurface_.release();
                    secureCameraPreviewSurface_ = null;
                }

                if (captureCamera_ != null) {
                    captureCamera_.stopPreview();
                    captureCamera_.release();
                    captureCamera_ = null;
                }

                if (secureCameraCaptureSurface_ != null) {
                    secureCameraCaptureSurface_.release();
                    secureCameraCaptureSurface_ = null;
                }
            } catch (Exception e) {
                Log.e(LOG_TAG, "EndTask Exception caught", e);
                return Boolean.FALSE;
            }

            return Boolean.TRUE;
        }

        protected void onPostExecute(Boolean result) {
            Log.d(getLogTag(), "CameraAPIActivity::EndTask::onPostExecute");
            endTask_ = null;
        }

    }

    //=========================================================================
    @Override
    protected AsyncTask<String, Void, Boolean> getStartTask() {
        return new StartTask();
    }

    @Override
    protected AsyncTask<String, Void, Boolean> getEndTask() {
        return new EndTask();
    }


    //=========================================================================
    @Override
    public void onPictureTaken(byte[] data, Camera camera) {
    }

}