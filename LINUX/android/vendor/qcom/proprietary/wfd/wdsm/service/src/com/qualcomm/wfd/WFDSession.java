/* ============================================================================
 * WFDService.java
 *
 * WFD service implementation for Multiple Instances
 *
 * Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 ============================================================================*/

package com.qualcomm.wfd;

import android.app.Notification;
import android.app.NotificationManager;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.res.Configuration;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Point;
import android.hardware.display.DisplayManager;
import android.hardware.display.VirtualDisplay;
import android.media.AudioSystem;
import android.os.*;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Display;
import android.view.InputEvent;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.WindowManager;
import android.view.WindowManagerPolicy;

import com.qualcomm.wfd.WfdEnums.*;
import com.qualcomm.wfd.service.IHIDEventListener;
import com.qualcomm.wfd.service.IWfdActionListener;
import com.qualcomm.wfd.service.IWfdSession;

import java.util.Arrays;
import java.util.concurrent.atomic.AtomicInteger;

import static com.qualcomm.wfd.WFDServiceUtils.getValue;

/**
 * Created by ichakrab on 13-04-15.
 */
public class WFDSession extends IWfdSession.Stub implements UIBCManager.HIDCEventListener, UIBCManager.EventListener {

    static {
        Log.d("WfdSession", "Loading libwfdnative.so ...");
        System.loadLibrary("wfdnative");
        Log.d("WfdSession", "libwfdnative.so loaded.");
    }

    private static final int OPERATION_TIMEOUT = 5000; // 5 sec

    private static final AtomicInteger NOTI_MSG_ID = new AtomicInteger(42);

    private static final AtomicInteger ID_BASE = new AtomicInteger(1);

    private int mNotiMsgId;

    private final Object mLock = new Object();

    private boolean mIsHDMIConnected = false;

    private boolean mIsHDMIConnectionAllowed = false;

    private Surface mSurface;

    private int[] mConfigItems = null;

    private boolean mSysInvoked = false;

    private DisplayManager mDisplayManager = null;

    private VirtualDisplay mVirtualDisplay = null;

    private static final String broadcastPermission = "com.qualcomm.permission.wfd.QC_WFD";

    private RemoteCallbackList<IHIDEventListener> mHIDEventListeners = null;

    private int mUIBCRotation = -1;

    private boolean mRequestProxyRouting = true;

    private boolean mAudioRoutingRequested = true;

    private NotificationManager mNotiMgr = null;

    private boolean mDebug = false;

    private static final String sDebugKey = "persist.debug.wfd.wfdsvc";

    private boolean mIsRtpTransportNegotiated = false;

    private boolean mIsPlaybackControlComplete = false;

    private WfdEnums.AVPlaybackMode mPlaybackMode = WfdEnums.AVPlaybackMode.AUDIO_VIDEO;

    private RemoteCallbackList<IWfdActionListener> mActionListeners = null;

    private UIBCManager mUIBCManager = null;

    private boolean mUIBCEnabled = false;

    private WfdEnums.SessionState mState = WfdEnums.SessionState.INVALID;

    private WfdEnums.SessionState mBeforeStandbyState = null;

    private static int mSessionId = -1;

    private int mCurrTransport = 0;

    private WfdEnums.WFDDeviceType mDeviceType = WfdEnums.WFDDeviceType.SOURCE;

    private WfdDevice mLocalDevice = null;

    private WfdDevice mPeerDevice = null;

    private Context mContext;

    private BroadcastReceiver mReceiver;

    private BroadcastReceiver mProtectedReceiver;

    private int mVirtualDisplayDPI;

    private boolean mIsHDCPEnabled;

    private long mNativePtr;

    private class surfaceProperties {
        private int[] surfaceCoord = new int[5];//L,T,R,B,format + Orientation
        private int[] screenProps = new int[3];//W X H, Orientation
        @Override
        public String toString(){
            StringBuilder sb = new StringBuilder();
            sb.append("surfaceCoord: ");
            sb.append(Arrays.toString(surfaceCoord));
            sb.append(" screenProps: ");
            sb.append(Arrays.toString(screenProps));
            return sb.toString();
        }
    };

    private surfaceProperties mSurfProp;
    /**
     *  Supported native functions
     */
    private native int nativeInit();
    private native int nativeCreateWfdSession(WfdDevice thisDevice,
                                          WfdDevice peerDevice, long nativePtr);
    private native int nativeStopWfdSession(int sessionId, long nativePtr);
    private native int nativePlay(int sessionId, boolean secureFlag, long nativePtr);
    private native int nativePause(int sessionId, boolean secureFlag, long nativePtr);
    private native int nativeTeardown(int sessionId, boolean isTriggered, long nativePtr);
    private native int nativeStandby(int sessionId, long nativePtr);
    private native int nativeEnableUIBC(int sessionId, long nativePtr);
    private native int nativeDisableUIBC(int sessionId, long nativePtr);
    private native int nativeStartUIBC(Object obj, long nativeptr);
    private native int nativeStopUIBC(long nativePtr);
    private native int nativeSetUIBC(int sessionId, long nativePtr);
    private native int nativeSetVideoSurface(Surface surface, long nativePtr);
    private native int nativeSendUIBCKeyEvent(KeyEvent ev, long nativePtr);
    private native int nativeSendUIBCMotionEvent(MotionEvent ev, long nativePtr);
    private native int nativeSendUIBCRotateEvent(int angle, long nativePtr);
    private native int nativeSetResolution(int formatType, int value,
                                               int[] resParams, long nativePtr);
    private native int nativeSetCodecResolution(int codec, int profile, int level,
                                                int formatType, int value,
                                               int[] resParams, long nativePtr);
    private native int nativeSetBitRate(int bitRate, long nativePtr);
    private native int nativeQueryTCPTransportSupport(long nativePtr);
    private native int nativeNegotiateRtpTransport(int transportType, int bufferLengthMs, int port,
                                                   long nativePtr);
    private native int nativeSetRtpTransport(int transportType, long nativePtr);
    private native int nativeTcpPlaybackControl(int cmdType, int cmdVal, long nativePtr);
    private native int nativeSetDecoderLatency(int latency, long nativePtr);
    private native int nativeSetSurfaceProp(WFDSession.surfaceProperties sp, long nativePtr);
    private native int nativeSetAvPlaybackMode(int mode, long nativePtr);
    private native int nativeGetConfigItems(int[] configItems, long nativePtr);
    private native int nativeExecuteRuntimeCommand(int CmdType, long nativePtr);
    private native int[] nativeGetCommonRes(int[] result, long nativePtr);
    private native int[] nativeGetNegotiatedRes(int[] result, long nativePtr);

    private final String mTAG;
    private int mId;

