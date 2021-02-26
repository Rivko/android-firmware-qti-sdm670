/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

package com.qualcomm.wfd.client.net;

import android.net.nsd.NsdServiceInfo;
import android.net.wifi.p2p.WifiP2pConfig;
import android.util.Log;

import com.qualcomm.wfd.WfdDevice;
import com.qualcomm.wfd.WfdEnums;
import com.qualcomm.wfd.WfdEnums.NetType;
import com.qualcomm.wfd.WfdEnums.WFDDeviceType;

import java.net.InetAddress;

public class LanDevice implements NetDeviceIF {
    static final String TAG = "LanDevice";

    private NsdServiceInfo mNsdInfo;
    private boolean mIsConnected;
    private final boolean mIsLocalDevice;

    LanDevice(NsdServiceInfo nsdInfo) {
        mNsdInfo = nsdInfo;
        mIsLocalDevice = false;
    }

    // constructor for local device
    LanDevice() {
        mIsLocalDevice = true;
    }

    void updateNsdInfo(NsdServiceInfo nsdInfo) {
        assert (nsdInfo != null);
        mNsdInfo = nsdInfo;
    }

    NsdServiceInfo getNsdInfo() {
        return mNsdInfo;
    }

    @Override
    public NetType getNetType() {
        return NetType.LAN;
    }

    @Override
    public LanManager getNetTypeManager() {
        return LanManager.sInstance;
    }

    @Override
    public void setConfig(WifiP2pConfig config) {
        // nothing, not used in LAN
    }

    @Override
    public void connect(ResultListener rl) {
        mIsConnected = true;
        getNetTypeManager().connect(this, rl);
    }

    @Override
    public void disconnect(ResultListener rl) {
        mIsConnected = false;
        rl.onSuccess(0);
    }

    @Override
    public boolean isStatusConnected() {
        return mNsdInfo != null && mNsdInfo.getHost() != null && mIsConnected;
    }

    @Override
    public boolean isStatusFailed() {
        return false;
    }

    @Override
    public String getStatusString() {
        return "Available";
    }

    @Override
    public String getWfdInfoString() {
        return null;
    }

    InetAddress getInetAddress() {
        return mNsdInfo.getHost();
    }

    @Override
    public String getIp() {
        InetAddress addr = mNsdInfo.getHost();
        return addr == null? null : addr.getHostAddress();
    }

    @Override
    public WfdDevice convertToWfdDevice() {
        if (mNsdInfo == null || mNsdInfo.getHost() == null) {
            return null;
        }

        WfdDevice wfdDevice = new WfdDevice();
        wfdDevice.deviceType = getWfdType().ordinal();
        wfdDevice.netType = getNetType().ordinal();
        wfdDevice.macAddress = null;
        wfdDevice.deviceName = getName();
        wfdDevice.ipAddress = getIp();

        //wfdDevice.rtspPort = wifiP2pDevice.wfdInfo.getControlPort();
        wfdDevice.rtspPort = mNsdInfo.getPort(); // TODO
        wfdDevice.isAvailableForSession = true;
        wfdDevice.addressOfAP = null;
        wfdDevice.coupledSinkStatus = 0;
        wfdDevice.preferredConnectivity = 0;

        return wfdDevice;
    }

    @Override
    public String getName() {
        return mNsdInfo.getServiceName();
    }

    @Override
    public String getAddress() {
        // mDNS service info doesn't have mac address info, return ip instead
        return getIp();
    }

    @Override
    public boolean supportWfd() {
        return true;
    }

    @Override
    public WfdEnums.WFDDeviceType getWfdType() {
        return mNsdInfo == null? WfdEnums.WFDDeviceType.UNKNOWN
                : LanManager.getDeviceTypeFromServiceType(mNsdInfo.getServiceType());
    }

    @Override
    public boolean hasDeviceInfo() {
        return mNsdInfo != null;
    }

    public boolean isResolved() {
        return mNsdInfo != null && mNsdInfo.getHost() != null;
    }

    @Override
    public boolean isWfdSessionAvailable() {
        return true;
    }

    @Override
    public String toString() {
        return "LanDevice -> " + mNsdInfo.toString();
    }

    @Override
    public boolean equals(Object other) {
        if (!(other instanceof LanDevice)) {
            return false;
        }
        LanDevice otherDev = (LanDevice)other;

        if (this.getName().equals(otherDev.getName())
                && this.getWfdType() == otherDev.getWfdType()
                && this.getWfdType() != WfdEnums.WFDDeviceType.UNKNOWN) {
            return true;
        }

        return false;
    }

    public boolean sendConnectionRequestIfNecessary() {
        WfdEnums.WFDDeviceType devType = getWfdType();
        Log.v(TAG, "sendConnectionRequestIfNecessary " + this);
        Log.v(TAG, "sendConnectionRequestIfNecessary " + devType);
        if (devType != WfdEnums.WFDDeviceType.PRIMARY_SINK
                && devType != WfdEnums.WFDDeviceType.SECONDARY_SINK) {
            Log.v(TAG, "The remote device is source, unnecessary to send tcp connection request");
            return false;
        }
        if (mNsdInfo == null) {
            return false;
        }

        InetAddress addr = mNsdInfo.getHost();
        int port = mNsdInfo.getPort();
        if (addr == null) {
            Log.e(TAG, "Unsolved NSD Service Info");
            return false;
        }
        TcpCommandClient client = new TcpCommandClient(addr, port);
        client.sendACommand(TcpCommand.newConnectoinRequestCommand(
                P2pWfdDeviceInfo.DEFAULT_SESSION_MGMT_CTRL_PORT));
        return true;
    }

    public String getServiceType() {
        return mNsdInfo.getServiceType();
    }
}
