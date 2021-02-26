/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 *
 * Copyright (C) 2008 The Android Open Source Project
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

package com.qualcomm.qti.wigig;

import android.annotation.SdkConstant;
import android.annotation.SdkConstant.SdkConstantType;
import android.annotation.SystemApi;
import android.content.Context;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiInfo;
import android.net.wifi.WpsInfo;
import android.net.wifi.WpsResult;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.IBinder;
import android.os.Looper;
import android.os.Message;
import android.os.Messenger;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.util.Log;
import android.util.SparseArray;

import com.android.internal.util.AsyncChannel;

import java.util.List;
import java.util.concurrent.CountDownLatch;

/**
 * This class provides the primary API for managing all aspects of Wigig
 * connectivity. Get an instance of this class by calling
 * {@link WigigManager#getInstance() WigigManager.getInstance()}
 *
 * It deals with several categories of items:
 * <ul>
 * <li>The list of configured networks. The list can be viewed and updated,
 * and attributes of individual entries can be modified.</li>
 * <li>The currently active Wigig network, if any. Connectivity can be
 * established or torn down, and dynamic information about the state of
 * the network can be queried.</li>
 * <li>Results of access point scans, containing enough information to
 * make decisions about what access point to connect to.</li>
 * <li>It defines the names of various Intent actions that are broadcast
 * upon any sort of change in Wigig state.
 * </ul>
 * This is the API to use when performing Wigig specific operations. To
 * perform operations that pertain to network connectivity at an abstract
 * level, use {@link android.net.ConnectivityManager}.
 */
public class WigigManager {
    private static final String TAG = "WigigManager";

    /**
     * define Message.what base addresses for Wigig protocols. These must be unique and must not
     * conflict with the ones defined in com.android.internal.util.Protocol.
     */
    /** @hide */
    public static final int BASE_WIGIG              = 0x00D90000;
    /** @hide */
    public static final int BASE_WIGIG_P2P_MANAGER  = 0x00D92000;
    /** @hide */
    public static final int BASE_WIGIG_P2P_SERVICE  = 0x00D93000;
    /** @hide */
    public static final int BASE_WIGIG_MONITOR      = 0x00D94000;
    /** @hide */
    public static final int BASE_WIGIG_MANAGER      = 0x00D95000;

    /**
     * Broadcast intent action indicating whether Wigig scanning is allowed currently
     * @hide
     */
    public static final String WIGIG_SCAN_AVAILABLE = "wigig_scan_available";

    /**
     * Extra int indicating scan availability, WIGIG_STATE_ENABLED and WIGIG_STATE_DISABLED
     * @hide
     */
    public static final String EXTRA_SCAN_AVAILABLE = "scan_enabled";

    /**
     * Broadcast intent action indicating that the credential of a Wigig network
     * has been changed. One extra provides the ssid of the network. Another
     * extra provides the event type, whether the credential is saved or forgot.
     * @hide
     */
    @SystemApi
    public static final String WIGIG_CREDENTIAL_CHANGED_ACTION =
            "com.qualcomm.qti.wigig.WIGIG_CREDENTIAL_CHANGED";
    /** @hide */
    @SystemApi
    public static final String EXTRA_WIGIG_CREDENTIAL_EVENT_TYPE = "et";
    /** @hide */
    @SystemApi
    public static final String EXTRA_WIGIG_CREDENTIAL_SSID = "ssid";
    /** @hide */
    @SystemApi
    public static final int WIGIG_CREDENTIAL_SAVED = 0;
    /** @hide */
    @SystemApi
    public static final int WIGIG_CREDENTIAL_FORGOT = 1;

    /**
     * Broadcast intent action indicating that Wigig has been enabled, disabled,
     * or unknown. One extra provides this state as an int.
     * Another extra provides the previous state, if available.
     *
     * @see #EXTRA_WIGIG_STATE
     * @see #EXTRA_PREVIOUS_WIGIG_STATE
     */
    @SdkConstant(SdkConstantType.BROADCAST_INTENT_ACTION)
    public static final String WIGIG_STATE_CHANGED_ACTION =
            "com.qualcomm.qti.wigig.WIGIG_STATE_CHANGED";

    /**
     * The lookup key for an int that indicates whether Wigig is enabled,
     * disabled or unknown.  Retrieve it with
     * {@link android.content.Intent#getIntExtra(String,int)}.
     *
     * @see #WIGIG_STATE_DISABLED
     * @see #WIGIG_STATE_ENABLED
     * @see #WIGIG_STATE_UNKNOWN
     * @see #WIGIG_STATE_FAILED
     * @see #WIGIG_STATE_RATE_UPGRADE
     */
    public static final String EXTRA_WIGIG_STATE = "wigig_state";

    /**
     * The previous Wigig state.
     *
     * @see #EXTRA_WIGIG_STATE
     */
    public static final String EXTRA_PREVIOUS_WIGIG_STATE = "previous_wigig_state";

    /**
     * Wigig is disabled.
     *
     * @see #WIGIG_STATE_CHANGED_ACTION
     * @see #getWigigState()
     */
    public static final int WIGIG_STATE_DISABLED = 1;

