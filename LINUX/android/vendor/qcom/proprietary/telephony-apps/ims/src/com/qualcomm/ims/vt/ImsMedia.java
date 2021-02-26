/* Copyright (c) 2014-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.ims.vt;

import android.graphics.Bitmap;
import android.os.Handler;
import android.os.Message;
import android.os.RegistrantList;
import android.os.SystemProperties;
import android.util.Log;
import android.view.Surface;

import java.util.concurrent.CopyOnWriteArrayList;
import java.util.List;

import com.android.internal.os.SomeArgs;
import org.codeaurora.ims.QtiCallConstants;
import org.codeaurora.ims.QtiVideoCallDataUsage;
import org.codeaurora.ims.ImsCallUtils;

/**
 * Provides an interface to handle the media part of the video telephony call
 */
class ImsMedia extends Handler {
    private static final String TAG = "VideoCall_ImsMedia";

    // DPL library's error codes.
    public static final int DPL_INIT_SUCCESSFUL = 0;
    public static final int DPL_INIT_FAILURE = -1;
    public static final int DPL_INIT_MULTIPLE = -2;

    public static final int PLAYER_STATE_STARTED = 0;
    public static final int PLAYER_STATE_STOPPED = 1;

    private static final int LOOPBACK_MODE_HEIGHT = 144;
    private static final int LOOPBACK_MODE_WIDTH = 176;
    private static final int LOOPBACK_MODE_FPS = 20;

    private static boolean mInitCalledFlag = false;

    // Native functions.
    private static native int nativeInit();
    private static native void nativeDeInit();
    private static native void nativeHandleRawFrame(byte[] frame);
    private static native int nativeSetSurface(Surface st);
    private static native void nativeSetDeviceOrientation(int orientation);
    private static native void nativeSetCameraFacing(int facing);
    private static native void nativeSetCameraInfo(int facing, int mount);
    private static native short nativeGetNegotiatedFPS();
    private static native int nativeGetNegotiatedHeight();
    private static native int nativeGetNegotiatedWidth();
    private static native int nativeGetUIOrientationMode();
    private static native int nativeGetPeerHeight();
    private static native int nativeGetPeerWidth();
    private static native int nativeGetVideoQualityIndication();
    private static native int nativeRequestRtpDataUsage(int mediaId);
    private static native void nativeRegisterForMediaEvents(ImsMedia instance);
    private static native Surface nativeGetRecordingSurface();
    private static native int nativeGetRecorderFrameRate();

    private static native boolean nativeHaveSameParent(Surface a, Surface b);
    private static native int nativeSetVideoImageBuffer(int[] pixels, int width,
            int height);

    public static final int MEDIA_EVENT = 0;

    // Following values are from the IMS VT API documentation
    public static final int CAMERA_PARAM_READY_EVT = 1;
    public static final int START_READY_EVT = 2;
    public static final int PLAYER_START_EVENT = 3;
    public static final int PLAYER_STOP_EVENT = 4;
    public static final int DISPLAY_MODE_EVT = 5;
    public static final int PEER_RESOLUTION_CHANGE_EVT = 6;
    public static final int VIDEO_QUALITY_EVT = 7;
    public static final int DATA_USAGE_EVT = 8;
    public static final int STOP_READY_EVT = 9;
    public static final int CAMERA_FRAME_RATE_CHANGE_EVT = 10;
    public static final int CACHED_MEDIA_EVENT = 100;

    /*
     * Initializing default negotiated parameters to a working set of values so that the application
     * does not crash in case we do not get the Param ready event
     */
    private int mNegotiatedHeight = ImsMediaConstants.DEFAULT_WIDTH;
    private int mNegotiatedWidth = ImsMediaConstants.DEFAULT_HEIGHT;
    private int mUIOrientationMode = ImsMediaConstants.PORTRAIT_MODE;
    private short mNegotiatedFps = ImsMediaConstants.DEFAULT_FPS;

    private int mPeerHeight = ImsMediaConstants.DEFAULT_HEIGHT;
    private int mPeerWidth = ImsMediaConstants.DEFAULT_WIDTH;
    private int mVideoQualityLevel = ImsMediaConstants.VIDEO_QUALITY_UNKNOWN;

    // This variable holds TRUE when media event caching is required else holds FALSE
    private boolean mShouldCacheMediaEvents = false;
    private List<Integer> mCachedMediaEvents = new CopyOnWriteArrayList<Integer>();

    private boolean mIsMediaLoopback = false;
    private Surface mSurface = null;
    private Surface mRecordingSurface = null;

