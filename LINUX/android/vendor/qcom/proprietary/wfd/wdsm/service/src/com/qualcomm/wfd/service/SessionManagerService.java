/* ==============================================================================
 * SessionManagerService.java
 *
 * Session Manager service implementation
 *
 * Copyright (c) 2012 - 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 ============================================================================== */

package com.qualcomm.wfd.service;

import com.qualcomm.wfd.WFDServiceUtils;
import com.qualcomm.wfd.WFDSession;
import com.qualcomm.wfd.WfdEnums.*;
import com.qualcomm.wfd.WfdStatus;
import com.qualcomm.wfd.WfdDevice;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.RemoteException;
import android.os.IBinder;
import android.os.SystemProperties;

import android.util.Log;
import android.view.InputEvent;
import android.view.Surface;

public class SessionManagerService extends ISessionManagerService.Stub {

    private static final String TAG = "SessionManagerService";

    private static final String sWFDEnableKey = "sys.wfdservice";

    private Context mContext;

    private int[] mConfigItems = null;

    private boolean mServiceStarted = false;

    private boolean mInitSysCalled = false;

    private WFDSession mWfdSession;

    private int mVirtualDisplayDPI;

    private static final long wfdScanInterval = 300000; // ms i.e. 5min

    // This value should be in sync with value of mDisconnectedScanPeriodMs in
    // WiFiStateMachine.java
    private static final long defScanInterval = 10000; // ms

    public SessionManagerService(Context context) {
        Log.d(TAG, "SessionManagerService ctor");
        SystemProperties.set(sWFDEnableKey, "enable");
        mContext = context;
        WFDServiceUtils.setup();
        Intent serviceIntent = new Intent(mContext, WfdService.class);
        if (!mServiceStarted) {
            // Start WFD service to be foreground service
            mContext.startService(serviceIntent);
            mServiceStarted = true;
        }
    }

    protected void destroyService() {
        Log.d(TAG, "destroyService()");
        SystemProperties.set(sWFDEnableKey, "disable");
        WFDServiceUtils.destroy();
    }
    @Override
    public int initSys(IWfdActionListener listener, WfdDevice thisDevice) {
        Log.d(TAG, "SessionManagerService initSys called");
        //set this flag to ensure calling initSys of wfdsession
        mInitSysCalled = true;
        return init(listener,thisDevice);
    }

    /**
     * INVALID => INITIALIZED
     */
    @Override
    public int init(IWfdActionListener listener, WfdDevice thisDevice) {
        if(thisDevice == null) {// TODO: MITask Secure UI is broken with this
            return ErrorType.UNKNOWN.getCode();
        }
        if(mWfdSession == null) {
            mWfdSession = WFDSession.createWFDSession(mContext);
        }
        if(mInitSysCalled){
          Log.d(TAG, "SessionManagerServic init: initSys");
          mWfdSession.initSys(listener,thisDevice);
        }
        return mWfdSession.init(listener,thisDevice);
    }

    @Override
    public synchronized IBinder getWiFiDisplaySession() throws RemoteException {
        Log.d(TAG, "SessionManagerServic getWiFiDisplaySession");
            mWfdSession = WFDSession.createWFDSession(mContext);
        return mWfdSession;
    }

    @Override
    public IBinder getManagedSession() throws RemoteException {
        return mWfdSession;
    }

    @Override
    public int deinit() {
        if (mWfdSession != null) {
            int ret = mWfdSession.deinit();
            mWfdSession = null;

            Intent serviceIntent = new Intent(mContext, WfdService.class);
            if (mServiceStarted) {
                mContext.stopService(serviceIntent);
                mServiceStarted = false;
                Log.d(TAG, "WfdService has stopped");
            }
            return ret;
        }
        return ErrorType.INCORRECT_STATE_FOR_OPERATION.getCode();
    }

    @Override
    public int startWfdSession(WfdDevice device) {
        if (mWfdSession != null) {
            return mWfdSession.startWfdSession(device);
        }
        return ErrorType.INCORRECT_STATE_FOR_OPERATION.getCode();
    }

    @Override
    public int stopWfdSession() {
        if (mWfdSession != null) {
            return mWfdSession.stopWfdSession();
        }
        return ErrorType.INCORRECT_STATE_FOR_OPERATION.getCode();
    }

    @Override
    public int startUibcSession() {
        if (mWfdSession != null) {
            return mWfdSession.startUibcSession();
        }
        return ErrorType.INCORRECT_STATE_FOR_OPERATION.getCode();
    }