    /**
     * Wigig is enabled.
     *
     * @see #WIGIG_STATE_CHANGED_ACTION
     * @see #getWigigState()
     */
    public static final int WIGIG_STATE_ENABLED = 3;

    /**
     * Wigig is in an unknown state. This state will occur when an error happens while enabling
     * or disabling.
     *
     * @see #WIGIG_STATE_CHANGED_ACTION
     * @see #getWigigState()
     */
    public static final int WIGIG_STATE_UNKNOWN = 4;

    /**
     * Wigig is in failed state. This state will occur when load driver failed or start
     * supplicant failed.
     *
     * @hide
     */
    public static final int WIGIG_STATE_FAILED = 5;

    /**
     * Wigig is in Rate Upgrade state.
     *
     * @see #WIGIG_STATE_CHANGED_ACTION
     * @see #getWigigState()
     */
    public static final int WIGIG_STATE_RATE_UPGRADE = 6;

    /**
     * Broadcast intent action indicating that Wigig is connnected in Rate Upgrade (FST) mode.
     * One extra provides the state as an int.
     *
     * @see #EXTRA_WIGIG_RATE_UPGRADE_STATE
     */
    @SdkConstant(SdkConstantType.BROADCAST_INTENT_ACTION)
    public static final String WIGIG_RATE_UPGRADE_STATE_CHANGED_ACTION =
            "com.qualcomm.qti.wigig.WIGIG_RATE_UPGRADE_STATE_CHANGED";

    /**
     * The lookup key for an int that indicates whether Wigig is connected or
     * disconnected in Rate Upgrade (FST) mode.  Retrieve it with
     * {@link android.content.Intent#getIntExtra(String,int)}.
     *
     * @see #WIGIG_RATE_UPGRADE_STATE_DISCONNECTED
     * @see #WIGIG_RATE_UPGRADE_STATE_CONNECTED
     */
    public static final String EXTRA_WIGIG_RATE_UPGRADE_STATE = "wigig_rate_upgrade_state";

    /**
     * Wigig is disconnected in Rate Upgrade (FST) mode.
     *
     * @see #WIGIG_RATE_UPGRADE_STATE_CHANGED_ACTION
     */
    public static final int WIGIG_RATE_UPGRADE_STATE_DISCONNECTED = 1;

    /**
     * Wigig is connected in Rate Upgrade (FST) mode.
     *
     * @see #WIGIG_RATE_UPGRADE_STATE_CHANGED_ACTION
     */
    public static final int WIGIG_RATE_UPGRADE_STATE_CONNECTED = 3;

    /**
     * Broadcast intent action indicating that Wigig AP has been enabled, disabled,
     * enabling, disabling, or failed.
     *
     * @hide
     */
    public static final String WIGIG_AP_STATE_CHANGED_ACTION =
        "com.qualcomm.qti.wigig.WIGIG_AP_STATE_CHANGED";

    /**
     * The lookup key for an int that indicates whether Wigig AP is enabled,
     * disabled, enabling, disabling, or failed.  Retrieve it with
     * {@link android.content.Intent#getIntExtra(String,int)}.
     *
     * @see #WIGIG_AP_STATE_DISABLED
     * @see #WIGIG_AP_STATE_DISABLING
     * @see #WIGIG_AP_STATE_ENABLED
     * @see #WIGIG_AP_STATE_ENABLING
     * @see #WIGIG_AP_STATE_FAILED
     *
     * @hide
     */
    public static final String EXTRA_WIGIG_AP_STATE = "wigig_ap_state";

    /**
     * The lookup key for an int that indicates why softAP start failed
     * currently support general and no_channel
     * @see #SAP_START_FAILURE_GENERAL
     * @see #SAP_START_FAILURE_NO_CHANNEL
     *
     * @hide
     */
    public static final String EXTRA_WIGIG_AP_FAILURE_REASON = "wigig_ap_error_code";

    /**
     * The previous Wigig state.
     *
     * @see #EXTRA_WIGIG_AP_STATE
     *
     * @hide
     */
    public static final String EXTRA_PREVIOUS_WIGIG_AP_STATE = "previous_wigig_ap_state";

    /**
     * Wigig AP is currently being disabled. The state will change to
     * {@link #WIGIG_AP_STATE_DISABLED} if it finishes successfully.
     *
     * @see #WIGIG_AP_STATE_CHANGED_ACTION
     * @see #getWigigApState()
     *
     * @hide
     */
    public static final int WIGIG_AP_STATE_DISABLING = 10;

    /**
     * Wigig AP is disabled.
     *
     * @see #WIGIG_AP_STATE_CHANGED_ACTION
     * @see #getWigigApState()
     *
     * @hide
     */
    public static final int WIGIG_AP_STATE_DISABLED = 11;

    /**
     * Wigig AP is currently being enabled. The state will change to
     * {@link #WIGIG_AP_STATE_ENABLED} if it finishes successfully.
     *
     * @see #WIGIG_AP_STATE_CHANGED_ACTION
     * @see #getWigigApState()
     *
     * @hide
     */
    public static final int WIGIG_AP_STATE_ENABLING = 12;

    /**
     * Wigig AP is enabled.
     *
     * @see #WIGIG_AP_STATE_CHANGED_ACTION
     * @see #getWigigApState()
     *
     * @hide
     */
    public static final int WIGIG_AP_STATE_ENABLED = 13;

