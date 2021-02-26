// Copyright (c) 2017 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
package com.qualcomm.qti.seccamservice;

import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.Map;
import java.util.Hashtable;
import android.app.Service;
import android.content.Intent;
import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.Messenger;
import android.os.Parcelable;
import android.os.RemoteException;
import android.os.SystemClock;
import android.util.Log;
import android.view.Surface;
import android.widget.Chronometer;
import android.os.SystemProperties;

import com.qualcomm.qti.seccamservice.SecCamServiceVendorHandler;

public class SecCamService extends Service {

    public static final int SERVICE_VERSION_MAJ = 2;
    public static final int SERVICE_VERSION_MIN = 2;
    public static final int SUPPORTED_JNI_VERSION_MAJ = 2;

    public static final int JNI_OK = 0;
    public static final int JNI_EPERM = -1;
    public static final int JNI_EINVAL = -22;
    public static final int JNI_ETIMEDOUT = -110;
    private final static String LOG_TAG = "SECCAM-SERVICE";
    private static final String TZ_APP_NAME = "seccamdemo64";
    private static final int TZ_APP_BUFFER_SIZE = (4*1024);
    private static final int MSG_SEVICE_VERSION = 1000;
    private static final int MSG_GET_CAPTURE_SURFACE = 1001;
    private static final int MSG_SET_PREVIEW_SURFACE = 1002;
    private static final int MSG_RELEASE_CAPTURE_SURFACE = 1003;
    private static final int MSG_RELEASE_PREVIEW_SURFACE = 1004;
    private static final int MSG_ENABLE_FRAME_CALLBACK = 1005;
    private static final int MSG_FRAME_CALLBACK = 1006;

    private static class TZappInfo {
        public int verMaj_ = 0;
        public int verMin_ = 0;
        public String appName_;
    }

    private static class JniInfo {
        public int verMaj_ = 0;
        public int verMin_ = 0;
    }

    public static class FrameInfo {
        public long frameNumber_ = 0;
        public long timeStamp_ = 0;
        public int width_ = 0;
        public int height_= 0;
        public int stride_ = 0;
        public int format_ = 0;
    }

    private static class SurfaceInfo {
        public long surfaceId_ = 0;
    }

    private static TZappInfo tzAppInfo_ = new TZappInfo();
    private static JniInfo jniInfo_ = new JniInfo();
    private static  Context serviceContext = null;
    private boolean tzAppIfAvalable_ = false;
    private Chronometer chronometer_;
    private static Hashtable<Long, SecCamServiceHandler.FrameCallbackThread> frameCallbacks_ = null;
    private String taName_ = TZ_APP_NAME;
    private int taBufferSize_ = TZ_APP_BUFFER_SIZE;
    private boolean is_2dfa = false;

    //=========================================================================
    // JNI API
    //=========================================================================
    private static native Surface getSecureCameraSurface(
            int cameraId, int width, int height, int format, int numOfBuffers,
            SurfaceInfo surfaceInfo);
    private static native boolean releaseSecureCameraSurface(long cSurfaceId);
    private static native boolean setSecurePreviewSurface(Surface previewSurface, long cSurfaceId,
            int width, int height, int format, int rotation, int numOfBuffers);
    private static native boolean releaseSecurePreviewSurface(Surface previewSurface, long cSurfaceId);

    private native boolean getJniVersion(JniInfo jniInfo);
    private native int doStartTzApp(String str, int buffSize, TZappInfo appInfo, boolean is_2dfa);
    private native int doShutdownTzApp();

    private static native boolean enableFrameCallback(long cSurfaceId, int returnParamsSize);
    private static native int isFrameAvailable(long cDurfaceId, int timeout,
            FrameInfo frameInfo, byte[] returnParams, int returnParamsSize);

    static {
        System.loadLibrary("seccamservice");
    }

    //=========================================================================
    //
    //=========================================================================
    static class SecCamServiceHandler extends Handler {
         private final WeakReference<SecCamService> service_;

         public SecCamServiceHandler(SecCamService service) {
             service_ = new WeakReference<SecCamService>(service);
         }

