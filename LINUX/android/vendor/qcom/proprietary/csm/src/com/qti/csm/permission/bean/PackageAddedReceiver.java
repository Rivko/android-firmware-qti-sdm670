/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qti.csm.permission.bean;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;

import com.qti.csm.permission.utils.PermissionUtils;

public class PackageAddedReceiver extends BroadcastReceiver {
    private static final int PACKAGE_NAME_START_INDEX = 8;

    @Override
    public void onReceive(Context context, Intent intent) {
        if (intent == null) {
            return;
        }

        if (intent.getAction().equals(Intent.ACTION_PACKAGE_ADDED)) {
            String data = intent.getDataString();

            if (data == null || data.length() <= PACKAGE_NAME_START_INDEX) {
                return;
            }

            String packageName = data.substring(PACKAGE_NAME_START_INDEX);
            PermissionUtils.getNewApkUid(context, packageName);
        } else if (intent.getAction().equals(Intent.ACTION_PACKAGE_REMOVED)) {
            PermissionUtils.removeUid(context);
        }

    }

}
