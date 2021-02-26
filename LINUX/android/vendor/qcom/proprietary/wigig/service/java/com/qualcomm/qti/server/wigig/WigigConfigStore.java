/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 *
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.qualcomm.qti.server.wigig;

import android.app.AppGlobals;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.IpConfiguration;
import android.net.IpConfiguration.IpAssignment;
import android.net.IpConfiguration.ProxySettings;
import android.net.NetworkInfo.DetailedState;
import android.net.ProxyInfo;
import android.net.StaticIpConfiguration;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiConfiguration.NetworkSelectionStatus;
import android.net.wifi.WifiConfiguration.Status;
import android.net.wifi.WifiSsid;
import android.net.wifi.WpsInfo;
import android.net.wifi.WpsResult;
import com.qualcomm.qti.wigig.WigigManager;
import android.os.Environment;
import android.os.RemoteException;
import android.os.UserHandle;
import android.text.TextUtils;
import android.util.Log;
import android.util.SparseArray;

import com.android.server.net.IpConfigStore;

import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.BitSet;
import java.util.Calendar;
import java.util.List;
import java.util.Map;
import java.util.Objects;

import static android.net.wifi.WifiConfiguration.INVALID_NETWORK_ID;

/**
 * This class provides the API to manage configured
 * wigig networks. The API is not thread safe and is being
 * used only from WigigStateMachine.
 *
 * It deals with the following
 * - Add/update/remove a WifiConfiguration
 *   The configuration contains two types of information.
 *     = IP and proxy configuration that is handled by WigigConfigStore and
 *       is saved to disk on any change.
 *
 *       The format of configuration file is as follows:
 *       <version>
 *       <netA_key1><netA_value1><netA_key2><netA_value2>...<EOS>
 *       <netB_key1><netB_value1><netB_key2><netB_value2>...<EOS>
 *       ..
 *
 *       (key, value) pairs for a given network are grouped together and can
 *       be in any order. A EOS at the end of a set of (key, value) pairs
 *       indicates that the next set of (key, value) pairs are for a new
 *       network. A network is identified by a unique ID_KEY. If there is no
 *       ID_KEY in the (key, value) pairs, the data is discarded.
 *
 *       An invalid version on read would result in discarding the contents of
 *       the file. On the next write, the latest version is written to file.
 *
 *       Any failures during read or write to the configuration file are ignored
 *       without reporting to the user since the likelihood of these errors are
 *       low and the impact on connectivity is low.
 *
 *     = SSID & security details that is pushed to the supplicant.
 *       supplicant saves these details to the disk on calling
 *       saveConfigCommand().
 *
 *       We have two kinds of APIs exposed:
 *        > public API calls that provide fine grained control
 *          - enableNetwork, disableNetwork, addOrUpdateNetwork(),
 *          removeNetwork(). For these calls, the config is not persisted
 *          to the disk. (TODO: deprecate these calls in WigigManager)
 *        > The new API calls - selectNetwork(), saveNetwork() & forgetNetwork().
 *          These calls persist the supplicant config to disk.
 *
 * - Maintain a list of configured networks for quick access
 *
 */
public class WigigConfigStore extends IpConfigStore {
    private Context mContext;
    public static final String TAG = "WigigConfigStore";
    private static final boolean DBG = true;

    /* configured networks with network id as the key */
    private final ConfigurationMap mConfiguredNetworks = new ConfigurationMap();

    /* Tracks the highest priority of configured networks */
    private int mLastPriority = -1;

    private static final String ipConfigFile = Environment.getDataDirectory() +
            "/misc/wifi/wigig_ipconfig.txt";

    boolean showNetworks = true; // TODO set this back to false, used for debugging 17516271

    private WigigNative mWigigNative;
    /**
     * The lastSelectedConfiguration is used to remember which network
     * was selected last by the user.
     * The connection to this network may not be successful, as well
     * the selection (i.e. network priority) might not be persisted.
     * WiFi state machine is the only object that sets this variable.
     */
    private String lastSelectedConfiguration = null;

    private WigigStateMachine mWigigStateMachine;

    public WigigConfigStore(Context c,  WigigStateMachine w, WigigNative wn) {
        mContext = c;
        mWigigNative = wn;
        mWigigStateMachine = w;
    }

    /**
     * Fetch the list of configured networks
     * and enable all stored networks in supplicant.
     */
    void loadAndEnableAllNetworks() {
        if (DBG) Log.i(TAG, "Loading config and enabling all networks");
        loadConfiguredNetworks();
        enableAllNetworks();
    }

    private List<WifiConfiguration>
    getConfiguredNetworks(Map<String, String> pskMap) {
        List<WifiConfiguration> networks = new ArrayList<>();
        for(WifiConfiguration config : mConfiguredNetworks.values()) {
            WifiConfiguration newConfig = new WifiConfiguration(config);

            if (pskMap != null && config.allowedKeyManagement != null
                    && config.allowedKeyManagement.get(WifiConfiguration.KeyMgmt.WPA_PSK)
                    && pskMap.containsKey(config.SSID)) {
                newConfig.preSharedKey = pskMap.get(config.SSID);
            }
            networks.add(newConfig);
        }

        if (DBG) Log.i(TAG, "getConfiguredNetworks: return " + networks.size() + " networks");
        return networks;
    }

    /**
     * Fetch the list of currently configured networks
     * @return List of networks
     */
    List<WifiConfiguration> getConfiguredNetworks() {
        return getConfiguredNetworks(null);
    }

    /**
     * get the Wificonfiguration for this netId
     *
     * @return Wificonfiguration
     */
    WifiConfiguration getWifiConfiguration(int netId) {
        return mConfiguredNetworks.get(netId);
    }

    /**
     * Get the Wificonfiguration for this key
     * @return Wificonfiguration
     */
    WifiConfiguration getWifiConfiguration(String key) {
        return mConfiguredNetworks.getByConfigKey(key);
    }

