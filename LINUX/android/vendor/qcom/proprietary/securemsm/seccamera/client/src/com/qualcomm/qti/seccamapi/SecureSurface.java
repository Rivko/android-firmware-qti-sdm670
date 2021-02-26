// Copyright (c) 2017 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
package com.qualcomm.qti.seccamapi;

import android.graphics.ImageFormat;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceHolder.Callback;

import com.qualcomm.qti.seccamapi.SecCamServiceClient;

public class SecureSurface {
    protected static final String  SECCAM_API_LOG_TAG = "SECCAM-API";

    //Android (android.graphics.ImageFormat) and proprietary (HAL) image format abstraction
    public static final int IMAGE_FORMAT_YUV420SP = ImageFormat.NV21; //17
    public static final int IMAGE_FORMAT_YUV420SP_UBWC = 2141391878; //HAL_PIXEL_FORMAT_YCbCr_420_SP_VENUS_UBWC - 0x7FA30C06
    public static final int IMAGE_FORMAT_YUV420 = ImageFormat.YUV_420_888; //35
    public static final int IMAGE_FORMAT_RAW = ImageFormat.RAW_PRIVATE; //36
    public static final int IMAGE_FORMAT_RAW_SENSOR = ImageFormat.RAW_SENSOR; //32
    public static final int IMAGE_FORMAT_RAW10 = ImageFormat.RAW10; //37
    public static final int IMAGE_FORMAT_PRIVATE = ImageFormat.PRIVATE; //34

    // no rotation or flip
    public static final int NO_ROTATION = 0;
    // flip source image horizontally (around the vertical axis)
    public static final int FLIP_HORIZONTALLY = 1;
    // flip source image vertically (around the horizontal axis)
    public static final int FLIP_VERTICALLY = 2;
    // rotate source image 180 degrees
    public static final int ROTATE_180 = 3;
    // rotate source image 90 degrees clockwise
    public static final int ROTATE_90  = 4;
    // rotate source image 90 degrees clockwise + horizontal flip
    public static final int ROTATE_90_HORIZONTAL_FLIP = 5;
    // rotate source image 90 degrees clockwise + vertical flip
    public static final int ROTATE_90_VERTICAL_FLIP = 6;
    // rotate source image 270 degrees clockwise
    public static final int ROTATE_270  = 7;

    //=========================================================================
    public interface FrameCallback {
        public void onSecureFrameAvalable(FrameInfo frameInfo, byte[] returnParams);
    }

    //=========================================================================
    static public class FrameInfo {
        public long frameNumber_ = 0;
        public long timeStamp_ = 0;
        public int width_ = 0;
        public int height_= 0;
        public int stride_ = 0;
        public int format_ = 0;
    }

    //=========================================================================
    static public class SurfaceInfo {
        public SurfaceInfo(Surface surface, Long surfaceId) {
            surface_ = surface;
            surfaceId_ = surfaceId;
        }

        public Surface getSurface () {
            return surface_;
        }

        public Long getSurfaceId () {
            return surfaceId_;
        }


        private long surfaceId_ = 0;
        private Surface surface_ = null;
    }

    protected SurfaceInfo captureSurfaceInfo_ = null;
    protected SurfaceHolder previewSurfaceHolder_ = null;
    private Callback previewSerfaceCallback_ = null;
    protected int imageFormat_;
    private int cameraId_ = -1;

    //=========================================================================
    //
    //=========================================================================
    protected SecureSurface(int cameraId, int width, int height, int format, int numOfBuffers) {
        try {
            cameraId_ = cameraId;
            imageFormat_ = format;
            captureSurfaceInfo_ = SecCamServiceClient.getInstance().getSecureCameraSurface(
                    cameraId, width, height, format, numOfBuffers);
            Log.d(SECCAM_API_LOG_TAG, "SecureSurface::SecureSurface - CaptureSurface:" + captureSurfaceInfo_.getSurface()+
                    ", surfaceId: " + captureSurfaceInfo_.getSurfaceId());
        }
        catch (RuntimeException e) {
            Log.e(SECCAM_API_LOG_TAG, "SecureSurface::SecureSurface - ERROR: " + e);
        }
        catch (Exception e) {
            Log.e(SECCAM_API_LOG_TAG, "SecureSurface::SecureSurface - ERROR: " + e);
        }
    }

    //=========================================================================
    //
    //=========================================================================
    public Long getCaptureSurfaceId() {
        return captureSurfaceInfo_.getSurfaceId();
    }

