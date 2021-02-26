/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.extsettings.hotspot;

import android.app.Activity;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.net.wifi.WifiManager;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;

import com.qualcomm.qti.extsettings.ExtSettingsApplication;
import com.qualcomm.qti.extsettings.sta.StaDevice;
import com.qualcomm.qti.extsettings.sta.StaMonitor;

import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

import com.qualcomm.qti.extsettings.hotspot.Res;

public class HotspotService extends Service {
    private static final String HOTSPOT_PREFERENCE_FILE = "wifi_hotsopt_preference";
    private static final String KEY_HOTSOPT_MODE = "wifi_hotsopt_mode";
    private static final int TIMER_SCHEDULE = 0;
    /* Always */
    private static final String HOTSPOT_MODE_ALWAYS = "Always";
    /* Turn off when idle for 5 min */
    private static final String HOTSPOT_MODE_5MIN = "Turn_off_5min";
    /* Turn off when idle for 10 min */
    private static final String HOTSPOT_MODE_10MIN = "Turn_off_10min";
    private long mHotspotLastValidTime = 0L;
    private Timer mTimer;

    private StaMonitor.StaChangedListener mStaListener =
            new StaMonitor.StaChangedListener() {
        @Override
        public void onChanged(List<StaDevice> staDevices) {
            setCurrentTimeToLastValidTime();
        }
    };

    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message message) {
            switch (message.what) {
                case TIMER_SCHEDULE:
                    long turnOffTime = getTurnOffTimebyMode();
                    if (turnOffTime > 0L) {
                        long currentTime = System.currentTimeMillis();
                        long a = currentTime - getHotspotLastValidTime();
                        if (currentTime - getHotspotLastValidTime() > turnOffTime) {
                            disableWifiAp();
                            stopSelf();
                        } else {
                            setCurrentTimeToLastValidTime();
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    };

    private long getTurnOffTimebyMode() {
        String hotsoptMode = getHotspotMode(this);
        if (hotsoptMode.equals(HOTSPOT_MODE_5MIN)) {
            return 5 * 60 * 1000;
        } else if (hotsoptMode.equals(HOTSPOT_MODE_10MIN)) {
            return 10 * 60 * 1000;
        }
        return 0L;
    }

    @Override
    public IBinder onBind(Intent arg0) {
        return null;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        long currentTime = System.currentTimeMillis();
        setHotspotLastValidTime(currentTime);

        ((ExtSettingsApplication)getApplication()).getStaMonitor()
                .addListener(mStaListener);

        TimerTask task = new TimerTask() {
            public void run() {
                Message message = mHandler.obtainMessage();
                message.what = TIMER_SCHEDULE;
                mHandler.sendMessage(message);
            }
        };
        mTimer = new Timer(true);
        mTimer.schedule(task, 10 * 1000, 10 * 1000);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        if (mTimer != null) {
            mTimer.cancel();
            mTimer = null;
        }
        ((ExtSettingsApplication)getApplication()).getStaMonitor()
                .removeListener(mStaListener);
    }

    private void setCurrentTimeToLastValidTime() {
        List<StaDevice> tetherList = ((ExtSettingsApplication)getApplication())
                .getStaMonitor().getStaDevices();
        if (tetherList != null && tetherList.size() > 0) {
            long currentTime = System.currentTimeMillis();
            setHotspotLastValidTime(currentTime);
        }
    }

    private void disableWifiAp() {
        WifiManager wifiManager = (WifiManager)getSystemService(
                Service.WIFI_SERVICE);
        if (wifiManager != null) {
            wifiManager.setWifiApEnabled(null, false);
        }
    }

    private void setHotspotLastValidTime(Long value) {
        synchronized (this) {
            mHotspotLastValidTime = value;
        }
    }

    private long getHotspotLastValidTime() {
        return mHotspotLastValidTime;
    }

    public static void setHotspotMode(Context context, String value) {
        if (context != null) {
            SharedPreferences prefs = context.getSharedPreferences(
                    HOTSPOT_PREFERENCE_FILE, Activity.MODE_PRIVATE);
            SharedPreferences.Editor editor = prefs.edit();
            editor.putString(KEY_HOTSOPT_MODE, value);
            editor.apply();
        }
    }

    public static String getHotspotMode(Context context, String defValue) {
        if (context != null) {
            SharedPreferences prefs = context.getSharedPreferences(
                    HOTSPOT_PREFERENCE_FILE, Activity.MODE_PRIVATE);
            return prefs.getString(KEY_HOTSOPT_MODE, defValue);
        }
        return defValue;
    }

    public static String getHotspotMode(Context context) {
        if (Res.getSettingsBoolean(context, Res.HOTSPOT_ON_10MINS_DEFAULT)) {
            return getHotspotMode(context, HOTSPOT_MODE_10MIN);
        } else {
            return getHotspotMode(context, HOTSPOT_MODE_ALWAYS);
        }
    }
}
