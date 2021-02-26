/*
* Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

package com.qualcomm.wfd.client.net;

import android.net.wifi.p2p.WifiP2pManager;

import com.qualcomm.wfd.WfdEnums;
import com.qualcomm.wfd.WfdEnums.NetType;

import java.util.Collection;

public interface NetManagerIF {

    // events, arg1 is NetType ordinal
    int WIFI_UTIL_RETRY_CHANNEL = 201;
    int WIFI_UTIL_CHANNAL_LOST_PERMANENTLY = 202;
    int WIFI_UTIL_P2P_STATE_CHANGED = 203;
    int WIFI_UTIL_DISCONNECTED = 204;
    int WIFI_UTIL_CONNECTION_INFO = 205; // obj = WifiP2pInfo
    int WIFI_UTIL_PEERS_CHANGED = 206; // obj = Collection<WifiP2pNetDevice> peers
    int WIFI_UTIL_CONNECTED_PEERS_CHANGED = 207; // obj = Collection<WifiP2pNetDevice> connectedPeers
    int WIFI_UTIL_LOCAL_DEVICE_CHANGED = 208;

    int WIFI_UTIL_CONNECTION_REQUEST = 209;

    // constants
    int MIRACAST_DISABLED = WifiP2pManager.MIRACAST_DISABLED;
    int MIRACAST_SOURCE = WifiP2pManager.MIRACAST_SOURCE;
    int MIRACAST_SINK = WifiP2pManager.MIRACAST_SINK;
    int WIFI_P2P_STATE_ENABLED = WifiP2pManager.WIFI_P2P_STATE_ENABLED;

    void destroy();

    NetType getNetType();

    void onMainActivityResume(); // called in onResume of main activity
    void onMainActivityPause(); // called in onPause of main activity

    void send_wfd_set(boolean isAvailableForSession, WfdEnums.WFDDeviceType type);

    void disconnect(ResultListener rl);

    // TODO: remove this method
    void setMiracastMode(int mode);

    void createGroup(ResultListener rl);

    void removeGroup(ResultListener rl);

    void discoveryPeers(ResultListener rl);

    void stopPeerDiscovery(ResultListener rl);

    void cancelConnect(ResultListener rl);

    boolean isEnabled();

    boolean isConnected();

    NetDeviceIF getLocalDevice();

    Collection<NetDeviceIF> getPeers();

    // TODO: unused method
    Collection getConnectedPeers();

    ConnectionInfoIF getLocalConnectionInfo();
}
