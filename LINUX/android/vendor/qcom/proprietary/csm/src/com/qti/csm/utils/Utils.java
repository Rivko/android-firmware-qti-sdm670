/*
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qti.csm.utils;

import java.text.Collator;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;

import android.os.SystemProperties;

@SuppressLint("UseSparseArrays")
public class Utils {

    public static PackageManager mPackageManager = null;
    public static List<PackageInfo> mAllApps = new ArrayList<PackageInfo>(0);

    //Property default value is true
    public static String app_restrict = "persist.sys.apprestrict";

    /********************************
     * Common
     ***********************************/
    public static void loadAllApps(Context ctx) {
        if (mAllApps != null) {
            mAllApps.clear();
        }
        mPackageManager = ctx.getPackageManager();
        for (final PackageInfo packageInfo : mPackageManager.getInstalledPackages(0)) {
            if (packageInfo.applicationInfo.uid < 10000) { //Process.FIRST_APPLICATION_UID
                continue;
            }
            mAllApps.add(packageInfo);
        }
        Collections.sort(mAllApps, new DisplayNameComparator(mPackageManager));
    }

    /********************************
     * Common
     ***********************************/
    public static void loadThirdPartyApps(Context ctx) {
        if (mAllApps != null) {
            mAllApps.clear();
        }
        mPackageManager = ctx.getPackageManager();
        for (final PackageInfo packageInfo : mPackageManager.getInstalledPackages(0)) {
            if (packageInfo.applicationInfo.uid < 10000) { //Process.FIRST_APPLICATION_UID
                continue;
            }
            //judge if third party app or not
            if ((packageInfo.applicationInfo.flags & packageInfo.applicationInfo.FLAG_SYSTEM) > 0) {
                // System app
                continue;
            }
            mAllApps.add(packageInfo);
        }
        Collections.sort(mAllApps, new DisplayNameComparator(mPackageManager));
    }

    public static List<PackageInfo> getAllApps(Context ctx) {
        return mAllApps;
    }

    public static class DisplayNameComparator implements Comparator<PackageInfo> {
        public DisplayNameComparator(PackageManager pm) {
            mPM = pm;
            mCollator.setStrength(Collator.PRIMARY);
        }

        public final int compare(PackageInfo a, PackageInfo b) {
            CharSequence sa = a.applicationInfo.loadLabel(mPM);
            if (sa == null)
                sa = a.packageName;
            CharSequence sb = b.applicationInfo.loadLabel(mPM);
            if (sb == null)
                sb = b.packageName;

            return mCollator.compare(sa.toString(), sb.toString());
        }

        private final Collator mCollator = Collator.getInstance();
        private PackageManager mPM;
    }
}