    private IMediaListener mMediaListener;
    private final List<MediaStateListener> mMediaStateListeners
            = new CopyOnWriteArrayList<>();
    private final CopyOnWriteArrayList<CameraListener> mCameraListener
            = new CopyOnWriteArrayList<>();
    // Use a singleton
    private static ImsMedia mInstance;

    /**
     * This method returns the single instance of ImsMedia object *
     */
    public static synchronized ImsMedia getInstance() {
        if (mInstance == null) {
            mInstance = new ImsMedia();
        }
        return mInstance;
    }

    /**
     * Private constructor for ImsMedia
     */
    private ImsMedia() {
        initializemIsMediaLoopback();
    }

    /**
     * Adds a new media state listener
     */
    public void addMediaStateListener(MediaStateListener listener) {
        if (listener != null) {
            mMediaStateListeners.add(listener);
        }
    }

    /**
     * Removes an existing media state listener
     */
    public void removeMediaStateListener(MediaStateListener listener) {
        mMediaStateListeners.remove(listener);
    }

    // TODO create CameraParams class and pass it as an argument to onParamReady
    public interface IMediaListener {

        void onPeerResolutionChanged(int width, int height);

        void onPlayerStateChanged(int state);

        void onVideoQualityEvent(int videoQuality);

        void onDataUsageChanged(int mediaId, QtiVideoCallDataUsage dataUsage);

        void onOrientationModeChanged(int orientationMode);
    };

    public interface CameraListener {
        void onCameraConfigChanged(int width, int height, int fps, Surface surface,
                int orientationMode);
        void onUpdateRecorderFrameRate(int rate);
        void onRecordingEnabled();
        void onRecordingDisabled();
    };

    public interface MediaStateListener {
        void onMediaInitialized();
        void onMediaDeinitialized();
    };

    /*
     * public abstract CameraRelatedEventListener implements ICameraRelatedEventListener { void
     * onParamReadyEvent() {} void onStartReadyEvent(){} void onStopReadyEvent(){} };
     */
    static {
        System.loadLibrary("imsmedia_jni");
    }

    /*
     * Initialize Media
     * @return DPL_INIT_SUCCESSFUL 0 initialization is successful. DPL_INIT_FAILURE -1 error in
     * initialization of QMI or other components. DPL_INIT_MULTIPLE -2 trying to initialize an
     * already initialized library.
     */
    /* package */int init() {
        if (!mInitCalledFlag) {
            int status = nativeInit();
            log("init called error = " + status);
            switch (status) {
                case DPL_INIT_SUCCESSFUL:
                    mInitCalledFlag = true;
                    registerForMediaEvents(this);
                    break;
                case DPL_INIT_FAILURE:
                    mInitCalledFlag = false;
                    break;
                case DPL_INIT_MULTIPLE:
                    mInitCalledFlag = true;
                    loge("Dpl init is called multiple times");
                    status = DPL_INIT_SUCCESSFUL;
                    break;
            }
            if (status == DPL_INIT_SUCCESSFUL) {
                notifyOnMediaInitialized();
            }
            return status;
        }

        // Dpl is already initialized. So return success
        return DPL_INIT_SUCCESSFUL;
    }

    /*
     * Deinitialize Media
     */
    /* package */void deInit() {
        log("deInit called");
        notifyOnMediaDeinitialized();
        nativeDeInit();
        mInitCalledFlag = false;
        clearMediaEventCache();
    }

    private void notifyOnMediaDeinitialized() {
        for (MediaStateListener listener : mMediaStateListeners) {
            try {
                listener.onMediaDeinitialized();
            } catch (Exception e) {
                loge("notifyOnMediaDeinitialized: Error=" + e);
            }
        }
    }

    private void notifyOnMediaInitialized() {
        for (MediaStateListener listener : mMediaStateListeners) {
            try {
                listener.onMediaInitialized();
            } catch (Exception e) {
                loge("notifyOnMediaInitialized: Error=" + e);
            }
        }
    }

    private void initializemIsMediaLoopback() {
        // Check the Media loopback property
        int property = SystemProperties.getInt("net.lte.VT_LOOPBACK_ENABLE", 0);
        mIsMediaLoopback = (property == 1) ? true : false;
    }

    public void sendCvoInfo(int orientation) {
        log("sendCvoInfo orientation=" + orientation);
        nativeSetDeviceOrientation(orientation);
    }

