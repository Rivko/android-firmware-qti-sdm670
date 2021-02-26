/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

package com.qualcomm.wfd.client.net;

import android.net.wifi.p2p.WifiP2pInfo;

class WifiWigigP2pConnectionInfo implements ConnectionInfoIF {

    private WifiP2pInfo mWifiP2pInfo;

    public WifiWigigP2pConnectionInfo(WifiP2pInfo wifiP2pInfo) {
        mWifiP2pInfo = wifiP2pInfo;
    }

    @Override
    public boolean isGroupOwner() {
        return mWifiP2pInfo.isGroupOwner;
    }

    @Override
    public String getGroupOwnerIp() {
        return mWifiP2pInfo.groupOwnerAddress.getHostAddress();
    }
}
