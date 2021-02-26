// Copyright (c) 2017 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
package com.qualcomm.qti.seccamapi;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.IBinder;
import android.os.Message;
import android.os.Messenger;
import android.os.Parcelable;
import android.os.RemoteException;
import android.util.Log;
import android.view.Surface;

import android.hardware.Camera;

import java.lang.ref.WeakReference;
import java.util.Map;
import java.util.Hashtable;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.locks.ReentrantLock;

import com.qualcomm.qti.seccamapi.SecureSurface.FrameCallback;
import com.qualcomm.qti.seccamapi.SecureSurface.FrameInfo;

public class SecCamServiceClient extends Handler {

    private final static String LOG_TAG = "SECCAM-SERVICE-CLIENT";
    private final static String SERVICE_PACKAGE_NAME = "com.qualcomm.qti.seccamservice";
    private final static String SERVICE_NAME = "SecCamService";

    private static final int MSG_REPLAY_TIMEOUT = 2;
    private static final int SERVICE_READY_TIMEOUT = 2;

    private static final int MSG_SEVICE_VERSION = 1000;
    private static final int MSG_GET_CAPTURE_SURFACE = 1001;
    private static final int MSG_SET_PREVIEW_SURFACE = 1002;
    private static final int MSG_RELEASE_CAPTURE_SURFACE = 1003;
    private static final int MSG_RELEASE_PREVIEW_SURFACE = 1004;
    private static final int MSG_ENABLE_FRAME_CALLBACK = 1005;
    private static final int MSG_FRAME_CALLBACK = 1006;

    public static final Integer HAL1 = 1;
    public static final Integer HAL3 = 3;

    //=========================================================================
    public interface ClientCallback {
        public void serviceConnected();
        public void serviceDisconnected();
    }

    //=========================================================================
    public static class ServiceVersion {
        public int serviceVerMaj_;
        public int serviceVerMin_;
        public int jniVerMaj_;
        public int jniVerMin_;
        public int taVerMaj_;
        public int taVerMin_;
    }

    private WeakReference<Context> context_;
    private ClientCallback callback_ = null;
    private Messenger bundServiceMessenger_;
    private boolean serviceConnected_ = false;
    private boolean serviceConnecting_ = false;
    private boolean serviceDisonnecting_ = false;
    private Messenger activityMessenger_ = null;

    private Surface captureSurface_ = null;
    private Long surfaceId_;
    private boolean result_ = false;

    private CountDownLatch replayReadyLatch_ = null;
    private CountDownLatch serviceReadyLatch_ = null;

    private static SecCamServiceClient instance_ = null;
    private final ReentrantLock accessLock_ = new ReentrantLock();
    private static HandlerThread handlerThread_ = null;
    private Hashtable<Long, SecureSurface.FrameCallback> frameCallbacks_ = null;
    private ServiceVersion serviceVersion_ = new ServiceVersion();

    //=========================================================================
    private SecCamServiceClient() {
        super(handlerThread_.getLooper());
    }

    //=========================================================================
    public static SecCamServiceClient getInstance() {
        if (instance_ == null) {
            handlerThread_ = new HandlerThread("SecCamServiceClientThread");
            handlerThread_.start();
            instance_ = new SecCamServiceClient();
        }
        return instance_;
    }

    //=========================================================================
    public static Integer getMinSupportedHAL() {
        return getMinSupportedHAL(0);
    }

    //=========================================================================
    public static Integer getMinSupportedHAL(Integer cameraId) {
        Integer minHAL = HAL3;
        try {
            Camera camera = Camera.openLegacy(cameraId, Camera.CAMERA_HAL_API_VERSION_1_0);
            Camera.Parameters params = camera.getParameters();
            String param = params.get("secure-mode");
            if (param != null ) {
                minHAL = HAL1;
            }
            camera.release();
        }
        catch (Exception e) {
            Log.d(LOG_TAG, "getMinSupportedHAL - HAL1 probe failed (Camera:" + cameraId + "),"  + e);
        }
        return minHAL;
    }


    //=========================================================================
    public boolean start(Context context, ClientCallback callback) {
        return start(context, callback, null, 0, false);
    }

