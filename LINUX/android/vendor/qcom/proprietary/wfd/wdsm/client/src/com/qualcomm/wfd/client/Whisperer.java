/*
* Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

package com.qualcomm.wfd.client;

import android.content.SharedPreferences;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;

import com.qualcomm.wfd.WfdEnums;
import com.qualcomm.wfd.client.net.NetManagerIF;

import java.util.HashMap;
import java.util.Map;

/**
 * Created by ichakrab on 01-07-15.
 */
public class Whisperer {
    private Map<String,Handler> mHandlers;
    private HandlerThread mProcessorThread;
    private static Whisperer w;
    private static final String TAG = "Whisperer";

    public static Whisperer getInstance() {
        synchronized (Whisperer.class) {
            if(w == null) {
                w = new Whisperer();
            }
        }
        return w;
    }

    private Whisperer() {
        mHandlers = new HashMap<String,Handler>();
        mProcessorThread = new HandlerThread(TAG);
    }

    public boolean registerHandler(Handler h, String key) {
        if(h == null || key == null) {
            throw new IllegalArgumentException("Null arguments passed for registering");
        }
        mHandlers.put(key,h);
        return true;
    }

    public boolean unRegisterHandler(String key) {
        if(key == null) {
            throw new IllegalArgumentException("Null value passed for un-registering");
        }
        mHandlers.remove(key);
        return true;
    }

    public boolean sendMessage(String key, Message m) {
        if(m == null) {
            throw new IllegalArgumentException("Null value while sending message");
        }
        Handler h = getHandler(key);
        if(h != null) {
            h.sendMessage(m);
            return true;
        }
        return false;
    }

    public Handler getHandler(String key) {
        if(key == null) {
            throw new IllegalArgumentException("Null key passed");
        }
        return mHandlers.get(key);
    }


    private WFDClientActivity mActivity;
    private WfdEnums.WFDDeviceType mLocalDeviceType = WfdEnums.WFDDeviceType.SOURCE;
    private boolean mUIBCM14 = false;
    private boolean mModeDS = false;
    private boolean mInSession = false;
    private SharedPreferences mSharedPrefs = null;
    private boolean mMultiSessionMode;
    private boolean mGroupCreated;
    private boolean mWifiP2pEnabled;
    private boolean mDiscoveryRequested;
    private int mDecoderLatency;

    private NetManagerIF mNetManager;

    public void clear() {
        mActivity = null;
        mLocalDeviceType = WfdEnums.WFDDeviceType.SOURCE;
        mUIBCM14 = false;
        mModeDS = false;
        mInSession = false;
        mSharedPrefs = null;
        mMultiSessionMode = false;
        mGroupCreated = false;
        mWifiP2pEnabled = false;
        mDiscoveryRequested = false;
        mDecoderLatency = 0;
        if (mNetManager != null) {
            mNetManager.destroy();
            mNetManager = null;
        }
    }

    public boolean isSourceMode() {
        return getLocalDeviceType() == WfdEnums.WFDDeviceType.SOURCE;
    }

    public boolean isMultiSinkMode() {
        return !isSourceMode() && isMultiSessionMode();
    }

    public WFDClientActivity getActivity() {
        return mActivity;
    }

    public void setActivity(WFDClientActivity activity) {
        mActivity = activity;
    }

    public WfdEnums.WFDDeviceType getLocalDeviceType() {
        return mLocalDeviceType;
    }

    public void setLocalDeviceType(WfdEnums.WFDDeviceType localDeviceType) {
        mLocalDeviceType = localDeviceType;
    }

    public boolean isUIBCM14() {
        return mUIBCM14;
    }

    public void setUIBCM14(boolean uibcm14) {
        mUIBCM14 = uibcm14;
    }

    public boolean isModeDS() {
        return mModeDS;
    }

    public void setModeDS(boolean modeDS) {
        mModeDS = modeDS;
    }

    public boolean isInSession() {
        return mInSession;
    }

    public void setInSession(boolean inSession) {
        mInSession = inSession;
    }

    public SharedPreferences getSharedPrefs() {
        return mSharedPrefs;
    }

    public void setSharedPrefs(SharedPreferences sharedPrefs) {
        mSharedPrefs = sharedPrefs;
    }

    public boolean isMultiSessionMode() {
        return mMultiSessionMode;
    }

    public void setMultiSessionMode(boolean multiSessionMode) {
        mMultiSessionMode = multiSessionMode;
    }

    public boolean isGroupCreated() {
        return mGroupCreated;
    }

    public void setGroupCreated(boolean groupCreated) {
        mGroupCreated = groupCreated;
    }

    public boolean isWifiP2pEnabled() {
        return mWifiP2pEnabled;
    }

    public void setWifiP2pEnabled(boolean wifiP2pEnabled) {
        mWifiP2pEnabled = wifiP2pEnabled;
    }

    public boolean isDiscoveryRequested() {
        return mDiscoveryRequested;
    }

    public void setDiscoveryRequested(boolean discoveryRequested) {
        mDiscoveryRequested = discoveryRequested;
    }

    public int getDecoderLatency() {
        return mDecoderLatency;
    }

    public void setDecoderLatency(int decoderLatency) {
        mDecoderLatency = decoderLatency;
    }

    public NetManagerIF getNetManager() {
        return mNetManager;
    }

    public void setNetManager(NetManagerIF netManager) {
        mNetManager = netManager;
    }
}
