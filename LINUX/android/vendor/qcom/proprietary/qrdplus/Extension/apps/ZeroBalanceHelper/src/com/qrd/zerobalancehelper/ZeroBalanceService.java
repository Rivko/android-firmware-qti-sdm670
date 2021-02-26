/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qrd.zerobalancehelper;

import android.app.AlarmManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.ContentValues;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.content.pm.ApplicationInfo;
import android.database.Cursor;
import android.net.ConnectivityManager;
import android.net.ZeroBalanceHelper;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.os.IBinder;
import android.os.SystemProperties;


import com.qrd.zerobalancehelper.ZeroBalanceReceiver;

import java.util.ArrayList;
import java.util.List;


public class ZeroBalanceService extends Service {
    private final String TAG = "ZeroBalanceService";

    private ZeroBalanceReceiver mReceiver = null;
    private int browserUid = -1;
    private int zbUid = -1;
    private String exceptionUids = null;

    private final int SYSTEM_PROPERTY_MAX_SIZE = 92;
    public static boolean isServiceRunning = false;


    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public void onCreate() {
        super.onCreate();

        //Register receiver
        IntentFilter filter = new IntentFilter();
        filter.addAction(ZeroBalanceHelper.BACKGROUND_DATA_BROADCAST);
        filter.addAction(ConnectivityManager.CONNECTIVITY_ACTION);
        filter.addAction(ZeroBalanceReceiver.SET_ZB_RETRY_UNBLOCK_TIMER);
        filter.addAction(ZeroBalanceReceiver.SET_ZB_SINGLE_PING_UNBLOCK_TIMER);
        filter.addAction(WifiManager.NETWORK_STATE_CHANGED_ACTION);
        filter.addAction(ZeroBalanceReceiver.CALL_STATE_CHANGE);
        mReceiver = new ZeroBalanceReceiver();
        registerReceiver(mReceiver, filter);
        Log.d(TAG,"register ZeroBalanceReceiver");
        exceptionUids = getApplicationContext().getResources().getString(
                R.string.exception_apps_uid);
        loadExceptionUids();
        isServiceRunning = true;

    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.d(TAG, "Service running");
        isServiceRunning = true;
        return Service.START_STICKY;
    }

    @Override
    public void onDestroy() {
        if (mReceiver != null) {
            unregisterReceiver(mReceiver);
            mReceiver = null;
            Log.d(TAG,"unregister ZeroBalanceReceiver");
        }
        isServiceRunning = false;
        super.onDestroy();
    }

    private int getUidFromPackage(String packageName) {
        try {
            ApplicationInfo packageInfo = getApplicationContext()
                    .getPackageManager().getApplicationInfo(packageName, 0);
            Log.d(TAG, "Package " + packageInfo.packageName +" has uid " + packageInfo.uid);
            return packageInfo.uid;
        } catch (PackageManager.NameNotFoundException ex) {
            Log.e(TAG, "Unexpected NameNotFoundException"+ ex);
        }
        return 0;
    }

    /**
     * Load all exception whitelist app uids which will be allowed data access in blocked state.
     * Excpetion uids are loaded from 1. strings.xml : direct uid 2. whitelistapp_config.xml : uid
     * from packagename 3. from metadata : uid from application info with matching meta data
     */

    private void loadExceptionUids() {
        // Excpetion uids are loaded from whitelistapp_config.xml
        // : uid from packagename
        loadExceptionUidsfromFile();
        Log.d(TAG, "Exception Uid = " + exceptionUids);
        // Feature config flag to enable disable loading apps from
        // matching metadata infos
        if (getApplicationContext().getResources().getBoolean(
                R.bool.whitelist_apps_add_enable)) {
            loadExceptionUidsThroughMetaData();
        } else {
            Log.d(TAG, "Custom Whitelist app is not enabled");
        }
        // If total uid string length exceeds SYSTEM_PROPERTY_MAX_SIZE
        // String is truncated to accommodate upto SYSTEM_PROPERTY_MAX_SIZE
        if (exceptionUids.length() > SYSTEM_PROPERTY_MAX_SIZE) {
            exceptionUids = exceptionUids
                    .substring(0, SYSTEM_PROPERTY_MAX_SIZE);
        }
        try {
            SystemProperties.set("sys.background.exception.app", exceptionUids);
        } catch (IllegalArgumentException ex) {
            Log.e(TAG, "Exception Uids length exceeds "
                    + SYSTEM_PROPERTY_MAX_SIZE + "character");
        }
    }

    /**
     * Exception app uids are loaded from whitelistapp_config.xml : uid from packagename.
     * Browser and Zero balance helper application package is already present in the list.
     * OEM can add more application's package name up to 12 to this file.
     */
    private void loadExceptionUidsfromFile() {
        String[] whiteListPackage = getResources().getStringArray(
                R.array.whitelist_app_package);
        if (whiteListPackage != null & whiteListPackage.length != 0) {
            for (String packageName : whiteListPackage) {
                int uid = getUidFromPackage(packageName);
                exceptionUids += "," + uid;
                Log.d(TAG, "Exception Uid from file is = " + uid);
            }
        } else {
            Log.d(TAG, "Whitelist app config file is empty");
        }
    }

    /**
     * Exception app uids are loaded by matching all installed application's
     * meta data info.
     * Imp**:This approach is not recommended for adding whitelist apps as going
     * through all installed application can cause little delay in the process.
     * For adding whitelist apps OEM should populate whitelistapp_config.xml
     * and disable the feature config flag for loading using metadata.
     */
    private void loadExceptionUidsThroughMetaData() {
        final String ExceptionAppKey = getApplicationContext().getResources()
                .getString(
                R.string.exception_apps_key);
        final String ExceptionAppVal = getApplicationContext().getResources()
                .getString(
                R.string.exception_apps_val);
        PackageManager pm = getPackageManager();
        // get a list of installed apps.
        List<ApplicationInfo> ai = pm
                .getInstalledApplications(PackageManager.GET_META_DATA);
        for (ApplicationInfo packageInfo : ai) {
            Bundle bundle = packageInfo.metaData;
            if (bundle != null) {
                String myApiVal = bundle.getString(ExceptionAppKey);
                if (myApiVal != null && myApiVal.equals(ExceptionAppVal)) {
                    Log.d(TAG, "Metadata matched"
                            + packageInfo.getClass().getName());
                    exceptionUids += "," + packageInfo.uid;
                }
            }
        }
    }

}