    /**
     * Wigig AP is in a failed state. This state will occur when an error occurs during
     * enabling or disabling
     *
     * @see #WIGIG_AP_STATE_CHANGED_ACTION
     * @see #getWigigApState()
     *
     * @hide
     */
    public static final int WIGIG_AP_STATE_FAILED = 14;

    /**
     *  If Wigig AP start failed, this reason code means no legal channel exists on
     *  user selected band by regulatory
     *
     *  @hide
     */
    public static final int SAP_START_FAILURE_GENERAL= 0;

    /**
     *  All other reason for AP start failed besides SAP_START_FAILURE_GENERAL
     *
     *  @hide
     */
    public static final int SAP_START_FAILURE_NO_CHANNEL = 1;

    /**
     * Broadcast intent action indicating that a connection to the supplicant has
     * been established (and it is now possible
     * to perform Wigig operations) or the connection to the supplicant has been
     * lost. One extra provides the connection state as a boolean, where {@code true}
     * means CONNECTED.
     * @see #EXTRA_SUPPLICANT_CONNECTED
     */
    @SdkConstant(SdkConstantType.BROADCAST_INTENT_ACTION)
    public static final String SUPPLICANT_CONNECTION_CHANGE_ACTION =
        "com.qualcomm.qti.wigig.supplicant.CONNECTION_CHANGE";

    private Context mContext;
    IWigigManager mService;

    /**
     * The lookup key for a boolean that indicates whether a connection to
     * the supplicant daemon has been gained or lost. {@code true} means
     * a connection now exists.
     * Retrieve it with {@link android.content.Intent#getBooleanExtra(String,boolean)}.
     */
    public static final String EXTRA_SUPPLICANT_CONNECTED = "connected";

    /**
     * Broadcast intent action indicating that the state of Wigig connectivity
     * has changed. One extra provides the new state
     * in the form of a {@link android.net.NetworkInfo} object. If the new
     * state is CONNECTED, additional extras may provide the BSSID and WifiInfo of
     * the access point.
     * as a {@code String}.
     * @see #EXTRA_NETWORK_INFO
     * @see #EXTRA_BSSID
     * @see #EXTRA_WIFI_INFO
     */
    @SdkConstant(SdkConstantType.BROADCAST_INTENT_ACTION)
    public static final String NETWORK_STATE_CHANGED_ACTION = "com.qualcomm.qti.wigig.STATE_CHANGE";
    /**
     * The lookup key for an int giving the Wigig network id as stored in supplicant. Retrieve with
     * {@link android.content.Intent#getIntExtra(String)}
     */
    public static final String EXTRA_NETWORK_ID = "networkId";
    /**
     * The lookup key for a {@link android.net.NetworkInfo} object associated with the
     * Wigig network. Retrieve with
     * {@link android.content.Intent#getParcelableExtra(String)}.
     */
    public static final String EXTRA_NETWORK_INFO = "networkInfo";
    /**
     * The lookup key for a String giving the BSSID of the access point to which
     * we are connected. Only present when the new state is CONNECTED.
     * Retrieve with
     * {@link android.content.Intent#getStringExtra(String)}.
     */
    public static final String EXTRA_BSSID = "bssid";
    /**
     * The lookup key for a {@link android.net.wifi.WifiInfo} object giving the
     * information about the access point to which we are connected. Only present
     * when the new state is CONNECTED.  Retrieve with
     * {@link android.content.Intent#getParcelableExtra(String)}.
     */
    public static final String EXTRA_WIFI_INFO = "wifiInfo";

    /**
     * Broadcast intent action indicating that the configured networks changed.
     * This can be as a result of adding/updating/deleting a network. If
     * {@link #EXTRA_MULTIPLE_NETWORKS_CHANGED} is set to false the new configuration
     * can be retreived with the {@link #EXTRA_WIFI_CONFIGURATION} extra. If multiple
     * Wigig configurations changed, {@link #EXTRA_WIFI_CONFIGURATION} will not be present.
     * @hide
     */
    @SystemApi
    public static final String CONFIGURED_NETWORKS_CHANGED_ACTION =
        "com.qualcomm.qti.wigig.CONFIGURED_NETWORKS_CHANGE";
    /**
     * The lookup key for a (@link android.net.wifi.WifiConfiguration} object representing
     * the changed Wigig configuration when the {@link #CONFIGURED_NETWORKS_CHANGED_ACTION}
     * broadcast is sent.
     * @hide
     */
    @SystemApi
    public static final String EXTRA_WIFI_CONFIGURATION = "wifiConfiguration";
    /**
     * Multiple network configurations have changed.
     * @see #CONFIGURED_NETWORKS_CHANGED_ACTION
     *
     * @hide
     */
    @SystemApi
    public static final String EXTRA_MULTIPLE_NETWORKS_CHANGED = "multipleChanges";
    /**
     * The lookup key for an integer indicating the reason a Wigig network configuration
     * has changed. Only present if {@link #EXTRA_MULTIPLE_NETWORKS_CHANGED} is {@code false}
     * @see #CONFIGURED_NETWORKS_CHANGED_ACTION
     * @hide
     */
    @SystemApi
    public static final String EXTRA_CHANGE_REASON = "changeReason";
    /**
     * The configuration is new and was added.
     * @hide
     */
    @SystemApi
    public static final int CHANGE_REASON_ADDED = 0;
    /**
     * The configuration was removed and is no longer present in the system's list of
     * configured networks.
     * @hide
     */
    @SystemApi
    public static final int CHANGE_REASON_REMOVED = 1;
    /**
     * The configuration has changed as a result of explicit action or because the system
     * took an automated action such as disabling a malfunctioning configuration.
     * @hide
     */
    @SystemApi
    public static final int CHANGE_REASON_CONFIG_CHANGE = 2;