    private void doOnCachedMediaEvent() {
       log("doOnCachedMediaEvent: scheduling the cache");
       if (!mInitCalledFlag) {
          logw("VT lib deinitialized. Do not process cached events");
          clearMediaEventCache();
          return;
       }

       synchronized(this) {
           for (Integer eventId : mCachedMediaEvents) {
               log("doOnCachedMediaEvent: scheduling event : " + eventId);
               doOnMediaEvent(eventId);
           }
           clearMediaEventCache();
       }
    }

    public void clearMediaEventCache() {
        mShouldCacheMediaEvents = false;
        mCachedMediaEvents.clear();
    }

    public void updateMergeStatus(int status) {
        synchronized(this) {
            log("updateMergeStatus status = " + status);

            switch (status) {
                case ImsCallUtils.SESSION_MERGE_STARTED:
                    mShouldCacheMediaEvents = true;
                    break;

                case ImsCallUtils.SESSION_MERGE_COMPLETED:
                   // In case of conference participant side, caching need to start now
                   mShouldCacheMediaEvents = true;
                   // Controls the delay after which cached media events are to be processed
                   final String PROPERTY_SCHEDULE_MEDIA_CACHE = "persist.radio.schd.cache";
                   final int MEDIA_CACHE_DELAY = 2300;
                   /* In order to ensure that event caching continues until the cache is scheduled,
                      defer updating the cacheStatus on merge complete */
                   final Message msg = obtainMessage(CACHED_MEDIA_EVENT);
                   int delay = SystemProperties.getInt(PROPERTY_SCHEDULE_MEDIA_CACHE,
                         MEDIA_CACHE_DELAY);
                   log("scheduling the cache with delay =" + delay);
                   sendMessageDelayed(msg, delay);
                   break;

                case ImsCallUtils.SESSION_MERGE_FAILED:
                    clearMediaEventCache();
                    break;
            }
        }
    }

    /**
     * Send camera facing info to IMS
     */
    public void setCameraFacing(boolean isFacingFront) {
        final int facing = isFacingFront ? ImsMediaConstants.CAMERA_FACING_FRONT
                : ImsMediaConstants.CAMERA_FACING_BACK;
        log("setCameraFacing isFacingFront=" + isFacingFront);
        nativeSetCameraFacing(facing);
    }

    /**
     * Send camera facing and mount angle info to IMS.
     */
    public void setCameraInfo(int facing, int mount) {
        log("setCameraInfo facing=" + facing + " mount=" + mount);
        nativeSetCameraInfo(facing, mount);
    }

    /**
     * Send the Surface to media module
     *
     * @param st
     */
    public void setSurface(Surface st) {
        log("setSurface(Surface: " + st + ")");
        mSurface = st;
        nativeSetSurface(st);
    }

    /**
     * Send the Surface to media module. This should be called only for re-sending an already
     * created surface
     */
    public void setSurface() {
        log("setSurface()");
        if (mSurface == null) {
            loge("sSurface is null. So not passing it down");
            return;
        }
        nativeSetSurface(mSurface);
    }

    /**
     * Get Negotiated Height
     */
    public int getNegotiatedHeight() {
        log("Negotiated Height = " + mNegotiatedHeight);
        return mNegotiatedHeight;
    }

    /**
     * Get Negotiated Width
     */
    public int getNegotiatedWidth() {
        log("Negotiated Width = " + mNegotiatedWidth);
        return mNegotiatedWidth;
    }

    public short getNegotiatedFps() {
        log("Negotiated Fps = " + mNegotiatedFps);
        return mNegotiatedFps;
    }

    /**
     * Get recording surface
     */
    public Surface getRecordingSurface() {
        log("RecordingSurface= " + mRecordingSurface);
        return mRecordingSurface;
    }

    /**
     * Get Negotiated Width
     */
    public int getUIOrientationMode() {
        log("UI Orientation Mode = " + mUIOrientationMode);
        return mUIOrientationMode;
    }

    /**
     * Get Peer Height
     */
    public int getPeerHeight() {
        log("Peer Height = " + mPeerHeight);
        return mPeerHeight;
    }

    /**
     * Get Peer Width
     */
    public int getPeerWidth() {
        log("Peer Width = " + mPeerWidth);
        return mPeerWidth;
    }

    /**
     * Get Video Quality level
     */
    public int getVideoQualityLevel() {
        log("Video Quality Level = " + mVideoQualityLevel);
        return mVideoQualityLevel;
    }

    /**
     * Request Call Data Usage
     */
    public void requestCallDataUsage(int mediaId) {
        log("requestCallDataUsage");
        int status = nativeRequestRtpDataUsage(mediaId);
        log("requestCallDataUsage: status = " + status);
    }