    //=========================================================================
    public boolean start(Context context, ClientCallback callback, String taName, int taBufferSize, boolean is_2dfa) {
        accessLock_.lock();
        if (false == serviceConnected_) {
            serviceConnecting_ = true;
            serviceReadyLatch_ = new CountDownLatch(1);
            frameCallbacks_ = new Hashtable<Long, SecureSurface.FrameCallback>();
            context_ = new WeakReference<Context>(context);
            callback_ = callback;
            activityMessenger_ = new Messenger(this);

            Intent intent = new Intent(SERVICE_PACKAGE_NAME + "." + SERVICE_NAME);
            intent.setPackage(SERVICE_PACKAGE_NAME);
            if (taName != null) {
                intent.putExtra("TANAME", taName);
                intent.putExtra("TABUFFERSIZE", taBufferSize);
                intent.putExtra("2DFA", is_2dfa);
                Log.d(LOG_TAG, "Start service, TA: " + taName + "2DFA " + is_2dfa);
            }
            else {
                Log.d(LOG_TAG, "Start service");
            }
            //Don't start service explicitly
            //context_.get().startService(intent);
            context_.get().bindService(intent, serviceConnection_, Context.BIND_AUTO_CREATE);
        }
        accessLock_.unlock();
        return true;
    }

    //=========================================================================
    public void release() {
        accessLock_.lock();
        if (serviceConnected_) {
            serviceDisonnecting_ = true;
            serviceReadyLatch_ = new CountDownLatch(1);
            Log.d(LOG_TAG, "unbindService");
            context_.get().unbindService(serviceConnection_);
            serviceConnected_ = false;
            context_ = null;
            callback_ = null;
        }
        accessLock_.unlock();
    }

    //=========================================================================
    public boolean isSeviceConnected() {
        if(serviceConnecting_ || serviceDisonnecting_) {
            try {
                if(!serviceReadyLatch_.await(SERVICE_READY_TIMEOUT, TimeUnit.SECONDS)) {
                    Log.d(LOG_TAG, "isSeviceConnected - ERROR: tmeout!");
                    return false;
                }
            } catch (InterruptedException ex) {
                Log.d(LOG_TAG, "isSeviceConnected - ERROR: " + ex);
                ex.printStackTrace();
                return false;
            }
        }
        return serviceConnected_;
    }

    //=========================================================================
    public SecureSurface.SurfaceInfo getSecureCameraSurface(
            int cameraId, int width, int height, int format, int numOfBuffers) {
        accessLock_.lock();
        if (isSeviceConnected()) {
            try {
                Log.d(LOG_TAG, "Send MSG: getSecureCameraSurface");
                Bundle out_bundle = new Bundle();
                out_bundle.putInt("cameraId", cameraId);
                out_bundle.putInt("width", width);
                out_bundle.putInt("height", height);
                out_bundle.putInt("format", format);
                out_bundle.putInt("numOfBuffers", numOfBuffers);
                Message msg = Message.obtain();
                msg.what = MSG_GET_CAPTURE_SURFACE;
                msg.setData(out_bundle);
                msg.replyTo = activityMessenger_;
                replayReadyLatch_ = new CountDownLatch(1);
                captureSurface_ = null;
                surfaceId_ = 0L;
                bundServiceMessenger_.send(msg);
                try {
                    if(!replayReadyLatch_.await(MSG_REPLAY_TIMEOUT, TimeUnit.SECONDS)) {
                        Log.d(LOG_TAG, "getSecureCameraSurface - ERROR: tmeout!");
                    }
                    else{
                        SecureSurface.SurfaceInfo surfaceInfo = new SecureSurface.SurfaceInfo(
                                captureSurface_, surfaceId_);
                        captureSurface_ = null;
                        surfaceId_ = 0L;
                        accessLock_.unlock();
                        return surfaceInfo;
                    }
                } catch (InterruptedException ex) {
                    Log.d(LOG_TAG, "getSecureCameraSurface - ERROR: " + ex);
                    ex.printStackTrace();
                }

            } catch (RemoteException ex) {
                Log.d(LOG_TAG, "getSecureCameraSurface - ERROR: " + ex);
                ex.printStackTrace();
            }
        }
        accessLock_.unlock();
        return null;
    }