         //=========================================================================
         //
         //=========================================================================
         public class FrameCallbackThread extends Thread {
            private Messenger callbackActivityMessenger_ = null;
            private long surfaceId_ = 0;
            private int timeout_ = 0;
            private int returnParamsSize_ = 0;
            private boolean done_ = false;

            public FrameCallbackThread(Messenger callbackActivityMessenger,
                 long surfaceId, int timeout, int returnParamsSize) {
                callbackActivityMessenger_ = callbackActivityMessenger;
                surfaceId_ = surfaceId;
                timeout_ = timeout;
                returnParamsSize_ = returnParamsSize;
            }

            public void shutdown() {
                done_ = true;
            }

            @Override
            public void run() {
                SecCamService.FrameInfo frameInfo = new FrameInfo();
                byte[] returnParams = new byte[returnParamsSize_];

                while(!done_) {
                    int result = isFrameAvailable(surfaceId_, timeout_,
                            frameInfo, returnParams, returnParamsSize_);
                    if (result == JNI_OK) {
                        Message replyMsg = Message.obtain();
                        replyMsg.what = MSG_FRAME_CALLBACK;
                        Bundle out_bundle = new Bundle();
                        out_bundle.putLong("surfaceId", surfaceId_);
                        out_bundle.putLong("frameNumber", frameInfo.frameNumber_);
                        out_bundle.putLong("timeStamp", frameInfo.timeStamp_);
                        out_bundle.putInt("width", frameInfo.width_);
                        out_bundle.putInt("height", frameInfo.height_);
                        out_bundle.putInt("stride", frameInfo.stride_);
                        out_bundle.putInt("format", frameInfo.format_);
                        out_bundle.putByteArray("returnParams", returnParams);
                        out_bundle.putBoolean("result", true);
                        Log.d(LOG_TAG, "::FrameCallbackThread::run - " +
                                "SurfaceId:" + surfaceId_ + ", " +
                                "FrameId: " + frameInfo.frameNumber_ + ", " +
                                "returnParamsSize: " + returnParamsSize_);
                        replyMsg.setData(out_bundle);
                        try {
                            callbackActivityMessenger_.send(replyMsg);
                        } catch (RemoteException e) {
                           e.printStackTrace();
                        }
                    }
                    else if(result != JNI_ETIMEDOUT) {
                        Log.e(LOG_TAG, "The Frame Callback is not available!");
                        break;
                    }
                }
                Log.d(LOG_TAG, "::FrameCallbackThread::run - Done, " +
                        " SurfaceId:" + surfaceId_);
            }
        }

        //=========================================================================
        //
        //=========================================================================
        private void handleMessage_MSG_GET_CAPTURE_SURFACE(Message msg) {
            Log.d(LOG_TAG, "::ihandleMessage_MSG_GET_CAPTURE_SURFACE");
            Bundle in_bundle = msg.getData();
            int cameraId = in_bundle.getInt("cameraId");
            int width = in_bundle.getInt("width");
            int height = in_bundle.getInt("height");
            int format = in_bundle.getInt("format");
            int numOfBuffers = in_bundle.getInt("numOfBuffers");

            Messenger activityMessenger = msg.replyTo;
            Message replyMsg = Message.obtain();
            replyMsg.what = MSG_GET_CAPTURE_SURFACE;
            Bundle out_bundle = new Bundle();
            SurfaceInfo surfaceInfo = new SurfaceInfo();
            Surface surface = getSecureCameraSurface(cameraId, width, height, format, numOfBuffers, surfaceInfo);
            Log.d(LOG_TAG, "::handleMessage_MSG_GET_CAPTURE_SURFACE =" + surface.toString() +
                    ", surfaceId: " + surfaceInfo.surfaceId_);
            out_bundle.putParcelable("SURFACE", surface);
            out_bundle.putLong("surfaceId", surfaceInfo.surfaceId_);
            replyMsg.setData(out_bundle);
            try {
                activityMessenger.send(replyMsg);
            } catch (RemoteException e) {
                e.printStackTrace();
            }
        }