    // Gets the bitmap colors from bitmap and sends to lower layers
    public boolean setPreviewImage(Bitmap bitmap) {
        if (!mInitCalledFlag) {
            log("setPreviewImage: VT lib deinitialized so ignore");
            return false;
        }

        log("setPreviewImage: bitmap = " + bitmap);

        int status;
        if (bitmap == null) {
            status = nativeSetVideoImageBuffer(null, -1, -1);
        } else {
            int height = bitmap.getHeight();
            int width = bitmap.getWidth();
            log("setPreviewImage: bitmap width = " + width + " height = " + height);

            int [] argb = new int[width * height];
            // argb contains bitmap colors
            bitmap.getPixels(argb, 0, width, 0, 0, width, height);
            status = nativeSetVideoImageBuffer(argb, width, height);
        }
        /* status takes below possible values
           0 - Success
          -1 - Generic Failure
          -2 - Buffer size didnt match the required dimension
          -3 - Not matching the required dimensions */
        log("setPreviewImage: status = " + status);
        return (status == 0);
    }

    /**
     * Register for event that will invoke {@link ImsMedia#onMediaEvent(int)}
     */
    private void registerForMediaEvents(ImsMedia instance) {
        log("Registering for Media Callback Events");
        nativeRegisterForMediaEvents(instance);
    }

    public void setMediaListener(IMediaListener listener) {
        log("Registering for Media Listener");
        mMediaListener = listener;
    }

    public void addCameraListener(CameraListener listener) {
        if (listener != null) {
            mCameraListener.addIfAbsent(listener);
        }
    }

    public void removeCameraListener(CameraListener listener) {
            mCameraListener.remove(listener);
    }

    private void doOnMediaEvent(int eventId) {
        switch (eventId) {
            case CAMERA_PARAM_READY_EVT:
                log("Received PARAM_READY_EVT. Updating negotiated values");
                if (updatePreviewParams()) {
                    log("Negotiated Camera values mNegotiatedWidth = " + mNegotiatedWidth
                            + " mNegotiatedHeight = " + mNegotiatedHeight + " mRecordingSurface = "
                            + mRecordingSurface);
                    for (CameraListener listener : mCameraListener) {
                        listener.onCameraConfigChanged(mNegotiatedWidth,
                                mNegotiatedHeight, mNegotiatedFps, mRecordingSurface,
                                mUIOrientationMode);
                    }
                }
                break;
            case PEER_RESOLUTION_CHANGE_EVT:
                mPeerHeight = nativeGetPeerHeight();
                mPeerWidth = nativeGetPeerWidth();
                log("Received PEER_RESOLUTION_CHANGE_EVENT. Updating peer values"
                        + " mPeerHeight=" + mPeerHeight + " mPeerWidth=" + mPeerWidth);
                if (mMediaListener != null) {
                    mMediaListener.onPeerResolutionChanged(mPeerWidth, mPeerHeight);
                }
                break;
            case START_READY_EVT:
                log("Received START_READY_EVT. Camera recording can be started");
                for (CameraListener listener : mCameraListener) {
                    listener.onRecordingEnabled();
                }
                break;

            case STOP_READY_EVT:
                log("Received STOP_READY_EVT");
                for (CameraListener listener : mCameraListener) {
                    listener.onRecordingDisabled();
                }
                break;
            case DISPLAY_MODE_EVT:
                log("Received DISPLAY_MODE_EVT");
                mUIOrientationMode = nativeGetUIOrientationMode();
                log("Received DISPLAY_MODE_EVT mUIOrientationMode=" + mUIOrientationMode);
                if (mMediaListener != null) {
                    mMediaListener.onOrientationModeChanged(mUIOrientationMode);
                }
                break;
            case PLAYER_START_EVENT:
                log("Received PLAYER_START_EVT");
                if (mMediaListener != null) {
                    mMediaListener.onPlayerStateChanged(PLAYER_STATE_STARTED);

                }
                break;
            case PLAYER_STOP_EVENT:
                log("Received PLAYER_STOP_EVT");
                if (mMediaListener != null) {
                    mMediaListener.onPlayerStateChanged(PLAYER_STATE_STOPPED);
                }
                break;
            case VIDEO_QUALITY_EVT:
                mVideoQualityLevel = nativeGetVideoQualityIndication();
                log("Received VIDEO_QUALITY_EVT" + mVideoQualityLevel);
                if (mMediaListener != null) {
                    mMediaListener.onVideoQualityEvent(mVideoQualityLevel);
                }
                break;
            case CAMERA_FRAME_RATE_CHANGE_EVT:
                final int rate = nativeGetRecorderFrameRate();
                log("Received CAMERA_FRAME_RATE_CHANGE_EVT, rate=" + rate);
                if (rate > 0) {
                    for (CameraListener listener : mCameraListener) {
                        listener.onUpdateRecorderFrameRate(rate);
                    }
                }
                break;
            default:
                loge("Received unknown event id=" + eventId);
        }
    }

