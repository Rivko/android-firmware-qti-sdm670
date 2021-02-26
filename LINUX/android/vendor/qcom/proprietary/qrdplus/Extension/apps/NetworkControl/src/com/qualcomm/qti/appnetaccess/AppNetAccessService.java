/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
     Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

package com.qualcomm.qti.appnetaccess;

import java.util.HashMap;
import java.util.concurrent.atomic.AtomicInteger;

import android.app.Service;
import android.content.Intent;
import android.database.Cursor;
import android.os.Bundle;
import android.os.IBinder;

import android.util.Log;

public class AppNetAccessService extends Service {

    private AppNetAccessSocket mSocket;
    private AppNetAccessDao mAccessDao;
    private static final String ACTION_BOOT = "android.intent.action.BOOT_COMPLETED";
    private static final String ACTION_REMOVE_PKG = "android.intent.action.PACKAGE_REMOVED";


    public AppNetAccessService() {
        super();
    }

    @Override
    public void onCreate() {
        super.onCreate();
        mAccessDao = AppNetAccessDao.getInstance(this);
    }

    @Override
    public IBinder onBind(Intent arg0) {
        return null;
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
    }

    private void setupWhenBoot() {
        new Thread() {
            @Override
            public void run() {
                Cursor cursor = null;
                HashMap<Integer, AppInfo> appMap = new HashMap<Integer, AppInfo>();
                try {
                    cursor = mAccessDao.queryAll();
                    boolean wlanDisabled, dataDisabled;
                    int appUid;
                    String appLable;
                    int colId;
                    while (cursor != null && cursor.moveToNext()) {
                        colId = cursor.getInt(cursor.getColumnIndexOrThrow(AppNetAccessDao.COL_ID));
                        wlanDisabled =
                                cursor.getInt(cursor.getColumnIndexOrThrow(
                                        AppNetAccessDao.COL_WIFI)) == 0 ?false : true;
                        dataDisabled =
                                cursor.getInt(cursor.getColumnIndexOrThrow(
                                        AppNetAccessDao.COL_MOBILE)) == 0 ? false : true;
                        appLable = cursor.getString(cursor.getColumnIndexOrThrow(
                                AppNetAccessDao.COL_NAME));

                        appUid = cursor.getInt(cursor.getColumnIndexOrThrow(
                                AppNetAccessDao.COL_APPS_UID));

                        AppInfo app = new AppInfo(appUid, appLable, wlanDisabled, dataDisabled);
                        appMap.put(colId, app);
                    }
                } catch (IllegalArgumentException e) {
                    e.printStackTrace();
                } finally {
                    if (cursor != null) {
                        cursor.close();
                        cursor = null;
                    }
                }
                mSocket = new AppNetAccessSocket();
                if (mSocket.connectToSocket()) {
                    for (int colId : appMap.keySet()) {
                        AppInfo app = appMap.get(colId);
                        if (app.dataDisabled) {
                            mSocket.setAppRestrictMobile(true, app.uid);
                        }
                        if (app.wlanDisabled) {
                            mSocket.setAppRestrictWlan(true, app.uid);
                        }
                    }
                }
                appMap.clear();
                if (mSocket != null) {
                    mSocket.closeSocket();
                }
                super.run();
            }
        }.run();
    }

    private void clearRulesIfNeed(final int uid) {
        new Thread () {
            @Override
            public void run() {
                Cursor cursor = null;
                boolean wlanDisable = false;
                boolean mobileDisable = false;
                // first delete the records in DB.
                // second delete the rules on iptables.
                try {
                    cursor = mAccessDao.query(uid);
                    if (cursor != null && cursor.moveToFirst()) {
                        wlanDisable = cursor.getInt(cursor
                                .getColumnIndexOrThrow(AppNetAccessDao.COL_WIFI)) == 0 ? false
                                : true;
                        mobileDisable = cursor.getInt(cursor
                                .getColumnIndexOrThrow(AppNetAccessDao.COL_MOBILE)) == 0 ? false
                                : true;
                        mAccessDao.delete(uid);
                        cursor.close();
                    }
                } catch (IllegalArgumentException e) {
                    e.printStackTrace();
                } finally {
                    cursor.close();
                    cursor = null;
                }

                mSocket = new AppNetAccessSocket();
                if (mSocket.connectToSocket()) {
                    if (mobileDisable) {
                        mSocket.setAppRestrictMobile(false, uid);
                    }
                    if (wlanDisable) {
                        mSocket.setAppRestrictWlan(false, uid);
                    }
                }
                if (mSocket != null) {
                    mSocket.closeSocket();
                }
                super.run();
            }

        }.run();
    }

    @Override
    public void onStart(Intent intent, int startId) {
        super.onStart(intent, startId);
        if (intent == null) {
            return;
        }
        String action = intent.getAction();
        if (ACTION_BOOT.equals(action)) {
            setupWhenBoot();
        } else if (ACTION_REMOVE_PKG.equals(action)) {
            // When remove pkg from device, remove the restriction rules too.
            Bundle extras = intent.getExtras();
            if (extras == null) {
                return;
            }
            final int uid = extras.getInt(Intent.EXTRA_UID);
            clearRulesIfNeed(uid);
        }
    }

    class AppInfo {
        public AppInfo(int uid, String lable, boolean wifi, boolean data) {
            this.uid = uid;
            this.lable = lable;
            this.wlanDisabled = wifi;
            this.dataDisabled = data;
        }
        public int uid;
        public String lable;
        public boolean wlanDisabled;
        public boolean dataDisabled;
    }

}