        //=========================================================================
        //
        //=========================================================================
        private void handleMessage_MSG_SET_PREVIEW_SURFACE(Message msg) {
            Log.d(LOG_TAG, "::handleMessage_MSG_SET_PREVIEW_SURFACE");
            Bundle in_bundle = msg.getData();
            int width = in_bundle.getInt("width");
            int height = in_bundle.getInt("height");
            int format = in_bundle.getInt("format");
            int rotation = in_bundle.getInt("rotation");
            int numOfBuffers = in_bundle.getInt("numOfBuffers");

            in_bundle.setClassLoader(this.getClass().getClassLoader());
            Surface previewSurface = null;
            Long cSurfaceId = in_bundle.getLong("cSurfaceId");
            Log.d(LOG_TAG, "::handleMessage_MSG_SET_PREVIEW_SURFACE - CaptureSurfaceId:" + cSurfaceId);
            Parcelable parcelable = in_bundle.getParcelable("PSURFACE");
            if (parcelable instanceof Surface) {
                previewSurface = (Surface) parcelable;
                Log.d(LOG_TAG, "::handleMessage_MSG_SET_PREVIEW_SURFACE - PreviewSurface:" + previewSurface.toString());
            }

            Messenger activityMessenger = msg.replyTo;
            Message replyMsg = Message.obtain();
            replyMsg.what = MSG_SET_PREVIEW_SURFACE;
            Bundle out_bundle = new Bundle();
            boolean result = setSecurePreviewSurface(previewSurface, cSurfaceId, width, height, format, rotation, numOfBuffers);

            //The parcelable instance increases the Java object reference count for the Surface,
            //therefore we release the recieved surface object. This assures the surface is released
            //when the preview session is completed. A reference to the surface is kept within the JNI, until
            //it is released during the MSG_RELEASE_PREVIEW_SURFACE handler.
            previewSurface.release();

            Log.v(LOG_TAG, "::handleMessage_MSG_SET_PREVIEW_SURFACE - " + result);
            out_bundle.putBoolean("result", result);
            replyMsg.setData(out_bundle);
            try {
                activityMessenger.send(replyMsg);
            } catch (RemoteException e) {
                e.printStackTrace();
            }
        }

        //=========================================================================
        //
        //=========================================================================
        private void handleMessage_MSG_RELEASE_CAPTURE_SURFACE(Message msg) {
            Log.d(LOG_TAG, "::handleMessage_MSG_RELEASE_CAPTURE_SURFACE");
            Bundle in_bundle = msg.getData();
            in_bundle.setClassLoader(this.getClass().getClassLoader());
            Surface surface = null;
            Long cSurfaceId = in_bundle.getLong("cSurfaceId");

            if (frameCallbacks_ != null && cSurfaceId != 0) {
                FrameCallbackThread val = frameCallbacks_.get(cSurfaceId);
                if (val != null) {
                    Log.d(LOG_TAG, "::handleMessage_MSG_RELEASE_CAPTURE_SURFACE - Disable Frame Callback - " + cSurfaceId);
                    try {
                        if(val.isAlive()) {
                            val.shutdown();
                        }
                    } catch (Exception e) {
                        Log.w(LOG_TAG, "::handleMessage_MSG_RELEASE_CAPTURE_SURFACE - Callback thread is not active");
                    }
                    frameCallbacks_.remove(cSurfaceId);
                }
            }

            Messenger activityMessenger = msg.replyTo;
            Message replyMsg = Message.obtain();
            replyMsg.what = MSG_RELEASE_CAPTURE_SURFACE;
            Bundle out_bundle = new Bundle();
            boolean result = releaseSecureCameraSurface(cSurfaceId);
            Log.d(LOG_TAG, "::handleMessage_MSG_RELEASE_CAPTURE_SURFACE - " + result);
            out_bundle.putBoolean("result", result);
            replyMsg.setData(out_bundle);
            try {
                activityMessenger.send(replyMsg);
            } catch (RemoteException e) {
                e.printStackTrace();
            }
        }