    /**
     * Callback method that is invoked when Media events occur
     */
    public void onMediaEvent(int eventId) {
        log("onMediaEvent eventId = " + eventId + " mShouldCacheMediaEvents: "
                + mShouldCacheMediaEvents);
        if (!mInitCalledFlag) {
            logw("VT lib deinitialized. Do not cache events");
            return;
        }
        synchronized(this) {
            // Do not cache camera START/STOP events
            if (mShouldCacheMediaEvents &&
                    !(eventId == START_READY_EVT || eventId == STOP_READY_EVT)) {
                 // Merge started so cache it
                mCachedMediaEvents.add(eventId);
            } else {
                final Message msg = obtainMessage(MEDIA_EVENT, eventId, 0);
                sendMessage(msg);
            }
        }
    }

    /**
     * Callback method that is invoked when Data Usage event occurs
     */
    private void onDataUsageEvent(int mediaId, long[] dataUsage) {
        // Index 0 of dataUsage array holds LTE data usage
        // Index 1 of dataUsage array holds WiFi data usage
        log("onDataUsageEvent mediaId = " + mediaId);
        SomeArgs args = SomeArgs.obtain();
        args.arg1 = mediaId;
        args.arg2 = dataUsage;
        Message msg = obtainMessage(DATA_USAGE_EVT, args);
        sendMessage(msg);
    }

    public void handleMessage(Message msg) {
        switch (msg.what) {
            case MEDIA_EVENT:
                doOnMediaEvent(msg.arg1);
                break;
            case DATA_USAGE_EVT:
                SomeArgs args = (SomeArgs) msg.obj;
                try {
                    int mediaId = (int) args.arg1;
                    long[] dataUsage = (long[]) args.arg2;
                    if (mMediaListener != null) {
                        mMediaListener.onDataUsageChanged(mediaId,
                                new QtiVideoCallDataUsage(dataUsage));
                    }
                } finally {
                    args.recycle();
                }
                break;
            case CACHED_MEDIA_EVENT:
                doOnCachedMediaEvent();
                break;
            default:
                loge("Received unknown msg id = " + msg.what);
        }
    }

    public static boolean haveSameParent(Surface a, Surface b) {
        if (a==null && b == null) {
            return true;
        } else if ( a==null || b==null) {
            return false;
        }
        return nativeHaveSameParent(a, b);
    }

    private static boolean areSurfacesSame(Surface a, Surface b) {
        if (a == null && b == null) {
            return true;
        } else if (a == null || b == null) {
            return false;
        }
        return a.equals(b);
    }

    private synchronized boolean updatePreviewParams() {
        if (mIsMediaLoopback) {
            mNegotiatedHeight = LOOPBACK_MODE_HEIGHT;
            mNegotiatedWidth = LOOPBACK_MODE_WIDTH;
            mNegotiatedFps = LOOPBACK_MODE_FPS;
            return true;
        } else {
            int h = nativeGetNegotiatedHeight();
            int w = nativeGetNegotiatedWidth();
            short fps = nativeGetNegotiatedFPS();
            int mode = nativeGetUIOrientationMode();
            // TODO Check if IMS-VT library will return null for camera1 case.
            // TODO Check if it is OK to update all camera params if new surface is received.
            Surface surface = nativeGetRecordingSurface();
            if (mNegotiatedHeight != h
                    || mNegotiatedWidth != w
                    || mNegotiatedFps != fps
                    || !areSurfacesSame(mRecordingSurface, surface)
                    || mUIOrientationMode != mode) {
                mNegotiatedHeight = h;
                mNegotiatedWidth = w;
                mNegotiatedFps = fps;
                mRecordingSurface = surface;
                mUIOrientationMode = mode;
                return true;
            }
            return false;
        }
    }

    private static void log(String msg) {
        Log.d(TAG, msg);
    }

    private static void loge(String msg) {
        Log.e(TAG, msg);
    }

    private static void logw(String msg) {
        Log.w(TAG, msg);
    }
}
