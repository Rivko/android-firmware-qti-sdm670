// Copyright (c) 2017 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
package com.qualcomm.qti.seccamapi;

import java.util.ArrayList;
import java.util.concurrent.locks.ReentrantLock;

import android.graphics.Canvas;
import android.graphics.Rect;
import android.hardware.Camera;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceHolder.Callback;

import com.qualcomm.qti.seccamapi.SecureSurface;

public class SecureCameraSurface extends SecureSurface {
    private boolean useHAL1_ = false;

    //=========================================================================
    //
    //=========================================================================
    protected class SecureSurfaceHolder implements SurfaceHolder {

        private final ReentrantLock secureSurfaceLock_ = new ReentrantLock();
        private final ArrayList<SurfaceHolder.Callback> secureSurfaceHolderCallbacks_
            = new ArrayList<SurfaceHolder.Callback>();
        final Rect secureSurfaceFrame_ = new Rect();
        private SecureCameraSurface secureSurface_ = null;

        //=========================================================================
        public SecureSurfaceHolder(SecureCameraSurface surface) {
            secureSurface_ = surface;
        }

        //=========================================================================
        public boolean releaseSurface() {
            boolean result = true;
            if(secureSurface_ != null) {
                result = secureSurface_.releaseCaptureSurface();
                secureSurface_ = null;
            }
            return result;
        }

        //=========================================================================
        @Override
        public boolean isCreating() {
            return false;
        }

        //=========================================================================
        @Override
        public void addCallback(Callback callback) {
            synchronized (secureSurfaceHolderCallbacks_) {
                // This is a linear search, but in practice we'll
                // have only a couple callbacks, so it doesn't matter.
                if (secureSurfaceHolderCallbacks_.contains(callback) == false) {
                    secureSurfaceHolderCallbacks_.add(callback);
                }
            }
        }

        //=========================================================================
        @Override
        public void removeCallback(Callback callback) {
            synchronized (secureSurfaceHolderCallbacks_) {
                secureSurfaceHolderCallbacks_.remove(callback);
            }
        }

        //=========================================================================
        @Override
        public void setFixedSize(int width, int height) {  }

        //=========================================================================
        @Override
        public void setSizeFromLayout() { }

        //=========================================================================
        @Override
        public void setFormat(int format) { }

        //=========================================================================
        // @deprecated setType is now ignored.
        //=========================================================================
        @Override
        @Deprecated
        public void setType(int type) { }

        //=========================================================================
        @Override
        public void setKeepScreenOn(boolean screenOn) {  }

        //=========================================================================
        @Override
        public Canvas lockCanvas() {
            return null;
        }

        //=========================================================================
        @Override
        public Canvas lockCanvas(Rect inOutDirty) {
            return null;
        }

        //=========================================================================
        @Override
        public void unlockCanvasAndPost(Canvas canvas) {
            if (secureSurface_ != null && secureSurface_.getCaptureSurface() != null) {
                secureSurface_.getCaptureSurface().unlockCanvasAndPost(canvas);
                secureSurfaceLock_.unlock();
            }
        }

        //=========================================================================
        @Override
        public Surface getSurface() {
            return secureSurface_.getCaptureSurface();
        }

        //=========================================================================
        @Override
        public Rect getSurfaceFrame() {
            return secureSurfaceFrame_;
        }
    };

    private SecureSurfaceHolder captureSurfaceHolder_ = null;

    //=========================================================================
    //
    //=========================================================================
    private void Init(Camera.Parameters params, int width, int height, int format, int numOfBuffers) {
        if (useHAL1_) {
            params.set("secure-mode", "enable");
            switch (format) {
                case SecureSurface.IMAGE_FORMAT_RAW:
                    params.set("rdi-mode", "enable");
                    //Workaround for RDI overflow issue
                    if (numOfBuffers < 9) {
                        numOfBuffers = 9;
                    }
                    break;
                case SecureSurface.IMAGE_FORMAT_YUV420SP:
                    params.set("secure-mode-ubwc", "disable");
                    break;
                case SecureSurface.IMAGE_FORMAT_YUV420SP_UBWC:
                default:
                    params.set("secure-mode-ubwc", "enable");
                    break;
            }

            params.set("secure-mode-queue-depth", numOfBuffers);
        }

        captureSurfaceHolder_ = new SecureSurfaceHolder(this);
        captureSurfaceHolder_.addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder holder) {
                Log.d(SECCAM_API_LOG_TAG, "secureSurface::surfaceCreated - " + holder.getSurface().toString());
            }

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
                Log.d(SECCAM_API_LOG_TAG, "secureSurface::surfaceChanged  - " + holder.getSurface().toString());
            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {
                Log.d(SECCAM_API_LOG_TAG, "secureSurface::surfaceDestroyed - " + holder.getSurface().toString());
            }
        });
    }

    //=========================================================================
    //
    //=========================================================================
    public SecureCameraSurface(int cameraId, Camera.Parameters params, int width, int height, int format, int numOfBuffers) {
        super(cameraId, width, height, format, 0);
        useHAL1_ = true;
        Init(params, width, height, format, numOfBuffers);
    }

    //=========================================================================
    //
    //=========================================================================
    public SecureCameraSurface(int cameraId, Camera.Parameters params, int width, int height, int format, int numOfBuffers, boolean useHAL1) {
        super(cameraId, width, height, format, numOfBuffers);
        useHAL1_ = useHAL1;
        Init(params, width, height, format, numOfBuffers);
    }

    //=========================================================================
    //
    //=========================================================================
    public SurfaceHolder getSurfaceHolder() {
        return captureSurfaceHolder_;
    }
}
