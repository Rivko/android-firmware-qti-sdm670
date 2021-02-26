/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.server.wigigapp;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.ConnectivityManager;
import android.net.ConnectivityManager.NetworkCallback;
import android.net.Network;
import android.net.NetworkCapabilities;
import android.net.NetworkInfo;
import android.net.NetworkRequest;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiInfo;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.Messenger;
import android.os.SystemProperties;
import android.util.Log;

import com.android.internal.util.AsyncChannel;
import com.qualcomm.qti.wigig.WigigManager;

import java.util.Objects;

public class WigigSettingsService extends Service {
    private static final String TAG = "WigigSettingsService";
    private BroadcastReceiver mReceiver;
    private static final String NETWORKTYPE = "WIGIG";
    private ConnectivityManager mCm;
    private NotificationManager mNm;
    private static final int NOTIFICATION_ID = 10002;
    private PendingIntent mContentIntent;
    private WigigManager mWigigManager;
    private AsyncChannel mWigigChannel;
    private State mCurrentState = new State();
    private static final String CHANNEL_ID_DEFAULT =
        "com.qualcomm.qti.server.wigigapp.DEFAULT_CHANNEL";

    // 1st dimension is internet connectivity
    // 2nd dimension is data activity
    // 3rd dimension is signal level
    private static int[][][] wigig_sb_icon = {
            {
                    {
                            R.drawable.sb_wigig_0,
                            R.drawable.sb_wigig_1,
                            R.drawable.sb_wigig_2,
                            R.drawable.sb_wigig_3,
                            R.drawable.sb_wigig_4
                    },
                    {
                            R.drawable.sb_wigig_downlink_0,
                            R.drawable.sb_wigig_downlink_1,
                            R.drawable.sb_wigig_downlink_2,
                            R.drawable.sb_wigig_downlink_3,
                            R.drawable.sb_wigig_downlink_4
                    },
                    {
                            R.drawable.sb_wigig_uplink_0,
                            R.drawable.sb_wigig_uplink_1,
                            R.drawable.sb_wigig_uplink_2,
                            R.drawable.sb_wigig_uplink_3,
                            R.drawable.sb_wigig_uplink_4
                    },
                    {
                            R.drawable.sb_wigig_bidir_0,
                            R.drawable.sb_wigig_bidir_1,
                            R.drawable.sb_wigig_bidir_2,
                            R.drawable.sb_wigig_bidir_3,
                            R.drawable.sb_wigig_bidir_4
                    }
            },
            {
                    {
                            R.drawable.sb_wigig_noinet_0,
                            R.drawable.sb_wigig_noinet_1,
                            R.drawable.sb_wigig_noinet_2,
                            R.drawable.sb_wigig_noinet_3,
                            R.drawable.sb_wigig_noinet_4
                    },
                    {
                            R.drawable.sb_wigig_noinet_downlink_0,
                            R.drawable.sb_wigig_noinet_downlink_1,
                            R.drawable.sb_wigig_noinet_downlink_2,
                            R.drawable.sb_wigig_noinet_downlink_3,
                            R.drawable.sb_wigig_noinet_downlink_4
                    },
                    {
                            R.drawable.sb_wigig_noinet_uplink_0,
                            R.drawable.sb_wigig_noinet_uplink_1,
                            R.drawable.sb_wigig_noinet_uplink_2,
                            R.drawable.sb_wigig_noinet_uplink_3,
                            R.drawable.sb_wigig_noinet_uplink_4
                    },
                    {
                            R.drawable.sb_wigig_noinet_bidir_0,
                            R.drawable.sb_wigig_noinet_bidir_1,
                            R.drawable.sb_wigig_noinet_bidir_2,
                            R.drawable.sb_wigig_noinet_bidir_3,
                            R.drawable.sb_wigig_noinet_bidir_4
                    }
            }

    };

