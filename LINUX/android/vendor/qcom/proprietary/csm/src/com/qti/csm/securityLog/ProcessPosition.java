/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qti.csm.securityLog;

import android.app.AppGlobals;
import android.app.AppOpsManager;
import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.IPackageManager;
import android.content.pm.PackageManager;
import android.graphics.drawable.Drawable;
import android.os.Process;
import android.os.RemoteException;
import android.os.UserHandle;
import android.os.UserManager;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;

class ProcessPosition {
    private static final String ANDROID_ID = "android";
    private static final int LOG_CHECKING = 8 * 60 * 60 * 1000;

    private final Context mContext;
    private final PackageManager mPackageManager;

    private static final int[] CHECK_OPS = new int[]{
            AppOpsManager.OP_MONITOR_LOCATION,
            AppOpsManager.OP_MONITOR_HIGH_POWER_LOCATION,
    };

    public ProcessPosition(Context context) {
        mContext = context;
        mPackageManager = context.getPackageManager();
    }

    private Request doRequest(long now,
                              AppOpsManager.PackageOps ops) {
        String packageName = ops.getPackageName();
        List<AppOpsManager.OpEntry> entries = ops.getOps();
        boolean highBattery = false;
        boolean normalBattery = false;
        long ent = 1L;
        long recentLocationCutoffTime = now - LOG_CHECKING;
        for (AppOpsManager.OpEntry entry : entries) {
            if (entry.isRunning() || entry.getTime() >= recentLocationCutoffTime) {
                if (entry.getTime() > ent) {
                    ent = entry.getTime();
                }
                switch (entry.getOp()) {
                    case AppOpsManager.OP_MONITOR_LOCATION:
                        normalBattery = true;
                        break;
                    case AppOpsManager.OP_MONITOR_HIGH_POWER_LOCATION:
                        highBattery = true;
                        break;
                    default:
                        break;
                }
            }
        }

        if (!highBattery && !normalBattery) {
            return null;
        }

        int uid = ops.getUid();
        int userId = UserHandle.getUserId(uid);
        SimpleDateFormat simpleDateFormat = new SimpleDateFormat("yyyy-MM-dd HH-mm-ss");
        CharSequence posCall = simpleDateFormat.format(new Date(ent));
        Request request = null;
        try {
            IPackageManager ipm = AppGlobals.getPackageManager();
            ApplicationInfo appInfo =
                    ipm.getApplicationInfo(packageName, PackageManager.GET_META_DATA, userId);
            if (appInfo == null) {
                return null;
            }

            final UserHandle userHandle = new UserHandle(userId);
            Drawable appIcon = mPackageManager.getApplicationIcon(appInfo);
            Drawable icon = mPackageManager.getUserBadgedIcon(appIcon, userHandle);
            CharSequence appLabel = mPackageManager.getApplicationLabel(appInfo);
            CharSequence infoTag = mPackageManager.getUserBadgedLabel(appLabel, userHandle);
            if (appLabel.toString().contentEquals(infoTag)) {
                infoTag = null;
            }
            request = new Request(packageName, userHandle, icon, appLabel, highBattery,
                    infoTag, posCall);
        } catch (RemoteException e) {
            e.printStackTrace();
        }

        return request;
    }

    public List<Request> getReq() {
        AppOpsManager myManager =
                (AppOpsManager) mContext.getSystemService(Context.APP_OPS_SERVICE);
        List<AppOpsManager.PackageOps> appOps = myManager.getPackagesForOps(CHECK_OPS);

        final int getCount = appOps != null ? appOps.size() : 0;

        ArrayList<Request> requests = new ArrayList<>(getCount);
        final long now = System.currentTimeMillis();
        final UserManager um = (UserManager) mContext.getSystemService(Context.USER_SERVICE);
        final List<UserHandle> profiles = um.getUserProfiles();

        for (int i = 0; i < getCount; ++i) {
            AppOpsManager.PackageOps ops = appOps.get(i);
            String packageName = ops.getPackageName();
            int uid = ops.getUid();
            int userId = UserHandle.getUserId(uid);
            boolean isSys =
                    (uid == Process.SYSTEM_UID) && ANDROID_ID.equals(packageName);
            if (isSys || !profiles.contains(new UserHandle(userId))) {
                continue;
            }
            Request request = doRequest(now, ops);
            if (request != null) {
                requests.add(request);
            }
        }

        return requests;
    }


    public static class Request {
        public final String packageName;
        public final boolean isHigh;
        public final CharSequence label;
        public final CharSequence contentDescription;
        public final CharSequence call;
        public final UserHandle userHandle;
        public final Drawable icon;

        private Request(String packageName, UserHandle userHandle, Drawable icon,
                        CharSequence label, boolean isHigh, CharSequence contentDescription, CharSequence call) {
            this.packageName = packageName;
            this.isHigh = isHigh;
            this.contentDescription = contentDescription;
            this.call = call;
            this.userHandle = userHandle;
            this.icon = icon;
            this.label = label;
        }
    }
}
