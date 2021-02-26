/*=============================================================================
Copyright (c) 2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
package com.qualcomm.qti.seccamsample;

import android.util.Log;
import android.util.Range;
import android.util.Size;
import android.content.Intent;
import java.nio.ByteBuffer;

import java.util.Arrays;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import android.content.Context;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
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
import android.media.ImageReader;
import android.os.Handler;
import android.os.HandlerThread;
import android.graphics.ImageFormat;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.YuvImage;

import java.util.concurrent.locks.ReentrantReadWriteLock;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.Semaphore;
import java.util.concurrent.TimeUnit;

import com.qualcomm.qti.seccamapi.SecureSurface;
import com.qualcomm.qti.seccamapi.SecureCamera2Surface;
import com.qualcomm.qti.seccamsample.CameraActivity;
import com.qualcomm.qti.seccamapi.SecCamServiceClient;
import com.qualcomm.qti.seccamapi.SecCamServiceVendorClient;

public class SecureCamera2API {

    private SurfaceView previewSurfaceView_ = null;
    private CameraActivity cameraActivity_ = null;
    private Integer cameraId_;
    protected CameraDevice cameraDevice_ = null;
    private HandlerThread backgroundThread_;
    private Handler backgroundHandler_;

    protected CameraCaptureSession captureSession_ = null;
    protected CaptureRequest.Builder captureBuilder_ = null;
    protected CameraCaptureSession.CaptureCallback captureCallback_ = null;

    protected CountDownLatch surfaceReadyLatch_ = new CountDownLatch(1);
    protected CountDownLatch cameraStartedLatch_ = new CountDownLatch(1);
    protected CountDownLatch cameraStopedLatch_ = new CountDownLatch(1);
    protected CountDownLatch captureSessionClosedLatch_ = new CountDownLatch(1);

    protected Semaphore cameraOpenCloseSemaphore_ = new Semaphore(1);

    protected SecureCamera2Surface secureCamera2Surface_ = null;

    protected boolean cameraStarted_ = false;
    private Integer numOfBuffers_ = 2;
    private Integer width_ = 1280;
    private Integer height_ = 720;
    private Integer format_ = ImageFormat.YUV_420_888;
    private Integer previewRotation_ = 0;
    private Integer previewSurfaceNumOfBuffers_ = 3;
    private static boolean is_2dfa = false; //enable for 2dfa

    //=========================================================================
    protected final CameraDevice.StateCallback mStateCallback = new CameraDevice.StateCallback() {

        @Override
        public void onOpened(CameraDevice cameraDevice) {
            // This method is called when the camera is opened.  We start camera preview here.
            Log.d(cameraActivity_.getLogTag(), "SecureCamera2API::CameraDevice.StateCallback::onOpened - Camera #" + cameraId_);
            cameraDevice_ = cameraDevice;
            createCameraSessionCommon();
        }

        @Override
        public void onDisconnected(CameraDevice cameraDevice) {
            Log.d(cameraActivity_.getLogTag(), "SecureCamera2API::CameraDevice.StateCallback::onDisconnected - Camera #" + cameraId_);
            cameraDevice.close();
        }

        @Override
        public void onError(CameraDevice cameraDevice, int error) {
            Log.d(cameraActivity_.getLogTag(), "SecureCamera2API::CameraDevice.StateCallback::onError - Camera #" + cameraId_ +
             " - Error: " + error);
            cameraDevice.close();
        }

        @Override
        public void onClosed(CameraDevice cameraDevice) {
            Log.d(cameraActivity_.getLogTag(), "SecureCamera2API::CameraDevice.StateCallback::onClosed - Camera #" + cameraId_);
            cameraDevice_ = null;
            cameraStartedLatch_.countDown();
            cameraStopedLatch_.countDown();
        }
    };

    //=========================================================================
    public SecureCamera2API(CameraActivity cameraActivity) {
        cameraActivity_ = cameraActivity;
    }

    protected void startBackgroundThread() {
        backgroundThread_ = new HandlerThread("CameraBackground");
        backgroundThread_.start();
        backgroundHandler_ = new Handler(backgroundThread_.getLooper());
    }

    protected void stopBackgroundThread() {
        if (null != backgroundThread_) {
            backgroundThread_.quitSafely();
            try {
                backgroundThread_.join(2000);
                backgroundThread_ = null;
                backgroundHandler_ = null;
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }

    protected Handler getBackgroundHandler() {
        return backgroundHandler_;
    }

    //=========================================================================
    public int getCameraRotation(int cameraId) {
        int rotation = 0;

        try {
            CameraManager manager = (CameraManager) cameraActivity_.getSystemService(Context.CAMERA_SERVICE);
            int orientation = manager.getCameraCharacteristics(Integer.toString(cameraId)).
                                get(CameraCharacteristics.SENSOR_ORIENTATION);

            int defaultRotation = orientation % 360;
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

        } catch (CameraAccessException e) {
            Log.d(cameraActivity_.getLogTag(), "SecureCamera2API::getCameraRotation - Camera #" + cameraId + " - ERROR: " + e);
        }

        return rotation;
    }

    //=========================================================================
    protected void createCameraSessionCommon() {
        try {
            secureCamera2Surface_ = null;
            secureCamera2Surface_ = new SecureCamera2Surface(cameraId_, width_, height_, format_, numOfBuffers_);

            if (previewSurfaceView_ != null) {
                if (cameraActivity_.frameCallbackEnabled_ == Boolean.TRUE) {
                    secureCamera2Surface_.enableFrameCallback(cameraActivity_, cameraActivity_.frameCallbackReturnParamsSize_);
                }

                // Get the preview rotation needed, according to the default rotation of the camera
                previewRotation_ = getCameraRotation(cameraId_);
                if(!is_2dfa){
                    secureCamera2Surface_.assignPreviewSurface(previewSurfaceView_.getHolder(),
                            width_, height_, ImageFormat.PRIVATE, previewRotation_, previewSurfaceNumOfBuffers_);
                }
            }
            Surface surface = secureCamera2Surface_.getCaptureSurface();
            captureBuilder_ = cameraDevice_.createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW);

            captureBuilder_.addTarget(surface);
            cameraDevice_.createCaptureSession(Arrays.asList(surface),
                new CameraCaptureSession.StateCallback() {

                    @Override
                    public void onActive(CameraCaptureSession session) {
                        Log.d(cameraActivity_.getLogTag(), "SecureCamera2API::CameraCaptureSession.StateCallback::onActive - Camera #" + cameraId_);
                    }

                    @Override
                    public void onClosed(CameraCaptureSession session) {
                        Log.d(cameraActivity_.getLogTag(), "SecureCamera2API::CameraCaptureSession.StateCallback::onClosed - Camera #" + cameraId_);
                        captureSessionClosedLatch_.countDown();
                    }

                    @Override
                    public void onConfigureFailed(CameraCaptureSession session) {
                        Log.d(cameraActivity_.getLogTag(), "SecureCamera2API::CameraCaptureSession.StateCallback::onConfigureFailed - Camera #" + cameraId_);
                        cameraStartedLatch_.countDown();
                    }

                    @Override
                    public void onConfigured(CameraCaptureSession session) {
                        Log.d(cameraActivity_.getLogTag(), "SecureCamera2API::CameraCaptureSession.StateCallback::onConfigured - Camera #" + cameraId_);

                        // The camera is already closed
                        if (null == cameraDevice_) {
                            cameraStartedLatch_.countDown();
                            return;
                        }

                        // When the session is ready, we start displaying the preview.
                        captureSession_ = session;

                        captureCallback_ = new CameraCaptureSession.CaptureCallback() {
                          @Override
                          public void onCaptureBufferLost(CameraCaptureSession session, CaptureRequest request,
                                  Surface target, long frameNumber) {
                                Log.d(cameraActivity_.getLogTag(), "SecureCamera2API::CameraCaptureSession.CaptureCallback::onCaptureBufferLost - Camera #" + cameraId_ + ": ("
                                        + frameNumber  + ")");
                          }

                            @Override
                            public void onCaptureCompleted(CameraCaptureSession session, CaptureRequest request,
                                    TotalCaptureResult result) {
                                Log.d(cameraActivity_.getLogTag(), "SecureCamera2API::CameraCaptureSession.CaptureCallback::onCaptureCompleted - Camera #" + cameraId_ + ": Done ("
                                        + result.getFrameNumber() + ")");
                            }

                            @Override
                            public void onCaptureFailed(CameraCaptureSession session, CaptureRequest request,
                                     CaptureFailure failure) {
                                Log.d(cameraActivity_.getLogTag(), "SecureCamera2API::CameraCaptureSession.CaptureCallback::onCaptureFailed - Camera #" + cameraId_ + ": "
                                        + failure.getReason());
                            }

                            @Override
                            public void onCaptureProgressed(CameraCaptureSession session, CaptureRequest request,
                                    CaptureResult partialResult) {
                                Log.d(cameraActivity_.getLogTag(), "SecureCamera2API::CameraCaptureSession.CaptureCallback::onCaptureProgressed - Camera #" + cameraId_ + ": "
                                        + partialResult.toString());
                            }

                            @Override
                            public void onCaptureSequenceAborted(CameraCaptureSession session, int sequenceId) {
                                Log.d(cameraActivity_.getLogTag(), "SecureCamera2API::CameraCaptureSession.CaptureCallback::onCaptureSequenceAborted - Camera #" + cameraId_ + ": "
                                        + sequenceId);
                            }

                            @Override
                            public void onCaptureSequenceCompleted(CameraCaptureSession session, int sequenceId, long frameNumber) {
                                Log.d(cameraActivity_.getLogTag(), "SecureCamera2API::CameraCaptureSession.CaptureCallback::onCaptureSequenceCompleted - Camera #" + cameraId_ + ": ("
                                        + frameNumber  + ")");
                            }

                            @Override
                            public void onCaptureStarted(CameraCaptureSession session, CaptureRequest request, long timestamp, long frameNumber) {
                                Log.d(cameraActivity_.getLogTag(), "SecureCamera2API::CameraCaptureSession.CaptureCallback::onCaptureStarted - Camera #" + cameraId_ + ": ("
                                      + frameNumber  + ")");
                            }
                        };

                        Log.d(cameraActivity_.getLogTag(), "SecureCamera2API::CameraCaptureSession.StateCallback::onConfigured - Camera #" + cameraId_ +
                                 " capture session is ready!");
                        try {
                            captureSession_.setRepeatingRequest(captureBuilder_.build(), captureCallback_ ,getBackgroundHandler());
                        } catch (CameraAccessException e) {
                            Log.d(cameraActivity_.getLogTag(), "SecureCamera2API::CameraCaptureSession.StateCallback:onConfigured - Camera #" + cameraId_ +
                                " - setRepeatingRequest failed: " + e);
                        }
                        cameraStartedLatch_.countDown();
                    }

                    @Override
                    public void onReady(CameraCaptureSession session) {
                        Log.d(cameraActivity_.getLogTag(), "SecureCamera2API::CameraCaptureSession.StateCallback::onReady - Camera #" + cameraId_);
                    }

                    @Override
                    public void onSurfacePrepared(CameraCaptureSession session, Surface surface) {
                        Log.d(cameraActivity_.getLogTag(), "SecureCamera2API::CameraCaptureSession.StateCallback::onSurfacePrepared - Camera #" + cameraId_);
                    }

                }, null
            );
        } catch (CameraAccessException e) {
            Log.d(cameraActivity_.getLogTag(), "SecureCamera2API::createCameraSession - Camera #" + cameraId_ + " - ERROR: " + e);
        }
    }

    //=========================================================================
    public Boolean start(int cameraId, SurfaceView previewSurfaceView,
            Integer width, Integer height, Integer format, Integer numOfBuffers,
            Integer previewSurfaceNumOfBuffers) {
        previewSurfaceView_ = previewSurfaceView;
        previewSurfaceNumOfBuffers_ = previewSurfaceNumOfBuffers;
        return start(cameraId, width, height, format, numOfBuffers);
    }

    //=========================================================================
    public Boolean start(Integer cameraId, Integer width, Integer height, Integer format, Integer numOfBuffers) {
        Log.d(cameraActivity_.getLogTag(), "SecureCamera2API::start - Camera #" + cameraId_);
        Boolean result = Boolean.FALSE;

        cameraId_ = cameraId;
        width_ = width;
        height_ = height;
        format_ = format;
        numOfBuffers_ = numOfBuffers;

        startBackgroundThread();
        try {

            CameraManager manager = (CameraManager) cameraActivity_.getSystemService(Context.CAMERA_SERVICE);
            try {
                if (!cameraOpenCloseSemaphore_.tryAcquire(2500, TimeUnit.MILLISECONDS)) {
                    Log.d(cameraActivity_.getLogTag(), "SecureCamera2API::start - Camera #" + cameraId_ + " - ERROR: Camera Lock TimeOut");
                }
                else {
                    cameraStartedLatch_ = new CountDownLatch(1);
                    manager.openCamera(Integer.toString(cameraId_), mStateCallback, getBackgroundHandler());
                    if(!cameraStartedLatch_.await(5000, TimeUnit.MILLISECONDS)) {
                        Log.d(cameraActivity_.getLogTag(), "SecureCamera2API::start - Camera #" + cameraId_ + " - ERROR: Camera Open TimeOut");
                    }
                    else {
                        if(null == cameraDevice_) {
                            Log.d(cameraActivity_.getLogTag(), "SecureCamera2API::start - Camera #" + cameraId_ + " - FAILED");
                        }
                        else {
                            Log.d(cameraActivity_.getLogTag(), "SecureCamera2API::start - Camera #" + cameraId_ + " - OK");
                            if(is_2dfa){
                                SecCamServiceClient.getInstance().dispatchVendorCommand(SecCamServiceVendorClient.MSG_VENDOR_START_MLVMAPP, null);
                            }
                            cameraStarted_ = true;
                            result = Boolean.TRUE;
                        }
                    }
                }
            }
            catch (CameraAccessException e) {
                Log.d(cameraActivity_.getLogTag(), "SecureCamera2API::start - Camera #" + cameraId_ + " - ERROR: Not accesible");
                throw new RuntimeException();
            }
            catch (InterruptedException e) {
                Log.d(cameraActivity_.getLogTag(), "SecureCamera2API::start - Camera #" + cameraId_ + " - ERROR: Interrupted while trying to lock camera for opening - ", e);
                throw new RuntimeException();
            }
            finally {
                cameraOpenCloseSemaphore_.release();
            }
        }
        catch (RuntimeException e) {
            Log.d(cameraActivity_.getLogTag(), "SecureCamera2API::start - Camera #" + cameraId_ + " - ERROR: " + e);
        }
        catch (Exception e) {
            Log.d(cameraActivity_.getLogTag(), "SecureCamera2API::start - Camera #" + cameraId_ + " - ERROR: " + e);
        }
        Log.d(cameraActivity_.getLogTag(), "SecureCamera2API::start - Camera #" + cameraId_ + " - Done");
        return result;
    }

    //=========================================================================
    public Boolean stop() {
        Boolean result = Boolean.FALSE;
        Log.d(cameraActivity_.getLogTag(), "SecureCamera2API::stop - Camera #" + cameraId_);

        // Dispatch the command using the vendor command ID, and the created bundle.
        if(is_2dfa){
            SecCamServiceClient.getInstance().dispatchVendorCommand(SecCamServiceVendorClient.MSG_VENDOR_STOP_MLVMAPP, null);
        }
        try {
            if (!cameraOpenCloseSemaphore_.tryAcquire(2500, TimeUnit.MILLISECONDS)) {
                Log.d(cameraActivity_.getLogTag(), "SecureCamera2API::stop - Camera #" + cameraId_ + " - ERROR: Camera Lock TimeOut");
            }
            // Ignore the timeout and proceed with camera closure
            if (null != captureSession_) {
                Log.d(cameraActivity_.getLogTag(), "SecureCamera2APIPreview::stop - Camera #" + cameraId_ + ": Close Capture Session");
                captureSessionClosedLatch_ = new CountDownLatch(1);
                captureSession_.close();
                if(!captureSessionClosedLatch_.await(2000, TimeUnit.MILLISECONDS)) {
                    Log.d(cameraActivity_.getLogTag(), "SecureCamera2API::stop - Camera #" + cameraId_ + " - ERROR: Close Capture Session TimeOut");
                }
                captureSession_ = null;
            }
            if (null != cameraDevice_) {
                Log.d(cameraActivity_.getLogTag(), "SecureCamera2API::stop - Camera #" + cameraId_ + ": Close Camera");
                cameraStopedLatch_ = new CountDownLatch(1);
                cameraDevice_.close();
                if(!cameraStopedLatch_.await(2000, TimeUnit.MILLISECONDS)) {
                    Log.d(cameraActivity_.getLogTag(), "SecureCamera2API::stop - Camera #" + cameraId_ + " - ERROR: Close Camera TimeOut");
                }
                cameraDevice_ = null;
            }
            result = Boolean.TRUE;
        } catch (InterruptedException e) {
            Log.d(cameraActivity_.getLogTag(), "SecureCamera2API::stop - ERROR: Interrupted while trying to lock camera closing.", e);
        } finally {
            cameraStarted_ = false;
            stopBackgroundThread();
            cameraOpenCloseSemaphore_.release();
        }

        if (null != secureCamera2Surface_) {
            Log.d(cameraActivity_.getLogTag(), "SecureCamera2API::stop - Camera #" + cameraId_ + ": Release Secure Camera Surface");
            secureCamera2Surface_.release();
            secureCamera2Surface_ = null;
        }
        Log.d(cameraActivity_.getLogTag(), "SecureCamera2API::stop - Camera #" + cameraId_ + ": Done");
        return result;
    }
}