    @Override
    public int stopUibcSession() {
        if (mWfdSession != null) {
            return mWfdSession.stopUibcSession();
        }
        return ErrorType.INCORRECT_STATE_FOR_OPERATION.getCode();
    }

    @Override
    public int setUIBC() {
        if (mWfdSession != null) {
            return mWfdSession.setUIBC();
        }
        return ErrorType.INCORRECT_STATE_FOR_OPERATION.getCode();
    }

    @Override
    public boolean getUIBCStatus() {
        if (mWfdSession != null) {
            return mWfdSession.getUIBCStatus();
        }
        return false;
    }

    @Override
    public int setDeviceType(int type) {
        if (mWfdSession == null) {
            mWfdSession = WFDSession.createWFDSession(mContext);
        }
        return mWfdSession.setDeviceType(type);
    }

    @Override
    public int play() {
        if (mWfdSession != null) {
            return mWfdSession.play();
        }
        return ErrorType.INCORRECT_STATE_FOR_OPERATION.getCode();
    }



    @Override
    public int pause() {
        if (mWfdSession != null) {
            return mWfdSession.pause();
        }
        return ErrorType.INCORRECT_STATE_FOR_OPERATION.getCode();
    }

    @Override
    public int teardown() {
        if (mWfdSession != null) {
            return mWfdSession.teardown();
        }
        return ErrorType.INCORRECT_STATE_FOR_OPERATION.getCode();
    }

    @Override
    public int standby() {
        if (mWfdSession != null) {
            return mWfdSession.standby();
        }
        return ErrorType.INCORRECT_STATE_FOR_OPERATION.getCode();
    }

    @Override
    public int getCommonCapabilities(Bundle capabilities) {
        Log.d(TAG, "getCommonCapabilities Un-implemented!");
        return 0;
    }

    @Override
    public int setNegotiatedCapabilities(Bundle capabilities) {
        Log.d(TAG, "setNegotiatedCapabilities() Un-implemented!");
        return 0;
    }

    @Override
    public int getSupportedTypes(int[] types) {
        if (mWfdSession != null) {
            return mWfdSession.getSupportedTypes(types);
        }
        return ErrorType.INCORRECT_STATE_FOR_OPERATION.getCode();
    }

    @Override
    public int getConfigItems(Bundle configItems) {
        if (mWfdSession != null) {
            return mWfdSession.getConfigItems(configItems);
        }
        return ErrorType.INCORRECT_STATE_FOR_OPERATION.getCode();
    }

    @Override
    public int uibcRotate(int angle) {
        if (mWfdSession != null) {
            return mWfdSession.uibcRotate(angle);
        }
        return ErrorType.INCORRECT_STATE_FOR_OPERATION.getCode();
    }

    @Override
    public synchronized IBinder createWiFiDisplay(IWfdActionListener listener, WfdDevice localDev, WfdDevice peerDev) {
        mWfdSession =  WFDSession.createWiFiDisplay(mContext,listener,localDev,peerDev);
        return mWfdSession;
    }

    @Override
    public WfdStatus getStatus() {
        if (mWfdSession != null) {
            return mWfdSession.getStatus();
        }
        return null;
    }

    @Override
    public int unregisterListener(IWfdActionListener listener) {
        if (mWfdSession != null) {
            return mWfdSession.unregisterListener(listener);
        }
        return ErrorType.INCORRECT_STATE_FOR_OPERATION.getCode();
    }

    public int setResolution(int formatType, int value) {
        if (mWfdSession != null) {
            return mWfdSession.setResolution(formatType,value);
        }
        return ErrorType.INCORRECT_STATE_FOR_OPERATION.getCode();
    }

    @Override
    public int setBitrate(int bitRate) {
        if (mWfdSession != null) {
            return mWfdSession.setBitRate(bitRate);
        }
        return ErrorType.INCORRECT_STATE_FOR_OPERATION.getCode();
    }

    @Override
    public int queryTCPTransportSupport() {
        if (mWfdSession != null) {
            return mWfdSession.queryTCPTransportSupport();
        }
        return ErrorType.INCORRECT_STATE_FOR_OPERATION.getCode();
    }

    @Override
    public int setRtpTransport(int transportType, int bufferLengthMs, int port) {
        if (mWfdSession != null) {
            return mWfdSession.setRtpTransport(transportType,bufferLengthMs,port);
        }
        return ErrorType.INCORRECT_STATE_FOR_OPERATION.getCode();
    }