        //=========================================================================
        //
        //=========================================================================
        private void handleMessage_MSG_RELEASE_PREVIEW_SURFACE(Message msg) {
            Log.d(LOG_TAG, "::handleMessage_MSG_RELEASE_PREVIEW_SURFACE");
            Bundle in_bundle = msg.getData();
            in_bundle.setClassLoader(this.getClass().getClassLoader());
            Surface surface = null;

            Surface captureSurface = null;
            Surface previewSurface = null;
            Long cSurfaceId = in_bundle.getLong("cSurfaceId");
            Log.d(LOG_TAG, "::handleMessage_MSG_SET_PREVIEW_SURFACE - CaptureSurface:" + cSurfaceId);
            Parcelable parcelable = in_bundle.getParcelable("PSURFACE");
            if (parcelable instanceof Surface) {
                previewSurface = (Surface) parcelable;
                Log.d(LOG_TAG, "::handleMessage_MSG_SET_PREVIEW_SURFACE - PreviewSurface:" + previewSurface.toString());
            }

            Messenger activityMessenger = msg.replyTo;
            Message replyMsg = Message.obtain();
            replyMsg.what = MSG_RELEASE_PREVIEW_SURFACE;
            Bundle out_bundle = new Bundle();
            boolean result = releaseSecurePreviewSurface(previewSurface, cSurfaceId);
            Log.d(LOG_TAG, "::handleMessage_MSG_RELEASE_PREVIEW_SURFACE - " + result);
            out_bundle.putBoolean("result", result);
            replyMsg.setData(out_bundle);
            try {
                activityMessenger.send(replyMsg);
            } catch (RemoteException e) {
                e.printStackTrace();
            }
        }

        //=========================================================================
        //
        //=========================================================================
        protected void handleMessage_MSG_ENABLE_FRAME_CALLBACK(Message msg) {
            Log.d(LOG_TAG, "::handleMessage_MSG_ENABLE_FRAME_CALLBACK");
            Bundle in_bundle = msg.getData();
            in_bundle.setClassLoader(this.getClass().getClassLoader());
            Messenger activityMessenger = msg.replyTo;
            Message replyMsg = Message.obtain();
            replyMsg.what = MSG_ENABLE_FRAME_CALLBACK;
            Bundle out_bundle = new Bundle();

            Surface surface = null;
            boolean result = false;
            Long cSurfaceId = in_bundle.getLong("cSurfaceId");
            int returnParamsSize = in_bundle.getInt("returnParamsSize");
            int timeout = in_bundle.getInt("timeout");
            Log.d(LOG_TAG, "::handleMessage_MSG_ENABLE_FRAME_CALLBACK - SurfaceId:" + cSurfaceId);
            result = enableFrameCallback(cSurfaceId, returnParamsSize);
            out_bundle.putBoolean("result", result);
            replyMsg.setData(out_bundle);
            try {
                activityMessenger.send(replyMsg);
            } catch (RemoteException e) {
                e.printStackTrace();
            }

            if (result == true) {
                if (frameCallbacks_ == null) {
                    frameCallbacks_ = new Hashtable<Long, FrameCallbackThread>();
                }
                //callback thread
                FrameCallbackThread frameCalbackThread = new FrameCallbackThread(activityMessenger,
                        cSurfaceId, timeout, returnParamsSize);
                frameCallbacks_.put(cSurfaceId, frameCalbackThread);
                frameCalbackThread.start();
            }
        }

