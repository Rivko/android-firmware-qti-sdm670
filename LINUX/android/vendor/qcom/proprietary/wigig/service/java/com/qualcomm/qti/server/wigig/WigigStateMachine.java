/*
 * Copyright (c) 2016, 2019 Qualcomm Technologies, Inc.
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

import android.Manifest;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.IPackageManager;
import android.content.pm.PackageManager;
import android.net.ConnectivityManager;
import android.net.ConnectivityManager.NetworkCallback;
import android.net.DhcpResults;
import android.net.InterfaceConfiguration;
import android.net.LinkAddress;
import android.net.LinkProperties;
import android.net.NetworkAgent;
import android.net.Network;
import android.net.NetworkCapabilities;
import android.net.NetworkInfo;
import android.net.NetworkInfo.DetailedState;
import android.net.NetworkRequest;
import android.net.NetworkUtils;
import android.net.RouteInfo;
import android.net.StaticIpConfiguration;
import android.net.StringNetworkSpecifier;
import android.net.dhcp.DhcpClient;
import android.net.ip.IpManager;
import android.net.wifi.SupplicantState;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.net.wifi.WpsInfo;
import android.net.wifi.WpsResult;
import android.net.wifi.WpsResult.Status;
import android.os.Binder;
import android.os.IBinder;
import android.os.INetworkManagementService;
import android.os.Looper;
import android.os.Message;
import android.os.PowerManager;
import android.os.Process;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.os.SystemProperties;
import android.os.UserHandle;
import android.provider.Settings;
import android.text.TextUtils;
import android.util.Log;
import android.util.LruCache;

import com.android.internal.R;
import com.android.internal.util.AsyncChannel;
import com.android.internal.util.HexDump;
import com.android.internal.util.State;
import com.android.internal.util.StateMachine;
import com.qualcomm.qti.server.wigig.WigigConfigStore;
import com.qualcomm.qti.server.wigig.p2p.WigigP2pServiceImpl;
import com.qualcomm.qti.wigig.ScanResult;
import com.qualcomm.qti.wigig.ScanResult.InformationElement;
import com.qualcomm.qti.wigig.WigigManager;
import com.qualcomm.qti.wigig.WigigSsid;
import com.qualcomm.qti.wigig.p2p.IWigigP2pManager;
import com.qualcomm.qti.wigig.p2p.WigigP2pManager;

import java.net.Inet4Address;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import static android.net.ConnectivityManager.TETHERING_WIFI;
import static com.qualcomm.qti.wigig.WigigManager.WIGIG_AP_STATE_DISABLED;
import static com.qualcomm.qti.wigig.WigigManager.WIGIG_AP_STATE_DISABLING;
import static com.qualcomm.qti.wigig.WigigManager.WIGIG_AP_STATE_ENABLED;
import static com.qualcomm.qti.wigig.WigigManager.WIGIG_AP_STATE_ENABLING;
import static com.qualcomm.qti.wigig.WigigManager.WIGIG_AP_STATE_FAILED;
import static com.qualcomm.qti.wigig.WigigManager.WIGIG_STATE_DISABLED;
import static com.qualcomm.qti.wigig.WigigManager.WIGIG_STATE_ENABLED;
import static com.qualcomm.qti.wigig.WigigManager.WIGIG_STATE_FAILED;
import static com.qualcomm.qti.wigig.WigigManager.WIGIG_STATE_RATE_UPGRADE;
import static com.qualcomm.qti.wigig.WigigManager.WIGIG_STATE_UNKNOWN;

/**
 * Track the state of Wigig connectivity. All event handling is done here,
 * and all changes in connectivity state are initiated here.
 */
public class WigigStateMachine extends StateMachine {

    private static final String NETWORKTYPE = "WIGIG";
    // mark our NetworkAgent with unique specifier to avoid ConnectivityService to reap our
    // agent when Wifi is also connected/validated
    private static final String NETWORK_SPECIFIER = "WIGIG_11AD";
    private static boolean DBG = true;
    private static boolean mLogMessages = true;

    /**
     * Log with error attribute
     *
     * @param s is string log
     */
    protected void loge(String s) {
        Log.e(getName(), s);
    }
    protected void log(String s) {;
        Log.d(getName(), s);
    }

    private WigigMonitor mWigigMonitor;
    private WigigNative mWigigNative;
    private WigigConfigStore mWigigConfigStore;
    private INetworkManagementService mNwService;
    private ConnectivityManager mCm;
    private WigigApConfigStore mWigigApConfigStore;
    private final boolean mP2pSupported;
    private final String mPrimaryDeviceType;

    /* Scan results handling */
    private List<ScanResult> mScanResults = new ArrayList<ScanResult>();
    private static final int SCAN_RESULT_CACHE_SIZE = 160;
    private final LruCache<String, ScanResult> mScanResultCache;

    private boolean mScreenOn = false;
    private boolean mDoPno = true;

    private final String mInterfaceName;
    /* The interface for ipManager */
    private final String mDataInterfaceName;

    private int mLastSignalLevel = -1;
    private String mLastBssid;
    private int mLastNetworkId; // The network Id we successfully joined

    /* Tracks sequence number on a periodic scan message after disconnect */
    private int mDisconnectedPeriodicScanToken = 0;
    private int mRssiPollToken = 0;

    /**
     * Interval in milliseconds between polling for RSSI
     * and linkspeed information
     */
    private static final int POLL_RSSI_INTERVAL_MSECS = 3000;

    /**
     * Delay between supplicant restarts upon failure to establish connection
     */
    private static final int SUPPLICANT_RESTART_INTERVAL_MSECS = 5000;
    /* Tracks sequence number on stop failure message */
    private int mSupplicantStopFailureToken = 0;

    /**
     * Tether state change notification time out
     */
    private static final int TETHER_NOTIFICATION_TIME_OUT_MSECS = 5000;

    /* Tracks sequence number on a tether notification time out */
    private int mTetherToken = 0;

    /**
     * The link properties of the wigig interface.
     * Do not modify this directly; use updateLinkProperties instead.
     */
    private LinkProperties mLinkProperties;

    private Context mContext;

    private final Object mDhcpResultsLock = new Object();
    private DhcpResults mDhcpResults;

    // NOTE: Do not return to clients - use #getWiFiInfoForUid(int)
    private WifiInfo mWifiInfo;
    private NetworkInfo mNetworkInfo;
    private NetworkCapabilities mNetworkCapabilities;

    private WigigP2pServiceImpl mWigigP2pServiceImpl;
    /* Used for interaction with Wifi for the purpose of switching to/from rate upgrade mode */
    private WifiManager mWifiManager;
    /* Used to receive broadcast from Wifi when the states changes */
    private final BroadcastReceiver mWifiReceiver;

    private WigigNetworkMonitor mWigigNetworkMonitor;

    /**
     * Checks to see if user has specified if the apps configuration is connectable.
     * If the user hasn't specified we query the user and return true.
     *
     * @param message The message to be deferred
     * @param netId Network id of the configuration to check against
     * @return True if we are waiting for user feedback or netId is invalid. False otherwise.
     */
    boolean deferForUserInput(Message message, int netId){
        final WifiConfiguration config = mWigigConfigStore.getWifiConfiguration(netId);

        // We can only evaluate saved configurations.
        if (config == null) {
            logd("deferForUserInput: configuration for netId=" + netId + " not stored");
            return true;
        }

        switch (config.userApproved) {
            case WifiConfiguration.USER_APPROVED:
            case WifiConfiguration.USER_BANNED:
                return false;
            case WifiConfiguration.USER_PENDING:
            default: // USER_UNSPECIFIED
               /* the intention was to ask user here; but a dialog box is   *
                * too invasive; so we are going to allow connection for now */
                config.userApproved = WifiConfiguration.USER_APPROVED;
                return false;
        }
    }

    private IpManager mIpManager;

    // Channel for sending replies.
    private AsyncChannel mReplyChannel = new AsyncChannel();

    // Used to initiate a connection with WigigP2pService
    private AsyncChannel mWigigP2pChannel;
    private AsyncChannel mWigigApConfigChannel;

    private WigigNetworkAgent mNetworkAgent;

    // Used to filter out requests we couldn't possibly satisfy.
    private final NetworkCapabilities mNetworkCapabilitiesFilter = new NetworkCapabilities();

    // Used to detect when WIGIG network is registered, for adding local route
    private NetworkCallback mNetworkCallback;

    private boolean mAddedLocalRoute;

    /* The base for wigig message types */
    static final int BASE = WigigManager.BASE_WIGIG;
    /* Start the supplicant */
    static final int CMD_START_SUPPLICANT                 = BASE + 11;
    /* Stop the supplicant */
    static final int CMD_STOP_SUPPLICANT                  = BASE + 12;
    /* Indicates Static IP succeeded */
    static final int CMD_STATIC_IP_SUCCESS                = BASE + 15;
    /* Indicates Static IP failed */
    static final int CMD_STATIC_IP_FAILURE                = BASE + 16;
    /* Indicates supplicant stop failed */
    static final int CMD_STOP_SUPPLICANT_FAILED           = BASE + 17;

    /* Start the soft access point */
    static final int CMD_START_AP                                       = BASE + 21;
    /* Indicates soft ap start succeeded */
    static final int CMD_START_AP_SUCCESS                               = BASE + 22;
    /* Indicates soft ap start failed */
    static final int CMD_START_AP_FAILURE                               = BASE + 23;
    /* Stop the soft access point */
    static final int CMD_STOP_AP                                        = BASE + 24;
    /* Set the soft access point configuration */
    static final int CMD_SET_AP_CONFIG                                  = BASE + 25;
    /* Soft access point configuration set completed */
    static final int CMD_SET_AP_CONFIG_COMPLETED                        = BASE + 26;
    /* Request the soft access point configuration */
    static final int CMD_REQUEST_AP_CONFIG                              = BASE + 27;
    /* Response to access point configuration request */
    static final int CMD_RESPONSE_AP_CONFIG                             = BASE + 28;
    /* Invoked when getting a tether state change notification */
    static final int CMD_TETHER_STATE_CHANGE                            = BASE + 29;
    /* A delayed message sent to indicate tether state change failed to arrive */
    static final int CMD_TETHER_NOTIFICATION_TIMED_OUT                  = BASE + 30;

    /* Supplicant commands */
    /* Get configured networks */
    static final int CMD_GET_CONFIGURED_NETWORKS          = BASE + 59;
    /* Initiate a scan */
    static final int CMD_START_SCAN                       = BASE + 71;
    /* Disconnect from a network */
    static final int CMD_DISCONNECT                       = BASE + 73;

    /* RSSI poll */
    static final int CMD_RSSI_POLL                                      = BASE + 83;

    /* DHCP/IP configuration watchdog */
    static final int CMD_OBTAINING_IP_ADDRESS_WATCHDOG_TIMER            = BASE + 93;

    /* Screen change intent handling */
    static final int CMD_SCREEN_STATE_CHANGED                           = BASE + 95;

    /* Disconnecting state watchdog */
    static final int CMD_DISCONNECTING_WATCHDOG_TIMER                   = BASE + 96;

    /**
     * Make this timer 40 seconds, which is about the normal DHCP timeout.
     * In no valid case, the WigigStateMachine should remain stuck in ObtainingIpAddress
     * for more than 30 seconds.
     */
    static final int OBTAINING_IP_ADDRESS_GUARD_TIMER_MSEC = 40000;

    int obtainingIpWatchdogCount = 0;

    int disconnectingWatchdogCount = 0;
    static final int DISCONNECTING_GUARD_TIMER_MSEC = 5000;

    /* P2p commands */
    /* We are ok with no response here since we wont do much with it anyway */
    public static final int CMD_ENABLE_P2P                              = BASE + 131;
    /* In order to shut down supplicant cleanly, we wait till p2p has
     * been disabled */
    public static final int CMD_DISABLE_P2P_REQ                         = BASE + 132;
    public static final int CMD_DISABLE_P2P_RSP                         = BASE + 133;

    /* We now have a valid IP configuration. */
    static final int CMD_IP_CONFIGURATION_SUCCESSFUL                    = BASE + 138;
    /* We no longer have a valid IP configuration. */
    static final int CMD_IP_CONFIGURATION_LOST                          = BASE + 139;
    /* Link configuration (IP address, DNS, ...) changes notified via netlink */
    static final int CMD_UPDATE_LINKPROPERTIES                          = BASE + 140;

    static final int CMD_ASSOCIATED_BSSID                               = BASE + 147;

    /**
     * Used to handle messages bounced between WigigStateMachine and IpManager.
     */
    static final int CMD_IPV4_PROVISIONING_SUCCESS                      = BASE + 200;
    static final int CMD_IPV4_PROVISIONING_FAILURE                      = BASE + 201;

    /**
     * After sending disconnect to supplicant, supplicant will stop periodic scan. When there
     * are saved networks, framework will do periodic scan to update list of nearby APs
     */
    static final int CMD_DISCONNECTED_PERIODIC_SCAN                     = BASE + 290;
    static final int CMD_AIRPLANE_TOGGLED                               = BASE + 291;

    /* request from P2P state machine to use the radio for P2P operations */
    public static final int CMD_P2P_REQUEST_RADIO                       = BASE + 300;
    /* notification from P2P state machine when P2P operation finished and
     * radio is released */
    public static final int CMD_P2P_RELEASED_RADIO                      = BASE + 301;
    /* P2P radio availability changed. Sent whenever the availability state
     * changes or as a response to CMD_P2P_REQUEST_RADIO */
    public static final int CMD_P2P_RADIO_STATE_UPDATE                  = BASE + 302;
    /* Internal message to request sending radio state update */
    public static final int CMD_P2P_REQUEST_RADIO_STATE_UPDATE          = BASE + 303;

    /* Start rate upgrade */
    static final int CMD_START_RATE_UPGRADE                             = BASE + 400;
    /* Stop rate upgrade */
    static final int CMD_STOP_RATE_UPGRADE                              = BASE + 401;
    /* Wifi state changed Enabled/Disabled */
    static final int CMD_RATE_UPGRADE_WIFI_STATE_CHANGED                = BASE + 402;
    /* Wifi state change watchdog */
    static final int CMD_WIFI_STATE_CHANGE_WATCHDOG_TIMER               = BASE + 403;

    /* Internal message to add local route for WIGIG interface */
    static final int CMD_ADD_LOCAL_ROUTE                                = BASE + 410;
    /* Internal message to update supplicant about new country code */
    static final int CMD_COUNTRY_CODE_CHANGED                           = BASE + 411;

    /* argument for CMD_P2P_RADIO_STATE_UPDATE */
    /* radio is ready, but P2P state machine must request exclusive usage
     * before proceeding with any P2P operation */
    public static final int P2P_RADIO_STATE_IDLE                        = 0;
    /* radio is available. P2P operation can proceed */
    public static final int P2P_RADIO_STATE_IN_USE_BY_P2P               = 1;
    /* radio is in-use by Wigig operation such as connected station.
     * P2P operation cannot proceed. */
    public static final int P2P_RADIO_STATE_IN_USE_BY_WIGIG             = 2;

     /* Wifi state change timeout */
    private static final int WIFI_STATE_CHANGE_GUARD_TIMER_MSEC = 10000;

    /**
     * Supplicant scan interval in milliseconds.
     * Comes from {@link Settings.Global#WIGIG_SUPPLICANT_SCAN_INTERVAL_MS} or
     * from the default config if the setting is not set
     */
    private long mSupplicantScanIntervalMs;

    /* Default parent state */
    private State mDefaultState = new DefaultState();
    /* Temporary initial state */
    private State mInitialState = new InitialState();
    /* Driver loaded, waiting for supplicant to start */
    private State mSupplicantStartingState = new SupplicantStartingState();
    /* Driver loaded and supplicant ready */
    private State mSupplicantStartedState = new SupplicantStartedState();
    /* Waiting for supplicant to stop and monitor to exit */
    private State mSupplicantStoppingState = new SupplicantStoppingState();
    /* Waiting for Rate Upgrade to start */
    private State mRateUpgradeStartingState = new RateUpgradeStartingState();
    /* Rate Upgrate ready */
    private State mRateUpgradeState = new RateUpgradeState();
    /* Waiting for Rate Upgrade to stop */
    private State mRateUpgradeStoppingState = new RateUpgradeStoppingState();
    /* Wait until p2p is disabled
     * This is a special state which is entered right after we exit out of DriverStartedState
     * before transitioning to another state.
     */
    private State mWaitForP2pDisableState = new WaitForP2pDisableState();
    /* radio is in use by P2P state machine. Wigig operations such as
     * connected station cannot proceed before P2P state machine releases
     * the radio */
    private State mRadioInUseByP2pState = new RadioInUseByP2pState();
    /* Connected at 802.11 (L2) level */
    private State mL2ConnectedState = new L2ConnectedState();
    /* fetching IP after connection to access point (assoc+auth complete) */
    private State mObtainingIpState = new ObtainingIpState();
    /* Connected with IP addr */
    private State mConnectedState = new ConnectedState();
    /* disconnect issued, waiting for network disconnect confirmation */
    private State mDisconnectingState = new DisconnectingState();
    /* Network is not connected, supplicant assoc+auth is not complete */
    private State mDisconnectedState = new DisconnectedState();
    /* Waiting for WPS to be completed*/
    private State mWpsRunningState = new WpsRunningState();

    /* Soft ap is starting up */
    private State mSoftApStartingState = new SoftApStartingState();
    /* Soft ap is running */
    private State mSoftApStartedState = new SoftApStartedState();
    /* Soft ap is running and we are waiting for tether notification */
    private State mTetheringState = new TetheringState();
    /* Soft ap is running and we are tethered through connectivity service */
    private State mTetheredState = new TetheredState();
    /* Waiting for untether confirmation before stopping soft Ap */
    private State mUntetheringState = new UntetheringState();