    /**
     * An access point scan has completed, and results are available from the supplicant.
     * Call {@link #getScanResults()} to obtain the results.
     */
    @SdkConstant(SdkConstantType.BROADCAST_INTENT_ACTION)
    public static final String SCAN_RESULTS_AVAILABLE_ACTION = "com.qualcomm.qti.wigig.SCAN_RESULTS";

    /**
     * Lookup key for a {@code boolean} representing the result of previous {@link #startScan}
     * operation, reported with {@link #SCAN_RESULTS_AVAILABLE_ACTION}.
     * @return true scan was successful, results are updated
     * @return false scan was not successful, results haven't been updated since previous scan
     */
    public static final String EXTRA_RESULTS_UPDATED = "resultsUpdated";

    /**
     * The RSSI (signal strength) has changed.
     * @see #EXTRA_NEW_RSSI
     */
    @SdkConstant(SdkConstantType.BROADCAST_INTENT_ACTION)
    public static final String RSSI_CHANGED_ACTION = "com.qualcomm.qti.wigig.RSSI_CHANGED";
    /**
     * The lookup key for an {@code int} giving the new RSSI in dBm.
     */
    public static final String EXTRA_NEW_RSSI = "newRssi";

    /**
     * Broadcast intent action indicating that the link configuration
     * changed on wigig.
     * @hide
     */
    public static final String LINK_CONFIGURATION_CHANGED_ACTION =
        "com.qualcomm.qti.wigig.LINK_CONFIGURATION_CHANGED";

    /**
     * The lookup key for a {@link android.net.LinkProperties} object associated with the
     * Wigig network. Retrieve with
     * {@link android.content.Intent#getParcelableExtra(String)}.
     * @hide
     */
    public static final String EXTRA_LINK_PROPERTIES = "linkProperties";

    /** Anything worse than or equal to this will show 0 bars. */
    private static final int MIN_RSSI = -75;

    /** Anything better than or equal to this will show the max bars. */
    private static final int MAX_RSSI = -30;

    /**
     * Number of RSSI levels used in the framework to initiate
     * {@link #RSSI_CHANGED_ACTION} broadcast
     * @hide
     */
    public static final int RSSI_LEVELS = 5;

    public static final int DATA_ACTIVITY_NOTIFICATION = 1;

    //Lowest bit indicates data reception and the second lowest
    //bit indicates data transmitted
    /** @hide */
    public static final int DATA_ACTIVITY_NONE         = 0x00;
    /** @hide */
    public static final int DATA_ACTIVITY_IN           = 0x01;
    /** @hide */
    public static final int DATA_ACTIVITY_OUT          = 0x02;
    /** @hide */
    public static final int DATA_ACTIVITY_INOUT        = 0x03;

    /* listeneres array */
    private static final int INVALID_KEY = 0;
    private static int sListenerKey = 1;
    private static final SparseArray sListenerMap = new SparseArray();
    private static final Object sListenerMapLock = new Object();

    /* channel between the manager and the service */
    private static AsyncChannel sAsyncChannel;
    /* wait in class creation until sAsyncChannel gets HALF_CONNECTED */
    private static CountDownLatch sConnected;

    /* create and connect sAsyncChannel only for 1st thread */
    private static final Object sThreadRefLock = new Object();
    private static int sThreadRefCount;
    private static HandlerThread sHandlerThread;

    /**
     * Create a new WigigManager instance.
     * Applications will almost always want to use
     * {@link WigigManager#getInstance() WigigManager.getInstance()} to retrieve an instance
     * of {@link WigigManager WigigManager}.
     * @param context the application context
     * @param service the Binder interface
     * @hide - hide this because it takes in a parameter of type IWigigManager, which
     * is a system private class.
     */
    private WigigManager(Context context, IWigigManager service) {
        mContext = context;
        mService = service;
        init();

        Log.i(TAG, "WigigManager created");
    }

    /**
     * Create a new WigigManager instance.
     * Applications should use this to retrieve an instance of {@link WigigManager WigigManager}.
     */
    public static WigigManager getInstance(Context context) {
        IBinder b = ServiceManager.getService("wigig");
        if (b == null) {
            Log.e(TAG, "Wigig service not available");
            return null;
        }
        IWigigManager wigigService = IWigigManager.Stub.asInterface(b);
        return new WigigManager(context, wigigService);
    }