    @Override
    public int tcpPlaybackControl(int cmdType, int cmdVal) {
        if (mWfdSession != null) {
            return mWfdSession.tcpPlaybackControl(cmdType,cmdVal);
        }
        return ErrorType.INCORRECT_STATE_FOR_OPERATION.getCode();
    }

    @Override
    public int setDecoderLatency(int latency) {
        if (mWfdSession != null) {
            return mWfdSession.setDecoderLatency(latency);
        }
        return ErrorType.INCORRECT_STATE_FOR_OPERATION.getCode();
    }

    @Override
    public int audioPause()
    {
        if (mWfdSession != null) {
            return mWfdSession.audioPause();
        }
        return ErrorType.INCORRECT_STATE_FOR_OPERATION.getCode();
    }

    @Override
    public int audioResume()
    {
        if (mWfdSession != null) {
            return mWfdSession.audioResume();
        }
        return ErrorType.INCORRECT_STATE_FOR_OPERATION.getCode();
    }

    @Override
    public int videoPause()
    {
        if (mWfdSession != null) {
            return mWfdSession.videoPause();
        }
        return ErrorType.INCORRECT_STATE_FOR_OPERATION.getCode();
    }

    @Override
    public int videoResume()
    {
        if (mWfdSession != null) {
            return mWfdSession.videoResume();
        }
        return ErrorType.INCORRECT_STATE_FOR_OPERATION.getCode();
    }

    @Override
    public int setAvPlaybackMode(int mode) {
        if (mWfdSession != null) {
            return mWfdSession.setAvPlaybackMode(mode);
        }
        return ErrorType.INCORRECT_STATE_FOR_OPERATION.getCode();
    }

    @Override
    public int setSurface(Surface surface) {
        if (mWfdSession != null) {
            return mWfdSession.setSurface(surface);
        }
        return ErrorType.INCORRECT_STATE_FOR_OPERATION.getCode();
    }

    @Override
    public int sendEvent(InputEvent event) {
        if (mWfdSession != null) {
            return mWfdSession.sendEvent(event);
        }
        return ErrorType.INCORRECT_STATE_FOR_OPERATION.getCode();
    }

    @Override
    public int setSurfaceProp(int width,int height,int orientation) {
        if (mWfdSession != null) {
            return mWfdSession.setSurfaceProp(width,height,orientation);
        }
        return ErrorType.INCORRECT_STATE_FOR_OPERATION.getCode();
    }

    @Override
    public int setSurfacePropEx(Bundle surfProp) {
        if (mWfdSession != null) {
            return mWfdSession.setSurfacePropEx(surfProp);
        }
        return ErrorType.INCORRECT_STATE_FOR_OPERATION.getCode();
    }

    @Override
    public int getNegotiatedResolution(Bundle negRes) {
        if (mWfdSession != null) {
            return mWfdSession.getNegotiatedResolution(negRes);
        }
        return ErrorType.INCORRECT_STATE_FOR_OPERATION.getCode();
    }

    @Override
    public int getCommonResolution(Bundle comRes) {
        if (mWfdSession != null) {
            return mWfdSession.getCommonResolution(comRes);
        }
        return ErrorType.INCORRECT_STATE_FOR_OPERATION.getCode();
    }

    @Override
    public int enableDynamicBitrateAdaptation(boolean flag) {
        if (mWfdSession != null) {
            return mWfdSession.enableDynamicBitRateAdaptation(flag);
        }
        return ErrorType.INCORRECT_STATE_FOR_OPERATION.getCode();
    }

    @Override
    public int registerHIDEventListener(IHIDEventListener listener) {
        if (mWfdSession != null) {
            return mWfdSession.registerHIDEventListener(listener);
        }
        return ErrorType.INCORRECT_STATE_FOR_OPERATION.getCode();
    }

    @Override
    public int unregisterHIDEventListener(IHIDEventListener listener) {
        if (mWfdSession != null) {
            return mWfdSession.unregisterHIDEventListener(listener);
        }
        return ErrorType.INCORRECT_STATE_FOR_OPERATION.getCode();
    }
    @Override
    public int toggleDSMode(boolean flag) {
        if (mWfdSession != null) {
            return mWfdSession.toggleDSMode(flag);
        }
        return ErrorType.INCORRECT_STATE_FOR_OPERATION.getCode();
    }
}
