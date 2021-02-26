/* ====================================================================
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * =====================================================================
 * @file WirelessManager.java
 *
 */

package com.qti.snapdragon.qdcm_ff;

import android.content.Context;
import android.content.BroadcastReceiver;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiManager;
import android.net.NetworkInfo;
import android.net.Network;
import android.net.ConnectivityManager;
import android.os.SystemClock;
import android.util.Log;
import java.util.List;

public class WirelessManager {

    private static final String LOG_TAG = "QDCM_FF_Mobile";
    private static final String AP_CONNECTED_INTENT = "com.qti.snapdragon.qdcm_ff.AP_CONNECTED";

    private Context mContext;
    private WifiConfiguration mWifiConf;
    private WifiManager mWifiMgr;
    private int mNetId;
    private static boolean mConnectedToAP = false;
    private Intent mAPConnectedIntent;

    WirelessManager (Context context) {
        mContext = context;

        // Intent for Wifi state (enable/disable) changes
        IntentFilter intent = new IntentFilter();
        intent.addAction(WifiManager.NETWORK_STATE_CHANGED_ACTION);
        mContext.registerReceiver(mReceiver, intent);

        // Custom Intent for when connection to AP is established
        mAPConnectedIntent = new Intent(AP_CONNECTED_INTENT);
    }

    private BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();
            if (action.equals(WifiManager.NETWORK_STATE_CHANGED_ACTION)) {

                NetworkInfo info = intent.getParcelableExtra(WifiManager.EXTRA_NETWORK_INFO);
                if (info != null && !info.isConnected()) {
                    mConnectedToAP = false;
                } else if (info.isConnected()) {
                    if ( !mConnectedToAP ) {
                        mConnectedToAP = true;
                        SystemClock.sleep(4000);
                        // Send custom intent to receiver as signal
                        // start diag_socket_log app
                        context.sendBroadcast(mAPConnectedIntent);
                    }
                }
            }
        }
    };

    public boolean startWifi(String ssid, String networkPass) {

        if (ssid.isEmpty() || networkPass.isEmpty())
            return false;

        try {
            mWifiMgr = (WifiManager)mContext.getSystemService(Context.WIFI_SERVICE);
            boolean wifiEnabled = mWifiMgr.isWifiEnabled();
            if (!wifiEnabled) {
                wifiEnabled = mWifiMgr.setWifiEnabled(true);
            }
            Log.d(LOG_TAG, "Wifi Enabled: " + wifiEnabled) ;

            // Setup wireless access to AP/Router
            mWifiConf = new WifiConfiguration();
            mWifiConf.SSID = "\"" + ssid + "\"";
            mWifiConf.preSharedKey = "\"" + networkPass + "\"";
            mWifiConf.hiddenSSID = true;
            mWifiConf.status = WifiConfiguration.Status.ENABLED;
            mWifiConf.allowedKeyManagement.set(WifiConfiguration.KeyMgmt.WPA_PSK);
            mWifiConf.allowedProtocols.set(WifiConfiguration.Protocol.RSN);
            mNetId = mWifiMgr.updateNetwork(mWifiConf);

            if (mNetId == -1) {
                mNetId = mWifiMgr.addNetwork(mWifiConf);
                if (mNetId == -1) {
                    Log.d(LOG_TAG, "Couldn't connect to AP/Router.");
                }
            }
        }
        catch (Exception e) {
            Log.d(LOG_TAG, "startWifi(): " + e.getMessage());
        }

        return connectToAccessPoint();
    }

    public boolean stopWifi() {
       boolean ret = false;
       if (isConnectedToAccessPoint()) {
          ret = disconnectFromAccessPoint();
       }
       return ret;
    }

    private boolean connectToAccessPoint() {
        boolean connected = isConnectedToAccessPoint();
        if ( !connected ) {
            boolean ret = false;

            // If mNetId = -1, network is already added to conf. file.
            // Bug for M builds only.
            if (mNetId != -1) {
                ret = mWifiMgr.enableNetwork(mNetId, true);
            }
            ret = mWifiMgr.reconnect();
            connected = ret;
         }
        return connected;
    }

    private boolean disconnectFromAccessPoint() {
        boolean ret = false;
        try {
            int networkId = mWifiMgr.getConnectionInfo().getNetworkId();
            ret = mWifiMgr.setWifiEnabled(false);
            ret = mWifiMgr.disconnect();
            ret = mWifiMgr.removeNetwork(networkId);
            ret = mWifiMgr.saveConfiguration();
        } catch (Exception e) {
            Log.d(LOG_TAG, "disconnectFromAccessPoint(): " + e.getMessage());
            ret = false;
        } finally {
            mWifiConf = null;
            mWifiMgr = null;
        }
        return ret;
    }

    private boolean isConnectedToAccessPoint() {
        ConnectivityManager cm = (ConnectivityManager)mContext.getSystemService(Context.CONNECTIVITY_SERVICE);
        try {
            // Code below works only for >= Lolipop OS
            Network[] networks = cm.getAllNetworks();
            NetworkInfo networkInfo;
            for (Network nw : networks) {
                networkInfo = cm.getNetworkInfo(nw);
                if (networkInfo.getState().equals(NetworkInfo.State.CONNECTED)) {
                    return true;
                }
            }
        } catch (Exception e) {
           Log.d(LOG_TAG, "Unable check status of connection");
        }
        return false;
    }
}