    //=========================================================================
    public boolean setSecurePreviewSurface(Surface previewSurface, SecureSurface captureSurface,
            int width, int height, int format, int rotation, int numOfBuffers) {
        accessLock_.lock();
        if (isSeviceConnected() &&
                previewSurface != null && captureSurface != null) {
            try {
                Log.d(LOG_TAG, "Send MSG: setSecurePreviewSurface");
                Bundle out_bundle = new Bundle();
                out_bundle.putParcelable("PSURFACE", previewSurface);
                out_bundle.putLong("cSurfaceId", captureSurface.getCaptureSurfaceId());
                out_bundle.putInt("width", width);
                out_bundle.putInt("height", height);
                out_bundle.putInt("format", format);
                out_bundle.putInt("rotation", rotation);
                out_bundle.putInt("numOfBuffers", numOfBuffers);
                Message msg = Message.obtain();
                msg.what = MSG_SET_PREVIEW_SURFACE;
                msg.setData(out_bundle);
                msg.replyTo = activityMessenger_;
                replayReadyLatch_ = new CountDownLatch(1);
                result_ = false;
                bundServiceMessenger_.send(msg);
                try {
                    if(!replayReadyLatch_.await(MSG_REPLAY_TIMEOUT, TimeUnit.SECONDS)) {
                        Log.d(LOG_TAG, "setSecurePreviewSurface - ERROR: timeout!");
                    }
                    else {
                        accessLock_.unlock();
                        return result_;
                    }
                } catch (InterruptedException ex) {
                    Log.d(LOG_TAG, "setSecurePreviewSurface - ERROR: " + ex);
                    ex.printStackTrace();
                }

            } catch (RemoteException ex) {
                Log.d(LOG_TAG, "setSecurePreviewSurface - ERROR: " + ex);
                ex.printStackTrace();
            }
        }
        accessLock_.unlock();
        return false;
    }

    //=========================================================================
    public boolean releaseCaptureSurface(SecureSurface secureSurface) {
        accessLock_.lock();
        if (isSeviceConnected() &&
                secureSurface != null && secureSurface.getCaptureSurface() != null) {
            try {
                Log.d(LOG_TAG, "Send MSG: releaseCaptureSurface");
                Bundle out_bundle = new Bundle();
                out_bundle.putLong("cSurfaceId", secureSurface.getCaptureSurfaceId());
                Message msg = Message.obtain();
                msg.what = MSG_RELEASE_CAPTURE_SURFACE;
                msg.setData(out_bundle);
                msg.replyTo = activityMessenger_;
                replayReadyLatch_ = new CountDownLatch(1);
                result_ = false;
                bundServiceMessenger_.send(msg);
                try {
                    if(!replayReadyLatch_.await(MSG_REPLAY_TIMEOUT, TimeUnit.SECONDS)) {
                        Log.d(LOG_TAG, "releaseCaptureSurface - ERROR: timeout!");
                    }
                    else {
                        accessLock_.unlock();
                        return result_;
                    }
                } catch (InterruptedException ex) {
                    Log.d(LOG_TAG, "releaseCaptureSurface - ERROR: " + ex);
                    ex.printStackTrace();
                }

            } catch (RemoteException ex) {
                Log.d(LOG_TAG, "releaseCaptureSurface - ERROR: " + ex);
                ex.printStackTrace();
            }
        }
        accessLock_.unlock();
        return false;
    }

    //=========================================================================
    public boolean releasePreviewSurface(Surface previewSurface, SecureSurface.SurfaceInfo captureSurfaceInfo) {
        accessLock_.lock();
        if (isSeviceConnected() && captureSurfaceInfo != null && previewSurface != null) {
            try {
                Log.d(LOG_TAG, "Send MSG: releasePreviewSurface");
                Bundle out_bundle = new Bundle();
                out_bundle.putParcelable("PSURFACE", previewSurface);
                out_bundle.putLong("cSurfaceId", captureSurfaceInfo.getSurfaceId());
                Message msg = Message.obtain();
                msg.what = MSG_RELEASE_PREVIEW_SURFACE;
                msg.setData(out_bundle);
                msg.replyTo = activityMessenger_;
                replayReadyLatch_ = new CountDownLatch(1);
                result_ = false;
                bundServiceMessenger_.send(msg);
                try {
                    if(!replayReadyLatch_.await(MSG_REPLAY_TIMEOUT, TimeUnit.SECONDS)) {
                        Log.d(LOG_TAG, "releasePreviewSurface - ERROR: timeout!");
                    }
                    else {
                        accessLock_.unlock();
                        return result_;
                    }
                } catch (InterruptedException ex) {
                    Log.d(LOG_TAG, "releasePreviewSurface - ERROR: " + ex);
                    ex.printStackTrace();
                }

            } catch (RemoteException ex) {
                Log.d(LOG_TAG, "releasePreviewSurface - ERROR: " + ex);
                ex.printStackTrace();
            }
        }
        accessLock_.unlock();
        return false;
    }

