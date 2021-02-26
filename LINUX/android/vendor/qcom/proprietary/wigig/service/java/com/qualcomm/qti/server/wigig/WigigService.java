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

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiInfo;
import android.os.Binder;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.os.Messenger;
import android.os.RemoteException;
import android.os.SystemProperties;
import android.util.Log;

import com.android.internal.util.AsyncChannel;
import com.android.server.SystemService;
import com.qualcomm.qti.wigig.IWigigManager;
import com.qualcomm.qti.wigig.ScanResult;
import com.qualcomm.qti.wigig.WigigManager;

import java.util.List;

public final class WigigService extends IWigigManager.Stub {

    private static final String TAG = "WigigService";
    private static final boolean DBG = true;

    private String mInterfaceName;

    final WigigStateMachine mWigigStateMachine;
    final WigigNetworkMonitor mWigigNetworkMonitor;

    private final Context mContext;

    /* Polls traffic stats and notifies clients */
    private WigigTrafficPoller mTrafficPoller;
    /* Tracks the persisted states for Wigig */
    private WigigSettingsStore mSettingsStore;
    /**
     * Asynchronous channel to WigigStateMachine
     */
    private AsyncChannel mWigigStateMachineChannel;

    /**
     * Handles client connections
     */
    private class ClientHandler extends Handler {

