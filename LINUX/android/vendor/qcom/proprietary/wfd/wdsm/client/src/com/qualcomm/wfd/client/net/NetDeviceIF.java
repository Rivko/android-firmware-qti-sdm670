/*
* Copyright (c) 2016 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

package com.qualcomm.wfd.client.net;

import android.net.wifi.p2p.WifiP2pConfig;

import com.qualcomm.wfd.WfdDevice;
import com.qualcomm.wfd.WfdEnums;
import com.qualcomm.wfd.WfdEnums.NetType;

public interface NetDeviceIF {
    NetType getNetType();

    /**
     * Get the net manager of this device
     *
     * @return WifiP2pNetManager or WigigP2pNetManager
     * will not return CombinedNetManager
     */
    NetManagerIF getNetTypeManager();

    void setConfig(WifiP2pConfig config);

    void connect(ResultListener initListener);

    void disconnect(ResultListener rl);

    boolean isStatusConnected();

    boolean isStatusFailed();

    String getStatusString();

    String getWfdInfoString();

    String getIp();

    WfdDevice convertToWfdDevice();

    String getName();

    String getAddress();

    boolean supportWfd();

    WfdEnums.WFDDeviceType getWfdType();

    boolean hasDeviceInfo();

    //boolean hasConnectionInfo();

    boolean isWfdSessionAvailable();
}