        //=========================================================================
        //
        //=========================================================================
        @Override
        public void handleMessage(Message msg) {

            if (!isUidPermitted(msg.sendingUid)) {
                Log.e(LOG_TAG, "The uid " + msg.sendingUid +  " is not authorized for seccamservice");
                return;
            }

            switch (msg.what) {
                case MSG_GET_CAPTURE_SURFACE:
                    handleMessage_MSG_GET_CAPTURE_SURFACE(msg);
                    break;
                case MSG_SET_PREVIEW_SURFACE:
                    handleMessage_MSG_SET_PREVIEW_SURFACE(msg);
                    break;
                case MSG_RELEASE_CAPTURE_SURFACE:
                    handleMessage_MSG_RELEASE_CAPTURE_SURFACE(msg);
                    break;
                case MSG_RELEASE_PREVIEW_SURFACE:
                    handleMessage_MSG_RELEASE_PREVIEW_SURFACE(msg);
                    break;
                case MSG_ENABLE_FRAME_CALLBACK:
                    handleMessage_MSG_ENABLE_FRAME_CALLBACK(msg);
                    break;
                case MSG_SEVICE_VERSION: {
                        Log.d(LOG_TAG, "::handleMessage - MSG_SEVICE_VERSION");
                        Bundle out_bundle = new Bundle();
                        Messenger activityMessenger = msg.replyTo;
                        out_bundle.putInt("svc_ver_maj", SERVICE_VERSION_MAJ);
                        out_bundle.putInt("svc_ver_min", SERVICE_VERSION_MIN);
                        out_bundle.putInt("jni_ver_maj", jniInfo_.verMaj_);
                        out_bundle.putInt("jni_ver_min", jniInfo_.verMin_);
                        out_bundle.putInt("ta_ver_maj", tzAppInfo_.verMaj_);
                        out_bundle.putInt("ta_ver_min", tzAppInfo_.verMin_);
                        Message replyMsg = Message.obtain(null, MSG_SEVICE_VERSION);
                        replyMsg.setData(out_bundle);
                        try {
                            activityMessenger.send(replyMsg);
                        } catch (RemoteException e) {
                            e.printStackTrace();
                        }
                    }
                    break;
                default: {
                    // In case an unfamiliar message was recieved, we check if it is a vendor specific one
                    // and handle it accordingly
                    try {
                        SecCamServiceVendorHandler vendorHandler = new SecCamServiceVendorHandler();
                        if (!vendorHandler.handleVendorMessage(msg)) {
                            super.handleMessage(msg);
                        }
                    } catch (UnsatisfiedLinkError e) {
                        Log.e(LOG_TAG, "seccam_vendor lib is unavailable");
                        return;
                    }
                }
            }
        }
    }

    final Messenger mMessenger = new Messenger(new SecCamServiceHandler(this));

    //=========================================================================
    //
    //=========================================================================
    public boolean init() {
        Log.d(LOG_TAG, "::init - TA: " + taName_);
        if (tzAppIfAvalable_ == false) {
            jniInfo_.verMaj_ = 0;
            jniInfo_.verMin_ = 0;
            if (true == getJniVersion(jniInfo_)) {
                Log.d(LOG_TAG, "::init - JNI v" +
                        jniInfo_.verMaj_ + "." + jniInfo_.verMin_ + "");
            }
            if (jniInfo_.verMaj_ != SUPPORTED_JNI_VERSION_MAJ) {
                Log.w(LOG_TAG, "::init - Unsupported JNI API version " +
                        jniInfo_.verMaj_ + ", version" +  SUPPORTED_JNI_VERSION_MAJ +
                        "is expected");
            }
            tzAppInfo_.verMaj_ = 0;
            tzAppInfo_.verMin_ = 0;
            tzAppInfo_.appName_ = "N/A";
            if(is_2dfa){
                SystemProperties.set("vendor.seccam_hal_service", "enable");
            }
            if (JNI_OK != doStartTzApp(taName_, taBufferSize_, tzAppInfo_, is_2dfa)) {
                Log.d(LOG_TAG, "::init - faild to start TA: " + taName_);
                tzAppIfAvalable_ = false;
                if(is_2dfa){
                    SystemProperties.set("vendor.seccam_hal_service", "disable");
                }
            }
            else {
                tzAppInfo_.appName_ = taName_;
                Log.d(LOG_TAG, "::init - TA loaded: " + tzAppInfo_.appName_ +
                        " v" + tzAppInfo_.verMaj_ + "." + tzAppInfo_.verMin_ + "");
                tzAppIfAvalable_ = true;
            }
        }
        return tzAppIfAvalable_;
    }

    //=========================================================================
    //
    //=========================================================================
    @Override
    public void onCreate() {
        super.onCreate();
        Log.d(LOG_TAG, "::onCreate");
        chronometer_ = new Chronometer(this);
        chronometer_.setBase(SystemClock.elapsedRealtime());
        chronometer_.start();

        // Save the service context, required for future use by isUidPermitted
        serviceContext = getApplicationContext();
    }