    //=========================================================================
    public ServiceVersion getServiceVersion() {
        accessLock_.lock();
        if (isSeviceConnected()) {
            try {
                Log.d(LOG_TAG, "Send MSG: getServiceVersion");
                Message msg = Message.obtain(null, MSG_SEVICE_VERSION, 0, 0);
                msg.replyTo = activityMessenger_;
                replayReadyLatch_ = new CountDownLatch(1);
                bundServiceMessenger_.send(msg);
                try {
                    if(!replayReadyLatch_.await(MSG_REPLAY_TIMEOUT, TimeUnit.SECONDS)) {
                        Log.d(LOG_TAG, "getServiceVersion - ERROR: timeout!");
                    }
                    else {
                        accessLock_.unlock();
                        return serviceVersion_;
                    }
                } catch (InterruptedException ex) {
                    Log.d(LOG_TAG, "getServiceVersion - ERROR: " + ex);
                    ex.printStackTrace();
                }
            } catch (RemoteException e) {
                e.printStackTrace();
            }
        }
        accessLock_.unlock();
        return null;
    }

    //=========================================================================
    public boolean enableFrameCallback(SecureSurface captureSurface,
            SecureSurface.FrameCallback callback, int returnParamsSize) {
        accessLock_.lock();
        if (isSeviceConnected() &&
                captureSurface != null && captureSurface.getCaptureSurface() != null &&
                callback != null) {
            try {
                Log.d(LOG_TAG, "Send MSG: enableFrameCallback");
                Bundle out_bundle = new Bundle();
                out_bundle.putInt("timeout", 100);
                out_bundle.putLong("cSurfaceId", captureSurface.getCaptureSurfaceId());
                out_bundle.putInt("returnParamsSize", returnParamsSize);
                Message msg = Message.obtain();
                msg.what = MSG_ENABLE_FRAME_CALLBACK;
                msg.setData(out_bundle);
                msg.replyTo = activityMessenger_;
                replayReadyLatch_ = new CountDownLatch(1);
                result_ = false;
                bundServiceMessenger_.send(msg);
                try {
                    if(!replayReadyLatch_.await(MSG_REPLAY_TIMEOUT, TimeUnit.SECONDS)) {
                        Log.d(LOG_TAG, "enableFrameCallback - ERROR: timeout!");
                    }
                    else {
                        if (result_) {
                            Log.d(LOG_TAG, "enableFrameCallback - Register callback, surfaceId: " +
                                    captureSurface.getCaptureSurfaceId());
                            frameCallbacks_.put(captureSurface.getCaptureSurfaceId(), callback);
                        }
                        accessLock_.unlock();
                        return result_;
                    }
                } catch (InterruptedException ex) {
                    Log.d(LOG_TAG, "enableFrameCallback - ERROR: " + ex);
                    ex.printStackTrace();
                }

            } catch (RemoteException ex) {
                Log.d(LOG_TAG, "enableFrameCallback - ERROR: " + ex);
                ex.printStackTrace();
            }
        }
        accessLock_.unlock();
        return false;
    }

    //=========================================================================

    public void dispatchVendorCommand(int commandId, Bundle bundle) {
        accessLock_.lock();
        if (isSeviceConnected()) {
            try {
                Message msg = Message.obtain();
                msg.what = commandId;
                if (bundle != null) {
                    msg.setData(bundle);
                }
                msg.replyTo = activityMessenger_;
                replayReadyLatch_ = new CountDownLatch(1);
                bundServiceMessenger_.send(msg);
                try {
                    if(!replayReadyLatch_.await(MSG_REPLAY_TIMEOUT, TimeUnit.SECONDS)) {
                        Log.d(LOG_TAG, "dispatchVendorCommand - ERROR: timeout!");
                    }
                    else {
                        accessLock_.unlock();
                        return;
                    }
                } catch (InterruptedException ex) {
                    Log.d(LOG_TAG, "dispatchVendorCommand - ERROR: " + ex);
                    ex.printStackTrace();
                }

            } catch (RemoteException ex) {
                Log.d(LOG_TAG, "dispatchVendorCommand - ERROR: " + ex);
                ex.printStackTrace();
            }
        }
        accessLock_.unlock();
    }