    //=========================================================================
    //
    //=========================================================================
    public Surface getCaptureSurface() {
        return captureSurfaceInfo_.getSurface();
    }

    //=========================================================================
    //
    //=========================================================================
    public Surface getPreviewSurface() {
        if (previewSurfaceHolder_ != null) {
            return previewSurfaceHolder_.getSurface();
        }
        return null;
    }

    //=========================================================================
    //
    //=========================================================================
    protected boolean releaseCaptureSurface() {
        return SecCamServiceClient.getInstance().releaseCaptureSurface(this);
    }

    //=========================================================================
    //
    //=========================================================================
    public int getCameraId() {
        return cameraId_;
    }

    //=========================================================================
    //
    //=========================================================================
    public int getImageFormat() {
        return imageFormat_;
    }

    static public String imageFormatToString (int imageFormat) {
        switch(imageFormat) {
            case IMAGE_FORMAT_YUV420SP:         return "YUV420SP";
            case IMAGE_FORMAT_YUV420SP_UBWC:    return "YUV420SP_UBWC";
            case IMAGE_FORMAT_YUV420:           return "YUV420";
            case IMAGE_FORMAT_RAW:              return "RAW(PRIVATE)";
            case IMAGE_FORMAT_RAW10:            return "RAW10";
            case IMAGE_FORMAT_RAW_SENSOR:       return "RAW(SENSOR)";
            case IMAGE_FORMAT_PRIVATE:          return "PRIVATE";
            default:                            return "UNKNOWN(" + imageFormat + ")" ;
        }
    }

    //=========================================================================
    //
    //=========================================================================
    static public String rotationToString(int rotation) {
        switch(rotation) {
            case ROTATE_90: return "90";
            case ROTATE_180: return "180";
            case ROTATE_270: return "270";
            case FLIP_HORIZONTALLY: return "HFLIP";
            case FLIP_VERTICALLY: return "VFLIP";
            case ROTATE_90_HORIZONTAL_FLIP: return "90+HFLIP";
            case ROTATE_90_VERTICAL_FLIP: return "90+VFLIP";
            default: return "0" ;
        }
    }

    //=========================================================================
    //
    //=========================================================================
    public boolean release() {
        boolean result = true;

        if (captureSurfaceInfo_ != null) {
            result = SecCamServiceClient.getInstance().releaseCaptureSurface(this);
            captureSurfaceInfo_ = null;
        }
        return result;
    }

    //=========================================================================
    //
    //=========================================================================
    public boolean assignPreviewSurface(SurfaceHolder previewSurfaceHolder, int width, int height, int format, int rotation, int numOfBuffers) {
        boolean result = SecCamServiceClient.getInstance().setSecurePreviewSurface(previewSurfaceHolder.getSurface(),
                this, width, height, format, rotation, numOfBuffers);
        if (result) {
            previewSurfaceHolder_ = previewSurfaceHolder;
            previewSerfaceCallback_ = new SurfaceHolder.Callback() {
                @Override
                public void surfaceCreated(SurfaceHolder holder) {
                    Log.d(SECCAM_API_LOG_TAG, "SecureSurface::assignPreviewSurface::surfaceCreated");
                }

                @Override
                public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
                    Log.d(SECCAM_API_LOG_TAG, "SecureSurface::assignPreviewSurface::surfaceChanged");
                }

                @Override
                public void surfaceDestroyed(SurfaceHolder holder) {
                    Log.d(SECCAM_API_LOG_TAG, "SecureSurface::assignPreviewSurface::surfaceDestroyed - Enter");
                    SecCamServiceClient.getInstance().releasePreviewSurface(
                            previewSurfaceHolder_.getSurface(), captureSurfaceInfo_);
                    try {
                        Thread.sleep(200);
                    } catch (InterruptedException ex) {
                        Log.e(SECCAM_API_LOG_TAG, "SecureSurface::assignPreviewSurface::surfaceDestroyed " + ex);
                    }
                    previewSurfaceHolder_.removeCallback(previewSerfaceCallback_);
                    previewSurfaceHolder_ = null;
                    Log.d(SECCAM_API_LOG_TAG, "SecureSurface::assignPreviewSurface::surfaceDestroyed - Done");
                }
            };
            previewSurfaceHolder_.addCallback(previewSerfaceCallback_);

        }
        return result;
    }

    //=========================================================================
    //
    //=========================================================================
    public boolean enableFrameCallback(FrameCallback frameCallback, int returnParamsSize) {
        boolean result = SecCamServiceClient.getInstance().enableFrameCallback(
                this, frameCallback, returnParamsSize);
        return result;
    }

}