    private class TetherStateChange {
        ArrayList<String> available;
        ArrayList<String> active;

        TetherStateChange(ArrayList<String> av, ArrayList<String> ac) {
            available = av;
            active = ac;
        }
    }

    /**
     * One of  {@link WigigManager#WIGIG_STATE_DISABLED},
     * {@link WigigManager#WIGIG_STATE_DISABLING},
     * {@link WigigManager#WIGIG_STATE_ENABLED},
     * {@link WigigManager#WIGIG_STATE_ENABLING},
     * {@link WigigManager#WIGIG_STATE_UNKNOWN}
     * {@link WigigManager#WIGIG_STATE_RATE_UPGRADE}
     */
    private final AtomicInteger mWigigState = new AtomicInteger(WIGIG_STATE_DISABLED);

    /**
     * One of  {@link WigigManager#WIGIG_AP_STATE_DISABLED},
     * {@link WigigManager#WIGIG_AP_STATE_DISABLING},
     * {@link WigigManager#WIGIG_AP_STATE_ENABLED},
     * {@link WigigManager#WIGIG_AP_STATE_ENABLING},
     * {@link WigigManager#WIGIG_AP_STATE_FAILED}
     */
    private final AtomicInteger mWigigApState = new AtomicInteger(WIGIG_AP_STATE_DISABLED);

    private final String mTcpBufferSizes;

    // we want WIFI to have higher priority for internet connectivity.
    // select a default score lower than WIFI but still higher than 4G.
    // Note, WIFI score can be dynamically reduced by bad WIFI link,
    // frequent WIFI disconnects etc. so it is theoretically possible
    // that WIGIG will get a higher score.
    private static int DEFAULT_SCORE = NetworkAgent.WIFI_BASE_SCORE - 5;

    private static int DEFAULT_MAX_NUM_STA = 8;

    public WigigStateMachine(Context context, String wlanInterface, WigigNetworkMonitor networkMonitor) {
        super("WigigStateMachine");
        mContext = context;
        mInterfaceName = wlanInterface;
        mDataInterfaceName = wlanInterface;
        mWigigNetworkMonitor = networkMonitor;

        mNetworkInfo = new NetworkInfo(ConnectivityManager.TYPE_WIFI, 0, NETWORKTYPE, "");

        IBinder b = ServiceManager.getService(Context.NETWORKMANAGEMENT_SERVICE);
        mNwService = INetworkManagementService.Stub.asInterface(b);

        // TODO do we need a separate feature flag for WIGIG P2P?
        mP2pSupported = mContext.getPackageManager().hasSystemFeature(
                PackageManager.FEATURE_WIFI_DIRECT);

        mWigigNative = new WigigNative(mInterfaceName);
        mWigigConfigStore = new WigigConfigStore(context, this, mWigigNative);
        mWigigMonitor = new WigigMonitor(this, mWigigNative);
        mWifiInfo = new WifiInfo();
        // TODO do we need this?
        mNetworkInfo.setDetailedState(DetailedState.DISCONNECTED, null, null);
        mLinkProperties = new LinkProperties();

        IBinder s1 = ServiceManager.getService(WigigP2pManager.WIGIG_P2P_SERVICE);
        mWigigP2pServiceImpl = (WigigP2pServiceImpl) IWigigP2pManager.Stub.asInterface(s1);

        mNetworkInfo.setIsAvailable(false);
        mLastBssid = null;
        mLastNetworkId = WifiConfiguration.INVALID_NETWORK_ID;
        mLastSignalLevel = -1;

        mIpManager = new IpManager(mContext, mDataInterfaceName, new IpManagerCallback());

        mPrimaryDeviceType = mContext.getResources().getString(
                R.string.config_wifi_p2p_device_type);

        mNetworkCapabilitiesFilter.addTransportType(NetworkCapabilities.TRANSPORT_WIFI);
        mNetworkCapabilitiesFilter.addCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET);
        mNetworkCapabilitiesFilter.addCapability(NetworkCapabilities.NET_CAPABILITY_NOT_RESTRICTED);
        mNetworkCapabilitiesFilter.setLinkUpstreamBandwidthKbps(3 * 1024 * 1024);
        mNetworkCapabilitiesFilter.setLinkDownstreamBandwidthKbps(3 * 1024 * 1024);
        mNetworkCapabilitiesFilter.setNetworkSpecifier(
            new StringNetworkSpecifier(NETWORK_SPECIFIER));
        // TODO - needs to be a bit more dynamic
        mNetworkCapabilities = new NetworkCapabilities(mNetworkCapabilitiesFilter);

        IntentFilter filter = new IntentFilter();
        filter.addAction(Intent.ACTION_SCREEN_ON);
        filter.addAction(Intent.ACTION_SCREEN_OFF);
        filter.addAction(ConnectivityManager.ACTION_TETHER_STATE_CHANGED);
        filter.addAction(WifiManager.WIFI_COUNTRY_CODE_CHANGED_ACTION);

        mContext.registerReceiver(
                new BroadcastReceiver() {
                    @Override
                    public void onReceive(Context context, Intent intent) {
                        String action = intent.getAction();
                        if (action.equals(ConnectivityManager.ACTION_TETHER_STATE_CHANGED)) {
                            ArrayList<String> available = intent.getStringArrayListExtra(
                                    ConnectivityManager.EXTRA_AVAILABLE_TETHER);
                            ArrayList<String> active = intent.getStringArrayListExtra(
                                    ConnectivityManager.EXTRA_ACTIVE_TETHER);
                            sendMessage(CMD_TETHER_STATE_CHANGE, new TetherStateChange(available, active));
                        } else if (action.equals(Intent.ACTION_SCREEN_ON) ||
                                action.equals(Intent.ACTION_SCREEN_OFF)) {
                            boolean screenOn = action.equals(Intent.ACTION_SCREEN_ON);
                            if (mScreenOn != screenOn) {
                                mScreenOn = screenOn;
                                sendMessage(CMD_SCREEN_STATE_CHANGED);
                            }
                        } else if (action.equals(WifiManager.WIFI_COUNTRY_CODE_CHANGED_ACTION)) {
                            sendMessage(CMD_COUNTRY_CODE_CHANGED,
                                    intent.getStringExtra(WifiManager.EXTRA_COUNTRY_CODE));
                        }
                    }
                }, filter);

        mScanResultCache = new LruCache<String, ScanResult>(SCAN_RESULT_CACHE_SIZE);
        PowerManager powerManager = (PowerManager) mContext.getSystemService(Context.POWER_SERVICE);
        mScreenOn = powerManager.isScreenOn();

        mWifiManager = (WifiManager) mContext.getSystemService(Context.WIFI_SERVICE);
        mWifiReceiver = new BroadcastReceiver() {
                @Override
                public void onReceive(Context context, Intent intent) {
                    String action = intent.getAction();
                    if (action.equals(WifiManager.WIFI_STATE_CHANGED_ACTION) && !isInitialStickyBroadcast()) {
                        int state = intent.getIntExtra(
                                WifiManager.EXTRA_WIFI_STATE,
                                WifiManager.WIFI_STATE_UNKNOWN);
                        if(state == WifiManager.WIFI_STATE_DISABLED || state == WifiManager.WIFI_STATE_ENABLED) {
                            log("Wifi State changed.");
                            sendMessage(CMD_RATE_UPGRADE_WIFI_STATE_CHANGED, state);
                        }
                    }
                    if (action.equals(WifiManager.WIFI_AP_STATE_CHANGED_ACTION) &&
                            !isInitialStickyBroadcast()) {
                        int state = intent.getIntExtra(WifiManager.EXTRA_WIFI_AP_STATE,
                                WifiManager.WIFI_AP_STATE_FAILED);
                        if (state == WifiManager.WIFI_AP_STATE_DISABLED
                                || state == WifiManager.WIFI_AP_STATE_ENABLED) {
                            log("SoftAp State changed.");
                            sendMessage(CMD_RATE_UPGRADE_WIFI_STATE_CHANGED, state);
                        }
                    }
                }
        };

        mTcpBufferSizes = SystemProperties.get(
            "persist.vendor.wigig.tcp_buffer_sizes", mContext.getResources().getString(
                com.android.internal.R.string.config_wifi_tcp_buffers));

        addState(mDefaultState);
            addState(mInitialState, mDefaultState);
            addState(mSupplicantStartingState, mDefaultState);
            addState(mSupplicantStartedState, mDefaultState);
                addState(mL2ConnectedState, mSupplicantStartedState);
                    addState(mObtainingIpState, mL2ConnectedState);
                    addState(mConnectedState, mL2ConnectedState);
                addState(mRadioInUseByP2pState, mSupplicantStartedState);
                addState(mDisconnectingState, mSupplicantStartedState);
                addState(mDisconnectedState, mSupplicantStartedState);
                addState(mWpsRunningState, mSupplicantStartedState);
            addState(mWaitForP2pDisableState, mDefaultState);
            addState(mSupplicantStoppingState, mDefaultState);
            addState(mRateUpgradeStartingState, mDefaultState);
            addState(mRateUpgradeState, mDefaultState);
            addState(mRateUpgradeStoppingState, mDefaultState);
            addState(mSoftApStartingState, mDefaultState);
            addState(mSoftApStartedState, mDefaultState);
                addState(mTetheringState, mSoftApStartedState);
                addState(mTetheredState, mSoftApStartedState);
                addState(mUntetheringState, mSoftApStartedState);

        if (SystemProperties.getInt("persist.vendor.fst.rate.upgrade.en", 0) == 1) {
            setFstPropEnabled(true); // sync persist.vendor.fst.softap.en with Wifi STA+SAP concurrency
            setInitialState(mRateUpgradeState);
        } else {
            setInitialState(mInitialState);
        }

        if (DBG) setDbg(true);