    /**
     * Enable all networks and save config. This will be a no-op if the list
     * of configured networks indicates all networks as being enabled
     */
    void enableAllNetworks() {
        long now = System.currentTimeMillis();
        boolean networkEnabledStateChanged = false;

        for(WifiConfiguration config : mConfiguredNetworks.values()) {
            if(config != null && config.status == Status.DISABLED) {
                if(mWigigNative.enableNetwork(config.networkId, false)) {
                    networkEnabledStateChanged = true;
                    config.status = Status.ENABLED;
                    NetworkSelectionStatus nstatus = config.getNetworkSelectionStatus();
                    nstatus.setNetworkSelectionStatus(
                        NetworkSelectionStatus.NETWORK_SELECTION_ENABLED);
                } else {
                    Log.e(TAG, "Enable network failed on " + config.networkId);
                }
            }
        }

        if (networkEnabledStateChanged) {
            mWigigNative.saveConfig();
            sendConfiguredNetworksChangedBroadcast();
        }
    }

    private boolean setNetworkPriorityNative(int netId, int priority) {
        return mWigigNative.setNetworkVariable(netId,
                WifiConfiguration.priorityVarName, Integer.toString(priority));
    }

    public boolean updateLastConnectUid(WifiConfiguration config, int uid) {
        if (config != null) {
            if (config.lastConnectUid != uid) {
                config.lastConnectUid = uid;
                return true;
            }
        }
        return false;
    }

    /**
     * Selects the specified network for connection. This involves
     * updating the priority of all the networks and enabling the given
     * network while disabling others.
     *
     * Selecting a network will leave the other networks disabled and
     * a call to enableAllNetworks() needs to be issued upon a connection
     * or a failure event from supplicant
     *
     * @param config network to select for connection
     * @param updatePriorities makes config highest priority network
     * @return false if the network id is invalid
     */
    boolean selectNetwork(WifiConfiguration config, boolean updatePriorities, int uid) {
        if (DBG) Log.i(TAG, "selectNetwork netId=" + config.networkId);
        if (config.networkId == INVALID_NETWORK_ID) return false;

        // Reset the priority of each network at start or if it goes too high.
        if (mLastPriority == -1 || mLastPriority > 1000000) {
            for(WifiConfiguration config2 : mConfiguredNetworks.values()) {
                if (updatePriorities) {
                    if (config2.networkId != INVALID_NETWORK_ID) {
                        config2.priority = 0;
                        setNetworkPriorityNative(config2.networkId, config.priority);
                    }
                }
            }
            mLastPriority = 0;
        }

        // Set to the highest priority and save the configuration.
        if (updatePriorities) {
            config.priority = ++mLastPriority;
            setNetworkPriorityNative(config.networkId, config.priority);
        }

        if (updatePriorities)
            mWigigNative.saveConfig();
        else
            mWigigNative.selectNetwork(config.networkId);

        updateLastConnectUid(config, uid);

        /* Enable the given network while disabling all other networks */
        enableNetworkWithoutBroadcast(config.networkId, true);

       /* Avoid saving the config & sending a broadcast to prevent settings
        * from displaying a disabled list of networks */
        return true;
    }

    /**
     * Add/update the specified configuration and save config
     *
     * @param config WifiConfiguration to be saved
     * @return network update result
     */
    NetworkUpdateResult saveNetwork(WifiConfiguration config, int uid) {
        WifiConfiguration conf;

        // A new network cannot have null SSID
        if (config == null || (config.networkId == INVALID_NETWORK_ID &&
                config.SSID == null)) {
            return new NetworkUpdateResult(INVALID_NETWORK_ID);
        }
        if (DBG) {
            Log.i(TAG, "saveNetwork, netId=" + config.networkId
                    + " size=" + mConfiguredNetworks.size()
                    + " SSID=" + config.SSID
                    + " Uid=" + Integer.toString(config.creatorUid)
                    + "/" + Integer.toString(config.lastUpdateUid));
        }

        boolean newNetwork = (config.networkId == INVALID_NETWORK_ID);
        NetworkUpdateResult result = addOrUpdateNetworkNative(config, uid);
        int netId = result.getNetworkId();

        if (DBG) Log.d(TAG, "saveNetwork got it back netId=" + netId);

        /* enable a new network */
        if (newNetwork && netId != INVALID_NETWORK_ID) {
            if (DBG) Log.i(TAG, "will enable netId=" + netId);

            mWigigNative.enableNetwork(netId, false);
            conf = mConfiguredNetworks.get(netId);
            if (conf != null) {
                conf.status = Status.ENABLED;
                NetworkSelectionStatus nstatus = config.getNetworkSelectionStatus();
                nstatus.setNetworkSelectionStatus(
                    NetworkSelectionStatus.NETWORK_SELECTION_ENABLED);
            }
        }

        conf = mConfiguredNetworks.get(netId);

        mWigigNative.saveConfig();
        sendConfiguredNetworksChangedBroadcast(conf, result.isNewNetwork() ?
                WigigManager.CHANGE_REASON_ADDED : WigigManager.CHANGE_REASON_CONFIG_CHANGE);
        return result;
    }

    void updateStatus(int netId, DetailedState state) {
        if (netId != INVALID_NETWORK_ID) {
            WifiConfiguration config = mConfiguredNetworks.get(netId);
            if (config == null) return;
            switch (state) {
                case CONNECTED:
                    config.status = Status.CURRENT;
                    break;
                case DISCONNECTED:
                    //If network is already disabled, keep the status
                    if (config.status == Status.CURRENT) {
                        config.status = Status.ENABLED;
                        NetworkSelectionStatus nstatus = config.getNetworkSelectionStatus();
                        nstatus.setNetworkSelectionStatus(
                            NetworkSelectionStatus.NETWORK_SELECTION_ENABLED);
                    }
                    break;
                default:
                    //do nothing, retain the existing state
                    break;
            }
        }
    }

    /**
     * Forget the specified network and save config
     *
     * @param netId network to forget
     * @return {@code true} if it succeeds, {@code false} otherwise
     */
    boolean forgetNetwork(int netId) {
        if (showNetworks) Log.i(TAG, "forgetNetwork, netId=" + netId);

        WifiConfiguration config = mConfiguredNetworks.get(netId);
        boolean remove = removeConfigAndSendBroadcastIfNeeded(netId);
        if (!remove) {
            //success but we dont want to remove the network from supplicant conf file
            return true;
        }
        if (mWigigNative.removeNetwork(netId)) {
            mWigigNative.saveConfig();
            return true;
        } else {
            Log.e(TAG, "Failed to remove network " + netId);
            return false;
        }
    }