        ClientHandler(android.os.Looper looper) {
            super(looper);
        }

        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case AsyncChannel.CMD_CHANNEL_HALF_CONNECTED: {
                    if (msg.arg1 == AsyncChannel.STATUS_SUCCESSFUL) {
                        if (DBG) Log.d(TAG, "New client listening to asynchronous messages");
                        // We track the clients by the Messenger
                        // since it is expected to be always available
                        mTrafficPoller.addClient(msg.replyTo);
                    } else {
                        Log.e(TAG, "Client connection failure, error=" + msg.arg1);
                    }
                    break;
                }
                case AsyncChannel.CMD_CHANNEL_DISCONNECTED: {
                    if (msg.arg1 == AsyncChannel.STATUS_SEND_UNSUCCESSFUL) {
                        if (DBG) Log.d(TAG, "Send failed, client connection lost");
                    } else {
                        if (DBG) Log.d(TAG, "Client connection lost with reason: " + msg.arg1);
                    }
                    mTrafficPoller.removeClient(msg.replyTo);
                    break;
                }
                case AsyncChannel.CMD_CHANNEL_FULL_CONNECTION: {
                    AsyncChannel ac = new AsyncChannel();
                    ac.connect(mContext, this, msg.replyTo);
                    break;
                }
                /* Client commands are forwarded to state machine */
                case WigigManager.CONNECT_NETWORK:
                case WigigManager.SAVE_NETWORK: {
                    WifiConfiguration config = (WifiConfiguration) msg.obj;
                    int networkId = msg.arg1;
                    if (msg.what == WigigManager.SAVE_NETWORK) {
                        Log.i(TAG, "SAVE"
                                + " nid=" + Integer.toString(networkId)
                                + " uid=" + msg.sendingUid
                                + " name="
                                + mContext.getPackageManager().getNameForUid(msg.sendingUid));
                    }
                    if (msg.what == WigigManager.CONNECT_NETWORK) {
                        Log.i(TAG, "CONNECT "
                                + " nid=" + Integer.toString(networkId)
                                + " uid=" + msg.sendingUid
                                + " name="
                                + mContext.getPackageManager().getNameForUid(msg.sendingUid));
                    }

                    if (config != null && isValid(config)) {
                        if (DBG) Log.d(TAG, "Connect with config" + config);
                        mWigigStateMachine.sendMessage(Message.obtain(msg));
                    } else if (config == null
                            && networkId != WifiConfiguration.INVALID_NETWORK_ID) {
                        if (DBG) Log.d(TAG, "Connect with networkId=" + networkId);
                        mWigigStateMachine.sendMessage(Message.obtain(msg));
                    } else {
                        Log.e(TAG, "ClientHandler.handleMessage ignoring invalid msg=" + msg);
                        if (msg.what == WigigManager.CONNECT_NETWORK) {
                            replyFailed(msg, WigigManager.CONNECT_NETWORK_FAILED,
                                    WigigManager.INVALID_ARGS);
                        } else {
                            replyFailed(msg, WigigManager.SAVE_NETWORK_FAILED,
                                    WigigManager.INVALID_ARGS);
                        }
                    }
                    break;
                }
                case WigigManager.FORGET_NETWORK:
                    mWigigStateMachine.sendMessage(Message.obtain(msg));
                    break;
                case WigigManager.START_WPS:
                case WigigManager.CANCEL_WPS:
                case WigigManager.DISABLE_NETWORK: {
                    mWigigStateMachine.sendMessage(Message.obtain(msg));
                    break;
                }
                default: {
                    Log.d(TAG, "ClientHandler.handleMessage ignoring msg=" + msg);
                    break;
                }
            }
        }

        private void replyFailed(Message msg, int what, int why) {
            Message reply = msg.obtain();
            reply.what = what;
            reply.arg1 = why;
            try {
                msg.replyTo.send(reply);
            } catch (RemoteException e) {
                // There's not much we can do if reply can't be sent!
            }
        }
    }
    private ClientHandler mClientHandler;

    /**
     * Handles interaction with WigigStateMachine
     */
    private class WigigStateMachineHandler extends Handler {
        private AsyncChannel mWsmChannel;

        WigigStateMachineHandler(android.os.Looper looper) {
            super(looper);
            mWsmChannel = new AsyncChannel();
            mWsmChannel.connect(mContext, this, mWigigStateMachine.getHandler());
        }

        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case AsyncChannel.CMD_CHANNEL_HALF_CONNECTED: {
                    if (msg.arg1 == AsyncChannel.STATUS_SUCCESSFUL) {
                        mWigigStateMachineChannel = mWsmChannel;
                    } else {
                        Log.e(TAG, "WigigStateMachine connection failure, error=" + msg.arg1);
                        mWigigStateMachineChannel = null;
                    }
                    break;
                }
                case AsyncChannel.CMD_CHANNEL_DISCONNECTED: {
                    Log.e(TAG, "WigigStateMachine channel lost, msg.arg1 =" + msg.arg1);
                    mWigigStateMachineChannel = null;
                    // Re-establish connection to state machine
                    mWsmChannel.connect(mContext, this, mWigigStateMachine.getHandler());
                    break;
                }
                default: {
                    Log.d(TAG, "WigigStateMachineHandler.handleMessage ignoring msg=" + msg);
                    break;
                }
            }
        }
    }

    WigigStateMachineHandler mWigigStateMachineHandler;

    public WigigService(Context context) {
        mContext = context;

        mInterfaceName = SystemProperties.get("vendor.wigig.interface", "wigig0");

        mWigigNetworkMonitor = new WigigNetworkMonitor(mContext, mInterfaceName);
        mWigigStateMachine = new WigigStateMachine(mContext, mInterfaceName, mWigigNetworkMonitor);

        mSettingsStore = new WigigSettingsStore(mContext);

        HandlerThread wigigThread = new HandlerThread("WigigService");
        wigigThread.start();
        mClientHandler = new ClientHandler(wigigThread.getLooper());
        mWigigStateMachineHandler = new WigigStateMachineHandler(wigigThread.getLooper());
        mTrafficPoller =  new WigigTrafficPoller(mContext, wigigThread.getLooper(),
                mInterfaceName);

        Log.i(TAG, "WigigService created. mInterfaceName=" + mInterfaceName);
    }

    public void onBootPhase(int phase) {
        if (phase == SystemService.PHASE_SYSTEM_SERVICES_READY) {
            checkAndStartWigig();
        }
    }

    /**
     * Check if Wigig needs to be enabled and start if needed
     *
     * This function is used only at boot time
     */
    public void checkAndStartWigig() {
        /* Check if Wigig needs to be enabled */
        boolean wigigEnabled = mSettingsStore.getWigigOn();
        Log.i(TAG, "starting with Wigig " + (wigigEnabled ? "enabled" : "disabled"));

        mContext.registerReceiver(
                new BroadcastReceiver() {
                    @Override
                    public void onReceive(Context context, Intent intent) {
                        if (mSettingsStore.handleAirplaneModeToggled()) {
                            mWigigStateMachine.sendMessage(WigigStateMachine.CMD_AIRPLANE_TOGGLED,
                                    mSettingsStore.getWigigOn() ? 1 : 0,
                                    mSettingsStore.isAirplaneModeOn() ? 1 : 0);
                        }
                    }
                },
                new IntentFilter(Intent.ACTION_AIRPLANE_MODE_CHANGED));

        // If we are already disabled (could be due to airplane mode), avoid changing persist
        // state here
        if (wigigEnabled) setWigigEnabled(wigigEnabled);
    }

    /**
     * see {@link com.qualcomm.qti.wigig.WigigManager#startScan}
     */
    public void startScan() {
        enforceChangePermission();
        Log.i(TAG, "startScan");

        mWigigStateMachine.startScan();
    }

    /**
     * Return the results of the most recent access point scan, in the form of
     * a list of {@link ScanResult} objects.
     * @return the list of results
     */
    public List<ScanResult> getScanResults() {
        enforceAccessPermission();
        Log.i(TAG, "getScanResults");

        return mWigigStateMachine.syncGetScanResultsList();
    }

    private void enforceAccessPermission() {
        mContext.enforceCallingOrSelfPermission(android.Manifest.permission.ACCESS_WIFI_STATE,
                "WigigService");
    }

    private void enforceChangePermission() {
        mContext.enforceCallingOrSelfPermission(android.Manifest.permission.CHANGE_WIFI_STATE,
                "WigigService");
    }

    /**
     * see {@link com.qualcomm.qti.wigig.WigigManager#setWigigEnabled(boolean)}
     * @param enable {@code true} to enable, {@code false} to disable.
     * @return {@code true} if the enable/disable operation was
     *         started or is already in the queue.
     */
    public boolean setWigigEnabled(boolean enable) {
        enforceChangePermission();
        Log.i(TAG, (enable ? "enabling" : "disabling") + " Wigig");

        /*
         * Caller might not have WRITE_SECURE_SETTINGS,
         * only CHANGE_WIFI_STATE is enforced
         */

        long ident = Binder.clearCallingIdentity();
        try {
            mSettingsStore.handleWigigToggled(enable);
        } finally {
            Binder.restoreCallingIdentity(ident);
        }

        mWigigStateMachine.setSupplicantRunning(enable);

        return true;
    }

    /**
     * see {@link com.qualcomm.qti.wigig.WigigManager#setWigigApEnabled(WifiConfiguration, boolean)}
     *
     * @param wifiConfig SSID, security and channel details as
     *        part of WifiConfiguration
     * @param enabled true to enable and false to disable
     */
    public void setWigigApEnabled(WifiConfiguration wifiConfig, boolean enabled) {
        enforceChangePermission();
        Log.i(TAG, (enabled ? "enabling" : "disabling") + " Wigig AP");

        // null wifiConfig is a meaningful input for setHostApRunning
        if (wifiConfig == null || isValid(wifiConfig)) {
            mWigigStateMachine.setHostApRunning(wifiConfig, enabled);
        } else {
            Log.e(TAG, "Invalid WifiConfiguration");
        }
    }

    /**
     * see {@link WigigManager#getWigigApState()}
     *
     * @return One of {@link WigigManager#WIGIG_AP_STATE_DISABLED},
     *         {@link WigigManager#WIGIG_AP_STATE_DISABLING},
     *         {@link WigigManager#WIGIG_AP_STATE_ENABLED},
     *         {@link WigigManager#WIGIG_AP_STATE_ENABLING},
     *         {@link WigigManager#WIGIG_AP_STATE_FAILED}
     */
    public int getWigigApEnabledState() {
        enforceAccessPermission();
        Log.i(TAG, "getWigigApEnabledState: state=" + mWigigStateMachine.syncGetWigigApStateByName());

        return mWigigStateMachine.syncGetWigigApState();
    }

    /**
     * see {@link WigigManager#getWigigApConfiguration()}
     *
     * @return soft access point configuration
     */
    public WifiConfiguration getWigigApConfiguration() {
        enforceAccessPermission();
        return mWigigStateMachine.syncGetWigigApConfiguration();
    }

    /**
     * see {@link WigigManager#setWigigApConfiguration(WifiConfiguration)}
     *
     * @param wifiConfig WifiConfiguration details for soft access point
     */
    public void setWigigApConfiguration(WifiConfiguration wifiConfig) {
        enforceChangePermission();
        Log.i(TAG, "setWigigApConfiguration");

        if (wifiConfig == null)
            return;
        if (isValid(wifiConfig)) {
            mWigigStateMachine.setWigigApConfiguration(wifiConfig);
        } else {
            Log.e(TAG, "Invalid WifiConfiguration");
        }
    }

    /**
     * see {@link com.qualcomm.qti.wigig.WigigManager#disconnect()}
     */
    public void disconnect() {
        enforceChangePermission();
        Log.i(TAG, "disconnect");

        mWigigStateMachine.disconnectCommand();
    }

    /**
     * see {@link com.qualcomm.qti.wigig.WigigManager#getConfiguredNetworks()}
     * @return the list of configured networks
     */
    public List<WifiConfiguration> getConfiguredNetworks() {
        enforceAccessPermission();
        Log.i(TAG, "getConfiguredNetworks");

        if (mWigigStateMachineChannel != null) {
            return mWigigStateMachine.syncGetConfiguredNetworks(Binder.getCallingUid(),
                    mWigigStateMachineChannel);
        } else {
            Log.e(TAG, "mWigigStateMachineChannel is not initialized");
            return null;
        }
    }

    /**
     * See {@link com.qualcomm.qti.wigig.WigigManager#getConnectionInfo()}
     * @return the Wigig information, contained in {@link WifiInfo}.
     */
    public WifiInfo getConnectionInfo() {
        enforceAccessPermission();
        Log.i(TAG, "getConnectionInfo");
        /*
         * Make sure we have the latest information, by sending
         * a status request to the supplicant.
         */
        return mWigigStateMachine.syncRequestConnectionInfo();
    }

    /**
     * Return whether Wigig is enabled or disabled.
     * @return {@code true} if Wigig is enabled
     * @see #getWigiState()
     */
    public boolean isWigigEnabled() {
        enforceAccessPermission();
        int state = mWigigStateMachine.syncGetWigigState();

        Log.i(TAG, "isWigigEnabled: state=" + mWigigStateMachine.syncGetWigigStateByName());

        return state == WigigManager.WIGIG_STATE_ENABLED;
    }

    /**
     * Enable or disable Wigig Rate Upgrade mode
     *
     * @param enable {@code true} to enable, {@code false} to
     *               disable
     * @return       {@code true} if the request to
     *              enable/disable the Rate Upgrade was accepted
     */
    public boolean setRateUpgradeEnabled(boolean enable) {
        enforceChangePermission();
        Log.i(TAG, (enable ? "enabling" : "disabling") + " Rate Upgrade");

        mWigigStateMachine.setRateUpgradeEnabled(enable);

        return true;
    }

    /**
     * Return whether Wigig rate upgrade mode enabled or not
     */
    public boolean isRateUpgradeEnabled() {
        enforceAccessPermission();
        int RateUpgradeState = mWigigStateMachine.syncGetWigigState();

        return (RateUpgradeState == WigigManager.WIGIG_STATE_RATE_UPGRADE);
    }

    /**
     * Get a reference to handler. This is used by a client to establish
     * an AsyncChannel communication with WifiService
     */
    public Messenger getWigigServiceMessenger() {
        enforceAccessPermission();
        enforceChangePermission();
        return new Messenger(mClientHandler);
    }

    /**
     * Get the IP and proxy configuration file
     */
    public String getConfigFile() {
        enforceAccessPermission();
        String fileName = mWigigStateMachine.getConfigFile();

        Log.i(TAG, "getConfigFile, returning " + fileName);

        return fileName;
    }

    public static boolean isValid(WifiConfiguration config) {
        String validity = checkValidity(config);

        if (validity == null) {
            return true;
        }

        Log.d(TAG, validity);
        return  false;
    }

    public static String checkValidity(WifiConfiguration config) {
        if (config.allowedKeyManagement == null || config.allowedKeyManagement.cardinality() == 0)
            return "allowed kmgmt";

        if (config.allowedKeyManagement.cardinality() > 1) {
            if (config.allowedKeyManagement.cardinality() != 2) {
                return "cardinality != 2";
            }
            if (!config.allowedKeyManagement.get(WifiConfiguration.KeyMgmt.WPA_EAP)) {
                return "not WPA_EAP";
            }
            if ((!config.allowedKeyManagement.get(WifiConfiguration.KeyMgmt.IEEE8021X))
                    && (!config.allowedKeyManagement.get(WifiConfiguration.KeyMgmt.WPA_PSK))) {
                return "not PSK or 8021X";
            }
        }
        return null;
    }
}