    //=========================================================================
    private ServiceConnection serviceConnection_ = new ServiceConnection() {
        @Override
        public void onServiceDisconnected(ComponentName name) {
            Log.d(LOG_TAG, "onServiceDisconnected");
            bundServiceMessenger_ = null;
            serviceDisonnecting_ = false;
            serviceConnected_ = false;
            serviceReadyLatch_.countDown();
            if (callback_ != null) {
                callback_.serviceDisconnected();
            }
        }

        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            Log.d(LOG_TAG, "onServiceConnected");
            bundServiceMessenger_ = new Messenger(service);
            serviceConnecting_ = false;
            serviceConnected_ = true;
            serviceReadyLatch_.countDown();
            if (callback_ != null) {
                callback_.serviceConnected();
            }
        }
    };

    //=========================================================================
    @Override
    public void handleMessage(Message msg) {
        Log.d(LOG_TAG, "handleMessage");
        switch (msg.what) {
            case MSG_SEVICE_VERSION: {
                Bundle in_bundle = msg.getData();
                serviceVersion_.serviceVerMaj_ = in_bundle.getInt("svc_ver_maj");
                serviceVersion_.serviceVerMin_= in_bundle.getInt("svc_ver_min");
                serviceVersion_.jniVerMaj_= in_bundle.getInt("jni_ver_maj");
                serviceVersion_.jniVerMin_= in_bundle.getInt("jni_ver_min");
                serviceVersion_.taVerMaj_= in_bundle.getInt("ta_ver_maj");
                serviceVersion_.taVerMin_= in_bundle.getInt("ta_ver_min");
                Log.d(LOG_TAG, "handleMessage - MSG_SEVICE_VERSION:" +
                        "SVC: v" + serviceVersion_.serviceVerMaj_ + "." + serviceVersion_.serviceVerMin_ + ", " +
                        "JNI: v" + serviceVersion_.jniVerMaj_ + "." + serviceVersion_.jniVerMin_ + ", " +
                        "TA: v" + serviceVersion_.taVerMaj_ + "." + serviceVersion_.taVerMin_);
                replayReadyLatch_.countDown();
                break;
            }
            case MSG_GET_CAPTURE_SURFACE: {
                Bundle in_bundle = msg.getData();
                in_bundle.setClassLoader(this.getClass().getClassLoader());
                Parcelable parcelable = in_bundle.getParcelable("SURFACE");
                if (parcelable instanceof Surface) {
                    captureSurface_ = (Surface) parcelable;
                    surfaceId_ = in_bundle.getLong("surfaceId");
                    Log.d(LOG_TAG, "handleMessage - MSG_GET_CAPTURE_SURFACE:" + captureSurface_.toString()+
                            ", surfaceId: " + surfaceId_);
                }
                replayReadyLatch_.countDown();
                break;
            }
            case MSG_SET_PREVIEW_SURFACE:
            case MSG_RELEASE_CAPTURE_SURFACE:
            case MSG_RELEASE_PREVIEW_SURFACE: {
                Bundle in_bundle = msg.getData();
                result_ = in_bundle.getBoolean("result");
                Log.d(LOG_TAG, "handleMessage - " + msg.toString() + ":" + result_);
                replayReadyLatch_.countDown();
                break;
            }
            case MSG_ENABLE_FRAME_CALLBACK: {
                Bundle in_bundle = msg.getData();
                result_ = in_bundle.getBoolean("result");
                Log.d(LOG_TAG, "handleMessage - " + MSG_ENABLE_FRAME_CALLBACK + ":" + result_);
                replayReadyLatch_.countDown();
                break;
            }
            case MSG_FRAME_CALLBACK: {
                Bundle in_bundle = msg.getData();
                boolean result = in_bundle.getBoolean("result");

                if (result) {
                    SecureSurface.FrameInfo frameInfo = new SecureSurface.FrameInfo();
                    long surfaceId = in_bundle.getLong("surfaceId");
                    frameInfo.frameNumber_ = in_bundle.getLong("frameNumber");
                    frameInfo.timeStamp_ = in_bundle.getLong("timeStamp");
                    frameInfo.width_ = in_bundle.getInt("width");
                    frameInfo.height_ = in_bundle.getInt("height");
                    frameInfo.stride_ = in_bundle.getInt("stride");
                    frameInfo.format_ = in_bundle.getInt("format");
                    byte[] returnParams = in_bundle.getByteArray("returnParams");
                    Log.d(LOG_TAG, "handleMessage - MSG_FRAME_CALLBACK: SurfaceId:" + surfaceId +
                            " FrameId: " + frameInfo.frameNumber_);
                    SecureSurface.FrameCallback callback = frameCallbacks_.get(surfaceId);
                    if (callback != null) {
                        callback.onSecureFrameAvalable(frameInfo, returnParams);
                    }
                }
                break;
            }
            default: {
                // In case an unfamiliar message was recieved, we check if it is a vendor specific one
                // and handle it accordingly
                SecCamServiceVendorClient vendorClient = new SecCamServiceVendorClient();
                if (vendorClient.handleVendorMessage(msg)) {
                    replayReadyLatch_.countDown();
                }
            }
        }
    }
}