    private boolean removeConfigAndSendBroadcastIfNeeded(int netId) {
        WifiConfiguration config = mConfiguredNetworks.get(netId);
        if (config != null) {
            if (DBG) {
                Log.e(TAG, "removeNetwork " + Integer.toString(netId) + " key=" +
                        config.configKey() + " config.id=" + Integer.toString(config.networkId));
            }

            // cancel the last user choice
            if (config.configKey().equals(lastSelectedConfiguration)) {
                lastSelectedConfiguration = null;
            }

            mConfiguredNetworks.remove(netId);

            writeIpAndProxyConfigurations();
            sendConfiguredNetworksChangedBroadcast(config, WigigManager.CHANGE_REASON_REMOVED);
        }
        return true;
    }

    boolean enableNetworkWithoutBroadcast(int netId, boolean disableOthers) {
        boolean ret = mWigigNative.enableNetwork(netId, disableOthers);

        WifiConfiguration config = mConfiguredNetworks.get(netId);
        if (config != null) {
            config.status = Status.ENABLED;
            NetworkSelectionStatus nstatus = config.getNetworkSelectionStatus();
            nstatus.setNetworkSelectionStatus(
                NetworkSelectionStatus.NETWORK_SELECTION_ENABLED);
        }

        if (disableOthers) {
            markAllNetworksDisabledExcept(netId);
        }
        return ret;
    }

    void disableAllNetworks() {
        if (DBG) Log.i(TAG, "disableAllNetworks");
        boolean networkDisabled = false;
        for (WifiConfiguration enabled : mConfiguredNetworks.getEnabledNetworks()) {
            if(mWigigNative.disableNetwork(enabled.networkId)) {
                networkDisabled = true;
                enabled.status = Status.DISABLED;
            } else {
                loge("Disable network failed on " + enabled.networkId);
            }
        }

        if (networkDisabled) {
            sendConfiguredNetworksChangedBroadcast();
        }
    }

    /**
     * Start WPS pin method configuration with pin obtained
     * from the access point
     * @param config WPS configuration
     * @return Wps result containing status and pin
     */
    WpsResult startWpsWithPinFromAccessPoint(WpsInfo config) {
        WpsResult result = new WpsResult();
        if (mWigigNative.startWpsRegistrar(config.BSSID, config.pin)) {
            /* WPS leaves all networks disabled */
            markAllNetworksDisabled();
            result.status = WpsResult.Status.SUCCESS;
        } else {
            loge("Failed to start WPS pin method configuration");
            result.status = WpsResult.Status.FAILURE;
        }
        return result;
    }

    /**
     * Start WPS pin method configuration with pin obtained
     * from the device
     * @param config WPS configuration
     * @return WpsResult indicating status and pin
     */
    WpsResult startWpsWithPinFromDevice(WpsInfo config) {
        WpsResult result = new WpsResult();
        result.pin = mWigigNative.startWpsPinDisplay(config.BSSID);
        /* WPS leaves all networks disabled */
        if (!TextUtils.isEmpty(result.pin)) {
            markAllNetworksDisabled();
            result.status = WpsResult.Status.SUCCESS;
        } else {
            loge("Failed to start WPS pin display method configuration");
            result.status = WpsResult.Status.FAILURE;
        }
        return result;
    }

    /**
     * Start WPS push button configuration
     * @param config WPS configuration
     * @return WpsResult indicating status and pin
     */
    WpsResult startWpsPbc(WpsInfo config) {
        WpsResult result = new WpsResult();
        if (mWigigNative.startWpsPbc(config.BSSID)) {
            /* WPS leaves all networks disabled */
            markAllNetworksDisabled();
            result.status = WpsResult.Status.SUCCESS;
        } else {
            loge("Failed to start WPS push button configuration");
            result.status = WpsResult.Status.FAILURE;
        }
        return result;
    }

    /**
     * Start WPS pin method configuration with pin obtained
     * from a label on the access point
     * there is some confusion here with the names, the
     * WigigNative method refers to this method as "keypad"
     * as the pin from the label is typed using a keypad.
     * @param config WPS configuration
     * @return WpsResult indicating status and pin
     */
    WpsResult startWpsWithPinFromLabel(WpsInfo config) {
        WpsResult result = new WpsResult();
        if (mWigigNative.startWpsPinKeypad(config.pin)) {
            /* WPS leaves all networks disabled */
            markAllNetworksDisabled();
            result.status = WpsResult.Status.SUCCESS;
        } else {
            loge("Failed to start WPS label configuration");
            result.status = WpsResult.Status.FAILURE;
        }
        return result;
    }

    /**
     * Mark a network as enabled without letting supplicant know about the status change
     *
     * @param config network to change
     */
    private void markNetworkEnabled(WifiConfiguration config) {
        if (DBG) Log.i(TAG, "mark network enabled");

        if (config.status == Status.DISABLED) {
            config.status = Status.ENABLED;
            NetworkSelectionStatus nstatus = config.getNetworkSelectionStatus();
            nstatus.setNetworkSelectionStatus(
                NetworkSelectionStatus.NETWORK_SELECTION_ENABLED);
            sendConfiguredNetworksChangedBroadcast(config,
                    WigigManager.CHANGE_REASON_CONFIG_CHANGE);
        }
    }

    /**
     * Mark a network as disabled without letting supplicant know about the status change
     *
     * @param config network to change
     * @param reason reason code network was disabled
     */
    private void markNetworkDisabled(WifiConfiguration config, int reason) {
        if (DBG) Log.i(TAG, "mark network disabled");

        if (config.status != Status.DISABLED) {
            config.status = Status.DISABLED;
            NetworkSelectionStatus nstatus = config.getNetworkSelectionStatus();
            nstatus.setNetworkSelectionStatus(
                NetworkSelectionStatus.NETWORK_SELECTION_TEMPORARY_DISABLED);
            nstatus.setNetworkSelectionDisableReason(reason);
            sendConfiguredNetworksChangedBroadcast(config,
                    WigigManager.CHANGE_REASON_CONFIG_CHANGE);
        }
    }

    /**
     * Fetch the static IP configuration for a given network id
     */
    StaticIpConfiguration getStaticIpConfiguration(int netId) {
        WifiConfiguration config = mConfiguredNetworks.get(netId);
        if (config != null) {
            return config.getStaticIpConfiguration();
        }
        return null;
    }

    /**
     * Fetch the proxy properties for a given network id
     * @param netId id
     * @return ProxyInfo for the network id
     */
    ProxyInfo getProxyProperties(int netId) {
        WifiConfiguration config = mConfiguredNetworks.get(netId);
        if (config != null) {
            return config.getHttpProxy();
        }
        return null;
    }