    /**
     * Return a list of all the networks configured in the supplicant.
     * Not all fields of WifiConfiguration are returned. Only the following
     * fields are filled in:
     * <ul>
     * <li>networkId</li>
     * <li>SSID</li>
     * <li>BSSID</li>
     * <li>priority</li>
     * <li>allowedProtocols</li>
     * <li>allowedKeyManagement</li>
     * <li>allowedAuthAlgorithms</li>
     * <li>allowedPairwiseCiphers</li>
     * <li>allowedGroupCiphers</li>
     * </ul>
     * @return a list of network configurations in the form of a list
     * of {@link WifiConfiguration} objects. Upon failure to fetch or
     * when Wigig is turned off, it can be null.
     */
    public List<WifiConfiguration> getConfiguredNetworks() {
        try {
            return mService.getConfiguredNetworks();
        } catch (RemoteException e) {
            Log.w(TAG, "Caught RemoteException trying to get configured networks: " + e);
            return null;
        }
    }

    /**
     * Disassociate from the currently active access point. This may result
     * in the asynchronous delivery of state change events.
     * @return {@code true} if the operation succeeded
     */
    public boolean disconnect() {
        try {
            mService.disconnect();
            return true;
        } catch (RemoteException e) {
            return false;
        }
    }

    /**
     * Request a scan for access points. Returns immediately. The availability
     * of the results is made known later by means of an asynchronous event sent
     * on completion of the scan.
     * @return {@code true} if the operation succeeded, i.e., the scan was initiated
     */
    public boolean startScan() {
        try {
            mService.startScan();
            return true;
        } catch (RemoteException e) {
            Log.e(TAG, "startScan: " + e);
            return false;
        }
    }

    /**
     * Return dynamic information about the current Wigig connection, if any is active.
     * @return the Wigig information, contained in {@link WifiInfo}.
     */
    public WifiInfo getConnectionInfo() {
        try {
            return mService.getConnectionInfo();
        } catch (RemoteException e) {
            return null;
        }
    }

    /**
     * Return the results of the latest access point scan.
     * @return the list of access points found in the most recent scan. An app must hold
     * {@link android.Manifest.permission#ACCESS_COARSE_LOCATION ACCESS_COARSE_LOCATION} or
     * {@link android.Manifest.permission#ACCESS_FINE_LOCATION ACCESS_FINE_LOCATION} permission
     * in order to get valid results.
     */
    public List<ScanResult> getScanResults() {
        try {
            return mService.getScanResults();
        } catch (RemoteException e) {
            Log.e(TAG, "getScanResults: " + e);
            return null;
        }
    }

    /**
     * Enable or disable Wigig networking stack. Following this request, WIGIG_STATE_CHANGED_ACTION
     * would be broadcast indicating the new Wigig stack state.
     * @param enable {@code true} to enable, {@code false} to disable
     * @return {@code true} if the request to enable/disable the Wigig networking stack was accepted
     */
    public boolean setWigigEnabled(boolean enable) {
        try {
            mService.setWigigEnabled(enable);
            return true;
        } catch (RemoteException e) {
            Log.e(TAG, "setWigigEnabled: " + e);
            return false;
        }
    }

    /**
     * Return whether Wigig stack is enabled
     * @return {@code true} if the Wigig stack is enabled
     */
    public boolean isWigigEnabled() {
        try {
            return mService.isWigigEnabled();
        } catch (RemoteException e) {
            Log.e(TAG, "isWigigEnabled: " + e);
            return false;
        }
    }

    /**
     * Enable or disable Wigig Rate Upgrade mode
     *
     * @param enable {@code true} to enable, {@code false} to
     *               disable
     * @return {@code true} if the request to enable/disable the
     *         Rate Upgrade was accepted
     */
    public boolean setRateUpgradeEnabled(boolean enable) {
        try {
            mService.setRateUpgradeEnabled(enable);
            return true;
        } catch (RemoteException e) {
            Log.e(TAG, "setRateUpgradeEnabled: " + e);
            return false;
        }
    }

    /**
     * Return whether WIGIG is in RateUpgrade mode.
     * @return {@code true} if RateUpgrade is enabled
     */
    public boolean isRateUpgradeEnabled() {
        try {
            return mService.isRateUpgradeEnabled();
        } catch (RemoteException e) {
            Log.e(TAG, "isRateUpgradeEnabled: " + e);
            return false;
        }
    }

    /**
     * Calculates the level of the signal. This should be used any time a signal
     * is being shown.
     *
     * @param rssi The power of the signal measured in RSSI.
     * @param numLevels The number of levels to consider in the calculated
     *            level.
     * @return A level of the signal, given in the range of 0 to numLevels-1
     *         (both inclusive).
     */
    public static int calculateSignalLevel(int rssi, int numLevels) {
        if (rssi <= MIN_RSSI) {
            return 0;
        } else if (rssi >= MAX_RSSI) {
            return numLevels - 1;
        } else {
            float inputRange = (MAX_RSSI - MIN_RSSI);
            float outputRange = numLevels;
            return (int)((float)(rssi - MIN_RSSI - 1) * outputRange / inputRange);
        }
    }

    /**
     * Start Access Point mode with the specified
     * configuration. If the radio is already running in
     * AP mode, update the new configuration.
     * Note that starting in access point mode disables station
     * mode operation
     *
     * @param wifiConfig SSID, security and channel details as
     *        part of WifiConfiguration
     * @return {@code true} if the operation succeeds, {@code false} otherwise
     *
     * @hide Dont open up yet
     */
    public boolean setWigigApEnabled(WifiConfiguration wifiConfig, boolean enabled) {
        try {
            mService.setWigigApEnabled(wifiConfig, enabled);
            return true;
        } catch (RemoteException e) {
            return false;
        }
    }

