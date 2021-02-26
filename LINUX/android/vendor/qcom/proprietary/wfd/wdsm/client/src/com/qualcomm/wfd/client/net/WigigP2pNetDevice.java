/*
* Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

package com.qualcomm.wfd.client.net;

import android.net.wifi.WpsInfo;
import android.net.wifi.p2p.WifiP2pConfig;
import android.net.wifi.p2p.WifiP2pDevice;
import android.net.wifi.p2p.WifiP2pWfdInfo;
import android.util.Log;
import android.widget.Toast;

import com.qualcomm.qti.wigig.p2p.WigigP2pManager;
import com.qualcomm.wfd.WfdDevice;
import com.qualcomm.wfd.WfdEnums;
import com.qualcomm.wfd.WfdEnums.NetType;
import com.qualcomm.wfd.client.Whisperer;

public class WigigP2pNetDevice implements NetDeviceIF {
    static final String TAG = "WigigP2pNetDevice";

    private WifiP2pDevice mDevice;
    //WifiP2pInfo mWifiP2pInfo;

    private WifiP2pConfig mConfig;

    WigigP2pNetDevice(WifiP2pDevice device) {
        mDevice = device;
    }

    WifiP2pDevice getDevice() {
        return mDevice;
    }

    void setDevice(WifiP2pDevice device) {
        if (device == null) {
            throw new IllegalArgumentException("device cannot be null");
        }
        mDevice = device;
    }

    @Override
    public NetType getNetType() {
        return NetType.WIGIG_P2P;
    }

    @Override
    public WigigP2pNetManager getNetTypeManager() {
        return WigigP2pNetManager.sInstance;
    }

    @Override
    public void setConfig(WifiP2pConfig config) {
        mConfig = config;
    }

    @Override
    public void connect(final ResultListener initListener) {
        Log.v(TAG, "connect");
        Whisperer whisperer = Whisperer.getInstance();
        if (mConfig == null) {
            mConfig = new WifiP2pConfig();
            mConfig.deviceAddress = mDevice.deviceAddress;
            mConfig.wps.setup = WpsInfo.PBC;
            if (whisperer.getSharedPrefs().getBoolean("perform_custom_group_owner_intent_value",
                    false) == true) {
                mConfig.groupOwnerIntent = Integer.parseInt(whisperer.getSharedPrefs().getString(
                        "group_owner_intent_value", "3"));
            }
            Toast.makeText(whisperer.getActivity(), "group owner intent value: "
                    + mConfig.groupOwnerIntent, Toast.LENGTH_LONG);
            Log.d(TAG, "group owner intent value: " + mConfig.groupOwnerIntent);

        } else {
            mConfig.deviceAddress = mDevice.deviceAddress;
        }

        getNetTypeManager().manager.connect(getNetTypeManager().channel, mConfig, new WigigP2pManager.ActionListener() {
            @Override
            public void onSuccess() {
                initListener.onSuccess(null);
            }

            @Override
            public void onFailure(int i) {
                initListener.onFailure(i);
            }
        });
    }

    @Override
    public void disconnect(final ResultListener rl) {
        Log.v(TAG, "disconnect");
        getNetTypeManager().disconnect(rl);
    }

    @Override
    public boolean isStatusConnected() {
        return mDevice.status == WifiP2pDevice.CONNECTED;
    }

    @Override
    public boolean isStatusFailed() {
        return mDevice.status == WifiP2pDevice.FAILED;
    }

    @Override
    public String getStatusString() {
        Log.d(TAG, "getDeviceStatus:" + mDevice.status);
        switch (mDevice.status) {
            case WifiP2pDevice.AVAILABLE:
                return "Available";
            case WifiP2pDevice.INVITED:
                return "Invited";
            case WifiP2pDevice.CONNECTED:
                return "Connected";
            case WifiP2pDevice.FAILED:
                return "Failed";
            case WifiP2pDevice.UNAVAILABLE:
                return "Unavailable";
            default:
                return "Unknown";
        }
    }

    @Override
    public String getWfdInfoString() {
        return ((mDevice.wfdInfo != null)?
                (new P2pWfdDeviceInfo(mDevice.wfdInfo.getDeviceInfoHex()).toString())
                : "");
    }

    private boolean isLocalDevice() {
        return mDevice != null && mDevice.deviceAddress != null &&
                mDevice.deviceAddress.equals(getNetTypeManager().getLocalDevice().getAddress());
    }

    @Override
    public String getIp() {
        if (isLocalDevice()) {
            // return null for local device
            Log.w(TAG, "getIp for local device, return null");
            return null;
        }
        ConnectionInfoIF localInfo = getNetTypeManager().getLocalConnectionInfo();
        if (localInfo != null && !localInfo.isGroupOwner()) {
            Log.v(TAG, "get ip for group owner");
            return localInfo.getGroupOwnerIp();
        }
        Log.v(TAG, "get ip from arp");
        return WigigP2pNetManager.getPeerIP(mDevice.deviceAddress);
    }

    @Override
    public WfdDevice convertToWfdDevice() {
        return getNetTypeManager().convertToWfdDevice(this);
    }

    @Override
    public String getName() {
        return mDevice.deviceName;
    }

    @Override
    public String getAddress() {
        return mDevice.deviceAddress;
    }

    @Override
    public boolean supportWfd() {
        return mDevice != null && mDevice.wfdInfo != null;
    }

    @Override
    public WfdEnums.WFDDeviceType getWfdType() {
        if (mDevice == null || mDevice.wfdInfo == null) {
            return WfdEnums.WFDDeviceType.UNKNOWN;
        }
        int type = mDevice.wfdInfo.getDeviceType();
        switch (type) {
            case WifiP2pWfdInfo.WFD_SOURCE: return WfdEnums.WFDDeviceType.SOURCE;
            case WifiP2pWfdInfo.PRIMARY_SINK: return WfdEnums.WFDDeviceType.PRIMARY_SINK;
            case WifiP2pWfdInfo.SECONDARY_SINK: return WfdEnums.WFDDeviceType.SECONDARY_SINK;
            case WifiP2pWfdInfo.SOURCE_OR_PRIMARY_SINK: return WfdEnums.WFDDeviceType.SOURCE_PRIMARY_SINK;
        }
        return WfdEnums.WFDDeviceType.UNKNOWN;
    }

    @Override
    public boolean hasDeviceInfo() {
        return mDevice != null;
    }

    @Override
    public boolean isWfdSessionAvailable() {
        return mDevice.wfdInfo != null && mDevice.wfdInfo.isSessionAvailable();
    }

    @Override
    public String toString() {
        return "WigigP2pNetDevice: " + mDevice.toString();
    }
}