    /**
     * Return if the specified network is using static IP
     * @param netId id
     * @return {@code true} if using static ip for netId
     */
    boolean isUsingStaticIp(int netId) {
        WifiConfiguration config = mConfiguredNetworks.get(netId);
        if (config != null && config.getIpAssignment() == IpAssignment.STATIC) {
            return true;
        }
        return false;
    }

    /**
     * Should be called when a single network configuration is made.
     *
     * @param network The network configuration that changed.
     * @param reason  The reason for the change, should be one of WigigManager.CHANGE_REASON_ADDED,
     *                WigigManager.CHANGE_REASON_REMOVED, or WigigManager.CHANGE_REASON_CONFIG_CHANGE.
     */
    private void sendConfiguredNetworksChangedBroadcast(WifiConfiguration network,
                                                        int reason) {
        Intent intent = new Intent(WigigManager.CONFIGURED_NETWORKS_CHANGED_ACTION);
        intent.addFlags(Intent.FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
        intent.putExtra(WigigManager.EXTRA_MULTIPLE_NETWORKS_CHANGED, false);
        intent.putExtra(WigigManager.EXTRA_WIFI_CONFIGURATION, network);
        intent.putExtra(WigigManager.EXTRA_CHANGE_REASON, reason);
        // network might be null, be careful with logging
        Log.i(TAG, "sending CONFIGURED_NETWORKS_CHANGED_ACTION, nid=" +
              ((network != null) ? network.networkId : INVALID_NETWORK_ID) +
               " reason=" + reason);
        mContext.sendBroadcastAsUser(intent, UserHandle.ALL);
    }

    /**
     * Should be called when multiple network configuration changes are made.
     */
    private void sendConfiguredNetworksChangedBroadcast() {
        Intent intent = new Intent(WigigManager.CONFIGURED_NETWORKS_CHANGED_ACTION);
        intent.addFlags(Intent.FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
        intent.putExtra(WigigManager.EXTRA_MULTIPLE_NETWORKS_CHANGED, true);
        Log.i(TAG, "sending CONFIGURED_NETWORKS_CHANGED_ACTION");
        mContext.sendBroadcastAsUser(intent, UserHandle.ALL);
    }

    void loadConfiguredNetworks() {

        mLastPriority = 0;

        mConfiguredNetworks.clear();
        List<WifiConfiguration> configTlsResetList = new ArrayList<WifiConfiguration>();
        int last_id = -1;
        boolean done = false;
        while (!done) {

            String listStr = mWigigNative.listNetworks(last_id);
            if (listStr == null)
                return;

            String[] lines = listStr.split("\n");

            if (showNetworks) {
                Log.d(TAG, "loadConfiguredNetworks:  ");
                for (String net : lines) {
                    Log.d(TAG, net);
                }
            }

            // Skip the first line, which is a header
            for (int i = 1; i < lines.length; i++) {
                String[] result = lines[i].split("\t");
                // network-id | ssid | bssid | flags
                WifiConfiguration config = new WifiConfiguration();
                try {
                    config.networkId = Integer.parseInt(result[0]);
                    last_id = config.networkId;
                } catch(NumberFormatException e) {
                    Log.e(TAG, "Failed to read network-id '" + result[0] + "'");
                    continue;
                }
                if (result.length > 3) {
                    if (result[3].indexOf("[CURRENT]") != -1)
                        config.status = WifiConfiguration.Status.CURRENT;
                    else if (result[3].indexOf("[DISABLED]") != -1) {
                        config.status = WifiConfiguration.Status.DISABLED;
                        NetworkSelectionStatus nstatus = config.getNetworkSelectionStatus();
                        nstatus.setNetworkSelectionStatus(
                            NetworkSelectionStatus.NETWORK_SELECTION_TEMPORARY_DISABLED);
                        nstatus.setNetworkSelectionDisableReason(
                            NetworkSelectionStatus.DISABLED_BY_WIFI_MANAGER);
                    } else
                        config.status = WifiConfiguration.Status.ENABLED;
                        NetworkSelectionStatus nstatus = config.getNetworkSelectionStatus();
                        nstatus.setNetworkSelectionStatus(
                            NetworkSelectionStatus.NETWORK_SELECTION_ENABLED);
                } else {
                    config.status = WifiConfiguration.Status.ENABLED;
                    NetworkSelectionStatus nstatus = config.getNetworkSelectionStatus();
                    nstatus.setNetworkSelectionStatus(
                        NetworkSelectionStatus.NETWORK_SELECTION_ENABLED);
                }

                readNetworkVariables(config);

                if (config.priority > mLastPriority) {
                    mLastPriority = config.priority;
                }

                config.setIpAssignment(IpAssignment.DHCP);
                config.setProxySettings(ProxySettings.NONE);

                if (mConfiguredNetworks.getByConfigKey(config.configKey()) != null) {
                    // That SSID is already known, just ignore this duplicate entry
                    if (showNetworks) Log.i(TAG, "discarded duplicate network " + config.networkId);
                } else if(WigigService.isValid(config)){
                    mConfiguredNetworks.put(config.networkId, config);
                    if (showNetworks) Log.i(TAG, "loaded configured network" + config.networkId);
                } else {
                    if (showNetworks) Log.d(TAG, "Ignoring loaded configured for network " + config.networkId
                        + " because config are not valid");
                }
            }

            done = (lines.length == 1);
        }

        readIpAndProxyConfigurations();

        sendConfiguredNetworksChangedBroadcast();

        if (showNetworks) Log.i(TAG, "loadConfiguredNetworks loaded " + mConfiguredNetworks.size() + " networks");
    }

    /* Mark all networks except specified netId as disabled */
    private void markAllNetworksDisabledExcept(int netId) {
        for(WifiConfiguration config : mConfiguredNetworks.values()) {
            if(config != null && config.networkId != netId) {
                if (config.status != Status.DISABLED) {
                    config.status = Status.DISABLED;
                    NetworkSelectionStatus nstatus = config.getNetworkSelectionStatus();
                    nstatus.setNetworkSelectionStatus(
                        NetworkSelectionStatus.NETWORK_SELECTION_TEMPORARY_DISABLED);
                    nstatus.setNetworkSelectionDisableReason(
                        NetworkSelectionStatus.DISABLED_BY_WIFI_MANAGER);
                }
            }
        }
    }

    private void markAllNetworksDisabled() {
        markAllNetworksDisabledExcept(INVALID_NETWORK_ID);
    }

    public void setLastSelectedConfiguration(int netId) {
        if (DBG) {
            Log.e(TAG, "setLastSelectedConfiguration " + Integer.toString(netId));
        }
        if (netId == WifiConfiguration.INVALID_NETWORK_ID) {
            lastSelectedConfiguration = null;
        } else {
            WifiConfiguration selected = getWifiConfiguration(netId);
            if (selected == null) {
                lastSelectedConfiguration = null;
            } else {
                lastSelectedConfiguration = selected.configKey();
                selected.numNoInternetAccessReports = 0;
                if (DBG) {
                    Log.e(TAG, "setLastSelectedConfiguration now: " + lastSelectedConfiguration);
                }
            }
        }
    }

    private void writeIpAndProxyConfigurations() {
        final SparseArray<IpConfiguration> networks = new SparseArray<IpConfiguration>();
        for(WifiConfiguration config : mConfiguredNetworks.values()) {
            if (!config.ephemeral) {
                networks.put(configKey(config), config.getIpConfiguration());
            }
        }

        // TODO: enable when IPConfigStore works
        super.writeIpAndProxyConfigurations(ipConfigFile, networks);
    }

    private void readIpAndProxyConfigurations() {
        // TODO: enable when IPConfigStore works
        SparseArray<IpConfiguration> networks = super.readIpAndProxyConfigurations(ipConfigFile);
        //SparseArray<IpConfiguration> networks = null;

        if (networks == null || networks.size() == 0) {
            // super has already logged an error.
            return;
        }

        for (int i = 0; i < networks.size(); i++) {
            int id = networks.keyAt(i);
            WifiConfiguration config = mConfiguredNetworks.getByConfigKeyID(id);
            // This is the only place the map is looked up through a (dangerous) hash-value!

            if (config == null || config.ephemeral) {
                Log.e(TAG, "configuration found for missing network, nid=" + id
                        +", ignored, networks.size=" + Integer.toString(networks.size()));
            } else {
                config.setIpConfiguration(networks.valueAt(i));
            }
        }
    }

    public static String toHex(byte[] octets) {
        final int BYTE_MASK = 0xff;

        StringBuilder sb = new StringBuilder(octets.length * 2);
        for (byte o : octets) {
            sb.append(String.format("%02x", o & BYTE_MASK));
        }
        return sb.toString();
    }

    /*
     * Convert string to Hexadecimal before passing to wigig native layer
     * In native function "doCommand()" have trouble in converting Unicode character string to UTF8
     * conversion to hex is required because SSIDs can have space characters in them;
     * and that can confuses the supplicant because it uses space charaters as delimiters
     */
    public static String encodeSSID(String str){
        return toHex(removeDoubleQuotes(str).getBytes(StandardCharsets.UTF_8));
    }

    private NetworkUpdateResult addOrUpdateNetworkNative(WifiConfiguration config, int uid) {
        /*
         * If the supplied networkId is INVALID_NETWORK_ID, we create a new empty
         * network configuration. Otherwise, the networkId should
         * refer to an existing configuration.
         */

        if (DBG) Log.i(TAG, "addOrUpdateNetworkNative " + config.getPrintableSsid());

        int netId = config.networkId;
        boolean newNetwork = false;
        // networkId of INVALID_NETWORK_ID means we want to create a new network
        if (netId == INVALID_NETWORK_ID) {
            WifiConfiguration savedConfig = mConfiguredNetworks.getByConfigKey(config.configKey());
            if (savedConfig != null) {
                netId = savedConfig.networkId;
            } else {
                newNetwork = true;
                netId = mWigigNative.addNetwork();
                if (netId < 0) {
                    Log.e(TAG, "Failed to add a network!");
                    return new NetworkUpdateResult(INVALID_NETWORK_ID);
                } else {
                    Log.e(TAG, "addOrUpdateNetworkNative created netId=" + netId);
                }
            }
        }

        boolean updateFailed = true;
        WifiConfiguration savedConfig = null;

        setVariables: {
            if (newNetwork == false) {
                savedConfig = mConfiguredNetworks.get(netId);
                if (savedConfig != null) {
                    readNetworkVariables(savedConfig);
                }
            }

            if (!((newNetwork == false) && (savedConfig != null) &&
                   (savedConfig.SSID != null) && (config.SSID != null) &&
                   (savedConfig.SSID.equals(config.SSID))) &&
                   (config.SSID != null &&
                    !mWigigNative.setNetworkVariable(
                        netId,
                        WifiConfiguration.ssidVarName,
                        encodeSSID(config.SSID)))) {
                Log.e(TAG, "failed to set SSID: " + config.SSID);
                break setVariables;
            }

            if (!((newNetwork == false) && (savedConfig != null) &&
                   (savedConfig.BSSID != null) && (config.BSSID != null) &&
                   (savedConfig.BSSID.equals(config.BSSID)))
                   && (config.BSSID != null)) {
                Log.i(TAG, "Setting BSSID for " + config.configKey() + " to " + config.BSSID);
                if (!mWigigNative.setNetworkVariable(
                        netId,
                        WifiConfiguration.bssidVarName,
                        config.BSSID)) {
                    Log.e(TAG, "failed to set BSSID: " + config.BSSID);
                    break setVariables;
                }
            }

            String allowedKeyManagementString =
                makeString(config.allowedKeyManagement, WifiConfiguration.KeyMgmt.strings);
            if (!((newNetwork == false) && (savedConfig != null) &&
                   (allowedKeyManagementString != null) &&
                   (savedConfig.allowedKeyManagement.equals(
                   config.allowedKeyManagement))) &&
                   config.allowedKeyManagement.cardinality() != 0 &&
                   !mWigigNative.setNetworkVariable(
                       netId,
                       WifiConfiguration.KeyMgmt.varName,
                       allowedKeyManagementString)) {
                Log.e(TAG, "failed to set key_mgmt: "+
                        allowedKeyManagementString);
                break setVariables;
            }

            String allowedProtocolsString =
                makeString(config.allowedProtocols, WifiConfiguration.Protocol.strings);
            if (!((newNetwork == false) && (savedConfig != null) &&
                   (allowedProtocolsString != null) &&
                   (savedConfig.allowedProtocols.equals(config.allowedProtocols)))
                   && config.allowedProtocols.cardinality() != 0 &&
                   !mWigigNative.setNetworkVariable(
                        netId,
                        WifiConfiguration.Protocol.varName,
                        allowedProtocolsString)) {
                Log.e(TAG, "failed to set proto: "+
                        allowedProtocolsString);
                break setVariables;
            }

            String allowedAuthAlgorithmsString =
                makeString(config.allowedAuthAlgorithms, WifiConfiguration.AuthAlgorithm.strings);
            if (!((newNetwork == false) && (savedConfig != null)
                  && (allowedAuthAlgorithmsString != null) &&
                  (savedConfig.allowedAuthAlgorithms.equals(
                  config.allowedAuthAlgorithms))) &&
                  config.allowedAuthAlgorithms.cardinality() != 0 &&
                  !mWigigNative.setNetworkVariable(
                      netId,
                      WifiConfiguration.AuthAlgorithm.varName,
                      allowedAuthAlgorithmsString)) {
                Log.e(TAG, "failed to set auth_alg: "+
                        allowedAuthAlgorithmsString);
                break setVariables;
            }

            boolean setPairwiseCipher = true;
            boolean setGroupCipher = true;
            /* PSK in wigig uses GCMP */
            String allowedPairwiseCiphersString = "GCMP";
            String allowedGroupCiphersString = "GCMP";

            if (!config.allowedKeyManagement.get(WifiConfiguration.KeyMgmt.WPA_PSK)) {
                allowedPairwiseCiphersString =
                        makeString(config.allowedPairwiseCiphers,
                                WifiConfiguration.PairwiseCipher.strings);
                setPairwiseCipher = !((newNetwork == false) && (savedConfig != null) &&
                        (allowedPairwiseCiphersString != null) &&
                        (savedConfig.allowedPairwiseCiphers.equals(
                                config.allowedPairwiseCiphers))) &&
                        config.allowedPairwiseCiphers.cardinality() != 0;
                allowedGroupCiphersString =
                        makeString(config.allowedGroupCiphers,
                                WifiConfiguration.GroupCipher.strings);
                setGroupCipher = !((newNetwork == false) && (savedConfig != null) &&
                        (allowedGroupCiphersString != null) &&
                        (savedConfig.allowedGroupCiphers.equals(
                                config.allowedGroupCiphers))) &&
                        config.allowedGroupCiphers.cardinality() != 0;
            }

            if (setPairwiseCipher && !mWigigNative.setNetworkVariable(
                    netId,
                    WifiConfiguration.PairwiseCipher.varName,
                    allowedPairwiseCiphersString)) {
                Log.e(TAG, "failed to set pairwise: " + allowedPairwiseCiphersString);
                break setVariables;
            }

            if (setGroupCipher && !mWigigNative.setNetworkVariable(
                    netId,
                    WifiConfiguration.GroupCipher.varName,
                    allowedGroupCiphersString)) {
                Log.e(TAG, "failed to set group: " + allowedGroupCiphersString);
                break setVariables;
            }

            // Prevent client screw-up by passing in a WifiConfiguration we gave it
            // by preventing "*" as a key.
            if (!((newNetwork == false) && (savedConfig != null) &&
                   (savedConfig.preSharedKey != null) &&
                   (config.preSharedKey != null) &&
                   (savedConfig.preSharedKey.equals(config.preSharedKey)))
                   && config.preSharedKey != null &&
                   !config.preSharedKey.equals("*") &&
                   !mWigigNative.setNetworkVariable(
                        netId,
                        WifiConfiguration.pskVarName,
                        config.preSharedKey)) {
                Log.e(TAG, "failed to set psk");
                break setVariables;
            }

            if (!((newNetwork == false) && (savedConfig != null) &&
                   (config.priority == savedConfig.priority )) &&
                   !mWigigNative.setNetworkVariable(
                        netId,
                        WifiConfiguration.priorityVarName,
                        Integer.toString(config.priority))) {
                Log.e(TAG, config.SSID + ": failed to set priority: "
                        +config.priority);
                break setVariables;
            }

            if (!((newNetwork == false) && (savedConfig != null) &&
                   (savedConfig.hiddenSSID == config.hiddenSSID)) &&
                   config.hiddenSSID && !mWigigNative.setNetworkVariable(
                        netId,
                        WifiConfiguration.hiddenSSIDVarName,
                        Integer.toString(config.hiddenSSID ? 1 : 0))) {
                Log.e(TAG, config.SSID + ": failed to set hiddenSSID: "+
                        config.hiddenSSID);
                break setVariables;
            }

            if (!((newNetwork == false) && (savedConfig != null) &&
                   (savedConfig.requirePMF == config.requirePMF)) &&
                   config.requirePMF && !mWigigNative.setNetworkVariable(
                        netId,
                        WifiConfiguration.pmfVarName,
                        "2")) {
                Log.e(TAG, config.SSID + ": failed to set requirePMF: "+
                        config.requirePMF);
                break setVariables;
            }

            if (!((newNetwork == false) && (savedConfig != null) &&
                   (savedConfig.updateIdentifier != null) &&
                   (config.updateIdentifier != null) &&
                   (savedConfig.updateIdentifier.equals(config.updateIdentifier)))
                   && config.updateIdentifier != null && !mWigigNative.setNetworkVariable(
                    netId,
                    WifiConfiguration.updateIdentiferVarName,
                    config.updateIdentifier)) {
                Log.e(TAG, config.SSID + ": failed to set updateIdentifier: "+
                        config.updateIdentifier);
                break setVariables;
            }

            updateFailed = false;
        } // End of setVariables

        if (updateFailed) {
            if (newNetwork) {
                mWigigNative.removeNetwork(netId);
                Log.e(TAG, "Failed to set a network variable, removed network: " + netId);
            }
            return new NetworkUpdateResult(INVALID_NETWORK_ID);
        }

        /* An update of the network variables requires reading them
         * back from the supplicant to update mConfiguredNetworks.
         * This is because some of the variables (SSID, wep keys &
         * passphrases) reflect different values when read back than
         * when written. For example, wep key is stored as * irrespective
         * of the value sent to the supplicant
         */
        WifiConfiguration currentConfig = mConfiguredNetworks.get(netId);
        if (currentConfig == null) {
            currentConfig = new WifiConfiguration();
            currentConfig.setIpAssignment(IpAssignment.DHCP);
            currentConfig.setProxySettings(ProxySettings.NONE);
            currentConfig.networkId = netId;
            if (config != null) {
                // Carry over the creation parameters
                currentConfig.selfAdded = config.selfAdded;
                currentConfig.didSelfAdd = config.didSelfAdd;
                currentConfig.ephemeral = config.ephemeral;
                currentConfig.lastConnectUid = config.lastConnectUid;
                currentConfig.lastUpdateUid = config.lastUpdateUid;
                currentConfig.creatorUid = config.creatorUid;
                currentConfig.creatorName = config.creatorName;
                currentConfig.lastUpdateName = config.lastUpdateName;
                currentConfig.peerWifiConfiguration = config.peerWifiConfiguration;
                currentConfig.FQDN = config.FQDN;
                currentConfig.providerFriendlyName = config.providerFriendlyName;
                currentConfig.roamingConsortiumIds = config.roamingConsortiumIds;
                currentConfig.validatedInternetAccess = config.validatedInternetAccess;
                currentConfig.numNoInternetAccessReports = config.numNoInternetAccessReports;
                currentConfig.updateTime = config.updateTime;
                currentConfig.creationTime = config.creationTime;
            }
            if (DBG) {
                Log.i(TAG, "created new config netId=" + Integer.toString(netId)
                        + " uid=" + Integer.toString(currentConfig.creatorUid)
                        + " name=" + currentConfig.creatorName);
            }
        }

        if (uid != WifiConfiguration.UNKNOWN_UID) {
            if (newNetwork) {
                currentConfig.creatorUid = uid;
            } else {
                currentConfig.lastUpdateUid = uid;
            }
        }

        // For debug, record the time the configuration was modified
        StringBuilder sb = new StringBuilder();
        sb.append("time=");
        Calendar c = Calendar.getInstance();
        c.setTimeInMillis(System.currentTimeMillis());
        sb.append(String.format("%tm-%td %tH:%tM:%tS.%tL", c, c, c, c, c, c));

        if (newNetwork) {
            currentConfig.creationTime = sb.toString();
        } else {
            currentConfig.updateTime = sb.toString();
        }

        if (DBG) Log.i(TAG, "will read network variables netId=" + Integer.toString(netId));

        readNetworkVariables(currentConfig);

        // Persist configuration paramaters that are not saved by supplicant.
        if (config.lastUpdateName != null) {
            currentConfig.lastUpdateName = config.lastUpdateName;
        }
        if (config.lastUpdateUid != WifiConfiguration.UNKNOWN_UID) {
            currentConfig.lastUpdateUid = config.lastUpdateUid;
        }

        mConfiguredNetworks.put(netId, currentConfig);

        NetworkUpdateResult result = writeIpAndProxyConfigurationsOnChange(currentConfig, config);
        result.setIsNewNetwork(newNetwork);
        result.setNetworkId(netId);

        return result;
    }

    /* Compare current and new configuration and write to file on change */
    private NetworkUpdateResult writeIpAndProxyConfigurationsOnChange(
            WifiConfiguration currentConfig,
            WifiConfiguration newConfig) {
        boolean ipChanged = false;
        boolean proxyChanged = false;

        if (DBG) {
            Log.e(TAG, "writeIpAndProxyConfigurationsOnChange: " + currentConfig.SSID + " -> " +
                    newConfig.SSID + " path: " + ipConfigFile);
        }


        switch (newConfig.getIpAssignment()) {
            case STATIC:
                if (currentConfig.getIpAssignment() != newConfig.getIpAssignment()) {
                    ipChanged = true;
                } else {
                    ipChanged = !Objects.equals(
                            currentConfig.getStaticIpConfiguration(),
                            newConfig.getStaticIpConfiguration());
                }
                break;
            case DHCP:
                if (currentConfig.getIpAssignment() != newConfig.getIpAssignment()) {
                    ipChanged = true;
                }
                break;
            case UNASSIGNED:
                /* Ignore */
                break;
            default:
                Log.e(TAG, "Ignore invalid ip assignment during write");
                break;
        }

        switch (newConfig.getProxySettings()) {
            case STATIC:
            case PAC:
                ProxyInfo newHttpProxy = newConfig.getHttpProxy();
                ProxyInfo currentHttpProxy = currentConfig.getHttpProxy();

                if (newHttpProxy != null) {
                    proxyChanged = !newHttpProxy.equals(currentHttpProxy);
                } else {
                    proxyChanged = (currentHttpProxy != null);
                }
                break;
            case NONE:
                if (currentConfig.getProxySettings() != newConfig.getProxySettings()) {
                    proxyChanged = true;
                }
                break;
            case UNASSIGNED:
                /* Ignore */
                break;
            default:
                Log.e(TAG, "Ignore invalid proxy configuration during write");
                break;
        }

        if (ipChanged) {
            currentConfig.setIpAssignment(newConfig.getIpAssignment());
            currentConfig.setStaticIpConfiguration(newConfig.getStaticIpConfiguration());
            Log.i(TAG, "IP config changed SSID = " + currentConfig.SSID);
            if (currentConfig.getStaticIpConfiguration() != null) {
                Log.i(TAG, " static configuration: " +
                    currentConfig.getStaticIpConfiguration().toString());
            }
        }

        if (proxyChanged) {
            currentConfig.setProxySettings(newConfig.getProxySettings());
            currentConfig.setHttpProxy(newConfig.getHttpProxy());
            Log.i(TAG, "proxy changed SSID = " + currentConfig.SSID);
            if (currentConfig.getHttpProxy() != null) {
                Log.i(TAG, " proxyProperties: " + currentConfig.getHttpProxy().toString());
            }
        }

        if (ipChanged || proxyChanged) {
            writeIpAndProxyConfigurations();
            sendConfiguredNetworksChangedBroadcast(currentConfig,
                    WigigManager.CHANGE_REASON_CONFIG_CHANGE);
        }
        return new NetworkUpdateResult(ipChanged, proxyChanged);
    }

    /**
     * Read the variables from the supplicant daemon that are needed to
     * fill in the WifiConfiguration object.
     *
     * @param config the {@link WifiConfiguration} object to be filled in.
     */
    private void readNetworkVariables(WifiConfiguration config) {

        int netId = config.networkId;
        if (netId < 0)
            return;

        /*
         * TODO: maybe should have a native method that takes an array of
         * variable names and returns an array of values. But we'd still
         * be doing a round trip to the supplicant daemon for each variable.
         */
        String value;

        value = mWigigNative.getNetworkVariable(netId, WifiConfiguration.ssidVarName);
        if (!TextUtils.isEmpty(value)) {
            if (value.charAt(0) != '"') {
                config.SSID = "\"" + WifiSsid.createFromHex(value).toString() + "\"";
                //TODO: convert a hex string that is not UTF-8 decodable to a P-formatted
                //supplicant string
            } else {
                config.SSID = value;
            }
        } else {
            config.SSID = null;
        }

        value = mWigigNative.getNetworkVariable(netId, WifiConfiguration.bssidVarName);
        if (!TextUtils.isEmpty(value)) {
            config.BSSID = value;
        } else {
            config.BSSID = null;
        }

        value = mWigigNative.getNetworkVariable(netId, WifiConfiguration.priorityVarName);
        config.priority = -1;
        if (!TextUtils.isEmpty(value)) {
            try {
                config.priority = Integer.parseInt(value);
            } catch (NumberFormatException ignore) {
            }
        }

        value = mWigigNative.getNetworkVariable(netId, WifiConfiguration.hiddenSSIDVarName);
        config.hiddenSSID = false;
        if (!TextUtils.isEmpty(value)) {
            try {
                config.hiddenSSID = Integer.parseInt(value) != 0;
            } catch (NumberFormatException ignore) {
            }
        }

        value = mWigigNative.getNetworkVariable(netId, WifiConfiguration.wepTxKeyIdxVarName);
        config.wepTxKeyIndex = -1;
        if (!TextUtils.isEmpty(value)) {
            try {
                config.wepTxKeyIndex = Integer.parseInt(value);
            } catch (NumberFormatException ignore) {
            }
        }

        for (int i = 0; i < 4; i++) {
            value = mWigigNative.getNetworkVariable(netId,
                    WifiConfiguration.wepKeyVarNames[i]);
            if (!TextUtils.isEmpty(value)) {
                config.wepKeys[i] = value;
            } else {
                config.wepKeys[i] = null;
            }
        }

        value = mWigigNative.getNetworkVariable(netId, WifiConfiguration.pskVarName);
        if (!TextUtils.isEmpty(value)) {
            config.preSharedKey = value;
        } else {
            config.preSharedKey = null;
        }

        readNetworkBitsetVariable(config.networkId, config.allowedProtocols,
                WifiConfiguration.Protocol.varName, WifiConfiguration.Protocol.strings);

        readNetworkBitsetVariable(config.networkId, config.allowedKeyManagement,
                WifiConfiguration.KeyMgmt.varName, WifiConfiguration.KeyMgmt.strings);

        readNetworkBitsetVariable(config.networkId, config.allowedAuthAlgorithms,
                WifiConfiguration.AuthAlgorithm.varName, WifiConfiguration.AuthAlgorithm.strings);

        readNetworkBitsetVariable(config.networkId, config.allowedPairwiseCiphers,
                WifiConfiguration.PairwiseCipher.varName, WifiConfiguration.PairwiseCipher.strings);

        readNetworkBitsetVariable(config.networkId, config.allowedGroupCiphers,
                WifiConfiguration.GroupCipher.varName, WifiConfiguration.GroupCipher.strings);
    }

    private static String removeDoubleQuotes(String string) {
        int length = string.length();
        if ((length > 1) && (string.charAt(0) == '"')
                && (string.charAt(length - 1) == '"')) {
            return string.substring(1, length - 1);
        }
        return string;
    }

    private static String makeString(BitSet set, String[] strings) {
        StringBuffer buf = new StringBuffer();
        int nextSetBit = -1;

        /* Make sure all set bits are in [0, strings.length) to avoid
         * going out of bounds on strings.  (Shouldn't happen, but...) */
        set = set.get(0, strings.length);

        while ((nextSetBit = set.nextSetBit(nextSetBit + 1)) != -1) {
            buf.append(strings[nextSetBit].replace('_', '-')).append(' ');
        }

        // remove trailing space
        if (set.cardinality() > 0) {
            buf.setLength(buf.length() - 1);
        }

        return buf.toString();
    }

    private int lookupString(String string, String[] strings) {
        int size = strings.length;

        string = string.replace('-', '_');

        for (int i = 0; i < size; i++)
            if (string.equals(strings[i]))
                return i;

        // if we ever get here, we should probably add the
        // value to WifiConfiguration to reflect that it's
        // supported by the WPA supplicant
        Log.e(TAG, "Failed to look-up a string: " + string);

        return -1;
    }

    /* Returns a unique for a given configuration */
    private static int configKey(WifiConfiguration config) {
        String key = config.configKey();
        return key.hashCode();
    }

    public String getConfigFile() {
        return ipConfigFile;
    }

    boolean checkConfigOverridePermission(int uid) {
        try {
            return (AppGlobals.getPackageManager().checkUidPermission(
                    android.Manifest.permission.OVERRIDE_WIFI_CONFIG, uid)
                    == PackageManager.PERMISSION_GRANTED);
        } catch (RemoteException e) {
            return false;
        }
    }

    /**
     * This function is different than the Wifi version because Wigig doesn't yet implement
     * AutoJoin.
     * The networks is not disabled/enabled at supplicant. Instead the network's status is
     * locally changed so GUI can present Authentication failure to user.
     */
    void handleSSIDStateChange(int netId, boolean enabled) {
        WifiConfiguration config = mConfiguredNetworks.get(netId);
        if (config == null) {
            return;
        }

        Log.d(TAG, "SSID " + (enabled ? "re-enabled" : "temp disabled") + " from supplicant: "
                + config.configKey());
        if (enabled) {
            markNetworkEnabled(config);
        }
        else {
            markNetworkDisabled(config, NetworkSelectionStatus.DISABLED_AUTHENTICATION_FAILURE);
        }
    }

    private void readNetworkBitsetVariable(int netId, BitSet variable, String varName,
            String[] strings) {
        String value = mWigigNative.getNetworkVariable(netId, varName);
        if (!TextUtils.isEmpty(value)) {
            variable.clear();
            String vals[] = value.split(" ");
            for (String val : vals) {
                int index = lookupString(val, strings);
                if (0 <= index) {
                    variable.set(index);
                }
            }
        }
    }
}