    /**
     * Gets the Wigig AP state
     *
     * @return One of {@link #WIGIG_AP_STATE_DISABLED},
     *         {@link #WIGIG_AP_STATE_DISABLING}, {@link #WIGIG_AP_STATE_ENABLED},
     *         {@link #WIGIG_AP_STATE_ENABLING}, {@link #WIGIG_AP_STATE_FAILED}
     *
     * @see #isWigigApEnabled()
     *
     * @hide Dont open yet
     */
    public int getWigigApState() {
        try {
            return mService.getWigigApEnabledState();
        } catch (RemoteException e) {
            return WIGIG_AP_STATE_FAILED;
        }
    }

    /**
     * Return whether Wigig AP is enabled or disabled.
     *
     * @return {@code true} if Wigig AP is enabled
     *
     * @see #getWigigApState()
     *
     * @hide Dont open yet
     */
    public boolean isWigigApEnabled() {
        return getWigigApState() == WIGIG_AP_STATE_ENABLED;
    }

    /**
     * Gets the Wigig AP Configuration.
     *
     * @return AP details in WifiConfiguration
     *
     * @hide Dont open yet
     */
    public WifiConfiguration getWigigApConfiguration() {
        try {
            return mService.getWigigApConfiguration();
        } catch (RemoteException e) {
            return null;
        }
    }

    /**
     * Sets the Wigig AP Configuration.
     *
     * @return {@code true} if the operation succeeded, {@code false} otherwise
     *
     * @hide Dont open yet
     */
    public boolean setWigigApConfiguration(WifiConfiguration wifiConfig) {
        try {
            mService.setWigigApConfiguration(wifiConfig);
            return true;
        } catch (RemoteException e) {
            return false;
        }
    }

    private static final int BASE = BASE_WIGIG_MANAGER;

    /* Commands to WigigService */
    /** @hide */
    public static final int CONNECT_NETWORK                 = BASE + 1;
    /** @hide */
    public static final int CONNECT_NETWORK_FAILED          = BASE + 2;
    /** @hide */
    public static final int CONNECT_NETWORK_SUCCEEDED       = BASE + 3;

    /** @hide */
    public static final int FORGET_NETWORK                  = BASE + 4;
    /** @hide */
    public static final int FORGET_NETWORK_FAILED           = BASE + 5;
    /** @hide */
    public static final int FORGET_NETWORK_SUCCEEDED        = BASE + 6;

    /** @hide */
    public static final int SAVE_NETWORK                    = BASE + 7;
    /** @hide */
    public static final int SAVE_NETWORK_FAILED             = BASE + 8;
    /** @hide */
    public static final int SAVE_NETWORK_SUCCEEDED          = BASE + 9;

    /** @hide */
    public static final int START_WPS                       = BASE + 10;
    /** @hide */
    public static final int START_WPS_SUCCEEDED             = BASE + 11;
    /** @hide */
    public static final int WPS_FAILED                      = BASE + 12;
    /** @hide */
    public static final int WPS_COMPLETED                   = BASE + 13;

    /** @hide */
    public static final int CANCEL_WPS                      = BASE + 14;
    /** @hide */
    public static final int CANCEL_WPS_FAILED               = BASE + 15;
    /** @hide */
    public static final int CANCEL_WPS_SUCCEDED             = BASE + 16;

    /** @hide */
    public static final int DISABLE_NETWORK                 = BASE + 17;
    /** @hide */
    public static final int DISABLE_NETWORK_FAILED          = BASE + 18;
    /** @hide */
    public static final int DISABLE_NETWORK_SUCCEEDED       = BASE + 19;

    /**
     * Passed with {@link ActionListener#onFailure}.
     * Indicates that the operation failed due to an internal error.
     * @hide
     */
    public static final int ERROR                       = 0;

    /**
     * Passed with {@link ActionListener#onFailure}.
     * Indicates that the operation is already in progress
     * @hide
     */
    public static final int IN_PROGRESS                 = 1;

    /**
     * Passed with {@link ActionListener#onFailure}.
     * Indicates that the operation failed because the framework is busy and
     * unable to service the request
     * @hide
     */
    public static final int BUSY                        = 2;

    /* WPS specific errors */
    /** WPS overlap detected */
    public static final int WPS_OVERLAP_ERROR           = 3;
    /** WEP on WPS is prohibited */
    public static final int WPS_WEP_PROHIBITED          = 4;
    /** TKIP only prohibited */
    public static final int WPS_TKIP_ONLY_PROHIBITED    = 5;
    /** Authentication failure on WPS */
    public static final int WPS_AUTH_FAILURE            = 6;
    /** WPS timed out */
    public static final int WPS_TIMED_OUT               = 7;

    /**
     * Passed with {@link ActionListener#onFailure}.
     * Indicates that the operation failed due to invalid inputs
     * @hide
     */
    public static final int INVALID_ARGS                = 8;