        //start the state machine
        start();
    }

    class IpManagerCallback extends IpManager.Callback {
        @Override
        public void onPreDhcpAction() {
            sendMessage(DhcpClient.CMD_PRE_DHCP_ACTION);
        }

        @Override
        public void onPostDhcpAction() {
            sendMessage(DhcpClient.CMD_POST_DHCP_ACTION);
        }

        @Override
        public void onNewDhcpResults(DhcpResults dhcpResults) {
            if (dhcpResults != null) {
                sendMessage(CMD_IPV4_PROVISIONING_SUCCESS, dhcpResults);
            } else {
                sendMessage(CMD_IPV4_PROVISIONING_FAILURE);
            }
        }

        @Override
        public void onProvisioningSuccess(LinkProperties newLp) {
            sendMessage(CMD_UPDATE_LINKPROPERTIES, newLp);
            sendMessage(CMD_IP_CONFIGURATION_SUCCESSFUL);
        }

        @Override
        public void onProvisioningFailure(LinkProperties newLp) {
            sendMessage(CMD_IP_CONFIGURATION_LOST);
        }

        @Override
        public void onLinkPropertiesChange(LinkProperties newLp) {
            sendMessage(CMD_UPDATE_LINKPROPERTIES, newLp);
        }
    }

    private void stopIpManager() {
        /* Restore power save and suspend optimizations */
        handlePostDhcpSetup();
        mIpManager.stop();
    }

    /*********************************************************
     * Methods exposed for public use
     ********************************************************/

    /**
     * Initiate a wigig scan.
     *
     */
    public void startScan() {
        log("startScan");

        sendMessage(CMD_START_SCAN, 0, 0, null);
    }

    private void handleScanRequest() {
        log("handleScanRequest");

        mWigigNative.scan(null);
    }

    /**
     * TODO: doc
     */
    public void setHostApRunning(WifiConfiguration wifiConfig, boolean enable) {
        if (enable) {
            sendMessage(CMD_START_AP, wifiConfig);
        } else {
            sendMessage(CMD_STOP_AP);
        }
    }

    public void setWigigApConfiguration(WifiConfiguration config) {
        mWigigApConfigChannel.sendMessage(CMD_SET_AP_CONFIG, config);
    }

    public WifiConfiguration syncGetWigigApConfiguration() {
        Message resultMsg = mWigigApConfigChannel.sendMessageSynchronously(CMD_REQUEST_AP_CONFIG);
        WifiConfiguration ret = (WifiConfiguration) resultMsg.obj;
        resultMsg.recycle();
        return ret;
    }

    /**
     * TODO: doc + sync
     */
    public int syncGetWigigState() {
        return mWigigState.get();
    }

    /**
     * TODO: doc
     */
    public String syncGetWigigStateByName() {
        switch (mWigigState.get()) {
            case WIGIG_STATE_DISABLED:
                return "disabled";
            case WIGIG_STATE_ENABLED:
                return "enabled";
            case WIGIG_STATE_RATE_UPGRADE:
                return "rate upgrade";
            case WIGIG_STATE_UNKNOWN:
                return "unknown state";
            default:
                return "[invalid state]";
        }
    }

    /**
     * TODO: doc
     */
    public int syncGetWigigApState() {
        return mWigigApState.get();
    }

    /**
     * TODO: doc
     */
    public String syncGetWigigApStateByName() {
        switch (mWigigApState.get()) {
            case WIGIG_AP_STATE_DISABLING:
                return "disabling";
            case WIGIG_AP_STATE_DISABLED:
                return "disabled";
            case WIGIG_AP_STATE_ENABLING:
                return "enabling";
            case WIGIG_AP_STATE_ENABLED:
                return "enabled";
            case WIGIG_AP_STATE_FAILED:
                return "failed";
            default:
                return "[invalid state]";
        }
    }

    /**
     * Get status information for the current connection, if any.
     *
     * @return a {@link WifiInfo} object containing information about the current connection
     */
    public WifiInfo syncRequestConnectionInfo() {
        return getWiFiInfoForUid(Binder.getCallingUid());
    }

    public void setSupplicantRunning(boolean enable) {
        log("setSupplicantRunning " + enable);

        if (enable) {
            sendMessage(CMD_START_SUPPLICANT);
        } else {
            sendMessage(CMD_STOP_SUPPLICANT);
        }
    }

    public void setRateUpgradeEnabled(boolean enable) {
        log("setRateUpgradeEnabled " + enable);

        if (enable) {
            sendMessage(CMD_START_RATE_UPGRADE);
        } else {
            sendMessage(CMD_STOP_RATE_UPGRADE);
        }
    }

    public List<ScanResult> syncGetScanResultsList() {
        synchronized (mScanResultCache) {
            List<ScanResult> scanList = new ArrayList<ScanResult>();
            for(ScanResult result: mScanResults) {
                scanList.add(new ScanResult(result));
            }

            log("syncGetScanResultsList, returning " + scanList.size() + " results");
            return scanList;
        }
    }

    /**
     * Disconnect from Access Point
     */
    public void disconnectCommand() {
        sendMessage(CMD_DISCONNECT);
    }

    /**
     * Get configured networks synchronously
     *
     * @param channel
     * @return
     */
    public List<WifiConfiguration> syncGetConfiguredNetworks(int uuid, AsyncChannel channel) {
        Message resultMsg = channel.sendMessageSynchronously(CMD_GET_CONFIGURED_NETWORKS, uuid);
        List<WifiConfiguration> result = (List<WifiConfiguration>) resultMsg.obj;
        resultMsg.recycle();
        return result;
    }

    /**
     * Returns the wigig configuration file
     */
    public String getConfigFile() {
        return mWigigConfigStore.getConfigFile();
    }

    /*********************************************************
     * Internal private functions
     ********************************************************/

    /**
     * Return the additional string to be logged by LogRec, default
     *
     * @param msg that was processed
     * @return information to be logged as a String
     */
    protected String getLogRecString(Message msg) {
        return smToString(msg.what);
    }

    String smToString(int what) {
        String s = "unknown";
        switch (what) {
            case AsyncChannel.CMD_CHANNEL_HALF_CONNECTED:
                s = "AsyncChannel.CMD_CHANNEL_HALF_CONNECTED";
                break;
            case AsyncChannel.CMD_CHANNEL_DISCONNECTED:
                s = "AsyncChannel.CMD_CHANNEL_DISCONNECTED";
                break;
            case CMD_OBTAINING_IP_ADDRESS_WATCHDOG_TIMER:
                s = "CMD_OBTAINING_IP_ADDRESS_WATCHDOG_TIMER";
                break;
            case CMD_STOP_SUPPLICANT:
                s = "CMD_STOP_SUPPLICANT";
                break;
            case CMD_STOP_SUPPLICANT_FAILED:
                s = "CMD_STOP_SUPPLICANT_FAILED";
                break;
            case CMD_START_SUPPLICANT:
                s = "CMD_START_SUPPLICANT";
                break;
            case CMD_START_AP:
                s = "CMD_START_AP";
                break;
            case CMD_START_AP_SUCCESS:
                s = "CMD_START_AP_SUCCESS";
                break;
            case CMD_START_AP_FAILURE:
                s = "CMD_START_AP_FAILURE";
                break;
            case CMD_STOP_AP:
                s = "CMD_STOP_AP";
                break;
            case CMD_SET_AP_CONFIG:
                s = "CMD_SET_AP_CONFIG";
                break;
            case CMD_SET_AP_CONFIG_COMPLETED:
                s = "CMD_SET_AP_CONFIG_COMPLETED";
                break;
            case CMD_REQUEST_AP_CONFIG:
                s = "CMD_REQUEST_AP_CONFIG";
                break;
            case CMD_RESPONSE_AP_CONFIG:
                s = "CMD_RESPONSE_AP_CONFIG";
                break;
            case CMD_TETHER_STATE_CHANGE:
                s = "CMD_TETHER_STATE_CHANGE";
                break;
            case CMD_TETHER_NOTIFICATION_TIMED_OUT:
                s = "CMD_TETHER_NOTIFICATION_TIMED_OUT";
                break;
            case WigigP2pServiceImpl.DISCONNECT_WIGIG_REQUEST:
                s = "WigigP2pServiceImpl.DISCONNECT_WIGIG_REQUEST";
                break;
            case WigigManager.DISABLE_NETWORK:
                s = "WigigManager.DISABLE_NETWORK";
                break;
            case DhcpClient.CMD_PRE_DHCP_ACTION:
                s = "CMD_PRE_DHCP_ACTION";
                break;
            case DhcpClient.CMD_POST_DHCP_ACTION:
                s = "CMD_POST_DHCP_ACTION";
                break;
            case DhcpClient.CMD_PRE_DHCP_ACTION_COMPLETE:
                s = "CMD_PRE_DHCP_ACTION_COMPLETE";
                break;
            case CMD_GET_CONFIGURED_NETWORKS:
                s = "CMD_GET_CONFIGURED_NETWORKS";
                break;
            case CMD_DISCONNECT:
                s = "CMD_DISCONNECT";
                break;
            case CMD_RSSI_POLL:
                s = "CMD_RSSI_POLL";
                break;
            case CMD_UPDATE_LINKPROPERTIES:
                s = "CMD_UPDATE_LINKPROPERTIES";
                break;
            case WigigManager.CONNECT_NETWORK:
                s = "CONNECT_NETWORK";
                break;
            case WigigManager.SAVE_NETWORK:
                s = "SAVE_NETWORK";
                break;
            case WigigManager.FORGET_NETWORK:
                s = "FORGET_NETWORK";
                break;
            case WigigMonitor.SUP_CONNECTION_EVENT:
                s = "SUP_CONNECTION_EVENT";
                break;
            case WigigMonitor.SUP_DISCONNECTION_EVENT:
                s = "SUP_DISCONNECTION_EVENT";
                break;
            case WigigMonitor.SCAN_RESULTS_EVENT:
                s = "SCAN_RESULTS_EVENT";
                break;
            case WigigMonitor.SCAN_FAILED_EVENT:
                s = "SCAN_FAILED_EVENT";
                break;
            case WigigMonitor.SUPPLICANT_STATE_CHANGE_EVENT:
                s = "SUPPLICANT_STATE_CHANGE_EVENT";
                break;
            case WigigMonitor.NETWORK_CONNECTION_EVENT:
                s = "NETWORK_CONNECTION_EVENT";
                break;
            case WigigMonitor.NETWORK_DISCONNECTION_EVENT:
                s = "NETWORK_DISCONNECTION_EVENT";
                break;
            case WigigMonitor.SSID_TEMP_DISABLED:
                s = "SSID_TEMP_DISABLED";
                break;
            case WigigMonitor.SSID_REENABLED:
                s = "SSID_REENABLED";
                break;
            case WigigMonitor.WPS_SUCCESS_EVENT:
                s = "WPS_SUCCESS_EVENT";
                break;
            case WigigMonitor.WPS_FAIL_EVENT:
                s = "WPS_FAIL_EVENT";
                break;
            case WigigMonitor.ASSOCIATION_REJECTION_EVENT:
                s = "ASSOCIATION_REJECTION_EVENT";
                break;
            case WigigMonitor.AUTHENTICATION_FAILURE_EVENT:
                s = "AUTHENTICATION_FAILURE_EVENT";
                break;
            case CMD_START_SCAN:
                s = "CMD_START_SCAN";
                break;
            case CMD_IP_CONFIGURATION_LOST:
                s = "CMD_IP_CONFIGURATION_LOST";
                break;
            case CMD_IP_CONFIGURATION_SUCCESSFUL:
                s = "CMD_IP_CONFIGURATION_SUCCESSFUL";
                break;
            case CMD_STATIC_IP_SUCCESS:
                s = "CMD_STATIC_IP_SUCCESSFUL";
                break;
            case CMD_STATIC_IP_FAILURE:
                s = "CMD_STATIC_IP_FAILURE";
                break;
            case CMD_ASSOCIATED_BSSID:
                s = "CMD_ASSOCIATED_BSSID";
                break;
            case CMD_DISABLE_P2P_RSP:
                s = "CMD_DISABLE_P2P_RSP";
                break;
            case CMD_DISABLE_P2P_REQ:
                s = "CMD_DISABLE_P2P_REQ";
                break;
            case WigigP2pServiceImpl.GROUP_CREATING_TIMED_OUT:
                s = "GROUP_CREATING_TIMED_OUT";
                break;
            case WigigP2pServiceImpl.P2P_CONNECTION_CHANGED:
                s = "P2P_CONNECTION_CHANGED";
                break;
            case WigigP2pServiceImpl.DISCONNECT_WIGIG_RESPONSE:
                s = "P2P.DISCONNECT_WIGIG_RESPONSE";
                break;
            case WigigP2pServiceImpl.SET_MIRACAST_MODE:
                s = "P2P.SET_MIRACAST_MODE";
                break;
            case WigigP2pServiceImpl.BLOCK_DISCOVERY:
                s = "P2P.BLOCK_DISCOVERY";
                break;
            case WigigManager.CANCEL_WPS:
                s = "CANCEL_WPS";
                break;
            case WigigManager.CANCEL_WPS_FAILED:
                s = "CANCEL_WPS_FAILED";
                break;
            case WigigManager.CANCEL_WPS_SUCCEDED:
                s = "CANCEL_WPS_SUCCEDED";
                break;
            case WigigManager.START_WPS:
                s = "START_WPS";
                break;
            case WigigManager.START_WPS_SUCCEEDED:
                s = "START_WPS_SUCCEEDED";
                break;
            case WigigManager.WPS_FAILED:
                s = "WPS_FAILED";
                break;
            case WigigManager.WPS_COMPLETED:
                s = "WPS_COMPLETED";
                break;
            case CMD_P2P_RELEASED_RADIO:
                s = "CMD_P2P_RELEASED_RADIO";
                break;
            case CMD_DISCONNECTING_WATCHDOG_TIMER:
                s = "CMD_DISCONNECTING_WATCHDOG_TIMER";
                break;
            case CMD_IPV4_PROVISIONING_SUCCESS:
                s = "CMD_IPV4_PROVISIONING_SUCCESS";
                break;
            case CMD_IPV4_PROVISIONING_FAILURE:
                s = "CMD_IPV4_PROVISIONING_FAILURE";
                break;
            case CMD_DISCONNECTED_PERIODIC_SCAN:
                s = "CMD_DISCONNECTED_PERIODIC_SCAN";
                break;
            case CMD_AIRPLANE_TOGGLED:
                s = "CMD_AIRPLANE_TOGGLED";
                break;
            case CMD_SCREEN_STATE_CHANGED:
                s = "CMD_SCREEN_STATE_CHANGED";
                break;
            case CMD_COUNTRY_CODE_CHANGED:
                s = "CMD_COUNTRY_CODE_CHANGED";
                break;
            default:
                s = "what:" + Integer.toString(what);
                break;
        }
        return s;
    }

    private void logStateAndMessage(Message message, String state) {
        if (mLogMessages)
            log(" " + state + " " + getLogRecString(message));
    }

    private void checkAndSetConnectivityInstance() {
        if (mCm == null) {
            mCm = (ConnectivityManager) mContext.getSystemService(Context.CONNECTIVITY_SERVICE);
        }
    }

    private boolean startTethering(ArrayList<String> available) {
        checkAndSetConnectivityInstance();

        String[] wifiRegexs = mCm.getTetherableWifiRegexs();

        for (String intf : available) {
            /* for Wigig, tethering interface is always the data interface */
            if (!intf.equals(mDataInterfaceName)) {
                continue;
            }
            for (String regex : wifiRegexs) {
                if (intf.matches(regex)) {
                    InterfaceConfiguration ifcg = null;
                    try {
                        ifcg = mNwService.getInterfaceConfig(intf);
                        if (ifcg != null) {
                            /* IP/netmask: 192.168.50.1/255.255.255.0 */
                            ifcg.setLinkAddress(new LinkAddress(
                                    NetworkUtils.numericToInetAddress("192.168.50.1"), 24));
                            ifcg.setInterfaceUp();

                            mNwService.setInterfaceConfig(intf, ifcg);
                        }
                    } catch (Exception e) {
                        loge("Error configuring interface " + intf + ", :" + e);
                        return false;
                    }

                    if (mCm.tether(intf) != ConnectivityManager.TETHER_ERROR_NO_ERROR) {
                        loge("Error tethering on " + intf);
                        return false;
                    }

                    return true;
                }
            }
        }
        // We found no interfaces to tether
        return false;
    }

    private void stopTethering() {
        checkAndSetConnectivityInstance();

        /* Clear the interface config to allow dhcp correctly configure new
           ip settings */
        InterfaceConfiguration ifcg = null;
        try {
            ifcg = mNwService.getInterfaceConfig(mDataInterfaceName);
            if (ifcg != null) {
                ifcg.setLinkAddress(
                        new LinkAddress(NetworkUtils.numericToInetAddress("0.0.0.0"), 0));
                mNwService.setInterfaceConfig(mDataInterfaceName, ifcg);
            }
        } catch (Exception e) {
            loge("Error resetting interface " + mDataInterfaceName + ", :" + e);
        }

        if (mCm.untether(mDataInterfaceName) != ConnectivityManager.TETHER_ERROR_NO_ERROR) {
            loge("Untether initiate failed!");
        }
    }

    private boolean isWigigTethered(ArrayList<String> active) {
        checkAndSetConnectivityInstance();

        String[] wifiRegexs = mCm.getTetherableWifiRegexs();
        for (String intf : active) {
            /* for Wigig, tethering interface is always the data interface */
            if (!intf.equals(mDataInterfaceName)) {
                continue;
            }
            for (String regex : wifiRegexs) {
                if (intf.matches(regex)) {
                    return true;
                }
            }
        }
        // We found no interfaces that are tethered
        return false;
    }

    private void setWigigState(int wigigState) {
        final int previousWigigState = mWigigState.get();

        mWigigState.set(wigigState);

        if (DBG) {
            log("setWigigState: " + syncGetWigigStateByName());
            log("sending WIGIG_STATE_CHANGED_ACTION, new state=" + wigigState);
        }
        final Intent intent = new Intent(WigigManager.WIGIG_STATE_CHANGED_ACTION);
        intent.addFlags(Intent.FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
        intent.putExtra(WigigManager.EXTRA_WIGIG_STATE, wigigState);
        intent.putExtra(WigigManager.EXTRA_PREVIOUS_WIGIG_STATE, previousWigigState);
        mContext.sendStickyBroadcastAsUser(intent, UserHandle.ALL);
    }

    private void setWigigApState(int wigigApState, int reason) {
        final int previousWigigApState = mWigigApState.get();

        // Update state
        mWigigApState.set(wigigApState);

        if (DBG) {
            log("setWigigApState: " + syncGetWigigApStateByName());
            log("sending WIGIG_AP_STATE_CHANGED_ACTION, new state=" + wigigApState);
        }

        final Intent intent = new Intent(WigigManager.WIGIG_AP_STATE_CHANGED_ACTION);
        intent.addFlags(Intent.FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
        intent.putExtra(WigigManager.EXTRA_WIGIG_AP_STATE, wigigApState);
        intent.putExtra(WigigManager.EXTRA_PREVIOUS_WIGIG_AP_STATE, previousWigigApState);
        if (wigigApState == WigigManager.WIGIG_AP_STATE_FAILED) {
            //only set reason number when softAP start failed
            intent.putExtra(WigigManager.EXTRA_WIGIG_AP_FAILURE_REASON, reason);
        }

        mContext.sendStickyBroadcastAsUser(intent, UserHandle.ALL);
    }

    public static InformationElement[] parseInformationElements(byte[] bytes) {
        final int BYTE_MASK = 0xff;
        final int EID_SSID = 0;

        if (bytes == null) {
            return new InformationElement[0];
        }
        ByteBuffer data = ByteBuffer.wrap(bytes).order(ByteOrder.LITTLE_ENDIAN);

        ArrayList<InformationElement> infoElements = new ArrayList<>();
        boolean found_ssid = false;
        while (data.remaining() > 1) {
            int eid = data.get() & BYTE_MASK;
            int elementLength = data.get() & BYTE_MASK;

            if (elementLength > data.remaining() || (eid == EID_SSID && found_ssid)) {
                // APs often pad the data with bytes that happen to match that of the EID_SSID
                // marker.  This is not due to a known issue for APs to incorrectly send the SSID
                // name multiple times.
                break;
            }
            if (eid == EID_SSID) {
                found_ssid = true;
            }

            InformationElement ie = new InformationElement();
            ie.id = eid;
            ie.bytes = new byte[elementLength];
            data.get(ie.bytes);
            infoElements.add(ie);
        }

        return infoElements.toArray(new InformationElement[infoElements.size()]);
    }

    private static final String IE_STR = "ie=";
    private static final String ID_STR = "id=";
    private static final String BSSID_STR = "bssid=";
    private static final String FREQ_STR = "freq=";
    private static final String LEVEL_STR = "level=";
    private static final String TSF_STR = "tsf=";
    private static final String FLAGS_STR = "flags=";
    private static final String SSID_STR = "ssid=";
    private static final String DELIMITER_STR = "====";
    private static final String END_STR = "####";

    int emptyScanResultCount = 0;

    // Used for matching BSSID strings, at least one character must be a non-zero number
    private static Pattern mNotZero = Pattern.compile("[1-9a-fA-F]");

    /**
     * Format:
     *
     * id=1
     * bssid=68:7f:76:d7:1a:6e
     * freq=2412
     * level=-44
     * tsf=1344626243700342
     * ie=00000
     * flags=[WPA2-PSK-CCMP][WPS][ESS]
     * ssid=ssid1
     * ====
     * id=2
     * bssid=68:5f:74:d7:1a:6f
     * freq=5180
     * level=-73
     * tsf=1344626243700373
     * ie=00000
     * flags=[WPA2-PSK-CCMP][WPS][ESS]
     * ssid=ssid2
     * ====
     */
    private void setScanResults() {
        String bssid = "";
        int level = 0;
        int freq = 0;
        long tsf = 0;
        String flags = "";
        WigigSsid wigigSsid = null;
        String scanResults;
        String tmpResults;
        StringBuffer scanResultsBuf = new StringBuffer();
        int sid = 0;

        while (true) {
            tmpResults = mWigigNative.scanResults(sid);
            if (TextUtils.isEmpty(tmpResults)) break;
            scanResultsBuf.append(tmpResults);
            scanResultsBuf.append("\n");
            String[] lines = tmpResults.split("\n");
            sid = -1;
            for (int i=lines.length - 1; i >= 0; i--) {
                if (lines[i].startsWith(END_STR)) {
                    break;
                } else if (lines[i].startsWith(ID_STR)) {
                    try {
                        sid = Integer.parseInt(lines[i].substring(ID_STR.length())) + 1;
                    } catch (NumberFormatException e) {
                        // Nothing to do
                    }
                    break;
                }
            }
            if (sid == -1) break;
        }

        scanResults = scanResultsBuf.toString();
        if (TextUtils.isEmpty(scanResults)) {
            emptyScanResultCount++;
            if (emptyScanResultCount > 10) {
                // If we got too many empty scan results, the current scan cache is stale,
                // hence clear it.
                mScanResults = new ArrayList<ScanResult>();
            }
           return;
        }

        emptyScanResultCount = 0;

        // note that all these splits and substrings keep references to the original
        // huge string buffer while the amount we really want is generally pretty small
        // so make copies instead (one example b/11087956 wasted 400k of heap here).
        synchronized(mScanResultCache) {
            mScanResults = new ArrayList<ScanResult>();
            String[] lines = scanResults.split("\n");
            final int bssidStrLen = BSSID_STR.length();
            final int flagLen = FLAGS_STR.length();
            String infoElements = null;

            for (String line : lines) {
                if (line.startsWith(BSSID_STR)) {
                    bssid = new String(line.getBytes(), bssidStrLen, line.length() - bssidStrLen);
                } else if (line.startsWith(FREQ_STR)) {
                    try {
                        freq = Integer.parseInt(line.substring(FREQ_STR.length()));
                    } catch (NumberFormatException e) {
                        freq = 0;
                    }
                } else if (line.startsWith(LEVEL_STR)) {
                    try {
                        /* level is expected in the range of 0..100 */
                        level = Integer.parseInt(line.substring(LEVEL_STR.length()));
                    } catch(NumberFormatException e) {
                        level = 0;
                    }
                } else if (line.startsWith(TSF_STR)) {
                    try {
                        tsf = Long.parseLong(line.substring(TSF_STR.length()));
                    } catch (NumberFormatException e) {
                        tsf = 0;
                    }
                } else if (line.startsWith(FLAGS_STR)) {
                    flags = new String(line.getBytes(), flagLen, line.length() - flagLen);
                } else if (line.startsWith(SSID_STR)) {
                    wigigSsid = WigigSsid.createFromAsciiEncoded(
                            line.substring(SSID_STR.length()));
                } else if (line.startsWith(IE_STR)) {
                    infoElements = line;
                } else if (line.startsWith(DELIMITER_STR) || line.startsWith(END_STR)) {
                    Matcher match = null;
                    if (bssid!= null) {
                        match = mNotZero.matcher(bssid);
                    }
                    if (match != null && !bssid.isEmpty() && match.find()) {
                        String ssid = (wigigSsid != null) ? wigigSsid.toString() : WigigSsid.NONE;
                        String key = bssid + ssid;
                        ScanResult scanResult = mScanResultCache.get(key);
                        if (scanResult != null) {
                            // TODO: average the RSSI, instead of overwriting it
                            scanResult.level = level;
                            scanResult.wigigSsid = wigigSsid;
                            // Keep existing API
                            scanResult.SSID = (wigigSsid != null) ? wigigSsid.toString() :
                                    WigigSsid.NONE;
                            scanResult.capabilities = flags;
                            scanResult.frequency = freq;
                            scanResult.timestamp = tsf;
                            scanResult.seen = System.currentTimeMillis();
                        } else {
                            scanResult =
                                new ScanResult(
                                        wigigSsid, bssid, flags, level, freq, tsf);
                            scanResult.seen = System.currentTimeMillis();
                            mScanResultCache.put(key, scanResult);
                        }

                        if (infoElements != null) {
                            byte[] iesAsByteArray = HexDump.hexStringToByteArray(infoElements.substring(IE_STR.length()));
                            scanResult.informationElements = parseInformationElements(iesAsByteArray);
                        }

                        mScanResults.add(scanResult);
                    } else {
                        if (bssid != null)  {
                            loge("setScanResults obtaining null BSSID results <"
                                + bssid + ">, discard it");
                        }
                    }
                    bssid = null;
                    level = 0;
                    freq = 0;
                    tsf = 0;
                    flags = "";
                    wigigSsid = null;
                    infoElements = null;
                }
            }
        }
    }

    /*
     * Fetch RSSI on current connection
     */
    private void fetchRssiNative() {
        Integer newLevel = null;
        String signalPoll = mWigigNative.signalPoll();

        if (signalPoll == null) {
            return;
        }

        String[] lines = signalPoll.split("\n");
        for (String line : lines) {
            String[] prop = line.split("=");
            if (prop.length < 2) continue;
            try {
                if (prop[0].equals("RSSI")) {
                    newLevel = Integer.parseInt(prop[1]);
                    if (newLevel == -9999)
                        newLevel = 0;
                }
            } catch (NumberFormatException e) {
                //Ignore, defaults are assigned
            }
        }

        if (DBG) {
            logd("fetchRssiNative level=" + newLevel);
        }

        if (newLevel != null) {
            /*
             * Rather then sending the raw RSSI out every time it
             * changes, we precalculate the signal level that would
             * be displayed in the status bar, and only send the
             * broadcast if that much more coarse-grained number
             * changes. This cuts down greatly on the number of
             * broadcasts, at the cost of not informing others
             * interested in RSSI of all the changes in signal
             * level.
             */
            int newSignalLevel = WigigManager.calculateSignalLevel(newLevel,
                    WigigManager.RSSI_LEVELS);
            if (newSignalLevel != mLastSignalLevel) {
                updateCapabilities(getCurrentWifiConfiguration());
                sendSignalLevelChangeBroadcast(newLevel);
            }
            mLastSignalLevel = newSignalLevel;
        } else {
            updateCapabilities(getCurrentWifiConfiguration());
        }
    }


    private void updateLinkProperties(LinkProperties newLp) {
        if (DBG) {
            log("Link configuration changed for netId: " + mLastNetworkId
                    + " old: " + mLinkProperties + " new: " + newLp);
        }
        // We own this instance of LinkProperties because IpManager passes us a copy.
        mLinkProperties = newLp;
        if (mNetworkAgent != null) {
            mNetworkAgent.sendLinkProperties(mLinkProperties);
        }

        if (getNetworkDetailedState() == DetailedState.CONNECTED) {
            // If anything has changed and we're already connected, send out a notification.
            // TODO: Update all callers to use NetworkCallbacks and delete this.
            sendLinkConfigurationChangedBroadcast();
        }

        if (DBG) {
            StringBuilder sb = new StringBuilder();
            sb.append("updateLinkProperties nid: " + mLastNetworkId);
            sb.append(" state: " + getNetworkDetailedState());

            if (mLinkProperties != null) {
                sb.append(" ");
                sb.append(getLinkPropertiesSummary(mLinkProperties));
            }
            logd(sb.toString());
        }
    }

    /**
     * Clears all our link properties.
     */
    private void clearLinkProperties() {
        // Clear the link properties obtained from DHCP. The only caller of this
        // function has already called IpManager#stop(), which clears its state.
        synchronized (mDhcpResultsLock) {
            if (mDhcpResults != null) {
                mDhcpResults.clear();
            }
        }

        // Now clear the merged link properties.
        mLinkProperties.clear();
        if (mNetworkAgent != null) mNetworkAgent.sendLinkProperties(mLinkProperties);
    }

    private void sendScanResultsAvailableBroadcast() {
        log("sending SCAN_RESULTS_AVAILABLE_ACTION");
        Intent intent = new Intent(WigigManager.SCAN_RESULTS_AVAILABLE_ACTION);
        intent.addFlags(Intent.FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
        mContext.sendBroadcastAsUser(intent, UserHandle.ALL);
    }

    private void sendSignalLevelChangeBroadcast(final int newLevel) {
        log("sending RSSI_CHANGED_ACTION");
        Intent intent = new Intent(WigigManager.RSSI_CHANGED_ACTION);
        intent.addFlags(Intent.FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
        intent.putExtra(WigigManager.EXTRA_NEW_RSSI, newLevel);
        mContext.sendStickyBroadcastAsUser(intent, UserHandle.ALL);
    }

    private void sendNetworkStateChangeBroadcast(String bssid) {
        Intent intent = new Intent(WigigManager.NETWORK_STATE_CHANGED_ACTION);
        intent.addFlags(Intent.FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
        intent.putExtra(WigigManager.EXTRA_NETWORK_INFO, new NetworkInfo(mNetworkInfo));
        intent.putExtra(WigigManager.EXTRA_NETWORK_ID, mLastNetworkId);
        intent.putExtra(WigigManager.EXTRA_LINK_PROPERTIES, new LinkProperties(mLinkProperties));
        if (bssid != null)
            intent.putExtra(WigigManager.EXTRA_BSSID, bssid);
        if (mNetworkInfo.getDetailedState() == DetailedState.CONNECTED) {
            // We no longer report MAC address to third-parties and our code does
            // not rely on this broadcast, so just send the default MAC address.
            fetchRssiNative();
            WifiInfo sentWifiInfo = new WifiInfo(mWifiInfo);
            sentWifiInfo.setMacAddress(WifiInfo.DEFAULT_MAC_ADDRESS);
            intent.putExtra(WigigManager.EXTRA_WIFI_INFO, sentWifiInfo);
        }
        log("sending NETWORK_STATE_CHANGED_ACTION, bssid=" + bssid
            + " state=" + mNetworkInfo.getDetailedState());
        mContext.sendStickyBroadcastAsUser(intent, UserHandle.ALL);
    }

    private WifiInfo getWiFiInfoForUid(int uid) {
        if (Binder.getCallingUid() == Process.myUid()) {
            return mWifiInfo;
        }

        WifiInfo result = new WifiInfo(mWifiInfo);
        result.setMacAddress(WifiInfo.DEFAULT_MAC_ADDRESS);

        IBinder binder = ServiceManager.getService("package");
        IPackageManager packageManager = IPackageManager.Stub.asInterface(binder);

        try {
            if (packageManager.checkUidPermission(Manifest.permission.LOCAL_MAC_ADDRESS,
                    uid) == PackageManager.PERMISSION_GRANTED) {
                result.setMacAddress(mWifiInfo.getMacAddress());
            }
        } catch (RemoteException e) {
            loge("Error checking receiver permission", e);
        }

        return result;
    }

    private void sendLinkConfigurationChangedBroadcast() {
        log("sending LINK_CONFIGURATION_CHANGED_ACTION");
        Intent intent = new Intent(WigigManager.LINK_CONFIGURATION_CHANGED_ACTION);
        intent.addFlags(Intent.FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
        intent.putExtra(WigigManager.EXTRA_LINK_PROPERTIES, new LinkProperties(mLinkProperties));
        mContext.sendBroadcastAsUser(intent, UserHandle.ALL);
    }

    private void sendSupplicantConnectionChangedBroadcast(boolean connected) {
        log("sending SUPPLICANT_CONNECTION_CHANGE_ACTION, connected=" + connected);
        Intent intent = new Intent(WigigManager.SUPPLICANT_CONNECTION_CHANGE_ACTION);
        intent.addFlags(Intent.FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
        intent.putExtra(WigigManager.EXTRA_SUPPLICANT_CONNECTED, connected);
        mContext.sendBroadcastAsUser(intent, UserHandle.ALL);
    }

    /**
     * Record the detailed state of a network.
     *
     * @param state the new {@code DetailedState}
     */
    private boolean setNetworkDetailedState(NetworkInfo.DetailedState state) {
        if (DBG) {
            log("setDetailed state, old ="
                    + mNetworkInfo.getDetailedState() + " and new state=" + state
            );
        }

        if (mNetworkInfo.getExtraInfo() != null && mWifiInfo.getSSID() != null) {
            // Always indicate that SSID has changed
            if (!mNetworkInfo.getExtraInfo().equals(mWifiInfo.getSSID())) {
                if (DBG) {
                    log("setDetailed state send new extra info" + mWifiInfo.getSSID());
                }
                mNetworkInfo.setExtraInfo(mWifiInfo.getSSID());
                sendNetworkStateChangeBroadcast(null);
            }
        }

        if (state != mNetworkInfo.getDetailedState()) {
            mNetworkInfo.setDetailedState(state, null, mWifiInfo.getSSID());
            if (mNetworkAgent != null) {
                mNetworkAgent.sendNetworkInfo(mNetworkInfo);
            }
            sendNetworkStateChangeBroadcast(null);
            return true;
        }
        return false;
    }

    private DetailedState getNetworkDetailedState() {
        return mNetworkInfo.getDetailedState();
    }

    private SupplicantState handleSupplicantStateChange(Message message) {
        StateChangeResult stateChangeResult = (StateChangeResult) message.obj;
        SupplicantState state = stateChangeResult.state;
        if (DBG) log("handleSupplicantStateChange, old=" + mWifiInfo.getSupplicantState()
            + " new=" + state);

        // Supplicant state change
        // [31-13] Reserved for future use
        // [8 - 0] Supplicant state (as defined in SupplicantState.java)
        // 50023 supplicant_state_changed (custom|1|5)
        mWifiInfo.setSupplicantState(state);
        // Network id is only valid when we start connecting
        if (SupplicantState.isConnecting(state)) {
            mWifiInfo.setNetworkId(stateChangeResult.networkId);
        } else {
            mWifiInfo.setNetworkId(WifiConfiguration.INVALID_NETWORK_ID);
        }

        mWifiInfo.setBSSID(stateChangeResult.BSSID);
        mWifiInfo.setSSID(stateChangeResult.wifiSsid);

        if (DBG) log("handleSupplicantStateChange, nid=" + mWifiInfo.getNetworkId()
            + " bssid=" + mWifiInfo.getBSSID()
            + " ssid=" + mWifiInfo.getSSID());

        return state;
    }

    /**
     * Resets the Wigig Connections by clearing the interface
     */
    private void handleNetworkDisconnect() {
        if (DBG) log("handleNetworkDisconnect: Stopping DHCP and clearing IP");

        stopIpManager();

        mWifiInfo.reset();

        setNetworkDetailedState(DetailedState.DISCONNECTED);
        if (mNetworkAgent != null) {
            mNetworkAgent.sendNetworkInfo(mNetworkInfo);
            mNetworkAgent = null;
        }
        mWigigConfigStore.updateStatus(mLastNetworkId, DetailedState.DISCONNECTED);

        /* Clear network properties */
        clearLinkProperties();

        /* Send event to CM & network change broadcast */
        // TODO: remove this because setNetworkDetailedState above already cover it
        sendNetworkStateChangeBroadcast(mLastBssid);

        mLastBssid = null;
        mLastNetworkId = WifiConfiguration.INVALID_NETWORK_ID;
    }

    private void handleSupplicantConnectionLoss() {
        /* Socket connection can be lost when we do a graceful shutdown
         * or when the driver is hung. Ensure supplicant is stopped here.
         */
        mWigigMonitor.killSupplicant();
        sendSupplicantConnectionChangedBroadcast(false);
        setWigigState(WIGIG_STATE_DISABLED);
    }

    void handlePreDhcpSetup() {
        /* P2p discovery breaks dhcp, shut it down in order to get through this */
        Message msg = new Message();
        msg.what = WigigP2pServiceImpl.BLOCK_DISCOVERY;
        msg.arg1 = WigigP2pServiceImpl.ENABLED;
        msg.arg2 = DhcpClient.CMD_PRE_DHCP_ACTION_COMPLETE;
        msg.obj = WigigStateMachine.this;
        mWigigP2pChannel.sendMessage(msg);
    }

    void handlePostDhcpSetup() {
        mWigigP2pChannel.sendMessage(WigigP2pServiceImpl.BLOCK_DISCOVERY, WigigP2pServiceImpl.DISABLED);
    }

    private void handleIPv4Success(DhcpResults dhcpResults) {
        if (DBG) {
            logd("handleIPv4Success <" + dhcpResults.toString() + ">");
            logd("link address " + dhcpResults.ipAddress);
        }

        Inet4Address addr;
        synchronized (mDhcpResultsLock) {
            mDhcpResults = dhcpResults;
            addr = (Inet4Address) dhcpResults.ipAddress.getAddress();
        }

        mWifiInfo.setInetAddress(addr);
        mWifiInfo.setMeteredHint(dhcpResults.hasMeteredHint());
    }

    private void handleSuccessfulIpConfiguration() {
        mLastSignalLevel = -1; // Force update of signal strength
        WifiConfiguration c = getCurrentWifiConfiguration();
        if (c != null) {
            // Tell the framework whether the newly connected network is trusted or untrusted.
            updateCapabilities(c);
        }
    }

    private void handleIPv4Failure() {
        synchronized(mDhcpResultsLock) {
             if (mDhcpResults != null) {
                 mDhcpResults.clear();
             }
        }
        if (DBG) {
            logd("handleIPv4Failure");
        }
    }

    private void handleIpConfigurationLost() {
        mWifiInfo.setInetAddress(null);
        mWifiInfo.setMeteredHint(false);

        /* DHCP times out after about 30 seconds, we do disable/enable in order to disconnect thru
         * supplicant. supplicant will then retry connect to same/other networks
         */
        mWigigNative.disableNetwork(mLastNetworkId);
        mWigigNative.enableNetwork(mLastNetworkId, false);
    }

    private void sendP2pRadioStateUpdate(int state)
    {
        if (DBG) log("sendP2pRadioStateUpdate: " + state);
        mWigigP2pChannel.sendMessage(CMD_P2P_RADIO_STATE_UPDATE, state);
    }

    private void setFstPropEnabled(boolean enabled) {
        String status = enabled ? "1" : "0";
        if (DBG) log((enabled ? "Enabling" : "Disabling") + " fst_prop. WifiStaSapConcurrency=" + mWifiManager.getWifiStaSapConcurrency());

        // we load the driver whenever we enable FST, this is done before enabling WIFI so
        // driver will be operational when WIFI services start (supplicant/hostapd/fstman)
        // driver is unloaded when re-entering InitialState
        if (enabled) {
            if (!mWigigNative.loadDriver()) {
                loge("failed to load driver for FST");
                // continue anyway, FST may not work
            }
        }

        SystemProperties.set("persist.vendor.fst.rate.upgrade.en", status);
        /* in case Wifi STA+SAP concurrency enabled, FST AP must be disabled as Wigig doesn't
         * support concurrency
         */
        if (mWifiManager.getWifiStaSapConcurrency()) {
            SystemProperties.set("persist.vendor.fst.softap.en", "0");
        } else {
            SystemProperties.set("persist.vendor.fst.softap.en", status);
        }
    }

    /* Current design is to not set the config on a running hostapd but instead
     * stop and start tethering when user changes config on a running access point
     *
     * TODO: Add control channel setup through hostapd that allows changing config
     * on a running daemon
     */
    private void startSoftApWithConfig(final WifiConfiguration configuration) {
        final WifiConfiguration config = new WifiConfiguration(configuration);

        if (DBG) {
            log("SoftAp config: channel=" + config.apChannel +
                " ssid=" + config.SSID +
                " config.hiddenSSID=" + config.hiddenSSID +
                " secured=" + config.allowedKeyManagement.get(WifiConfiguration.KeyMgmt.WPA_PSK));
        }

        // no HAL, use default channel
        if (config.apChannel == 0) {
            config.apChannel = 2;
        }

        // Start hostapd on a separate thread
        new Thread(new Runnable() {
            public void run() {
                boolean res;
                int maxNumSta = SystemProperties.getInt("persist.vendor.wigig.max_num_sta",
                        DEFAULT_MAX_NUM_STA);
                String countryCode = null;

                if (!mWifiManager.isWifiEnabled() && !mWifiManager.isWifiApEnabled()) {
                    countryCode = mWifiManager.getCountryCode();
                    logd("current country code is " + countryCode);
                }

                res = mWigigNative.setSoftAp(mInterfaceName, config.SSID, config.hiddenSSID,
                        config.apChannel,
                        config.allowedKeyManagement.get(WifiConfiguration.KeyMgmt.WPA_PSK),
                        config.preSharedKey, maxNumSta, countryCode);

                if (!res) {
                    if (DBG) loge("setSoftAp failed");
                    sendMessage(CMD_START_AP_FAILURE, WifiManager.SAP_START_FAILURE_GENERAL);
                    return;
                }

                String triggers = SystemProperties.get("persist.vendor.wigig.sap.wowlan_triggers", "any");
                // continue even if we fail to set wowlan_triggers, this is not critical
                // empty value means remove all triggers
                mWigigNative.setHostapdConfigWowlanTriggers(triggers);

                res = mWigigNative.startHostapd();
                if (!res) {
                    if (DBG) loge("startHostapd failed");
                    sendMessage(CMD_START_AP_FAILURE, WifiManager.SAP_START_FAILURE_GENERAL);
                    return;
                }

                if (DBG) log("Soft AP start successful");
                sendMessage(CMD_START_AP_SUCCESS);
            }
        }).start();
    }

    /********************************************************
     * HSM states
     *******************************************************/

    class DefaultState extends State {
        @Override
        public boolean processMessage(Message message) {
            logStateAndMessage(message, getClass().getSimpleName());

            switch (message.what) {
                case AsyncChannel.CMD_CHANNEL_HALF_CONNECTED: {
                    AsyncChannel ac = (AsyncChannel) message.obj;
                    if (ac == mWigigP2pChannel) {
                        if (message.arg1 == AsyncChannel.STATUS_SUCCESSFUL) {
                            mWigigP2pChannel.sendMessage(AsyncChannel.CMD_CHANNEL_FULL_CONNECTION);
                        } else {
                            loge("WigigP2pService connection failure, error=" + message.arg1);
                        }
                    } else {
                        loge("got HALF_CONNECTED for unknown channel");
                    }
                    break;
                }
                case AsyncChannel.CMD_CHANNEL_DISCONNECTED: {
                    AsyncChannel ac = (AsyncChannel) message.obj;
                    if (ac == mWigigP2pChannel) {
                        loge("WigigP2pService channel lost, message.arg1 =" + message.arg1);
                        //TODO: Re-establish connection to state machine after a delay
                        // mWigigP2pChannel.connect(mContext, getHandler(),
                        // mWigigP2pManager.getMessenger());
                    }
                    break;
                }
                case CMD_P2P_REQUEST_RADIO:
                    sendP2pRadioStateUpdate(P2P_RADIO_STATE_IN_USE_BY_WIGIG);
                    break;
                case WigigManager.CONNECT_NETWORK:
                    replyToMessage(message, WigigManager.CONNECT_NETWORK_FAILED,
                            WigigManager.BUSY);
                    break;
                case WigigManager.FORGET_NETWORK:
                    replyToMessage(message, WigigManager.FORGET_NETWORK_FAILED,
                            WigigManager.BUSY);
                    break;
                case WigigManager.SAVE_NETWORK:
                    replyToMessage(message, WigigManager.SAVE_NETWORK_FAILED,
                            WigigManager.BUSY);
                    break;
                case WigigManager.START_WPS:
                    replyToMessage(message, WigigManager.WPS_FAILED,
                            WigigManager.BUSY);
                    break;
                case WigigManager.CANCEL_WPS:
                    replyToMessage(message, WigigManager.CANCEL_WPS_FAILED,
                            WigigManager.BUSY);
                    break;
                case CMD_GET_CONFIGURED_NETWORKS:
                    replyToMessage(message, message.what, (List<WifiConfiguration>) null);
                    break;
                /* Link configuration (IP address, DNS, ...) changes notified via netlink */
                case CMD_UPDATE_LINKPROPERTIES:
                    updateLinkProperties((LinkProperties) message.obj);
                    break;
                default:
                    loge("Error! unhandled message" + message);
                    break;
            }
            return HANDLED;
        }
    }

    class InitialState extends State {
        @Override
        public void enter() {
             mWigigNative.unloadDriver();
             if (mWigigP2pChannel == null) {
                 mWigigP2pChannel = new AsyncChannel();
                 mWigigP2pChannel.connect(mContext, getHandler(),
                     mWigigP2pServiceImpl.getP2pStateMachineMessenger());
             }

            if (mWigigApConfigChannel == null) {
                mWigigApConfigChannel = new AsyncChannel();
                mWigigApConfigStore = WigigApConfigStore.makeWigigApConfigStore(
                        mContext, getHandler());
                mWigigApConfigStore.loadApConfiguration();
                mWigigApConfigChannel.connectSync(mContext, getHandler(),
                        mWigigApConfigStore.getMessenger());
            }
        }

        @Override
        public boolean processMessage(Message message) {
            logStateAndMessage(message, getClass().getSimpleName());

            switch (message.what) {
                case CMD_START_SUPPLICANT:
                    /* Stop a running supplicant after a runtime restart
                     * Avoids issues with drivers that do not handle interface down
                     * on a running supplicant properly.
                     */
                    mWigigNative.killSupplicant();
                    if (mWigigNative.loadDriver()) {
                        try {
                            // A runtime crash can leave the interface up and
                            // IP addresses configured, and this affects
                            // connectivity when supplicant starts up.
                            // Ensure interface is down and we have no IP
                            // addresses before a supplicant start.
                            mNwService.setInterfaceDown(mInterfaceName);
                            mNwService.clearInterfaceAddresses(mDataInterfaceName);

                            // Set privacy extensions
                            mNwService.setInterfaceIpv6PrivacyExtensions(mDataInterfaceName, true);

                            // IPv6 is enabled only as long as access point is connected since:
                            // - IPv6 addresses and routes stick around after disconnection
                            // - kernel is unaware when connected and fails to start IPv6 negotiation
                            // - kernel can start autoconfiguration when 802.1x is not complete
                            mNwService.disableIpv6(mDataInterfaceName);
                        } catch (RemoteException re) {
                            loge("Unable to change interface settings: " + re);
                        } catch (IllegalStateException ie) {
                            loge("Unable to change interface settings: " + ie);
                        }

                        mWigigNetworkMonitor.updateVRProfile();

                        if(mWigigNative.startSupplicant()) {
                            if (DBG) log("Supplicant start successful");
                            mWigigMonitor.startMonitoring();
                            transitionTo(mSupplicantStartingState);
                        }
                        else {
                            loge("Failed to start supplicant!");
                            setWigigState(WIGIG_STATE_FAILED);
                        }
                    }
                    else {
                        loge("Failed to load driver");
                        setWigigState(WIGIG_STATE_FAILED);
                    }

                    break;
                case CMD_START_RATE_UPGRADE:
                    transitionTo(mRateUpgradeStartingState);
                    break;
                case CMD_START_AP:
                    if (mWigigNative.loadDriver() == false) {
                        loge("Failed to load driver for softap");
                    } else {
                        // we skip enableSoftAp() as we don't have HAL
                        setWigigApState(WIGIG_AP_STATE_ENABLING, 0);
                        transitionTo(mSoftApStartingState);
                    }
                    break;
                case CMD_AIRPLANE_TOGGLED:
                    boolean newWigigMode = (message.arg1 == 1);
                    // message.arg2 contains airplaneModeOn which is irrelvant in this state

                    log("newWigigMode=" + newWigigMode);

                    if (newWigigMode) {
                        log("enabling wigig due to airplane mode change");
                        setSupplicantRunning(true);
                    }
                    break;
                default:
                    return NOT_HANDLED;
            }
            return HANDLED;
        }
    }

    class SupplicantStartingState extends State {
        private void initializeWpsDetails() {
            String detail;
            detail = SystemProperties.get("ro.product.name", "");
            if (!mWigigNative.setDeviceName(detail)) {
                loge("Failed to set device name " +  detail);
            }
            detail = SystemProperties.get("ro.product.manufacturer", "");
            if (!mWigigNative.setManufacturer(detail)) {
                loge("Failed to set manufacturer " + detail);
            }
            detail = SystemProperties.get("ro.product.model", "");
            if (!mWigigNative.setModelName(detail)) {
                loge("Failed to set model name " + detail);
            }
            detail = SystemProperties.get("ro.product.model", "");
            if (!mWigigNative.setModelNumber(detail)) {
                loge("Failed to set model number " + detail);
            }
            detail = SystemProperties.get("ro.serialno", "");
            if (!mWigigNative.setSerialNumber(detail)) {
                loge("Failed to set serial number " + detail);
            }
            if (!mWigigNative.setConfigMethods("physical_display virtual_push_button keypad")) {
                loge("Failed to set WPS config methods");
            }
            if (!mWigigNative.setDeviceType(mPrimaryDeviceType)) {
                loge("Failed to set primary device type " + mPrimaryDeviceType);
            }
        }

        private void initializeWowlanTriggers() {
            String triggers = SystemProperties.get("persist.vendor.wigig.sta.wowlan_triggers", "any");
            // continue even if we fail to set wowlan_triggers, this is not critical
            // empty value means remove all triggers
            mWigigNative.setWowlanTriggers(triggers);
        }

        @Override
        public boolean processMessage(Message message) {
            logStateAndMessage(message, getClass().getSimpleName());

            switch(message.what) {
                case WigigMonitor.SUP_CONNECTION_EVENT:
                    if (DBG) log("Supplicant connection established");
                    setWigigState(WIGIG_STATE_ENABLED);
                    /* Initialize data structures */
                    mLastBssid = null;
                    mLastNetworkId = WifiConfiguration.INVALID_NETWORK_ID;
                    mLastSignalLevel = -1;

                    mWifiInfo.setMacAddress(mWigigNative.getMacAddress());

                    mWigigNative.enableSaveConfig();
                    mWigigConfigStore.loadAndEnableAllNetworks();
                    try {
                        mNwService.setInterfaceUp(mInterfaceName);
                    } catch (RemoteException re) {
                        loge("Unable to change interface settings: " + re);
                    } catch (IllegalStateException ie) {
                        loge("Unable to change interface settings: " + ie);
                    }

                    initializeWpsDetails();
                    initializeWowlanTriggers();

                    sendSupplicantConnectionChangedBroadcast(true);
                    transitionTo(mDisconnectedState);
                    break;
                case WigigMonitor.SUP_DISCONNECTION_EVENT:
                    loge("Failed to start supplicant, unload driver");
                    try {
                        mNwService.setInterfaceDown(mInterfaceName);
                    } catch (RemoteException re) {
                        loge("Unable to change interface settings: " + re);
                    } catch (IllegalStateException ie) {
                        loge("Unable to change interface settings: " + ie);
                    }
                    setWigigState(WIGIG_STATE_UNKNOWN);
                    transitionTo(mInitialState);
                    break;
                case CMD_START_SUPPLICANT:
                case CMD_STOP_SUPPLICANT:
                case CMD_START_AP:
                case CMD_STOP_AP:
                case CMD_AIRPLANE_TOGGLED:
                case CMD_COUNTRY_CODE_CHANGED:
                    deferMessage(message);
                    break;
                default:
                    return NOT_HANDLED;
            }
            return HANDLED;
        }
    }

    class SupplicantStartedState extends State {
        @Override
        public void enter() {
            /* Wigig is available as long as we have a connection to supplicant */
            mNetworkInfo.setIsAvailable(true);
            if (mNetworkAgent != null) mNetworkAgent.sendNetworkInfo(mNetworkInfo);

            /* TODO: get it from Settings.Global.WIGIG_SUPPLICANT_SCAN_INTERVAL_MS */
            mSupplicantScanIntervalMs = 10000;

            /* initialize network state */
            setNetworkDetailedState(DetailedState.DISCONNECTED);

            if (mP2pSupported) {
                    mWigigP2pChannel.sendMessage(WigigStateMachine.CMD_ENABLE_P2P);
            }

            mWigigNative.setScanInterval((int)mSupplicantScanIntervalMs / 1000);

            final Intent intent = new Intent(WigigManager.WIGIG_SCAN_AVAILABLE);
            intent.addFlags(Intent.FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
            intent.putExtra(WigigManager.EXTRA_SCAN_AVAILABLE, WIGIG_STATE_ENABLED);
            mContext.sendStickyBroadcastAsUser(intent, UserHandle.ALL);

            if (!mWifiManager.isWifiEnabled() && !mWifiManager.isWifiApEnabled()) {
                String countryCode = mWifiManager.getCountryCode();
                logd("current country code is " + countryCode);
                if (!TextUtils.isEmpty(countryCode)) {
                    mWigigNative.setCountryCode(countryCode);
                }
            }
        }
        @Override
        public boolean processMessage(Message message) {
            WifiConfiguration config;
            int netId;
            boolean didDisconnect;
            NetworkUpdateResult result;
            logStateAndMessage(message, getClass().getSimpleName());

            switch(message.what) {
                case WigigMonitor.SSID_TEMP_DISABLED:
                case WigigMonitor.SSID_REENABLED:
                    String substr = (String) message.obj;
                    String en = (message.what == WigigMonitor.SSID_TEMP_DISABLED ?
                            "temp-disabled" : "re-enabled");
                    logd("SSID state=" + en + " nid="
                            + Integer.toString(message.arg1) + " [" + substr + "]");
                    synchronized(mScanResultCache) {
                        mWigigConfigStore.handleSSIDStateChange(message.arg1, message.what ==
                                WigigMonitor.SSID_REENABLED);
                    }
                    break;
                case CMD_STOP_SUPPLICANT:   /* Supplicant stopped by user */
                    if (mP2pSupported) {
                        transitionTo(mWaitForP2pDisableState);
                    } else {
                        transitionTo(mSupplicantStoppingState);
                    }
                    break;
                case WigigMonitor.SUP_DISCONNECTION_EVENT:  /* Supplicant connection lost */
                    loge("Connection lost, restart supplicant");
                    handleSupplicantConnectionLoss();
                    handleNetworkDisconnect();
                    transitionTo(mInitialState);
                    sendMessageDelayed(CMD_START_SUPPLICANT, SUPPLICANT_RESTART_INTERVAL_MSECS);
                    break;
                case CMD_START_SCAN:
                    handleScanRequest();
                    break;
                case WigigMonitor.SCAN_RESULTS_EVENT:
                case WigigMonitor.SCAN_FAILED_EVENT:
                    setScanResults();
                    sendScanResultsAvailableBroadcast();
                    break;
                case CMD_START_AP:
                    /* Cannot start soft AP while in client mode */
                    loge("Failed to start soft AP with a running supplicant");
                    setWigigApState(WIGIG_AP_STATE_FAILED, WigigManager.SAP_START_FAILURE_GENERAL);
                    break;
                case CMD_GET_CONFIGURED_NETWORKS:
                    replyToMessage(message, message.what,
                            mWigigConfigStore.getConfiguredNetworks());
                    break;
                case WigigMonitor.SUPPLICANT_STATE_CHANGE_EVENT:
                    SupplicantState state = handleSupplicantStateChange(message);

                    // Supplicant can fail to report a NETWORK_DISCONNECTION_EVENT
                    // when authentication times out after a successful connection,
                    // we can figure this from the supplicant state. If supplicant
                    // state is DISCONNECTED, but the mNetworkInfo says we are not
                    // disconnected, we need to handle a disconnection
                    if (state == SupplicantState.DISCONNECTED &&
                            mNetworkInfo.getState() != NetworkInfo.State.DISCONNECTED) {
                        if (DBG) log("Missed CTRL-EVENT-DISCONNECTED, disconnect");
                        handleNetworkDisconnect();
                        transitionTo(mDisconnectedState);
                    }

                    if (state == SupplicantState.COMPLETED) {
                        mIpManager.confirmConfiguration();
                    }
                    break;
                case WigigManager.SAVE_NETWORK:
                    config = (WifiConfiguration) message.obj;
                    if (config == null) {
                        loge("ERROR: SAVE_NETWORK with null configuration."
                                + mWifiInfo.getSupplicantState().toString()
                                + " my state " + getCurrentState().getName());
                        replyToMessage(message, WigigManager.SAVE_NETWORK_FAILED,
                                WigigManager.ERROR);
                        break;
                    }
                    int nid = config.networkId;
                    logd("SAVE_NETWORK id=" + Integer.toString(nid)
                                + " config=" + config.SSID
                                + " nid=" + config.networkId
                                + " supstate=" + mWifiInfo.getSupplicantState().toString()
                                + " my state " + getCurrentState().getName());

                    result = mWigigConfigStore.saveNetwork(config, WifiConfiguration.UNKNOWN_UID);
                    if (result.getNetworkId() != WifiConfiguration.INVALID_NETWORK_ID) {
                        if (mWifiInfo.getNetworkId() == result.getNetworkId()) {
                            if (result.hasIpChanged()) {
                                // The currently connection configuration was changed
                                // We switched from DHCP to static or from static to DHCP, or the
                                // static IP address has changed.
                                log("Reconfiguring IP on connection");
                                // TODO: clear addresses and disable IPv6
                                // to simplify obtainingIpState.
                                mWigigNative.disconnect();
                                transitionTo(mDisconnectingState);
                                // reconnect to the same network
                                if (mWigigNative.reconnect()) {
                                    log("Reconnecting after IP reconfiguration");
                                } else {
                                    loge("Failed to reconnect after IP reconfiguration");
                                    // we will transition to disconnected state by previous code
                                }
                            }
                            if (result.hasProxyChanged()) {
                                log("Reconfiguring proxy on connection");
                            }
                        }
                        replyToMessage(message, WigigManager.SAVE_NETWORK_SUCCEEDED);
                        broadcastWigigCredentialChanged(WigigManager.WIGIG_CREDENTIAL_SAVED, config);

                        if (DBG) {
                           logd("Success save network nid="
                                    + Integer.toString(result.getNetworkId()));
                        }
                    } else {
                        loge("Failed to save network");
                        replyToMessage(message, WigigManager.SAVE_NETWORK_FAILED,
                                WigigManager.ERROR);
                    }
                    break;
                case WigigManager.FORGET_NETWORK:
                    if (mWigigConfigStore.forgetNetwork(message.arg1)) {
                        replyToMessage(message, WigigManager.FORGET_NETWORK_SUCCEEDED);
                        broadcastWigigCredentialChanged(WigigManager.WIGIG_CREDENTIAL_FORGOT,
                                (WifiConfiguration) message.obj);
                    } else {
                        loge("Failed to forget network");
                        replyToMessage(message, WigigManager.FORGET_NETWORK_FAILED,
                                WigigManager.ERROR);
                    }
                    break;
                case WigigManager.START_WPS:
                    WpsInfo wpsInfo = (WpsInfo) message.obj;
                    WpsResult wpsResult;
                    switch (wpsInfo.setup) {
                        case WpsInfo.PBC:
                            wpsResult = mWigigConfigStore.startWpsPbc(wpsInfo);
                            break;
                        case WpsInfo.KEYPAD:
                            wpsResult = mWigigConfigStore.startWpsWithPinFromAccessPoint(wpsInfo);
                            break;
                        case WpsInfo.DISPLAY:
                            wpsResult = mWigigConfigStore.startWpsWithPinFromDevice(wpsInfo);
                            break;
                        case WpsInfo.LABEL:
                            wpsResult = mWigigConfigStore.startWpsWithPinFromLabel(wpsInfo);
                            break;
                        default:
                            wpsResult = new WpsResult(Status.FAILURE);
                            loge("Invalid setup for WPS");
                            break;
                    }
                    mWigigConfigStore.setLastSelectedConfiguration
                            (WifiConfiguration.INVALID_NETWORK_ID);
                    if (wpsResult.status == Status.SUCCESS) {
                        replyToMessage(message, WigigManager.START_WPS_SUCCEEDED, wpsResult);
                        transitionTo(mWpsRunningState);
                    } else {
                        loge("Failed to start WPS with config " + wpsInfo.toString());
                        replyToMessage(message, WigigManager.WPS_FAILED, WigigManager.ERROR);
                    }
                    break;
                case WigigMonitor.NETWORK_CONNECTION_EVENT:
                    if (DBG) log("Network connection established");
                    mLastNetworkId = message.arg1;
                    mLastBssid = (String) message.obj;

                    mWifiInfo.setBSSID(mLastBssid);
                    mWifiInfo.setNetworkId(mLastNetworkId);

                    // TODO: call setSupplicantStateChanged instead? or it will happen anyway and this call is not needed at all
                    sendNetworkStateChangeBroadcast(mLastBssid);
                    transitionTo(mObtainingIpState);
                    break;
                case WigigMonitor.NETWORK_DISCONNECTION_EVENT:
                    // Calling handleNetworkDisconnect here is redundant because we might already
                    // have called it when leaving L2ConnectedState to go to disconnecting state
                    // or thru other path
                    // We should normally check the mWifiInfo or mLastNetworkId so as to check
                    // if they are valid, and only in this case call handleNetworkDisconnect,
                    // TODO: this should be fixed for a L MR release
                    // The side effect of calling handleNetworkDisconnect twice is that a bunch of
                    // idempotent commands are executed twice (stopping Dhcp, enabling the SPS mode
                    // at the chip etc...
                    if (DBG) log("Network connection lost");
                    handleNetworkDisconnect();
                    transitionTo(mDisconnectedState);
                    break;
                /* Do a redundant disconnect without transition */
                case CMD_DISCONNECT:
                    mDoPno = false;
                    mWigigConfigStore.setLastSelectedConfiguration
                            (WifiConfiguration.INVALID_NETWORK_ID);
                    mWigigNative.disconnect();

                    logd("turn on periodic scan after supplicant disconnect command");
                    sendMessageDelayed(CMD_DISCONNECTED_PERIODIC_SCAN,
                            ++mDisconnectedPeriodicScanToken, mSupplicantScanIntervalMs);
                    break;
                case WigigManager.CONNECT_NETWORK:
                    /**
                     * The connect message can contain a network id passed as arg1 on message or
                     * a config passed as obj on message.
                     * For a new network, a config is passed to create and connect.
                     * For an existing network, a network id is passed
                     */
                    netId = message.arg1;
                    config = (WifiConfiguration) message.obj;
                    boolean updatedExisting = false;

                    /* Save the network config */
                    if (config != null) {
                        String configKey = config.configKey(true /* allowCached */);
                        WifiConfiguration savedConfig =
                                mWigigConfigStore.getWifiConfiguration(configKey);
                        if (savedConfig != null) {
                            // There is an existing config with this netId, but it wasn't exposed
                            // (either AUTO_JOIN_DELETED or ephemeral; see WigigConfigStore#
                            // getConfiguredNetworks). Remove those bits and update the config.
                            // Note this is not expected in Wigig but keeping it to be on the safe side
                            config = savedConfig;
                            logd("CONNECT_NETWORK updating existing config with id=" +
                                    config.networkId + " configKey=" + configKey);
                            config.ephemeral = false;
                            updatedExisting = true;
                        }

                        result = mWigigConfigStore.saveNetwork(config, message.sendingUid);
                        netId = result.getNetworkId();
                    }
                    config = mWigigConfigStore.getWifiConfiguration(netId);

                    if (config == null) {
                        logd("CONNECT_NETWORK no config for id=" + Integer.toString(netId)
                                + " my state " + getCurrentState().getName());
                        replyToMessage(message, WigigManager.CONNECT_NETWORK_FAILED,
                                WigigManager.ERROR);
                        break;
                    } else {
                        logd("CONNECT_NETWORK id=" + Integer.toString(netId)
                                + " config=" + config.SSID
                                + " cnid=" + config.networkId
                                + " my state " + getCurrentState().getName()
                                + " uid = " + message.sendingUid);
                    }

                    if (deferForUserInput(message, netId)) {
                        break;
                    } else if (mWigigConfigStore.getWifiConfiguration(netId).userApproved ==
                                                                    WifiConfiguration.USER_BANNED) {
                        replyToMessage(message, WigigManager.CONNECT_NETWORK_FAILED,
                                WigigManager.NOT_AUTHORIZED);
                        break;
                    }

                    mWigigConfigStore.setLastSelectedConfiguration(netId);

                    didDisconnect = false;
                    if (mLastNetworkId != WifiConfiguration.INVALID_NETWORK_ID
                            && mLastNetworkId != netId) {
                        /**
                         * Supplicant will ignore the reconnect if we are currently associated,
                         * hence trigger a disconnect
                         */
                        didDisconnect = true;
                        mWigigNative.disconnect();
                    }

                    // Make sure the network is enabled, since supplicant will not reenable it
                    mWigigConfigStore.enableNetworkWithoutBroadcast(netId, false);

                    if (mWigigConfigStore.selectNetwork(config, /* updatePriorities = */ true,
                            message.sendingUid) && mWigigNative.reconnect()) {
                        replyToMessage(message, WigigManager.CONNECT_NETWORK_SUCCEEDED);
                        if (didDisconnect) {
                            /* Expect a disconnection from the old connection */
                            transitionTo(mDisconnectingState);
                        } else if (updatedExisting && getCurrentState() == mConnectedState &&
                                getCurrentWifiConfiguration().networkId == netId) {
                            logd("unexpected config change of saved network, nid=" + Integer.toString(netId));
                            // Update the current set of network capabilities, but stay in the
                            // current state.
                            updateCapabilities(config);
                        } else {
                            /**
                             * Directly go to disconnected state where we
                             * process the connection events from supplicant
                             */
                            transitionTo(mDisconnectedState);
                        }
                    } else {
                        loge("Failed to connect config: " + config + " netId: " + netId);
                        replyToMessage(message, WigigManager.CONNECT_NETWORK_FAILED,
                                WigigManager.ERROR);
                        break;
                    }
                    break;
                case CMD_AIRPLANE_TOGGLED:
                    boolean newWigigMode = (message.arg1 == 1);
                    // message.arg2 contains airplaneModeOn which is irrelvant in STA mode

                    log("newWigigMode=" + newWigigMode);

                    if (!newWigigMode) {
                        log("disabling wigig due to airplane mode change");
                        setSupplicantRunning(false);
                    }
                    break;
                case CMD_COUNTRY_CODE_CHANGED:
                    String countryCode = (String)message.obj;
                    logd("new country code is " + countryCode);
                    if (!TextUtils.isEmpty(countryCode)) {
                        mWigigNative.setCountryCode(countryCode);
                    }
                    break;
                default:
                    return NOT_HANDLED;
            }
            return HANDLED;
        }
        @Override
        public void exit() {
            mNetworkInfo.setIsAvailable(false);
            if (mNetworkAgent != null) mNetworkAgent.sendNetworkInfo(mNetworkInfo);
            mScanResults = new ArrayList<ScanResult>();
        }
    }

    class SupplicantStoppingState extends State {
        @Override
        public void enter() {
            /* Send any reset commands to supplicant before shutting it down */
            handleNetworkDisconnect();

            loge("SupplicantStoppingState: stopSupplicant");
            mWigigNative.stopSupplicant();

            /* Send ourselves a delayed message to indicate failure after a wait time */
            sendMessageDelayed(obtainMessage(CMD_STOP_SUPPLICANT_FAILED,
                    ++mSupplicantStopFailureToken, 0), SUPPLICANT_RESTART_INTERVAL_MSECS);
        }
        @Override
        public boolean processMessage(Message message) {
            logStateAndMessage(message, getClass().getSimpleName());

            switch(message.what) {
                case WigigMonitor.SUP_DISCONNECTION_EVENT:
                    if (DBG) log("Supplicant connection lost");
                    handleSupplicantConnectionLoss();
                    transitionTo(mInitialState);
                    break;
                case CMD_STOP_SUPPLICANT_FAILED:
                    if (message.arg1 == mSupplicantStopFailureToken) {
                        loge("Timed out on a supplicant stop, kill and proceed");
                        handleSupplicantConnectionLoss();
                        transitionTo(mInitialState);
                    }
                    break;
                case CMD_START_SUPPLICANT:
                case CMD_STOP_SUPPLICANT:
                case CMD_START_AP:
                case CMD_STOP_AP:
                case CMD_AIRPLANE_TOGGLED:
                    deferMessage(message);
                    break;
                default:
                    return NOT_HANDLED;
            }
            return HANDLED;
        }
    }

    private ConnectivityManager.OnStartTetheringCallback mOnStartTetheringCallback =
        new ConnectivityManager.OnStartTetheringCallback() {
        @Override
        public void onTetheringStarted() {
            if (DBG) log("onTetheringStarted");
            // do nothing here, we look for WIFI_AP_STATE_CHANGED events
        }

        @Override
        public void onTetheringFailed() {
            if (DBG) log("onTetheringFailed");
            // do nothing here, we have watchdog that will be triggered
        }
    };

    class RateUpgradeStartingState extends State {
        @Override
        public void enter() {
            /* disable Wifi STA/SAP before enabling fst properties. In case Wifi concurrency is ON,
             * Wifi SAP is ignored.
             */
            checkAndSetConnectivityInstance();
            if (mWifiManager.isWifiEnabled()) {
                mContext.registerReceiver(mWifiReceiver, new IntentFilter(
                        WifiManager.WIFI_STATE_CHANGED_ACTION));
                mWifiManager.setWifiEnabled(false);
                sendMessageDelayed(CMD_WIFI_STATE_CHANGE_WATCHDOG_TIMER,
                        WIFI_STATE_CHANGE_GUARD_TIMER_MSEC);
            } else if (mWifiManager.isWifiApEnabled() && !mWifiManager.getWifiStaSapConcurrency()) {
                mContext.registerReceiver(mWifiReceiver, new IntentFilter(
                        WifiManager.WIFI_AP_STATE_CHANGED_ACTION));
                mCm.stopTethering(TETHERING_WIFI);
                sendMessageDelayed(CMD_WIFI_STATE_CHANGE_WATCHDOG_TIMER,
                        WIFI_STATE_CHANGE_GUARD_TIMER_MSEC);
            } else {
                setFstPropEnabled(true);
                transitionTo(mRateUpgradeState);
            }
        }

        @Override
        public boolean processMessage(Message message) {
            logStateAndMessage(message, getClass().getSimpleName());

            switch(message.what) {
                case CMD_RATE_UPGRADE_WIFI_STATE_CHANGED:
                    if (message.arg1 == WifiManager.WIFI_STATE_DISABLED){
                        setFstPropEnabled(true);
                        mWifiManager.setWifiEnabled(true);
                    } else if (message.arg1 == WifiManager.WIFI_STATE_ENABLED){
                        transitionTo(mRateUpgradeState);
                    } else if (message.arg1 == WifiManager.WIFI_AP_STATE_DISABLED) {
                        setFstPropEnabled(true);
                        mCm.startTethering(TETHERING_WIFI, false, mOnStartTetheringCallback, null);
                    } else if (message.arg1 == WifiManager.WIFI_AP_STATE_ENABLED) {
                        transitionTo(mRateUpgradeState);
                    }
                    break;
                case CMD_STOP_RATE_UPGRADE:
                    transitionTo(mRateUpgradeStoppingState);
                    break;
                case CMD_WIFI_STATE_CHANGE_WATCHDOG_TIMER:
                    loge("Timeout waiting for Wifi/SoftAp enabled/disabled");
                    transitionTo(mRateUpgradeStoppingState);
                    break;
                case CMD_START_RATE_UPGRADE:
                case CMD_START_SUPPLICANT:
                case CMD_START_AP:
                    deferMessage(message);
                    break;
                default:
                    return NOT_HANDLED;
            }
            return HANDLED;
        }

        @Override
        public void exit() {
            removeMessages(CMD_WIFI_STATE_CHANGE_WATCHDOG_TIMER);
            /* Receiver may be unregistered and throw exception */
            try {
                mContext.unregisterReceiver(mWifiReceiver);
            } catch (IllegalArgumentException e) {
                logd("Couldn't unregister mWifiReceiver");
            }
        }

    }

    class RateUpgradeState extends State {
        @Override
        public void enter() {
            setWigigState(WIGIG_STATE_RATE_UPGRADE);
        }

        @Override
        public boolean processMessage(Message message) {
            logStateAndMessage(message, getClass().getSimpleName());

            switch(message.what) {
                case CMD_STOP_RATE_UPGRADE:
                    transitionTo(mRateUpgradeStoppingState);
                    setWigigState(WIGIG_STATE_DISABLED);
                    break;
                default:
                    return NOT_HANDLED;
            }
            return HANDLED;
        }
    }

    class RateUpgradeStoppingState extends State {
        @Override
        public void enter() {
            /* disable Wifi STA/SAP before disabling fst properties. In case Wifi concurrency is ON,
             * Wifi SAP is ignored.
             */
            checkAndSetConnectivityInstance();
            if (mWifiManager.isWifiEnabled()) {
                mContext.registerReceiver(mWifiReceiver, new IntentFilter(
                        WifiManager.WIFI_STATE_CHANGED_ACTION));
                mWifiManager.setWifiEnabled(false);
                sendMessageDelayed(CMD_WIFI_STATE_CHANGE_WATCHDOG_TIMER,
                        WIFI_STATE_CHANGE_GUARD_TIMER_MSEC);
            } else if (mWifiManager.isWifiApEnabled() && !mWifiManager.getWifiStaSapConcurrency()) {
                mContext.registerReceiver(mWifiReceiver, new IntentFilter(
                        WifiManager.WIFI_AP_STATE_CHANGED_ACTION));
                mCm.stopTethering(TETHERING_WIFI);
                sendMessageDelayed(CMD_WIFI_STATE_CHANGE_WATCHDOG_TIMER,
                        WIFI_STATE_CHANGE_GUARD_TIMER_MSEC);
            } else {
                setFstPropEnabled(false);
                transitionTo(mInitialState);
            }
        }

        @Override
        public boolean processMessage(Message message) {
            logStateAndMessage(message, getClass().getSimpleName());

            switch(message.what) {
                case CMD_RATE_UPGRADE_WIFI_STATE_CHANGED:
                    if (message.arg1 == WifiManager.WIFI_STATE_DISABLED){
                        setFstPropEnabled(false);
                        mWifiManager.setWifiEnabled(true);
                    } else if (message.arg1 == WifiManager.WIFI_STATE_ENABLED){
                        transitionTo(mInitialState);
                    } else if (message.arg1 == WifiManager.WIFI_AP_STATE_DISABLED) {
                        setFstPropEnabled(false);
                        mCm.startTethering(TETHERING_WIFI, false, mOnStartTetheringCallback, null);
                    } else if (message.arg1 == WifiManager.WIFI_AP_STATE_ENABLED) {
                        transitionTo(mInitialState);
                    }
                    break;
                case CMD_WIFI_STATE_CHANGE_WATCHDOG_TIMER:
                    loge("Timeout waiting for Wifi/SoftAp enabled/disabled, disabling fst_prop");
                    setFstPropEnabled(false);
                    transitionTo(mInitialState);
                    break;
                case CMD_START_RATE_UPGRADE:
                case CMD_START_SUPPLICANT:
                case CMD_START_AP:
                    deferMessage(message);
                    break;
                default:
                    return NOT_HANDLED;
            }
            return HANDLED;
        }

        @Override
        public void exit() {
            removeMessages(CMD_WIFI_STATE_CHANGE_WATCHDOG_TIMER);
            /* Receiver may be unregistered and throw exception */
            try {
                mContext.unregisterReceiver(mWifiReceiver);
            } catch (IllegalArgumentException e) {
                logd("Couldn't unregister mWifiReceiver");
            }
        }
    }

    class WaitForP2pDisableState extends State {
        private State mTransitionToState;
        @Override
        public void enter() {
            switch (getCurrentMessage().what) {
                case WigigMonitor.SUP_DISCONNECTION_EVENT:
                    mTransitionToState = mInitialState;
                    break;
                case CMD_STOP_SUPPLICANT:
                    mTransitionToState = mSupplicantStoppingState;
                    break;
                default:
                    mTransitionToState = mSupplicantStoppingState;
                    break;
            }
            mWigigP2pChannel.sendMessage(WigigStateMachine.CMD_DISABLE_P2P_REQ);
        }
        @Override
        public boolean processMessage(Message message) {
            logStateAndMessage(message, getClass().getSimpleName());

            switch (message.what) {
                case WigigStateMachine.CMD_DISABLE_P2P_RSP:
                    transitionTo(mTransitionToState);
                    break;
                /* Defer wigig start/shut and driver commands */
                case WigigMonitor.SUPPLICANT_STATE_CHANGE_EVENT:
                case CMD_START_SUPPLICANT:
                case CMD_STOP_SUPPLICANT:
                case CMD_START_AP:
                case CMD_STOP_AP:
                case CMD_START_SCAN:
                case CMD_DISCONNECT:
                case CMD_AIRPLANE_TOGGLED:
                    deferMessage(message);
                    break;
                default:
                    return NOT_HANDLED;
            }
            return HANDLED;
        }
    }

    class RadioInUseByP2pState extends State {
        @Override
        public void enter() {
            mWigigConfigStore.disableAllNetworks();
            clearLinkProperties();
            // TODO send the radio state update with a delay to allow
            // periodic scan to complete. Once we have support for
            // aborting scan, we can send the update immediately
            sendMessageDelayed(CMD_P2P_REQUEST_RADIO_STATE_UPDATE, 500);
        }

        @Override
        public boolean processMessage(Message message) {
            logStateAndMessage(message, getClass().getSimpleName());

            boolean handled = HANDLED;

            switch(message.what) {
                case CMD_START_SCAN:
                    if (DBG) log("P2P in use, ignore start scan");
                    break;
                case WigigMonitor.SCAN_RESULTS_EVENT:
                case WigigMonitor.SCAN_FAILED_EVENT:
                    sendP2pRadioStateUpdate(P2P_RADIO_STATE_IN_USE_BY_P2P);
                    // let parent handle this as well, to send back scan results
                    handled = NOT_HANDLED;
                    break;
                case CMD_P2P_RELEASED_RADIO:
                    transitionTo(mDisconnectedState);
                    break;
                case CMD_P2P_REQUEST_RADIO:
                case CMD_P2P_REQUEST_RADIO_STATE_UPDATE:
                    sendP2pRadioStateUpdate(P2P_RADIO_STATE_IN_USE_BY_P2P);
                    break;
                case CMD_UPDATE_LINKPROPERTIES:
                    if (DBG) log("P2P in use, ignore CMD_UPDATE_LINKPROPERTIES");
                    break;
                case WigigManager.SAVE_NETWORK:
                    replyToMessage(message, WigigManager.SAVE_NETWORK_FAILED,
                            WigigManager.BUSY);
                    break;
                case WigigManager.FORGET_NETWORK:
                    replyToMessage(message, WigigManager.FORGET_NETWORK_FAILED,
                            WigigManager.BUSY);
                    break;
                case CMD_DISCONNECT:
                    if (DBG) log("P2P in use, ignore CMD_DISCONNECT");
                    break;
                case WigigManager.CONNECT_NETWORK:
                    replyToMessage(message, WigigManager.CONNECT_NETWORK_FAILED,
                            WigigManager.BUSY);
                    break;
                case WigigManager.START_WPS:
                    replyToMessage(message, WigigManager.WPS_FAILED,
                            WigigManager.BUSY);
                    break;
                default:
                    handled = NOT_HANDLED;
            }
            return handled;
        }

        @Override
        public void exit()
        {
            // TODO we may need to remember which networks were disabled and
            // re-enable only those, but currently we don't have any scenario
            // which justifies the added logic to remember disabled networks
            mWigigConfigStore.enableAllNetworks();
        }
    }

    WifiConfiguration getCurrentWifiConfiguration() {
        if (mLastNetworkId == WifiConfiguration.INVALID_NETWORK_ID) {
            return null;
        }
        return mWigigConfigStore.getWifiConfiguration(mLastNetworkId);
    }

    private void updateCapabilities(WifiConfiguration config) {
        if (config != null) {
            if (config.ephemeral) {
                mNetworkCapabilities.removeCapability(
                        NetworkCapabilities.NET_CAPABILITY_TRUSTED);
            } else {
                mNetworkCapabilities.addCapability(
                        NetworkCapabilities.NET_CAPABILITY_TRUSTED);
            }
        }
        mNetworkAgent.sendNetworkCapabilities(mNetworkCapabilities);
    }

    private class WigigNetworkAgent extends NetworkAgent {
        public WigigNetworkAgent(Looper l, Context c, String TAG, NetworkInfo ni,
                NetworkCapabilities nc, LinkProperties lp, int score) {
            super(l, c, TAG, ni, nc, lp, score);
        }
        protected void unwanted() {
            // Ignore if we're not the current networkAgent.
            if (this != mNetworkAgent) return;
            if (DBG) log("WifiNetworkAgent -> Wifi unwanted score "
                    + Integer.toString(mWifiInfo.score));
            // TODO ignore for now, we don't support on-demand disconnect
        }
    }

    class L2ConnectedState extends State {
        @Override
        public void enter() {
            logd("turn on RSSI poll");
            mRssiPollToken++;
            sendMessage(CMD_RSSI_POLL, mRssiPollToken, 0);
            if (mNetworkAgent != null) {
                loge("Have NetworkAgent when entering L2Connected");
                setNetworkDetailedState(DetailedState.DISCONNECTED);
            }
            setNetworkDetailedState(DetailedState.CONNECTING);
            int score = SystemProperties.getInt("persist.vendor.wigig.net.score", DEFAULT_SCORE);
            mNetworkAgent = new WigigNetworkAgent(getHandler().getLooper(), mContext,
                    "WigigNetworkAgent", mNetworkInfo, mNetworkCapabilitiesFilter,
                    mLinkProperties, score);
            final NetworkRequest request =
                new NetworkRequest.Builder()
                    .clearCapabilities()
                    .addTransportType(NetworkCapabilities.TRANSPORT_WIFI)
                    .addCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET)
                    .setNetworkSpecifier(NETWORK_SPECIFIER)
                    .build();
            // we use a network callback to get notification when WIGIG network is actually
            // registered with netd. This is because we cannot add a local route before
            // it is registered, and the registration takes place asynchronously, shortly
            // after calling NetworkAgent.sendLinkProperties.
            mNetworkCallback = new NetworkCallback() {
                @Override
                public void onLinkPropertiesChanged(Network network, LinkProperties linkProperties) {
                    if (DBG) log("onLinkPropertiesChanged, netId " + network.netId + " lp: " + linkProperties);
                    sendMessage(CMD_ADD_LOCAL_ROUTE, network);
                }

                @Override
                public void onPreCheck(Network network) {
                    // we monitor also the pre-check event because the ConnectivityService skips
                    // the first call to onLinkPropertiesChanged
                    if (DBG) log("onPreCheck, netId " + network.netId);
                    sendMessage(CMD_ADD_LOCAL_ROUTE, network);
                }
            };
            checkAndSetConnectivityInstance();
            mCm.requestNetwork(request, mNetworkCallback);
            mAddedLocalRoute = false;
        }

        @Override
        public void exit() {
            mCm.unregisterNetworkCallback(mNetworkCallback);
            mNetworkCallback = null;
            mAddedLocalRoute = false;

            mIpManager.stop();

            if (DBG) {
                StringBuilder sb = new StringBuilder();
                sb.append("leaving L2ConnectedState state nid=" + Integer.toString(mLastNetworkId));
                if (mLastBssid !=null) {
                    sb.append(" ").append(mLastBssid);
                }
                logd(sb.toString());
            }
            // For paranoia's sake, call handleNetworkDisconnect
            // only if BSSID is null or last networkId
            // is not invalid.
            if (mLastBssid != null || mLastNetworkId != WifiConfiguration.INVALID_NETWORK_ID) {
                handleNetworkDisconnect();
            }
        }

        @Override
        public boolean processMessage(Message message) {
            logStateAndMessage(message, getClass().getSimpleName());

            switch (message.what) {
                case DhcpClient.CMD_PRE_DHCP_ACTION:
                    handlePreDhcpSetup();
                    break;
                case DhcpClient.CMD_PRE_DHCP_ACTION_COMPLETE:
                    mIpManager.completedPreDhcpAction();
                    break;
                case DhcpClient.CMD_POST_DHCP_ACTION:
                    handlePostDhcpSetup();
                    // We advance to mConnectedState because IpManager will also send a
                    // CMD_IPV4_PROVISIONING_SUCCESS message, which calls handleIPv4Success(),
                    // which calls updateLinkProperties, which then sends
                    // CMD_IP_CONFIGURATION_SUCCESSFUL.
                    //
                    // In the event of failure, we transition to mDisconnectingState
                    // similarly--via messages sent back from IpManager.
                    break;
                case CMD_IPV4_PROVISIONING_SUCCESS: {
                    handleIPv4Success((DhcpResults) message.obj);
                    sendNetworkStateChangeBroadcast(mLastBssid);
                    break;
                }
                case CMD_IPV4_PROVISIONING_FAILURE: {
                    handleIPv4Failure();
                    break;
                }
                case CMD_IP_CONFIGURATION_SUCCESSFUL:
                    handleSuccessfulIpConfiguration();
                    sendConnectedState();
                    transitionTo(mConnectedState);
                    break;
                case CMD_IP_CONFIGURATION_LOST:
                    handleIpConfigurationLost();
                    transitionTo(mDisconnectingState);
                    break;
                case CMD_DISCONNECT:
                    mDoPno = false;
                    mWigigNative.disconnect();

                    logd("turn on periodic scan after supplicant disconnect command");
                    sendMessageDelayed(CMD_DISCONNECTED_PERIODIC_SCAN,
                            ++mDisconnectedPeriodicScanToken, mSupplicantScanIntervalMs);
                    transitionTo(mDisconnectingState);
                    break;
                case WigigManager.CONNECT_NETWORK:
                    int netId = message.arg1;
                    if (mWifiInfo.getNetworkId() == netId) {
                        if (DBG) logd("ignoring connection to already connected network");
                        break;
                    }
                    return NOT_HANDLED;
                case WigigMonitor.NETWORK_CONNECTION_EVENT:
                    mWifiInfo.setBSSID((String) message.obj);
                    mLastNetworkId = message.arg1;
                    mWifiInfo.setNetworkId(mLastNetworkId);
                    if(!mLastBssid.equals((String) message.obj)) {
                        mLastBssid = (String) message.obj;
                        sendNetworkStateChangeBroadcast(mLastBssid);
                    }
                    break;
                case CMD_RSSI_POLL:
                    if (message.arg1 == mRssiPollToken) {
                        // Get Info and continue polling
                        fetchRssiNative();
                        sendMessageDelayed(obtainMessage(CMD_RSSI_POLL,
                                mRssiPollToken, 0), POLL_RSSI_INTERVAL_MSECS);
                    }
                    break;
                case CMD_ASSOCIATED_BSSID:
                    if ((String) message.obj == null) {
                        logd("Associated command w/o BSSID");
                        break;
                    }
                    mLastBssid = (String) message.obj;
                    if (mLastBssid != null
                            && (mWifiInfo.getBSSID() == null
                            || !mLastBssid.equals(mWifiInfo.getBSSID()))) {
                        mWifiInfo.setBSSID((String) message.obj);
                        sendNetworkStateChangeBroadcast(mLastBssid);
                    }
                    break;
                case CMD_ADD_LOCAL_ROUTE:
                    Network network = (Network)message.obj;
                    if (mNetworkAgent != null && mNetworkAgent.netId == network.netId) {
                        checkAndAddLocalRoute();
                    }
                    break;
                default:
                    return NOT_HANDLED;
            }
            return HANDLED;
        }
    }

    class ObtainingIpState extends State {
        @Override
        public void enter() {
            if (DBG) {
                String key = "";
                if (getCurrentWifiConfiguration() != null) {
                    key = getCurrentWifiConfiguration().configKey();
                }
                log("enter ObtainingIpState netId=" + Integer.toString(mLastNetworkId)
                        + " " + key + " "
                        + " static=" + mWigigConfigStore.isUsingStaticIp(mLastNetworkId)
                        + " watchdog= " + obtainingIpWatchdogCount);
            }

            // Send event to CM & network change broadcast
            setNetworkDetailedState(DetailedState.OBTAINING_IPADDR);

            // Stop IpManager in case we're switching from DHCP to static
            // configuration or vice versa.
            //
            // TODO: Only ever enter this state the first time we connect to a
            // network, never on switching between static configuration and
            // DHCP. When we transition from static configuration to DHCP in
            // particular, we must tell ConnectivityService that we're
            // disconnected, because DHCP might take a long time during which
            // connectivity APIs such as getActiveNetworkInfo should not return
            // CONNECTED.
            stopIpManager();

            if (!TextUtils.isEmpty(mTcpBufferSizes)) {
                mIpManager.setTcpBufferSizes(mTcpBufferSizes);
            }

            if (!mWigigConfigStore.isUsingStaticIp(mLastNetworkId)) {
                final IpManager.ProvisioningConfiguration prov =
                        mIpManager.buildProvisioningConfiguration()
                            .withPreDhcpAction()
                            .build();
                mIpManager.startProvisioning(prov);
                obtainingIpWatchdogCount++;
                logd("Start Dhcp Watchdog " + obtainingIpWatchdogCount);
                sendMessageDelayed(obtainMessage(CMD_OBTAINING_IP_ADDRESS_WATCHDOG_TIMER,
                        obtainingIpWatchdogCount, 0), OBTAINING_IP_ADDRESS_GUARD_TIMER_MSEC);
            } else {
                StaticIpConfiguration config = mWigigConfigStore.getStaticIpConfiguration(
                        mLastNetworkId);
                if (config.ipAddress == null) {
                    logd("Static IP lacks address");
                    sendMessage(CMD_IPV4_PROVISIONING_FAILURE);
                } else {
                    final IpManager.ProvisioningConfiguration prov =
                            mIpManager.buildProvisioningConfiguration()
                                .withStaticConfiguration(config)
                                .build();
                    mIpManager.startProvisioning(prov);
                }
            }
        }

        @Override
        public boolean processMessage(Message message) {
            logStateAndMessage(message, getClass().getSimpleName());

            switch(message.what) {
                case WigigManager.SAVE_NETWORK:
                    deferMessage(message);
                    break;
                case CMD_OBTAINING_IP_ADDRESS_WATCHDOG_TIMER:
                    if (message.arg1 == obtainingIpWatchdogCount) {
                        logd("ObtainingIpAddress: Watchdog Triggered, count="
                                + obtainingIpWatchdogCount);
                        handleIpConfigurationLost();
                        transitionTo(mDisconnectingState);
                        break;
                    }
                    break;
                default:
                    return NOT_HANDLED;
            }
            return HANDLED;
        }
    }

    class ConnectedState extends State {
        @Override
        public void enter() {
             mWigigConfigStore.enableAllNetworks();
        }

        @Override
        public boolean processMessage(Message message) {
            logStateAndMessage(message, getClass().getSimpleName());

            switch (message.what) {
                case CMD_ASSOCIATED_BSSID:
                    // ASSOCIATING to a new BSSID while already connected, indicates
                    // that driver is roaming
                    // TODO: roaming. fallthrough for now
                default:
                    return NOT_HANDLED;
            }
            //return HANDLED;
        }
    }

    private void sendConnectedState() {
        setNetworkDetailedState(DetailedState.CONNECTED);
        mWigigConfigStore.updateStatus(mLastNetworkId, DetailedState.CONNECTED);
        // TODO: remove because setNetworkDetailedState above already cover it
        sendNetworkStateChangeBroadcast(mLastBssid);
    }

    class DisconnectingState extends State {
        @Override
        public void enter() {
            // Make sure we disconnect: we enter this state prior to connecting to a new
            // network, waiting for either a DISCONNECT event or a SUPPLICANT_STATE_CHANGE
            // event which in this case will be indicating that supplicant started to associate.
            // In some cases supplicant doesn't ignore the connect requests (it might not
            // find the target SSID in its cache),
            // Therefore we end up stuck that state, hence the need for the watchdog.
            disconnectingWatchdogCount++;
            logd("Start Disconnecting Watchdog " + disconnectingWatchdogCount);
            sendMessageDelayed(obtainMessage(CMD_DISCONNECTING_WATCHDOG_TIMER,
                    disconnectingWatchdogCount, 0), DISCONNECTING_GUARD_TIMER_MSEC);
        }

        @Override
        public boolean processMessage(Message message) {
            logStateAndMessage(message, getClass().getSimpleName());
            switch (message.what) {
                case CMD_DISCONNECTING_WATCHDOG_TIMER:
                    if (disconnectingWatchdogCount == message.arg1) {
                        if (DBG) log("disconnecting watchdog! -> disconnect");
                        handleNetworkDisconnect();
                        transitionTo(mDisconnectedState);
                    }
                    break;
                case WigigMonitor.SUPPLICANT_STATE_CHANGE_EVENT:
                    /**
                     * If we get a SUPPLICANT_STATE_CHANGE_EVENT before NETWORK_DISCONNECTION_EVENT
                     * we have missed the network disconnection, transition to mDisconnectedState
                     * and handle the rest of the events there
                     */
                    deferMessage(message);
                    handleNetworkDisconnect();
                    transitionTo(mDisconnectedState);
                    break;
                case CMD_DISCONNECTED_PERIODIC_SCAN:
                    deferMessage(message);
                    break;
                default:
                    return NOT_HANDLED;
            }
            return HANDLED;
        }
    }

    class DisconnectedState extends State {
        private boolean mPnoStarted;

        private void startScanOrPNO() {
            /**
             * when saved networks exists, trigger scan (either PNO or regular scan). In case there
             * are no saved networks, application is expected to do periodic scans.
             * PNO is started in case auto connect is desired (app didn't explicitly requested
             * disconnect) and screen is OFF. Otherwise kickoff supplicant's periodic scan with
             * an initial scan request
             */
            if (mWigigConfigStore.getConfiguredNetworks().size() == 0)
                return;

            if (mDoPno && !mScreenOn) {
                mPnoStarted = mWigigNative.setPnoScan(true);
                if (mPnoStarted) {
                    return;
                }
            }

            if (mPnoStarted) {
                mWigigNative.setPnoScan(false);
                mPnoStarted = false;
            }

            log("sending initial scan request");
            mWigigNative.scan(null);
        }

        public void enter() {
            if (DBG) {
                logd(" Enter DisconnectedState screenOn=" + mScreenOn);
            }

            sendP2pRadioStateUpdate(P2P_RADIO_STATE_IDLE);

            mPnoStarted = false;
            startScanOrPNO();
        }

        public void exit() {
            if (mPnoStarted) {
                mWigigNative.setPnoScan(false);
            }
            mDoPno = true;
        }

        @Override
        public boolean processMessage(Message message) {
            boolean ret = HANDLED;

            logStateAndMessage(message, getClass().getSimpleName());

            switch (message.what) {
                case WigigMonitor.NETWORK_DISCONNECTION_EVENT:
                    /* Ignore network disconnect */
                    break;
                case WigigMonitor.SUPPLICANT_STATE_CHANGE_EVENT:
                    StateChangeResult stateChangeResult = (StateChangeResult) message.obj;
                    if (DBG) {
                        logd("SUPPLICANT_STATE_CHANGE_EVENT supstate="
                                + stateChangeResult.state + " detailed state="
                                + WifiInfo.getDetailedStateOf(stateChangeResult.state));
                    }
                    setNetworkDetailedState(WifiInfo.getDetailedStateOf(stateChangeResult.state));
                    /* parent state does the rest of the handling */
                    ret = NOT_HANDLED;
                    break;
                case CMD_P2P_REQUEST_RADIO:
                    transitionTo(mRadioInUseByP2pState);
                    break;
                case CMD_DISCONNECTED_PERIODIC_SCAN:
                    SupplicantState supplicantState = mWifiInfo.getSupplicantState();
                    boolean savedNetworks = (mWigigConfigStore.getConfiguredNetworks().size() > 0);
                    logd("disconnected periodic scan, supplicantState=" + supplicantState
                            + " savedNetworks=" + (savedNetworks ? "yes" : "no"));
                    if (message.arg1 == mDisconnectedPeriodicScanToken &&
                            supplicantState == SupplicantState.DISCONNECTED && savedNetworks) {
                        startScan();
                        sendMessageDelayed(CMD_DISCONNECTED_PERIODIC_SCAN,
                                ++mDisconnectedPeriodicScanToken, mSupplicantScanIntervalMs);
                    }
                    break;
                case CMD_SCREEN_STATE_CHANGED:
                    startScanOrPNO();
                    break;
                default:
                    return NOT_HANDLED;
            }
            return ret;
        }
    }

    class SoftApStartingState extends State {
        @Override
        public void enter() {
            final Message message = getCurrentMessage();
            if (message.what == CMD_START_AP) {
                final WifiConfiguration config = (WifiConfiguration) message.obj;

                if (config == null) {
                    mWigigApConfigChannel.sendMessage(CMD_REQUEST_AP_CONFIG);
                } else {
                    mWigigApConfigChannel.sendMessage(CMD_SET_AP_CONFIG, config);
                    startSoftApWithConfig(config);
                }
            } else {
                throw new RuntimeException("Illegal transition to SoftApStartingState: " + message);
            }
        }

        @Override
        public boolean processMessage(Message message) {
            logStateAndMessage(message, getClass().getSimpleName());

            switch(message.what) {
                case CMD_START_SUPPLICANT:
                case CMD_STOP_SUPPLICANT:
                case CMD_START_AP:
                case CMD_STOP_AP:
                case CMD_TETHER_STATE_CHANGE:
                case CMD_AIRPLANE_TOGGLED:
                    deferMessage(message);
                    break;
                case CMD_RESPONSE_AP_CONFIG:
                    WifiConfiguration config = (WifiConfiguration) message.obj;
                    if (config != null) {
                        startSoftApWithConfig(config);
                    } else {
                        loge("Softap config is null!");
                        sendMessage(CMD_START_AP_FAILURE, WigigManager.SAP_START_FAILURE_GENERAL);
                    }
                    break;
                case CMD_START_AP_SUCCESS:
                    setWigigApState(WIGIG_AP_STATE_ENABLED, 0);
                    transitionTo(mSoftApStartedState);
                    break;
                case CMD_START_AP_FAILURE:
                    setWigigApState(WIGIG_AP_STATE_FAILED, message.arg1);
                    transitionTo(mInitialState);
                    break;
                default:
                    return NOT_HANDLED;
            }
            return HANDLED;
        }
    }

    class SoftApStartedState extends State {
        @Override
        public boolean processMessage(Message message) {
            logStateAndMessage(message, getClass().getSimpleName());

            switch(message.what) {
                case CMD_STOP_AP:
                    if (DBG) log("Stopping Soft AP");
                    /* We have not tethered at this point, so we just shutdown soft Ap */
                    // first give hostapd a chance to terminate gracefully
                    boolean res = mWigigNative.stopHostapd();
                    if (!res) {
                        loge("Failed to teminate hostapd gracefully, try to shutdown forcibly");
                    } else {
                        try {
                            Thread.sleep(700);
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                            // continue
                        }
                    }
                    res = mWigigNative.killHostapd();
                    if (!res) {
                        loge("Failed to stop Wigig soft AP ");
                    }
                    setWigigApState(WIGIG_AP_STATE_DISABLED, 0);
                    transitionTo(mInitialState);
                    break;
                case CMD_START_AP:
                    // Ignore a start on a running access point
                    break;
                case CMD_START_SUPPLICANT:
                    // Fail client mode operation when soft AP is enabled
                    loge("Cannot start supplicant with a running soft AP");
                    setWigigState(WIGIG_STATE_UNKNOWN);
                    break;
                case CMD_TETHER_STATE_CHANGE:
                    TetherStateChange stateChange = (TetherStateChange) message.obj;
                    if (startTethering(stateChange.available)) {
                        transitionTo(mTetheringState);
                    }
                    break;
                case CMD_AIRPLANE_TOGGLED:
                    // message.arg1 contains newWigigMode which is irrelevant in AP mode
                    boolean airplaneModeOn = (message.arg2 == 1);

                    log("airplaneModeOn=" + airplaneModeOn);

                    if (airplaneModeOn) {
                        log("disabling AP due to airplane mode ON");
                        setHostApRunning(null, false);
                    }
                    break;
                default:
                    return NOT_HANDLED;
            }
            return HANDLED;
        }
    }

    class TetheringState extends State {
        @Override
        public void enter() {
            /* Send ourselves a delayed message to shut down if tethering fails to notify */
            sendMessageDelayed(obtainMessage(CMD_TETHER_NOTIFICATION_TIMED_OUT,
                    ++mTetherToken, 0), TETHER_NOTIFICATION_TIME_OUT_MSECS);
        }
        @Override
        public boolean processMessage(Message message) {
            logStateAndMessage(message, getClass().getSimpleName());

            switch(message.what) {
                case CMD_TETHER_STATE_CHANGE:
                    TetherStateChange stateChange = (TetherStateChange) message.obj;
                    if (isWigigTethered(stateChange.active)) {
                        transitionTo(mTetheredState);
                    }
                    return HANDLED;
                case CMD_TETHER_NOTIFICATION_TIMED_OUT:
                    if (message.arg1 == mTetherToken) {
                        loge("Failed to get tether update, shutdown soft access point");
                        transitionTo(mSoftApStartedState);
                        setWigigApState(WIGIG_AP_STATE_FAILED, WigigManager.SAP_START_FAILURE_GENERAL);
                        // Needs to be first thing handled
                        sendMessageAtFrontOfQueue(CMD_STOP_AP);
                    }
                    break;
                case CMD_START_SUPPLICANT:
                case CMD_STOP_SUPPLICANT:
                case CMD_START_AP:
                case CMD_STOP_AP:
                    deferMessage(message);
                    break;
                default:
                    return NOT_HANDLED;
            }
            return HANDLED;
        }
    }

    class TetheredState extends State {
        @Override
        public boolean processMessage(Message message) {
            logStateAndMessage(message, getClass().getSimpleName());

            switch(message.what) {
                case CMD_TETHER_STATE_CHANGE:
                    TetherStateChange stateChange = (TetherStateChange) message.obj;
                    if (!isWigigTethered(stateChange.active)) {
                        loge("Tethering reports wigig as untethered!, shut down soft Ap");
                        setHostApRunning(null, false);
                        setHostApRunning(null, true);
                    }
                    return HANDLED;
                case CMD_STOP_AP:
                    if (DBG) log("Untethering before stopping AP");
                    setWigigApState(WIGIG_AP_STATE_DISABLING, 0);
                    stopTethering();
                    transitionTo(mUntetheringState);
                    // More work to do after untethering
                    deferMessage(message);
                    break;
                default:
                    return NOT_HANDLED;
            }
            return HANDLED;
        }
    }

    class UntetheringState extends State {
        @Override
        public void enter() {
            /* Send ourselves a delayed message to shut down if tethering fails to notify */
            sendMessageDelayed(obtainMessage(CMD_TETHER_NOTIFICATION_TIMED_OUT,
                    ++mTetherToken, 0), TETHER_NOTIFICATION_TIME_OUT_MSECS);

        }
        @Override
        public boolean processMessage(Message message) {
            logStateAndMessage(message, getClass().getSimpleName());

            switch(message.what) {
                case CMD_TETHER_STATE_CHANGE:
                    TetherStateChange stateChange = (TetherStateChange) message.obj;

                    /* Wait till wigig is untethered */
                    if (isWigigTethered(stateChange.active)) break;

                    transitionTo(mSoftApStartedState);
                    break;
                case CMD_TETHER_NOTIFICATION_TIMED_OUT:
                    if (message.arg1 == mTetherToken) {
                        loge("Failed to get tether update, force stop access point");
                        transitionTo(mSoftApStartedState);
                    }
                    break;
                case CMD_START_SUPPLICANT:
                case CMD_STOP_SUPPLICANT:
                case CMD_START_AP:
                case CMD_STOP_AP:
                    deferMessage(message);
                    break;
                default:
                    return NOT_HANDLED;
            }
            return HANDLED;
        }
    }

    class WpsRunningState extends State {
        // Tracks the source to provide a reply
        private Message mSourceMessage;

        @Override
        public void enter() {
            mSourceMessage = Message.obtain(getCurrentMessage());
        }

        @Override
        public boolean processMessage(Message message) {
            logStateAndMessage(message, getClass().getSimpleName());

            switch (message.what) {
                case WigigMonitor.WPS_SUCCESS_EVENT:
                    // Ignore intermediate success, wait for full connection
                    break;
                case WigigMonitor.NETWORK_CONNECTION_EVENT:
                    replyToMessage(mSourceMessage, WigigManager.WPS_COMPLETED);
                    mSourceMessage.recycle();
                    mSourceMessage = null;
                    deferMessage(message);
                    transitionTo(mDisconnectedState);
                    break;
                case WigigMonitor.WPS_OVERLAP_EVENT:
                    replyToMessage(mSourceMessage, WigigManager.WPS_FAILED,
                            WigigManager.WPS_OVERLAP_ERROR);
                    mSourceMessage.recycle();
                    mSourceMessage = null;
                    transitionTo(mDisconnectedState);
                    break;
                case WigigMonitor.WPS_FAIL_EVENT:
                    // Arg1 has the reason for the failure
                    if ((message.arg1 != WigigManager.ERROR) || (message.arg2 != 0)) {
                        replyToMessage(mSourceMessage, WigigManager.WPS_FAILED, message.arg1);
                        mSourceMessage.recycle();
                        mSourceMessage = null;
                        transitionTo(mDisconnectedState);
                    } else {
                        if (DBG) log("Ignore unspecified fail event during WPS connection");
                    }
                    break;
                case WigigMonitor.WPS_TIMEOUT_EVENT:
                    replyToMessage(mSourceMessage, WigigManager.WPS_FAILED,
                            WigigManager.WPS_TIMED_OUT);
                    mSourceMessage.recycle();
                    mSourceMessage = null;
                    transitionTo(mDisconnectedState);
                    break;
                case WigigManager.START_WPS:
                    replyToMessage(message, WigigManager.WPS_FAILED, WigigManager.IN_PROGRESS);
                    break;
                case WigigManager.CANCEL_WPS:
                    if (mWigigNative.cancelWps()) {
                        replyToMessage(message, WigigManager.CANCEL_WPS_SUCCEDED);
                    } else {
                        replyToMessage(message, WigigManager.CANCEL_WPS_FAILED, WigigManager.ERROR);
                    }
                    transitionTo(mDisconnectedState);
                    break;
                /**
                 * Defer all commands that can cause connections to a different network
                 * or put the state machine out of connect mode
                 */
                case WigigManager.CONNECT_NETWORK:
                    deferMessage(message);
                    break;
                case CMD_START_SCAN:
                    return HANDLED;
                case WigigMonitor.NETWORK_DISCONNECTION_EVENT:
                    if (DBG) log("Network connection lost");
                    handleNetworkDisconnect();
                    break;
                case WigigMonitor.ASSOCIATION_REJECTION_EVENT:
                    if (DBG) log("Ignore Assoc reject event during WPS Connection");
                    break;
                case WigigMonitor.AUTHENTICATION_FAILURE_EVENT:
                    // Disregard auth failure events during WPS connection. The
                    // EAP sequence is retried several times, and there might be
                    // failures (especially for wps pin). We will get a WPS_XXX
                    // event at the end of the sequence anyway.
                    if (DBG) log("Ignore auth failure during WPS connection");
                    break;
                case WigigMonitor.SUPPLICANT_STATE_CHANGE_EVENT:
                    // Throw away supplicant state changes when WPS is running.
                    // We will start getting supplicant state changes once we get
                    // a WPS success or failure
                    break;
                default:
                    return NOT_HANDLED;
            }
            return HANDLED;
        }

        @Override
        public void exit() {
            mWigigConfigStore.enableAllNetworks();
            mWigigConfigStore.loadConfiguredNetworks();
        }
    }

    /**
     * State machine initiated requests can have replyTo set to null indicating
     * there are no recepients, we ignore those reply actions.
     */
    private void replyToMessage(Message msg, int what) {
        if (msg.replyTo == null) return;
        Message dstMsg = obtainMessageWithWhatAndArg2(msg, what);
        mReplyChannel.replyToMessage(msg, dstMsg);
    }

    private void replyToMessage(Message msg, int what, int arg1) {
        if (msg.replyTo == null) return;
        Message dstMsg = obtainMessageWithWhatAndArg2(msg, what);
        dstMsg.arg1 = arg1;
        mReplyChannel.replyToMessage(msg, dstMsg);
    }

    private void replyToMessage(Message msg, int what, Object obj) {
        if (msg.replyTo == null) return;
        Message dstMsg = obtainMessageWithWhatAndArg2(msg, what);
        dstMsg.obj = obj;
        mReplyChannel.replyToMessage(msg, dstMsg);
    }

    /**
     * arg2 on the source message has a unique id that needs to be retained in replies
     * to match the request
     * <p>see WigigManager for details
     */
    private Message obtainMessageWithWhatAndArg2(Message srcMsg, int what) {
        Message msg = Message.obtain();
        msg.what = what;
        msg.arg2 = srcMsg.arg2;
        return msg;
    }

    /**
     * @param wigigCredentialEventType WIGIG_CREDENTIAL_SAVED or WIGIG_CREDENTIAL_FORGOT
     * @param msg Must have a WifiConfiguration obj to succeed
     */
    private void broadcastWigigCredentialChanged(int wigigCredentialEventType,
            WifiConfiguration config) {
        if (config != null && config.preSharedKey != null) {
            Intent intent = new Intent(WigigManager.WIGIG_CREDENTIAL_CHANGED_ACTION);
            intent.putExtra(WigigManager.EXTRA_WIGIG_CREDENTIAL_SSID, config.SSID);
            intent.putExtra(WigigManager.EXTRA_WIGIG_CREDENTIAL_EVENT_TYPE,
                    wigigCredentialEventType);
            log("sending WIGIG_CREDENTIAL_CHANGED_ACTION, ssid=" + config.SSID
                + (wigigCredentialEventType == WigigManager.WIGIG_CREDENTIAL_SAVED ?
                    " saved" : " forgot"));
             mContext.sendBroadcastAsUser(intent, UserHandle.CURRENT,
                      android.Manifest.permission.RECEIVE_WIFI_CREDENTIAL_CHANGE);
        }
    }

    private static String getLinkPropertiesSummary(LinkProperties lp) {
        List<String> attributes = new ArrayList(6);
        if (lp.hasIPv4Address()) {
            attributes.add("v4");
        }
        if (lp.hasIPv4DefaultRoute()) {
            attributes.add("v4r");
        }
        if (lp.hasIPv4DnsServer()) {
            attributes.add("v4dns");
        }
        if (lp.hasGlobalIPv6Address()) {
            attributes.add("v6");
        }
        if (lp.hasIPv6DefaultRoute()) {
            attributes.add("v6r");
        }
        if (lp.hasIPv6DnsServer()) {
            attributes.add("v6dns");
        }

        return TextUtils.join(" ", attributes);
    }

    private void checkAndAddLocalRoute() {
        if (mAddedLocalRoute) {
            if (DBG) logd("Local route already added");
            return;
        }

        if (mNetworkAgent == null) {
            if (DBG) logd("No NetworkAgent (should never happen!)");
            return;
        }

        if (mNetworkInfo.getDetailedState() != DetailedState.CONNECTED) {
            if (DBG) logd("not connected, local route will not be added");
            return;
        }

        List<LinkAddress> addrs = mLinkProperties.getLinkAddresses();
        if (addrs.isEmpty()) {
            if (DBG) logd("WIGIG does not have IP address, skipping");
            return;
        }

        // add a local route to the WIGIG network, so it will always be accessible
        // even when internet connectivity is provided by another network such as WIFI
        for (LinkAddress addr : addrs) {
            if (DBG) logd("adding local route for WIGIG: " + addr);
            RouteInfo route = new RouteInfo(addr, null, mDataInterfaceName);
            try {
                mNwService.addLegacyRouteForNetId(mNetworkAgent.netId, route, 0);
            } catch (Exception e) {
                loge("Error adding route :" + e);
            }
        }

        mAddedLocalRoute = true;
    }
}