    @Override
    public void onCreate() {
        Log.i(TAG, "onCreate");
        super.onCreate();

        if (SystemProperties.getBoolean("persist.vendor.wigig.icon.disabled", false)) {
            // Wigig icon display disabled. quit the service
            Log.i(TAG, "Wigig icon display disabled");
            return;
        }

        mWigigManager = WigigManager.getInstance(this);
        if (mWigigManager == null) {
            // Wigig Manager not present (or not loaded). quit the service
            Log.e(TAG, "Failed to get WigigManager instance");
            return;
        }
        mCm = (ConnectivityManager) getSystemService(Context.CONNECTIVITY_SERVICE);
        mNm = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
        mContentIntent = PendingIntent.getActivity(this, 0,
                new Intent(this, WigigSettingsActivity.class), 0);
        createNotificationChannels();

        Handler handler = new WigigHandler();
        mWigigChannel = new AsyncChannel();
        Messenger wigigMessenger = mWigigManager.getWigigServiceMessenger();
        if (wigigMessenger != null) {
            mWigigChannel.connect(this, handler, wigigMessenger);
        }

        final NetworkRequest request =
                new NetworkRequest.Builder()
                        .addTransportType(NetworkCapabilities.TRANSPORT_WIFI)
                        .build();
        NetworkCallback networkCallback = new ConnectivityManager.NetworkCallback() {
            private void updateState(Network network) {
                State newState = new State(mCurrentState);
                newState.inetCondition = false;
                NetworkInfo ni = mCm.getNetworkInfo(network);
                if (ni == null || !ni.getTypeName().equals(NETWORKTYPE)) {
                    return;
                }
                NetworkCapabilities nc = mCm.getNetworkCapabilities(network);
                if (nc != null && nc.hasCapability(NetworkCapabilities.NET_CAPABILITY_VALIDATED)) {
                    newState.inetCondition = true;
                }

                applyNotification(newState);
            }

            @Override
            public void onAvailable(Network network) {
                Log.i(TAG, "onAvailable, netId " + network.netId);
                updateState(network);
            }
            @Override
            public void onCapabilitiesChanged(Network network,
                                              NetworkCapabilities networkCapabilities) {
                Log.i(TAG, "onCapabilitiesChanged, netId " + network.netId + " capa: " + networkCapabilities);
                updateState(network);
            }
        };
        mCm.registerNetworkCallback(request, networkCallback);

        final IntentFilter filter = new IntentFilter();
        filter.addAction(WigigManager.NETWORK_STATE_CHANGED_ACTION);
        filter.addAction(WigigManager.WIGIG_AP_STATE_CHANGED_ACTION);
        filter.addAction(WigigManager.WIGIG_RATE_UPGRADE_STATE_CHANGED_ACTION);
        filter.addAction(WigigManager.RSSI_CHANGED_ACTION);

        mReceiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                Log.i(TAG, "received " + intent);
                final String action = intent.getAction();
                State newState = new State(mCurrentState);

                // check wigig enabled/disabled
                if (action.equals(WigigManager.NETWORK_STATE_CHANGED_ACTION)) {
                    try {
                        NetworkInfo networkInfo = intent.getParcelableExtra(WigigManager.EXTRA_NETWORK_INFO);
                        NetworkInfo.DetailedState ds = networkInfo.getDetailedState();
                        if (ds == NetworkInfo.DetailedState.CONNECTED) {
                            WifiInfo wifiInfo = mWigigManager.getConnectionInfo();
                            newState.connected = true;
                            newState.ssid = wifiInfo.getSSID().replace("\"", "");
                        } else {
                            newState = cleanState();
                        }
                    } catch (Exception e) {
                        newState = cleanState();
                    }
                }

                // signal level changed
                if (action.equals(WigigManager.RSSI_CHANGED_ACTION)) {
                    newState.rssi = intent.getIntExtra(WigigManager.EXTRA_NEW_RSSI, 0);
                }

                // check wigig AP mode
                if (action.equals(WigigManager.WIGIG_AP_STATE_CHANGED_ACTION)) {
                    try {
                        int apState = intent.getIntExtra(WigigManager.EXTRA_WIGIG_AP_STATE,
                                WigigManager.WIGIG_AP_STATE_DISABLED);
                        if (apState == WigigManager.WIGIG_AP_STATE_ENABLED) {
                            newState.ap = true;
                            WifiConfiguration wifiConfig = mWigigManager.getWigigApConfiguration();
                            if (wifiConfig != null) {
                                newState.ssid = wifiConfig.SSID;
                            }
                        } else {
                            newState = cleanState();
                        }
                    } catch (Exception e) {
                        newState = cleanState();
                    }
                }

                // check FST mode
                if (action.equals(WigigManager.WIGIG_RATE_UPGRADE_STATE_CHANGED_ACTION)) {
                    int state = intent.getIntExtra(WigigManager.EXTRA_WIGIG_RATE_UPGRADE_STATE,
                            WigigManager.WIGIG_RATE_UPGRADE_STATE_DISCONNECTED);
                    newState.fst_connected =
                            (state == WigigManager.WIGIG_RATE_UPGRADE_STATE_CONNECTED);
                }

                applyNotification(newState);
            }
        };

        registerReceiver(mReceiver, filter);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        unregisterReceiver(mReceiver);
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    private void applyNotification(State newState) {
        Log.i(TAG, "newState: connected " + newState.connected + " ap " + newState.ap +
                " ssid " + newState.ssid + " rssi " + newState.rssi +
                " activityIn " + ((newState.dataActivity & WigigManager.DATA_ACTIVITY_IN) != 0) +
                " activityOut " + ((newState.dataActivity & WigigManager.DATA_ACTIVITY_OUT) != 0) +
                " inetCondition " + newState.inetCondition +
                " fst_connected " + newState.fst_connected);

        if (newState.equals(mCurrentState)) {
            return;
        }
        mCurrentState.copyFrom(newState);

        Notification.Builder nb = null;
        if (mCurrentState.ap) {
            nb = new Notification.Builder(this, CHANNEL_ID_DEFAULT)
                    .setSmallIcon(R.drawable.sb_wigig_hotspot)
                    .setContentTitle(mCurrentState.ssid)
                    .setContentIntent(mContentIntent);
        } else if (mCurrentState.fst_connected) {
            nb = new Notification.Builder(this, CHANNEL_ID_DEFAULT)
                    .setSmallIcon(R.drawable.sb_wigig_fst)
                    .setContentTitle("FST Mode")
                    .setContentIntent(mContentIntent);
        } else if (mCurrentState.connected) {
            int inetIndex = mCurrentState.inetCondition ? 0 : 1;
            int signalIndex = WigigManager.calculateSignalLevel(newState.rssi,
                    WigigManager.RSSI_LEVELS);
            if (mCurrentState.dataActivity > WigigManager.DATA_ACTIVITY_INOUT) {
                Log.e(TAG, "bad icon array index");
                return;
            }
            nb = new Notification.Builder(this, CHANNEL_ID_DEFAULT)
                    .setSmallIcon(wigig_sb_icon[inetIndex][mCurrentState.dataActivity][signalIndex])
                    .setContentTitle(mCurrentState.ssid)
                    .setContentIntent(mContentIntent);
        }

        if (nb != null) {
            Notification notification = nb.build();
            notification.flags |= Notification.FLAG_NO_CLEAR | Notification.FLAG_ONGOING_EVENT;

            mNm.notify(NOTIFICATION_ID, notification);
        } else {
            mNm.cancel(NOTIFICATION_ID);
        }
    }

    void setActivity(int wigigActivity) {
        State newState = new State(mCurrentState);
        newState.dataActivity = wigigActivity;
        applyNotification(newState);
    }

    /**
     * Handler to receive the data activity on wigig.
     */
    private class WigigHandler extends Handler {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case AsyncChannel.CMD_CHANNEL_HALF_CONNECTED:
                    if (msg.arg1 == AsyncChannel.STATUS_SUCCESSFUL) {
                        mWigigChannel.sendMessage(Message.obtain(this,
                                AsyncChannel.CMD_CHANNEL_FULL_CONNECTION));
                    } else {
                        Log.e(TAG, "Failed to connect to wigig");
                    }
                    break;
                case WigigManager.DATA_ACTIVITY_NOTIFICATION:
                    setActivity(msg.arg1);
                    break;
                default:
                    // Ignore
                    break;
            }
        }
    }

    protected State cleanState() {
        return new State();
    }

    private class State {
        boolean connected;
        boolean ap;
        String ssid;
        int dataActivity;
        boolean inetCondition;
        boolean fst_connected;
        int rssi;

        public State() {
        }

        public State(State s) {
            copyFrom(s);
        }

        public void copyFrom(State s) {
            connected = s.connected;
            ap = s.ap;
            ssid = s.ssid;
            dataActivity = s.dataActivity;
            inetCondition = s.inetCondition;
            fst_connected = s.fst_connected;
            rssi = s.rssi;
        }

        public boolean equals(Object o) {
            if (!o.getClass().equals(getClass())) {
                return false;
            }
            State other = (State) o;
            return other.connected == connected
                    && other.ap == ap
                    && Objects.equals(other.ssid, ssid)
                    && other.inetCondition == inetCondition
                    && other.dataActivity == dataActivity
                    && other.fst_connected == fst_connected
                    && other.rssi == rssi;
        }
    }

    private void createNotificationChannels() {
        NotificationChannel defaultChannel =
                new NotificationChannel(
                        CHANNEL_ID_DEFAULT,
                        getString(R.string.notification_channel_default_name),
                        NotificationManager.IMPORTANCE_LOW);
        mNm.createNotificationChannel(defaultChannel);
    }
}
