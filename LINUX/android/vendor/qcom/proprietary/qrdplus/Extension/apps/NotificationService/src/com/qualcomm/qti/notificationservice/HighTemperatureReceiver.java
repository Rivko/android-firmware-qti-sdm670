/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 */

/*
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

package com.qualcomm.qti.notificationservice;

import android.app.Notification;
import android.app.NotificationManager;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.BatteryManager;
import android.os.UserHandle;
import android.util.Log;
import android.view.View;

public final class HighTemperatureReceiver extends BroadcastReceiver {
    private static final String TAG = "HighTemperatureReceiver";
    private static final boolean DEBUG = false;
    private static final String TAG_NOTIFICATION = "high_battery_temperature";
    private static final int ID_NOTIFICATION = 100;

    private Context mContext;
    private NotificationManager mNoMan;

    private int mBatteryLevel = 100;
    private int mBatteryStatus = BatteryManager.BATTERY_STATUS_UNKNOWN;
    private int mPlugType = 0;
    private int mInvalidCharger = 0;
    private int mBatteryTemperature = 0;
    private int mHealth = BatteryManager.BATTERY_HEALTH_UNKNOWN;

    private void init() {
        mNoMan = (NotificationManager) mContext.getSystemService(Context.NOTIFICATION_SERVICE);
    }

    private void dismissBatteryOverTemperatureNotification() {
        mNoMan.cancelAsUser(TAG_NOTIFICATION, ID_NOTIFICATION, UserHandle.ALL);
    }

    private void showBatteryOverTemperatureNotification() {
        final Notification.Builder nb = new Notification.Builder(mContext)
                .setSmallIcon(R.drawable.high_temperature_warning)
                .setShowWhen(true)
                .setOngoing(true)
                .setTicker(mContext.getString(R.string.battery_over_temperature_title))
                .setContentTitle(mContext.getString(R.string.battery_over_temperature_title))
                .setContentText(mContext.getString(R.string.battery_over_temperature_content))
                .setOnlyAlertOnce(true)
                .setDefaults(Notification.DEFAULT_ALL)
                .setPriority(Notification.PRIORITY_MAX)
                .setVisibility(Notification.VISIBILITY_PUBLIC)
                .setColor(mContext.getResources().getColor(R.color.battery_over_temperature_color));
        final Notification n = nb.build();
        if (n.headsUpContentView != null) {
            n.headsUpContentView.setViewVisibility(com.android.internal.R.id.right_icon, View.GONE);
        }
        mNoMan.notifyAsUser(TAG_NOTIFICATION, ID_NOTIFICATION, n, UserHandle.ALL);
    }

    @Override
    public void onReceive(Context context, Intent intent) {
        Log.d(TAG, "Receive action: " + intent.getAction());
        mContext = context;
        String action = intent.getAction();
        if (action.equals(Intent.ACTION_BATTERY_CHANGED)) {
            final int oldBatteryLevel = mBatteryLevel;
            mBatteryLevel = intent.getIntExtra(BatteryManager.EXTRA_LEVEL, 100);
            final int oldBatteryStatus = mBatteryStatus;
            mBatteryStatus = intent.getIntExtra(BatteryManager.EXTRA_STATUS,
                    BatteryManager.BATTERY_STATUS_UNKNOWN);
            final int oldPlugType = mPlugType;
            mPlugType = intent.getIntExtra(BatteryManager.EXTRA_PLUGGED, 0);
            final int oldInvalidCharger = mInvalidCharger;
            mInvalidCharger = intent.getIntExtra(BatteryManager.EXTRA_INVALID_CHARGER, 0);
            final int oldBatteryTemperature = mBatteryTemperature;
            mBatteryTemperature = intent.getIntExtra(BatteryManager.EXTRA_TEMPERATURE, 0);
            final int oldHealth = mHealth;
            mHealth = intent.getIntExtra(BatteryManager.EXTRA_HEALTH,
                    BatteryManager.BATTERY_HEALTH_UNKNOWN);


            final boolean plugged = mPlugType != 0;
            final boolean oldPlugged = oldPlugType != 0;

            if (DEBUG) {
                Log.d(TAG, "level          " + oldBatteryLevel + " --> " + mBatteryLevel);
                Log.d(TAG, "status         " + oldBatteryStatus + " --> " + mBatteryStatus);
                Log.d(TAG, "plugType       " + oldPlugType + " --> " + mPlugType);
                Log.d(TAG, "invalidCharger " + oldInvalidCharger + " --> " + mInvalidCharger);
                Log.d(TAG, "plugged        " + oldPlugged + " --> " + plugged);
                Log.d(TAG, "temperature    " + oldBatteryTemperature + " --> "
                    + mBatteryTemperature);
                Log.d(TAG, "health         " + oldHealth + " --> " + mHealth);
            }

            init();

            if (plugged && mHealth == BatteryManager.BATTERY_HEALTH_OVERHEAT) {
                showBatteryOverTemperatureNotification();
            } else {
                dismissBatteryOverTemperatureNotification();
            }
        } else {
            Log.d(TAG, "unknown intent: " + intent);
        }
    }
}