    private WFDSession(Context c) {
        mContext = c;
        mId = ID_BASE.getAndIncrement();
        mTAG = "WFDSession_".concat(Integer.toString(mId));
        Log.d(mTAG, "Creating new WFDSession instance ...");
        mActionListeners = new RemoteCallbackList<IWfdActionListener>();
        mHIDEventListeners = new RemoteCallbackList<IHIDEventListener>();
        mConfigItems = new int[WfdEnums.ConfigKeys.TOTAL_CFG_KEYS.ordinal()];
        mDisplayManager = (DisplayManager) mContext
                .getSystemService(Context.DISPLAY_SERVICE);
        mSurfProp = new WFDSession.surfaceProperties();
        getScreenConfiguration(mSurfProp.screenProps);
        broadcastWifiDisplayAudioIntent(false);
        broadcastWifiDisplayVideoEnabled(false);
        int ret = teardown();
        Log.d(mTAG, "Cleanup any existing sessions. ret: " + ret);
        mReceiver = new BroadcastReceiver() {
            public void onReceive(Context context, Intent intent) {
                Log.d(mTAG, "Received intent: " + intent.toUri(0));
                String action = intent.getAction();
                synchronized (mLock) {
                    if (action.equals(WindowManagerPolicy.ACTION_HDMI_PLUGGED)) {
                        mIsHDMIConnected = intent.getBooleanExtra(
                                WindowManagerPolicy.EXTRA_HDMI_PLUGGED_STATE, false);
                        if (!mIsHDMIConnectionAllowed && mIsHDMIConnected &&
                                mDeviceType == WFDDeviceType.SOURCE) {
                            stopWfdSession();
                            stopUibcSession();
                        }
                    } else if (action.equals(Intent.ACTION_SCREEN_OFF)) {
                        mBeforeStandbyState = mState;
                        if (mBeforeStandbyState == SessionState.PLAYING) {
                            try {
                                Log.e(mTAG, "Waiting to play");
                                mLock.wait(OPERATION_TIMEOUT);
                            } catch (InterruptedException e) {
                                Log.e(mTAG, "Wait for PLAY state interrupted");
                            }
                            if (mState != SessionState.PLAY) {
                                Log.e(mTAG, "Session failed to move to play state");
                            }
                        }
                        if (AudioSystem.isStreamActive(AudioSystem.STREAM_MUSIC, 0)) {
                            if ((getValue("AudioStreamInSuspend")).contains("1")) {
                                Log.e(mTAG,"Audio Stream is on so not calling standby");
                            } else {
                                Log.e(mTAG,"Calling Standby even if Audio Stream is on");
                                standby();
                            }
                        } else {
                            Log.e(mTAG,"Calling Standby as no Audio Stream is playing");
                            standby();
                        }
                    } else if (action.equals(Intent.ACTION_SCREEN_ON)) {
                        if (mBeforeStandbyState != null
                                && (mBeforeStandbyState == SessionState.PLAY || mBeforeStandbyState == SessionState.PLAYING)) {
                            if (mState == SessionState.STANDING_BY) {
                                try {
                                    Log.e(mTAG, "Waiting to standby");
                                    mLock.wait(OPERATION_TIMEOUT);
                                } catch (InterruptedException e) {
                                    Log.e(mTAG, "Wait for standby state interrupted");
                                }
                                if (mState != SessionState.STANDBY) {
                                    Log.e(mTAG, "Session failed to move to standby state");
                                }
                            }
                            Log.d(mTAG, "Resume session calling play()");
                            mBeforeStandbyState = null;
                            play();
                        }
                    } else if(action.equals(Intent.ACTION_SHUTDOWN)) {
                        teardown();
                    } else if(action.equals(Intent.ACTION_CONFIGURATION_CHANGED)){
                        if (mDeviceType == WFDDeviceType.SOURCE) {
                        /*
                        Only for source since sink surface will be tied to
                        orientation and surface properties shall be set when
                        that update comes along
                         */
                            setSurfaceProp();
                        }
                    }
                }
            }
        };
        IntentFilter filter = new IntentFilter(WindowManagerPolicy.ACTION_HDMI_PLUGGED);
        filter.addAction(Intent.ACTION_SCREEN_ON);
        filter.addAction(Intent.ACTION_SCREEN_OFF);
        filter.addAction(Intent.ACTION_SHUTDOWN);
        filter.addAction(Intent.ACTION_CONFIGURATION_CHANGED);
        mContext.registerReceiver(mReceiver, filter);

        /**
         * Protect QTI specific proprietary intents from being broadcast
         * by any third-party app. Only system apps or those signed with the
         * platform signature can broadcast to this protected intent receiver
         * after acquiring the permission com.qualcomm.permission.wfd.QC_WFD
         */

        mProtectedReceiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                Log.d(mTAG, "Received intent: " + intent.toUri(0));
                String action = intent.getAction();
                synchronized (mLock) {
                    if (action.equals(WfdEnums.ACTION_WIFI_DISPLAY_RESOLUTION)) {
                        int formatType = intent.getIntExtra("format",
                                WfdEnums.CapabilityType.WFD_CEA_RESOLUTIONS_BITMAP
                                        .ordinal());
                        int value = intent.getIntExtra("value", 0);
                        Log.d(mTAG, "Calculated formatType: " + formatType
                                + " value: 0x" + Integer.toHexString(value));
                        int ret = setResolution(formatType, value);
                        Log.d(mTAG, "setResoltuion returned: " + ret);
                    } else if (action.equals(WfdEnums.ACTION_WIFI_DISPLAY_VIDEO_CODEC_RESOLUTION)) {
                        int codec = intent.getIntExtra("codec",
                                WfdEnums.VideoFormat.WFD_VIDEO_H264.ordinal());
                        int profile = intent.getIntExtra("profile", 0);
                        int level   = intent.getIntExtra("level", 0);
                        int formatType = intent.getIntExtra("format",
                                WfdEnums.CapabilityType.WFD_CEA_RESOLUTIONS_BITMAP
                                        .ordinal());
                        int value = intent.getIntExtra("value", 0);
                        Log.d(mTAG, "Calculated codec: " + codec +
                                    " profile: " + profile +
                                    " level: " + level +
                                    " formatType: " + formatType
                                + " value: 0x" + Integer.toHexString(value));
                        int ret = setCodecResolution(codec, profile, level, formatType, value);
                        Log.d(mTAG, "setCodecResolution returned: " + ret);
                    } else if (action
                            .equals(WfdEnums.ACTION_WIFI_DISPLAY_BITRATE)) {
                        int bitrate = intent.getIntExtra("value", 0);
                        Log.d(mTAG, "Calculated bitrate: " + bitrate);
                        int ret = setBitRate(bitrate);
                        Log.d(mTAG, "setBitrate returned: " + ret);
                    } else if (action
                            .equals(WfdEnums.ACTION_WIFI_DISPLAY_RTP_TRANSPORT)) {
                        int transportType = intent.getIntExtra("type", -1);
                        int bufferLengthMs = intent.getIntExtra("bufferLength",
                                0);
                        int port = intent.getIntExtra("port", -1);
                        Log.d(mTAG, "Calculted type: " + transportType
                                + ", bufferLengthMs: " + bufferLengthMs
                                + ", port: " + port);
                        int ret = setRtpTransport(transportType,
                                bufferLengthMs, port);
                        Log.d(mTAG, "setRtpTransport returned: " + ret);
                    } else if (action
                            .equals(WfdEnums.ACTION_WIFI_DISPLAY_TCP_PLAYBACK_CONTROL)) {
                        int cmdType = intent.getIntExtra("type", -1);
                        Log.d(mTAG, "Calculated type:" + cmdType);
                        int ret = tcpPlaybackControl(cmdType, 0);
                        Log.d(mTAG, "tcpPlaybackControl returned: " + ret);
                    } else if (action
                            .equals(WfdEnums.ACTION_WIFI_DISPLAY_PLAYBACK_MODE)) {
                        int mode = intent.getIntExtra("mode", -1);
                        Log.d(mTAG, "Calculated mode:" + mode);
                        int ret = setAvPlaybackMode(mode);
                        Log.d(mTAG, "setAvPlaybackMode returned: " + ret);
                    } else if (action.equals(
                        WfdEnums.ACTION_WIFI_DISPLAY_SINGLE_STREAM_CTRL)) {
                        // for testing only
                        String streamAction =
                            intent.getStringExtra("streamAction");
                        Log.d(mTAG, "ACTION_WIFI_DISPLAY_SINGLE_STREAM_CTRL "+
                                    "streamAction=" + streamAction);

                        int ret =  -1;
                        switch (streamAction) {
                            case "audioPause":
                                ret = audioPause();
                                break;

                            case "audioResume":
                                ret = audioResume();
                                break;

                            case "videoPause":
                                ret = videoPause();
                                break;

                            case "videoResume":
                                ret = videoResume();
                                break;
                        }
                        Log.d(mTAG, "ACTION_WIFI_DISPLAY_SINGLE_STREAM_CTRL "+
                                    "ret=" + ret);
                    }
                }
            }
        };
        IntentFilter ProtectedFilter = new IntentFilter(
                WfdEnums.ACTION_WIFI_DISPLAY_RESOLUTION);
        ProtectedFilter.addAction(WfdEnums.ACTION_WIFI_DISPLAY_VIDEO_CODEC_RESOLUTION);
        ProtectedFilter.addAction(WfdEnums.ACTION_WIFI_DISPLAY_BITRATE);
        ProtectedFilter.addAction(WfdEnums.ACTION_WIFI_DISPLAY_RTP_TRANSPORT);
        ProtectedFilter
                .addAction(WfdEnums.ACTION_WIFI_DISPLAY_TCP_PLAYBACK_CONTROL);
        ProtectedFilter.addAction(WfdEnums.ACTION_WIFI_DISPLAY_PLAYBACK_MODE);
        ProtectedFilter.addAction(WfdEnums.ACTION_WIFI_DISPLAY_SINGLE_STREAM_CTRL);
        mContext.registerReceiver(mProtectedReceiver, ProtectedFilter,
                broadcastPermission, null);
        eventCallback(null,null);//Hack to overcome progaurd stripping
        Log.d(mTAG,"Done creating new WFDSession instance ...");
    }

    public static WFDSession createWFDSession(Context c) {
        return new WFDSession(c);
    }

    public static WFDSession createWiFiDisplay(Context c, IWfdActionListener listener, WfdDevice localDev, WfdDevice peerDev) {
        WFDSession display = createWFDSession(c);
        if (display != null && display.init(listener,localDev) == 0 &&  display.startWfdSession(peerDev) == 0 ) {
            return display;
        }
        return null;
    }

    @Override
    public int setDeviceType(int type) {
        Log.d(mTAG, "setDeviceType(): type - " + type);
        synchronized (mLock) {
            if (mState == SessionState.INVALID || mState == SessionState.INITIALIZED) {
                for (WFDDeviceType each : WFDDeviceType.values()) {
                    if (each.getCode() == type) {
                        mDeviceType = each;
                        return 0;
                    }
                }
                return ErrorType.INVALID_ARG.getCode();
            } else {
                Log.e(mTAG, "Session state is not INVALID or INITIALIZED");
                return ErrorType.INCORRECT_STATE_FOR_OPERATION.getCode();
            }
        }
    }

    @Override
    public int getSupportedTypes(int[] types) {
        Log.d(mTAG, "getSupportedTypes()");
        WFDDeviceType[] deviceTypes = {
                WFDDeviceType.SOURCE
        }; // Only Source is supported
        types = new int[deviceTypes.length];
        for (int i = 0; i < deviceTypes.length; i++) {
            types[i] = deviceTypes[i].getCode();
        }
        return 0;
    }

    @Override
    public WfdStatus getStatus() {
        WfdStatus ret = new WfdStatus();
        ret.sessionId = mSessionId;
        ret.state = mState.ordinal();
        ret.connectedDevice = mPeerDevice;
        return ret;
    }

    @Override
    public int init(IWfdActionListener listener, WfdDevice thisDevice) {
        Log.d(mTAG, "init()");
        synchronized (mLock) {
            if(nativeInit() == -1) {
                Log.e(mTAG,"nativeInit failed!");
                return -1;
            }
            Log.d(mTAG, "Value of mNativePtr after native set = " + mNativePtr);
            if (!mIsHDMIConnectionAllowed && mIsHDMIConnected &&
                    mDeviceType == WFDDeviceType.SOURCE) {
                return ErrorType.HDMI_CABLE_CONNECTED.getCode();
            }
            if (listener != null) {
                mActionListeners.register(listener);
            }
            if (mState == SessionState.INVALID) {
                if (thisDevice == null) {
                    Log.e(mTAG, "WfdDevice arg can not be null");
                    return ErrorType.INVALID_ARG.getCode();
                }
                mLocalDevice = thisDevice;
                mPeerDevice = null;
                if (mLocalDevice.deviceType == WFDDeviceType.SOURCE.getCode()) {
                    if (mSysInvoked) {
                        Log.d(mTAG, "WFD invoked from system settings on source");
                    } else {
                        Log.d(mTAG, "WFD invoked in non-system context on source");
                    }
                }
                updateState(SessionState.INITIALIZED, -1);
                mSessionId = 1;// Move to a +ve session ID in order to handle
                // cases of teardown from transient states like
                // INITIALIZED, IDLE, ESTABLISHED

                // Broadcast the WIFI_DISPLAY_ENABLED intent for legacy purposes
                // as well as secure application apps that take a decision based
                // on this whether to display secure content or not
                Intent intent = new Intent(WfdEnums.ACTION_WIFI_DISPLAY_ENABLED);
                Log.d(mTAG, "Broadcasting WFD intent: " + intent);
                mContext.sendBroadcast(intent, broadcastPermission);
                return 0;
            } else if (mState == SessionState.INITIALIZED){
                return ErrorType.ALREADY_INITIALIZED.getCode();
            } else {
                return ErrorType.INCORRECT_STATE_FOR_OPERATION.getCode();
            }
        }
    }

    @Override
    public int initSys(IWfdActionListener listener, WfdDevice thisDevice) {
        if (Binder.getCallingUid() != android.os.Process.SYSTEM_UID) {
            Log.e(mTAG,"Can't call initSys from non system-server context");
            return WfdEnums.ErrorType.UNKNOWN.getCode();
        }
        mSysInvoked = true;
        return 0;
    }

    @Override
    public int registerListener(IWfdActionListener listener) {
        Log.d(mTAG, "registerListener: " + listener);
        synchronized (mLock) {
            if (listener != null) {
                return mActionListeners.register(listener) ? 0
                : ErrorType.UNKNOWN.getCode();
            } else {
                Log.e(mTAG, "Listener to register is null");
                return ErrorType.INVALID_ARG.getCode();
            }
        }
    }

    @Override
    public int unregisterListener(IWfdActionListener listener) {
        Log.d(mTAG, "unregisterListener: " + listener);
        synchronized (mLock) {
            if (listener != null && mActionListeners != null) {
                return mActionListeners.unregister(listener) ? 0 : ErrorType.UNKNOWN.getCode();
            } else {
                Log.e(mTAG, "Listener to unregister is null");
                return ErrorType.INVALID_ARG.getCode();
            }
        }
    }

    @Override
    public int deinit() {
        boolean ret = false;
        synchronized (mLock) {
            Log.e(mTAG, "deinit: mState = " + mState);
            if (mState != SessionState.INVALID) {
                nativeTeardown(mSessionId, false, mNativePtr);
                ret = (nativeStopWfdSession(mSessionId,mNativePtr) == 0);
                updateState(SessionState.INVALID, -1);
                mPeerDevice = null;
                // Broadcast the WIFI_DISPLAY_DISABLED intent for legacy
                // purposes as well as secure application apps that take a
                // decision based on this whether to display secure content or
                // not
                Intent intent = new Intent(
                        WfdEnums.ACTION_WIFI_DISPLAY_DISABLED);
                Log.d(mTAG, "Broadcasting WFD intent: " + intent);
                mContext.sendBroadcast(intent, broadcastPermission);
                clearSession();
            }
        }
        return ret ? 0 : -1;
    }

    @Override
    public int startWfdSession(WfdDevice device) {
        synchronized (mLock) {
            if (!mIsHDMIConnectionAllowed && mIsHDMIConnected &&
                    mDeviceType == WFDDeviceType.SOURCE) {
                return ErrorType.HDMI_CABLE_CONNECTED.getCode();
            } else if (mState == SessionState.INVALID) {
                return ErrorType.NOT_INITIALIZED.getCode();
            } else if (mState != SessionState.INITIALIZED) {
                return ErrorType.SESSION_IN_PROGRESS.getCode();
            } else if (device == null) {
                Log.e(mTAG, "Peer device is null");
                return ErrorType.INVALID_ARG.getCode();
            }
            mPeerDevice = device;

            if (nativeCreateWfdSession(mLocalDevice, mPeerDevice,mNativePtr) != 0) {
                Log.e(mTAG,"nativeCreateWfdSession failed!");
                return ErrorType.UNKNOWN.getCode();
            }
            if(mSurface != null) {
                int ret = nativeSetVideoSurface(mSurface, mNativePtr);
                if(ret != 0) {
                    Log.e(mTAG,"nativeSetVideoSurface failed!");
                }
            }
            if (nativeSetAvPlaybackMode(mPlaybackMode.ordinal(), mNativePtr) != 0) {
                Log.w(mTAG, "Failed to set av playback mode " + mPlaybackMode);
            }
            //The session ID maintained by SM need not be changed since it
            //does not reflect any valid session ID per se
            updateState(SessionState.IDLE, mSessionId);
            mDebug = SystemProperties.getBoolean(sDebugKey, false);
        }
        return 0;
    }

    @Override
    public int stopWfdSession() {
        return teardown();
    }

    @Override
    public int setResolution(int formatType, int value) {
        int ret = 0;
        synchronized (mLock) {
            if(formatType!= CapabilityType.WFD_CEA_RESOLUTIONS_BITMAP.ordinal() &&
                    formatType!= CapabilityType.WFD_VESA_RESOLUTIONS_BITMAP.ordinal() &&
                    formatType!= CapabilityType.WFD_HH_RESOLUTIONS_BITMAP.ordinal()) {
                Log.e(mTAG, "Invalid format type for resolution change");
                return ErrorType.INVALID_ARG.getCode();
            }
            CapabilityType resolutionType = CapabilityType.values()[formatType];
            switch (resolutionType) {
                case WFD_CEA_RESOLUTIONS_BITMAP:
                    if (!WfdEnums.isCeaResolution(value)) {
                        Log.e(mTAG,"Invalid resolution type for resolution change");
                        return ErrorType.INVALID_ARG.getCode();
                    }
                    break;
                case WFD_HH_RESOLUTIONS_BITMAP:
                    if (!WfdEnums.isHhResolution(value)) {
                        Log.e(mTAG,"Invalid resolution type for resolution change");
                        return ErrorType.INVALID_ARG.getCode();
                    }
                    break;
                case WFD_VESA_RESOLUTIONS_BITMAP:
                    if (!WfdEnums.isVesaResolution(value)) {
                        Log.e(mTAG, "Invalid resolution type for resolution change");
                        return ErrorType.INVALID_ARG.getCode();
                    }
                    break;
                default:
                    return ErrorType.INVALID_ARG.getCode();
            }

            int[] resParams = WfdEnums.getResParams();

            if (nativeSetResolution(formatType, value, resParams, mNativePtr) != 0) {
                Log.e(mTAG, "Setting new resolution failed!");
            }

        }
        return 0;
    }

    @Override
    public int setCodecResolution(int codec, int profile, int level, int formatType, int value) {
        int ret = 0;
        synchronized (mLock) {

            if (mDeviceType != WFDDeviceType.SOURCE) {
                Log.e(mTAG, "Operation not permitted for a sink device");
                return ErrorType.NOT_SOURCE_DEVICE.getCode();
            }
            if (mState != SessionState.PLAY) {
                Log.e(mTAG, "Operation not permitted while not in play");
                return ErrorType.INCORRECT_STATE_FOR_OPERATION.getCode();
            }

            if(codec != VideoFormat.WFD_VIDEO_H264.ordinal() && codec != VideoFormat.WFD_VIDEO_H265.ordinal()) {
                Log.e(mTAG, "Invalid codec type");
                return ErrorType.INVALID_ARG.getCode();
            }

            if(
               (codec == VideoFormat.WFD_VIDEO_H264.ordinal() &&
                (profile != H264Profile.WFD_VIDEO_H264_CONSTRAINED_BASELINE.ordinal() &&
                 profile != H264Profile.WFD_VIDEO_H264_RESTRICTED_HIGH.ordinal() &&
                 profile != H264Profile.WFD_VIDEO_H264_RESTRICTED_HIGH2.ordinal()))
                ||
               (codec == VideoFormat.WFD_VIDEO_H265.ordinal() &&
                profile != H265Profile.WFD_VIDEO_H265_MAIN.ordinal())
               )
            {
                Log.e(mTAG, "Invalid profile");
                return ErrorType.INVALID_ARG.getCode();
            }

            if(level < VideoCodecLevel.WFD_VIDEO_CODEC_LEVEL_3_1.ordinal() ||
               level > VideoCodecLevel.WFD_VIDEO_CODEC_LEVEL_5_2.ordinal()) {
                Log.e(mTAG, "Invalid level");
                return ErrorType.INVALID_ARG.getCode();
            }

            if(formatType!= CapabilityType.WFD_CEA_RESOLUTIONS_BITMAP.ordinal() &&
                    formatType!= CapabilityType.WFD_VESA_RESOLUTIONS_BITMAP.ordinal() &&
                    formatType!= CapabilityType.WFD_HH_RESOLUTIONS_BITMAP.ordinal()) {
                Log.e(mTAG, "Invalid format type for resolution change");
                return ErrorType.INVALID_ARG.getCode();
            }
            CapabilityType resolutionType = CapabilityType.values()[formatType];
            switch (resolutionType) {
                case WFD_CEA_RESOLUTIONS_BITMAP:
                    if (!WfdEnums.isCeaResolution(value)) {
                        Log.e(mTAG,"Invalid resolution type for resolution change");
                        return ErrorType.INVALID_ARG.getCode();
                    }
                    break;
                case WFD_HH_RESOLUTIONS_BITMAP:
                    if (!WfdEnums.isHhResolution(value)) {
                        Log.e(mTAG,"Invalid resolution type for resolution change");
                        return ErrorType.INVALID_ARG.getCode();
                    }
                    break;
                case WFD_VESA_RESOLUTIONS_BITMAP:
                    if (!WfdEnums.isVesaResolution(value)) {
                        Log.e(mTAG, "Invalid resolution type for resolution change");
                        return ErrorType.INVALID_ARG.getCode();
                    }
                    break;
                default:
                    return ErrorType.INVALID_ARG.getCode();
            }

            int[] resParams = WfdEnums.getResParams();

            if (nativeSetCodecResolution(codec, profile, level,
                                         formatType, value, resParams, mNativePtr) != 0) {
                Log.e(mTAG, "Setting new codec & resolution failed!");
                return -1;
            }

        }
        return 0;
    }

    @Override
    public int setBitRate(int bitRate) {
        Log.d(mTAG, "setBitRate()");
        synchronized (mLock) {
            nativeSetBitRate(bitRate, mNativePtr);
        }
        return 0;
    }

    @Override
    public int setRtpTransport(int transportType, int bufferLengthMs, int port) {
        synchronized(mLock) {
            boolean valid = false;
            for (RtpTransportType e: RtpTransportType.values()) {
                if (transportType == e.ordinal()) {
                    valid = true;
                    break;
                }
            }
            if (!valid) {
                Log.e(mTAG, "Invalid transport type:" + transportType);
                return ErrorType.INVALID_ARG.getCode();
            }
            if (transportType == mCurrTransport) {
                Log.w(mTAG, "setRtpTransport to same type: " + transportType);
                return ErrorType.INVALID_ARG.getCode();
            }

            int ret = 0;

            /* For tcp, pause before negotiation*/
            SessionState beforeState = mState;

            mIsRtpTransportNegotiated = false;
            nativeNegotiateRtpTransport(transportType, bufferLengthMs, port, mNativePtr);

            while(!mIsRtpTransportNegotiated) {
                try {
                    Log.d(mTAG, "Wait for RTSP negotiation for new RTP transport");
                    mLock.wait(5);
                } catch (InterruptedException e) {
                    Log.e(mTAG, "setRtpTransport interrupted", e);
                    return ErrorType.UNKNOWN.getCode();
                }
            }
            if (!mIsRtpTransportNegotiated) {
                return ErrorType.UNKNOWN.getCode();
            }
            mIsRtpTransportNegotiated = false;

            mCurrTransport = transportType;
            return 0;
        }
    }

    @Override
    public int tcpPlaybackControl(int cmdType, int cmdVal) {
        synchronized(mLock) {
            boolean valid = false;
            for (ControlCmdType e: ControlCmdType.values()) {
                if (cmdType == e.ordinal()) {
                    valid = true;
                    break;
                }
            }
            if (!valid) {
                Log.e(mTAG, "Invalid control cmd type:" + cmdType);
                return ErrorType.INVALID_ARG.getCode();
            }

            mIsPlaybackControlComplete = false;
            int ret = nativeTcpPlaybackControl(cmdType, cmdVal, mNativePtr);

            if(ret != 0) {
                Log.e(mTAG, "Failed to send playback control");
                return ErrorType.UNKNOWN.getCode();
            }

            if(cmdType == WfdEnums.ControlCmdType.FLUSH.ordinal()) {
                while(!mIsPlaybackControlComplete &&
                      (mState == SessionState.PLAY || mState == SessionState.PAUSE)){
                    try {
                        Log.d(mTAG, "Waiting for flush to finish");
                        mLock.wait(5);
                    } catch (InterruptedException e) {
                        Log.e(mTAG, "tcpPlaybackControl interrupted", e);
                        return ErrorType.UNKNOWN.getCode();
                    }
                }
                if (!mIsPlaybackControlComplete) {
                    return ErrorType.UNKNOWN.getCode();
                }
                mIsPlaybackControlComplete = false;
            }
            return 0;
        }
    }

    @Override
    public int setDecoderLatency(int latency) {
        synchronized (mLock) {
            nativeSetDecoderLatency(latency, mNativePtr);
        }
        Log.d(mTAG, "setDecoderLatency done: ");
        return 0;
    }

    @Override
    public int queryTCPTransportSupport() {
        Log.d(mTAG, "queryTCPTransportSupport");
        synchronized (mLock) {
            nativeQueryTCPTransportSupport(mNativePtr);
        }
        return 0;
    }

    @Override
    public int setAvPlaybackMode(int mode) {
        Log.d(mTAG, "setAvPlaybackMode mode: " + mode);
        synchronized (mLock) {
            if (mState == SessionState.INVALID || mState == SessionState.INITIALIZED) {
                boolean valid = false;
                AVPlaybackMode m = AVPlaybackMode.AUDIO_VIDEO;
                for (AVPlaybackMode e : AVPlaybackMode.values()) {
                    if (mode == e.ordinal()) {
                        m = e;
                        valid = true;
                        break;
                    }
                }
                if (!valid) {
                    Log.e(mTAG, "Invalid AV playback mode:" + mode);
                    return ErrorType.INVALID_ARG.getCode();
                }
                if (mNativePtr == 0) {
                    Log.i(mTAG, "set av playback mode later");
                    mPlaybackMode = m;
                    return 0;
                }
                if (nativeSetAvPlaybackMode(mode, mNativePtr) == 0) {
                    mPlaybackMode = m;
                    return 0;
                } else {
                    return ErrorType.UNKNOWN.getCode();
                }
            } else {
                Log.e(mTAG, "Session state is not INVALID or INITIALIZED");
                return ErrorType.INCORRECT_STATE_FOR_OPERATION.getCode();
            }
        }
    }

    @Override
    public int play() {
        Log.d(mTAG, "play()");
        if (mDeviceType != WFDDeviceType.SOURCE && mSurface == null) {
            Log.d(mTAG, "Not calling play when surface is null");
            return 0;
        }
        return internalPlay(false);
    }

    private int internalPlay(boolean secureFlag) {
        Log.d(mTAG, "internalPlay(): secureFlag - " + secureFlag);
        synchronized (mLock) {
            if (!mIsHDMIConnectionAllowed && mIsHDMIConnected &&
                    mDeviceType == WFDDeviceType.SOURCE) {
                return ErrorType.HDMI_CABLE_CONNECTED.getCode();
            }
            if (mState == SessionState.PLAYING) {
                Log.d(mTAG, "Already in the middle of PLAYING");
                return 0;
            }
            if (mState == SessionState.ESTABLISHED || mState == SessionState.PAUSE ||
                    mState == SessionState.STANDBY) {
                nativePlay(mSessionId, secureFlag, mNativePtr);
                mState = SessionState.PLAYING;
                return 0;
            } else {
                Log.e(mTAG, "Session state is not ESTABLISHED or PAUSE or STANDBY");
                return ErrorType.INCORRECT_STATE_FOR_OPERATION.getCode();
            }
        }
    }

    @Override
    public int pause() {
        return internalPause(false);
    }

    private int internalPause(boolean secureFlag) {
        Log.d(mTAG, "internalPause(): secureFlag - " + secureFlag);
        synchronized (mLock) {
            if (!mIsHDMIConnectionAllowed && mIsHDMIConnected &&
                    mDeviceType == WFDDeviceType.SOURCE) {
                return ErrorType.HDMI_CABLE_CONNECTED.getCode();
            }
            if (mState == SessionState.PAUSING) {
                Log.d(mTAG, "Already in the middle of PAUSING");
                return 0;
            }
            if (mState == SessionState.PLAY) {
                nativePause(mSessionId, secureFlag, mNativePtr);
                mState = SessionState.PAUSING;
                return 0;
            } else {
                Log.e(mTAG, "Session state is not PLAY");
                return ErrorType.INCORRECT_STATE_FOR_OPERATION.getCode();
            }
        }
    }

    @Override
    public int teardown() {
        synchronized (mLock) {
            Log.d(mTAG, "Teardown session");
            if (mState == SessionState.TEARDOWN || mState == SessionState.TEARING_DOWN) {
                Log.d(mTAG, "Already in the middle of teardown. mState: "
                        + mState);
                return 0;
            }

            if (mState == SessionState.INVALID || mState == SessionState.INITIALIZED) {
                Log.d(mTAG, "No session in progress");
                return 0;
            }

            mRequestProxyRouting = false;
            // Now since we are going to proceed with teardown, release the
            // Virtual Display
            releaseVDS();
            if (mState == SessionState.PLAY || mState == SessionState.PLAYING
                    || mState == SessionState.PAUSE || mState == SessionState.PAUSING
                    || mState == SessionState.STANDBY || mState == SessionState.STANDING_BY
                    || mState == SessionState.ESTABLISHED) {
                Log.d(mTAG, "Perform triggered TEARDOWN in " + mState + " state");
                nativeTeardown(mSessionId, true, mNativePtr);
                mState = SessionState.TEARING_DOWN;
                return 0;
            } else {
                Log.e(mTAG, "Session state is neither PLAY nor PAUSE");
                Log.d(mTAG, "Perform local TEARDOWN without RTSP");
                nativeTeardown(mSessionId, false, mNativePtr);
                mState = SessionState.TEARING_DOWN;
                if(mDeviceType == WFDDeviceType.PRIMARY_SINK)
                {
                   Log.d(mTAG, "perform local teradown if no RTSP and MM calback expected ");
                   updateState(WfdEnums.SessionState.TEARDOWN, mSessionId);
                }
                return 0;
            }
        }
    }

    @Override
    public int standby() {
        synchronized (mLock) {
            if (!mIsHDMIConnectionAllowed && mIsHDMIConnected &&
                    mDeviceType == WFDDeviceType.SOURCE) {
                return ErrorType.HDMI_CABLE_CONNECTED.getCode();
            }
            if (mState == SessionState.STANDING_BY) {
                Log.d(mTAG, "Already in the middle of STANDING_BY");
                return 0;
            }
            if (mState == SessionState.PAUSE || mState == SessionState.PLAY) {
                if (nativeStandby(mSessionId, mNativePtr) == 0) {
                    mState = SessionState.STANDING_BY;
                } else {
                    Log.e(mTAG, "Calling standby failed.");
                    return ErrorType.UNKNOWN.getCode();
                }
                return 0;
            } else {
                Log.e(mTAG, "Session state is not PAUSE or PLAY");
                return ErrorType.INCORRECT_STATE_FOR_OPERATION.getCode();
            }
        }
    }

    @Override
    public int startUibcSession() {
        Log.d(mTAG, "startUibcSession()");
        synchronized (mLock) {
            if (!mIsHDMIConnectionAllowed && mIsHDMIConnected &&
                    mDeviceType == WFDDeviceType.SOURCE) {
                return ErrorType.HDMI_CABLE_CONNECTED.getCode();
            }
            if (mState == SessionState.INVALID) {
                return ErrorType.NOT_INITIALIZED.getCode();
            }
            if (mUIBCEnabled) {
                return ErrorType.UIBC_ALREADY_ENABLED.getCode();
            }
            if (mDeviceType == WFDDeviceType.SOURCE) {
                setSurfaceProp();
            }
            //Enable UIBC from RTSP path first
            nativeEnableUIBC(mSessionId,mNativePtr);
            return 0;
        }

    }

    @Override
    public int stopUibcSession() {
        Log.d(mTAG, "stopUibcSession");
        synchronized (mLock) {
            if (mUIBCEnabled) {
                nativeDisableUIBC(mSessionId, mNativePtr);
                mUIBCEnabled = false;
                return 0;
            } else {
                return ErrorType.UIBC_NOT_ENABLED.getCode();
            }
        }
    }

    @Override
    public int getCommonCapabilities(Bundle capabilities) {
        return 0;
    }

    @Override
    public int setNegotiatedCapabilities(Bundle capabilities) {
        return 0;
    }

    @Override
    public int getConfigItems(Bundle configItems) {
        Log.d(mTAG, "getConfigItems()");
        nativeGetConfigItems(mConfigItems, mNativePtr);
        configItems.putIntArray(WfdEnums.CONFIG_BUNDLE_KEY, mConfigItems);
        return 0;
    }

    @Override
    public int setUIBC() {
        synchronized (mLock) {
            Log.e(mTAG, "setUibc");
            if (nativeSetUIBC(mSessionId, mNativePtr) == 1) {
                return 0;
            } else {
                Log.e(mTAG,"Unable to set UIBC parameters");
                return ErrorType.UNKNOWN.getCode();
            }
        }
    }

    @Override
    public boolean getUIBCStatus() {
        return mUIBCEnabled;
    }

    @Override
    public int getCommonResolution(Bundle comRes) {
        synchronized (mLock) {
            Log.d(mTAG, "getCommonResolution()");
            if (mState == SessionState.INVALID
                    || mState == SessionState.INITIALIZED
                    || mState == SessionState.IDLE
                    || mState == SessionState.ESTABLISHED) {
                return ErrorType.INCORRECT_STATE_FOR_OPERATION.getCode();
            }
            int[] result = new int[1];
            int[] ret = nativeGetCommonRes(result, mNativePtr);
            if (result[0] == -1) {
                Log.e(mTAG, "Failed to retrieve negotiated resolution");
                return ErrorType.UNKNOWN.getCode();
            }
            // The array will be containing values for a particular H264 profile
            // at [0],[1],[2] and [3] i.e. the array size should always be a
            // multiple of 4. Hence the number of profiles can be directly
            // calculated using the array length

            // Sanity Check
            if (ret == null || ret.length % 4 != 0) {
                Log.e(mTAG, "Something is terribly wrong!");
                return ErrorType.UNKNOWN.getCode();
            }
            comRes.putInt("numProf", ret.length / 4);
            comRes.putIntArray("comRes", ret);
        }
        return 0;
    }

    @Override
    public int getNegotiatedResolution(Bundle negRes) {
        synchronized (mLock) {
            Log.d(mTAG, "getNegotiatedResolution()");
            if (mState == SessionState.INVALID
                    || mState == SessionState.INITIALIZED
                    || mState == SessionState.IDLE
                    || mState == SessionState.ESTABLISHED) {
                return ErrorType.INCORRECT_STATE_FOR_OPERATION.getCode();
            }
            int[] result = new int[1];
            int[] ret = nativeGetNegotiatedRes(result, mNativePtr);
            if (result[0] == -1) {
                Log.e(mTAG, "Failed to retrieve negotiated resolution");
                return ErrorType.UNKNOWN.getCode();
            }

            // Sanity Check
            if (ret == null || ret.length != 4) {
                Log.e(mTAG, "Something is terribly wrong!");
                return ErrorType.UNKNOWN.getCode();
            }
            negRes.putIntArray("negRes", ret);
        }
        return 0;
    }

    @Override
    public int enableDynamicBitRateAdaptation(boolean flag) {
        synchronized (mLock) {
            if (mDeviceType != WFDDeviceType.SOURCE) {
                Log.e(mTAG, "Operation not permitted for a sink device");
                return ErrorType.NOT_SOURCE_DEVICE.getCode();
            }
            if (mState == SessionState.INVALID
                    || mState == SessionState.INITIALIZED
                    || mState == SessionState.IDLE
                    || mState == SessionState.ESTABLISHED) {
                return ErrorType.INCORRECT_STATE_FOR_OPERATION.getCode();
            }
            Log.d(mTAG, "enableDynamicBitRateAdaptation() called with " + flag);
            if (flag) {
                if (nativeExecuteRuntimeCommand(RuntimecmdType.ENABLE_BITRATE_ADAPT
                                .getCmdType(), mNativePtr) != 0) {
                    return ErrorType.UNKNOWN.getCode();
                }
            } else {
                if (nativeExecuteRuntimeCommand(RuntimecmdType.DISABLE_BITRATE_ADAPT
                                .getCmdType(), mNativePtr)!= 0) {
                    return ErrorType.UNKNOWN.getCode();
                }
            }
        }
        return 0;
    }

    @Override
    public int registerHIDEventListener(IHIDEventListener listener) {
        synchronized (mLock) {
            Log.d(mTAG, "registerHIDEventListener()");
            if (listener != null) {
                return mHIDEventListeners.register(listener) ? 0
                        : ErrorType.UNKNOWN.getCode();
            }
            Log.e(mTAG, "HIDEventListener cannot be null!");
            return ErrorType.INVALID_ARG.getCode();
        }
    }

    @Override
    public int unregisterHIDEventListener(IHIDEventListener listener) {
        synchronized (mLock) {
            Log.d(mTAG, "unregisterHIDEventListener()");
            if (listener != null) {
                return mHIDEventListeners.unregister(listener) ? 0
                        : ErrorType.UNKNOWN.getCode();
            }
            Log.e(mTAG, "HIDEventListener cannot be null!");
            return ErrorType.INVALID_ARG.getCode();
        }
    }

    @Override
    public int setSurface(Surface mmSurface) {
        synchronized(mLock) {
            if (mDeviceType == WFDDeviceType.SOURCE) {
                Log.d(mTAG, "Device type is source. Ignoring setSurface");
                return ErrorType.NOT_SINK_DEVICE.getCode();
            }
            if(mSurface != null) {
                mSurface.release();
                if(mSurface.isValid()) {
                    Log.e(mTAG, "Something really bad happened!");
                }
            }
            mSurface = mmSurface;
            Log.d(mTAG,"Value of mNativePtr = " + mNativePtr);
            if(mNativePtr != 0) {
                nativeSetVideoSurface(mmSurface, mNativePtr);
            } else {
                Log.d(mTAG, "Defer setSurface until start session");
            }
            return 0;
        }
    }

    @Override
    public int sendEvent(InputEvent event) {
        if(mUIBCEnabled)
        {
            synchronized(mLock) {
                if (mDeviceType == WFDDeviceType.SOURCE) {
                    Log.d(mTAG, "Device type is source. Ignoring sendEvent");
                    return ErrorType.NOT_SINK_DEVICE.getCode();
                }
            }
            if (event instanceof KeyEvent ||event instanceof MotionEvent) {
                mUIBCManager.addUIBCEvent(event);
            } else {
                return ErrorType.INVALID_ARG.getCode();
            }
        } else {
            Log.d(mTAG, "UIBC connection is not established yet. Ignoring sendEvents");
            return ErrorType.UIBC_NOT_ENABLED.getCode();
        }
        return 0;
    }

    @Override
    @Deprecated
    public int setSurfaceProp(int width, int height, int orientation) {
        synchronized(mLock) {
            if (mDeviceType == WFDDeviceType.SOURCE) {
                Log.e(mTAG,"setSurfaceProp invalid on source");
                return ErrorType.NOT_SINK_DEVICE.getCode();
            } else {
                //Assuming full screen
                int t[] = {0,0,width,height};
                //Just a sanity check for future proofing
                if(t.length != mSurfProp.surfaceCoord.length)
                    throw new ArrayIndexOutOfBoundsException();
                mSurfProp.surfaceCoord = Arrays.copyOf(t,t.length);
                setSurfaceProp();
            }
            return 0;
        }
    }

    private int setSurfaceProp() {
        synchronized (mLock) {
            if(mState != SessionState.TEARING_DOWN &&
               mState != SessionState.INVALID &&
               mState != SessionState.TEARDOWN) {
                getScreenConfiguration(mSurfProp.screenProps);
                Log.e(mTAG,"Setting surface properties as: "+mSurfProp);
                return nativeSetSurfaceProp(mSurfProp,mNativePtr);
            } else {
                Log.e(mTAG, "Set Surface Prop called in bad state");
                return -1;
            }
        }
    }

    private void getScreenConfiguration(int[] cfg) {
        if(cfg == null || cfg.length < 3)
            throw new IllegalArgumentException("Invalid args to " +
                    "getScreenConfiguration");
        Display display = ((WindowManager) mContext
                .getSystemService(mContext.WINDOW_SERVICE))
                .getDefaultDisplay();
        Point size = new Point();
        display.getRealSize(size);
        Configuration config = mContext.getResources()
                .getConfiguration();
        cfg[0] = size.x;
        cfg[1] = size.y;
        cfg[2] = config.orientation;
        Log.e(mTAG,"Screen config: "+ Arrays.toString(cfg));
    }

    @Override
    public int setSurfacePropEx(Bundle surfaceProp){
        synchronized (mLock){
            int[] surfProp = surfaceProp.getIntArray(WfdEnums.SURFACE_PROP_KEY);
            if((surfProp != null) && (surfProp.length ==
                    mSurfProp.surfaceCoord.length) ) {
                mSurfProp.surfaceCoord = Arrays.copyOf(surfProp,surfProp.length);
                return setSurfaceProp();
            } else {
                return ErrorType.INVALID_ARG.getCode();
            }
        }
    }

    @Override
    public int uibcRotate(int angle) {
        synchronized (mLock) {
            Log.d(mTAG, "uibcRotate");
            if(mDeviceType == WFDDeviceType.SOURCE || mDeviceType == WFDDeviceType.SOURCE_PRIMARY_SINK) {
                Log.e(mTAG,"Source device can't send UIBC rotation event");
                return ErrorType.NOT_SINK_DEVICE.getCode();
            }
            if(angle!=0 && angle!=90 && angle!=180 && angle!=270) {
                //Right now WFD source supports only 0,90,180,270 degrees rotations
                Log.e(mTAG, "Unsupported angle value!");
                return ErrorType.INVALID_ARG.getCode();
            }
            if (!mUIBCEnabled) {
                Log.e(mTAG, "UIBC not enabled");
                return ErrorType.INCORRECT_STATE_FOR_OPERATION.getCode();
            }
            if (nativeSendUIBCRotateEvent(angle,mNativePtr) == 1) {
                return 0;
            } else {
                return ErrorType.UNKNOWN.getCode();
            }
        }
    }

    @Override
    public int execRuntimeCommand(int command) throws RemoteException {
        int ret = -1;
        synchronized(mLock) {
            Log.d(mTAG,"execRuntimeCommand called with " + command);
            if (command == RuntimecmdType.DISABLE_AUDIO.getCmdType() ||
                command == RuntimecmdType.ENABLE_AUDIO.getCmdType() ||
                command == RuntimecmdType.DISABLE_VIDEO.getCmdType() ||
                command == RuntimecmdType.ENABLE_VIDEO.getCmdType()) {
                ret = nativeExecuteRuntimeCommand(command, mNativePtr);
            }
        }
        return ret == 1?0:-1;
    }

    @Override
    public int audioPause()
    {
        Log.d(mTAG, "audioPause");

        int retval = nativeExecuteRuntimeCommand(
            RuntimecmdType.DISABLE_AUDIO.getCmdType(), mNativePtr);
        Log.d(mTAG, "audioPause retval = " + retval);
        return 0 == retval ? 0 : ErrorType.UNKNOWN.getCode();
    }

    @Override
    public int audioResume()
    {
        Log.d(mTAG, "audioResume");
        int retval = nativeExecuteRuntimeCommand(
            RuntimecmdType.ENABLE_AUDIO.getCmdType(), mNativePtr);
        Log.d(mTAG, "audioResume retval = " + retval);
        return 0 == retval ? 0 : ErrorType.UNKNOWN.getCode();
    }

    @Override
    public int videoPause()
    {
        Log.d(mTAG, "videoPause");
        int retval = nativeExecuteRuntimeCommand(
            RuntimecmdType.DISABLE_VIDEO.getCmdType(), mNativePtr);
        Log.d(mTAG, "videoPause retval = " + retval);

        return 0 == retval ? 0 : ErrorType.UNKNOWN.getCode();
    }

    @Override
    public int videoResume()
    {
        Log.d(mTAG, "videoResume");
        int retval = nativeExecuteRuntimeCommand(
            RuntimecmdType.ENABLE_VIDEO.getCmdType(), mNativePtr);
        Log.d(mTAG, "videoResume retval = " + retval);
        return 0 == retval ? 0 : ErrorType.UNKNOWN.getCode();
    }
    private void eventCallback(String eventName, Object[] objectArray) {
        Log.d(mTAG, "eventCallback triggered");
        if (eventName == null || objectArray == null) {
            Log.d(mTAG, "No event info, ignore.");
            return;
        }
        int array_length = objectArray.length;
        Log.d(mTAG, "CallbackEvent \"" + eventName + "\" --- objectArray length=" + array_length);
        for (int i=0;i<array_length;i++) {
            if (objectArray[i] != null) {
                Log.d(mTAG,
                        "\tobjectArray[" + i + "] = "
                                + objectArray[i].toString());
            }
        }

        if ("Error".equalsIgnoreCase(eventName)) {
            if (objectArray.length > 0) {
                if ("RTSPCloseCallback".equalsIgnoreCase((String)objectArray[0])) {
                    Log.d(mTAG, "RTSP close callback, treat as TEARDOWN start");
                    notifyEvent(WfdEnums.WfdEvent.TEARDOWN_START, -1);
                }
                if ("StartSessionFail".equalsIgnoreCase((String)objectArray[0])) {
                    Log.e(mTAG, "Start of WFD Session Failed");
                    notifyEvent(WfdEnums.WfdEvent.START_SESSION_FAIL, -1);
                }
            }

        } else if ("WFDServiceDied".equalsIgnoreCase(eventName)) {
            Log.e(mTAG, "WFD native service died");
            updateState(WfdEnums.SessionState.TEARDOWN, -1);
        } else if ("ServiceStateChanged".equalsIgnoreCase(eventName)) {
            boolean enabled = "enabled".equalsIgnoreCase((String)objectArray[0]);
            if (enabled) {
                notifyEvent(WfdEnums.WfdEvent.WFD_SERVICE_ENABLED, 0);
                Log.d(mTAG, eventName + " WFD_SERVICE_ENABLED");
            } else {
                notifyEvent(WfdEnums.WfdEvent.WFD_SERVICE_DISABLED, 0);
                Log.d(mTAG, eventName + " WFD_SERVICE_DISABLED");
            }

        } else if ("SessionStateChanged".equalsIgnoreCase(eventName)) {
            //if (objectArray.length == 3 && "ESTABLISHED".compareToIgnoreCase((String)objectArray[0]) == 0) {
            if (objectArray.length == 3) {
                String state = (String)objectArray[0];
                int sessionId = Integer.parseInt((String)objectArray[2]);
                if ("STANDBY".equalsIgnoreCase(state)) {
                    notifyEvent(WfdEnums.WfdEvent.STANDBY_START, sessionId);
                } else if ("ESTABLISHED".equalsIgnoreCase(state)) {
                    updateState(WfdEnums.SessionState.ESTABLISHED, sessionId);
                } else if ("STOPPED".equalsIgnoreCase(state)){
                    updateState(WfdEnums.SessionState.TEARDOWN, sessionId);
                } else {
                    Log.d(mTAG, "No Session state change is required for native state " + state);
                }
                Log.d(mTAG, "Event: " + eventName + " mState: " + (String)objectArray[2]);
            }
            //WFDService.wfdServiceState.obtainMessage(WFDState.WFD_SESSION_STATE_CHANGED, objectArray).sendToTarget();

        } else if ("StreamControlCompleted".equalsIgnoreCase(eventName)) {
            Log.d(mTAG, eventName);
            if (objectArray.length >= 2) {
                String state = (String)objectArray[1];
                int sessionId = Integer.parseInt((String)objectArray[0]);
                if ("PLAY".equalsIgnoreCase(state)) {
                    notifyEvent(WfdEnums.WfdEvent.PLAY_START, sessionId);
                } else if ("PLAY_DONE".equalsIgnoreCase(state)) {
                    updateState(WfdEnums.SessionState.PLAY, sessionId);
                } else if ("PAUSE".equalsIgnoreCase(state)) {
                    notifyEvent(WfdEnums.WfdEvent.PAUSE_START, sessionId);
                } else if ("PAUSE_DONE".equalsIgnoreCase(state)) {
                    updateState(WfdEnums.SessionState.PAUSE, sessionId);
                } else if ("TEARDOWN".equalsIgnoreCase(state)) {
                    notifyEvent(WfdEnums.WfdEvent.TEARDOWN_START, sessionId);
                }
            }
        } else if ("UIBCControlCompleted".equalsIgnoreCase(eventName)) {
            if (objectArray.length >= 2) {
                int sessionId = Integer.parseInt((String)objectArray[0]);
                String state = (String) objectArray[1];
                if ("ENABLED".equalsIgnoreCase(state)) {
                    notifyEvent(WfdEnums.WfdEvent.UIBC_ENABLED, sessionId);
                    Log.d(mTAG, eventName + " ENABLED");
                } else {
                    notifyEvent(WfdEnums.WfdEvent.UIBC_DISABLED, sessionId);
                    Log.d(mTAG, eventName + " DISABLED");
                }
            }
        } else if ("UIBCRotateEvent".equalsIgnoreCase(eventName)) {
            if (objectArray.length >= 1) {
                int angle = -1;
                try {
                    angle = Integer.parseInt((String) objectArray[0]);
                } catch (NumberFormatException e) {
                    Log.e(mTAG, "Number Format Exceptionwhile parsing value");
                    e.printStackTrace();
                }
                Bundle b = new Bundle(2);
                b.putString("event", eventName);
                b.putInt("rot_angle", angle);
                notify(b, -1);
                Log.e(mTAG, eventName + "Angle = " + angle);
            }
        } else if ("MMEvent".equalsIgnoreCase(eventName)) {
            if (objectArray.length >= 2) {
                String var = (String) objectArray[0];
                String value = (String) objectArray[1];
                if ("AUDIO_STREAM".equalsIgnoreCase(var)) {
                    if ("ENABLED".equalsIgnoreCase(value)) {
                        Log.e(mTAG, "Received audio_stream enabled");
                        notifyEvent(WfdEnums.WfdEvent.AUDIO_STREAM_ENABLED, -1);
                    }
                    else if ("DISABLED".equalsIgnoreCase(value)) {
                        Log.e(mTAG, "Received audio_stream disabled");
                        notifyEvent(WfdEnums.WfdEvent.AUDIO_STREAM_DISABLED, -1);
                    }
                }
                else if ("VIDEO_STREAM".equalsIgnoreCase(var)) {
                    if ("ENABLED".equalsIgnoreCase(value)) {
                        Log.e(mTAG, "Received video_stream enabled");
                        notifyEvent(WfdEnums.WfdEvent.VIDEO_STREAM_ENABLED, -1);
                    }
                    else if ("DISABLED".equalsIgnoreCase(value)) {
                        Log.e(mTAG, "Received video_stream disabled");
                        notifyEvent(WfdEnums.WfdEvent.VIDEO_STREAM_DISABLED, -1);
                    }
                }
                else
                if ("HDCP_CONNECT".equalsIgnoreCase(var)) {
                    if ("SUCCESS".equalsIgnoreCase(value)) {
                        notifyEvent(WfdEnums.WfdEvent.HDCP_CONNECT_SUCCESS, -1);
                    } else if ("UNSUPPORTEDBYPEER".equalsIgnoreCase(value)) {
                        notifyEvent(WfdEnums.WfdEvent.HDCP_ENFORCE_FAIL, -1);
                    } else {
                        notifyEvent(WfdEnums.WfdEvent.HDCP_CONNECT_FAIL, -1);
                    }
                } else if("MMStreamStarted".equalsIgnoreCase(var)) {
                    int width = Integer.parseInt((String) objectArray[1]);
                    int height = Integer.parseInt((String) objectArray[2]);
                    int hdcp = 0;
                    if (objectArray.length > 3) {
                        hdcp = Integer.parseInt((String) objectArray[3]);
                    }
                    Surface surface = null;
                    if (array_length > 4) {
                        // Surface argument is there in the callback as well
                        try {
                            if (objectArray[4] != null) {
                                Log.e(mTAG, objectArray[4].getClass().getName());
                                surface = (Surface) objectArray[4];
                            }
                        } catch (Exception e) {
                            e.printStackTrace();
                        }
                        // Sanity check
                        if (surface != null) {
                            if (surface.isValid()) {
                                Log.d(mTAG, "Received a valid surface");
                            }
                        }
                    }
                    Log.d(mTAG, "MMStreamStarted " + width + " " + height + " "
                            + hdcp);
                    Bundle b = new Bundle();
                    b.putString("event", var);
                    b.putInt("width", width);
                    b.putInt("height", height);
                    b.putInt("hdcp", hdcp);
                    b.putParcelable("surface", surface);
                    notify(b, -1);
                }
            }
        } else {
            if ("VideoEvent".equalsIgnoreCase(eventName)) {
                if (objectArray.length >= 1) {
                    String desc = (String) objectArray[0];
                    if ("RuntimeError".equalsIgnoreCase(desc)) {
                        notifyEvent(WfdEnums.WfdEvent.VIDEO_RUNTIME_ERROR, -1);
                    } else if ("ConfigureFailure".equalsIgnoreCase(desc)) {
                        notifyEvent(WfdEnums.WfdEvent.VIDEO_CONFIGURE_FAILURE, -1);
                    } else {
                        Log.d(mTAG, "Unknown description:" + desc);
                    }
                } else {
                    Log.e(mTAG, "No description for VideoEvent");
                }
            } else {
                if ("AudioEvent".equalsIgnoreCase(eventName)) {
                    if (objectArray.length >= 1) {
                        String desc = (String) objectArray[0];
                        if ("RuntimeError".equalsIgnoreCase(desc)) {
                            notifyEvent(WfdEnums.WfdEvent.AUDIO_RUNTIME_ERROR, -1);
                        } else if ("ConfigureFailure".equalsIgnoreCase(desc)) {
                            notifyEvent(WfdEnums.WfdEvent.AUDIO_CONFIGURE_FAILURE, -1);
                        } else if ("AudioProxyOpened".equalsIgnoreCase(desc)) {
                            notifyEvent(WfdEnums.WfdEvent.AUDIOPROXY_OPENED, -1);
                        } else if ("AudioProxyClosed".equalsIgnoreCase(desc)) {
                            notifyEvent(WfdEnums.WfdEvent.AUDIOPROXY_CLOSED, -1);
                        } else if ("AudioOnlySession".equalsIgnoreCase(desc)) {
                            notifyEvent(WfdEnums.WfdEvent.AUDIO_ONLY_SESSION, -1);
                        } else {
                            Log.e(mTAG, "Unknown description:" + desc);
                        }
                    } else {
                        Log.e(mTAG, "No description for AudioEvent");
                    }
                } else {
                    if ("HdcpEvent".equalsIgnoreCase(eventName)) {
                        if (objectArray.length >= 1) {
                            String desc = (String) objectArray[0];
                            if ("RuntimeError".equalsIgnoreCase(desc)) {
                                notifyEvent(WfdEnums.WfdEvent.HDCP_RUNTIME_ERROR, -1);
                            } else {
                                Log.e(mTAG, "Unknown description:" + desc);
                            }
                        } else {
                            Log.e(mTAG, "No description for AudioEvent");
                        }
                    } else {
                        if ("NetworkEvent".equalsIgnoreCase(eventName)) {
                            if (objectArray.length >= 1) {
                                String desc = (String) objectArray[0];
                                if ("RuntimeError".equalsIgnoreCase(desc)) {
                                    notifyEvent(WfdEnums.WfdEvent.NETWORK_RUNTIME_ERROR, -1);
                                } else if ("ConfigureFailure".equalsIgnoreCase(desc)) {
                                    notifyEvent(WfdEvent.NETWORK_CONFIGURE_FAILURE, -1);
                                } else if ("RtpTransportNegotiationSuccess".equalsIgnoreCase(desc)) {
                                    notifyEvent(WfdEvent.RTP_TRANSPORT_NEGOTIATED, -1);
                                    Bundle b = new Bundle(4);
                                    b.putString("event", desc);
                                    b.putString("status", "0");
                                    if (objectArray.length > 1) {
                                        b.putString("prevMode", (String) objectArray[1]);
                                        b.putString("newMode", (String) objectArray[2]);
                                    }
                                    notify(b, -1);
                                } else if ("BufferingUpdate".equalsIgnoreCase(desc) && objectArray.length >= 3) {
                                    Bundle b = new Bundle(3);
                                    b.putString("event", desc);
                                    b.putString("bufferLength", (String) objectArray[1]);
                                    b.putString("windowSize", (String) objectArray[2]);
                                    notify(b, -1);
                                } else if ("setDecoderLatency".equalsIgnoreCase(desc)) {
                                    Bundle b = new Bundle(3);
                                    b.putString("event", desc);
                                    b.putString("status", (String) objectArray[1]);
                                    notify(b, -1);
                                } else if ("TCPTransportSupport".equalsIgnoreCase(desc)) {
                                    Bundle b = new Bundle(3);
                                    b.putString("event", desc);
                                    b.putString("status", (String) objectArray[1]);
                                    notify(b, -1);
                                } else if ("TCPPlaybackControl".equalsIgnoreCase(desc)) {
                                    Bundle b = new Bundle(3);
                                    b.putString("event", desc);
                                    b.putString("cmd", (String) objectArray[1]);
                                    b.putString("status", (String) objectArray[2]);
                                    if (objectArray.length >= 5) {
                                        b.putString("bufferLength", (String) objectArray[3]);
                                        b.putString("windowSize", (String) objectArray[4]);
                                    }
                                    notify(b, -1);
                                    notifyEvent(WfdEvent.TCP_PLAYBACK_CONTROL, -1);
                                } else if ("RTCPRRMessage".equalsIgnoreCase(desc)) {
                                    // The RTCPRR callback should have a minimum of 3 objects in
                                    // the array [0] will have the description name [1] will
                                    // have the length of the message and following that there
                                    // will be the RTCP message itself. Hence don't accept
                                    // callback without a minimum length of 3
                                    if (array_length >= 3) {
                                        Bundle b = new Bundle(3);
                                        b.putString("event", desc);
                                        try {
                                            b.putInt("length",
                                                    Integer.parseInt((String) objectArray[1]));
                                            StringBuilder sb = new StringBuilder();
                                            for (int i = 2; i < array_length; i++) {
                                                sb.append((String) objectArray[i]);
                                            }
                                            b.putString("mesg", sb.toString());
                                            notify(b, -1);
                                        } catch (Exception e) {
                                            Log.e(mTAG, "Exception while parsing RTCP message"
                                                    + e);
                                        }
                                    } else {
                                        Log.e(mTAG, "Too few params in RTCPRR callback");
                                    }
                                }

                            } else {
                                Log.e(mTAG, "No description for NetworkEvent");
                            }
                        } else {
                            Log.e(mTAG, "Receive unrecognized event from native layer: " + eventName);
                        }
                    }
                }
            }
        }
    }

    private void updateState(WfdEnums.SessionState state, int sessionId) {
        synchronized (mLock) {
            if (state == mState && sessionId == mSessionId) {
                Log.d(mTAG, "Nothing has changed. Ignoring updateState");
                return;
            }
            if (mState == SessionState.STANDING_BY
                    && state == SessionState.PAUSE) {
                mState = SessionState.STANDBY;
            } else {
                mState = state;
            }
            if (sessionId != mSessionId) {
                Log.d(mTAG, "Session id changed from " + mSessionId + " to " + sessionId
                        + " with state " + state);
            } else {
                Log.d(mTAG, "Session id " + sessionId + " with state " + state);
            }
            mSessionId = sessionId;
            mLock.notifyAll();

            if (mState == SessionState.PAUSE) {
                Log.d(mTAG, "PAUSE done");
            } else if (mState == SessionState.STANDBY) {
                Log.d(mTAG, "STANDBY Done");
            } else if (mState == SessionState.PLAY) {
                Log.d(mTAG, "PLAY done");
            } else if (mState == SessionState.TEARDOWN) {
                Log.d(mTAG, "TEARDOWN done");
                if (mDeviceType != WFDDeviceType.SOURCE) {
                    if(mSurface != null) {
                        mSurface.release();
                        if(mSurface.isValid()) {
                            Log.e(mTAG, "Something really bad happened!");
                        }
                        else {
                            mSurface = null;
                            Log.e(mTAG,"Released Surface during TEARDOWN Done!");
                        }
                    }
                } else {
                    releaseVDS();
                    mRequestProxyRouting = false;
                    broadcastWifiDisplayAudioIntent(false);
                }
                mState = SessionState.INITIALIZED;
                if (mSessionId < 0) {
                    // teardown called in some transient SM state
                    // Update it to a +ve value to give callback with a +ve
                    // session Id so that client handling remains intact
                    mSessionId = 1;
                }
            }

            // Broadcast status update to listeners
            if (mActionListeners != null) {
                Log.d(mTAG, "Sending onStateUpdate() to listeners");
                final int N = mActionListeners.beginBroadcast();
                for (int i = 0; i < N; i++) {
                    try {
                        mActionListeners.getBroadcastItem(i).onStateUpdate(mState.ordinal(),
                                mSessionId);
                    } catch (RemoteException e) {
                        Log.e(mTAG, "Error sending status update to client, removing listener");
                    }
                }
                mActionListeners.finishBroadcast();
            } else {
                Log.d(mTAG, "updateState: Remote callback list is null");
            }
        }
    }


    private void notify(Bundle b, int SessionId) {
        synchronized (mLock) {
            Log.d(mTAG, "notify()");
            if (b != null) {
                String event = b.getString("event", "none");
                if (!event.equalsIgnoreCase("none")) {
                    if ("UIBCRotateEvent".equalsIgnoreCase(event)) {
                        int angle = b.getInt("rot_angle", -1);
                        switch (angle) {
                            case 0:
                                mUIBCRotation = Surface.ROTATION_0;
                                break;
                            case 90:
                                mUIBCRotation = Surface.ROTATION_90;
                                break;
                            case 180:
                                mUIBCRotation = Surface.ROTATION_180;
                                break;
                            case 270:
                                mUIBCRotation = Surface.ROTATION_270;
                                break;
                            default:
                                mUIBCRotation = -1;
                        }
                        Log.d(mTAG, "Value of mUIBCRotation is " + mUIBCRotation);
                        broadcastUIBCRotation(mUIBCRotation);
                    } else if ("MMStreamStarted".equalsIgnoreCase(event)) {
                        // Only in case the WFD session is established through
                        // client app do we need to create a virtual display,
                        // else a WifiDisplay is created anyway which will be
                        // taking care of notifying the surface to Display
                        // framework
                        int width = b.getInt("width");
                        int height = b.getInt("height");
                        int secure = b.getInt("hdcp");
                        if (!mSysInvoked) {
                            Log.d(mTAG,
                                    "MM Stream Started Width, Height and secure: "
                                            + " " + width + " " + height + " "
                                            + secure);
                            Surface surface = (Surface) (b
                                    .getParcelable("surface"));
                            if (surface != null) {
                                Log.d(mTAG, "Surface supplied by source modules");
                                int flags = 0;
                                flags |= DisplayManager.VIRTUAL_DISPLAY_FLAG_PUBLIC
                                        | DisplayManager.VIRTUAL_DISPLAY_FLAG_PRESENTATION;
                                if (secure == 1) {
                                    flags |= DisplayManager.VIRTUAL_DISPLAY_FLAG_SECURE;
//                                            | DisplayManager.VIRTUAL_DISPLAY_FLAG_SUPPORTS_PROTECTED_BUFFERS;
                                }
                                mVirtualDisplayDPI = Math.min(width, height)
                                        * DisplayMetrics.DENSITY_XHIGH / 1080;
                                if (mDisplayManager != null) {
                                    mVirtualDisplay = mDisplayManager
                                            .createVirtualDisplay(
                                                    mPeerDevice.deviceName,
                                                    width,
                                                    height,
                                                    mVirtualDisplayDPI,
                                                    surface, flags);
                                }
                            }
                        }

                        if (mDebug) {
                            createNotification(width, height, secure);
                        }
                    }
                }
            }
            //Broadcast event to the listeners
            final int N = mActionListeners.beginBroadcast();
            for (int i = 0; i < N; i++) {
                try {
                    mActionListeners.getBroadcastItem(i).notify(b, SessionId);
                } catch (RemoteException e) {
                    Log.e(mTAG, "Error sending notification to client, removing listener");
                }
            }
            mActionListeners.finishBroadcast();
        }
    }

    private void notifyEvent(WfdEvent event, int SessId) {
        synchronized (mLock) {
            {
                if (event == WfdEvent.UIBC_ENABLED) {
                    Log.d(mTAG, "UIBC called from "+ mDeviceType);
                    try {
                        if (mUIBCManager == null) {
                            mUIBCManager = new UIBCManager(mContext, mDeviceType.getCode(), this, this);
                        }
                    } catch (InstantiationException e) {
                        Log.e(mTAG, "InstantiationException: " + e);
                    }
                    catch (IllegalAccessException e) {
                        Log.e(mTAG, "IllegalAccessException: " + e);
                    }
                    catch (ClassNotFoundException e) {
                        Log.e(mTAG, "ClassNotFoundException: " + e);
                    }
                    if (mDeviceType == WFDDeviceType.SOURCE) {
                        setSurfaceProp();
                    }
                    /* For source device, 1) start thread. 2) enable UIBCAdaptor. */
                    if(mDeviceType == WFDDeviceType.SOURCE) {
                        mUIBCManager.start();
                        nativeStartUIBC(mUIBCManager.eventDispatcher,mNativePtr);
                    } else {
                    /* For sink device, 1) enable UIBCAdaptor. 2) start thread. */
                        nativeStartUIBC(mUIBCManager.eventDispatcher, mNativePtr);
                        mUIBCManager.start();
                    }
                    mUIBCEnabled = true;
                    broadcastUIBCRotation(mUIBCRotation);
                } else if (event == WfdEvent.UIBC_DISABLED) {
                    if (mDeviceType == WFDDeviceType.SOURCE) {
                        /* For source device, 1) disable UIBCAdaptor. 2) stop thread. */
                        nativeStopUIBC(mNativePtr);
                        mUIBCManager.stop();
                    } else {
                        /* For sink device, 1) stop thread. 2) disable UIBCAdaptor. */
                        mUIBCManager.stop();
                        nativeStopUIBC(mNativePtr);
                    }
                    mUIBCEnabled = false;
                    broadcastUIBCRotation(-1);
                } else if (event == WfdEvent.PAUSE_START) {
                    Log.d(mTAG, "PAUSE start");
                    mState = SessionState.PAUSING;
                } else if (event == WfdEvent.STANDBY_START) {
                    mState = SessionState.STANDING_BY;
                    Log.d(mTAG, "STANDBY_START");
                } else if (event == WfdEvent.PLAY_START) {
                    Log.d(mTAG, "PLAY start");
                    mState = SessionState.PLAYING;
                } else if (event == WfdEvent.AUDIO_STREAM_ENABLED) {
                    Log.d(mTAG, "handle AUDIO_STREAM_ENABLED");
                    // nothing to do
                } else if (event == WfdEvent.AUDIO_STREAM_DISABLED) {
                    Log.d(mTAG, "handle AUDIO_STREAM_DISABLED");
                    broadcastWifiDisplayAudioIntent(false);
                } else if (event == WfdEvent.VIDEO_STREAM_ENABLED) {
                    Log.d(mTAG, "handle VIDEO_STREAM_ENABLED");
                    broadcastWifiDisplayVideoEnabled(true);
                } else if (event == WfdEvent.VIDEO_STREAM_DISABLED) {
                    Log.d(mTAG, "handle VIDEO_STREAM_DISABLED");
                    broadcastWifiDisplayVideoEnabled(false);
                }
                else if (event == WfdEvent.TEARDOWN_START) {
                    Log.d(mTAG, "TEARDOWN start");
                    mRequestProxyRouting = false;
                    mState = SessionState.TEARING_DOWN;
                    // At this point we are sure that RTSP teardown sequence has
                    // been completed successfully, so release the virtual display
                    // in case it hasn't been done already. In case sink is abruptly
                    // shutdown, then closecallback should map here as well, which
                    // will again take care of releasing virtual display, for all
                    // other runtime failures, teardown is called which will release
                    // the surface
                    releaseVDS();
                } else if (event == WfdEvent.HDCP_CONNECT_SUCCESS) {
                    Log.d(mTAG, "HDCP Connect Success");
                    mIsHDCPEnabled = true;
                } else if (event == WfdEvent.HDCP_CONNECT_FAIL) {
                    Log.d(mTAG, "HDCP Connect Fail");
                    mIsHDCPEnabled = false;
                } else if (event == WfdEvent.HDCP_ENFORCE_FAIL) {
                    Log.d(mTAG, "HDCP Unsupported by Peer. Fail");
                    mIsHDCPEnabled = false;
                    nativeTeardown(mSessionId, true, mNativePtr);
                } else if(event == WfdEvent.VIDEO_RUNTIME_ERROR ||
                        event == WfdEvent.AUDIO_RUNTIME_ERROR ||
                        event == WfdEvent.HDCP_RUNTIME_ERROR ||
                        event == WfdEvent.VIDEO_CONFIGURE_FAILURE ||
                        event == WfdEvent.AUDIO_CONFIGURE_FAILURE ||
                        event == WfdEvent.NETWORK_RUNTIME_ERROR ||
                        event == WfdEvent.NETWORK_CONFIGURE_FAILURE ||
                        event == WfdEvent.START_SESSION_FAIL) {
                    Log.d(mTAG, "Error event received:" + event);
                    teardown();
                } else if (event == WfdEvent.RTP_TRANSPORT_NEGOTIATED) {
                    Log.d(mTAG, "RTP transport is changed successfully");
                    mIsRtpTransportNegotiated = true;
                }else if (event == WfdEvent.AUDIOPROXY_CLOSED ) {
                    broadcastWifiDisplayAudioIntent(false);
                }else if (event == WfdEvent.AUDIOPROXY_OPENED ) {
                    broadcastWifiDisplayAudioIntent(true);
                }else if (event == WfdEvent.TCP_PLAYBACK_CONTROL) {
                    mIsPlaybackControlComplete = true;
                }

                // Broadcast event to the listeners
                if (mActionListeners != null) {
                    Log.d(mTAG, "Sending notifyEvent() to listeners");
                    final int N = mActionListeners.beginBroadcast();
                    for (int i = 0; i < N; i++) {
                        try {
                            mActionListeners.getBroadcastItem(i).notifyEvent(event.ordinal(), mSessionId);
                        } catch (RemoteException e) {
                            Log.e(mTAG, "Error sending notification to client, removing listener");
                        }
                    }
                    mActionListeners.finishBroadcast();
                } else {
                    Log.d(mTAG, "notifyEvent: Remote callback list is null");
                }
            }
        }
    }

    /**
     * @param width
     *            Width of WFD session resolution
     * @param height
     *            Height of WFD session resolution
     * @param secure
     *            Whether WFD session is secure or not This is a helper method
     *            to create a notification in the notification drawer
     */
    private void createNotification(int width, int height, int secure) {
        final int iconWidth = 200;
        final int iconHeight = 200;
        mNotiMgr = (NotificationManager) mContext
                .getSystemService(Context.NOTIFICATION_SERVICE);
        Notification.Builder notiBldr = new Notification.Builder(mContext)
                .setContentTitle("Miracast")
                .setContentText(
                        "Session: " + width + " X " + height + " Secure: "
                                + secure).setUsesChronometer(true);
        Bitmap notiIcon = null, fwIcon = null;
        if (secure == 1) {
            notiBldr.setSmallIcon(android.R.drawable.ic_secure);
            try {
                fwIcon = BitmapFactory.decodeResource(Resources.getSystem(),
                        android.R.drawable.ic_lock_lock);
                notiIcon = Bitmap.createScaledBitmap(fwIcon, iconWidth,
                        iconHeight, false);
            } catch (Exception e) {
                // Ignore
            }
        } else {
            notiBldr.setSmallIcon(android.R.drawable.ic_menu_view);
            try {
                fwIcon = BitmapFactory.decodeResource(Resources.getSystem(),
                        android.R.drawable.ic_menu_compass);
                notiIcon = Bitmap.createScaledBitmap(fwIcon, iconWidth,
                        iconHeight, false);
            } catch (Exception e) {
                // Ignore
            }
        }
        if (fwIcon != null) {
            fwIcon.recycle();
            fwIcon = null;
        }
        if (notiIcon != null) {
            notiBldr.setLargeIcon(notiIcon);
        }
        Notification noti = notiBldr.build();
        if (mNotiMgr != null) {
            mNotiMsgId = NOTI_MSG_ID.getAndIncrement();
            mNotiMgr.notify(mNotiMsgId, noti);
        }
    }


    /**
     * This is a helper method to release the instance of Virtual Display
     * created
     */
    private void releaseVDS() {
        synchronized (mLock) {
            if (mVirtualDisplay != null) {
                Log.e(mTAG, "Releasing Virtual Display");
                mVirtualDisplay.release();
                mVirtualDisplay = null;
            }
        }
    }

    /**
     * This function is used to set default values to all variables that
     * might have been altered during the session but might not have got
     * a chance to clear up due to abrupt teardown. This will enable the
     * session to start in a clean slate state
     */
    private void clearSession() {
        synchronized (mLock) {
            releaseVDS();
            mActionListeners.kill();
            mActionListeners = null;
            mHIDEventListeners.kill();
            mHIDEventListeners = null;
            mContext.unregisterReceiver(mReceiver);
            mReceiver = null;
            mContext.unregisterReceiver(mProtectedReceiver);
            mProtectedReceiver = null;
            mDisplayManager = null;
            mUIBCManager = null;
            mUIBCEnabled = false;
            mUIBCRotation = -1;
            mPeerDevice = null;
            mSysInvoked = false;
            mRequestProxyRouting = true;
            if (mNotiMgr != null) {
                mNotiMgr.cancel(mNotiMsgId);
                mNotiMgr = null;
            }
            mDebug = false;
            mContext = null;
        }
    }

    private void broadcastWifiDisplayAudioIntent(boolean flag) {
        Log.d(mTAG, "broadcastWifiDisplayAudioIntent() - flag: " + flag);
        if(mDeviceType != WFDDeviceType.SOURCE) {
            Log.e(mTAG, "Abort broadcast as device type is not source");
            return;
        }
        if (mPlaybackMode == AVPlaybackMode.VIDEO_ONLY) {
            Log.d(mTAG, "Playback mode is VIDEO_ONLY, ignore audio intent broadcast");
            return;
        }
        if (flag) {
            if (mRequestProxyRouting) {
                Log.e(mTAG, "Calling Audio System Proxy enable");
                mAudioRoutingRequested = true;
                AudioSystem.setDeviceConnectionState(
                        AudioSystem.DEVICE_OUT_PROXY,
                        AudioSystem.DEVICE_STATE_AVAILABLE,"","");
            }
        } else {
            if(mAudioRoutingRequested) {
                Log.e(mTAG, "Calling Audio System Proxy disable");
                AudioSystem.setDeviceConnectionState(AudioSystem.DEVICE_OUT_PROXY,
                        AudioSystem.DEVICE_STATE_UNAVAILABLE,"","");
                mAudioRoutingRequested = false;
            }
            Log.e(mTAG, "Calling Audio System Proxy disable ...done");
        }
    }

    private void broadcastWifiDisplayVideoEnabled(boolean flag) {
        Log.d(mTAG, "broadcastWifiDisplayVideoEnabled() - flag: " + flag);
        if(mDeviceType != WFDDeviceType.SOURCE) {
            Log.d(mTAG, "Abort broadcast as device type is not source");
            return;
        }
        if (mPlaybackMode == AVPlaybackMode.AUDIO_ONLY) {
            Log.d(mTAG, "Playback mode is AUDIO_ONLY, ignore video intent broadcast");
            return;
        }
        Intent intent = new Intent(WfdEnums.ACTION_WIFI_DISPLAY_VIDEO);
        if (flag) {
            intent.putExtra("state", 1);
            int extra = mUIBCEnabled ? mUIBCRotation :-1;
            intent.putExtra("wfd_UIBC_rot",extra);
        } else {
            intent.putExtra("state", 0);
            intent.putExtra("wfd_UIBC_rot", -1);
        }
        Log.d(mTAG, "Broadcasting WFD video intent: " + intent);
        mContext.sendBroadcast(intent);
    }

    private void broadcastUIBCRotation(int value) {
        if(mDeviceType != WFDDeviceType.SOURCE) {
            Log.d(mTAG, "Abort broadcast as device type is not source");
            return;
        }
        if(mState != SessionState.PLAY) {
            Log.e(mTAG, "Aborting UIBC rotation in invalid WFD state");
            return;
        }
        Log.d(mTAG, "broadcastUIBCRotation called with value " + value);
        Intent intent = new Intent(WfdEnums.ACTION_WIFI_DISPLAY_VIDEO);
        //WFD has to be in PLAY state to support UIBC rotation
        //hence the state should be 1
        intent.putExtra("state", 1);
        intent.putExtra("wfd_UIBC_rot",value);
        mContext.sendBroadcast(intent);
    }
    /*
    Implementation of HIDListener callbacks

     */
    @Override
    public void HIDReportDescRecvd(byte[] hidRepDesc) {
        synchronized (mLock) {
            if (hidRepDesc != null) {
                Log.e(mTAG,
                        "HID Report Descriptor Received: "
                                + Arrays.toString(hidRepDesc));
            }
            if (mHIDEventListeners != null) {
                Log.d(mTAG, "Sending HIDReportDescRecvd() to listeners");
                final int N = mHIDEventListeners.beginBroadcast();
                for (int i = 0; i < N; i++) {
                    try {
                        mHIDEventListeners.getBroadcastItem(i)
                                .onHIDReprtDescRcv(hidRepDesc);
                    } catch (RemoteException e) {
                        Log.e(mTAG,
                                "Error sending HID Report descriptor to client");
                    }
                }
                mHIDEventListeners.finishBroadcast();
            } else {
                Log.d(mTAG, "HIDReportDescRecvd Remote callback list is null");
            }
        }
    }

    @Override
    public void HIDReportRecvd(byte[] hidRep) {
        synchronized (mLock) {
            if (hidRep != null) {
                Log.e(mTAG,
                        "HID Report Received: "
                                + Arrays.toString(hidRep));
            }
            if (mHIDEventListeners != null) {
                Log.d(mTAG, "Sending HIDReportRecvd() to listeners");
                final int N = mHIDEventListeners.beginBroadcast();
                for (int i = 0; i < N; i++) {
                    try {
                        mHIDEventListeners.getBroadcastItem(i).onHIDReprtRcv(
                                hidRep);
                    } catch (RemoteException e) {
                        Log.e(mTAG, "Error sending HID Report to client");
                    }
                }
                mHIDEventListeners.finishBroadcast();
            } else {
                Log.d(mTAG, "HIDReportRecvd Remote callback list is null");
            }
        }
    }

    @Override
    public void sendKeyEvent(KeyEvent k, Bundle b) {
        nativeSendUIBCKeyEvent(k,mNativePtr );
    }

    @Override
    public void sendMotionEvent(MotionEvent m, Bundle b) {
        nativeSendUIBCMotionEvent(m,mNativePtr);
    }

    @Override
     public int toggleDSMode(boolean flag) {

            synchronized (mLock) {
                Log.d(mTAG, "toggleDSMode with flag=" + flag);

                if(flag) {
                    if(nativeExecuteRuntimeCommand(
                            RuntimecmdType.ENABLE_STREAMING_FEATURE.getCmdType(),
                            mNativePtr)!=1) {
                        Log.e(mTAG, "ENABLE_STREAMING_FEATURE failed");
                        return ErrorType.UNKNOWN.getCode();
                    }
                } else {
                    if(nativeExecuteRuntimeCommand(
                            RuntimecmdType.DISABLE_STREAMING_FEATURE.getCmdType(),
                            mNativePtr) !=1) {
                        Log.e(mTAG, "DISABLE_STREAMING_FEATURE failed");
                        return ErrorType.UNKNOWN.getCode();
                    }
                }
                return 1;
            }
        }
}
