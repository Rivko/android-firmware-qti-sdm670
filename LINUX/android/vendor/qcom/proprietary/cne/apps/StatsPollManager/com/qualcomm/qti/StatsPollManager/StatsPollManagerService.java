/*
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.StatsPollManager;

import android.app.usage.NetworkStatsManager;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.ConnectivityManager;
import android.os.Binder;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.IBinder;
import android.os.Looper;
import android.os.Message;
import android.os.ServiceManager;
import android.os.SystemClock;
import android.provider.Settings;
import android.telephony.PhoneStateListener;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.util.Log;

public class StatsPollManagerService extends Service {

    public static final String TAG = StatsPollManagerService.class.getSimpleName();

    private static final String NETSTATS_GLOBAL_ALERT_BYTES = "netstats_global_alert_bytes";
    private static final long MB_IN_BYTES = 1 * 1024 * 1024;

    private static final long TWO_MB = 2 * MB_IN_BYTES;
    private static final long TWENTY_MB = 20 * MB_IN_BYTES;
    private static final long FIFTY_MB = 50 * MB_IN_BYTES;

    private static long currBufferValue = TWO_MB;

    /* T1 = Time taken to consume 2 MB data for low tput cases (less than CAT6 rates)
     * T2 = Time taken to consume 20 MB data for mid tput cases (CAT6 rates)
     * T3 = Time taken to consume 50 MB data for high tput cases (CAT7, CAT8 or more)
     *
     * So, for low tput, buffer values stays 2 MB for a longer time. For mid tputs,
     * 20 MB and for high tputs, buffer stays on 50 MB
     *
     *
     *           |     low        mid       high
     *    _______|___________________________________
     *           |
     *    2MB    |     T1         T1-        T1--
     *           |
     *    20MB   |     T2+        T2         T2-
     *           |
     *    50MB   |     T3++       T3+        T3
     *
     *
     *    >> For example, consider a high tput scenario, current buffer is 2MB, since
     *       time taken is less than T1, it moves to 20MB state, there the time taken
     *       is less than T2, so it moves to 50 MB and stays there at 50 MB.
     *
     *    >> Similarly, for low tput scenario, buffer value stays at 2MB
     *
     */

    private static final long T1 = 1900;
    private static final long T2 = 2300;
    private static final long T3 = 2700;
    private static final long delta = 150;

    private long time;
    private UsageCallback usageCallback;
    private Handler mStatsPollHandler;
    private TelephonyManager mTelephonyManager;
    private NetworkStatsManager mNetworkStatsManager;
    private PackageManager mPackageManager;
    private static final boolean DBG = false;

    private class UsageCallback extends NetworkStatsManager.UsageCallback {
        @Override
        public void onThresholdReached(int networkType, String subscriberId) {
            long currTime = SystemClock.elapsedRealtime();
            if (DBG) Log.d(TAG, "currBuffer = " + currBufferValue +
                                " currTime = " + currTime + " time = " + time);

            setGlobalAlert(getRecommendedGlobalAlert(currBufferValue,timeDiff(time, currTime)));
            time = currTime;
        }
    }

    /* StatsPollManagerService algorithm should run only in case of 4G.
       In case of 4G networks, register for the usage call back.
       In case of other networks (3G or lower), fallback to 2 MB Global
       Alert and unregister to stop the algorithm from running.
     */

    PhoneStateListener mPhoneStateListener = new PhoneStateListener() {
        @Override
        public void onDataConnectionStateChanged(int state, int networkType) {

             int networkClass = mTelephonyManager.getNetworkClass(networkType);
             if (DBG) Log.d(TAG, "state = " + state + " networkType = " + networkType +
                                 " networkClass = " + networkClass);

             if (state == TelephonyManager.DATA_CONNECTED &&
                 networkClass == TelephonyManager.NETWORK_CLASS_4_G) {
                 registerObserver(2 * MB_IN_BYTES);
             } else {
                 setGlobalAlert(TWO_MB);
                 unregisterObserver();
             }
        }
    };

    private static long timeDiff(long prev, long curr) {
        if (curr > prev) return (curr - prev);
        else return 0;
    }

    private static long getRecommendedGlobalAlert(long currBufferValue, long elapsedTime) {
        if ((currBufferValue == TWO_MB) && (elapsedTime < T1)) {
             return TWENTY_MB;
        } else if ((currBufferValue == TWENTY_MB) && (elapsedTime < T2 - delta)){
             return FIFTY_MB;
        } else if ((currBufferValue == TWENTY_MB) && (elapsedTime > T2 + delta)){
             return TWO_MB;
        } else if ((currBufferValue == FIFTY_MB) && (elapsedTime > T3)){
             return TWENTY_MB;
        } else {
             return currBufferValue;
        }
    }

    private void registerObserver(long bufferValue) {
        if (DBG) Log.d(TAG, "register observer");
        mNetworkStatsManager.registerUsageCallback(ConnectivityManager.TYPE_MOBILE,
                mTelephonyManager.getSubscriberId(), bufferValue, usageCallback,
                mStatsPollHandler);
    }

    private void unregisterObserver() {
        if (DBG) Log.d(TAG, "unregister observer");

        try {
            mNetworkStatsManager.unregisterUsageCallback(usageCallback);
        } catch(IllegalArgumentException ex) {
            Log.e(TAG, "Invalid callback encountered while unregister.");
        }
    }

    @Override
    public void onCreate(){
        if (DBG) Log.d(TAG, "onCreate()");
        super.onCreate();

        mNetworkStatsManager = (NetworkStatsManager) getApplicationContext()
                .getSystemService(Context.NETWORK_STATS_SERVICE);

        mTelephonyManager = (TelephonyManager) getApplicationContext()
                .getSystemService(Context.TELEPHONY_SERVICE);

        mPackageManager = getApplicationContext().getPackageManager();
        mPackageManager.grantRuntimePermission(getApplicationContext().getPackageName(),
                                     "android.permission.READ_PHONE_STATE",
                                     Binder.getCallingUserHandle());

        usageCallback = new UsageCallback();

        HandlerThread thread = new HandlerThread("statspoll-thread");
        thread.start();
        mStatsPollHandler = new Handler(thread.getLooper());

        mTelephonyManager
                .listen(mPhoneStateListener, PhoneStateListener.LISTEN_DATA_CONNECTION_STATE);
        time = SystemClock.elapsedRealtime();
    }

    @Override
    public void onStart(Intent intent, int id) {
        if (DBG) Log.d(TAG, "onStart()");
        super.onStart(intent, id);
    }

    @Override
    public void onDestroy() {
        if (DBG) Log.d(TAG, "onDestroy()");
        super.onDestroy();
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    /* set Global Alert. Unregister usage calback and register back
       with the new value.
     */

    private void setGlobalAlert(long bytes) {
        if (DBG) Log.d(TAG, "setGlobalAlert(" + bytes + ")");
        if (currBufferValue != bytes) {
            Settings.Global.putLong(getContentResolver(), NETSTATS_GLOBAL_ALERT_BYTES, bytes);
            currBufferValue = bytes;

            unregisterObserver();
            registerObserver(currBufferValue);
        }
    }
}