    //=========================================================================
    //
    //=========================================================================
    public void extractTaParams(Intent intent) {
        taName_ = null;
        taBufferSize_ = TZ_APP_BUFFER_SIZE;
        if(null != intent.getExtras()) {
            taName_ = intent.getStringExtra("TANAME");
            taBufferSize_ = intent.getIntExtra("TABUFFERSIZE", TZ_APP_BUFFER_SIZE);
            is_2dfa = intent.getBooleanExtra("2DFA", false);
        }
        if(null == taName_) {
            taName_ = TZ_APP_NAME;
        }
    }

    //=========================================================================
    //
    //=========================================================================
    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        //Called every time the service is connected to a client
        super.onStartCommand(intent, flags, startId);

        if (!tzAppIfAvalable_) {
            extractTaParams(intent);
            Log.d(LOG_TAG, "::onStartCommand - " + taName_ + " (" + taBufferSize_ + ")");
        }
        else {
            Log.d(LOG_TAG, "::onStartCommand - Already in use, TA: " + taName_);
        }
        return START_NOT_STICKY;
    }

    //=========================================================================
    //
    //=========================================================================
    @Override
    public IBinder onBind(Intent intent) {
        //Called once at the begining, after that onRebind
        extractTaParams(intent);
        Log.d(LOG_TAG, "::onBind - " + taName_ + " (" + taBufferSize_ + ")");
        init();
        // In case the TA is not loaded, binding to the service is disabled
        if (tzAppIfAvalable_) {
            return mMessenger.getBinder();
        }
        else {
            Log.e(LOG_TAG, "::onBind - binding to service failed, TZ app is not available");
            return null;
        }
    }

    //=========================================================================
    //
    //=========================================================================
    @Override
    public void onRebind(Intent intent) {
        Log.d(LOG_TAG, "::onRebind");
        super.onRebind(intent);
        init();
    }

    //=========================================================================
    //
    //=========================================================================
    @Override
    public boolean onUnbind(Intent intent) {
        Log.d(LOG_TAG, "::onUnbind");
        if (tzAppIfAvalable_) {
            doShutdownTzApp();
            tzAppIfAvalable_ = false;
            if(is_2dfa){
                SystemProperties.set("vendor.seccam_hal_service", "disable");
            }
        }
        if (frameCallbacks_ != null) {
            for (SecCamServiceHandler.FrameCallbackThread val: frameCallbacks_.values()) {
                try {
                    if(val.isAlive()) {
                        val.shutdown();
                    }
                } catch (Exception e) {
                    Log.d(LOG_TAG, "::onUnbind - Callback thread is not active");
                }
            }
            frameCallbacks_ = null;
        }
        return true;
    }

    //=========================================================================
    //
    //=========================================================================
    @Override
    public void onDestroy() {
        super.onDestroy();
        Log.d(LOG_TAG, "::onDestroy");
        if (tzAppIfAvalable_) {
            doShutdownTzApp();
            tzAppIfAvalable_ = false;
            if(is_2dfa){
                SystemProperties.set("vendor.seccam_hal_service", "disable");
            }
        }
        chronometer_.stop();

        serviceContext = null;
    }

    //=========================================================================
    // isUidPermitted is intendent for service access control based on package
    // name. It should be updated accordingly, to allow supported applications.
    //=========================================================================
    static private boolean isUidPermitted(int sendingUid) {

        // Set the packageNames that are allowed to use the service
        ArrayList<String> allowedPackages = new ArrayList<String>();
        allowedPackages.add("com.qualcomm.qti.seccamsample");
        allowedPackages.add("com.qualcomm.qti.seccamdemoapp");

        try {
            // Get the packageNames installed under the app uid, which is attempting to use the service
            String[] packageNames = serviceContext.getPackageManager().getPackagesForUid(sendingUid);

            // Check if one of the packageNames installed under the given uid is permitted
            for (int i=0; i<packageNames.length; i++) {
                for (int j=0; j<allowedPackages.size(); j++) {
                    if (packageNames[i].equals(allowedPackages.get(j))) {
                        return true;
                    }
                }
            }
        } catch (Exception e) {
             e.printStackTrace();
        }

        return false;
    }

}