    /**
     * Passed with {@link ActionListener#onFailure}.
     * Indicates that the operation failed due to user permissions.
     * @hide
     */
    public static final int NOT_AUTHORIZED              = 9;

    /**
     * Interface for callback invocation on an application action
     * @hide
     */
    public interface ActionListener {
        /** The operation succeeded */
        public void onSuccess();
        /**
         * The operation failed
         * @param reason The reason for failure could be one of
         * {@link #ERROR}, {@link #IN_PROGRESS} or {@link #BUSY}
         */
        public void onFailure(int reason);
    }

    /** Interface for callback invocation on a start WPS action */
    public static abstract class WpsCallback {
        /** WPS start succeeded */
        public abstract void onStarted(String pin);

        /** WPS operation completed succesfully */
        public abstract void onSucceeded();

        /**
         * WPS operation failed
         * @param reason The reason for failure could be one of
         * {@link #WPS_TKIP_ONLY_PROHIBITED}, {@link #WPS_OVERLAP_ERROR},
         * {@link #WPS_WEP_PROHIBITED}, {@link #WPS_TIMED_OUT} or {@link #WPS_AUTH_FAILURE}
         * and some generic errors.
         */
        public abstract void onFailed(int reason);
    }

    private static class ServiceHandler extends Handler {
        ServiceHandler(Looper looper) {
            super(looper);
        }

        @Override
        public void handleMessage(Message message) {
            Object listener = removeListener(message.arg2);
            switch (message.what) {
                case AsyncChannel.CMD_CHANNEL_HALF_CONNECTED:
                    if (message.arg1 == AsyncChannel.STATUS_SUCCESSFUL) {
                        sAsyncChannel.sendMessage(AsyncChannel.CMD_CHANNEL_FULL_CONNECTION);
                    } else {
                        Log.e(TAG, "Failed to set up channel connection");
                        // This will cause all further async API calls on the WigigManager
                        // to fail and throw an exception
                        sAsyncChannel = null;
                    }
                    sConnected.countDown();
                    break;
                case AsyncChannel.CMD_CHANNEL_FULLY_CONNECTED:
                    // Ignore
                    break;
                case AsyncChannel.CMD_CHANNEL_DISCONNECTED:
                    Log.e(TAG, "Channel connection lost");
                    // This will cause all further async API calls on the WigigManager
                    // to fail and throw an exception
                    sAsyncChannel = null;
                    getLooper().quit();
                    break;
                    /* ActionListeners grouped together */
                case WigigManager.CONNECT_NETWORK_FAILED:
                case WigigManager.FORGET_NETWORK_FAILED:
                case WigigManager.SAVE_NETWORK_FAILED:
                case WigigManager.DISABLE_NETWORK_FAILED:
                    if (listener != null) {
                        ((ActionListener) listener).onFailure(message.arg1);
                    }
                    break;
                    /* ActionListeners grouped together */
                case WigigManager.CONNECT_NETWORK_SUCCEEDED:
                case WigigManager.FORGET_NETWORK_SUCCEEDED:
                case WigigManager.SAVE_NETWORK_SUCCEEDED:
                case WigigManager.DISABLE_NETWORK_SUCCEEDED:
                    if (listener != null) {
                        ((ActionListener) listener).onSuccess();
                    }
                    break;
                case WigigManager.START_WPS_SUCCEEDED:
                    if (listener != null) {
                        WpsResult result = (WpsResult) message.obj;
                        ((WpsCallback) listener).onStarted(result.pin);
                        //Listener needs to stay until completion or failure
                        synchronized(sListenerMapLock) {
                            sListenerMap.put(message.arg2, listener);
                        }
                    }
                    break;
                case WigigManager.WPS_COMPLETED:
                    if (listener != null) {
                        ((WpsCallback) listener).onSucceeded();
                    }
                    break;
                case WigigManager.WPS_FAILED:
                    if (listener != null) {
                        ((WpsCallback) listener).onFailed(message.arg1);
                    }
                    break;
                case WigigManager.CANCEL_WPS_SUCCEDED:
                    if (listener != null) {
                        ((WpsCallback) listener).onSucceeded();
                    }
                    break;
                case WigigManager.CANCEL_WPS_FAILED:
                    if (listener != null) {
                        ((WpsCallback) listener).onFailed(message.arg1);
                    }
                    break;
                default:
                    //ignore
                    break;
            }
        }
    }

    private static int putListener(Object listener) {
        if (listener == null) return INVALID_KEY;
        int key;
        synchronized (sListenerMapLock) {
            do {
                key = sListenerKey++;
            } while (key == INVALID_KEY);
            sListenerMap.put(key, listener);
        }
        return key;
    }

    private static Object removeListener(int key) {
        if (key == INVALID_KEY) return null;
        synchronized (sListenerMapLock) {
            Object listener = sListenerMap.get(key);
            sListenerMap.remove(key);
            return listener;
        }
    }

    private void init() {
        synchronized (sThreadRefLock) {
            if (++sThreadRefCount == 1) {
                Messenger messenger = getWigigServiceMessenger();
                if (messenger == null) {
                    sAsyncChannel = null;
                    return;
                }

                sHandlerThread = new HandlerThread("WigigManager");
                sAsyncChannel = new AsyncChannel();
                sConnected = new CountDownLatch(1);

                sHandlerThread.start();
                Handler handler = new ServiceHandler(sHandlerThread.getLooper());
                sAsyncChannel.connect(mContext, handler, messenger);
                try {
                    sConnected.await();
                } catch (InterruptedException e) {
                    Log.e(TAG, "interrupted wait at init");
                }
            }
        }
    }

