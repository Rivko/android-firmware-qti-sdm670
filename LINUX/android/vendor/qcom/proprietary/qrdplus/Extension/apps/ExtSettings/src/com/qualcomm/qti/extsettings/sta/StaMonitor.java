/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.extsettings.sta;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.ConnectivityManager;
import android.net.wifi.WifiDevice;
import android.util.Log;

import java.util.ArrayList;
import java.util.List;


/**
 * Sta monitor and manager. If you want it available you
 * should call {@link StaMonitor#startMonitor()}
 * And call {@link StaMonitor#stopMonitor()} to close it.
 */
public class StaMonitor {

    private static final String TAG = StaMonitor.class.getSimpleName();

    private static final String PRE_NAME = "sta_device_info";

    private static final String KYE_STA_DEVICE = "list";

    private static final String STA_CHANGED_BROADCAST =
            "codeaurora.net.conn.TETHER_CONNECT_STATE_CHANGED";
    private static final String TRIGGER_STA_BROADCAST =
            "codeaurora.net.conn.TETHER_CONNECTED_LIST";

    private final List<StaChangedListener> mListener = new ArrayList<>();
    private final List<StaDevice> mStaDevices = new ArrayList<>();

    private final Context mContext;

    public StaMonitor(Context context) {
        mContext = context;
    }

    /**
     * Start Listen Sta change notification.
     */
    public void startMonitor() {
        IntentFilter filter = new IntentFilter(STA_CHANGED_BROADCAST);
        mContext.registerReceiver(mReceiver, filter);

        Intent i = new Intent(TRIGGER_STA_BROADCAST);
        mContext.sendBroadcast(i);
    }

    /**
     * Stop Listen Sta change notification.
     */
    public void stopMonitor() {
        mContext.unregisterReceiver(mReceiver);
    }

    /**
     * Get current Sta Device list.
     * Attention : You maybe get a list which size is 0, in case
     */
    public List<StaDevice> getStaDevices() {
        List<StaDevice> devices = null;
        synchronized (mStaDevices) {
            devices = new ArrayList<>(mStaDevices);
        }
        return devices;
    }

    public void addListener(StaChangedListener l) {
        mListener.add(l);
    }

    public void removeListener(StaChangedListener l) {
        mListener.remove(l);
    }

    private void notifyListener(List<StaDevice> staDevices) {
        for (StaChangedListener l : mListener) {
            List<StaDevice> devices = new ArrayList<>(staDevices);
            l.onChanged(devices);
        }
    }

    private void handleStaChange(Intent intent) {
        synchronized (mStaDevices) {
            mStaDevices.clear();
            ConnectivityManager connectivityManager =
                    (ConnectivityManager) mContext.getSystemService(Context.CONNECTIVITY_SERVICE);
            List<WifiDevice> tetherConnectedList = connectivityManager.getTetherConnectedSta();
            for (int i = 0; i < tetherConnectedList.size(); i++) {
                mStaDevices.add(new StaDevice(tetherConnectedList.get(i)));
            }
            notifyListener(mStaDevices);
        }
    }

    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            Log.d(TAG, "onReceive TETHER_CONNECT_STATE_CHANGED");
            handleStaChange(intent);
        }
    };

    /**
     * StaDevice state change listener.
     */
    public interface StaChangedListener {
        /**
         * Callback when StaDevice state change.
         * Attention : this method always run on UI thread.
         * @param staDevices Sta Devices list which has refreshed just now.
         */
        void onChanged(List<StaDevice> staDevices);
    }
}
