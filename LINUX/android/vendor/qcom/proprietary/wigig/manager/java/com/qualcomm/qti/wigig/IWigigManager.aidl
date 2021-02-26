/**
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 *
 * Copyright (c) 2008, The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.qualcomm.qti.wigig;

import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiInfo;
import java.util.List;
import com.qualcomm.qti.wigig.ScanResult;

import android.os.Messenger;

/**
 * Interface that allows controlling and querying Wigig connectivity.
 *
 * {@hide}
 */
interface IWigigManager
{
    List<WifiConfiguration> getConfiguredNetworks();
    void startScan();
    List<ScanResult> getScanResults();
    void disconnect();
    WifiInfo getConnectionInfo();
    boolean setWigigEnabled(boolean enable);
    boolean isWigigEnabled();
    void setWigigApEnabled(in WifiConfiguration wifiConfig, boolean enable);
    int getWigigApEnabledState();
    WifiConfiguration getWigigApConfiguration();
    void setWigigApConfiguration(in WifiConfiguration wifiConfig);
    Messenger getWigigServiceMessenger();
    String getConfigFile();
    boolean setRateUpgradeEnabled(boolean enable);
    boolean isRateUpgradeEnabled();
}