    private void validateChannel() {
        if (sAsyncChannel == null) throw new IllegalStateException(
                "No permission to access and change wigig or a bad initialization");
    }

    /**
     * Connect to a network with the given configuration. The network also
     * gets added to the supplicant configuration.
     *
     * This function is used for connecting to a new network (i.e. not saved)
     *
     * @param config the set of variables that describe the configuration,
     *            contained in a {@link WifiConfiguration} object.
     * @param listener for callbacks on success or failure. Can be null.
     * @throws IllegalStateException if the WigigManager instance needs to be
     * initialized again
     *
     * @hide
     */
    public void connect(WifiConfiguration config, ActionListener listener) {
        if (config == null) throw new IllegalArgumentException("config cannot be null");
        validateChannel();
        // Use INVALID_NETWORK_ID for arg1 when passing a config object
        // arg1 is used to pass network id when the network already exists
        sAsyncChannel.sendMessage(CONNECT_NETWORK, WifiConfiguration.INVALID_NETWORK_ID,
                putListener(listener), config);
    }

    /**
     * Connect to a network with the given networkId.
     *
     * This function is used for connecting to a saved network (i.e. not new)
     *
     * @param networkId the network id identifiying the network in the
     *                supplicant configuration list
     * @param listener for callbacks on success or failure. Can be null.
     * @throws IllegalStateException if the WigigManager instance needs to be
     * initialized again
     * @hide
     */
    public void connect(int networkId, ActionListener listener) {
        if (networkId < 0) throw new IllegalArgumentException("Network id cannot be negative");
        validateChannel();
        sAsyncChannel.sendMessage(CONNECT_NETWORK, networkId, putListener(listener));
    }

    /**
     * Save the given network in the supplicant config. If the network already
     * exists, the configuration is updated. A new network is enabled
     * by default.
     *
     * For a new network, this function adds, enables and saves it to supplicant.
     * For an existing network, it updates and saves it to supplicant.
     *
     * @param config the set of variables that describe the configuration,
     *            contained in a {@link WifiConfiguration} object.
     * @param listener for callbacks on success or failure. Can be null.
     * @throws IllegalStateException if the WigigManager instance needs to be
     * initialized again
     * @hide
     */
    public void save(WifiConfiguration config, ActionListener listener) {
        if (config == null) throw new IllegalArgumentException("config cannot be null");
        validateChannel();
        sAsyncChannel.sendMessage(SAVE_NETWORK, 0, putListener(listener), config);
    }

    /**
     * Delete the network in the supplicant config.
     *
     * @param config the set of variables that describe the configuration,
     *            contained in a {@link WifiConfiguration} object.
     * @param listener for callbacks on success or failure. Can be null.
     * @throws IllegalStateException if the WigigManager instance needs to be
     * initialized again
     * @hide
     */
    public void forget(int netId, ActionListener listener) {
        if (netId < 0) throw new IllegalArgumentException("Network id cannot be negative");
        validateChannel();
        sAsyncChannel.sendMessage(FORGET_NETWORK, netId, putListener(listener));
    }

    /**
     * Start Wi-fi Protected Setup
     *
     * @param config WPS configuration
     * @param listener for callbacks on success or failure. Can be null.
     * @throws IllegalStateException if the WigigManager instance needs to be
     * initialized again
     * @throws IllegalArgumentException if config is null
     */
    public void startWps(WpsInfo config, WpsCallback listener) {
        if (config == null) throw new IllegalArgumentException("config cannot be null");
        validateChannel();
        sAsyncChannel.sendMessage(START_WPS, 0, putListener(listener), config);
    }

    /**
     * Cancel any ongoing Wi-fi Protected Setup
     *
     * @param listener for callbacks on success or failure. Can be null.
     * @throws IllegalStateException if the WigigManager instance needs to be
     * initialized again
     */
    public void cancelWps(WpsCallback listener) {
        validateChannel();
        sAsyncChannel.sendMessage(CANCEL_WPS, 0, putListener(listener));
    }

    /**
     * Get a reference to WigigService handler. This is used by a client to establish
     * an AsyncChannel communication with WigigService
     *
     * @return Messenger pointing to the WigigService handler
     * @hide
     */
    public Messenger getWigigServiceMessenger() {
        try {
            return mService.getWigigServiceMessenger();
        } catch (RemoteException e) {
            return null;
        } catch (SecurityException e) {
            return null;
        }
    }

    /**
     * Returns the file in which IP and proxy configuration data is stored
     * @hide
     */
    public String getConfigFile() {
        try {
            return mService.getConfigFile();
        } catch (RemoteException e) {
            return null;
        }
    }

    protected void finalize() throws Throwable {
        try {
            synchronized (sThreadRefLock) {
                if (--sThreadRefCount == 0 && sAsyncChannel != null) {
                    sAsyncChannel.disconnect();
                }
            }
        } finally {
            super.finalize();
        }
    }
}
